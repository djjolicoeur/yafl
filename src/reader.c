#include <stdio.h>
#include <stdlib.h>
#include "yafl.h"






lval* lval_read_num(mpc_ast_t* t){
    errno = 0;
    long x = strtol(t->contents, NULL, 10);
    return errno != ERANGE ? lval_num(x): lval_err("invalid number");
}

lval* lval_read(mpc_ast_t* t){
    if(strstr(t->tag, "number")) {return lval_read_num(t);}
    if(strstr(t->tag, "symbol")) {return lval_sym(t->contents);}

    lval* x = NULL;
    if(strcmp(t->tag, ">") == 0) {x = lval_sexpr();}
    if(strstr(t->tag, "sexpr"))  {x = lval_sexpr();}
    if(strstr(t->tag, "qexpr"))  {x = lval_qexpr();}

    for(int i = 0; i < t->children_num; i++){
        if(strcmp(t->children[i]->contents, "(") == 0) {continue;}
        if(strcmp(t->children[i]->contents, ")") == 0) {continue;}
        if(strcmp(t->children[i]->contents, "{") == 0) {continue;}
        if(strcmp(t->children[i]->contents, "}") == 0) {continue;}
        if(strcmp(t->children[i]->tag, "regex") == 0) {continue;}
        x = lval_add(x, lval_read(t->children[i]));
    }

    return x;
}



void lval_expr_print(lval* v, char open, char close) {
    putchar(open);
    for (int i = 0; i < v->count; i++) {

        lval_print(v->cell[i]);

        if(i != (v->count - 1)) {
            putchar(' ');
        }
    }
    putchar(close);
}


void lval_print(lval* v){
    switch (v->type) {
    case LVAL_FUN:
        if(v->builtin){
            printf ("<function>");
        }else{
            printf("(\\ ");
            lval_print(v->formals);
            putchar(' ');
            lval_print(v->body);
            putchar(')');
        }
        break;
    case LVAL_NUM:   printf("%li", v->num);        break;
    case LVAL_ERR:   printf("Error: %s", v->err);  break;
    case LVAL_SYM:   printf("%s", v->sym);         break;
    case LVAL_SEXPR: lval_expr_print(v, '(', ')'); break;
    case LVAL_QEXPR: lval_expr_print(v, '{', '}'); break;
    }
}


void lval_println(lval* v) {lval_print(v); putchar('\n');}
