#include <stdio.h>
#include <stdlib.h>
#ifndef GRAPH_C
#define GRAPH_C

typedef struct edge{
    int adjv;
    int weight;
    struct edge* next;
}edge, *edgeptr;

typedef struct {
    edgeptr* edges;
    int e;  
    int v;
}graph, *graphptr;


// Given approximate size, initialize a graph
graphptr initGraph(int size) {
    edgeptr* edges = (edgeptr*)malloc(size * sizeof(edgeptr));
    int i;
    for ( i = 0; i < size; i++) {
        edgeptr dum = (edgeptr)malloc(sizeof(edge));
        dum->next = NULL;
        edges[i] = dum;
    }

    graphptr G = (graphptr)malloc(sizeof(graph));
    G->edges = edges;
    G->v = 0;
    G->e = 0;

    return G;
}


// add new edge to the graph
void addEdge(graphptr G, int src, int dst, int weight) {

    // create the new edge
    edgeptr new_edge = (edgeptr)malloc(sizeof(edge));
    new_edge->adjv = dst;
    new_edge->weight = weight;
    new_edge->next = NULL;

    // add the new edge to graph
    edgeptr adj_of_src = G->edges[src];
    edgeptr p = adj_of_src;


    // update number of vertices ??
    if(src + 1 > G->v) {
        G->v = src + 1;
    }

    if(dst + 1 > G->v) {
        G->v = dst + 1;
    }


    // concatenate 
    new_edge->next = p->next;
    p->next = new_edge;

    // update number of edges
    G->e++;

}


// create graph struct from external file
// filename: nust be absolute path
graphptr graphFromFile(const char* filename, int app_size) {
    FILE* file;
    file = fopen(filename, "r");
    if (!file) {
        printf("Error: Could not open file %s\n", filename);
        return NULL;
    }


    graphptr G = initGraph(app_size);

    char line[256];
    
    while (fgets(line, sizeof(line), file)) {
        int src, dest, weight;
        if (line[0] == 'a') {
            sscanf(line, "a %d %d %d", &src, &dest, &weight);
            addEdge(G, src - 1, dest - 1, weight);  // start from 0
        } else if (line[0] >= '0' && line[0] <= '9') {
            sscanf(line, "%d %d %d", &src, &dest, &weight);
            addEdge(G, src - 1, dest - 1, weight);
        }
    }

    fclose(file);
    return G;
}

// debugging
void printGraph(graphptr G) {
    edgeptr* edges = G->edges;
    int i;
    for (i = 0; i < G->v; i++) {
        printf("%d: ", i);
        edgeptr e = edges[i];
        edgeptr p = e;
        while(p->next) {
            p = p->next;
            printf("%d(%d) ", p->adjv, p->weight);
        }
        printf("\n");
    }
}
#endif


// // testing
// int main (void) {
//     graphptr G = initGraph(10);
//     addEdge(G, 0, 1, 2);
//     addEdge(G, 1, 0, 2);
//     addEdge(G, 0, 2, 3);
//     addEdge(G, 2, 0, 3);

//     printGraph(G);
// }
