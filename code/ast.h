

typedef enum { SUM, SUB, TIMES, DIVISION, EQUAL, INEQUAL, ORexp, ANDexp, XORexp, LESSexp, GREATERexp, LEQexp, GEQexp, MODULUS, REMAINDER }
  BinOp;

typedef enum {NOTexp, UMINUS}
  UnOp;

typedef enum { IDEXP, NUMEXP, OPEXP, STREXP, BOOLEXP, UNOEXP }
  ExpType;

struct _Exp {
  ExpType exp_t;    // tag
  union {           // alternatives
    char *ident;    // IDEXP
    char *string; // STREXP
    int num;        // NUMEXP
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
  } fields;
};

typedef struct _Exp *Exp;

typedef enum { ASSIGNSTM, COMPOUNDSTM }
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
  } fields;
};

typedef struct _Stm *Stm;

extern Exp root;

extern Stm mk_compound(Stm, Stm);
extern Stm mk_assign(char *, Exp);

extern Exp mk_opexp(Exp, BinOp, Exp);
extern Exp mk_numexp(int);
extern Exp mk_idexp(char *);
extern Exp mk_strexp(char *);
extern Exp mk_boolexp(int);
extern Exp mk_unoexp(UnOp, Exp);

extern void print_exp(Exp);
extern void print_stm(Stm);
