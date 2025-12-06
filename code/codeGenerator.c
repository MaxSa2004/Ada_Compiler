#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "codeGenerator.h"


CodeGenState cg_state = (CodeGenState){0}; // global state of the code generator initialized to zero
/* secure string duplication *
*/
static char *strdup_safe(const char *s){
    if (!s) return NULL;
    size_t n = strlen(s) + 1;
    char *p = (char *)malloc(n);
    if(!p) return NULL;
    memcpy(p, s, n);
    return p;
}

/* create new instruction list */
InstrList *new_instr_list(void){
    InstrList *list = (InstrList *)malloc(sizeof(InstrList));
    if (!list) return NULL;
    list->head = NULL;
    list->tail = NULL;
    list->size = 0;
    return list;
}
/* append instruction to list */
void append_instr(InstrList *list, Instr *instr){
    if (!list || !instr) return;
    InstrNode *node = (InstrNode *)malloc(sizeof(InstrNode));
    if (!node) return;
    node->instr = instr;
    node->next = NULL;
    if (!list->head) { // empty list (first element is the head and tail)
        list->head = list->tail = node;
    } else { // non-empty list (append to tail)
        list->tail->next = node;
        list->tail = node;
    }
    list->size++;
}
/* get head of list */
Instr *get_first(const InstrList *list){
    if (!list || !list->head) return NULL;
    return list->head->instr;
}
/* get next instruction of a node */
InstrNode *next_instrs(const InstrNode *node){
    if (!node) return NULL;
    return node->next;
}
/* get the whole list */
InstrList *get_instr_list(void){
    return cg_state.instructions;
}

/* initialization of code gen */
void init_code_generator(void){
    cg_state.instructions = new_instr_list();
    cg_state.temp_count = 0;
    cg_state.label_count = 0;
}

/* memory management (free) */
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

/* Atoms */
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
/* create new temp */
Atom new_temp(void){
    int id = cg_state.temp_count++;
    return atom_temp(id);
}

/* create new instruction*/
static Instr *new_instr(instrop op){
    Instr *i = (Instr*)malloc(sizeof(Instr));
    if(!i) return NULL;
    memset(i, 0, sizeof(Instr));
    i->iop = op;
    return i;
}
/* emitters */
/* emitter for 2 args */
Instr *emit2(const char *dest, Atom src){
    Instr *i = new_instr(OP_ASSIGN);
    if(!i) return NULL;
    i->dest = strdup_safe(dest);
    i->arg1 = src;
    i->has_binop = false;
    append_instr(cg_state.instructions, i);
    return i;
}
/*emitter for 3 args in binary operation*/
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

/* emitter for 3 args in relational operation */
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
/*emitter for label*/
Instr *emit_label(const char *label){
    Instr *i = new_instr(OP_LABEL);
    if(!i) return NULL;
    i->dest = strdup_safe(label);
    append_instr(cg_state.instructions, i);
    return i;
}
/*emitter for jump*/
Instr *emit_jump(const char *label){
    Instr *i = new_instr(OP_JUMP);
    if(!i) return NULL;
    i->dest = strdup_safe(label);
    append_instr(cg_state.instructions, i);
    return i;
}
/*emitter for jump false*/
Instr *emit_jump_false(Atom cond, const char *label){
    Instr *i = new_instr(OP_JUMP_FALSE);
    if(!i) return NULL;
    i->cond = cond;
    i->dest = strdup_safe(label);
    append_instr(cg_state.instructions, i);
    return i;
}
/*emitter for print (put)*/
Instr *emit_print(Atom value){
    Instr *i = new_instr(OP_PRINT);
    if(!i) return NULL;
    i->arg1 = value;
    append_instr(cg_state.instructions, i);
    return i;
}
/*emitter for read(get)*/
Instr *emit_read(const char *var_name){
    Instr *i = new_instr(OP_READ);
    if(!i) return NULL;
    i->dest = strdup_safe(var_name);
    append_instr(cg_state.instructions, i);
    return i;
}

/* printing operators */
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
/*printing atoms (their values)*/
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
/*printing instructions*/
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
/*printing instruction list*/
void print_instr_list(const InstrList *list){
    if(!list) {printf("(null-instr-list)\n"); return;}
    for (InstrNode *n = list->head; n; n = n->next) {
        print_instr(n->instr);
    }
}

