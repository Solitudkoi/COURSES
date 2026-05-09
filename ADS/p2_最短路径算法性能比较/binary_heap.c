#include <stdio.h>
#include <stdlib.h>
#ifndef BINARY_HEAP_C
#define BINARY_HEAP_C

#define MAX 10000

typedef struct {
    int key;
    int dist;
}heapnode, *heapnodeptr;

typedef struct heap{
    int vertices;
    int size;
    heapnodeptr* heaparr;    
    int* position;
}heap, *heapptr;

heapptr initHeap(int size) {
    int* position = (int*)malloc(size * sizeof(int));
    heapnodeptr* heaparr = (heapnodeptr*)malloc((size + 1) * sizeof(heapnodeptr));

    int i;
    for (i = 0; i < size; i++) {
        position[i] = i + 1;
        heapnodeptr n = (heapnodeptr)malloc(sizeof(heapnode));
        n->key = i;
        n->dist = MAX;
        heaparr[i + 1] = n;
    }

    heapptr H = (heapptr)malloc(sizeof(heap));
    H->heaparr = heaparr;
    H->position = position;
    H->size = size;
    H->vertices = size;

    return H;
}



void changePos(heapptr h, heapnodeptr n, int pos) {
    h->heaparr[pos] = n;
    h->position[n->key] = pos;
}

void percolateUp(heapptr h, int index) {
    heapnodeptr n = h->heaparr[index];
    // heapnodeptr p = h->heaparr[index / 2];heapnodeptr p = n
    heapnodeptr p;
    for(; index > 1; index /= 2) 
	{
        p = h->heaparr[index / 2];
        if (p->dist > n->dist) {
            // h->heaparr[index] = p;
            changePos(h, p, index);
        } else {
            break;
        }
    }
    // h->heaparr[index] = n;
    changePos(h, n, index);
}

void percolateDown(heapptr h, int index) {
    heapnodeptr n = h->heaparr[index];

    // child with shorter dist
    int childindex;
    for (index; index * 2 <= h->size; index = childindex) {
        childindex = index * 2;
        heapnodeptr child = h->heaparr[index * 2];
        if (index * 2 != h->size && h->heaparr[index * 2 + 1]->dist < child->dist) {
            childindex = 2 * index + 1;
            child = h->heaparr[childindex];
        }

        if (child->dist < n->dist) {
            // h->heaparr[index] = child;
            changePos(h, child, index);
        } else {
            break;
        }
    }

    // h->heaparr[index] = n;
    changePos(h, n, index);

}

heapnodeptr deleteMin(heapptr h) {
    heapnodeptr firstnode = h->heaparr[1];
    heapnodeptr lastnode = h->heaparr[h->size];
    // h->heaparr[1] = lastnode;
    changePos(h, lastnode, 1);
    percolateDown(h, 1);
    
    h->size--;

    return firstnode;
}




// update the distance of the vertex to new_dist while maintaining the heap property
void decreaseKey(heapptr h, int node_key, int new_dist) {
    int index = h->position[node_key];
    heapnodeptr n = h->heaparr[index];
    n->dist = new_dist;
    percolateUp(h, index);
}




// debugging
void printHeap(heapptr h) {
	int i;
    for(i=1;i<=h->size;i++) 
	{
        printf("%d:%d,", h->heaparr[i]->key, h->heaparr[i]->dist);
    }
}

void printPos(heapptr h) {
	int i;
    for (i = 0; i < h->vertices; i++) {
        printf("%d:%d ", i, h->position[i]);
    }
}
#endif


// // testing
// int main(void) {
//     heapptr H = initHeap(5);

//     decreaseKey(H, 2, 200);
//     printHeap(H);
//     decreaseKey(H, 3, 300);
//     printHeap(H);
//     deleteMin(H);
//     printHeap(H);
//     deleteMin(H);
//     printHeap(H);


//     // printPos(H);
//     decreaseKey(H, 4, 10);
//     // printPos(H);
//     printHeap(H);
// }
