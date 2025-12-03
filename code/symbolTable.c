# include "ast.h"
#include "symbolTable.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
static size_t global_offset_counter = 0;
static char* current_scope = "Global";
const int WORD_SIZE = 4; // assuming 4 bytes for word size  
/* create a new empty symbol table
*/
Table create(void) {
  return NULL;
}
/* Lookup a name in a table; returns entry or NULL if it doesn't occur
 */
Entry *lookup(Table tbl, char *name) {
  if(name==NULL) return NULL;
  while (tbl != NULL) {
    if(strcmp(tbl->key, name) == 0) 
      return tbl;
    tbl = tbl->next;
  }
  return NULL;
}

/* Lookup a name in table; return a value or exits with error
 */
SymbolInfo* lookup_value(Table tbl, char *name) {
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
    fprintf(stderr, "unknown entry\n");
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
  ptr->key = strdup(name);
  ptr->value = value;
  ptr->next = tbl;
  return ptr;
}

/* Free the memory used by a table
 */
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
/* remove entry from table
*/
Table remove_entry(Table tbl, Entry *ptr){
    
    if(ptr == NULL) {
        fprintf(stderr, "unknown entry\n");
        return tbl;
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
    fprintf(stderr, "entry not found in table: %s\n", ptr->key);
    return tbl;

}


static TypeKind infer_type(Exp e, Table t){
  if(!e) return T_VOID;
  switch(e->exp_t){
    case NUMEXP:
      return T_INT;
    case FLOATEXP:
      return T_FLOAT;
    case BOOLEXP:
      return T_BOOLEAN;
    case STREXP:
      return T_STRING;
    case OPEXP: {
      TypeKind left_type = infer_type(e->fields.opexp.left, t);
      TypeKind right_type = infer_type(e->fields.opexp.right, t);
      int op = e->fields.opexp.op;
      if(op == SUM || op == SUB || op == TIMES || op == DIVISION || op == MODULUS || op == REMAINDER || op == POW){
        if(left_type == T_FLOAT || right_type == T_FLOAT){
          return T_FLOAT;
        } 
        if(left_type == T_INT && right_type == T_INT){
          return T_INT;
        } 
      } 
      return T_INT; // unknown
    }
    case IDEXP:
      {
        SymbolInfo *info = lookup_value(t, e->fields.ident);
        if(info){
          return info->type;
        } else {
          return T_VOID; // unknown
        }
      }
    case PAREXP:
      return infer_type(e->fields.parexp.inner, t);
    default:
      return T_VOID;
  }

}

/* create new symbol info
*/
SymbolInfo* symbolInfo_new(void){
    SymbolInfo *symInfo = malloc(sizeof(SymbolInfo));
    if(symInfo == NULL){
        fprintf(stderr, "out of memory\n");
        exit(1);
    }
    symInfo->kind = VAR; // default kind
    symInfo->type = T_VOID;
    symInfo->size = 0;
    symInfo->offset = 0;
    symInfo->canonical_name = NULL;
    symInfo->name = NULL;
    symInfo->scope = NULL;
    return symInfo;
}

/* free symbol info
*/
void symbolInfo_free(SymbolInfo *symInfo){
    if(symInfo == NULL) return;
    if(symInfo->canonical_name) free(symInfo->canonical_name);
    if(symInfo->name) free(symInfo->name);
    if(symInfo->scope) free(symInfo->scope);
    free(symInfo);
}

/* generate canonical name for a symbol (case insensitive)
*/
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

static Table register_var(Table t, char *name, TypeKind type){
  if(name==NULL) return t;
  char *canon = canonicalize_name(name);
  // fprintf(stderr, "Registering variable: %s (canonical: %s)\n", name, canon);
  if(lookup(t, canon) == NULL){
    SymbolInfo *info = symbolInfo_new();
    info->kind = VAR;
    info->type = type;
    info->name = strdup(name);
    info->canonical_name = strdup(canon);
    info->scope = strdup(current_scope);
    info->size = WORD_SIZE;
    info->offset = global_offset_counter;
    global_offset_counter += WORD_SIZE;
    t = add_entry(t, canon, info);
    // fprintf(stderr, "Variable %s registered.\n", name);
  } else {
    // fprintf(stderr, "Variable %s already registered.\n", name);
  }
  free(canon);
  return t;
}
/* perform semantic checks and build symbol table
*/
Table check_semantics(Stm s, Table t){
  if(!s) return t;
  switch(s->stm_t){
    case ASSIGNSTM:{
    TypeKind kind = infer_type(s->fields.assign.exp, t);
      t  = register_var(t, s->fields.assign.ident, kind);
      break;
    }
    case COMPOUNDSTM:
      t = check_semantics(s->fields.compound.fst, t);
      t = check_semantics(s->fields.compound.snd, t);
      break;
    case IFSTM:
      t = check_semantics(s->fields.ifstm.then_branch, t);
      if(s->fields.ifstm.else_branch){
        t = check_semantics(s->fields.ifstm.else_branch, t);
      }
      break;
    case WHILESTM:
      t = check_semantics(s->fields.whilestm.branch, t);
      break;
    case GETSTM: {
      char* canon = canonicalize_name(s->fields.getstm.ident);
      Entry *existing = lookup(t, canon);
      if(existing && existing->value){
        t = register_var(t, s->fields.getstm.ident, existing->value->type);
      } else {
      t  = register_var(t, s->fields.getstm.ident, T_STRING);
      }
      free(canon);
      break;
    }
    case PUTSTM:
      infer_type(s->fields.putstm.output, t);
      break;
    case PROCSTM: {
      char *canon = canonicalize_name(s->fields.proc.name);
      if(lookup(t, canon) == NULL){
        SymbolInfo *info = symbolInfo_new();
        info->kind = PROC;
        info->name = strdup(s->fields.proc.name);
        info->canonical_name = strdup(canon);
        info->scope = strdup(current_scope);
        t = add_entry(t, canon, info);
      }
      free(canon);
      char* old_scope = current_scope;
      current_scope = s->fields.proc.name;

      t = check_semantics(s->fields.proc.statements, t);
      current_scope = old_scope;
      break;
    }
    default:
      break;

  }
  return t;
}

static const char* get_kind_string(SymbolKind kind){
  switch(kind){
    case VAR:
      return "VAR";
    case CONST:
      return "CONST";
    case TYPE:
      return "TYPE";
    case PROC:
      return "PROC";
    default:
      return "UNKNOWN";
  }
}

void printSymbolTable(Table t){
  printf("\nSymbol Table:\n");
  printf("---------------------------------------------------------------------------------\n");
  printf("| %-15s | %-15s | %-8s | %-10s | %-6s | %-8s |\n", "CANONICAL NAME", "NAME", "KIND", "SCOPE", "SIZE", "OFFSET");
  printf("---------------------------------------------------------------------------------\n");
  if(t == NULL){
    printf("| %-98s |\n", "(empty)");
    printf("-------------------------------------------------------------------------------\n");
    return;
  }
  Entry *cur = t;
  while(cur != NULL){
    SymbolInfo *info = cur->value;
    printf("| %-15s | %-15s | %-8s | %-10s | %-6d | %-8zu |\n",
           info->canonical_name ? info->canonical_name : "(null)",
           info->name ? info->name : "(null)",
           get_kind_string(info->kind),
           info->scope ? info->scope : "(null)",
           info->size,
           info->offset);
    cur = cur->next;
  }
  printf("---------------------------------------------------------------------------------\n");
  printf("\n");
}