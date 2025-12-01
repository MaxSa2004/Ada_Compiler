#ifndef MIPS_BACKEND_H
#define MIPS_BACKEND_H

#include "codeGenerator.h" // included to use Instr type defined in codeGenerator.h

void generateMIPS(Instr *head, const char *filename);

#endif
