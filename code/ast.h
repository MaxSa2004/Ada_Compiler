

typedef enum { PLUS, MINUS, TIMES, DIV }
  BinOp;

typedef enum { IDEXP, NUMEXP, OPEXP }
  ExpType;

struct _Exp {
  ExpType exp_t;    // tag
  union {           // alternatives
    char *ident;    // IDEXP
    int num;        // NUMEXP
    struct {   
      struct _Exp *left;
      BinOp op;
      struct _Exp *right;
    } opexp;        // OPEXP
  } fields;
};

typedef struct _Exp *Exp;

typedef enum { ASSIGNSTM, INCRSTM, COMPOUNDSTM }
  StmType;

struct _Stm {
  StmType stm_t;     // tag
  union {            // alternatives
    struct {
      char *ident;
      Exp exp;
    } assign;        // ASSIGNSTM
    char *incr;      // INCRSTM
    struct {
      struct _Stm *fst;
      struct _Stm *snd;
    } compound;      // COMPOUNDSTM
  } fields;
};

typedef struct _Stm *Stm;


extern Stm mk_compound(Stm, Stm);
extern Stm mk_assign(char *, Exp);
extern Stm mk_incr(char *);

extern Exp mk_opexp(Exp, BinOp, Exp);
extern Exp mk_numexp(int);
extern Exp mk_idexp(char *);

extern void print_exp(Exp);
extern void print_stm(Stm);
