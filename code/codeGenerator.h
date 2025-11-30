#ifndef CODE_GENERATOR_H
#define CODE_GENERATOR_H
#include <stdint.h>
#include "ast.h"
#include "symbolTable.h"



typedef enum {
    OP_ADD, OP_SUB, OP_MUL, OP_DIV, OP_MOD, OP_REM, OP_POW,
    OP_MOVE, OP_LOAD, OP_LABEL, OP_JUMP, OP_JUMP_FALSE, OP_PRINT, OP_READ,
    OP_EQ, OP_NEQ, OP_LT, OP_GT, OP_LEQ, OP_GEQ, 
    OP_AND, OP_OR, OP_NOT, OP_XOR, OP_NEG
} Opcode;

typedef enum {
    OP_TEMP,
    OP_VAR,
    OP_CONST_INT,
    OP_CONST_FLOAT,
    OP_CONST_STRING
} op_kind;

typedef struct Op {
    op_kind kind;
    union {
        int temp_id;
        char *name;
        int ival;
        double fval;
        char *sval;
    } contents;
} Op;

typedef struct Instr {
    Opcode opcode; 
    Op arg1, arg2, arg3;
    struct Instr* next;
} Instr;

extern Instr *instr_head;
extern Instr *instr_tail;
void init_code_generator();
Op transExpr(Exp e);
void transStm(Stm s);
void transCond(Exp e, Op labelF, Op labelT);
void printTAC(Instr *head);
void freeInstructions(Instr *head);
void allocate_var_temps_from_table(Table tbl);
void emit_var_prologue(void);
void printVarTemps(void);

#endif