/* helpers for AST mapping */
/* looking for a variable name in a table, to check its existance */
static const char* lookup_var_name(const char *id, Table table){
    (void)table;
    return id ? id : "(null-id)";
}
/*mapping binary operators*/
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
/*mapping relational operators*/
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

/* compiler for expressions (def) */
static void compileExpr(Exp e, Table table, const char* dest_name);
/*compiler for conditions*/
static void compileCond(Exp e, Table table, const char *labelT, const char *labelF){
    if(!e) {emit_jump(labelF); return;}
    switch(e->exp_t){
        case BOOLEXP: // in a boolean expression, jump directly (if the value is 0 then jump to false label, else to true)
            if(e->fields.boolVal) emit_jump(labelT);
            else emit_jump(labelF);
            break; 
        case UNOEXP:  // in a unary expression, handle NOT operator by compiling what's inside, else it's a NEG operator: create a temp and a buffer, compile the inner expression into the temp, and emit a COND instruction checking if the temp is not equal to 0
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
            // in a binary operation expression, check if it's a relational operator, if so create two temps and buffers, compile left and right expressions into the temps, and emit a COND instruction with the relational operator; 
            if (map_ast_relop_to_relop(op, &rop)) {
                Atom t1 = new_temp(), t2 = new_temp();
                char t1n[32], t2n[32];
                snprintf(t1n, sizeof(t1n), "t%d", t1.contents.temp_id);
                snprintf(t2n, sizeof(t2n), "t%d", t2.contents.temp_id);
                compileExpr(e->fields.opexp.left, table, t1n);
                compileExpr(e->fields.opexp.right, table, t2n);
                emit3_relop(t1, rop, t2, labelT, labelF);
                // if it's a logical AND, OR, or XOR, handle short-circuit evaluation by creating mid labels and compiling left and right expressions accordingly; 
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
                char Lmid[32]; snprintf(Lmid, sizeof(Lmid), "L%d", cg_state.label_count++);
                compileCond(e->fields.opexp.left, table, Lmid, labelF);
                emit_label(Lmid);
                compileCond(e->fields.opexp.right, table, labelF, labelT); 
            } else { // if it's an arithmetic expression, create a temp and buffer, compile the left expression into the temp, and emit a COND instruction checking if the temp is not equal to 0
                Atom t = new_temp();
                char tn[32]; snprintf(tn, sizeof(tn), "t%d", t.contents.temp_id);
                compileExpr(e->fields.opexp.left, table, tn);
                emit3_relop(t, RELOP_NEQ, atom_number(0), labelT, labelF);
            }
            break;
        }
        case PAREXP: // in a parenthesized expression, compile what's inside
            compileCond(e->fields.parexp.inner, table, labelT, labelF);
            break;
        default: { // in other expressions, create a temp and buffer, compile the expression into the temp, and emit a COND instruction checking if the temp is not equal to 0
            Atom t = new_temp();
            char tn[32]; snprintf(tn, sizeof(tn), "t%d", t.contents.temp_id);
            compileExpr(e, table, tn);
            emit3_relop(t, RELOP_NEQ, atom_number(0), labelT, labelF);
            break;
        }
    }
}
// compiler for expressions
static void compileExpr(Exp e, Table table, const char* dest_name){
    if(!e || !dest_name) return;
    switch(e->exp_t){
        case NUMEXP: emit2(dest_name, atom_number(e->fields.num)); break; // in a numeric expression, emit an assignment instruction with the number
        case FLOATEXP: emit2(dest_name, atom_float(e->fields.fnum)); break; // in a float expression, emit an assignment instruction with the float
        case STREXP: emit2(dest_name, atom_string(e->fields.string)); break; // in a string expression, emit an assignment instruction with the string
        case BOOLEXP: emit2(dest_name, atom_boolean(e->fields.boolVal ? 1 : 0)); break; // in a boolean expression, emit an assignment instruction with 1 or 0
        case IDEXP: { // in an identifier expression, look up the variable name in the table and emit an assignment instruction with the variable
            const char *var_name = lookup_var_name(e->fields.ident, table);
            emit2(dest_name, atom_var(var_name));
            break;
        }
        case UNOEXP: { // in a unary expression, create a temp and buffer, compile the inner expression into the temp, and depending on the operator (NOT or NEG), emit the appropriate instructions
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
        case OPEXP: { // in a binary operation expression, check if it's an arithmetic operator, if so create two temps and buffers, compile left and right expressions into the temps, and emit a binary operation instruction;
            binop bop;
            if(map_ast_binop_to_binop(e->fields.opexp.op, &bop)){
                Atom t1 = new_temp(), t2 = new_temp();
                char t1n[32], t2n[32];
                snprintf(t1n, sizeof(t1n), "t%d", t1.contents.temp_id);
                snprintf(t2n, sizeof(t2n), "t%d", t2.contents.temp_id);
                compileExpr(e->fields.opexp.left, table, t1n);
                compileExpr(e->fields.opexp.right, table, t2n);
                emit3_binop(dest_name, t1, bop, t2);
            } else { // if it's a logical operator, create a temp and buffer, compile the expression into the temp, and emit instructions to set dest to 1 or 0 based on the value of the temp
                /* dest := 1/0 via cond */
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
        case PAREXP: // in a parenthesized expression, compile what's inside
            compileExpr(e->fields.parexp.inner, table, dest_name);
            break;
        default: // in other expressions, emit an assignment instruction with 0
            emit2(dest_name, atom_number(0));
            break;
    }
}

// translators for expressions using the compiler functions
Atom transExpr(Exp e){
    Atom dest = new_temp();
    char buf[32];
    snprintf(buf, sizeof(buf), "t%d", dest.contents.temp_id);
    compileExpr(e, NULL, buf);
    return dest;
}
// translator for conditions using the compiler functions
void transCond(Exp e, Atom labelF, Atom labelT){
    const char *lf = (labelF.kind == ATOM_VAR && labelF.contents.name) ? labelF.contents.name : "LFalse";
    const char *lt = (labelT.kind == ATOM_VAR && labelT.contents.name) ? labelT.contents.name : "LTrue";
    compileCond(e, NULL, lt, lf);
}

/* translator for statements using the compiler functions */
void transStm(Stm s){
    if(!s) return;
    switch(s->stm_t){
        case ASSIGNSTM: { // in an assignment statement, look up the variable name in the table and compile the expression into that variable
            const char *dest_name = lookup_var_name(s->fields.assign.ident, NULL);
            compileExpr(s->fields.assign.exp, NULL, dest_name);
            break;
        }
        case COMPOUNDSTM: // in a compound statement, translate the first and second statements
            transStm(s->fields.compound.fst);
            transStm(s->fields.compound.snd);
            break;
        case IFSTM: { // in an if statement, create labels for true, false, and end, compile the condition into the true and false labels, translate the then branch, and if there's an else branch, emit a jump to the end label, translate the else branch, and emit the end label; otherwise, just emit the false label
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
        case WHILESTM: { // in a while statement, create labels for condition, loop, and end, emit a jump to the condition label, emit the loop label, translate the branch statements, emit the condition label, compile the condition into the loop and end labels, and emit the end label;
            /* Printing: JUMP cond; LABEL loop; ...; LABEL cond; COND ... loop end; LABEL end */
            const char *Lcond = "cond";
            const char *Lloop = "loop";
            const char *Lend  = "end";
            emit_jump(Lcond);
            emit_label(Lloop);
            transStm(s->fields.whilestm.branch);
            emit_label(Lcond);
            compileCond(s->fields.whilestm.cond, NULL, Lloop, Lend); /*  OP_COND */
            emit_label(Lend);
            break;
        }
        case PUTSTM: { // in a put (print) statement, check the type of the output expression and emit a print instruction accordingly
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
        case GETSTM: { // in a get (read) statement, look up the variable name in the table and emit a read instruction for that variable
            const char *var_name = lookup_var_name(s->fields.getstm.ident, NULL);
            emit_read(var_name);
            break;
        }
        case PROCSTM: // in a procedure statement, translate the procedure's statements
            transStm(s->fields.proc.statements);
            break;
        default: break;
    }
}