/*
    Módulo principal do analisador sintático para a linguagem Ada.
  Este módulo apenas lê uma linha de input e invoca
  o analisador lexical e sintático em sequência.

    o lexer.x, após compilação, gera o arquivo lex.yy.c (comando para compilar: flex lexer.x)
    o parser.y, após compilação, gera o arquivo parser.tab.c e parser.tab.h (comando para compilar: bison -d parser.y)
    o arquivo main.c (este arquivo) deve ser compilado junto com os arquivos gerados (comando para compilar tudo junto: gcc main.c lex.yy.c parser.tab.c -o meu_analisador -lfl)

*/

#include <stdio.h>
#include "ast.h"
#include "parser.tab.h" /* depois de compilar o parser.y */
extern FILE* yyin; // to read from files
int main(int argc, char** argv) {
  if(argc>1){
    yyin = fopen(argv[1], "r");
    if(!yyin) {perror("fopen"); return 1;}
  }
  if(yyparse()==0 && root){
    print_stm(root);
    free_stm(root);
  }
  if(yyin && yyin != stdin) fclose(yyin);
  return 0;
}
