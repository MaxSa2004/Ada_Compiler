/* Functions for building Abstract Syntax Trees (AST)
 */
#include "ast.h"
#include <stdlib.h>
#include <string.h>
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
/* make float*/
Exp mk_floatexp(float num){
  Exp ptr = malloc(sizeof(struct _Exp));
  ptr->exp_t = FLOATEXP;
  ptr->fields.fnum = num;
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

Exp mk_parexp(Exp inner){
  Exp ptr = malloc(sizeof(struct _Exp));
  ptr->exp_t = PAREXP;
  ptr->fields.parexp.inner = inner;
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

Stm mk_proc(char *name, Stm statements)
{
  Stm ptr = malloc(sizeof(struct _Stm));
  ptr->stm_t = PROCSTM;
  ptr->fields.proc.name = name ? strdup(name) : strdup("Main");
  ptr->fields.proc.statements = statements;
  return ptr;
}


/* Pretty-Print an expression
 */

static void print_ada_string(const char *s){
  putchar('"');
  for(; *s; ++s){
    if(*s == '"'){
      putchar('"');
      putchar('"');
    } else {
      putchar(*s);
    }
  }
  putchar('"');
}

void print_op(BinOp op) {
  switch(op) {
  case SUM:
    printf("+ ");
    break;
  case SUB:
    printf("- ");
    break;
  case TIMES:
    printf("* ");
    break;
  case DIVISION:
    printf("/ ");
    break;
  case POW:
    printf("** ");
    break;
  case EQUAL:
    printf("= ");
    break;
  case INEQUAL:
    printf("/= ");
    break;
  case ORexp:
    printf("OR ");
    break;
  case ANDexp:
    printf("AND ");
    break;
  case XORexp:
    printf("XOR ");
    break;
  case LESSexp:
    printf("< ");
    break;
  case GREATERexp:
    printf("> ");
    break;
  case LEQexp:
    printf("<= ");
    break;
  case GEQexp:
    printf(">= ");
    break;
  case MODULUS:
    printf("MOD ");
    break;
  case REMAINDER:
    printf("REM ");
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
    printf("NOT ");
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
    printf("%d ", ptr->fields.num);
    break;
  case FLOATEXP:
    printf("%g ", (double) ptr->fields.fnum);
    break;
  case IDEXP:
    printf("%s ", ptr->fields.ident);
    break;
  case OPEXP:
    print_exp(ptr->fields.opexp.left);
    print_op(ptr->fields.opexp.op);
    print_exp(ptr->fields.opexp.right);
    break;
  case PAREXP:
    printf("(");
    print_exp(ptr->fields.parexp.inner);
    printf(") ");
    break;
  case STREXP:
    print_ada_string(ptr->fields.string);
    break;
  case BOOLEXP:
    printf(ptr->fields.boolVal  ? "TRUE " : "FALSE ");
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
    printf("IF ");
    print_exp(ptr->fields.ifstm.cond);
    printf("THEN ");
    print_stm(ptr->fields.ifstm.then_branch);
    if(ptr->fields.ifstm.else_branch != NULL){
      printf("ELSE ");
      print_stm(ptr->fields.ifstm.else_branch);
    }
    printf("END IF; ");
    break;
  case WHILESTM:
    printf("WHILE ");
    print_exp(ptr->fields.whilestm.cond);
    printf("LOOP ");
    print_stm(ptr->fields.whilestm.branch);
    printf("END LOOP; ");
    break;
  case PUTSTM:
    printf("PUT_LINE(");
    print_exp(ptr->fields.putstm.output);
    printf("); ");
    break;
  case GETSTM:
    printf("GET_LINE(");
    printf("%s", ptr->fields.getstm.ident);
    printf("); ");
    break;
  case PROCSTM:
    printf("PROCEDURE %s ", ptr->fields.proc.name);
    printf("IS BEGIN ");
    print_stm(ptr->fields.proc.statements);
    printf("END %s;\n", ptr->fields.proc.name);
    break;
  default:
    fprintf(stderr, "print_stm: unknown StmType %d\n", ptr->stm_t);
    break;
  }
}

void free_exp(Exp ptr){
  if(!ptr) return;
  switch (ptr->exp_t) {
  case NUMEXP:
  case FLOATEXP:
  case BOOLEXP:
    break;
  case IDEXP:
    if(ptr->fields.ident) free(ptr->fields.ident);
    break;
  case OPEXP:
    free_exp(ptr->fields.opexp.left);
    free_exp(ptr->fields.opexp.right);
    break;
  case PAREXP:
    free_exp(ptr->fields.parexp.inner);
    break;
  case STREXP:
    if(ptr->fields.string) free(ptr->fields.string);
    break;
  case UNOEXP:
    free_exp(ptr->fields.unoexp.child);
    break;
  default:
    break;
  }
  free(ptr);
}

void free_stm(Stm ptr){
  if(ptr == NULL) return;
  switch(ptr->stm_t) {
  case ASSIGNSTM:
    if(ptr->fields.assign.ident) free(ptr->fields.assign.ident);
    free_exp(ptr->fields.assign.exp);
    break;
  case COMPOUNDSTM:
    free_stm(ptr->fields.compound.fst);
    free_stm(ptr->fields.compound.snd);
    break;
  case IFSTM:
    free_exp(ptr->fields.ifstm.cond);
    free_stm(ptr->fields.ifstm.then_branch);
    if(ptr->fields.ifstm.else_branch) free_stm(ptr->fields.ifstm.else_branch);
    break;
  case WHILESTM:
    free_exp(ptr->fields.whilestm.cond);
    free_stm(ptr->fields.whilestm.branch);
    break;
  case PUTSTM:
    free_exp(ptr->fields.putstm.output);
    break;
  case GETSTM:
    if(ptr->fields.getstm.ident) free(ptr->fields.getstm.ident);
    break;
  case PROCSTM:
    if(ptr->fields.proc.name) free(ptr->fields.proc.name);
    free_stm(ptr->fields.proc.statements);
    break;
  default:
    break;
  }
  free(ptr);
}