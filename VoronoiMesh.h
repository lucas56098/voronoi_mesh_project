#include "VoronoiCell.h"
#include "Point.h"
#include <vector>

#ifndef VoronoiMesh_h
#define VoronoiMesh_h

class VoronoiMesh {

public:
    VoronoiMesh(Point* points, int N_seeds);
    ~VoronoiMesh();
    Point * pts;
    vector<VoronoiCell> vcells;
    int N;
    void construct_mesh();
    void save_mesh_to_files();
private:

};

#endif