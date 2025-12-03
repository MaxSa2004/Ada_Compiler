#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "codeGenerator.h"

CodeGenState cg_state = (CodeGenState){0};

static char *strdup_safe(const char *s){
    if (!s) return NULL;
    size_t n = strlen(s) + 1;
    char *p = (char *)malloc(n);
    if(!p) return NULL;
    memcpy(p, s, n);
    return p;
}

/* Lista de instruções */
InstrList *new_instr_list(void){
    InstrList *list = (InstrList *)malloc(sizeof(InstrList));
    if (!list) return NULL;
    list->head = NULL;
    list->tail = NULL;
    list->size = 0;
    return list;
}

void append_instr(InstrList *list, Instr *instr){
    if (!list || !instr) return;
    InstrNode *node = (InstrNode *)malloc(sizeof(InstrNode));
    if (!node) return;
    node->instr = instr;
    node->next = NULL;
    if (!list->head) {
        list->head = list->tail = node;
    } else {
        list->tail->next = node;
        list->tail = node;
    }
    list->size++;
}

Instr *get_first(const InstrList *list){
    if (!list || !list->head) return NULL;
    return list->head->instr;
}

InstrNode *next_instrs(const InstrNode *node){
    if (!node) return NULL;
    return node->next;
}

InstrList *get_instr_list(void){
    return cg_state.instructions;
}

/* Inicialização */
void init_code_generator(void){
    cg_state.instructions = new_instr_list();
    cg_state.temp_count = 0;
    cg_state.label_count = 0;
}

/* Libertação */
static void free_atom(Atom *a){
    if(!a) return;
    if(a->kind == ATOM_VAR && a->contents.name) {
        free(a->contents.name);
        a->contents.name = NULL;
    } else if(a->kind == ATOM_STRING && a->contents.sval) {
        free(a->contents.sval);
        a->contents.sval = NULL;
    }
}

void free_instr(Instr *instr){
    if(!instr) return;
    if(instr->dest) free(instr->dest);
    if(instr->label_true) free(instr->label_true);
    if(instr->label_false) free(instr->label_false);
    free_atom(&instr->arg1);
    free_atom(&instr->arg2);
    free_atom(&instr->cond);
    free_atom(&instr->rel_left);
    free_atom(&instr->rel_right);
    free(instr);
}

void free_instr_list(InstrList *list){
    if(!list) return;
    InstrNode *n = list->head;
    while(n){
        InstrNode *next = n->next;
        free_instr(n->instr);
        free(n);
        n = next;
    }
    free(list);
}

void free_code_generator(void){
    if(cg_state.instructions){
        free_instr_list(cg_state.instructions);
        cg_state.instructions = NULL;
    }
    cg_state.temp_count = 0;
    cg_state.label_count = 0;
}

/* Átomos */
Atom atom_var(const char *name){
    Atom a; a.kind = ATOM_VAR; a.contents.name = strdup_safe(name); return a;
}
Atom atom_number(int value){
    Atom a; a.kind = ATOM_NUMBER; a.contents.ival = value; return a;
}
Atom atom_float(double value){
    Atom a; a.kind = ATOM_FLOAT; a.contents.fval = value; return a;
}
Atom atom_string(const char *value){
    Atom a; a.kind = ATOM_STRING; a.contents.sval = strdup_safe(value); return a;
}
Atom atom_boolean(int value){
    Atom a; a.kind = ATOM_BOOLEAN; a.contents.ival = value ? 1 : 0; return a;
}
Atom atom_temp(int temp_id){
    Atom a; a.kind = ATOM_TEMP; a.contents.temp_id = temp_id; return a;
}
Atom new_temp(void){
    int id = cg_state.temp_count++;
    return atom_temp(id);
}

/* Emissores */
static Instr *new_instr(instrop op){
    Instr *i = (Instr*)malloc(sizeof(Instr));
    if(!i) return NULL;
    memset(i, 0, sizeof(Instr));
    i->iop = op;
    return i;
}

Instr *emit2(const char *dest, Atom src){
    Instr *i = new_instr(OP_ASSIGN);
    if(!i) return NULL;
    i->dest = strdup_safe(dest);
    i->arg1 = src;
    i->has_binop = false;
    append_instr(cg_state.instructions, i);
    return i;
}

