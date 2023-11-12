#include <iostream>
#include <random>
#include "Point.h"
#include "VoronoiMesh.h"
//#include "Halfplane.h"
//#include "VoronoiCell.h"
//using namespace std;

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
    int N_seeds = 20;
    Point* pts = generate_seed_points(N_seeds, true);
    
    /* int N_seeds = 4;
    Point* pts = new Point[N_seeds];
    pts[0] = Point(0.5, 0.5);
    pts[1] = Point(0.5, 0.3);
    pts[2] = Point(0.2, 0.8);
    pts[3] = Point(0.8, 0.7);
    */

    // construct mesh
    VoronoiMesh vmesh(pts, N_seeds);
    vmesh.construct_mesh();

    // save mesh to file
    vmesh.save_mesh_to_files();


// testing area 

    // for given seed: get seedpoint and other points out of all points
    /*
    int var = 0;

    Point seed = pts[var];
    Point* other_pts = new Point[N_seeds-1];

    for (int i = 0; i<N_seeds-1; i++) {
        if (var > i) {
            other_pts[i] = pts[i];
        } else {
            other_pts[i] = pts[i+1];
        }
    }*/

    // initalize VoronoiCell
    //VoronoiCell vcell(seed, other_pts, N_seeds);    
    //vcell.construct_cell();


    /*
    // test specific cell
    Point seedl = Point(0.5, 0.5);
    Point* other_ptsl = new Point[2];
    other_ptsl[0] = Point(0.5, 0.3);
    other_ptsl[1] = Point(-0.5, 0.3);

    VoronoiCell vcell(seedl, other_ptsl, 3);
    vcell.construct_cell();
    vcell.print_cell();
    */
    cout << "done" << endl;
    

}


