#include "Point.h"

#ifndef Halfplane_h
#define Halfplane_h

class Halfplane {

public:
    Halfplane();
    Halfplane(Point inseed1, Point inseed2, int N_pts);
    ~Halfplane();
    Point seed1;
    Point seed2;
    int N;
    struct intersection
    {
        Point intersect_pt;
        Point* intersecting_with;
        float dist_to_midpoint;
    };
    intersection* intersections;
    


private:

};

#endif