Instr *emit3_binop(const char *dest, Atom arg1, binop op, Atom arg2){
    Instr *i = new_instr(OP_ASSIGN);
    if(!i) return NULL;
    i->dest = strdup_safe(dest);
    i->arg1 = arg1;
    i->arg2 = arg2;
    i->has_binop = true;
    i->bop = op;
    append_instr(cg_state.instructions, i);
    return i;
}

/* Pseudo-instrução COND para impressão compacta */
Instr *emit3_relop(Atom arg1, relop op, Atom arg2, const char *true_label, const char *false_label){
    Instr *i = new_instr(OP_COND);
    if(!i) return NULL;
    i->rel_left = arg1;
    i->rel_right = arg2;
    i->rop = op;
    i->label_true = strdup_safe(true_label);
    i->label_false = strdup_safe(false_label);
    append_instr(cg_state.instructions, i);
    return i;
}

Instr *emit_label(const char *label){
    Instr *i = new_instr(OP_LABEL);
    if(!i) return NULL;
    i->dest = strdup_safe(label);
    append_instr(cg_state.instructions, i);
    return i;
}

Instr *emit_jump(const char *label){
    Instr *i = new_instr(OP_JUMP);
    if(!i) return NULL;
    i->dest = strdup_safe(label);
    append_instr(cg_state.instructions, i);
    return i;
}

Instr *emit_jump_false(Atom cond, const char *label){
    Instr *i = new_instr(OP_JUMP_FALSE);
    if(!i) return NULL;
    i->cond = cond;
    i->dest = strdup_safe(label);
    append_instr(cg_state.instructions, i);
    return i;
}

Instr *emit_print(Atom value){
    Instr *i = new_instr(OP_PRINT);
    if(!i) return NULL;
    i->arg1 = value;
    append_instr(cg_state.instructions, i);
    return i;
}

Instr *emit_read(const char *var_name){
    Instr *i = new_instr(OP_READ);
    if(!i) return NULL;
    i->dest = strdup_safe(var_name);
    append_instr(cg_state.instructions, i);
    return i;
}

/* Impressão */
static const char *binop_to_str(binop op){
    switch(op){
        case BINOP_ADD: return "+";
        case BINOP_SUB: return "-";
        case BINOP_MUL: return "*";
        case BINOP_DIV: return "/";
        case BINOP_MOD: return "%";
        case BINOP_REM: return "rem";
        case BINOP_POW: return "^";
        case BINOP_AND: return "AND";
        case BINOP_OR:  return "OR";
        case BINOP_XOR: return "XOR";
        default: return "?";
    }
}
static const char *relop_to_str(relop op){
    switch(op){
        case RELOP_EQ:  return "==";
        case RELOP_NEQ: return "!=";
        case RELOP_LT:  return "<";
        case RELOP_GT:  return ">";
        case RELOP_LEQ: return "<=";
        case RELOP_GEQ: return ">=";
        default: return "?";
    }
}
static void print_atom(const Atom *a){
    if(!a){printf("(null)"); return;}
    switch(a->kind){
        case ATOM_VAR:    printf("%s", a->contents.name ? a->contents.name : "(var-null)"); break;
        case ATOM_NUMBER: printf("%d", a->contents.ival); break;
        case ATOM_FLOAT:  printf("%g", a->contents.fval); break;
        case ATOM_STRING: printf("\"%s\"", a->contents.sval ? a->contents.sval : ""); break;
        case ATOM_BOOLEAN:printf("%d", a->contents.ival ? 1 : 0); break;
        case ATOM_TEMP:   printf("t%d", a->contents.temp_id); break;
        default:          printf("(unknown atom)"); break;
    }
}
void print_instr(const Instr *instr){
    if(!instr) {printf("(null-instr)\n"); return;}
    switch(instr->iop){
        case OP_ASSIGN:
            printf("%s := ", instr->dest ? instr->dest : "(null-dest)");
            print_atom(&instr->arg1);
            if(instr->has_binop){
                printf(" %s ", binop_to_str(instr->bop));
                print_atom(&instr->arg2);
            }
            printf("\n");
            break;
        case OP_LABEL:
            printf("LABEL %s\n", instr->dest ? instr->dest : "(null-label)");
            break;
        case OP_JUMP:
            printf("JUMP %s\n", instr->dest ? instr->dest : "(null-label)");
            break;
        case OP_JUMP_FALSE:
            printf("JUMP_FALSE ");
            print_atom(&instr->cond);
            printf(" %s\n", instr->dest ? instr->dest : "(null-label)");
            break;
        case OP_PRINT:
            printf("PUT ");
            print_atom(&instr->arg1);
            printf("\n");
            break;
        case OP_READ:
            printf("GET %s\n", instr->dest ? instr->dest : "(null-dest)");
            break;
        case OP_COND:
            printf("COND ");
            print_atom(&instr->rel_left);
            printf(" %s ", relop_to_str(instr->rop));
            print_atom(&instr->rel_right);
            printf(" %s %s\n",
                   instr->label_true ? instr->label_true : "(null-true)",
                   instr->label_false ? instr->label_false : "(null-false)");
            break;
        default:
            printf("(unknown-instr)\n");
    }
}
void print_instr_list(const InstrList *list){
    if(!list) {printf("(null-instr-list)\n"); return;}
    for (InstrNode *n = list->head; n; n = n->next) {
        print_instr(n->instr);
    }
}

