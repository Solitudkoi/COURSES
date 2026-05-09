#include <stdio.h>
#include <stdlib.h>

#include "graph.c"
#include "binary_heap.c"
#include "fib_heap.c"
#include "dijkstra_binary.c"
#include "dijkstra_fib.cpp"

graphptr small_graph() {
    const char* filename = "E:\\p2\\small_gr.txt";
    graphptr G = graphFromFile(filename, 3);

    printGraph(G);
    return G;
}

graphptr ny_graph() {
    const char* filename = "E:\\p2\\USA-road-d.NY.gr";
    graphptr G = graphFromFile(filename, 270000);


    // debugging
    edgeptr adjnodes = G->edges[0];
    edgeptr p = adjnodes;
    while(p->next) {
        p = p->next;
        printf("%d(%d) ", p->adjv, p->weight);
    }

    return G;
}

graphptr estern_usa_graph() {
    const char* filename = "E:\\p2\\USA-road-d.E.gr";
    graphptr G = graphFromFile(filename, 4000000);

    // debugging
    edgeptr adjnodes = G->edges[0];
    edgeptr p = adjnodes;
    while(p->next) {
        p = p->next;
        printf("%d(%d) ", p->adjv, p->weight);
    }

    return G;
}

void test_small_fib() {
    graphptr G = small_graph();
    int* d = dijkstra_fib(G, 0);
    int i;  
    for (i = 0; i < G->v; i++)
        printf("%d:%d ", i, d[i]);
}

void test_ny_fib() {
    graphptr G = ny_graph();
    int i;
    for ( i = 0; i < 1000; i++) {
        int s = rand() % G->v;
        int t = rand() % G->v;
        int* d = dijkstra_fib(G, s);
        printf("Query %d: dist(%d -> %d) = %d\n", i, s, t, d[t]);
        free(d);
    }
}

void test_eastern_usa_fib() {
    graphptr G = estern_usa_graph();
    int i; 
    for(i = 0; i < 1000; i++) {
        int s = rand() % G->v;
        int t = rand() % G->v;
        int* d = dijkstra_fib(G, s);
        printf("Query %d: shortest path %d -> %d = %d\n",
               i, s, t, d[t]);
        free(d);
    }
}

int main(void) {
    // test_ny();
    // test_estern_usa();
    test_eastern_usa_fib();
    //test_ny_fib(); 
    //test_small_fib();
}







