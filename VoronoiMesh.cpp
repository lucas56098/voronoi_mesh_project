#include "VoronoiMesh.h"
#include <fstream>
#include <string>
#include <iostream>

VoronoiMesh::VoronoiMesh(deque<Point> points, int N_seeds) {
    pts = points;
    N = N_seeds;
}

VoronoiMesh::~VoronoiMesh() {}

// construct all cells using Halfplane Intersection Algorithm
void VoronoiMesh::construct_mesh() {

    vector<int> indices;

    for (int i = 0; i < N; i++) {
        indices.push_back(i);
    }

    for (int i = 0; i < N; i++) {

        // construct individual cell and add to vcells deque        
        VoronoiCell vcell(pts[i], i, N);
        vcell.construct_cell(pts, indices);
        vcells.push_back(vcell);
    }

}


// find cell in which the point is in
int VoronoiMesh::find_cell_index(Point point) {
    
    VoronoiCell current_cell = vcells[0];
    double new_cell_index;
    bool found_cell = true;

    do {

        double current_cell_dist = sqrt((point.x - current_cell.seed.x)*(point.x - current_cell.seed.x) + (point.y - current_cell.seed.y)*(point.y - current_cell.seed.y));
        double new_cell_dist = current_cell_dist;
        found_cell = true;

        for (int i = 0; i<current_cell.edges.size(); i++) {

            int index = current_cell.edges[i].index2;

            if (index >= 0) {
                double dist = sqrt((point.x - pts[index].x)*(point.x - pts[index].x) + 
                                    (point.y - pts[index].y)*(point.y - pts[index].y));
                if (dist < new_cell_dist) {
                    new_cell_dist = dist;
                    new_cell_index = index;
                    found_cell = false;
                }
            }
        }

        current_cell = vcells[new_cell_index];

    } while (!found_cell);

    return new_cell_index;
}

// add another seed to existing voronoi mesh and adapt the mesh
void VoronoiMesh::insert_cell(Point new_seed, int new_seed_index) {

    // find index of cell where the new seed is in
    int current_cell_index = find_cell_index(new_seed);

    // vector to store the indices of points that are relevant for calculation of the cell of new_seed
    vector<int> relevant_seeds_index;

    // push back new and current index
    relevant_seeds_index.push_back(new_seed_index);
    relevant_seeds_index.push_back(current_cell_index);

    // push back indices of neighbours of current_cell
    for (int i = 0; i<vcells[current_cell_index].edges.size(); i++) {
        if (vcells[current_cell_index].edges[i].index2 >= 0) {
            relevant_seeds_index.push_back(vcells[current_cell_index].edges[i].index2);
            //cout << pts[vcells[current_cell_index].edges[i].index2].x << endl;
        }
    }

    // array of the Points relevant for calculating the cell of new_seed
    deque<Point> relevant_pts;

    // fill the array with relevant points
    relevant_pts.push_back(new_seed);
    for (int i = 1; i < relevant_seeds_index.size(); i++) {
        relevant_pts.push_back(pts[relevant_seeds_index[i]]);
    }

// just print out those for testing
    for (int i= 0; i< relevant_seeds_index.size(); i++) {
        cout << relevant_pts[i].x << ":" << relevant_pts[i].y << "   " << relevant_seeds_index[i] << endl;
    }

    // construct cell given the relevant points
    VoronoiCell vcell(relevant_pts[0], relevant_seeds_index[0], relevant_seeds_index.size());
    vcell.construct_cell(relevant_pts, relevant_seeds_index);
    vcells.push_back(vcell);

    relevant_pts.clear();

// now we need to adapt the neigbouring cells
    for (int i=1; i< relevant_seeds_index.size(); i++) {
        
// just printing some stuff
        int seed_index = relevant_seeds_index[i];
        cout << "new seed" << endl;
        cout << seed_index << endl;
        cout << "-----------" << endl;

        vector<int> indices;
        indices.push_back(new_seed_index);
        indices.push_back(relevant_seeds_index[i]);
    

        for (int j=0; j<vcells[relevant_seeds_index[i]].edges.size(); j++) {
            if (vcells[relevant_seeds_index[i]].edges[j].index2 >=0) {
                indices.push_back(vcells[relevant_seeds_index[i]].edges[j].index2);
            }
        }

// just printing some stuff for testing
        for (int j = 0; j<indices.size(); j++) {
            cout << indices[j] << endl;
        }

        // array of the Points relevant for calculating the new adapted cell
        relevant_pts.push_back(new_seed);

        for (int k = 1; k < indices.size(); k++) {
            relevant_pts.push_back(pts[indices[k]]);
        }

        for (int k=0; k < indices.size(); k++) {
            cout << relevant_pts[k].x << ":" <<  relevant_pts[k].y << endl;
        }

        // construct cell given the relevant points
        VoronoiCell vcell1(relevant_pts[1], indices[1], indices.size());
        vcell1.construct_cell(relevant_pts, indices);
        vcells[relevant_seeds_index[i]] = vcell1;

        relevant_pts.clear();

    }
    
    N += 1;
    pts.push_back(new_seed);


}

// save the mesh to files (seedfile, edgefile, vertexfile)
void VoronoiMesh::save_mesh_to_files(int nr) {

    // save seeds to file

    string name = "files/seed_list" + to_string(nr) + ".csv";

    ofstream seed_list(name);

    seed_list << "seed_x,seed_y";

    for (int i = 0; i < vcells.size(); i++) {
        seed_list << "\n" << vcells[i].seed.x << "," << vcells[i].seed.y;
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
    

    // go through each cell
    for (int i = 0; i < vcells.size(); i++) {

        // through each edge (first edge)
        for (int j = 0; j < vcells[i].edges.size(); j++) {

            // exclude boundaries
            if (vcells[i].edges[j].boundary == false) {

                nr_of_checked_edges += 1;

                // check edges (seconde edge) of neigbour cell
                for (int k = 0; k < vcells[vcells[i].edges[j].index2].edges.size(); k++) {

                    // if there is a second edge corresponding to the first edge add nr of known neigbours +1
                    if (vcells[vcells[i].edges[j].index2].edges[k].boundary == false && 
                        vcells[vcells[i].edges[j].index2].edges[k].index2 == vcells[i].edges[j].index1) {

                            nr_of_known_neighbours +=1;

                        }

                }

            }
            
        }

    }

    // check wether or not every neighbour knows its neighbour
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
