#include "VoronoiMesh.h"
#include <fstream>
#include <string>
#include <iostream>

VoronoiMesh::VoronoiMesh(deque<Point> points, int N_seeds) {
    pts = points;
    N = N_seeds;
    boundary_cells = 0;
}

VoronoiMesh::~VoronoiMesh() {}

// construct all cells using Halfplane Intersection Algorithm
void VoronoiMesh::construct_mesh() {

    vector<int> indices;

    for (int i = 0; i < pts.size(); i++) {
        indices.push_back(i);
    }

    for (int i = 0; i < pts.size(); i++) {

        // construct individual cell and add to vcells deque        
        VoronoiCell vcell(pts[i], i);
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


// add another seed into existing mesh 
// CONSTRUCTION SITE :: (until now only works if new cell doesnt get affected by the border)
void VoronoiMesh::insert_cell(Point new_seed, int new_seed_index) {

    // find cell the new seed is in
    int cell_im_in_index = find_cell_index(new_seed);
    int current_cell_index = cell_im_in_index;
    
    // generate new_cell and initial halfplane
    VoronoiCell new_cell(new_seed, new_seed_index);
    Halfplane current_hp(new_seed, vcells[cell_im_in_index].seed, new_seed_index, cell_im_in_index);
    Halfplane first_hp = current_hp;

    // set initial last_vertex
    Point last_vertex = current_hp.midpoint;
    int last_cell_index = -42;


    int counter = 0;
    do {

    // calculate distance from last_vertex to the midpoint of the current cell
    double last_vertex_dist_to_midpoint = (last_vertex.x - current_hp.midpoint.x)*current_hp.hp_vec.x
                                        + (last_vertex.y - current_hp.midpoint.y)*current_hp.hp_vec.y;

    // get intersections of current halfplane with all edges of the current cell
    deque<intersection> intersections;
    for (int i=0; i<vcells[current_cell_index].edges.size(); i++) {
        new_cell.intersect_two_halfplanes(current_hp, vcells[current_cell_index].edges[i], intersections);
    }

    double dist = 42;
    Point vertex;
    Halfplane edge_hp;
    bool need_to_check_for_degeneracy = false;

    // find the intersection with smallest but positive relative distance
    for (int i=0; i<intersections.size(); i++) {
        
        // calculate relative distance between intersection and last vertex
        double rel_dist = intersections[i].dist_to_midpoint - last_vertex_dist_to_midpoint;

        // reduce dist if rel_dist is smaller
        if (rel_dist > 0 && rel_dist < dist && !(last_cell_index == (*intersections[i].intersecting_with).index2) ) {
        
            // check for possibility of degeneracy
            if (rel_dist == dist) {
                need_to_check_for_degeneracy = true;
            }
            
            // provisionally set dist, vertex and edge_hp (will eventually be the true ones)
            dist = rel_dist;
            vertex = intersections[i].intersect_pt;
            edge_hp = *intersections[i].intersecting_with;
            

        }
    }


    // if degenerate case possible do further checks
    if (need_to_check_for_degeneracy) {
        vector<Halfplane> deg_hp_list;

        // recalculate the distances and store the ones same to dist (the minimal distance)
        for (int i = 0; i < intersections.size(); i++) {

            double rel_dist = intersections[i].dist_to_midpoint - last_vertex_dist_to_midpoint;

            if (rel_dist == dist) {
                deg_hp_list.push_back(*intersections[i].intersecting_with);
            }

        }

        // out of the degenerate halfplanes find the one with maximum signed angle
        double max_angle = 0;
        for (int i = 0; i < deg_hp_list.size(); i++) {

            // calculate signed angle
            double angle = new_cell.get_signed_angle(current_hp.hp_vec, deg_hp_list[i].hp_vec);

            // maximise
            if (angle > max_angle) {
                max_angle = angle;
                edge_hp = deg_hp_list[i];
            }
        }

    }

    // store the found edge and vertex in cell
    new_cell.edges.push_back(current_hp);
    new_cell.verticies.push_back(vertex);

    if (edge_hp.boundary) {

        // this needs to be changed!! 
        // find a way to compute the whole new_cell
        VoronoiCell vcell(new_seed, new_seed_index);
        deque<Point> new_pts = pts;

        new_pts.push_back(new_seed);

        vector<int> indices;

        for (int i = 0; i < new_pts.size(); i++) {
            indices.push_back(i);
        }

        vcell.construct_cell(new_pts, indices);

        new_cell = vcell;

        //cout << "do boundary cell with old algorithm" << endl;
        boundary_cells +=1;
        counter = 10000;

    } else {

        // some updates of variables before redoing all steps
        last_vertex = vertex;
        last_cell_index = current_cell_index;
        current_cell_index = edge_hp.index2;
        current_hp = Halfplane(new_seed, vcells[current_cell_index].seed, new_seed_index, current_cell_index);
        intersections.clear();
        counter += 1;

    }


    } while (!(first_hp.index2 == current_hp.index2) && counter < 10000);

    // store new_cell in vcells and new point in pts
    vcells.push_back(new_cell);
    pts.push_back(new_seed);

    //cout << "new cell edges: ";
    //cout << new_cell.edges.size() << endl;

    // find indices for which the cells need to be adapted
    vector<int> cells_to_adapt;

    for (int i = 0; i<new_cell.edges.size(); i++) {
        if (new_cell.edges[i].index2 >= 0) {
            cells_to_adapt.push_back(new_cell.edges[i].index2);
        }
    }


    // adapt each of the cells that need ot be adapted
    for (int i = 0; i < cells_to_adapt.size(); i++) {
        // find indices of relevant points with respect to adapting given cell
        vector<int> relevant_pt_indices;
        relevant_pt_indices.push_back(cells_to_adapt[i]);
        relevant_pt_indices.push_back(new_seed_index);

        //cout << "---" << endl;
        //cout << cells_to_adapt[i] << endl;

        //cout << vcells[cells_to_adapt[i]].edges.size() << endl;

        for (int j = 0; j < vcells[cells_to_adapt[i]].edges.size(); j++) {

            if (vcells[cells_to_adapt[i]].edges[j].index2 >= 0) {
                relevant_pt_indices.push_back(vcells[cells_to_adapt[i]].edges[j].index2);
            }

        }


        // make point list with relevant points
        deque<Point> relevant_points;
        for (int j = 0; j < relevant_pt_indices.size(); j++) {
            relevant_points.push_back(pts[relevant_pt_indices[j]]);
        }

        // construct adapted cell and store in position of old version of cell
        VoronoiCell adapted_cell(vcells[cells_to_adapt[i]].seed, cells_to_adapt[i]);
        adapted_cell.construct_cell(relevant_points, relevant_pt_indices);
        vcells[cells_to_adapt[i]] = adapted_cell;

    }

}

// perform point insertion algorithm on pts
void VoronoiMesh::do_point_insertion() {

    deque<Point> all_pts = pts;
    pts.clear();
    pts.push_back(all_pts[0]);

    construct_mesh();
    boundary_cells += 1;

    for (int i = 1; i<all_pts.size(); i++) {
        insert_cell(all_pts[i], i);
    }

    double fraction;
    fraction = static_cast<double>(boundary_cells)/static_cast<double>(all_pts.size());

    cout << "fraction of boundary cells: ";
    cout << fraction << endl;

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
