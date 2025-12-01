#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include "mips_backend.h"

static FILE *out = NULL;

// Helpers to identify strings and variables and their respective lists
typedef struct VarEntry
{
    char *name;
    struct VarEntry *next;
} VarEntry;

typedef struct StrEntry
{
    char *str;
    char *label;
    struct StrEntry *next;
} StrEntry;

static VarEntry *var_list = NULL;
static StrEntry *str_list = NULL;
static int highest_temp_id = -1; // store largest temp ID in TAC
static int str_count = 0;        // num of string constants

// helper to add var to list
static void addVar(const char *name)
{
    if (!name)
        return;
    for (VarEntry *i = var_list; i; i = i->next)
    {
        if (strcmp(i->name, name) == 0)
            return; // already in list
    }
    VarEntry *new = (VarEntry *)malloc(sizeof(VarEntry));
    new->name = strdup(name);
    new->next = var_list;
    var_list = new;
}

// helper to add a string constant to list of strings and return its respective label
static const char *addString(const char *str)
{
    if (!str)
        return NULL; // default error
    for (StrEntry *i = str_list; i; i = i->next)
    {
        if (strcmp(i->str, str) == 0)
            return i->label;
    }
    StrEntry *new = (StrEntry *)malloc(sizeof(StrEntry));
    new->str = strdup(str);
    char buf[128];
    snprintf(buf, sizeof(buf), "str_%d", str_count++);
    new->label = strdup(buf);
    new->next = str_list;
    str_list = new;
    return new->label;
}

// helper to scan through instructions, collecting vars and constant strings
static void scanInstrs(Instr *head)
{
    highest_temp_id = -1;

    for (Instr *instr = head; instr; instr = instr->next)
    {
        Op ops[3] = {instr->arg1, instr->arg2, instr->arg3};
        for (int i = 0; i < 3; i++)
        {
            Op *op = &ops[i];

            /* update highest temp id when we see a temp */
            if (op->kind == OP_TEMP)
            {
                if (op->contents.temp_id > highest_temp_id)
                {
                    highest_temp_id = op->contents.temp_id;
                }
                continue;
            }

            /* collect string constants as before */
            if (op->kind == OP_CONST_STRING)
            {
                if (op->contents.sval)
                {
                    addString(op->contents.sval);
                }
                continue;
            }

            /* for OP_VAR, only add to var_list if this occurrence is actually a data variable,
               not a label. In our IR the following are label positions and should be skipped:
               - opcode == OP_LABEL and i == 2 (arg3 is the label name)
               - opcode == OP_JUMP and i == 2 (jump target)
               - opcode == OP_JUMP_FALSE and i == 2 (jump target)

               Otherwise, treat OP_VAR as a real variable and add it to var_list.
            */
            if (op->kind == OP_VAR)
            {
                int is_label_pos = 0;
                if ((instr->opcode == OP_LABEL || instr->opcode == OP_JUMP || instr->opcode == OP_JUMP_FALSE) && i == 2)
                {
                    is_label_pos = 1;
                }

                if (!is_label_pos && op->contents.name)
                {
                    addVar(op->contents.name);
                }
            }
        }
    }
}

// escape double quotes and backslashes for .asciiz
static char *escapeStringForASCIIZ(const char *s)
{
    if (!s)
    {
        /* return an empty string (caller frees) */
        char *e = malloc(1);
        if (e)
            e[0] = '\0';
        return e;
    }

    size_t len = strlen(s);
    /* worst-case every char becomes two bytes (\" or \\ or \n) */
    char *out = (char *)malloc(len * 2 + 1);
    if (!out)
        return NULL;

    char *w = out;
    for (const char *i = s; *i; ++i)
    {
        if (*i == '\\' || *i == '"')
        {
            *w++ = '\\';
            *w++ = *i;
        }
        else if (*i == '\n')
        {
            *w++ = '\\';
            *w++ = 'n';
        }
        else
        {
            *w++ = *i;
        }
    }
    *w = '\0';
    return out;
}

// map temp num to stack offset so can generate lw/sw instructions
static int tempOffset(int tempID)
{
    return -12 - 4 * tempID;
}

