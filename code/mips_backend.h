#ifndef MIPS_BACKEND_H
#define MIPS_BACKEND_H
#include <stdio.h>

#include "codeGenerator.h" // included to use Instr type defined in codeGenerator.h
#include "symbolTable.h" // included to use var offset defined in symbolTable.h
// gera um caminho de arquivo MIPS a partir da lista de instruções (0 se sucedido, !0 se falhou)
int generateMIPS( const InstrList *head, Table table, const char *outfile);
// gera código MIPS para lista de instruções e escreve no FILE *out
void printMIPS(const InstrList *list, Table table, FILE *out);
#endif
