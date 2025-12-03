#ifndef _SYMBOLTABLE_H_ // include guard
#define _SYMBOLTABLE_H_ // include guard
#include <stdbool.h> // for bool type
#include <stddef.h> // for size_t type
#include "ast.h" // for Stm type


typedef enum { // kinds of symbols
    VAR, CONST, TYPE, FIELD, PROC
} SymbolKind;

typedef enum {
    T_INT,
    T_FLOAT,
    T_BOOLEAN,
    T_STRING,
    T_VOID,
    T_UNKNOWN
} TypeKind;

typedef struct SymbolInfo {
    SymbolKind kind; // kind of symbol
    TypeKind type; // pointer to type information
    int size; // size in bytes
    size_t offset; // offset in activation record or object
    char* canonical_name; // canonical name for the symbol
    char *name; // original name of the symbol
    char *scope; // scope where the symbol is declared

} SymbolInfo;

typedef struct _entry {
    char *key; // name of the symbol
    SymbolInfo *value; // pointer to symbol information
    struct _entry *next; // pointer to next entry in the table
} Entry;


typedef Entry *Table; // symbol table is a linked list of entries
extern Table create(void); // create a new empty symbol table
extern SymbolInfo* lookup_value(Table, char *); // lookup a name and return its value
extern Entry *lookup(Table, char *); // lookup a name and return its entry
extern Table add_entry(Table, char *, SymbolInfo *); // add a new entry to the table
extern void update_value(Entry *, SymbolInfo *); // update the value in an entry
extern void free_table(Table); // free the memory used by the table
extern Table remove_entry(Table, Entry*); // remove an entry from the table
extern SymbolInfo *symbolInfo_new(void); // create a new SymbolInfo
extern void symbolInfo_free(SymbolInfo *); // free a SymbolInfo
extern char* canonicalize_name(char *); // generate a canonical name for a symbol, because of the Ada case insensitivity
extern Table check_semantics(Stm root, Table t); // perform semantic checks and build symbol table
extern void printSymbolTable(Table t); // print the symbol table for debugging

#endif