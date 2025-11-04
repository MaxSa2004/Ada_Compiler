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
Stm mk_assign(char *ident, Exp exp) {
  Stm ptr = malloc(sizeof(struct _Stm));
  ptr->stm_t = ASSIGNSTM;
  ptr->fields.assign.ident = ident;
  ptr->fields.assign.exp = exp;
  return ptr;
}


Stm mk_compound(Stm fst, Stm snd) {
  Stm ptr = malloc(sizeof(struct _Stm));
  ptr->stm_t = COMPOUNDSTM;
  ptr->fields.compound.fst = fst;
  ptr->fields.compound.snd = snd;
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
    printf("or");
    break;
  case ANDexp:
    printf("and");
    break;
  case XORexp:
    printf("xor");
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
    printf("mod");
    break;
  case REMAINDER:
    printf("rem");
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
    printf("not");
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
    if(ptr->fields.boolVal == 0){
      printf("false");
    } else {
      printf("true");
    }
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
  switch(ptr->stm_t) {
  case ASSIGNSTM:
    printf("%s", ptr->fields.assign.ident);
    printf("=");
    print_exp(ptr->fields.assign.exp);
    printf("; ");
    break;
  case COMPOUNDSTM:
    print_stm(ptr->fields.compound.fst);
    print_stm(ptr->fields.compound.snd);
    break;
  }
}

