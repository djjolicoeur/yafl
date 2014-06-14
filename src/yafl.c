#include <stdio.h>
#include <stdlib.h>
#include <editline/readline.h>
#include "yafl.h"
//#include "mpc/mpc.h"





int main(int argc, char** argv){
    puts("yafl Version 0.0.1");
    puts("Press CTRL+C to Exit\n");
    mpc_parser_t* Number   = mpc_new("number");
    mpc_parser_t* Symbol   = mpc_new("symbol");
    mpc_parser_t* Sexpr    = mpc_new("sexpr");
    mpc_parser_t* Qexpr    = mpc_new("qexpr");
    mpc_parser_t* Expr     = mpc_new("expr");
    mpc_parser_t* Yafl    = mpc_new("yafl");
    gen_parsers(Number, Symbol, Sexpr, Qexpr, Expr, Yafl);

    lenv* e = lenv_new();
    lenv_add_builtins(e);

    while(1){
        char* input = readline("yafl> ");
        add_history(input);
        mpc_result_t r;
        if(mpc_parse("<stdin>", input, Yafl, &r)){
            //mpc_ast_print(r.output);
            //lval result = eval(r.output);
            lval* x = lval_eval(e, lval_read(r.output));
            lval_println(x);
            lval_del(x);
            mpc_ast_delete(r.output);
        }else{
            mpc_err_print(r.error);
            mpc_err_delete(r.error);
        }

        free(input);
    }

    teardown_parsers(Number, Symbol, Sexpr, Qexpr, Expr, Yafl);
    return 0;
}
