/* Functions for building Abstract Syntax Trees (AST)
 */
#include "ast.h"
#include <stdlib.h>
#include <stdio.h>

/* Construct expressions
 */
/* make number */
Exp mk_numexp(int num)
{
  Exp ptr = malloc(sizeof(struct _Exp));
  ptr->exp_t = NUMEXP;
  ptr->fields.num = num;
  return ptr;
}
/* make id */
Exp mk_idexp(char *ident)
{
  Exp ptr = malloc(sizeof(struct _Exp));
  ptr->exp_t = IDEXP;
  ptr->fields.ident = ident;
  return ptr;
}
/* make string */
Exp mk_strexp(char* string)
{
  Exp ptr = malloc(sizeof(struct _Exp));
  ptr->exp_t = STREXP;
  ptr->fields.string = string;
  return ptr;
}
/* make boolean */
Exp mk_boolexp(int bool)
{
  Exp ptr = malloc(sizeof(struct _Exp));
  ptr->exp_t = BOOLEXP;
  ptr->fields.boolVal = bool;
  return ptr;
}
/* make operation */
Exp mk_opexp(Exp left, BinOp op, Exp right)
{
  Exp ptr = malloc(sizeof(struct _Exp));
  ptr->exp_t = OPEXP;
  ptr->fields.opexp.left = left;
  ptr->fields.opexp.right = right;
  ptr->fields.opexp.op = op;
  return ptr;
}
/* expression NOT and Unary Minus*/
Exp mk_unoexp(UnOp op, Exp child)
{
  Exp ptr = malloc(sizeof(struct _Exp));
  ptr->exp_t = UNOEXP;
  ptr->fields.unoexp.op = op;
  ptr->fields.unoexp.child = child;
  return ptr;
}

/* Construct statements
 */
Stm mk_assign(char *ident, Exp exp) 
{
  Stm ptr = malloc(sizeof(struct _Stm));
  ptr->stm_t = ASSIGNSTM;
  ptr->fields.assign.ident = ident;
  ptr->fields.assign.exp = exp;
  return ptr;
}


Stm mk_compound(Stm fst, Stm snd) 
{
  Stm ptr = malloc(sizeof(struct _Stm));
  ptr->stm_t = COMPOUNDSTM;
  ptr->fields.compound.fst = fst;
  ptr->fields.compound.snd = snd;
  return ptr;
}

Stm mk_if(Exp cond, Stm then_branch, Stm else_branch)
{
  Stm ptr = malloc(sizeof(struct _Stm));
  ptr->stm_t = IFSTM;
  ptr->fields.ifstm.cond = cond;
  ptr->fields.ifstm.then_branch = then_branch;
  ptr->fields.ifstm.else_branch = else_branch;
  return ptr;
}

Stm mk_while(Exp cond, Stm branch)
{
  Stm ptr = malloc(sizeof(struct _Stm));
  ptr->stm_t = WHILESTM;
  ptr->fields.whilestm.cond = cond;
  ptr->fields.whilestm.branch = branch;
  return ptr;
}

Stm put_line(Exp output)
{
  Stm ptr = malloc(sizeof(struct _Stm));
  ptr->stm_t = PUTSTM;
  ptr->fields.putstm.output = output;
  return ptr;
}

Stm get_line(char *ident)
{
  Stm ptr = malloc(sizeof(struct _Stm));
  ptr->stm_t = GETSTM;
  ptr->fields.getstm.ident = ident;
  return ptr;
}

Stm mk_proc(Stm statements)
{
  Stm ptr = malloc(sizeof(struct _Stm));
  ptr->stm_t = PROCSTM;
  ptr->fields.proc.statements = statements;
  return ptr;
}


/* Pretty-Print an expression
 */
void print_op(BinOp op) {
  switch(op) {
  case SUM:
    printf("+");
    break;
  case SUB:
    printf("-");
    break;
  case TIMES:
    printf("*");
    break;
  case DIVISION:
    printf("/");
    break;
  case EQUAL:
    printf("=");
    break;
  case INEQUAL:
    printf("/=");
    break;
  case ORexp:
    printf(" or ");
    break;
  case ANDexp:
    printf(" and ");
    break;
  case XORexp:
    printf(" xor ");
    break;
  case LESSexp:
    printf("<");
    break;
  case GREATERexp:
    printf(">");
    break;
  case LEQexp:
    printf("<=");
    break;
  case GEQexp:
    printf(">=");
    break;
  case MODULUS:
    printf(" mod ");
    break;
  case REMAINDER:
    printf(" rem ");
    break;
  default:
    fprintf(stderr, "print_op: unknown Op %d\n", op);
    break;
  }           
}

void print_unop(UnOp op){
  switch (op)
  {
  case NOTexp:
    printf(" not ");
    break;
  case UMINUS:
    printf("-");
    break;
  default:
    fprintf(stderr, "print_unop: unknown UnOp %d\n", op);
    break;
  }
}

void print_exp(Exp ptr) {
  if(ptr == NULL) return;
  switch (ptr->exp_t) {
  case NUMEXP:
    printf("%d", ptr->fields.num);
    break;
  case IDEXP:
    printf("%s", ptr->fields.ident);
    break;
  case OPEXP:
    printf("(");
    print_exp(ptr->fields.opexp.left);
    print_op(ptr->fields.opexp.op);
    print_exp(ptr->fields.opexp.right);
    printf(")");
    break;
  case STREXP:
    printf("%s", ptr->fields.string);
    break;
  case BOOLEXP:
    printf(ptr->fields.boolVal  ? "true" : "false");
    break;
  case UNOEXP:
    print_unop(ptr->fields.unoexp.op);
    print_exp(ptr->fields.unoexp.child);
    break;
  default:
    fprintf(stderr, "print_exp: unknown ExpType %d\n", ptr->exp_t);
    break;
  }
}

/* Pretty-print a statement
 */
void print_stm(Stm ptr) {
  if(ptr == NULL) return;
  switch(ptr->stm_t) {
  case ASSIGNSTM:
    printf("%s", ptr->fields.assign.ident);
    printf(" := ");
    print_exp(ptr->fields.assign.exp);
    printf("; ");
    break;
  case COMPOUNDSTM:
    print_stm(ptr->fields.compound.fst);
    print_stm(ptr->fields.compound.snd);
    break;
  case IFSTM:
    printf("if ");
    print_exp(ptr->fields.ifstm.cond);
    printf(" then ");
    print_stm(ptr->fields.ifstm.then_branch);
    if(ptr->fields.ifstm.else_branch != NULL){
      printf(" else ");
      print_stm(ptr->fields.ifstm.else_branch);
    }
    printf(" end if;");
    break;
  case WHILESTM:
    printf("while ");
    print_exp(ptr->fields.whilestm.cond);
    printf(" loop ");
    print_stm(ptr->fields.whilestm.branch);
    printf(" end loop;");
    break;
  case PUTSTM:
    printf("put_line(");
    print_exp(ptr->fields.putstm.output);
    printf(");");
    break;
  case GETSTM:
    printf("get_line(");
    printf("%s", ptr->fields.getstm.ident);
    printf(");");
    break;
  case PROCSTM:
    printf("Procedure Main is ");
    printf("begin ");
    print_stm(ptr->fields.proc.statements);
    printf(" end Main;");
    break;
  default:
    fprintf(stderr, "print_stm: unknown StmType %d\n", ptr->stm_t);
    break;
  }
}

