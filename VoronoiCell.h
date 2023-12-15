#include <deque>
#include <vector>
#include "Point.h"
#include "Halfplane.h"

#ifndef VoronoiCell_h
#define VoronoiCell_h

struct intersection
    {
        Point intersect_pt;
        Halfplane* intersecting_with;
        double dist_to_midpoint; //distance signed relative to half_plane_vec
    };

class VoronoiCell {

public:
    VoronoiCell();
    VoronoiCell(Point in_seed, int index);
    ~VoronoiCell();
    int index;
    Point seed;
    deque<Halfplane> halfplanes;
    deque<Halfplane> edges;
    deque<Point> verticies;
    void intersect_two_halfplanes(Halfplane &hp1, Halfplane &hp2, deque<intersection> &intersections);
    void construct_cell(deque<Point> pts, vector<int> indices);
    bool check_equidistance_condition(deque<Point> seeds);
    double get_area();
    void generate_halfplane_vector(deque<Point> pts, vector<int> indices);
    double get_signed_angle(Point u, Point v);
private:
    void search_hp_closest_to_seed(Halfplane &first_hp);


};

#endif