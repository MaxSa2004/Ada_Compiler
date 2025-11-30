// função transExpr : (Exp, Table, Temp) -> [Instr] recebe o destino onde colocar o resultado
// função transStm : (Stm, Table) -> [Instr]
// função transCond para compilar condições: transCond : (Cond, Table, Label(t), Label(f)) -> [Instr]
// função transArgs : (Exps, table) -> ...s
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "codeGenerator.h"
// gerar temporários e etiquetas
Instr *instr_head = NULL, *instr_tail = NULL;
int temp_count = 0, label_count = 0;
void emit(Opcode op, Op dest, Op arg1, Op arg2);
// static const char *getOpCodeString(Opcode op);
typedef struct VarTemp
{
    char *name;
    Op temp;
    struct VarTemp *next;
} VarTemp;
static VarTemp *var_temp_head = NULL;
static VarTemp *var_temp_tail = NULL;

static Op getVarTemp(char *name)
{
    VarTemp *curr = var_temp_head;
    while (curr)
    {
        if (strcmp(curr->name, name) == 0)
        {
            return curr->temp;
        }
        curr = curr->next;
    }
    Op temp = {0};
    temp.kind = OP_TEMP;
    temp.contents.temp_id = temp_count++;
    VarTemp *new_var_temp = (VarTemp *)malloc(sizeof(VarTemp));
    new_var_temp->name = strdup(name);
    new_var_temp->temp = temp;
    new_var_temp->next = NULL;
    if (var_temp_tail == NULL)
    {
        var_temp_head = var_temp_tail = new_var_temp;
    }
    else
    {
        var_temp_tail->next = new_var_temp;
        var_temp_tail = new_var_temp;
    }
    return temp;
}

static void freeVarTemps()
{
    VarTemp *curr = var_temp_head;
    while (curr)
    {
        VarTemp *next = curr->next;
        if (curr->name)
            free(curr->name);
        free(curr);
        curr = next;
    }
    var_temp_head = NULL;
}

// criar operador temporário
Op newTemp()
{
    Op op;
    op.kind = OP_TEMP;
    op.contents.temp_id = temp_count++;
    return op;
}

// criar operador label
Op newLabel()
{
    Op op;
    op.kind = OP_VAR;
    char buffer[20];
    snprintf(buffer, sizeof(buffer), "Label%d", label_count++);
    op.contents.name = strdup(buffer);
    return op;
}

Op newVar(char *name)
{
    Op op;
    op.kind = OP_VAR;
    op.contents.name = strdup(name);
    return op;
}

// criar operador constante
Op newConstInt(int val)
{
    Op op;
    op.kind = OP_CONST_INT;
    op.contents.ival = val;
    return op;
}

Op newConstFloat(double f)
{
    Op op;
    op.kind = OP_CONST_FLOAT;
    op.contents.fval = f;
    return op;
}

Op newConstString(char *s)
{
    Op op;
    op.kind = OP_CONST_STRING;
    op.contents.sval = strdup(s);
    return op;
}

void init_code_generator()
{
    instr_head = NULL;
    instr_tail = NULL;
    temp_count = 0;
    label_count = 0;
}

void emit(Opcode op, Op dest, Op arg1, Op arg2)
{
    Instr *new_instr = (Instr *)malloc(sizeof(Instr));
    new_instr->opcode = op;
    new_instr->arg3 = dest;
    new_instr->arg1 = arg1;
    new_instr->arg2 = arg2;
    new_instr->next = NULL;
    if (instr_head == NULL)
    {
        instr_head = new_instr;
        instr_tail = new_instr;
    }
    else
    {
        instr_tail->next = new_instr;
        instr_tail = new_instr;
    }
    // fprintf(stderr, "Emitted instruction: %s\n", getOpCodeString(op));
}

