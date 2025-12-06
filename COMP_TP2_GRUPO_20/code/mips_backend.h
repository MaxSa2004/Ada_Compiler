#ifndef MIPS_BACKEND_H
#define MIPS_BACKEND_H
#include <stdio.h>

#include "codeGenerator.h" // included to use Instr type defined in codeGenerator.h
#include "symbolTable.h"   // included to use var offset defined in symbolTable.h

// creates a MIPS file using printMIPS function and list of instructions TAC
int generateMIPS(const InstrList *head, Table table, const char *outfile);
// generates MIPS code for the list of instructions and writes to file out
void printMIPS(const InstrList *list, Table table, FILE *out);

#endif