/* Helpers de mapeamento do AST (ajusta para nomes reais do teu ast.h) */
static const char* lookup_var_name(const char *id, Table table){
    (void)table;
    return id ? id : "(null-id)";
}
static int map_ast_binop_to_binop(int ast_op, binop *out){
    switch(ast_op){
        case SUM:        *out = BINOP_ADD; return 1;
        case SUB:        *out = BINOP_SUB; return 1;
        case TIMES:      *out = BINOP_MUL; return 1;
        case DIVISION:   *out = BINOP_DIV; return 1;
        case MODULUS:    *out = BINOP_MOD; return 1;
        case REMAINDER:  *out = BINOP_REM; return 1;
        case POW:        *out = BINOP_POW; return 1;
        case ANDexp:     *out = BINOP_AND; return 1;
        case ORexp:      *out = BINOP_OR;  return 1;
        case XORexp:     *out = BINOP_XOR; return 1;
        default: return 0;
    }
}
static int map_ast_relop_to_relop(int ast_op, relop *out){
    switch(ast_op){
        case EQUAL:      *out = RELOP_EQ;  return 1;
        case INEQUAL:    *out = RELOP_NEQ; return 1;
        case LESSexp:    *out = RELOP_LT;  return 1;
        case GREATERexp: *out = RELOP_GT;  return 1;
        case LEQexp:     *out = RELOP_LEQ; return 1;
        case GEQexp:     *out = RELOP_GEQ; return 1;
        default: return 0;
    }
}

/* Compiladores de expressão/condição */
static void compileExpr(Exp e, Table table, const char* dest_name);