Op transExpr(Exp exp)
{
    Op dest, t1, t2;
    Op empty = {0};
    if (!exp)
        return empty;

    switch (exp->exp_t)
    {
    case NUMEXP:
    {
        Op c = newConstInt(exp->fields.num);
        Op dest = newTemp();
        emit(OP_MOVE, dest, c, empty);
        return dest;
    }

    case FLOATEXP:
    {
        Op c = newConstFloat(exp->fields.fnum);
        Op dest = newTemp();
        emit(OP_MOVE, dest, c, empty);
        return dest;
    }
    case IDEXP:
        return getVarTemp(exp->fields.ident);
    case STREXP:
    {
        Op s = newConstString(exp->fields.string);
        return s;
    }

    case BOOLEXP:
    {
        Op dest = newTemp();
        if (exp->fields.boolVal)
        {
            Op one = newConstInt(1);
            emit(OP_MOVE, dest, one, empty);
        }
        else
        {
            Op zero = newConstInt(0);
            emit(OP_MOVE, dest, zero, empty);
        }
        return dest;
    }

    case UNOEXP:
        t1 = transExpr(exp->fields.unoexp.child);
        dest = newTemp();
        if (exp->fields.unoexp.op == NOTexp)
        {
            emit(OP_NOT, dest, t1, empty);
        }
        else
        {
            emit(OP_NEG, dest, t1, empty);
        }
        return dest;
    case OPEXP:
        t1 = transExpr(exp->fields.opexp.left);
        t2 = transExpr(exp->fields.opexp.right);
        dest = newTemp();
        Opcode op;
        switch (exp->fields.opexp.op)
        {
        case SUM:
            op = OP_ADD;
            break;
        case SUB:
            op = OP_SUB;
            break;
        case TIMES:
            op = OP_MUL;
            break;
        case DIVISION:
            op = OP_DIV;
            break;
        case MODULUS:
            op = OP_MOD;
            break;
        case REMAINDER:
            op = OP_REM;
            break;
        case POW:
            op = OP_POW;
            break;
        case EQUAL:
            op = OP_EQ;
            break;
        case INEQUAL:
            op = OP_NEQ;
            break;
        case LESSexp:
            op = OP_LT;
            break;
        case GREATERexp:
            op = OP_GT;
            break;
        case LEQexp:
            op = OP_LEQ;
            break;
        case GEQexp:
            op = OP_GEQ;
            break;
        case ORexp:
            op = OP_OR;
            break;
        case ANDexp:
            op = OP_AND;
            break;
        case XORexp:
            op = OP_XOR;
            break;

        default:
            op = OP_ADD;
            break; // default case
        }
        emit(op, dest, t1, t2);
        return dest;
    case PAREXP:
        return transExpr(exp->fields.parexp.inner);
    default:
        fprintf(stderr, "Unknown expression type\n");
        return empty;
    }
}

void transCond(Exp exp, Op labelT, Op labelF)
{
    Op t1, t2;
    Op empty = {0};
    if (exp->exp_t == OPEXP)
    {
        t1 = transExpr(exp->fields.opexp.left);
        t2 = transExpr(exp->fields.opexp.right);
        Op dest = newTemp();
        Opcode op;
        switch (exp->fields.opexp.op)
        {
        case EQUAL:
            op = OP_EQ;
            break;
        case INEQUAL:
            op = OP_NEQ;
            break;
        case LESSexp:
            op = OP_LT;
            break;
        case GREATERexp:
            op = OP_GT;
            break;
        case LEQexp:
            op = OP_LEQ;
            break;
        case GEQexp:
            op = OP_GEQ;
            break;
        case ANDexp:
        case ORexp:
        case XORexp:
            op = (exp->fields.opexp.op == ANDexp) ? OP_AND : (exp->fields.opexp.op == ORexp) ? OP_OR
                                                                                             : OP_XOR;
            break;
        default:
            op = OP_NEQ;
            break;
        }
        emit(op, dest, t1, t2);
        emit(OP_JUMP_FALSE, labelF, dest, empty);
        emit(OP_JUMP, labelT, empty, empty);
        return;
    }
    if (exp->exp_t == UNOEXP && exp->fields.unoexp.op == NOTexp)
    {
        transCond(exp->fields.unoexp.child, labelF, labelT);
        return;
    }
    Op condVal = transExpr(exp);
    Op zero = newConstInt(0);
    Op cmp = newTemp();
    emit(OP_NEQ, cmp, condVal, zero);
    emit(OP_JUMP_FALSE, labelF, cmp, empty);
    emit(OP_JUMP, labelT, empty, empty);
}

