#include <stdint.h>

typedef struct {
    Opcode opcode; 
    Addr arg1, arg2, arg3;
} Instr;

typedef enum {MOVE, MOVEI, OP, OPI, LABEL, JUMP, COND} Opcode;

typedef intptr_t Addr;



void emit2(Opcode opc, Addr arg1, Addr arg2);
void emit3(Opcode opc, Addr arg1, Addr arg2, Addr arg3);