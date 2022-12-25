#include "HEDS.h"

HEDS::HEDS(shared_ptr<PolygonSoup> soup)
{
    halfEdges->clear();
    faces->clear();
    faces->reserve(soup->faceList.size());
    vertices = soup->vertexList;
    for (auto &face : soup->faceList) {
		/**
		 * TODO: 2 Build the half edge data structure from the polygon soup, triangulating non-triangular faces.
		 */
        // triangle fan
        int idx0 = face[0];
        for (int i = 1; i < face.size()-1; i++) {
            int idx1 = face[i];
            int idx2 = face[i+1];
            HalfEdge *he1 = createHalfEdge(soup, idx0, idx1);
            HalfEdge *he2 = createHalfEdge(soup, idx1, idx2);
            HalfEdge *he3 = createHalfEdge(soup, idx2, idx0);
            he1->next = he2;
            he2->next = he3;
            he3->next = he1;
            faces->push_back(new Face(he1));
        }
    }
    // set vertex normals
    for (auto &v : *vertices) {
        v->n = glm::vec3(0, 0, 0);
    }
    for (auto &f : *faces) {
        f->computeNormal();
    }
    /**
     * TODO: 3 Compute vertex normals.
     */
    for (auto &v : *vertices) {
        HalfEdge* h = v->he;
        int facenum = 0;
        do {
            h = h->next->twin;
            // summing the normals of adjacent faces
            v->n += h->leftFace->n;
            facenum++;
        } while (h != v->he);
        // taking the average
        v->n /= facenum;
    }
}

HalfEdge *HEDS::createHalfEdge(shared_ptr<PolygonSoup> soup, int i, int j)
{
    std::string p = to_string(i) + "," + to_string(j);
    if (halfEdges->count(p) > 0) {
        throw runtime_error("non orientable manifold");
    }
    std::string twin = to_string(j) + "," + to_string(i);
    HalfEdge *he = new HalfEdge();
    he->head = soup->vertexList->at(j);
    he->head->he = he; // make sure the vertex has at least one half edge that points to it.
    int twinCount = halfEdges->count(twin);
    if (twinCount > 0) {
        he->twin = halfEdges->at(twin);
        he->twin->twin = he;
    }
    halfEdges->emplace(p, he);
    return he;
}

void HEDS::initHeatFlow()
{
    for (auto &v : *vertices) {
        if (v->constrained) {
            v->u0 = 1;
            v->ut = 1;
        } else {
            v->u0 = 0;
            v->ut = 0;
        }
        v->phi = 0;
    }
}

void HEDS::solveHeatFlowStep(int GSSteps, double t)
{
    // we'll naively choose some random vertex as a source, and
    // then do lots of GS iterations on that for a backward Euler solve of
    // (A-tL) u_t = u_0

    // what is a good value of t?  t small for accuracy, but t large for possibly floating point error
    for (int i = 0; i < GSSteps; i++) {
        for (auto &v : *vertices) {
            if (v->constrained) {
                continue; // do nothing for the constrained vertex!
            }
            /**
             * TODO: 7 write inner loop code for the PGS heat solve.
             */
            v->ut = 0;
            HalfEdge* h = v->he;
            for (int j = 0; j < v->valence(); j++) {
                v->ut += t * v->Lij[j] * h->twin->head->ut;
                h = h->next->twin;
            }
            v->ut = (v->u0 + v->ut) / (v->area - t*v->Lii);
        }
    }
}

void HEDS::precomputeQuantities()
{
    /**
     * TODO: you can do some pre-computation here to make things faster!
     */

    for (auto &f : *faces) {
        HalfEdge *he = f->he;
    }

    for (auto &v : *vertices) {
        v->divX = 0;
    }
}

