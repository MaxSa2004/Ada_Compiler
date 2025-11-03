%{
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "ast.h"
int yylex (void);
void yyerror (char const *);
%}
%union {
   int i_val;
   char* s_val;
   Exp exp_node;
   Stm stm_node;
   BinOp op_type;
}

/* Tokens */
-- Expressions
%%token PLUS MINUS -- + -
%%token MULT DIV -- * /
%%token LPAREN RPAREN -- ()
%%token EQ INEQ -- = /=
%%token OR -- or
%%token AND  -- and
%%token XOR -- xor
%%token LESS GREATER -- < >
%%token LEQ GEQ -- <= >=
%%token MOD REM -- mod rem
%%token ASSIGN -- :=
-- Literals
%%token <s_val> ID
%%token TRUE FALSE -- true false
%%token STRING_LITERAL
%%token <i_val> NUM
-- IO
%%token PUT GET -- put_line get_line ?
-- Control Flow
%%token IF THEN ELSE -- if A then B else C
%%token WHILE LOOP END -- while A loop end
-- Basic Types
%%token INTEGER
%%token BOOLEAN
%%token STRING
-- Procedure
%%token MAIN
-- Precendence
%%left PLUS MINUS
%%left MULT DIV MOD REM

%type <exp_node> expr -- expression
%type <stm_node> stm -- statement
%%
-- Regras de produção 
top: expr {printf("%d\n", $1);}
;

expr:
   NUM
   | expr PLUS expr {$$ = $1 + $3;}
   | expr MINUS expr {$$ = $1 - $3;}
   | expr MULT expr {$$ = $1 * $3;}
   | expr DIV expr {$$ = $1 / $3;}
   | LPAREN expr RPAREN {$$ = $2;}
   | expr EQ expr {$$ = $1 == $3;}
   | expr INEQ expr {$$ = $1 != $3;}
   | expr OR expr {$$ = $1 || $3;}
   | expr AND expr {$$ = $1 && $3;}
   | expr XOR expr {$$ = $1 ^ $3;}
   | expr LESS expr {$$ = $1 < $3;}
   | expr GREATER expr {$$ = $1 > $3;}
   | expr LEQ expr {$$ = $1 <= $3;}
   | expr GEQ expr {$$ = $1 >= $3;}
   | expr MOD expr {$$ = ($1 % $3 + $3) + $3;}
   | expr REM expr {$$ = $1 % $3;}
   ;

stm: ID ASSIGN expr {$$ = mk_assign($1, $3);};



%%

-- Código C adicional (erro)
void yyerror(char const *msg) {
   printf("parse error: %s\n", msg);
   exit(-1);
}

