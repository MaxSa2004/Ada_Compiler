#define _SYMBOLTABLE_H_
#include <stdbool.h>
#include <stddef.h>
#ifndef _SYMBOLTABLE_H_
typedef struct Type Type;
typedef enum {
    VAR, CONST, TYPE, FIELD, PROC
} SymbolKind;

typedef struct SymbolInfo {
    SymbolKind kind;
    Type *type;
    int size;
    bool isParameter;
    size_t offset;
    void* decl_node;
    int line;
    int column;
    char* canonical_name;
    char *name;
} SymbolInfo;

typedef struct _entry {
    char *key;
    SymbolInfo *value;
    struct _entry *next;
} Entry;


typedef Entry *Table;
extern Table create(void);
extern int lookup_value(Table, char *);
extern Entry *lookup(Table, char *);
extern Table add_entry(Table, char *, SymbolInfo *);
extern void update_value(Entry *, SymbolInfo *);
extern void free_table(Table);
extern void remove_entry(Table, char *);
SymbolInfo *symbolInfo_new(void);
void symbolInfo_free(SymbolInfo *);
char* canonicalize_name(char *);

#endif