void transStm(Stm s)
{
    Op empty = {0};
    if (!s)
        return;

    // fprintf(stderr, "Translating statement of type %d\n", s->stm_t);
    switch (s->stm_t)
    {
    case ASSIGNSTM:
    {
        Op right = transExpr(s->fields.assign.exp);
        Op left = getVarTemp(s->fields.assign.ident);
        emit(OP_MOVE, left, right, empty);
        break;
    }
    case COMPOUNDSTM:
        transStm(s->fields.compound.fst);
        transStm(s->fields.compound.snd);
        break;
    case IFSTM:
    {
        Op thenLabel = newLabel();
        Op elseLabel = newLabel();
        Op endLabel = newLabel();
        Op empty = (Op){0};
        transCond(s->fields.ifstm.cond, thenLabel, elseLabel);
        emit(OP_LABEL, thenLabel, empty, empty);
        transStm(s->fields.ifstm.then_branch);
        emit(OP_JUMP, endLabel, empty, empty);

        emit(OP_LABEL, elseLabel, empty, empty);
        if (s->fields.ifstm.else_branch)
        {
            transStm(s->fields.ifstm.else_branch);
        }
        emit(OP_LABEL, endLabel, empty, empty);
        break;
    }
    case WHILESTM:
    {
        Op startLabel = newLabel();
        Op bodyLabel = newLabel();
        Op endLabel = newLabel();
        Op empty = (Op){0};
        emit(OP_LABEL, startLabel, empty, empty);
        transCond(s->fields.whilestm.cond, bodyLabel, endLabel);
        emit(OP_LABEL, bodyLabel, empty, empty);
        transStm(s->fields.whilestm.branch);
        emit(OP_JUMP, startLabel, empty, empty);
        emit(OP_LABEL, endLabel, empty, empty);
        break;
    }
    case PUTSTM:
    {
        Op val = transExpr(s->fields.putstm.output);
        emit(OP_PRINT, val, empty, empty);
        break;
    }
    case GETSTM:
    {
        Op dest = getVarTemp(s->fields.getstm.ident);
        emit(OP_READ, dest, empty, empty);
        break;
    }
    case PROCSTM:
        transStm(s->fields.proc.statements);
        break;
    default:
        break;
    }
}

void printOp(Op op)
{
    switch (op.kind)
    {
    case OP_TEMP:
        printf("t%d ", op.contents.temp_id);
        break;
    case OP_VAR:
        printf("%s ", op.contents.name);
        break;
    case OP_CONST_INT:
        printf("%d ", op.contents.ival);
        break;
    case OP_CONST_FLOAT:
        printf("%g ", op.contents.fval);
        break;
    case OP_CONST_STRING:
        printf("\"%s\"", op.contents.sval);
        break;
    default:
        printf("unknown_op");
    }
}

/* static const char *getOpCodeString(Opcode op)
{
    switch (op)
    {
    case OP_ADD:
        return "ADD";
    case OP_SUB:
        return "SUB";
    case OP_MUL:
        return "MUL";
    case OP_DIV:
        return "DIV";
    case OP_MOD:
        return "MOD";
    case OP_REM:
        return "REM";
    case OP_POW:
        return "POW";
    case OP_AND:
        return "AND";
    case OP_OR:
        return "OR";
    case OP_NOT:
        return "NOT";
    case OP_XOR:
        return "XOR";
    case OP_NEG:
        return "NEG";
    case OP_MOVE:
        return "MOVE";
    case OP_LOAD:
        return "LOAD";
    case OP_LABEL:
        return "LABEL";
    case OP_JUMP:
        return "JUMP";
    case OP_JUMP_FALSE:
        return "JUMP_FALSE";
    case OP_PRINT:
        return "PRINT";
    case OP_READ:
        return "READ";
    case OP_EQ:
        return "EQ";
    case OP_NEQ:
        return "NEQ";
    case OP_LT:
        return "LT";
    case OP_GT:
        return "GT";
    case OP_LEQ:
        return "LEQ";
    case OP_GEQ:
        return "GEQ";
    default:
        return "UNKNOWN_OPCODE";
    }
} */

