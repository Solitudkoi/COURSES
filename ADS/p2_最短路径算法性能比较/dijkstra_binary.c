#include <stdio.h>
#include <stdlib.h>
#ifndef DIJKSTRA_BINARY_C
#define DIJKSTRA_BINARY_C


#include "binary_heap.c"
#include "graph.c"


// Given a node whose shortest dist is known, relax the heap 
void relaxation(int key, int dist, heapptr H, graphptr G) {
    edgeptr advnodes = G->edges[key];
    edgeptr p = advnodes;

    while(p->next) {
        edgeptr e = p->next;
        int adjv = e->adjv;
        int weight = e->weight;

        heapnodeptr hn = H->heaparr[H->position[adjv]];
        int cur_dist = hn->dist;

        if (dist + weight < cur_dist) {
            decreaseKey(H, adjv, dist + weight);
        }

        p = p->next;    // !!!
    }
}

int* shortest_path(graphptr G, int s) {
    int v = G->v;

    int* shortest_dist = (int*)malloc(v * sizeof(int));

    int i;

    // setup
    int* known = (int*)malloc(v * sizeof(int));
    for (i = 0; i < v; i++) {
        known[i] = 0;
    }

    heapptr H = initHeap(v);
    decreaseKey(H, s, 0);


    // iteractively 
    for (i = 0; i < v; i++) {
        heapnodeptr min_node = deleteMin(H);
        int min_key = min_node->key;
        shortest_dist[min_key] = min_node->dist;
        relaxation(min_key, min_node->dist, H, G);
    }

    return shortest_dist;
}

#endif

// // testing
// int main(void) {
//     graphptr G = initGraph(5);
//     addEdge(G, 0, 1, 1);
//     addEdge(G, 0, 2, 2);
//     addEdge(G, 0, 3, 3);
//     addEdge(G, 1, 4, 10);

//     printGraph(G);

//     int* dist = shortest_path(G, 0);

//     for (int i = 0; i < 5; i++){
//         printf("%d:%d ", i, dist[i]);
//     }
    
//     return 0;
// }
