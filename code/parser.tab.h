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
    LPAREN = 262,                  /* LPAREN  */
    RPAREN = 263,                  /* RPAREN  */
    EQ = 264,                      /* EQ  */
    INEQ = 265,                    /* INEQ  */
    OR = 266,                      /* OR  */
    AND = 267,                     /* AND  */
    XOR = 268,                     /* XOR  */
    LESS = 269,                    /* LESS  */
    GREATER = 270,                 /* GREATER  */
    LEQ = 271,                     /* LEQ  */
    GEQ = 272,                     /* GEQ  */
    MOD = 273,                     /* MOD  */
    REM = 274,                     /* REM  */
    ASSIGN = 275,                  /* ASSIGN  */
    NOT = 276,                     /* NOT  */
    ID = 277,                      /* ID  */
    TRUE = 278,                    /* TRUE  */
    FALSE = 279,                   /* FALSE  */
    STRING_LITERAL = 280,          /* STRING_LITERAL  */
    NUM = 281,                     /* NUM  */
    PUT_LINE = 282,                /* PUT_LINE  */
    GET_LINE = 283,                /* GET_LINE  */
    IF = 284,                      /* IF  */
    THEN = 285,                    /* THEN  */
    ELSE = 286,                    /* ELSE  */
    WHILE = 287,                   /* WHILE  */
    LOOP = 288,                    /* LOOP  */
    END = 289,                     /* END  */
    INTEGER = 290,                 /* INTEGER  */
    BOOLEAN = 291,                 /* BOOLEAN  */
    STRING = 292,                  /* STRING  */
    SEMICOLON = 293,               /* SEMICOLON  */
    MAIN = 294,                    /* MAIN  */
    BEGIN = 295,                   /* BEGIN  */
    PROCEDURE = 296,               /* PROCEDURE  */
    IS = 297,                      /* IS  */
    UNARY_MINUS = 298              /* UNARY_MINUS  */
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
   char* s_val; /* for both ID and STRING_LITERAL */
   Exp exp_node;
   Stm stm_node;
   BinOp op_type;

#line 115 "parser.tab.h"

};
typedef union YYSTYPE YYSTYPE;
# define YYSTYPE_IS_TRIVIAL 1
# define YYSTYPE_IS_DECLARED 1
#endif


extern YYSTYPE yylval;


int yyparse (void);


#endif /* !YY_YY_PARSER_TAB_H_INCLUDED  */
