#include "mpc/mpc.h"

#define LASSERT(args, cond, fmt, ...)             \
    if(!(cond)) {                                 \
        lval* err = lval_err(fmt, ##__VA_ARGS__); \
        lval_del(args);                           \
        return err;                               \
    }

#define LASSERT_TYPE(func, args, index, expect)                           \
    LASSERT(args, args->cell[index]->type == expect,                      \
      "'%s' passed incorrect type for argument %i. Got %s, Expected %s.", \
    func, index, ltype_name(args->cell[index]->type), ltype_name(expect))

#define LASSERT_NUM(func, args, num)     \
  LASSERT(args, args->count == num,      \
    "'%s' expected %i args.  got %i.",   \
          func, num, args->count)

#define LASSERT_NOT_EMPTY(func, args, index)                  \
  LASSERT(args, args->cell[index]->count != 0,                \
    "'%s' passed {} for argument %i.", func, index);

//Type enums
enum {LVAL_ERR, LVAL_NUM,
      LVAL_SYM, LVAL_SEXPR,
      LVAL_QEXPR, LVAL_FUN, LVAL_STR};

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
    char* str;
    lbuiltin builtin;
    lenv* env;
    lval* formals;
    lval* body;


    int count;
    lval** cell;
};

struct lenv{
    lenv* par;
    int count;
    char** syms;
    lval **vals;
};


lenv* lenv_new(void);

lval* lval_add(lval* v, lval* x);

int lval_eq(lval* x, lval* y);

lval* lval_num(long x);

lval* lval_err(char* fmt, ...);

lval* lval_sym(char* s);

lval* lval_str(char* s);

lval* lval_sexpr(void);

lval* lval_qexpr(void);

lval* lval_lambda(lval* formals, lval* body);

void lval_del(lval* v);

lval* lval_copy(lval* v);

lval* lval_pop(lval* v, int i);

lval* lval_take(lval* v, int i);

lval* lval_join(lval* x, lval* y);

lval* lval_call(lenv* e, lval* f, lval* a);

lenv* lenv_new(void);

lenv* lenv_copy(lenv* e);

void lenv_del(lenv* e);

lval* lenv_get(lenv* e, lval* k);

void lenv_put(lenv *e, lval* k, lval* v);

void lenv_def(lenv* e, lval* k, lval* v);

char* ltype_name(int t);

lval* lval_fun(lbuiltin func);

lval* builtin_op(lenv* e, lval* a, char* op);

lval* builtin_head(lenv* e, lval* a);

lval* builtin_tail(lenv* e, lval* a);

lval* builtin_list(lenv* e, lval* a);

lval* builtin_eval(lenv* e, lval* a);

lval* builtin_join(lenv* e, lval* a);

lval* builtin_def(lenv* e, lval* a);

lval* builtin_load(lenv* e, lval* a);

lval* builtin_print(lenv* e, lval* a);

lval* builtin_error(lenv* e, lval* a);

void lenv_add_builtins(lenv* e);

lval* lval_eval_sexpr(lenv* e, lval* v);

lval* lval_eval(lenv* e, lval* v);

lval* builtin_add(lenv* e, lval* a);

lval* builtin_sub(lenv* e, lval* a);

lval* builtin_mul(lenv *e, lval* a);

lval* builtin_div(lenv* e, lval* a);

lval* builtin_mod(lenv* e, lval* a);

lval* builtin_var(lenv* e, lval* a, char* func);

void lenv_add_builtin(lenv* e, char* name, lbuiltin func);

void lenv_add_builtins(lenv* e);

lval* lval_read_num(mpc_ast_t* t);

lval* lval_read_str(mpc_ast_t* t);

lval* lval_read(mpc_ast_t* t);

void lval_expr_print(struct lval* v, char open, char close);

void lval_print(struct lval* v);

void lval_println(struct lval* v);

mpc_parser_t* Number;
mpc_parser_t* Symbol;
mpc_parser_t* String;
mpc_parser_t* Comment;
mpc_parser_t* Sexpr;
mpc_parser_t* Qexpr;
mpc_parser_t* Expr;
mpc_parser_t* Yafl;


/*void gen_parsers(mpc_parser_t* Number,
                 mpc_parser_t* Symbol,
                 mpc_parser_t* String,
                 mpc_parser_t* Comment,
                 mpc_parser_t* Sexpr,
                 mpc_parser_t* Qexpr,
                 mpc_parser_t* Expr,
                 mpc_parser_t* Yafl);
*/

void gen_parsers();

/*
void teardown_parsers(mpc_parser_t* Number,
                      mpc_parser_t* Symbol,
                      mpc_parser_t* String,
                      mpc_parser_t* Comment,
                      mpc_parser_t* Sexpr,
                      mpc_parser_t* Qexpr,
                      mpc_parser_t* Expr,
                      mpc_parser_t* Yafl);
*/

void teardown_parsers();
