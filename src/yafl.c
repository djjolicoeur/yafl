#include <stdio.h>
#include <stdlib.h>
#include <editline/readline.h>
#include "yafl.h"
//#include "mpc/mpc.h"





int main(int argc, char** argv){
    puts("yafl Version 0.0.1");
    puts("Press CTRL+C to Exit\n");

    gen_parsers();
    lenv* e = lenv_new();
    lenv_add_builtins(e);
    lval* core = lval_add(lval_sexpr(), lval_str("src/yfl/core.yfl"));
    //printf("gets here");
    lval* core_loaded = builtin_load(e, core);

    if(core_loaded->type == LVAL_ERR) {lval_println(core_loaded);}
    lval_del(core_loaded);

    if(argc >= 2){ //execute file
        for(int i = 1; i < argc; i++){

            lval* args = lval_add(lval_sexpr(), lval_str(argv[i]));

            lval* x = builtin_load(e, args);

            if(x->type == LVAL_ERR) { lval_println(x); }
            lval_del(x);
        }
    }
    else{ //Start REPL

        while(1){
            char* input = readline("yafl> ");
            add_history(input);
            mpc_result_t r;
            if(mpc_parse("<stdin>", input, Yafl, &r)){

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
    }

    teardown_parsers();
    return 0;
}
