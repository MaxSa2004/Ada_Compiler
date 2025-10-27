============================================================
Abstract syntax trees (ASTs)
============================================================

Summary: we will work on building ASTs for a language of 
arithmetic expressions involving integers.

We will start with "programs" defined by a single expression
made of numbers and sum (+) operations.  

After parsing, the AST for a program is visited/interpreted 
to print values for expressions.

============================================================
1. Get started: 

a) Check the available files:
   - Makefile : makefile to build the interpreter.
   - scanner.flex : the lexical analyser (scanner) in flex
   - parser.bison: the parser in bison
   - ast.h, ast.c: AST declarations & constructor implementations
   - interpreter.c: the interpreter routines including main
   - example[1234].txt : example files

b) Compile the interpreter by typing "make" in the command line.
   The generated executable file is called "interpreter".

c)  The interpreter recognises a single expression composed 
of single integers or expressions making use
of the '+' operator.  

Execute it for example1.txt and example2.txt, for instance 
type "./interpreter example1.txt". 

============================================================
2. Handle binary operators '-', '*', '/', '%'.
You'll need to:

a) Modify the parser / scanner by defining / recognising 
a new tokens for each operator, and also operator 
associativity and precedence.

b) Add new rules to the grammar for 'expr'.

c) Modify the 'eval' function in 'interpreter.c'.

d) Test with examples that make use of the new
operators, for instance the available 'example3.txt'.

e) Define a new file printAbsTree.c and define there a C function printExpr(Expr* exp) which prints the abstract syntax tree stored in the Expr C structure exp.

f) Change the main function to print the abstract syntax tree.



============================================================
3. Relational operators  

Handle boolean expressions. 
Boolean expressions can either an integer (0 stands for false and 1 for true) 
or 'expr <relop> expr' where <relop> is one of the 
relational operators '==', '!=', '<', '>', <=' and '>='.

The following steps are suggested:

a) In ast.h: define the BoolExpr datatype and the declare the corresponding boolean expressions constructors.

b) In asa.c: implement the new constructor functions.

c) In scanner.flex add the new boolean operators.

d) In parser.bison change the grammar by:
-- adding grammar rules for boolean expressions.
-- define new type rules for boolean expressions.

e) In printAbsTree.c : define a function printBool(BoolExpr* b) which prints the abstract syntax tree stored in b.

============================================================

3. Commands

Handle commands following C syntax consisting of assignments, if then and if then else conditional commands, while loops and sequences of commands c1 ; c2 where c1 and c2 are commands.

The following steps are suggested:

a) In ast.h: define the Cmd datatype and the declare the corresponding command constructors.

b) In asa.c: implement the new constructor functions.

c) In scanner.flex add the new tokens belonging to the language of commands (while, if, then, else, = , ; , { , }).

d) In parser.bison change the grammar by:
-- adding grammar rules for commands.
-- define new type rules for commands.
-- define the root variable as a command.

e) In printAbsTree.c : define a function printCmd(Cmd* command) which prints the abstract syntax tree stored in the structure pointed by command.

f) Change the main function to print commands.





