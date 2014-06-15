#include <stdio.h>
#include <stdlib.h>

#include "yafl.h"



char* ltype_name(int t){
    switch(t){
    case LVAL_FUN: return "Function";
    case LVAL_NUM: return "Number";
    case LVAL_ERR: return "Error";
    case LVAL_SYM: return "Symbol";
    case LVAL_STR: return "String";
    case LVAL_SEXPR: return "S-Expression";
    case LVAL_QEXPR: return "Q-Expression";
    default: return "Unknown";
    }
}

lval* builtin_op(lenv* e, lval* a, char* op){

    for (int i = 0; i < a->count; i++){
        if(a->cell[i]->type != LVAL_NUM){
            lval* e =  lval_err("invalid type '%s' for '%s'!",
                            ltype_name(a->cell[i]->type), op);
            lval_del(a);
            return e;
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

lval* builtin_comparison(lenv* e, lval* a, char* op){
    LASSERT_NUM(op, a, 2);
    LASSERT_TYPE(op, a, 0, LVAL_NUM);
    LASSERT_TYPE(op, a, 1, LVAL_NUM);

    lval* x = lval_pop(a, 0);
    lval* y = lval_pop(a, 0);

    int result;
    if(strcmp(op, ">") == 0) {result = x->num > y->num;}
    if(strcmp(op, ">=") == 0) {result = x->num >= y->num;}
    if(strcmp(op, "<") == 0) {result = x->num < y->num;}
    if(strcmp(op, "<=") == 0) {result = x->num <= y->num;}
    lval_del(a);
    return lval_num(result);
}

lval* builtin_eq(lenv* e, lval* a, char* op){
    LASSERT_NUM(op, a, 2);
    int result;
    if(strcmp(op, "==") == 0){
        result = lval_eq(a->cell[0], a->cell[1]);
    }
    if(strcmp(op, "!=") == 0){
        result = !lval_eq(a->cell[0], a->cell[1]);
    }
    lval_del(a);
    return lval_num(result);
}


lval* builtin_head(lenv* e, lval* a){
    LASSERT(a, (a->count == 1), "'head'. expected %i,  got %i", 1, a->count);
    LASSERT(a, (a->cell[0]->type == LVAL_QEXPR),
            "'head' expcted %s, but received %s",
            ltype_name(LVAL_QEXPR), ltype_name(a->cell[0]->type));
    LASSERT(a, (a->cell [0]->count != 0), "Empty %s passed to 'head'",
            ltype_name(LVAL_QEXPR));

    lval* v = lval_take(a, 0);
    while (v->count > 1) {lval_del(lval_pop(v, 1)); }
    return v;
}


lval* builtin_tail(lenv* e, lval* a){
    LASSERT(a, (a->count == 1), "'tail' expected 1 arg but got %i", a->count);
    LASSERT(a, (a->cell[0]->type == LVAL_QEXPR),
            "'tail' expcted %s, but received %s",
            ltype_name(LVAL_QEXPR), ltype_name(a->cell[0]->type));
    LASSERT(a, (a->cell [0]->count != 0), "Empty %s passed to 'tail'",
            (ltype_name(LVAL_QEXPR)));

    lval *v = lval_take(a, 0);
    lval_del(lval_pop(v, 0));
    return v;
}

lval* builtin_list(lenv* e, lval* a){
    a->type = LVAL_QEXPR;
    return a;
}


lval* builtin_eval(lenv* e, lval* a){
    LASSERT(a, (a->count == 1), "'eval' expects 1 arg, but got %i", a->count);
    LASSERT(a, (a->cell[0]->type == LVAL_QEXPR),
            "'eval' expects %s, but got %s",
            ltype_name(LVAL_QEXPR), ltype_name(a->cell[0]->type));

    lval* x = lval_take(a, 0);
    x->type = LVAL_SEXPR;
    return lval_eval(e, x);
}



lval* builtin_join(lenv* e, lval* a){
    for (int i = 0; i < a->count; i++){
        LASSERT(a, (a->cell[i]->type == LVAL_QEXPR),
                "'eval' expects %s, but got %s",
                ltype_name(LVAL_QEXPR), ltype_name(a->cell[0]->type));

    }

    lval* x = lval_pop(a, 0);

    while (a->count){
        x = lval_join(x, lval_pop(a, 0));
    }

    lval_del(a);
    return x;
}

lval* builtin_load(lenv* e, lval* a){
    LASSERT_NUM("load", a, 1);
    LASSERT_TYPE("load", a, 0, LVAL_STR);
    mpc_result_t parsed;
    if(mpc_parse_contents(a->cell[0]->str, Yafl, &parsed)){
        lval* expr = lval_read(parsed.output);
        mpc_ast_delete(parsed.output);

        while(expr->count){
            lval* x = lval_eval(e, lval_pop(expr, 0));
            if(x->type == LVAL_ERR) {lval_println(x);}
            lval_del(x);
        }

        lval_del(expr);
        lval_del(a);

        return lval_sexpr();
    }else{
        char* err_msg = mpc_err_string(parsed.error);
        mpc_err_delete(parsed.error);

        lval* err = lval_err("Library %s on fire.", err_msg);
        free(err_msg);
        lval_del(a);

        return err;
    }
}

lval* builtin_print(lenv* e, lval* a){

    for(int i = 0; i < a->count; i++){
        lval_print(a->cell[i]); putchar(' ');
    }

    putchar('\n');
    lval_del(a);

    return lval_sexpr();
}

lval* builtin_error(lenv* e, lval* a){
    LASSERT_NUM("error", a, 1);
    LASSERT_TYPE("error", a, 0, LVAL_STR);

    lval* err = lval_err(a->cell[0]->str);

    lval_del(a);
    return err;
}

lval* builtin_if(lenv* e, lval* a){
    LASSERT_NUM("if", a, 3);
    LASSERT_TYPE("if", a, 0, LVAL_NUM);
    LASSERT_TYPE("if", a, 1, LVAL_QEXPR);
    LASSERT_TYPE("if", a, 2, LVAL_QEXPR);

    lval* x;
    a->cell[1]->type = LVAL_SEXPR;
    a->cell[2]->type = LVAL_SEXPR;


    if(a->cell[0]->num){
        x = lval_eval(e, lval_pop(a, 1));
    }else{
        x = lval_eval(e, lval_pop(a, 2));
    }
    lval_del(a);
    return x;
}


lval* builtin_lambda(lenv* e, lval* a){
    LASSERT_NUM("\\", a, 2);
    LASSERT_TYPE("\\", a, 0, LVAL_QEXPR);
    LASSERT_TYPE("\\", a, 1, LVAL_QEXPR);

    for(int i = 0; i < a->cell[0]->count; i++){
        LASSERT(a, (a->cell[0]->cell[i]->type == LVAL_SYM),
                "non-symbol %s detected.  expected %s",
                ltype_name(a->cell[0]->cell[i]->type), ltype_name(LVAL_SYM));
    }

    lval* formals = lval_pop(a, 0);
    lval* body = lval_pop(a, 0);
    lval_del(a);

    return lval_lambda(formals, body);
}

lval* builtin_var(lenv* e, lval* a, char* func){
    LASSERT_TYPE(func, a, 0, LVAL_QEXPR);

    lval* syms = a->cell[0];
    for(int i = 0; i < syms->count; i++){
        LASSERT(a, (syms->cell[i]->type == LVAL_SYM),
                "'%s' expects %s, got %s",
                func, ltype_name(LVAL_SYM), ltype_name(syms->cell[i]->type));
    }

    LASSERT(a, (syms->count == a->count - 1),
            "'%s' argument binding mismath. Expected %i, got %i",
            func, a->count-1, syms->count);

    for (int i = 0; i < syms->count; i++){
        if(strcmp(func, "def") == 0) {
            lenv_def(e, syms->cell[i], a->cell[i + 1]);
        }
        if(strcmp(func, "=") == 0){
            lenv_put(e, syms->cell[i], a->cell[i + 1]);
        }
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
        return lval_err("%s must start with function", ltype_name(LVAL_SEXPR));
    }

    //lval* result = f->fun(e, v);
    lval* result = lval_call(e, f, v);
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
lval* builtin_def(lenv* e, lval* a) {return builtin_var(e, a, "def");}
lval* builtin_put(lenv* e, lval* a) {return builtin_var(e, a, "=");  }
lval* builtin_gt(lenv* e, lval* a)  {return builtin_comparison(e, a, ">");}
lval* builtin_gte(lenv* e, lval* a) {return builtin_comparison(e, a, ">=");}
lval* builtin_lt(lenv* e, lval* a) {return builtin_comparison(e, a, "<");}
lval* builtin_lte(lenv* e, lval* a) {return builtin_comparison(e, a, "<=");}
lval* builtin_eql(lenv* e, lval* a) {return builtin_eq(e, a, "==");}
lval* builtin_ne(lenv* e, lval* a) {return builtin_eq(e, a, "!=");}


void lenv_add_builtins(lenv* e){

    //Lambda (the great)
    lenv_add_builtin(e, "\\", builtin_lambda);

    //list funcs
    lenv_add_builtin(e, "list", builtin_list);
    lenv_add_builtin(e, "head", builtin_head);
    lenv_add_builtin(e, "tail", builtin_tail);
    lenv_add_builtin(e, "eval", builtin_eval);
    lenv_add_builtin(e, "join", builtin_join);

    //variables
    lenv_add_builtin(e, "def", builtin_def);
    lenv_add_builtin(e, "=", builtin_put);

    //Builtin math /num-ops
    lenv_add_builtin(e, "+", builtin_add);
    lenv_add_builtin(e, "-", builtin_sub);
    lenv_add_builtin(e, "*", builtin_mul);
    lenv_add_builtin(e, "/", builtin_div);
    lenv_add_builtin(e, "%", builtin_mod);
    lenv_add_builtin(e, ">", builtin_gt);
    lenv_add_builtin(e, ">=", builtin_gte);
    lenv_add_builtin(e, "<",  builtin_lt);
    lenv_add_builtin(e, "<=", builtin_lte);

    //equality
    lenv_add_builtin(e, "==", builtin_eql);
    lenv_add_builtin(e, "!=", builtin_ne);

    //conditionals
    lenv_add_builtin(e, "if", builtin_if);

    //utils
    lenv_add_builtin(e, "load", builtin_load);
    lenv_add_builtin(e, "print", builtin_print);
    lenv_add_builtin(e, "error", builtin_error);

}
