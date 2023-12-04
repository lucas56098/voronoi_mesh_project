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
    int boundary_cells;
private:

};

#endif