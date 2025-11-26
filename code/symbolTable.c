# include "ast.h"
#include "symbolTable.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>


/* Lookup a name in a table; returns entry or NULL if it doesn't occur
 */
Entry *lookup(Table tbl, char *name) {
  while (tbl != NULL) {
    if(strcmp(tbl->key, name) == 0) 
      return tbl;
    tbl = tbl->next;
  }
  return NULL;
}

/*
 * Lookup a name in table; return a value or exits with error
 */
int lookup_value(Table tbl, char *name) {
  Entry *ptr = lookup(tbl, name);
  if(ptr == NULL) {
    fprintf(stderr, "unknown name: %s\n", name);
    return NULL;
  }
  return ptr->value;
}


/* Update the value in an entry
 */
void update_value(Entry *ptr, SymbolInfo* new_value) {
  if(ptr == NULL) {
    fprintf(stderr, "unknown entry: %s\n", ptr);
    return;
  }
  if(ptr->value !=NULL){
    symbolInfo_free(ptr->value);
  }
  ptr->value = new_value;
}

/* Add an entry to the begining of a table
 */
Table add_entry(Table tbl, char *name, SymbolInfo* value) {
  Entry *ptr = malloc(sizeof(Entry));
  if(ptr == NULL) {
    fprintf(stderr, "out of memory\n");
    exit(1);
  }
  ptr->key = name;
  ptr->value = value;
  ptr->next = tbl;
  return ptr;
}

/* libertar espaço na tabela */
void free_table(Table tbl){
    Entry *cur = tbl;
    while(cur != NULL){
        Entry *next = cur->next;
        if(cur->key) free(cur->key);
        if(cur->value) symbolInfo_free(cur->value);
        free(cur);
        cur = next;
    }
}
/* remover entrada da tabela */
void remove_entry(Table tbl, Entry *ptr){
    if(ptr == NULL) {
        fprintf(stderr, "unknown entry: %s\n", ptr);
        return NULL;
    }
    Entry *prev = NULL;
    Entry *cur = tbl;
    while(cur!=NULL){
        if(cur == ptr){
          Table new_head = tbl;
            if(prev == NULL){
                new_head = cur->next;
            } else {
                prev->next = cur->next;
            }
            if(cur->key) free(cur->key);
            if(cur->value) symbolInfo_free(cur->value);
            free(cur);
            return new_head;
        }
        prev = cur;
        cur = cur->next;
    }
    fprintf(stderr, "entry not found in table: %s\n", ptr);
    return tbl;

}

SymbolInfo* symbolInfo_new(void){
    SymbolInfo *symInfo = malloc(sizeof(SymbolInfo));
    if(symInfo == NULL){
        fprintf(stderr, "out of memory\n");
        exit(1);
    }
    symInfo->kind = VAR; // default kind
    symInfo->type = NULL;
    symInfo->size = 0;
    symInfo->offset = 0;
    symInfo->decl_node = NULL;
    symInfo->line = 0;
    symInfo->column = 0;
    symInfo->canonical_name = NULL;
    symInfo->name = NULL;
    return symInfo;
}

void symbolInfo_free(SymbolInfo *symInfo){
    if(symInfo == NULL) return;
    if(symInfo->canonical_name) free(symInfo->canonical_name);
    if(symInfo->name) free(symInfo->name);
    free(symInfo);
}

char* canonicalize_name(char *name){
    if(name == NULL) return NULL;
    size_t len = strlen(name);
    char *canonical_name = malloc(len + 1);
    if(canonical_name == NULL){
        fprintf(stderr, "out of memory\n");
        exit(1);
    }
    for(size_t i = 0; i < len; i++){
        if(name[i] >= 'A' && name[i] <= 'Z'){
            canonical_name[i] = name[i] + ('a' - 'A');
        } else {
            canonical_name[i] = name[i];
        }
    }
    canonical_name[len] = '\0';
    return canonical_name;
}

