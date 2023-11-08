#include "Point.h"
#include <vector>
using namespace std;

#ifndef Halfplane_h
#define Halfplane_h

class Halfplane;

struct intersection
    {
        Point intersect_pt;
        Halfplane* intersecting_with;
        float dist_to_midpoint;
    };

class Halfplane {

public:
    Halfplane();
    Halfplane(Point inseed1, Point inseed2, int N_pts);
    Halfplane(Point inseed1, Point inseed2, int N_pts, bool is_boundary);
    ~Halfplane();
    Point seed1;
    Point seed2;
    int N;
    vector<intersection> intersections;
    bool boundary;
    Point get_half_plane_vec();
    Point get_midpoint();
    


private:

};

#endif