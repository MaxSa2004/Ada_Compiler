#ifndef CODE_GENERATOR_H
#define CODE_GENERATOR_H
#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>
#include "ast.h"
#include "symbolTable.h"

// Three Address Code Instruction types
typedef enum {
    OP_LABEL,       // LABEL <dest>
    OP_JUMP,        // JUMP <dest>
    OP_JUMP_FALSE,  // JUMP_FALSE <cond> <dest>
    OP_PRINT,       // PUT <arg1>
    OP_READ,        // GET <dest>
    OP_ASSIGN,      // dest := arg1 [bin_op arg2]
    OP_COND         // COND arg1 relop arg2 labelT labelF (pseudo-instr para impressão)
} instrop;

// Arithmetic and Logical Operations (binários para atribuições)
typedef enum {
    BINOP_ADD,
    BINOP_SUB,
    BINOP_MUL,
    BINOP_DIV,
    BINOP_MOD,
    BINOP_REM,
    BINOP_POW,
    BINOP_AND,
    BINOP_OR,
    BINOP_XOR
} binop;

// Unary Operations (se necessário para avaliação de expr)
typedef enum {
    UNOP_NEG,
    UNOP_NOT
} unop;

// Comparison Operations
typedef enum {
    RELOP_EQ,
    RELOP_NEQ,
    RELOP_LT,
    RELOP_GT,
    RELOP_LEQ,
    RELOP_GEQ
} relop;

// Types of Atoms/Operands
typedef enum {
    ATOM_VAR,
    ATOM_NUMBER,
    ATOM_FLOAT,
    ATOM_STRING,
    ATOM_BOOLEAN,
    ATOM_TEMP
} atom_kind;

// Characterizing an operand in TAC
typedef struct Atom {
    atom_kind kind;
    union {
        int temp_id;   // ATOM_TEMP
        char *name;    // ATOM_VAR
        int ival;      // ATOM_NUMBER/ATOM_BOOLEAN
        double fval;   // ATOM_FLOAT
        char *sval;    // ATOM_STRING
    } contents;
} Atom;

// Three Address Code Instruction
typedef struct Instr {
    instrop iop;       // tipo da instrução

    // Campos comuns
    char* dest;        // destino (var/label) conforme iop

    // Atribuição
    Atom arg1;
    Atom arg2;
    bool has_binop;
    binop bop;      // operador binário para OP_ASSIGN

    // Salto condicional
    Atom cond;         // condição (temp/boolean) para OP_JUMP_FALSE

    // Pseudo-instrução COND (para impressão compacta ao estilo do prof)
    relop rop;      // operador relacional
    Atom rel_left;     // lhs da comparação
    Atom rel_right;    // rhs da comparação
    char *label_true;  // destino quando verdadeiro
    char *label_false; // destino quando falso
} Instr;

typedef struct InstrNode {
    struct Instr *instr;
    struct InstrNode *next;
} InstrNode;

typedef struct InstrList {
    InstrNode *head;
    InstrNode *tail;
    size_t size;
} InstrList;

typedef struct CodeGenState {
    InstrList *instructions;
    int temp_count;
    int label_count;
} CodeGenState;

// initialization
void init_code_generator(void);
void free_code_generator(void);

// atom constructors
Atom atom_var(const char *name);
Atom atom_number(int value);
Atom atom_float(double value);
Atom atom_string(const char *value);
Atom atom_boolean(int value); // 0 = false, 1 = true
Atom atom_temp(int temp_id);

// temp management
Atom new_temp(void);

// instruction constructors (emitters)
Instr *emit2(const char *dest, Atom src); // dest := src
Instr *emit3_binop(const char *dest, Atom arg1, binop op, Atom arg2); // dest := arg1 op arg2
Instr *emit3_relop(Atom arg1, relop op, Atom arg2, const char *true_label, const char *false_label); // COND arg1 rel_op arg2 true_label false_label (como pseudo-instr)
Instr *emit_label(const char *label); // LABEL label
Instr *emit_jump(const char *label); // JUMP label
Instr *emit_jump_false(Atom cond, const char *label); // JUMP_FALSE cond label
Instr *emit_print(Atom value); // PUT value
Instr *emit_read(const char *var_name); // GET var_name

// printing
void print_instr(const Instr *instr);
void print_instr_list(const InstrList *list);

// memory management
void free_instr( Instr *instr);
void free_instr_list(InstrList *list);

// list management
InstrList *new_instr_list(void);
void append_instr(InstrList *list, Instr *instr);
Instr *get_first(const InstrList *list);
InstrNode *next_instrs(const InstrNode *node);
InstrList *get_instr_list(void);

// translation functions from ast to TAC
Atom transExpr(Exp e);
void transStm(Stm s);
void transCond(Exp e, Atom labelF, Atom labelT);

#endif