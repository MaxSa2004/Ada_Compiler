#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <ctype.h>
#include "mips_backend.h"

/*
Mapeamento de temporários e variáveis para registos
t0..t9 -> $t0..$t9
t10+ -> $s0..$s7 (limitado)
variáveis -> na stack (lw/sw) 
*/

// helpers
static int getVarOffset(Table table, const char *name){
    SymbolInfo* s = lookup_value(table, (char *)name);
    if(s == NULL){
        fprintf(stderr, "Variable %s not found in symbol table\n", name);
        exit(1);
    } else {
        return s->offset;
    }
}

static int getVarSize(Table table, const char* name){
    SymbolInfo* s = lookup_value(table, (char *)name);
    if(s == NULL){
        fprintf(stderr, "Variable %s not found in symbol table\n", name);
        exit(1);
    } else {
        return s->size;
    }
}

// temporários em registos
static const char* reg_for_temp(int temp_id){
    if(temp_id>=0 && temp_id<=9){
        static char buf[8];
        snprintf(buf, sizeof(buf), "$t%d", temp_id);
        return strdup(buf);
    }
    int sidx = temp_id - 10;
    if(sidx>=0 && sidx<=7){
        static char buf[8];
        snprintf(buf, sizeof(buf), "$s%d", sidx);
        return strdup(buf);
    }
    return strdup("$t9"); // fallback
}

// variáveis: usar memória via $fp + offset 
static int is_temp_name(const char* name, int *outTempId){
    if(!name||name[0]!='t') return 0;
    char *endptr = NULL;
    long v = strtol(name +1, &endptr, 10);
    if(endptr && *endptr=='\0'&&v>=0){
        if(outTempId) *outTempId = (int)v;
        return 1;
    }
    return 0;
}
// helpers para literais de strings e floats
typedef struct StrLit {
    char *text;
    char label[32];
    struct StrLit *next;
} StrLit;

typedef struct FloatLit {
    double value;
    char label[32];
    struct FloatLit *next;
} FloatLit;

static StrLit *g_str_pool = NULL;
static FloatLit *g_float_pool = NULL;
static int g_str_count = 0;
static int g_float_count = 0;

static const char* pool_string_literal(const char* s){
    if(!s) s = "";
    for(StrLit *lit = g_str_pool; lit; lit = lit->next){
        if(strcmp(lit->text, s)==0){
            return lit->label;
        }
    }
    StrLit *new_lit = (StrLit *)malloc(sizeof(StrLit));
    new_lit->text = strdup(s);
    snprintf(new_lit->label, sizeof(new_lit->label), "str_%d", g_str_count++);
    new_lit->next = g_str_pool;
    g_str_pool = new_lit;
    return new_lit->label;
}

static const char* pool_float_literal(double v){
    for(FloatLit *lit = g_float_pool; lit; lit = lit->next){
        if(lit->value == v){
            return lit->label;
        }
    }
    FloatLit *new_lit = (FloatLit *)malloc(sizeof(FloatLit));
    new_lit->value = v;
    snprintf(new_lit->label, sizeof(new_lit->label), "flt_%d", g_float_count++);
    new_lit->next = g_float_pool;
    g_float_pool = new_lit;
    return new_lit->label;
}

static void emit_data_section(FILE *out){
    if(!out) return;
    if(!g_str_pool && !g_float_pool) return;
    fprintf(out, ".data\n");
    for(StrLit *lit = g_str_pool; lit; lit = lit->next){
        fprintf(out, "%s: .asciiz \"%s\"\n", lit->label, lit->text);
    }
    for(FloatLit *lit = g_float_pool; lit; lit = lit->next){
        fprintf(out, "%s: .float %f\n", lit->label, lit->value);
    }
}

// emitir li/addi ou lui/ori para constantes
static void emit_const(FILE *out, const char *rd, int k){
    unsigned int uk = (unsigned int)k;
    unsigned int upper = (uk >> 16) & 0xFFFFu;
    unsigned int lower = uk & 0xFFFFu;
    if(upper==0){
        fprintf(out, "    addi %s, $zero, %d\n", rd, k);
    } else {
        fprintf(out, "    lui $1, 0x%X\n", upper);
        fprintf(out, "    ori %s, $1, 0x%X\n", rd, lower);
        
    }
}

// carregar atom (temp/var/const) para registo

