#include "ext.h"


void init_stack(stack *s){
    s->head = NULL;
    s->top = NULL;
    s->len = 0;
}

int isFull_stack(stack s){
    if(s.len >= 10){
        return 1;
    }
    return 0;
}

int isEmpty_stack(stack s) {
    if(s.top == NULL)
        return 1;
    else
        return 0;
}

void push(stack *s, struct window d){
    if(isFull_stack(*s)){
        node *m = s->head;
        s->head = s->head->right;
        free(m);
        s->len--;
    }
    
    node *p = (node *)malloc(sizeof(node));
    p->w = d;
    p->left = p->right = NULL;
    if(isEmpty_stack(*s)){
        s->head = p;
        s->top = p;
        s->len++;
        return;
    }
    s->top->right = p;
    p->left = s->top;
    s->top = s->top->right;
    s->len++;
    return;
}

node* pop(stack *s){
    if(isEmpty_stack(*s))
        return NULL;
    node *p = s->top;
    s->top = s->top->left;
    s->len--;
    return p;
}



