#include <stdio.h>
#include <stdlib.h>
#include "yafl.h"


mpc_parser_t* Number;
mpc_parser_t* Symbol;
mpc_parser_t* String;
mpc_parser_t* Comment;
mpc_parser_t* Sexpr;
mpc_parser_t* Qexpr;
mpc_parser_t* Expr;
mpc_parser_t* Yafl;


void gen_parsers(){
    Number   = mpc_new("number");
    Symbol   = mpc_new("symbol");
    String   = mpc_new("string");
    Comment  = mpc_new("comment");
    Sexpr    = mpc_new("sexpr");
    Qexpr    = mpc_new("qexpr");
    Expr     = mpc_new("expr");
    Yafl    = mpc_new("yafl");

    mpca_lang (MPCA_LANG_DEFAULT,
    "                                                             \
           number   : /-?[0-9]+/ ;                                \
           symbol   : /[a-zA-Z0-9_+\\-*\\/\\\\=<>!&]+/ ;          \
           string   : /\"(\\\\.|[^\"])*\"/ ;                      \
           comment  : /;[^\\r\\n]*/ ;                             \
           sexpr    : '(' <expr>* ')' ;                           \
           qexpr    : '{' <expr>* '}' ;                           \
           expr     : <number> | <symbol> | <string> | <comment>  \
                    |  <sexpr> | <qexpr> ;                        \
           yafl    : /^/ <expr>* /$/ ;                            \
     ",
               Number, Symbol, String, Comment, Sexpr, Qexpr, Expr, Yafl);
}

void teardown_parsers()
{
    mpc_cleanup(8, Number, Symbol, String, Comment, Sexpr, Qexpr, Expr, Yafl);
}
