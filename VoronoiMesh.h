#include "VoronoiCell.h"
#include "Point.h"
#include <deque>

#ifndef VoronoiMesh_h
#define VoronoiMesh_h

class VoronoiMesh {

public:
    VoronoiMesh(Point* points, int N_seeds);
    ~VoronoiMesh();
    Point * pts;
    int N;
    deque<VoronoiCell> vcells;
    void construct_mesh();
    void save_mesh_to_files(int nr);
    bool check_equidistance();
    double check_area();
    bool check_neighbours();
    bool check_mesh();
private:

};

#endif