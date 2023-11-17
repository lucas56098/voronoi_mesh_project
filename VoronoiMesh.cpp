#include "VoronoiMesh.h"
#include <fstream>
#include <string>
#include <iostream>

VoronoiMesh::VoronoiMesh(Point* points, int N_seeds) {
    pts = points;
    N = N_seeds;
}

VoronoiMesh::~VoronoiMesh() {}


void VoronoiMesh::construct_mesh() {

    // loop through all seeds to generate individual cells
    for (int i = 0; i<N; i++) {

        // split points into seed and other
        Point seed = pts[i];
        Point* other_pts = new Point[N-1];

        for (int j = 0; j<N-1; j++) {
            if (i > j) {
                other_pts[j] = pts[j];
            } else {
                other_pts[j] = pts[j+1];
            }
        }

        // construct cell for given seed
        VoronoiCell vcell(seed, other_pts, N);
        vcell.construct_cell();

        // save constructed cell in vector
        vcells.push_back(vcell);
    }

}

void VoronoiMesh::save_mesh_to_files(int nr) {

    // save seeds to file

    string name = "files/seed_list" + to_string(nr) + ".csv";

    ofstream seed_list(name);

    seed_list << "seed_x,seed_y";

    for (int i = 0; i < N; i++) {
        seed_list << "\n" << pts[i].x << "," << pts[i].y;
    }

    seed_list.close();

    // save vertices to file

    name = "files/vertex_list" + to_string(nr) + ".csv";

    ofstream vertex_list(name);

    vertex_list << "vertex_x,vertex_y";

    // loop through cells
    for (int i = 0; i<vcells.size(); i++) {
        
        // loop through verticies for vertex list
        for (int j = 0; j<vcells[i].verticies.size(); j++) {
            float x = vcells[i].verticies[j].x;
            float y = vcells[i].verticies[j].y;
            vertex_list << "\n" << x << "," << y;
        }

    }

    vertex_list.close();

    // save edges to file
    name = "files/edge_list" + to_string(nr) + ".csv";

    ofstream edge_list(name);

    edge_list << "edge1_x, edge1_y, edge2_x, edge2_y";

    // loop through cells
    for (int i = 0; i<vcells.size(); i++) {
        
        int nr = vcells[i].verticies.size();

        // loop through edges
        for (int j = 0; j<nr; j++) {
            float x1 = vcells[i].verticies[j].x;
            float y1 = vcells[i].verticies[j].y;
            float x2 = vcells[i].verticies[(j+1)%nr].x;
            float y2 = vcells[i].verticies[(j+1)%nr].y;

            edge_list << "\n" << x1 << "," << y1 << "," << x2 << "," << y2;
        }
    }

    edge_list.close();

}
