%{
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "ast.h"
int yylex (void);
void yyerror (char const *);
Stm root = NULL;
%}
%union { /* used in bison, defines type of yylval and the semantic values passed between rules */
   int i_val;
   float f_val;
   char* s_val; /* for both ID and STRING_LITERAL */
   Exp exp_node;
   Stm stm_node;
   BinOp op_type;
}

/* Tokens */
/* Expressions */
%token PLUS MINUS /* + - */
%token MULT DIV /* * / */
%token POWER /* ** */
%token LPAREN RPAREN /* () */
%token EQ INEQ /* = /= */
%token OR /* or */
%token AND  /* and */
%token XOR /* xor */
%token LESS GREATER /* < > */
%token LEQ GEQ /* <= >= */
%token MOD REM /* mod rem */
%token ASSIGN /* := */
%token NOT
/* Literals */
%token <s_val> ID
%token TRUE FALSE /* true false */
%token <s_val> STRING_LITERAL
%token <i_val> NUM
%token <f_val> FLOAT
/* IO */
%token PUT_LINE GET_LINE /* put_line get_line ? */
/* Control Flow */
%token IF THEN ELSE /* if A then B else C */
%token WHILE LOOP END /* while A loop end */
/* Basic Types */
%token INTEGER
%token BOOLEAN
%token STRING
%token SEMICOLON
/* Procedure */
%token MAIN
%token BEGIN_T
%token PROCEDURE
%token IS
/* Precendence */
%left OR XOR
%left AND
%nonassoc EQ INEQ LESS GREATER LEQ GEQ
%left PLUS MINUS
%left MULT DIV MOD REM
%right POWER
%right NOT 
%right UNARY_MINUS

%start top
%type <stm_node> top
%type <exp_node> expr /* expression */
%type <stm_node> stm /* statement */
%type <stm_node> proc /* procedure */
%type <stm_node> stm_list /* stm_list */
%%
/* Regras de produção  */
top: proc {root = $1;} /* return AST root -> Main Procedure */
;

expr:
   NUM {$$ = mk_numexp($1);}
   | FLOAT {$$ = mk_floatexp($1);}
   | ID {$$ = mk_idexp($1);}
   | TRUE {$$ = mk_boolexp(1);}
   | FALSE {$$ = mk_boolexp(0);}
   | STRING_LITERAL {$$ = mk_strexp($1);}
   | expr PLUS expr {$$ = mk_opexp($1, SUM, $3);}
   | expr MINUS expr {$$ = mk_opexp($1, SUB, $3);}
   | expr MULT expr {$$ = mk_opexp($1, TIMES, $3);}
   | expr POWER expr {$$ = mk_opexp($1, POW, $3);}
   | expr DIV expr {$$ = mk_opexp($1, DIVISION, $3);}
   | LPAREN expr RPAREN {$$ = $2;}
   | expr EQ expr {$$ = mk_opexp($1, EQUAL, $3);}
   | expr INEQ expr {$$ = mk_opexp($1, INEQUAL, $3);}
   | expr OR expr {$$ = mk_opexp($1, ORexp, $3);}
   | expr AND expr {$$ = mk_opexp($1, ANDexp, $3);}
   | expr XOR expr {$$ = mk_opexp($1, XORexp, $3);}
   | expr LESS expr {$$ = mk_opexp($1, LESSexp, $3);}
   | expr GREATER expr {$$ = mk_opexp($1, GREATERexp, $3);}
   | expr LEQ expr {$$ = mk_opexp($1, LEQexp, $3);}
   | expr GEQ expr {$$ = mk_opexp($1, GEQexp, $3);}
   | expr MOD expr {$$ = mk_opexp($1, MODULUS, $3);}
   | expr REM expr {$$ = mk_opexp($1, REMAINDER, $3);}
   | MINUS expr %prec UNARY_MINUS {$$ = mk_unoexp(UMINUS, $2);}
   | NOT expr {$$ = mk_unoexp(NOTexp, $2);}
   ;

proc: PROCEDURE MAIN IS BEGIN_T stm_list END MAIN SEMICOLON {$$ = mk_proc($5);};

stm_list: 
   stm_list stm { $$ = ($1 ? mk_compound($1, $2) : $2);}
   |/* empty */ {$$ = NULL;}
   ;

stm: ID ASSIGN expr SEMICOLON {$$ = mk_assign($1, $3);}
   | IF expr THEN stm_list ELSE stm_list END IF SEMICOLON {$$ = ($6 ? mk_if($2, $4, $6) : mk_if($2, $4, NULL));}
   | WHILE expr LOOP stm_list END LOOP SEMICOLON {$$ = mk_while($2, $4);}
   | PUT_LINE expr SEMICOLON {$$ = put_line($2);}
   | GET_LINE ID SEMICOLON {$$ = get_line($2);}
   ;
%%

/* Código C adicional (erro) */
void yyerror(char const *msg) {
   printf("parse error: %s\n", msg);
   exit(-1);
}

