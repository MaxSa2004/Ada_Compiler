/*
    Módulo principal do analisador sintático para a linguagem Ada.
  Este módulo apenas lê uma linha de input e invoca
  o analisador lexical e sintático em sequência.

    o lexer.x, após compilação, gera o arquivo lex.yy.c (comando para compilar: flex lexer.x)
    o parser.y, após compilação, gera o arquivo parser.tab.c e parser.tab.h (comando para compilar: bison -d parser.y)
    o arquivo main.c (este arquivo) deve ser compilado junto com os arquivos gerados (comando para compilar tudo junto: gcc main.c lex.yy.c parser.tab.c -o meu_analisador -lfl)

*/

#include <stdio.h>
#include "parser.tab.h" /* depois de compilar o parser.y */

int main(void) {
  yyparse();
}
