#ifndef AST_H
#define AST_H

typedef enum { SUM, SUB, TIMES, DIVISION, EQUAL, INEQUAL, ORexp, ANDexp, XORexp, LESSexp, GREATERexp, LEQexp, GEQexp, MODULUS, REMAINDER, POW }
  BinOp;

typedef enum {NOTexp, UMINUS}
  UnOp;

typedef enum { IDEXP, NUMEXP, OPEXP, STREXP, BOOLEXP, UNOEXP, FLOATEXP, PAREXP }
  ExpType;

struct _Exp {
  ExpType exp_t;    // tag
  union {           // alternatives
    char *ident;    // IDEXP
    char *string; // STREXP
    int num;        // NUMEXP
    float fnum;     //FLOATEXP
    int boolVal; // BOOLEXP
    struct {   
      struct _Exp *left;
      BinOp op;
      struct _Exp *right;
    } opexp;        // OPEXP
    struct {
      UnOp op;
      struct _Exp *child;
    } unoexp;
    struct {
      struct _Exp *inner;
    } parexp;
  } fields;
};

typedef struct _Exp *Exp;

typedef enum { ASSIGNSTM, COMPOUNDSTM, IFSTM, WHILESTM, PUTSTM, GETSTM, PROCSTM }
  StmType;

struct _Stm {
  StmType stm_t;     // tag
  union {            // alternatives
    struct {
      char *ident;
      Exp exp;
    } assign;        // ASSIGNSTM
    struct {
      struct _Stm *fst;
      struct _Stm *snd;
    } compound;      // COMPOUNDSTM
    struct {
      Exp cond;
      struct _Stm *then_branch;
      struct _Stm *else_branch;
    } ifstm;        // IFSTM
    struct {
      Exp cond;
      struct _Stm *branch;
    } whilestm;     // WHILESTM
    struct {
      Exp output;
    } putstm;       // PUT LINE STM
    struct {
      char *ident; 
    } getstm;       // GET LINE STM
    struct {
      char *name;
      struct _Stm *statements;
    } proc;
  } fields;
};




typedef struct _Stm *Stm;

extern Stm root;

extern Stm mk_compound(Stm, Stm);
extern Stm mk_assign(char *, Exp);
extern Stm mk_if(Exp, Stm, Stm);
extern Stm mk_while(Exp, Stm);
extern Stm put_line(Exp);
extern Stm get_line(char *);
extern Stm mk_proc(char *, Stm);

extern Exp mk_opexp(Exp, BinOp, Exp);
extern Exp mk_numexp(int);
extern Exp mk_floatexp(float);
extern Exp mk_idexp(char *);
extern Exp mk_strexp(char *);
extern Exp mk_boolexp(int);
extern Exp mk_unoexp(UnOp, Exp);
extern Exp mk_parexp(Exp);

extern void print_exp(Exp);
extern void print_stm(Stm);

void free_exp(Exp ptr);
void free_stm(Stm ptr);

void debug_print_exp(Exp ptr);
void debug_print_stm(Stm ptr, int indent);
void debug_print(Stm root);

#endif // AST_H