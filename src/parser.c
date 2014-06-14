#include <stdio.h>
#include <stdlib.h>
#include "yafl.h"



void gen_parsers(mpc_parser_t* Number,
                 mpc_parser_t* Symbol,
                 mpc_parser_t* Sexpr,
                 mpc_parser_t* Qexpr,
                 mpc_parser_t* Expr,
                 mpc_parser_t* Yafl){
    mpca_lang (MPCA_LANG_DEFAULT,
               "                                                             \
           number   : /-?[0-9]+/ ;                                \
           symbol   : /[a-zA-Z0-9_+\\-*\\/\\\\=<>!&]+/ ;          \
           sexpr    : '(' <expr>* ')' ;                           \
           qexpr    : '{' <expr>* '}' ;                           \
           expr     : <number> | <symbol> | <sexpr> | <qexpr> ;   \
           yafl    : /^/ <expr>* /$/ ;                           \
     ",
               Number, Symbol, Sexpr, Qexpr, Expr, Yafl);
}

void teardown_parsers(mpc_parser_t* Number,
                 mpc_parser_t* Symbol,
                 mpc_parser_t* Sexpr,
                 mpc_parser_t* Qexpr,
                 mpc_parser_t* Expr,
                 mpc_parser_t* Yafl) {
    mpc_cleanup(6, Number, Symbol, Sexpr, Qexpr, Expr, Yafl);
}
