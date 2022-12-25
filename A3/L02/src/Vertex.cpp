#include "GLHeaders.h"

#include "HalfEdge.h"

int Vertex::valence() {
    int v = 0;

    /**
     * TODO: 5 compute the valence of this vertex
     */
    HalfEdge* h = this->he;
    do {
        h = h->next->twin;
        v++;
    } while (h != this->he);

    return v;
}