// helper to print to file
static void emit(const char *str, ...)
{              // ... means multiple arguments allowed
    va_list a; // var to iterate args
    va_start(a, str);
    vfprintf(out, str, a);
    fprintf(out, "\n");
    va_end(a);
}

// load an operand into a register name
static void loadOpToReg(Op op, const char *reg)
{
    // empty op
    if (op.kind == 0)
    {
        emit("  li %s, 0", reg); // load 0 into register named reg by default
        return;
    }

    switch (op.kind)
    {
    case OP_CONST_INT:
        emit("  li %s, %d", reg, op.contents.ival);
        break;
    case OP_CONST_STRING:
        const char *str = addString(op.contents.sval);
        emit("  la %s, %s", reg, str);
        break;
    case OP_CONST_FLOAT: // Note: NEED TO FULLY IMPLEMENT AND FIX
        emit("  # FLOAT CONST NOT FULLY SUPPORTED! val: %g", op.contents.fval);
        emit("  li %s, 0", reg); // temporarily emit as a 0 int
        break;
    case OP_VAR:
        if (op.contents.name)
        {
            emit("  lw %s, %s", reg, op.contents.name); // var label in .data
        }
        else
        {
            emit("  li %s, 0", reg); // if name field is null revert to loading 0
        }
        break;
    case OP_TEMP:
        int offset = tempOffset(op.contents.temp_id);
        emit("  lw %s, %d($fp)  #load temp t%d", reg, offset, op.contents.temp_id);
        break;
    default:
        emit("  # loadOpToReg: encountered unspported kind %d", op.kind);
        emit("  li %s, 0", reg);
        break;
    }
}

// store register value into an Op (temp or var)
static void loadRegToOp(const char *reg, Op op)
{
    if (op.kind == 0)
        return;
    switch (op.kind)
    {
    case OP_VAR:
        if (op.contents.name)
        {
            // store into global labels ($t9 used as address temp)
            emit("  la $t9, %s", op.contents.name);
            emit("  sw %s, 0($t9)", reg);
        }
        break;
    case OP_TEMP:
        int offset = tempOffset(op.contents.temp_id);
        emit("  sw %s, %d($fp)  #load temp t%d", reg, offset, op.contents.temp_id);
        break;
        break;
    default:
        emit("  # loadRegToOp: encountered unspported kind %d", op.kind);
        break;
    }
}

// emit logical and binary math ops.
static void emitBinOp(Opcode op, Op dest, Op r1, Op r2)
{
    loadOpToReg(r1, "$t0"); // static registers safe to use as they execute then store immediately
    loadOpToReg(r2, "$t1");

    switch (op)
    {
    case OP_ADD:
        emit("  add $t0, $t0, $t1");
        break;
    case OP_SUB:
        emit("  sub $t0, $t0, $t1");
        break;
    case OP_MUL:
        emit("  mul $t0, $t0, $t1");
        break;
    case OP_DIV:
        emit("  div $t0, $t1");
        emit("  mflo $t0"); // copies quotient into int $rd
        break;
    case OP_MOD:
    case OP_REM:
        emit("  div $t0, $t1");
        emit("  mfhi $t0"); // machine remainder
        break;
    case OP_AND:
        emit("  and $t0, $t0, $t1");
        break;
    case OP_OR:
        emit("  or $t0, $t0, $t1");
        break;
    case OP_XOR:
        emit("  xor $t0, $t0, $t1");
        break;
    case OP_EQ:
        emit("  xor $t2, $t0, $t1");
        emit("  sltiu $t0, $t2, 1"); // 1 if equal
        break;
    case OP_NEQ:
        emit("  xor $t2, $t0, $t1");
        emit("  sltu $t0, $zero, $t2"); // 1 if non-zero
        break;
    case OP_LT:
        emit("  slt $t0, $t0, $t1");
        break;
    case OP_GT:
        emit("  slt $t0, $t1, $t0");
        break;
    case OP_LEQ:
        // !(a2 < a1)  i.e., not (a1 > a2) -> slt tmp,a2,a1 ; not
        emit("  slt $t2, $t1, $t0");
        emit("  xori $t0, $t2, 1");
        break;
    case OP_GEQ:
        emit("  slt $t2, $t0, $t1");
        emit("  xori $t0, $t2, 1");
        break;
    default:
        emit("  # binary op not implemented: %d", op);
        emit("  li $t0, 0");
        break;
    }
    loadRegToOp("$t0", dest);
}