static void compileCond(Exp e, Table table, const char *labelT, const char *labelF){
    if(!e) {emit_jump(labelF); return;}
    switch(e->exp_t){
        case BOOLEXP:
            if(e->fields.boolVal) emit_jump(labelT);
            else emit_jump(labelF);
            break;
        case UNOEXP:
            if(e->fields.unoexp.op == NOTexp){
                compileCond(e->fields.unoexp.child, table, labelF, labelT);
            } else {
                Atom t = new_temp();
                char tn[32]; snprintf(tn, sizeof(tn), "t%d", t.contents.temp_id);
                compileExpr(e->fields.unoexp.child, table, tn);
                emit3_relop(t, RELOP_NEQ, atom_number(0), labelT, labelF);
            }
            break;
        case OPEXP: {
            int op = e->fields.opexp.op;
            relop rop;
            if (map_ast_relop_to_relop(op, &rop)) {
                Atom t1 = new_temp(), t2 = new_temp();
                char t1n[32], t2n[32];
                snprintf(t1n, sizeof(t1n), "t%d", t1.contents.temp_id);
                snprintf(t2n, sizeof(t2n), "t%d", t2.contents.temp_id);
                compileExpr(e->fields.opexp.left, table, t1n);
                compileExpr(e->fields.opexp.right, table, t2n);
                /* Apenas COND para o formato do prof; não emitir JUMPs aqui */
                emit3_relop(t1, rop, t2, labelT, labelF);
            } else if (op == ANDexp) {
                char Lmid[32]; snprintf(Lmid, sizeof(Lmid), "L%d", cg_state.label_count++);
                compileCond(e->fields.opexp.left, table, Lmid, labelF);
                emit_label(Lmid);
                compileCond(e->fields.opexp.right, table, labelT, labelF);
            } else if (op == ORexp) {
                char Lmid[32]; snprintf(Lmid, sizeof(Lmid), "L%d", cg_state.label_count++);
                compileCond(e->fields.opexp.left, table, labelT, Lmid);
                emit_label(Lmid);
                compileCond(e->fields.opexp.right, table, labelT, labelF);
            } else if (op == XORexp) {
                /* simplificação: (a && !b) || (!a && b) */
                char Lmid[32]; snprintf(Lmid, sizeof(Lmid), "L%d", cg_state.label_count++);
                compileCond(e->fields.opexp.left, table, Lmid, labelF);
                emit_label(Lmid);
                compileCond(e->fields.opexp.right, table, labelF, labelT); /* !b */
                /* Para impressão estilo prof, não emitimos jumps aqui */
            } else {
                /* expressão aritmética como condição: exp != 0 */
                Atom t = new_temp();
                char tn[32]; snprintf(tn, sizeof(tn), "t%d", t.contents.temp_id);
                compileExpr(e->fields.opexp.left, table, tn);
                emit3_relop(t, RELOP_NEQ, atom_number(0), labelT, labelF);
            }
            break;
        }
        case PAREXP:
            compileCond(e->fields.parexp.inner, table, labelT, labelF);
            break;
        default: {
            Atom t = new_temp();
            char tn[32]; snprintf(tn, sizeof(tn), "t%d", t.contents.temp_id);
            compileExpr(e, table, tn);
            emit3_relop(t, RELOP_NEQ, atom_number(0), labelT, labelF);
            break;
        }
    }
}

static void compileExpr(Exp e, Table table, const char* dest_name){
    if(!e || !dest_name) return;
    switch(e->exp_t){
        case NUMEXP: emit2(dest_name, atom_number(e->fields.num)); break;
        case FLOATEXP: emit2(dest_name, atom_float(e->fields.fnum)); break;
        case STREXP: emit2(dest_name, atom_string(e->fields.string)); break;
        case BOOLEXP: emit2(dest_name, atom_boolean(e->fields.boolVal ? 1 : 0)); break;
        case IDEXP: {
            const char *var_name = lookup_var_name(e->fields.ident, table);
            emit2(dest_name, atom_var(var_name));
            break;
        }
        case UNOEXP: {
            Atom t1 = new_temp();
            char t1n[32]; snprintf(t1n, sizeof(t1n), "t%d", t1.contents.temp_id);
            compileExpr(e->fields.unoexp.child, table, t1n);
            if(e->fields.unoexp.op == NOTexp){
                /* dest := 1/0 via cond */
                char Ltrue[32], Lfalse[32], Lend[32];
                snprintf(Ltrue, sizeof(Ltrue), "L%d", cg_state.label_count++);
                snprintf(Lfalse, sizeof(Lfalse), "L%d", cg_state.label_count++);
                snprintf(Lend,  sizeof(Lend),  "L%d", cg_state.label_count++);
                emit3_relop(t1, RELOP_NEQ, atom_number(0), Ltrue, Lfalse);
                emit_label(Ltrue);
                emit2(dest_name, atom_number(0));
                emit_jump(Lend);
                emit_label(Lfalse);
                emit2(dest_name, atom_number(1));
                emit_label(Lend);
            } else {
                /* NEG: dest := 0 - t1 */
                emit3_binop(dest_name, atom_number(0), BINOP_SUB, t1);
            }
            break;
        }
        case OPEXP: {
            binop bop;
            if(map_ast_binop_to_binop(e->fields.opexp.op, &bop)){
                Atom t1 = new_temp(), t2 = new_temp();
                char t1n[32], t2n[32];
                snprintf(t1n, sizeof(t1n), "t%d", t1.contents.temp_id);
                snprintf(t2n, sizeof(t2n), "t%d", t2.contents.temp_id);
                compileExpr(e->fields.opexp.left, table, t1n);
                compileExpr(e->fields.opexp.right, table, t2n);
                emit3_binop(dest_name, t1, bop, t2);
            } else {
                /* Relacionais/lógicos: dest := 1/0 via cond */
                char Ltrue[32], Lfalse[32], Lend[32];
                snprintf(Ltrue, sizeof(Ltrue), "L%d", cg_state.label_count++);
                snprintf(Lfalse, sizeof(Lfalse), "L%d", cg_state.label_count++);
                snprintf(Lend,  sizeof(Lend),  "L%d", cg_state.label_count++);
                compileCond(e, table, Ltrue, Lfalse);
                emit_label(Ltrue);
                emit2(dest_name, atom_number(1));
                emit_jump(Lend);
                emit_label(Lfalse);
                emit2(dest_name, atom_number(0));
                emit_label(Lend);
            }
            break;
        }
        case PAREXP:
            compileExpr(e->fields.parexp.inner, table, dest_name);
            break;
        default:
            emit2(dest_name, atom_number(0));
            break;
    }
}