static char *load_atom_to_reg(FILE *out, const Atom *a, Table table){
    if(!a) return strdup("$t9");
    if(a->kind == ATOM_TEMP){
        const char *rt = reg_for_temp(a->contents.temp_id);
        return (char *)rt;
    }
    if(a->kind == ATOM_VAR){
        const char *name = a->contents.name ? a->contents.name : "";
        int offset = getVarOffset(table, name);
        char *rt = strdup("$t0");
        fprintf(out, "    lw %s, %d($fp)\n", rt, offset);
        return rt;
    }
    if(a->kind==ATOM_NUMBER || a->kind==ATOM_BOOLEAN){
        char *rt = "$t0";
        emit_const(out, (char *)rt, a->contents.ival);
        return (char *)rt;
    }
    if(a->kind==ATOM_STRING){
        const char *str_label = pool_string_literal(a->contents.sval ? a->contents.sval : "");
        char *rt = strdup("$t0");
        fprintf(out, "    la %s, %s\n", rt, str_label);
        return (char *)rt;
    }
    if(a->kind== ATOM_FLOAT){
        const char *flt_label = pool_float_literal(a->contents.fval);
        fprintf(out, "    la $t0, %s\n", flt_label);
        fprintf(out, "    lwc1 $f12, 0($t0)\n");
        return strdup("$f12");
    }
    return strdup("$t9"); // fallback
    // floats e strings não são tratados nesta função
}
// armazenar registo em destino (temp/var)
static void store_reg_to_dest(FILE *out, const char *rs, const char *dest, Table table){
    int tempId;
    if(is_temp_name(dest, &tempId)){
        const char *rd = reg_for_temp(tempId);
        fprintf(out, "    move %s, %s\n", rd, rs);
        free((void*)rd);
    } else {
        int offset = getVarOffset(table, dest);
        if(rs && rs[0]=='$' && rs[1]=='f'){
            fprintf(out, "   # WARN Storing float to variable not implemented\n");
        } else {
            fprintf(out, "    sw %s, %d($fp)\n", rs, offset);
        }
        
    }
}

static void emit_binop(FILE *out, const char* dest, const Atom *a1, binop bop,  const Atom *a2, Table table){
    char *r1 = load_atom_to_reg(out, a1, table);
    char *r2 = load_atom_to_reg(out, a2, table);
    char* rd = strdup("$t1"); // usar $t1 como registo temporário para resultado
    switch(bop){
        case BINOP_ADD:
            fprintf(out, "    add %s, %s, %s\n", rd, r1, r2);
            break;
        case BINOP_SUB:
            fprintf(out, "    sub %s, %s, %s\n", rd, r1, r2);
            break;
        case BINOP_MUL:
            fprintf(out, "    mul %s, %s, %s\n", rd, r1, r2);
            break;
        case BINOP_DIV:
            fprintf(out, "    div %s, %s\n", r1, r2);
            fprintf(out, "    mflo %s\n", rd);
            break;
        case BINOP_MOD:
        case BINOP_REM:
            fprintf(out, "    div %s, %s\n", r1, r2);
            fprintf(out, "    mfhi %s\n", rd);
            break;
        case BINOP_AND:
            fprintf(out, "    and %s, %s, %s\n", rd, r1, r2);
            break;
        case BINOP_OR:
            fprintf(out, "    or %s, %s, %s\n", rd, r1, r2);
            break;
        case BINOP_XOR:
            fprintf(out, "    xor %s, %s, %s\n", rd, r1, r2);
            break;
        case BINOP_POW:
            // não é suportado diretamente em MIPS-> podemos fazer usando outras funções
            char *base = strdup("$t4");
            char *exp = strdup("$t2");
            char *acc = strdup("$t3");
            fprintf(out, "    move %s, %s\n", base, r1);
            fprintf(out, "    move %s, %s\n", exp, r2);
            fprintf(out, "    addi %s, $zero, 1\n", acc);
            fprintf(out, "    beq %s, $zero, pow_end_%s\n", exp, dest);
            fprintf(out, "pow_loop_%s:\n", dest);
            fprintf(out, "    mul %s, %s, %s\n", acc, acc, base);
            fprintf(out, "    addi %s, %s, -1\n", exp, exp);
            fprintf(out, "    bne %s, $zero, pow_loop_%s\n", exp, dest);
            fprintf(out, "pow_end_%s:\n", dest);
            fprintf(out, "    move %s, %s\n", rd, acc);
            free(base);
            free(exp);
            free(acc);
            break;

        default:
            fprintf(out, "    # Unsupported binary operation\n");
        
    }
    
    store_reg_to_dest(out, rd, dest, table);
    free(r1);
    free(r2);
    free(rd);
}

