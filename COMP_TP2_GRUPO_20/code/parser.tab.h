/* A Bison parser, made by GNU Bison 3.8.2.  */

/* Bison interface for Yacc-like parsers in C

   Copyright (C) 1984, 1989-1990, 2000-2015, 2018-2021 Free Software Foundation,
   Inc.

   This program is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program.  If not, see <https://www.gnu.org/licenses/>.  */

/* As a special exception, you may create a larger work that contains
   part or all of the Bison parser skeleton and distribute that work
   under terms of your choice, so long as that work isn't itself a
   parser generator using the skeleton or a modified version thereof
   as a parser skeleton.  Alternatively, if you modify or redistribute
   the parser skeleton itself, you may (at your option) remove this
   special exception, which will cause the skeleton and the resulting
   Bison output files to be licensed under the GNU General Public
   License without this special exception.

   This special exception was added by the Free Software Foundation in
   version 2.2 of Bison.  */

/* DO NOT RELY ON FEATURES THAT ARE NOT DOCUMENTED in the manual,
   especially those whose name start with YY_ or yy_.  They are
   private implementation details that can be changed or removed.  */

#ifndef YY_YY_PARSER_TAB_H_INCLUDED
# define YY_YY_PARSER_TAB_H_INCLUDED
/* Debug traces.  */
#ifndef YYDEBUG
# define YYDEBUG 0
#endif
#if YYDEBUG
extern int yydebug;
#endif

/* Token kinds.  */
#ifndef YYTOKENTYPE
# define YYTOKENTYPE
  enum yytokentype
  {
    YYEMPTY = -2,
    YYEOF = 0,                     /* "end of file"  */
    YYerror = 256,                 /* error  */
    YYUNDEF = 257,                 /* "invalid token"  */
    PLUS = 258,                    /* PLUS  */
    MINUS = 259,                   /* MINUS  */
    MULT = 260,                    /* MULT  */
    DIV = 261,                     /* DIV  */
    POWER = 262,                   /* POWER  */
    LPAREN = 263,                  /* LPAREN  */
    RPAREN = 264,                  /* RPAREN  */
    EQ = 265,                      /* EQ  */
    INEQ = 266,                    /* INEQ  */
    OR = 267,                      /* OR  */
    AND = 268,                     /* AND  */
    XOR = 269,                     /* XOR  */
    LESS = 270,                    /* LESS  */
    GREATER = 271,                 /* GREATER  */
    LEQ = 272,                     /* LEQ  */
    GEQ = 273,                     /* GEQ  */
    MOD = 274,                     /* MOD  */
    REM = 275,                     /* REM  */
    ASSIGN = 276,                  /* ASSIGN  */
    NOT = 277,                     /* NOT  */
    ID = 278,                      /* ID  */
    TRUE = 279,                    /* TRUE  */
    FALSE = 280,                   /* FALSE  */
    STRING_LITERAL = 281,          /* STRING_LITERAL  */
    NUM = 282,                     /* NUM  */
    FLOAT = 283,                   /* FLOAT  */
    PUT_LINE = 284,                /* PUT_LINE  */
    GET_LINE = 285,                /* GET_LINE  */
    IF = 286,                      /* IF  */
    THEN = 287,                    /* THEN  */
    ELSE = 288,                    /* ELSE  */
    WHILE = 289,                   /* WHILE  */
    LOOP = 290,                    /* LOOP  */
    END = 291,                     /* END  */
    INTEGER = 292,                 /* INTEGER  */
    BOOLEAN = 293,                 /* BOOLEAN  */
    STRING = 294,                  /* STRING  */
    SEMICOLON = 295,               /* SEMICOLON  */
    BEGIN_T = 296,                 /* BEGIN_T  */
    PROCEDURE = 297,               /* PROCEDURE  */
    IS = 298,                      /* IS  */
    UNARY_MINUS = 299              /* UNARY_MINUS  */
  };
  typedef enum yytokentype yytoken_kind_t;
#endif

/* Value type.  */
#if ! defined YYSTYPE && ! defined YYSTYPE_IS_DECLARED
union YYSTYPE
{
#line 10 "parser.y"
 /* used in bison, defines type of yylval and the semantic values passed between rules */
   int i_val;
   float f_val;
   char* s_val; /* for both ID and STRING_LITERAL */
   Exp exp_node;
   Stm stm_node;
   BinOp op_type;

#line 117 "parser.tab.h"

};
typedef union YYSTYPE YYSTYPE;
# define YYSTYPE_IS_TRIVIAL 1
# define YYSTYPE_IS_DECLARED 1
#endif


extern YYSTYPE yylval;


int yyparse (void);


#endif /* !YY_YY_PARSER_TAB_H_INCLUDED  */
