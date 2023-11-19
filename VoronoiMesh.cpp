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

// check that all neighbours know each other
bool VoronoiMesh::check_neighbours() {

    bool all_neighbours_known = false;

    int nr_of_known_neighbours = 0;
    int nr_of_checked_edges = 0;

    for (int i = 0; i < vcells.size(); i++) {
        for (int j = 0; j < vcells[i].edges.size(); j++) {
            double x_1 = vcells[i].edges[j].seed2.x;
            double y_1 = vcells[i].edges[j].seed2.y;

            if (vcells[i].edges[j].boundary == false) {
                nr_of_checked_edges += 1;
            }

            for (int k = 0; k < vcells.size(); k++) {
                if (vcells[k].seed.x == x_1 && vcells[k].seed.y == y_1) {
                    for (int l = 0; l < vcells[k].edges.size(); l++) {
                        if (vcells[k].edges[l].seed2.x == vcells[i].seed.x && vcells[k].edges[l].seed2.y == vcells[i].seed.y) {
                            nr_of_known_neighbours += 1;
                        }
                    }
                }
            }
        }
    }

    if (nr_of_checked_edges == nr_of_known_neighbours) {
        all_neighbours_known = true;
    }

    return all_neighbours_known;
}

// check some conditions for mesh
bool VoronoiMesh::check_mesh() {

    bool correct_mesh = true;

    // first check : equidistance
    bool equidist = true;
    if (!check_equidistance()) {
        correct_mesh = false;
        equidist = false;
    }
    cout << "equidistance condition: " << boolalpha << equidist << endl;
    
    // second check : area
    double total_area = check_area();
    cout << "total area = " << total_area << "+" << total_area - 1 << endl;
    if (total_area > 1 + 0.000001 && total_area < 1 - 0.000001) {
        correct_mesh = false;
    }

    // third check : neighbours

    bool neighbour = true;
    if (!check_neighbours()) {
        correct_mesh = false;
        neighbour = false;
    }
    cout << "neighbour condition: " << boolalpha << neighbour << endl;

    // return total check outcome
    return correct_mesh;
}