static InstrNode* next_node(InstrNode *n){
    return n ? n->next : NULL;
}

static int is_label_with_name(const Instr *i, const char *name){
    return i && i->iop == OP_LABEL && i->dest && name && strcmp(i->dest, name)==0;
}

static void emit_relop_pair(FILE *out, const Atom *l, relop rop, const Atom *r, const char *labelT, const char *labelF, Table table){
    char *rl = load_atom_to_reg(out, l, table);
    char *rr = load_atom_to_reg(out, r, table);
    switch(rop){
        case RELOP_EQ:
            fprintf(out, "    beq %s, %s, %s\n", rl, rr, labelT);
            fprintf(out, "    bne %s, %s, %s\n", rl, rr, labelF);
            break;
        case RELOP_NEQ:
            fprintf(out, "    bne %s, %s, %s\n", rl, rr, labelT);
            fprintf(out, "    beq %s, %s, %s\n", rl, rr, labelF);
            break;
        case RELOP_LT:
            fprintf(out, "    slt $1, %s, %s\n", rl, rr);
            fprintf(out, "    bne $1, $zero, %s\n",labelT);
            fprintf(out, "    beq $1, $zero, %s\n",labelF);
            break;
        case RELOP_GT:
            fprintf(out, "    slt $1, %s, %s\n", rr, rl);
            fprintf(out, "    beq $1, $zero, %s\n",labelT);
            fprintf(out, "    bne $1, $zero, %s\n",labelF);
            break;
        case RELOP_LEQ:
            fprintf(out, "    slt $1, %s, %s\n", rr, rl);
            fprintf(out, "    bne $1, $zero, %s\n",labelT);
            fprintf(out, "    beq $1, $zero, %s\n",labelF);
            break;
        case RELOP_GEQ:
            fprintf(out, "    slt $1, %s, %s\n", rl, rr);
            fprintf(out, "    beq $1, $zero, %s\n",labelT);
            fprintf(out, "    bne $1, $zero, %s\n",labelF);
            break;
        default:
            fprintf(out, "    # Unsupported relational operation\n");
            break;
        
    }
    free(rl);
    free(rr);
}