// emit unary op
static void emimtUnaryOp(Opcode op, Op dest, Op r1)
{
    loadOpToReg(r1, "$t0");

    switch (op)
    {
    case OP_NEG:
        emit("  sub $t0, $zero, $t0");
        break;
    case OP_NOT:
        // result = (a1 == 0) ? 1 : 0
        emit("  sltiu $t0, $t0, 1");
        break;
    default:
        emit("  # unary op not implemented %d", op);
        emit("  li $t0, 0");
        break;
    }
    loadRegToOp("$t0", dest);
}

// integer power helper (pow_int). expects $a0=base, $a1=exp; returns $v0
static void emitPOWHelper()
{
    emit("# integer power helper (pow_int). expects $a0=base, $a1=exp; returns $v0");
    emit("pow_int:");
    emit("  li $t0, 0");
    emit("  blez $a1, pow_done"); // exp <= 0 -> return 1
    emit("  li $v0, 1");
    emit("pow_loop:");
    emit("  mul $v0, $v0, $a0");
    emit("  addi $a1, $a1, -1");
    emit("  bgtz $a1, pow_loop");
    emit("pow_done:");
    emit("  jr $ra");
}

void generateMIPS(Instr *head, const char *filename)
{
    if (!head || !filename)
    {
        return;
    }

    // ensure that lists are initially empty and global vars correctly initialized
    while (var_list)
    {
        VarEntry *i = var_list->next;
        free(var_list->name);
        free(var_list);
        var_list = i;
    }
    while (str_list)
    {
        StrEntry *i = str_list->next;
        free(str_list->str);
        free(str_list->label);
        str_list = i;
    }
    highest_temp_id = -1;
    str_count = 0;

    scanInstrs(head);

    out = fopen(filename, "w"); // open file for writing
    if (!out)
    {
        fprintf(stderr, "Error creating file %s\n", filename);
        return;
    }

    // initial lines of MIPS file - .data variables and strings
    emit("  .data");
    emit("newline: .asciiz \"\\n\""); // newline label
    // list vars
    for (VarEntry *v = var_list; v; v = v->next)
    {
        // alloc 1 word for each, 0 init.
        emit("%s: .word 0", v->name);
    }
    // list strings
    for (StrEntry *s = str_list; s; s = s->next)
    {
        char *esc = escapeStringForASCIIZ(s->str);
        emit("%s: .asciiz \"%s\"", s->label, esc);
        free(esc);
    }

    // text section
    emit("\n    .text");
    emit("  .globl main");
    emit("main:");

    // frame size: at least 12+ 4*(numTemps) --- 4bytes $ra, 4bytes $fp, 4bytes base to position first temp at -12($fp)
    // frame size is num bytes function prologue reserves on stack
    int numTemps = highest_temp_id + 1;
    int frameSize = 12 + 4 * numTemps;
    if (frameSize % 8 != 0)
    {
        frameSize += 4; // must be multiple of 8
    }

    // prologue
    emit("  addi $sp, $sp, -%d", frameSize);
    emit("  sw $ra, %d($sp)     # save ra", frameSize - 4);
    emit("  sw $fp, %d($sp)     # save fp", frameSize - 8);
    emit("  addi $fp, $sp, %d", frameSize);

    // process and emit each instruction
    for (Instr *i = head; i; i = i->next)
    {
        // comment TAC into MIPS code to see line by line translation
        emit("  #TAC: opcode:%d", i->opcode);

        switch (i->opcode)
        {
        case OP_LABEL:
            if (i->arg3.kind == OP_VAR && i->arg3.contents.name)
            {
                emit("%s:", i->arg3.contents.name);
            }
            else
            {
                emit("Label_unknown:");
            }
            break;
        case OP_JUMP:
            if (i->arg3.kind == OP_VAR && i->arg3.contents.name)
            {
                emit("  j %s", i->arg3.contents.name);
            }
            break;
        case OP_JUMP_FALSE:
            // arg3 = label, arg1 = (temp/expr condition)
            if (i->arg1.kind == OP_TEMP || i->arg1.kind == OP_VAR || i->arg1.kind == OP_CONST_INT)
            {
                loadOpToReg(i->arg1, "$t0");
                if (i->arg3.kind == OP_VAR && i->arg3.contents.name)
                {
                    emit("  beq $t0, $zero, %s", i->arg3.contents.name);
                }
                else
                {
                    emit("  # JUMP_FALSE with unknown label");
                }
            }
            else
            {
                emit("  # JUMP_FALSE with unsupported arg1 kind %d", i->arg1.kind);
            }
            break;
        case OP_MOVE:
            // arg3 := arg1
            loadOpToReg(i->arg1, "$t0");
            loadRegToOp("$t0", i->arg3);
            break;
        case OP_ADD:
        case OP_SUB:
        case OP_MUL:
        case OP_DIV:
        case OP_MOD:
        case OP_REM:
        case OP_AND:
        case OP_OR:
        case OP_XOR:
        case OP_EQ:
        case OP_NEQ:
        case OP_LT:
        case OP_GT:
        case OP_LEQ:
        case OP_GEQ:
            emitBinOp(i->opcode, i->arg3, i->arg1, i->arg2);
            break;
        case OP_NOT:
        case OP_NEG:
            emimtUnaryOp(i->opcode, i->arg3, i->arg3);
            break;
        case OP_PRINT:
            // arg3 contains what to print
            if (i->arg3.kind == OP_CONST_STRING)
            {
                const char *lab = addString(i->arg3.contents.sval);
                emit("  la $a0, %s", lab);
                emit("  li $v0, 4");
                emit("  syscall");
            }
            else if (i->arg3.kind == OP_CONST_INT ||
                     i->arg3.kind == OP_TEMP ||
                     i->arg3.kind == OP_VAR)
            {
                loadOpToReg(i->arg3, "$a0");
                emit("  li $v0, 1");
                emit("  syscall");
            }
            else if (i->arg3.kind == OP_CONST_FLOAT)
            {
                // NOTE: either implement float printing (load to $f12, syscall 2) or fallback
                emit("  # NOTE: float not supported yet");
                emit("  li $a0, 0");
                emit("  li $v0, 1");
                emit("  syscall");
            }
            else
            {
                fprintf(stderr, "PRINT: unsupported operand kind=%d\n", i->arg3.kind);
            }
            emit("  la $a0, newline");
            emit("  li $v0, 4");
            emit("  syscall");
            break;
        case OP_READ:
            // only reads ints
            emit("  li $v0, 5");
            emit("  syscall");
            emit("  move $t0, $v0");
            loadRegToOp("$t0", i->arg3);
            break;
        case OP_LOAD:
            // treated as move
            // arg3 := arg1
            loadOpToReg(i->arg1, "$t0");
            loadRegToOp("$t0", i->arg3);
            break;
        case OP_POW:
            // compute arg1 ^ arg2 -> arg3; use helper pow_int
            // load args into $a0(base), $a1(exp) and jal pow_int
            loadOpToReg(i->arg1, "$a0");
            loadOpToReg(i->arg2, "$a1");
            emit("  jal pow_int");
            emit("  move $t0, $v0");
            loadRegToOp("$t0", i->arg3);
            break;
        default:
            emit("  # Unsupported opcode %d", i->opcode);
            break;
        }
    }

    // epilogue
    emit("  lw $ra, %d($sp)     # restore ra", frameSize - 4);
    emit("  lw $fp, %d($sp)     # restore fp", frameSize - 8);
    emit("  addi $sp, $sp, %d", frameSize);

    // exit
    emit("  li $v0, 10");
    emit("  syscall");

    // emit power helper
    if (strstr(out ? "" : "", ""), 1)
    { // noop to satisfy analyzers
    }

    emit("\n    # Helper routines");
    emitPOWHelper();

    fclose(out);

    // cleanup
    while (var_list)
    {
        VarEntry *v = var_list->next;
        free(var_list->name);
        free(var_list);
        var_list = v;
    }
    while (str_list)
    {
        StrEntry *v = str_list->next;
        free(str_list->str);
        free(str_list->label);
        free(str_list);
        str_list = v;
    }
}
