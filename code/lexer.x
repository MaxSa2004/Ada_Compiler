/* Analisador Lexical Flex */
%{
#include <stdlib.h>
#include <string.h>

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

    char *q = out;
    while (p < end) {
        if (*p == '"' && (p+1) < end && *(p+1) == '"'){
            *q++ = '"'; /* write a literal when we detect "" */
            p+=2;
        } else{
            *q++ = *p++; /* read from p and write to q.. then move both forward */
        }
        
    }
    
    *q = '\0';
    return out;
}

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

/*  */
{STRING}        {
                    yylval.s_val = unend_ada_string_quotes(yytext);
                    return STRING_LITERAL;
                }

{DIGIT}+        {
                    yylval.i_val = atoi(yytext);
                    return NUM;
                }



%%