static const char *arith_symbol(Opcode op)
{
    switch (op)
    {
    case OP_ADD:
        return "+";
    case OP_SUB:
        return "-";
    case OP_MUL:
        return "*";
    case OP_DIV:
        return "/";
    case OP_MOD:
        return "%";
    case OP_REM:
        return "rem";
    case OP_POW:
        return "^";
    case OP_AND:
        return "and";
    case OP_OR:
        return "or";
    case OP_XOR:
        return "xor";

    default:
        return "NULL";
    }
}

static const char *rel_symbol(Opcode op)
{
    switch (op)
    {
    case OP_EQ:
        return "==";
    case OP_NEQ:
        return "!=";
    case OP_LT:
        return "<";
    case OP_GT:
        return ">";
    case OP_LEQ:
        return "<=";
    case OP_GEQ:
        return ">=";
    default:
        return "NULL";
    }
}

void printTAC(Instr *head)
{
    Instr *curr = head;
    printf("\nThree Address Code:\n");

    while (curr)
    {
        // COND: relational + JUMP_FALSE -> COND left rel_op right true_label false_label
        if ((curr->opcode == OP_EQ || curr->opcode == OP_NEQ ||
             curr->opcode == OP_LT || curr->opcode == OP_GT ||
             curr->opcode == OP_LEQ || curr->opcode == OP_GEQ) &&
            curr->next && curr->next->opcode == OP_JUMP_FALSE)
        {
            Instr *rel = curr;
            Instr *jf = rel->next;
            int match_temp = 0;
            if (jf->arg1.kind == OP_TEMP && rel->arg3.kind == OP_TEMP &&
                jf->arg1.contents.temp_id == rel->arg3.contents.temp_id)
            {
                match_temp = 1;
            }
            if (match_temp)
            {
                const char *else_label = (jf->arg3.kind == OP_VAR && jf->arg3.contents.name) ? jf->arg3.contents.name : "(no_false)";
                const char *true_label = NULL;
                if (jf->next && jf->next->opcode == OP_JUMP && jf->next->arg3.kind == OP_VAR)
                {
                    true_label = jf->next->arg3.contents.name;
                    printf("\tCOND ");
                    printOp(rel->arg1);
                    printf(" %s ", rel_symbol(rel->opcode));
                    printOp(rel->arg2);
                    printf("%s %s\n", true_label, else_label);
                    curr = jf->next->next;
                    continue;
                }
                if(jf->next && jf->next->opcode == OP_LABEL && jf->next->arg3.kind == OP_VAR)
                {
                    true_label = jf->next->arg3.contents.name;
                    printf("\tCOND ");
                    printOp(rel->arg1);
                    printf(" %s ", rel_symbol(rel->opcode));
                    printOp(rel->arg2);
                    printf("%s %s\n", true_label, else_label);
                    curr = jf->next;
                    continue;
                }
                printf("\tCOND ");
                printOp(rel->arg1);
                printf(" %s ", rel_symbol(rel->opcode));
                printOp(rel->arg2);
                printf("(true) %s\n", else_label);
                curr = jf->next;
                continue;
            }
        }
        // LABEL:
        if (curr->opcode == OP_LABEL)
        {
            if (curr->arg3.kind == OP_VAR && curr->arg3.contents.name)
            {
                printf("%s:\n", curr->arg3.contents.name);
            }
            else
            {
                printf("unknown_label");
            }
            curr = curr->next;
            continue;
        }
        // JUMP:
        if (curr->opcode == OP_JUMP)
        {
            printf("\tJUMP ");
            printOp(curr->arg3);
            printf("\n");
            curr = curr->next;
            continue;
        }
        // JUMP FALSE:
        if (curr->opcode == OP_JUMP_FALSE)
        {
            printf("\tJUMP_FALSE ");
            printOp(curr->arg3);
            printf(" ");
            printOp(curr->arg1);
            printf("\n");
            curr = curr->next;
            continue;
        }

        if (curr->opcode == OP_MOVE)
        {
            printf("\t");
            printOp(curr->arg3);
            printf(":= ");
            printOp(curr->arg1);
            printf("\n");
            curr = curr->next;
            continue;
        }
        else if (strcmp(arith_symbol(curr->opcode), "NULL") != 0)
        {
            printf("\t");
            printOp(curr->arg3);
            printf(" := ");
            printOp(curr->arg1);
            printf(" %s ", arith_symbol(curr->opcode));
            printOp(curr->arg2);
            printf("\n");
            curr = curr->next;
            continue;
        }
        else if (strcmp(rel_symbol(curr->opcode), "NULL") != 0)
        {
            printf("\t");
            printOp(curr->arg3);
            printf(" := ");
            printOp(curr->arg1);
            printf(" %s ", rel_symbol(curr->opcode));
            printOp(curr->arg2);
            printf("\n");
            curr = curr->next;
            continue;
        }
        else if (curr->opcode == OP_PRINT)
        {
            printf("\tPUT ");
            printOp(curr->arg3);
            printf("\n");
            curr = curr->next;
            continue;
        }
        else if( curr->opcode == OP_READ)
        {
            printf("\tGET ");
            printOp(curr->arg3);
            printf("\n");
            curr = curr->next;
            continue;
        }

        else if (curr->opcode == OP_NOT || curr->opcode == OP_NEG)
        {
            printf("\t");
            printOp(curr->arg3);
            printf(" := ");
            printOp(curr->arg1);
            printf("\n");
            curr = curr->next;
            continue;
        }

        else
        {
            printOp(curr->arg3);
            printf(" := ");
            if (curr->arg1.kind != 0)
                printOp(curr->arg1);
            printf(" , ");
            if (curr->arg2.kind != 0)
                printf(", ");
            printOp(curr->arg2);
        }
        printf("\n");

        curr = curr->next;
    }

    printf("\n");
}
static void freeOpIfNeeded(Op *op)
{
    if (!op)
        return;
    if (op->kind == OP_VAR && op->contents.name)
    {
        free(op->contents.name);
        op->contents.name = NULL;
    }
}

