#include <iostream>
#include <random>
#include "Point.h"
#include "Halfplane.h"
#include "VoronoiCell.h"
using namespace std;

// generates seed points to use for mesh generation
Point* generate_seed_points(int N, bool fixed_random_seed) {
    Point* points = new Point[N];

    unsigned int random_seed;
    default_random_engine eng;

    // set either fixed or changing random seed
    if (fixed_random_seed) {
        //cout << "specify random_seed: ";
        //cin >> random_seed;
        random_seed = 42;
    } else {
        random_device rd;
        random_seed = rd();

    }

    // define uniform random distribution
    eng = default_random_engine(random_seed);
    uniform_real_distribution<float> distr(0, 1);

    // generate random coordinates for Points
    for (int i = 0; i < N; ++i) {
        float x = distr(eng);
        float y = distr(eng);
        points[i] = Point(x, y);
    }

    return points;
}

// generates points, will generate mesh, stop time and do tests
int main () {

    // generate seeds for mesh
    int N_seeds = 5;
    Point* pts = generate_seed_points(N_seeds, true);


// testing area 

    // for given seed: get seedpoint and other points out of all points
    int var = 0;

    Point seed = pts[var];
    Point* other_pts = new Point[N_seeds-1];

    for (int i = 0; i<N_seeds-1; i++) {
        if (var > i) {
            other_pts[i] = pts[i];
        } else {
            other_pts[i] = pts[i+1];
        }
    }

    // initalize VoronoiCell
    VoronoiCell vcell(pts[0], other_pts);    

    //Halfplane hp1(pts[0], pts[1], 3);
    //Halfplane hp2(pts[0], pts[2], 3);
    Halfplane hp1(Point(4,-3), Point(3,-4), 3);
    Halfplane hp2(Point(-7,-1), Point(-5, -5), 3);

    vcell.intersect_two_halfplanes(hp1, hp2);

    // read everyting out about that intersection
    cout << hp1.intersections[0].intersect_pt.x << endl;
    cout << hp1.intersections[0].intersect_pt.y << endl;
    cout << hp1.intersections[0].dist_to_midpoint << endl;
    cout << (*hp1.intersections[0].intersecting_with).seed1.x << endl;

    cout << "done" << endl;


}


