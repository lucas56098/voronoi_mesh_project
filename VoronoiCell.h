#include "Point.h"
#include "Halfplane.h"
#include <vector>

#ifndef VoronoiCell_h
#define VoronoiCell_h
//using namespace std;

class VoronoiCell {

public:
    VoronoiCell();
    VoronoiCell(Point inseed, Point* in_other_points, int N_pts);
    ~VoronoiCell();
    int N;
    Point seed;
    Point* other_points;
    vector<Halfplane> halfplanes;
    vector<Halfplane> edges;
    vector<Point> verticies;
    void intersect_all_halfplanes();
    void intersect_two_halfplanes(Halfplane &hp1, Halfplane &hp2);
    void construct_cell();
    void print_cell();
    bool check_equidistance_condition(Point* seeds);
    double get_area();
private:
    void generate_halfplane_vector();
    void search_hp_closest_to_seed(Halfplane &first_hp);
    double get_signed_angle(Point u, Point v);


};

#endif