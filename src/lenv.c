#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "yafl.h"




lenv* lenv_new(void){
    //UT_hash_handle* hh;
    lenv_table* table = NULL;
    lenv* e = malloc(sizeof(lenv));
    e->par = NULL;
    e->count = 0;
    e->syms = table;
    return e;
}


void lenv_del(lenv* e){
    lenv_table *current, *tmp;
    //UT_hash_handle* hh = e->hh;
    HASH_ITER(hh, e->syms, current, tmp){
        HASH_DELETE(hh, e->syms, current);
        lval_del(current->val);
        free(current->sym);
    }

    free(e);
}


lval* lenv_get(lenv* e, lval* k){
    lenv_table* result;
    HASH_FIND_STR(e->syms, k->sym, result);

    if(result){
        return lval_copy(result->val);
    }

    if (e->par){
        return lenv_get(e->par, k);
    }else{
        return lval_err ("unbound symbol! '%s'", k->sym);
    }

}


lenv* lenv_copy(lenv* e){
    lenv* n = malloc(sizeof(lenv));
    n = lenv_new();

    lenv_table *current, *tmp;
    n->par = e->par;
    n->count = e->count;

    HASH_ITER(hh, e->syms, current, tmp){
        char* key = malloc(strlen(current->sym) + 1);
        strcpy(key, current->sym);
        lval* value = lval_copy(current->val);
        lenv_table* v = malloc(sizeof(lenv_table));
        v->sym = key;
        v->val = value;
        HASH_ADD_KEYPTR(hh, n->syms, key, strlen(key), v);
    }

    return n;
}


void lenv_put(lenv* e, lval* k, lval* v){
    lenv_table* existing;
    UT_hash_handle* hh = e->hh;
    HASH_FIND_STR(e->syms, k->sym, existing);

    if(existing){
        char* key = malloc(strlen(k->sym) + 1);
        strcpy(key, k->sym);
        lval_del(existing->val);
        lval* val = lval_copy(v);
        HASH_DELETE(hh, e->syms, existing);
        lenv_table* new = malloc(sizeof(lenv_table));
        new->sym = key;
        new->val = val;
        HASH_ADD_KEYPTR(hh, e->syms, key, strlen(key), new);
        return;
    }

    lenv_table* new = malloc(sizeof(lenv_table));
    char* key = malloc(strlen(k->sym) + 1);
    strcpy(key, k->sym);
    lval* val = lval_copy(v);
    new->sym = key;
    new->val = val;

    HASH_ADD_KEYPTR(hh, e->syms, key, strlen(key), new);
    e->count++;
    return;
}

void lenv_def(lenv* e, lval* k, lval* v){
    while(e->par) {e = e->par;}
    lenv_put(e, k, v);
}

lval* lval_fun(lbuiltin func){
    lval* v = malloc(sizeof(lval));
    v->type = LVAL_FUN;
    v->builtin = func;
    return v;
}

void lenv_add_builtin(lenv* e, char* name, lbuiltin func){
    lval* k = lval_sym(name);
    lval* v = lval_fun(func);
    lenv_put(e, k, v);
    lval_del(k); lval_del(v);
}
