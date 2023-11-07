#include <iostream>
#include <random>
#include "Point.h"
#include "Halfplane.h"
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
    Point* pts = generate_seed_points(20, true);


// testing area 

    Halfplane hp(pts[0], pts[1], 20);
    cout << hp.seed1.x << ":" << hp.seed1.y << endl;
    cout << hp.seed2.x << ":" << hp.seed2.y << endl;
    cout << hp.N << endl;
    cout << hp.intersections[0].dist_to_midpoint << endl;

    cout << "done" << endl;

}


