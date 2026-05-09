#include <stdio.h>
#include <stdlib.h>
#ifndef DIJKSTRA_FIB_C
#define DIJKSTRA_FIB_C

#include "fib_heap.c"
#include "graph.c"

// Dijkstra using Fibonacci Heap
int* dijkstra_fib(graphptr G, int s) {
    int n=G->v;
    int* dist=(int*)malloc(n*sizeof(int));
    FibNode** pos=(FibNode**)malloc(n*sizeof(FibNode*));
    FibHeap* H=make_heap();
	int i;
    for(i=0;i<n;i++) 
	{
        dist[i]=1e9;
        fib_insert(H,i,dist[i],pos);
    }

    // initialize s
    dist[s]=0;
    fib_decrease_key(H,pos[s],0);

    while(H->n>0) 
	{
        FibNode* mn=fib_extract_min(H);
        int u=mn->key;
        int d=mn->dist;
        dist[u]=d;

        // relax adjacency
        edgeptr p=G->edges[u]->next;
        while(p) 
		{
            int v=p->adjv;
            int w=p->weight;

            if(d+w<pos[v]->dist) 
			{
                fib_decrease_key(H,pos[v],d+w);
            }
            p=p->next;
        }
    }
    return dist;
}

#endif