void printMIPS(const InstrList *list, Table table, FILE *out){
    if(!list || !out) return;
    for(InstrNode *n = list->head ; n; n = n->next){
        Instr *i = n->instr;
        if(i->iop==OP_PRINT){
            if(i->arg1.kind == ATOM_STRING){
                (void) pool_string_literal(i->arg1.contents.sval);
            } else if(i->arg1.kind == ATOM_FLOAT){
                (void) pool_float_literal(i->arg1.contents.fval);
            }
        } else if(i->iop == OP_ASSIGN){
            if(i->arg1.kind == ATOM_STRING){
                (void) pool_string_literal(i->arg1.contents.sval);
            } else if(i->arg1.kind == ATOM_FLOAT){
                (void) pool_float_literal(i->arg1.contents.fval);
            }
            if(i->has_binop){
                if(i->arg2.kind == ATOM_STRING){
                    (void) pool_string_literal(i->arg2.contents.sval);
                } else if(i->arg2.kind == ATOM_FLOAT){
                    (void) pool_float_literal(i->arg2.contents.fval);
                }
            }
        }else if(i->iop==OP_COND){
            if(i->rel_left.kind == ATOM_STRING) (void)pool_string_literal(i->rel_left.contents.sval);
            if(i->rel_left.kind == ATOM_FLOAT) (void)pool_float_literal(i->rel_left.contents.fval);
            if(i->rel_right.kind == ATOM_STRING) (void)pool_string_literal(i->rel_right.contents.sval);
            if(i->rel_right.kind == ATOM_FLOAT) (void)pool_float_literal(i->rel_right.contents.fval);
        }

    }
    emit_data_section(out);

    fprintf(out, ".text\n");
    fprintf(out, ".globl main\n");
    fprintf(out, "main:\n");
    fprintf(out, "    addi $sp, $sp, -16\n");
    fprintf(out, "    sw $ra, 12($sp)\n");
    fprintf(out, "    sw $fp, 8($sp)\n");
    fprintf(out, "    addi $fp, $sp, 16\n");

    for(InstrNode *n = list->head; n; n = n->next){
        Instr *instr = n->instr;
        switch(instr->iop){
            case OP_LABEL:
                fprintf(out, "%s:\n", instr->dest ? instr->dest : "L_unknown");
                break;
            case OP_JUMP:
                fprintf(out, "    j %s\n", instr->dest ? instr->dest : "L_unknown");
                break;
            case OP_JUMP_FALSE:
                {
                    char *rc = load_atom_to_reg(out, &instr->cond, table);
                    fprintf(out, "    beq %s, $zero, %s\n", rc, instr->dest ? instr->dest : "L_unknown");
                    free(rc);
                    break;
                }
                
            case OP_PRINT:
                {
                    if(instr->arg1.kind == ATOM_STRING){
                        const char *str_label = pool_string_literal(instr->arg1.contents.sval ? instr->arg1.contents.sval : "");
                        fprintf(out, "    la $a0, %s\n", str_label);
                        fprintf(out, "    li $v0, 4\n");
                        fprintf(out, "    syscall\n");
                    } else if(instr->arg1.kind == ATOM_NUMBER || instr->arg1.kind == ATOM_BOOLEAN){
                        char *rv = load_atom_to_reg(out, &instr->arg1, table);
                        fprintf(out, "    move $a0, %s\n", rv);
                        fprintf(out, "    li $v0, 1\n");
                        fprintf(out, "    syscall\n");
                        free(rv);
                    } else if(instr->arg1.kind == ATOM_FLOAT){
                        char *rf = load_atom_to_reg(out, &instr->arg1, table);
                        fprintf(out, "    la $t0, %s\n", rf);
                        fprintf(out, "    lwc1 $f12, %s\n", rf);
                        fprintf(out, "    li $v0, 2\n");
                        fprintf(out, "    syscall\n");
                        free(rf);
                    } else {
                        fprintf(out, "    # Unsupported PRINT argument\n");
                    }
                    break;
                }
            case OP_READ:
                {
                    int offset = getVarOffset(table, instr->dest);
                    fprintf(out, "    li $v0, 5\n");
                    fprintf(out, "    syscall\n");
                    fprintf(out, "    sw $v0, %d($fp)\n", offset);
                }
            case OP_ASSIGN:
                {
                    if(!instr->has_binop){
                        char *r1 = load_atom_to_reg(out, &instr->arg1, table);
                        store_reg_to_dest(out, r1, instr->dest, table);
                        free(r1);
                    } else {
                        emit_binop(out, instr->dest,& instr->arg1, instr->bop, &instr->arg2, table);
                    }
                    break;
                }
            case  OP_COND:
                {
                    InstrNode *nn = next_node(n);
                    int next_is_true = nn && is_label_with_name(nn->instr, instr->label_true);
                    int next_is_false = next_is_true && next_node(nn) && is_label_with_name(next_node(nn)->instr, instr->label_false);
                    if(next_is_true){
                        switch(instr->rop){
                            case RELOP_EQ: {
                                char *rl = load_atom_to_reg(out, &instr->rel_left, table);
                                char *rr = load_atom_to_reg(out, &instr->rel_right, table);
                                fprintf(out, "    bne %s, %s, %s\n", rl, rr, instr->label_false);
                                free(rl);
                                free(rr);
                                break;
                            }
                            case RELOP_NEQ: {
                                char *rl = load_atom_to_reg(out, &instr->rel_left, table);
                                char *rr = load_atom_to_reg(out, &instr->rel_right, table);
                                fprintf(out, "    beq %s, %s, %s\n", rl, rr, instr->label_false);
                                free(rl);
                                free(rr);
                                break;
                            }
                            case RELOP_LT: {
                                char *rl = load_atom_to_reg(out, &instr->rel_left, table);
                                char *rr = load_atom_to_reg(out, &instr->rel_right, table);
                                fprintf(out, "    slt $1, %s, %s\n", rl, rr);
                                fprintf(out, "    beq $1, $zero, %s\n", instr->label_false);
                                free(rl);
                                free(rr);
                                break;
                            }
                            case RELOP_GT: {
                                char *rl = load_atom_to_reg(out, &instr->rel_left, table);
                                char *rr = load_atom_to_reg(out, &instr->rel_right, table);
                                fprintf(out, "    slt $1, %s, %s\n", rr, rl);
                                fprintf(out, "    beq $1, $zero, %s\n", instr->label_false);
                                free(rl);
                                free(rr);
                                break;
                            }
                            case RELOP_LEQ: {
                                char *rl = load_atom_to_reg(out, &instr->rel_left, table);
                                char *rr = load_atom_to_reg(out, &instr->rel_right, table);
                                fprintf(out, "    slt $1, %s, %s\n", rr, rl);
                                fprintf(out, "    bne $1, $zero, %s\n", instr->label_false);
                                free(rl);
                                free(rr);
                                break;
                            }
                            case RELOP_GEQ: {
                                char *rl = load_atom_to_reg(out, &instr->rel_left, table);
                                char *rr = load_atom_to_reg(out, &instr->rel_right, table);
                                fprintf(out, "    slt $1, %s, %s\n", rl, rr);
                                fprintf(out, "    bne $1, $zero, %s\n", instr->label_false);
                                free(rl);
                                free(rr);
                                break;
                            }
                            default:
                                fprintf(out, "    # Unsupported relational operation\n");
                                break;
                        }
                    } else if(next_is_false){
                        switch(instr->rop){
                            case RELOP_EQ: {
                                char *rl = load_atom_to_reg(out, &instr->rel_left, table);
                                char *rr = load_atom_to_reg(out, &instr->rel_right, table);
                                fprintf(out, "    beq %s, %s, %s\n", rl, rr, instr->label_true);
                                free(rl);
                                free(rr);
                                break;
                            }
                            case RELOP_NEQ: {
                                char *rl = load_atom_to_reg(out, &instr->rel_left, table);
                                char *rr = load_atom_to_reg(out, &instr->rel_right, table);
                                fprintf(out, "    bne %s, %s, %s\n", rl, rr, instr->label_true);
                                free(rl);
                                free(rr);
                                break;
                            }
                            case RELOP_LT: {
                                char *rl = load_atom_to_reg(out, &instr->rel_left, table);
                                char *rr = load_atom_to_reg(out, &instr->rel_right, table);
                                fprintf(out, "    slt $1, %s, %s\n", rl, rr);
                                fprintf(out, "    bne $1, $zero, %s\n", instr->label_true);
                                free(rl);
                                free(rr);
                                break;
                            }
                            case RELOP_GT: {
                                char *rl = load_atom_to_reg(out, &instr->rel_left, table);
                                char *rr = load_atom_to_reg(out, &instr->rel_right, table);
                                fprintf(out, "    slt $1, %s, %s\n", rr, rl);
                                fprintf(out, "    bne $1, $zero, %s\n", instr->label_true);
                                free(rl);
                                free(rr);
                                break;
                            }
                            case RELOP_LEQ: {
                                char *rl = load_atom_to_reg(out, &instr->rel_left, table);
                                char *rr = load_atom_to_reg(out, &instr->rel_right, table);
                                fprintf(out, "    slt $1, %s, %s\n", rr, rl);
                                fprintf(out, "    beq $1, $zero, %s\n", instr->label_true);
                                free(rl);
                                free(rr);
                                break;
                            }
                            case RELOP_GEQ: {
                                char *rl = load_atom_to_reg(out, &instr->rel_left, table);
                                char *rr = load_atom_to_reg(out, &instr->rel_right, table);
                                fprintf(out, "    slt $1, %s, %s\n", rl, rr);
                                fprintf(out, "    beq $1, $zero, %s\n", instr->label_true);
                                free(rl);
                                free(rr);
                                break;
                            }
                            default:
                                fprintf(out, "    # Unsupported relational operation\n");
                                break;
                        }
                    } else {
                        emit_relop_pair(out, &instr->rel_left, instr->rop, &instr->rel_right, instr->label_true, instr->label_false, table);
                    }
                    break;
                }
            default:
                fprintf(out, "    # Unsupported instruction\n");
                break;
        }
    }  
    fprintf(out, "   lw $fp, 8($sp)\n");
    fprintf(out, "   lw $ra, 12($sp)\n");
    fprintf(out, "   addi $sp, $sp, 16\n");
    fprintf(out, "   jr $ra\n");         
}

int generateMIPS(const InstrList *list, Table table, const char *out){
    FILE *f = fopen(out, "w");
    if(!f) return -1;
    printMIPS(list, table, f);
    fclose(f);
    return 0;
}

