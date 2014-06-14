#include <stdio.h>
#include <stdlib.h>

#include "yafl.h"




lval* builtin_op(lenv* e, lval* a, char* op){

    for (int i = 0; i < a->count; i++){
        if(a->cell[i]->type != LVAL_NUM){
            lval_del(a);
            return lval_err("Cannot Operate on a non number!");
        }
    }

    lval* x = lval_pop(a, 0);

    if ((strcmp(op, "-") == 0) && a->count == 0) {x->num = -x->num;}

    while(a->count > 0){
        lval* y = lval_pop(a,0);

        if (strcmp(op, "+") == 0) {x->num += y->num;}
        if (strcmp(op, "-") == 0) {x->num -= y->num;}
        if (strcmp(op, "*") == 0) {x->num *= y->num;}
        if (strcmp(op, "%") == 0) {x->num %= y->num;}
        if (strcmp(op, "/") == 0) {
            if (y->num == 0) {
                lval_del(x); lval_del(y);
                x = lval_err("Division By Zero!"); break;
            }
            x->num /= y->num;
        }

        lval_del(y);
    }

    lval_del(a);
    return x;
}

lval* builtin_head(lenv* e, lval* a){
    LASSERT(a, (a->count == 1), "Too many args to 'head'");
    LASSERT(a, (a->cell[0]->type == LVAL_QEXPR), "'head' expects Qexpr");
    LASSERT(a, (a->cell [0]->count != 0), "Empty Qexpr passed to 'head'");

    lval* v = lval_take(a, 0);
    while (v->count > 1) {lval_del(lval_pop(v, 1)); }
    return v;
}


lval* builtin_tail(lenv* e, lval* a){
    LASSERT(a, (a->count == 1), "Too many args to 'tail'");
    LASSERT(a, (a->cell[0]->type == LVAL_QEXPR), "'tail' expects Qexpr");
    LASSERT(a, (a->cell [0]->count != 0), "Empty Qexpr passed to 'tail'");

    lval *v = lval_take(a, 0);
    lval_del(lval_pop(v, 0));
    return v;
}

lval* builtin_list(lenv* e, lval* a){
    a->type = LVAL_QEXPR;
    return a;
}


lval* builtin_eval(lenv* e, lval* a){
    LASSERT(a, (a->count == 1), "'eval' passed too many args");
    LASSERT(a, (a->cell[0]->type == LVAL_QEXPR),
            "'eval' passed incorrect type");

    lval* x = lval_take(a, 0);
    x->type = LVAL_SEXPR;
    return lval_eval(e, x);
}



lval* builtin_join(lenv* e, lval* a){
    for (int i = 0; i < a->count; i++){
        LASSERT(a, (a->cell[i]->type == LVAL_QEXPR),
                "'join' passed incorrect type");
    }

    lval* x = lval_pop(a, 0);

    while (a->count){
        x = lval_join(x, lval_pop(a, 0));
    }

    lval_del(a);
    return x;
}

lval* builtin_def(lenv* e, lval* a) {
    LASSERT(a, (a->cell[0]->type == LVAL_QEXPR), "'def' expects qexpr");

    lval* syms = a->cell[0];

    for (int i = 0; i < syms->count; i++){
        LASSERT(a, (syms->cell[i]->type == LVAL_SYM),
                "'def' cannot define non symbols");
    }

    LASSERT(a, (syms->count == a->count - 1),
            "'def' expects matching number of bindings");

    for(int i = 0; i < syms->count; i++){
        lenv_put(e, syms->cell[i], a->cell[i+1]);
    }

    lval_del(a);
    return lval_sexpr();
}

lval* lval_eval_sexpr(lenv* e, lval* v){

    for (int i = 0; i < v->count; i++){
        v->cell[i] = lval_eval(e, v->cell[i]);
    }

    for(int i = 0; i < v->count; i++){
        if (v->cell[i]->type == LVAL_ERR) {return lval_take(v, i);}
    }

    if(v->count == 0) {return v;}

    if(v->count == 1) {return lval_take(v, 0);}

    lval* f = lval_pop(v, 0);
    if(f->type != LVAL_FUN) {
        lval_del(f); lval_del(v);
        return lval_err("S-expression must start with function");
    }

    lval* result = f->fun(e, v);
    lval_del(f);
    return result;
}



lval* lval_eval(lenv* e, lval* v){
    if(v->type == LVAL_SYM){
        lval* x = lenv_get(e,v);
        lval_del(v);
        return x;
    }
    if(v->type == LVAL_SEXPR) {return lval_eval_sexpr(e, v);}
    return v;
}


lval* builtin_add(lenv* e, lval* a) {return builtin_op(e, a, "+");}
lval* builtin_sub(lenv* e, lval* a) {return builtin_op(e, a, "-");}
lval* builtin_mul(lenv *e, lval* a) {return builtin_op(e, a, "*");}
lval* builtin_div(lenv* e, lval* a) {return builtin_op(e, a, "/");}
lval* builtin_mod(lenv* e, lval* a) {return builtin_op(e, a, "%");}

void lenv_add_builtins(lenv* e){
    //list funcs
    lenv_add_builtin(e, "list", builtin_list);
    lenv_add_builtin(e, "head", builtin_head);
    lenv_add_builtin(e, "tail", builtin_tail);
    lenv_add_builtin(e, "eval", builtin_eval);
    lenv_add_builtin(e, "join", builtin_join);

    //variables
    lenv_add_builtin(e, "def", builtin_def);

    //Builtin math
    lenv_add_builtin(e, "+", builtin_add);
    lenv_add_builtin(e, "-", builtin_sub);
    lenv_add_builtin(e, "*", builtin_mul);
    lenv_add_builtin(e, "/", builtin_div);
    lenv_add_builtin(e, "%", builtin_mod);
}
