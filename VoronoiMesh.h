#include "VoronoiCell.h"
#include "Point.h"
#include <deque>

#ifndef VoronoiMesh_h
#define VoronoiMesh_h

class VoronoiMesh {

public:
    VoronoiMesh(deque<Point> points, int N_seeds);
    ~VoronoiMesh();
    deque<Point> pts;
    int N;
    deque<VoronoiCell> vcells;
    void construct_mesh();
    void insert_cell(Point new_seed, int new_seed_index);
    void save_mesh_to_files(int nr);
    bool check_equidistance();
    double check_area();
    bool check_neighbours();
    bool check_mesh();
    void do_point_insertion();
    int find_cell_index(Point point);
private:
    void find_smallest_pos_intersect(Halfplane &current_hp, int &current_cell_index, VoronoiCell &new_cell, Point &last_vertex, 
                                        int &last_cell_index, Point &vertex, Halfplane &edge_hp);
    int get_edge_index_in_cell(int &edge_index, VoronoiCell &vcell);
    bool intersection_between_start_stop(int index, Point &new_seed, int &new_seed_index, int &next_cell_index, 
                                            VoronoiCell &new_cell);

};

#endif