void freeInstructions(Instr *head)
{
    Instr *curr = head;
    while (curr)
    {
        Instr *next = curr->next;
        freeOpIfNeeded(&curr->arg1);
        freeOpIfNeeded(&curr->arg2);
        freeOpIfNeeded(&curr->arg3);
        free(curr);
        curr = next;
    }
    freeVarTemps();
}

void allocate_var_temps_from_table(Table tbl)
{
    if (!tbl)
        return;
    int count = 0;
    Entry *e = tbl;
    while (e)
    {
        count++;
        e = e->next;
    }
    Entry *arr = NULL;
    if (count > 0)
    {
        arr = (Entry *)malloc(sizeof(Entry) * count);
        e = tbl;
        int i = 0;
        while (e)
        {
            arr[i++] = *e;
            e = e->next;
        }
        for (int j = count - 1; j >= 0; --j)
        {
            Entry *entry = &arr[j];
            if (entry && entry->value && entry->value->kind == VAR)
            {
                const char *orig = entry->value->name ? entry->value->name : entry->key;
                getVarTemp((char *)orig);
            }
        }
        free(arr);
    }
}

void emit_var_prologue(void)
{
    Op empty = {0};
    for (VarTemp *p = var_temp_head; p != NULL; p = p->next)
    {
        Op varOp = newVar(p->name);
        emit(OP_MOVE, p->temp, varOp, empty);
    }
}

void printVarTemps(void)
{
    printf("\nVariable Temps:\n");
    if( var_temp_head == NULL){
        printf("(none)\n");
        return;
    }
    for (VarTemp *p = var_temp_head; p != NULL; p = p->next)
    {
        printf("Variable %s mapped to Temp t%d\n", p->name, p->temp.contents.temp_id);
    }
    printf("\n");
}