void HEDS::updateDivx()
{
    // Compute the divergence of these normalized grad u vectors, at vertex locations
    for (auto &v : *vertices) {
        v->divX = 0;
        /**
         * TODO: 9 Update the divergence of the normalized gradients, ie., v.divX for each Vertex v.
         */
        HalfEdge* h = v->he;
        int degree = v->valence();
        double divX = 0;
        for (int i=0; i<degree; i++) {
            // outgoing edges
            glm::vec3 e1 = h->twin->head->p - h->head->p;
            glm::vec3 e2 = h->next->head->p - h->head->p;

            double theta1 = angleWithNext(h->next->next);
            double theta2 = angleWithNext(h->next);
            double cot1 = cos(theta1)/sin(theta1);
            double cot2 = cos(theta2)/sin(theta2);

            glm::vec3 Xj = - h->leftFace->gradu;

            divX += cot1*dot(e1, Xj) + cot2*dot(e2, Xj);

            h = h->next->twin;
        }
        v->divX = 0.5*divX;
    }
}

void HEDS::updateGradu() {
    // do a pass to compute face gradients of u with the current solution
    for (auto& f : *faces) {
        f->gradu[0] = 0;
        f->gradu[1] = 0;
        f->gradu[2] = 0;
        /**
         * TODO: 8 update the gradient of u from the heat values, i.e., f.gradu for each Face f.
         */
        HalfEdge* h = f->he;
        glm::vec3 normal = f->n;
        do {
            glm::vec3 oe = h->head->p - h->twin->head->p;
            glm::vec3 cp = cross(normal, oe);
            double u = h->next->head->ut;
            f->gradu[0] -= u * cp.x;
            f->gradu[1] -= u * cp.y;
            f->gradu[2] -= u * cp.z;
            h = h->next;
        } while (h != f->he);
        f->gradu = normalize(f->gradu);
    }
}

void HEDS::solveDistanceStep(int GSSteps)
{
    // Finally step the solution to the distance problem
    for (int i = 0; i < GSSteps; i++) {
        for (auto &v : *vertices) {
            // LHS matrix is L, so to take all the LHS to the RSH for one variable we get
            // Lii phi_i = div X + sum_{j!=i} tLij phi_j
            /**
             * TODO: 10 Implement the inner loop of the Gauss-Seidel solve to compute the distances to each vertex, phi.
             */
        }
    }

    // Note that the solution to step III is unique only up to an additive constant,
    // final values simply need to be shifted such that the smallest distance is zero. 
    // We also identify the max phi value here to identify the maximum geodesic and to 
    // use adjusting the colour map for rendering
    minphi = DBL_MAX;
    maxphi = DBL_MIN;
    for (auto &v : *vertices) {
        if (v->phi < minphi)
            minphi = v->phi;
        if (v->phi > maxphi)
            maxphi = v->phi;
    }
    maxphi -= minphi;
    for (auto& v : *vertices) {
        v->phi -= minphi;
    }
}

void HEDS::computeLaplacian()
{
    for (auto &v : *vertices) {
        v->area = 0;
        v->Lii = 0;
        // get degree of v
        int degree = v->valence();
        v->Lij = new double[degree];

        /**
         * TODO: 6 Compute the Laplacian and store as vertex weights, and cotan operator diagonal Lii and off diagonal Lij terms. 
         */
        HalfEdge* h = v->he;
        for (int i=0; i<degree; i++) {
            double aij = angleWithNext(h->next);
            double bij = angleWithNext(h->twin->next);
            double cota = cos(aij)/sin(aij);
            double cotb = cos(bij)/sin(bij);
            double wij = 0.5 * (cota + cotb);
            v->Lij[i] = wij;
            v->Lii -= wij;
            h = h->next->twin;
        }
    }
}

double HEDS::angleWithNext(HalfEdge *he)
{
    /**
     * TODO: 6 Implement this function to compute the angle with next edge... you'll want to use this in a few places.
     */
    double angle;
    glm::vec3 v0 = he->head->p;
    glm::vec3 v1 = he->next->head->p;
    glm::vec3 v2 = he->next->next->head->p;
    angle = acos(dot(normalize(v1-v0), normalize(v2-v0)));
    return angle;
}