/* API mantida */
Atom transExpr(Exp e){
    Atom dest = new_temp();
    char buf[32];
    snprintf(buf, sizeof(buf), "t%d", dest.contents.temp_id);
    compileExpr(e, NULL, buf);
    return dest;
}
void transCond(Exp e, Atom labelF, Atom labelT){
    const char *lf = (labelF.kind == ATOM_VAR && labelF.contents.name) ? labelF.contents.name : "LFalse";
    const char *lt = (labelT.kind == ATOM_VAR && labelT.contents.name) ? labelT.contents.name : "LTrue";
    compileCond(e, NULL, lt, lf);
}

/* Statements, com while a imprimir loop/cond/end */
void transStm(Stm s){
    if(!s) return;
    switch(s->stm_t){
        case ASSIGNSTM: {
            const char *dest_name = lookup_var_name(s->fields.assign.ident, NULL);
            compileExpr(s->fields.assign.exp, NULL, dest_name);
            break;
        }
        case COMPOUNDSTM:
            transStm(s->fields.compound.fst);
            transStm(s->fields.compound.snd);
            break;
        case IFSTM: {
            char Ltrue[32], Lfalse[32], Lend[32];
            snprintf(Ltrue, sizeof(Ltrue), "L%d", cg_state.label_count++);
            snprintf(Lfalse, sizeof(Lfalse), "L%d", cg_state.label_count++);
            snprintf(Lend, sizeof(Lend), "L%d", cg_state.label_count++);
            compileCond(s->fields.ifstm.cond, NULL, Ltrue, Lfalse);
            emit_label(Ltrue);
            transStm(s->fields.ifstm.then_branch);
            if(s->fields.ifstm.else_branch){
                emit_jump(Lend);
                emit_label(Lfalse);
                transStm(s->fields.ifstm.else_branch);
                emit_label(Lend);
            } else {
                emit_label(Lfalse);
            }
            break;
        }
        case WHILESTM: {
            /* Imprimir exatamente: JUMP cond; LABEL loop; ...; LABEL cond; COND ... loop end; LABEL end */
            const char *Lcond = "cond";
            const char *Lloop = "loop";
            const char *Lend  = "end";
            emit_jump(Lcond);
            emit_label(Lloop);
            transStm(s->fields.whilestm.branch);
            emit_label(Lcond);
            compileCond(s->fields.whilestm.cond, NULL, Lloop, Lend); /* emite OP_COND */
            emit_label(Lend);
            break;
        }
        case PUTSTM: {
            Exp e = s->fields.putstm.output;
            if(e->exp_t==STREXP){
                emit_print(atom_string(e->fields.string));
            }
            else if(e->exp_t == IDEXP){
                const char *var_name = lookup_var_name(e->fields.ident, NULL);
                emit_print(atom_var(var_name));
            }
            else {
                Atom val = transExpr(e);
                emit_print(val);
            }
            break;
            
        }
        case GETSTM: {
            const char *var_name = lookup_var_name(s->fields.getstm.ident, NULL);
            emit_read(var_name);
            break;
        }
        case PROCSTM:
            transStm(s->fields.proc.statements);
            break;
        default: break;
    }
}