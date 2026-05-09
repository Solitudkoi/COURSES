#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#ifndef FIB_HEAP_C
#define FIB_HEAP_C

typedef struct FibNode 
{
    int key;        // vertex id
    int dist;       // distance value
    int degree;
    int mark;

    struct FibNode* parent;
    struct FibNode* child;
    struct FibNode* left;
    struct FibNode* right;
} FibNode;

typedef struct FibHeap 
{
    FibNode* min;
    int n;   // number of nodes
} FibHeap;


FibNode* make_node(int key,int dist) 
{
    FibNode* x=(FibNode*)malloc(sizeof(FibNode));
    x->key=key;
    x->dist=dist;
    x->degree=0;
    x->mark=0;

    x->parent=NULL;
    x->child=NULL;
    x->left=x;
    x->right=x;

    return x;
}

FibHeap* make_heap() 
{
    FibHeap* H=(FibHeap*)malloc(sizeof(FibHeap));
    H->min=NULL;
    H->n=0;
    return H;
}

void insert_node(FibHeap* H,FibNode* x) 
{
    if(H->min==NULL) 
	{
        H->min=x;
    } 
	else 
	{
        // insert into root list
        x->right=H->min->right;
        x->left=H->min;
        H->min->right->left=x;
        H->min->right=x;

        if(x->dist<H->min->dist) 
		{
            H->min=x;
        }
    }
    H->n++;
}

void fib_insert(FibHeap* H,int key,int dist,FibNode** position) {
    FibNode* x=make_node(key, dist);
    insert_node(H,x);
    position[key]=x;
}

static void fib_heap_link(FibHeap* H, FibNode* y, FibNode* x) {
    // remove y from root list
    y->left->right=y->right;
    y->right->left=y->left;

    // make y a child of x
    y->parent=x;
    if(x->child==NULL) 
	{
        x->child=y;
        y->left=y;
        y->right=y;
    } 
	else 
	{
        y->right=x->child->right;
        y->left=x->child;
        x->child->right->left=y;
        x->child->right=y;
    }

    x->degree++;
    y->mark=0;
}

static void consolidate(FibHeap* H) 
{
    int D=(int)(log(H->n)/log(1.618))+2;
    FibNode** A=(FibNode**)calloc(D, sizeof(FibNode*));
    int i; 
	for(i=0;i<D;i++) A[i]=NULL;

    FibNode* w=H->min;
    int numRoots=0;

    // count roots
    if(w!=NULL) 
	{
        numRoots++;
        FibNode* x;
        for(x=w->right;x!=w;x = x->right)
            numRoots++;
    }

    FibNode* x=w;
    //int i;
    for(i=0;i<numRoots;i++) 
	{
        FibNode* next=x->right;
        int d=x->degree;

        while(A[d]) 
		{
            FibNode* y=A[d];
            if(y->dist<x->dist) 
			{
                FibNode* tmp =x;x=y;y=tmp;
            }
            fib_heap_link(H,y,x);
            A[d]=NULL;
            d++;
        }
        A[d]=x;
        x=next;
    }

    H->min=NULL;
    for(i=0;i<D;i++) 
	{
        if(A[i]) 
		{
            if(H->min==NULL) 
			{
                H->min=A[i];
                H->min->left=H->min;
                H->min->right=H->min;
            } 
			else{
                // add A[i] to root list
                A[i]->right=H->min->right;
                A[i]->left=H->min;
                H->min->right->left=A[i];
                H->min->right=A[i];

                if(A[i]->dist<H->min->dist)
                    H->min=A[i];
            }
        }
    }
    free(A);
}

FibNode* fib_extract_min(FibHeap* H) 
{
    FibNode* z=H->min;

    if(z!=NULL) 
	{
        if(z->child!=NULL) 
		{
            FibNode* x=z->child;
            do{
                FibNode* next=x->right;

                // add x to root list
                x->parent=NULL;
                x->left=H->min;
                x->right=H->min->right;
                H->min->right->left=x;
                H->min->right=x;

                x=next;
            } while(x!=z->child);
        }

        // remove z from root list
        z->left->right=z->right;
        z->right->left=z->left;

        if(z==z->right) {
            H->min=NULL;
        } else {
            H->min=z->right;
            consolidate(H);
        }

        H->n-=1;
    }
    return z;
}

static void cut(FibHeap* H, FibNode* x, FibNode* y) {
    // remove x from y's children
    if (x->right == x) {
        y->child = NULL;
    } else {
        x->left->right = x->right;
        x->right->left = x->left;
        if (y->child == x)
            y->child = x->right;
    }
    y->degree--;

    // add x to root list
    x->parent = NULL;
    x->left = H->min;
    x->right = H->min->right;
    H->min->right->left = x;
    H->min->right = x;

    x->mark = 0;
}

static void cascading_cut(FibHeap* H, FibNode* y) {
    FibNode* z = y->parent;
    if (z != NULL) {
        if (!y->mark)
            y->mark = 1;
        else {
            cut(H, y, z);
            cascading_cut(H, z);
        }
    }
}

void fib_decrease_key(FibHeap* H, FibNode* x, int new_dist) {
    if (new_dist > x->dist) return;

    x->dist = new_dist;
    FibNode* y = x->parent;

    if (y != NULL && x->dist < y->dist) {
        cut(H, x, y);
        cascading_cut(H, y);
    }
    if (x->dist < H->min->dist)
        H->min = x;
}
#endif




