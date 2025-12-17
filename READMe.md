# Ada Scanner and Parser

A scanner (Flex) and parser (Bison) for a small, well-defined subset of [Ada](https://ada-lang.io). The parser builds an Abstract Syntax Tree (AST) for a single main procedure and supports pretty-printing and a structural debug dump of the AST. From there, a symbol table is built. Also, with the help of the AST, its three-address code is generated, printed, and sent to be transformed into MIPS. Last but not least, the MIPS-generated code, when run on [MARS](https://dpetersanderson.github.io/index.html), gives the result of the initial code we sent out as input. This code was made in [C](https://devdocs.io/c/).
**Note**: All of the examples presented are from input3.ada.

![C](https://img.shields.io/badge/-C/C++-lightpink?style=for-the-badge&logo=c&logoColor=black)
[![EN](https://img.shields.io/badge/lang-EN-0078d4?labelColor=555&logo=google-translate&logoColor=white)](#english)

## Supported Subset
- One procedure: `procedure Main is ... end Main;`, but is made to read any name of procedure, not just Main;
- Statements:
    - Assignment: `x := expr; `
    - If / If-Else: `if expr then ... [else ...] end if;`
    - While: `while expr loop ... end loop; `
    - I/O: `Put_Line(expr);` and `Get_Line(id);`
- Expressions:
    - Integers and floats;
    - Strings with Ada doubled-quote escaping (`"He said ""hi"""`);
    - Identifiers
    - Operators:
        - Arithmetic: `+ - * / ** mod rem`
        - Boolean: `and or xor not`
        - Comparisons: `= /= < > <= >=`
        - Parentheses: `(expr)`
    - Lexical:
        - Line comments: `-- ...` (until end of file)
        - Whitespace ignored

## Requirements
- Build tools: `make`, `flex`, `bison` and `gcc`
- Notes: 
    - Link with `-lm` for portability
    - `-lfl` is not required since lexer uses `%option noyywrap`

## Build
- With Makefile (recommended): 
    - `make && mv adaCompiler adaCompiler_{your OS}`
    - Regenerates `parser.tab.c/.h` and `lex.yy.c` when needed and builds `adaCompiler_{your OS}` 
- Manual (fallback):
    - bison: `bison -d parser.y`
    - flex: `flex lexer.x`
    - `gcc -Wall -g -o (nameOfExecutable) parser.tab.c lex.yy.c ast.c ast_debug.c symbolTable.c codeGenerator.c mips_backend.c main.c -lm`

## Run
There are two different executables in this repository: adaCompiler_Mac and adaCompiler_Linux. The user must choose the one with its OS. If the user wants to create another one, go to [Build](#build)
- Pretty-print (source-like):
    - `./adaCompiler_{your OS}`
    - or `./adaCompiler_{your OS} path/to/file.ada`
- Debug AST dump (structural):
    - `./adaCompiler_{your OS} --debug path/to/file.ada`

Here, the path/to/file.ada is optional, and, if omitted, uses standard input as the input.
## Usage
- `./adaCompiler_{your OS} [--debug] [file]`
    - `--debug` prints a structural AST dump
    - `file` is optional; if omitted, reads from stdin
- Exit status: non-zero on lexical/syntax errors

## Directory Tree
```text
Ada_Compiler/
├── code/
│   ├── ast_debug.c
│   ├── ast.c
|   ├── ast.h
│   ├── lexer.x
|   ├── main.c
|   ├── Makefile
│   ├── parser.y
│   ├── codeGenerator.c
│   ├── codeGenerator.h
│   ├── mips_backend.c
│   ├── mips_backend.h
│   ├── symbolTable.c
│   ├── symbolTable.h
│   ├── adaCompiler_Mac
│   ├── adaCompiler_Linux
│   └── test_inputs/
|        |── input1.ada
|        ├── input2.ada
|        ├── input3.ada
|        ├── input4.ada
|        ├── input5.ada
|        ├── input6.ada
|        ├── input7.ada
|        └── input8.ada
│   └── test_outputs/
|        |── output1MIPS.s
|        ├── output2MIPS.s
|        ├── output3MIPS.s
|        ├── output4MIPS.s
|        ├── output5MIPS.s
|        ├── output6MIPS.s
|        ├── output7MIPS.s
|        └── output8MIPS.s
├── worksheet/
|    ├── Compilers_CourseWork2.pdf
|    └── Compiladores_Trabalho.pdf
└── READMe.md

README.md
```

## Grammar 
- Program: `procedure ID is begin stm_list end ID;`
- Statement list: zero or more `stm`
- Statements: 
    - `ID := expr;`
    - `if expr then stm_list end if;`
    - `if expr then stm_list else stm_list end if;`
    - `while expr loop stm_list end loop;`
    - `Put_Line(expr);`
    - `Get_Line(id);`
- Expressions: literals | `ID` | unary `-` / `not` | binary ops | `(expr)`

## Operator Precedence

From highest to lowest:
1. Unary: `not`, unary `-`
2. Power: `**` (right-associative)
3. Multiplicative: `* / mod rem`
4. Additive: `+ -`
5. Relational/equality: `= /= < > <= >=`
6. Boolean AND
7. Boolean OR / XOR

## AST
- Expressions: `NUM`, `FLOAT`, `ID`, `STRING`, `BOOL`, `OP(op, left, right)`, `UNARY(op, child)`
- Statements: `ASSIGN(id, expr)`, `PUT_LINE(expr)`, `GET_LINE(id)`, `IF(cond, then, else)`, `WHILE(cond, body)`, `COMPOUND(left, right)`, `PROC(name, body)`
- Printing modes:
    - Pretty (Ada-like) via `print_stm`
    - Debug (Structural) via `debug_print`

## Symbol Table
To define each symbol presented and to pass that definition to the Three Address Code, a symbol table is essential. It includes, for each symbol that appears, its type, kind, size, offset, canonical and original name,and scope (which is always the name of the procedure, except the actual procedure, since functions are not implemented).

```
----------------------------------------------------------------------
| CANONICAL NAME  | NAME   | KIND     | SCOPE      | SIZE   | OFFSET |
----------------------------------------------------------------------
| y               | y      | VAR      | main       | 4      | 4      |
| x               | x      | VAR      | main       | 4      | 0      |
| main            | main   | PROC     | Global     | 0      | 0      |
----------------------------------------------------------------------
```

More technically, the symbol table header defines kinds of symbols (VAR, CONST, TYPE, FIELD, PROC), kinds of types (Integer, Float, Boolean, String, Void), the backbone of entries and symbols, as mentioned above, and different functions, such as creating the table, adding and removing entries, looking up for values and even checking semantics, to deal with the table while reading the AST.

## Code Generator (Three Address Code)
To succeed at this translation, we need to define instruction types (LABEL,
JUMP, JUMP FALSE, PRINT, READ, ASSIGN, COND), binary, unary, and relational operations, an atom (operand of TAC), and its kind, an instruction, an instruction’s node and list, and the state of the code generator. 
We also need to create functions to initialize the generator, construct the different types of atoms, manage temporary values, construct instructions (emitters), print an instruction and a list of them, manage the memory and the list, and the most importantly, translate functions from AST to TAC (transExp, transCond, transStm).

```
x := 5
y := 3.14
t0 := x
t1 := 3
COND t0 > t1 L0 L1
LABEL L0
PUT "ok"
JUMP L2
LABEL L1
PUT "no"
LABEL L2
```

## MIPS generator
MIPS or assembly is a text representation of machine code (0’s and 1’s), more readable than binary code. It uses symbolic labels for addresses and constants. It can be transformed into executable machine code by an assembler. The MIPS architecture uses 32 registers ($0-$31) of 32 bits, has a set of load/store instructions, and the instructions in general have a fixed size of 32 bits. In addition to that, the operations can be between 3 registers or registers and constants.
Our MIPS code generates an archive MIPS from a list of instructions, and the content inside the archive is the actual code of those instructions. Because the MIPS architecture has a finite number of registers, something that TAC does not have. The implementation uses a cyclic mapping (round-robin) that defines 16 available registers (8 temporary registers and 8 saved registers). When the TAC asks for $t20, the backend calculates 20%16 = 4, and the compiler reuses the physical address that corresponds to index 4.
Another important aspect of this implementation is that the reading of complex types, such as float or string, demands the use of specific syscalls and a different memory management compared to simple integers.
For floats, the MIPS architecture uses a specific coprocessor for the floating point. The compiler emits syscall 6, and the value read by the system is stored automatically in the float register $f0. Then, that value is moved from $f0 to the memory position that was reserved in the stack for that variable, using the instruction swc1 (Store Word Coprocessor 1).
For strings, this process requires a pre-allocated buffer. The generator detects when the string reading is necessary, and allocates a global space for str_buf in .data section. The syscall 8 is used, which needs two arguments: the buffer address $a0 and the reading maximum size $a1. After the scanning, the buffer’s address is stored in a local variable in the stack. So, the variable points to the address where the text was actually written.

```
.data
str_1: .asciiz "no"
str_0: .asciiz "ok"
flt_0: .float 3.140000
.text
.globl main
main:
    addi $sp, $sp, -16
    sw $ra, 12($sp)
    sw $fp, 8($sp)
    addi $fp, $sp, 16
    addi $t0, $zero, 5
    sw $t0, 0($fp)
    la $t0, flt_0
    lwc1 $f12, 0($t0)
   swc1 $f12, 4($fp)
    lw $t1, 0($fp)
    move $t2, $t1
    addi $t0, $zero, 3
    move $t3, $t0
    slt $1, $t3, $t2
    bne $1, $zero, L0
    beq $1, $zero, L1
L0:
    la $a0, str_0
    li $v0, 4
    syscall
    j L2
L1:
    la $a0, str_1
    li $v0, 4
    syscall
L2:
    li $v0, 10
    syscall

```
The MARS (MIPS Assembler and Runtime Simulator) prints, for this list of
instructions, the string "ok".
## Output
Using `test_inputs/input3.ada`:
- Pretty-printing AST:
```
PROCEDURE main IS BEGIN x := 5 ; y := 3.14 ; IF x > 3 THEN PUT_LINE("ok"); ELSE PUT_LINE("no"); END IF; END main;
```
- Debug AST:
```
PROCEDURE(main)
  body:
    ASSIGN(x, NUM(5))
    ASSIGN(y, FLOAT(3.14))
    IF
      cond: OP(GT, ID(x), NUM(3))
      then:
        PUT_LINE(STRING("ok"))
      else:
        PUT_LINE(STRING("no"))
```
- Symbol table:
```
----------------------------------------------------------------------
| CANONICAL NAME  | NAME   | KIND     | SCOPE      | SIZE   | OFFSET |
----------------------------------------------------------------------
| y               | y      | VAR      | main       | 4      | 4      |
| x               | x      | VAR      | main       | 4      | 0      |
| main            | main   | PROC     | Global     | 0      | 0      |
----------------------------------------------------------------------
```
- Three Adress Code:
```
x := 5
y := 3.14
t0 := x
t1 := 3
COND t0 > t1 L0 L1
LABEL L0
PUT "ok"
JUMP L2
LABEL L1
PUT "no"
LABEL L2
```
- MIPS (at output3MIPS.s):
```
.data
str_1: .asciiz "no"
str_0: .asciiz "ok"
flt_0: .float 3.140000
.text
.globl main
main:
    addi $sp, $sp, -16
    sw $ra, 12($sp)
    sw $fp, 8($sp)
    addi $fp, $sp, 16
    addi $t0, $zero, 5
    sw $t0, 0($fp)
    la $t0, flt_0
    lwc1 $f12, 0($t0)
   swc1 $f12, 4($fp)
    lw $t1, 0($fp)
    move $t2, $t1
    addi $t0, $zero, 3
    move $t3, $t0
    slt $1, $t3, $t2
    bne $1, $zero, L0
    beq $1, $zero, L1
L0:
    la $a0, str_0
    li $v0, 4
    syscall
    j L2
L1:
    la $a0, str_1
    li $v0, 4
    syscall
L2:
    li $v0, 10
    syscall

```

## Memory Management
- Nodes and strings allocated with `malloc` / `strdup`
- Entire AST freed with `free_stm(root)` which recursively frees expressions/statements and owned strings

## Examples
See `test-inputs/`.

## Authors
Made by Maximiliano Sá and Rita Moreira.

Built as part of coursework at [Faculdade de Ciências da Universidade do Porto](https://www.up.pt/fcup/pt/).

## Acknowledgments
- [ada-auth.org](http://www.ada-auth.org/standards/aarm12_w_tc1/html/AA-A-10-1.html)
- [ada-lang.io](https://ada-lang.io)
- [compiler-design-series](https://github.com/marcauberer/compiler-design-series)
- [geeksforgeeks](https://www.geeksforgeeks.org/compiler-design/introduction-of-compiler-design/)
- [building-my-own-compiler](https://medium.com/@nevo.krien/from-zero-to-building-my-own-compiler-ed0fcec9970d)
