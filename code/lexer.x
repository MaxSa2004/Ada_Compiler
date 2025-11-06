/* Analisador Lexical Flex */
%{
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

/* includes token definitions from parser */
#include "parser.tab.h"

/* expose yylval produced by bison */
extern YYSTYPE yylval;

/* helper to allow double quotes inside strings (like Ada does): example of string "he says "hello" to me" */
/* s points to the entire matched string including surrounding quotes */
/* p is inout cursor and q is output cursor */
static char *unend_ada_string_quotes(const char *s){
    size_t len = strlen(s);

    /* content between the two outer quotes */
    const char *p = s + 1;
    const char *end = s + len - 1;

    /* worst possible length is len, so allocate memory for worst case */
    char *out = malloc(len);
    if(!out){
        perror("malloc");
        exit(1);
    }

    char *q = out;
    while (p < end) {
        if (*p == '"' && (p+1) < end && *(p+1) == '"'){
            *q++ = '"'; /* write a literal when we detect "" */
            p+=2;
        } else{
            *q++ = *p++; /* read from p and write to q.. then move both forward */
        }
        
    }
    
    *q = '\0'; /* end of string */
    return out;
}

/* helper to make lowercase */
static void toLowerCase(char *s){
    int i = 0;

    while (s[i] != '\0'){
        s[i] = (char)tolower((unsigned char)s[i]);
        i++;
    }
}

/* create a custom structure to store keywords of ada */
typedef struct {
    const char *name;
    int token;
} keyword_tokens;

static keyword_tokens keywords[] = {
    {"main", MAIN},
    {"begin", BEGIN},
    {"procedure", PROCEDURE},
    {"is", IS},
    {"if", IF},
    {"then", THEN},
    {"else", ELSE},
    {"while", WHILE},
    {"loop", LOOP},
    {"end", END},
    {"put_line", PUT_LINE},
    {"get_line", GET_LINE},
    {"true", TRUE},
    {"false", FALSE},
    {"integer", INTEGER},
    {"boolean", BOOLEAN},
    {"string", STRING},
    {"or", OR},
    {"xor", XOR},
    {"and", AND},
    {"not", NOT},
    {"mod", MOD},
    {"rem", REM},
    {NULL, 0}
};

%}

// flex options
%option noyywrap
%option yylineno

/* Declarations */ 
DIGIT           [0-9]
IDENT_START     [A-Za-z]  /* Starting chars allowed for identifiers*/
IDENT_REST      [A-Za-z0-9_] /* rest of chars allowed*/
IDENT           {IDENT_START}{IDENT_REST}*
STRING_CHARS    ([^"\n]|"") /* anything except double quote or newline */
STRING          \"{STRING_CHARS}*\"


%%
/* expressões regulares e ações */
[ \t\r\n]+      /* skip whitespace */

"--".*          { /* skip the comments */ ; }

/* operator symbols */
":="            { return ASSIGN; }
"/="            { return INEQ; }
"="             { return EQ; }
"<="            { return LEQ; }
">="            { return GEQ; }
"<"             { return LESS; }
">"             { return GREATER; }
"+"             { return PLUS; }
"-"             { return MINUS; }
"*"             { return MULT; }
"/"             { return DIV; }
"("             { return LPAREN; }
")"             { return RPAREN; }
";"             { return SEMICOLON; }

/* string, numbers and ids */
{STRING}            {
                        yylval.s_val = unend_ada_string_quotes(yytext);
                        return STRING_LITERAL;
                    }

/*floats supported: (.14) (0.14) (42.) (3.14) */
({DIGIT}+"."{DIGIT}* | "."{DIGIT}+) {
                        yylval.f_val = atof(yytext);
                        return FLOAT;
                    }

{DIGIT}+            {
                        yylval.i_val = atoi(yytext);
                        return NUM;
                    }

{IDENT}             {
                        /* lowercase duplicate for keyword comparisons */
                        char *s = strdup(yytext);
                        toLowerCase(s);
                        
                        /*check for keywords */
                        for(keyword_tokens *kw = keywords; kw->name; ++kw){
                            if(strcmp(s,kw->name) == 0){
                                free(s);
                                return kw->token;
                            }
                        }

                        /*otherwise it is an identifier*/
                        yylval.s_val = strdup(yytext); /*keep original case and not lowered case*/
                        free(s);
                        return ID;
                    }

/*anything else not recognised is warned to user*/
.                   {
                        fprintf(stderr, "lexer error: unexpected character '%s' (line: %d)\n", yytext, yylineno);
                    }

%%
