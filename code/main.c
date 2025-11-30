/*
    Módulo principal do analisador sintático para a linguagem Ada.
  Este módulo apenas lê uma linha de input e invoca
  o analisador lexical e sintático em sequência.

    o lexer.x, após compilação, gera o arquivo lex.yy.c (comando para compilar: flex lexer.x)
    o parser.y, após compilação, gera o arquivo parser.tab.c e parser.tab.h (comando para compilar: bison -d parser.y)
    o arquivo main.c (este arquivo) deve ser compilado junto com os arquivos gerados (comando para compilar tudo junto: gcc main.c lex.yy.c parser.tab.c -o meu_analisador -lfl)

*/

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "ast.h"
#include "parser.tab.h" /* depois de compilar o parser.y */
#include "codeGenerator.h"
#include "symbolTable.h"
extern FILE *yyin;      // to read from files

void printSymbolTable(Table t){
  printf("\n---Symbol Table ---\n");
  if(t==NULL){
    printf("Empty Symbol Table\n");
    return;
  } else {
    Entry *scan = t;
    while(scan != NULL){
      printf("Nome: %-15s | Kind: ", scan->key);
      if(scan->value){
        switch(scan->value->kind){
          case VAR: 
            printf("VAR");
            break;
          case CONST:
            printf("CONST");
            break;
          case TYPE: 
            printf("TYPE");
            break;
          case PROC:
            printf("PROC");
            break;
          default:
            printf("UNKNOWN");
            break;
        }
      }
      printf("\n");
      scan = scan->next;
    }
  }
  printf("------------------\n");
}
int main(int argc, char **argv)
{
  int debug = 0;
  const char* input = NULL;
  for(int i = 1; i<argc; ++i){
    if(strcmp(argv[i], "--debug")==0) debug = 1;
    else input = argv[i];
  }
  if (input)
  {
    yyin = fopen(input, "r");
    if (!yyin)
    {
      perror("fopen");
      return 1;
    }
  }
  if (yyparse() == 0 && root)
  {
    if (debug)
      debug_print(root);
    else
    {
      print_stm(root);
    }
    Table symTable = create();
    symTable = check_semantics(root, symTable);
    printSymbolTable(symTable);
    init_code_generator();
    allocate_var_temps_from_table(symTable);
    printVarTemps();
    emit_var_prologue();
    transStm(root);
    printTAC(instr_head);
    free_stm(root);
    free_table(symTable);

  }
  if (yyin && yyin != stdin)
    fclose(yyin);
  return 0;
}