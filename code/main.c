/*
    Módulo principal do analisador sintático para a linguagem Ada.
    Este módulo apenas lê uma linha de input e invoca
    o analisador lexical e sintático em sequência.

    Compilação dos componentes:
      flex lexer.x            -> lex.yy.c
      bison -d parser.y       -> parser.tab.c / parser.tab.h
      gcc main.c lex.yy.c parser.tab.c -o meu_analisador -lfl

*/

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>
#include <sys/stat.h>
#include <sys/types.h>

#include "ast.h"
#include "parser.tab.h"
#include "codeGenerator.h"
#include "symbolTable.h"
#include "mips_backend.h"

extern FILE *yyin; // to read from files

// extracts base filename from a path (no directories)
static const char *basename_only(const char *path)
{
  if (!path)
    return NULL;
  const char *slash = strrchr(path, '/');
#ifdef _WIN32
  const char *bslash = strrchr(path, '\\');
  if (!slash || (bslash && bslash > slash))
    slash = bslash;
#endif
  return slash ? slash + 1 : path;
}

/* removes the final extension (last '.'), returning a newly allocated string.
   if no dot found, returns a duplicate of the input. */
static char *strip_extension(const char *filename)
{
  if (!filename)
    return NULL;
  char *copy = strdup(filename);
  if (!copy)
    return NULL;
  char *dot = strrchr(copy, '.');
  if (dot)
    *dot = '\0';
  return copy;
}

// build output path test_outputs/<base>MIPS.s
/* static char *build_output_path(const char *inputPath)
{
  const char *base = basename_only(inputPath);
  char *stem = strip_extension(base ? base : "stdin");
  if (!stem)
    return NULL;

  // ensure directory exists
  if (mkdir("test_outputs", 0777) == -1)
  {
    if (errno != EEXIST)
    {
      fprintf(stderr, "Warning: could not create test_outputs directory (%s)\n", strerror(errno));
    }
  }

  size_t needed = strlen("test_outputs/") + strlen(stem) + strlen("MIPS.s") + 1;
  char *outPath = (char *)malloc(needed);
  if (!outPath)
  {
    free(stem);
    return NULL;
  }
  snprintf(outPath, needed, "test_outputs/%sMIPS.s", stem);
  free(stem);
  return outPath;
} */

int main(int argc, char **argv)
{
  int debug = 0;
  const char *input = NULL;

  for (int i = 1; i < argc; ++i)
  {
    if (strcmp(argv[i], "--debug") == 0)
      debug = 1;
    else
      input = argv[i];
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
    {
      debug_print(root);
    }
    else
    {
      print_stm(root);
    }

    Table symTable = create();
    symTable = check_semantics(root, symTable);
    printSymbolTable(symTable);

    init_code_generator();
    /* allocate_var_temps_from_table(symTable);
    printVarTemps();
    emit_var_prologue(); */
    transStm(root);



    /* char *outPath = build_output_path(input);
    if (!outPath)
    {
      fprintf(stderr, "Error: could not build output path. Using default 'outputMIPS.s'\n");
      generateMIPS(instr_head, "outputMIPS.s");
    }
    else
    {
      generateMIPS(instr_head, outPath);
      printf("MIPS code written to: %s\n", outPath);
      free(outPath);
    } */

    free_stm(root);
    free_table(symTable);
  }

  if (yyin && yyin != stdin)
    fclose(yyin);

  return 0;
}