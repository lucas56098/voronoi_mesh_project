#include "VoronoiMesh.h"
#include <fstream>
#include <string>
#include <iostream>

VoronoiMesh::VoronoiMesh(Point* points, int N_seeds) {
    pts = points;
    N = N_seeds;
}

VoronoiMesh::~VoronoiMesh() {}

// construct all cells
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

// save the mesh to files (seedfile, edgefile, vertexfile)
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
            double x = vcells[i].verticies[j].x;
            double y = vcells[i].verticies[j].y;
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
            double x1 = vcells[i].verticies[j].x;
            double y1 = vcells[i].verticies[j].y;
            double x2 = vcells[i].verticies[(j+1)%nr].x;
            double y2 = vcells[i].verticies[(j+1)%nr].y;

            edge_list << "\n" << x1 << "," << y1 << "," << x2 << "," << y2;
        }
    }

    edge_list.close();

}

// check equidistance
bool VoronoiMesh::check_equidistance() {
    bool correct_mesh = true;

    // check every cell individually
    for (int i = 0; i < vcells.size(); i++) {

        // check cell for its conditions
        bool correct_cell = vcells[i].check_equidistance_condition(pts);

        // if a single cell is false the mesh is also false
        if (!correct_cell) {
            correct_mesh = false;
        }
    }

    return correct_mesh;
}

// add up areas of all cells
double VoronoiMesh::check_area() {

    double total_area = 0;

    for (int i = 0; i < vcells.size(); i++) {
        total_area += vcells[i].get_area();
    }

    return total_area;

}

// check some conditions for mesh -> for conditions look at VoronoiCell::check_cell()
bool VoronoiMesh::check_mesh() {

    bool correct_mesh = true;

    // first check
    if (!check_equidistance()) {
        correct_mesh = false;
    }
    cout << "equidistance condition: " << boolalpha << correct_mesh << endl;
    cout << "total area = " << check_area() << endl;

    // possible futher checks here

    // return total check outcome
    return correct_mesh;
}
