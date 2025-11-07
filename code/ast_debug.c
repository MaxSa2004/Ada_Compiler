#include <stdio.h>
#include <string.h>
#include "ast.h"

static void pad(int n)
{
    while (n-- > 0)
        putchar(' ');
}

static void print_debug_string(const char *s)
{
    putchar('"');
    for (const char *p = s; p && *p; ++p)
    {
        if (*p == '\\')
        {
            putchar('\\');
            putchar('\\');
        }
        else if (*p == '"')
        {
            putchar('\\');
            putchar('"');
        }
        else if (*p == '\n')
        {
            putchar('\\');
            putchar('n');
        }
        else if (*p == '\t')
        {
            putchar('\\');
            putchar('t');
        }
        else
        {
            putchar(*p);
        }
    }
    putchar('"');
}

static const char *binop_name(BinOp op)
{
    switch (op)
    {
    case SUM:
        return "PLUS";
    case SUB:
        return "MINUS";
    case TIMES:
        return "MULT";
    case DIVISION:
        return "DIV";
    case POW:
        return "POWER";
    case EQUAL:
        return "EQ";
    case INEQUAL:
        return "NEQ";
    case ORexp:
        return "OR";
    case ANDexp:
        return "AND";
    case XORexp:
        return "XOR";
    case LESSexp:
        return "LT";
    case GREATERexp:
        return "GT";
    case LEQexp:
        return "LEQ";
    case GEQexp:
        return "GEQ";
    case MODULUS:
        return "MOD";
    case REMAINDER:
        return "REM";
    default:
        return "?";
    }
}

static const char *unop_name(UnOp op)
{
    switch (op)
    {
    case NOTexp:
        return "NOT";
    case UMINUS:
        return "NEG";
    default:
        return "?";
    }
}

void debug_print_exp(Exp ptr)
{
    if (!ptr){
        printf("NULL");
        return;
    }
    switch (ptr->exp_t)
    {
    case NUMEXP:
        printf("NUM(%d)", ptr->fields.num);
        break;
    case FLOATEXP:
        printf("FLOAT(%g ", (double)ptr->fields.fnum);
        break;
    case IDEXP:
        printf("ID(%s)", ptr->fields.ident);
        break;
    case OPEXP:
        printf("OP(%s, ", binop_name(ptr->fields.opexp.op));
        debug_print_exp(ptr->fields.opexp.left);
        printf(", ");
        debug_print_exp(ptr->fields.opexp.right);
        printf(")");
        break;
    case STREXP:
        printf("STRING(");
        print_debug_string(ptr->fields.string);
        printf(")");
        break;
    case BOOLEXP:
        printf("BOOL(%s)", ptr->fields.boolVal ? "TRUE" : "FALSE");
        break;
    case UNOEXP:
        printf("UNARY(%s, ", unop_name(ptr->fields.unoexp.op));
        debug_print_exp(ptr->fields.unoexp.child);
        printf(")");
        break;
    default:
        printf("?");
        break;
    }
}

static void debug_print_stm_list(Stm ptr, int indent);

void debug_print_stm(Stm ptr, int indent)
{
    if (!ptr)
    {
        pad(indent);
        printf("(none)\n");
        return;
    }
    if (ptr == NULL)
        return;
    switch (ptr->stm_t)
    {
    case ASSIGNSTM:
    pad(indent);
        printf("ASSIGN(%s, ", ptr->fields.assign.ident);
        debug_print_exp(ptr->fields.assign.exp);
        printf(")\n");
        break;
    case COMPOUNDSTM: 
        debug_print_stm_list(ptr, indent);
        break;
    case IFSTM:
        pad(indent);
        printf("IF\n");
        pad(indent + 2);
        printf("cond: ");
        debug_print_exp(ptr->fields.ifstm.cond);
        printf("\n");
        pad(indent+2);
        printf("then:\n");
        debug_print_stm_list(ptr->fields.ifstm.then_branch, indent+4);
        pad(indent+2);
        printf("else:\n");
        if(ptr->fields.ifstm.else_branch) debug_print_stm_list(ptr->fields.ifstm.else_branch, indent+4);
        else{
            pad(indent+4);
            printf("(none)\n");
        }
        break;
    case WHILESTM:
        pad(indent);
        printf("WHILE\n");
        pad(indent+2);
        printf("cond: ");
        debug_print_exp(ptr->fields.whilestm.cond);
        printf("\n");
        pad(indent+2);
        printf("body: ");
        debug_print_stm_list(ptr->fields.whilestm.branch, indent+4);
        break;
    case PUTSTM:
        pad(indent);
        printf("PUT_LINE(");
        debug_print_exp(ptr->fields.putstm.output);
        printf(")\n");
        break;
    case GETSTM:
    pad(indent);
        printf("GET_LINE(%s)\n", ptr->fields.getstm.ident);
        break;
    case PROCSTM:
        pad(indent);
        printf("PROCEDURE(%s)\n", ptr->fields.proc.name);
        pad(indent+2);
        printf("body:\n");
        debug_print_stm_list(ptr->fields.proc.statements, indent+4);
        break;
    default:
        pad(indent);
        printf("STM(?)\n");
        break;
    }
}

static void debug_print_stm_list(Stm ptr, int indent){
    if(!ptr) {
        pad(indent);
        printf("(empty)\n");
        return;
    }
    if(ptr->stm_t==COMPOUNDSTM){
        debug_print_stm_list(ptr->fields.compound.fst, indent);
        debug_print_stm_list(ptr->fields.compound.snd, indent);
    } else {
        debug_print_stm(ptr, indent);
    }
}

void debug_print(Stm root){
    debug_print_stm(root, 0);
}
