/* Functions for building Abstract Syntax Trees (AST)
 */
#include "ast.h"
#include <stdlib.h>
#include <stdio.h>

/* Construct expressions
 */
Exp mk_numexp(int num)
{
  Exp ptr = malloc(sizeof(struct _Exp));
  ptr->exp_t = NUMEXP;
  ptr->fields.num = num;
  return ptr;
}

Exp mk_idexp(char *ident)
{
  Exp ptr = malloc(sizeof(struct _Exp));
  ptr->exp_t = IDEXP;
  ptr->fields.ident = ident;
  return ptr;
}

Exp mk_opexp(Exp left, BinOp op, Exp right)
{
  Exp ptr = malloc(sizeof(struct _Exp));
  ptr->exp_t = OPEXP;
  ptr->fields.opexp.left = left;
  ptr->fields.opexp.right = right;
  ptr->fields.opexp.op = op;
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

Stm mk_incr(char *ident) {
  Stm ptr = malloc(sizeof(struct _Stm));
  ptr->stm_t = INCRSTM;
  ptr->fields.incr = ident;
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
  case PLUS:
    printf("+");
    break;
  case MINUS:
    printf("-");
    break;
  case TIMES:
    printf("*");
    break;
  case DIV:
    printf("/");
    break;
  }              
}

void print_exp(Exp ptr) {
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
  case INCRSTM:
    printf("%s++", ptr->fields.incr);
    printf("; ");
    break;
  case COMPOUNDSTM:
    print_stm(ptr->fields.compound.fst);
    print_stm(ptr->fields.compound.snd);
    break;
  }
}

