#include "mpc/mpc.h"

#define LASSERT(args, cond, err) if(!(cond)) {lval_del(args); return lval_err(err);}

//Type enums
enum {LVAL_ERR, LVAL_NUM,
      LVAL_SYM, LVAL_SEXPR,
      LVAL_QEXPR, LVAL_FUN};

struct lval;

struct lenv;

typedef struct lenv lenv;
typedef struct lval lval;

typedef lval*(*lbuiltin)(lenv*, lval*);

struct lval{
    int type;
    long num;

    char* err;
    char* sym;
    lbuiltin fun;

    int count;
    struct lval** cell;
};

struct lenv{
    int count;
    char** syms;
    lval **vals;
};


lenv* lenv_new(void);

lval* lval_add(lval* v, lval* x);

lval* lval_num(long x);

lval* lval_err(char* m);

lval* lval_sym(char* s);

lval* lval_sexpr(void);

lval* lval_qexpr(void);

void lval_del(lval* v);

lval* lval_copy(lval* v);

lval* lval_pop(lval* v, int i);

lval* lval_take(lval* v, int i);

lval* lval_join(lval* x, lval* y);

lenv* lenv_new(void);

void lenv_del(lenv* e);

lval* lenv_get(lenv* e, lval* k);

void lenv_put(lenv *e, lval* k, lval* v);

lval* lval_fun(lbuiltin func);

lval* builtin_op(lenv* e, lval* a, char* op);

lval* builtin_head(lenv* e, lval* a);

lval* builtin_tail(lenv* e, lval* a);

lval* builtin_tail(lenv* e, lval* a);

lval* builtin_eval(lenv* e, lval* a);

lval* builtin_join(lenv* e, lval* a);

lval* builtin_def(lenv* e, lval* a);

void lenv_add_builtins(lenv* e);

lval* lval_eval_sexpr(lenv* e, lval* v);

lval* lval_eval(lenv* e, lval* v);

lval* builtin_add(lenv* e, lval* a);

lval* builtin_sub(lenv* e, lval* a);

lval* builtin_mul(lenv *e, lval* a);

lval* builtin_div(lenv* e, lval* a);

lval* builtin_mod(lenv* e, lval* a);

void lenv_add_builtin(lenv* e, char* name, lbuiltin func);

void lenv_add_builtins(lenv* e);

lval* lval_read_num(mpc_ast_t* t);

lval* lval_read(mpc_ast_t* t);

void lval_expr_print(struct lval* v, char open, char close);

void lval_print(struct lval* v);

void lval_println(struct lval* v);


void gen_parsers(mpc_parser_t* Number,
                 mpc_parser_t* Symbol,
                 mpc_parser_t* Sexpr,
                 mpc_parser_t* Qexpr,
                 mpc_parser_t* Expr,
                 mpc_parser_t* Yafl);


void teardown_parsers(mpc_parser_t* Number,
                      mpc_parser_t* Symbol,
                      mpc_parser_t* Sexpr,
                      mpc_parser_t* Qexpr,
                      mpc_parser_t* Expr,
                      mpc_parser_t* Yafl);
