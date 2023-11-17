#include <iostream>
#include <random>
#include "Point.h"
#include "VoronoiMesh.h"
#include <chrono>
//#include "Halfplane.h"
//#include "VoronoiCell.h"
//using namespace std;

// generates seed points to use for mesh generation
Point* generate_seed_points(int N, bool fixed_random_seed, int min, int max, int rd_seed) {
    Point* points = new Point[N];

    unsigned int random_seed;
    default_random_engine eng;

    // set either fixed or changing random seed
    if (fixed_random_seed) {
        //cout << "specify random_seed: ";
        //cin >> random_seed;
        random_seed = rd_seed;
    } else {
        random_device rd;
        random_seed = rd();

    }

    // define uniform random distribution
    eng = default_random_engine(random_seed);
    uniform_real_distribution<double> distr(min, max);

    // generate random coordinates for Points
    for (int i = 0; i < N; ++i) {
        double x = distr(eng);
        double y = distr(eng);
        points[i] = Point(x, y);
    }

    return points;
}

// generates moving mesh and stores it frame by frame in files
void generate_animation_files() {
    
    // generate initial points and velocities for mesh
    int N_seeds = 30;
    Point* pts = generate_seed_points(N_seeds, true, 0, 1, 42);
    Point* vel = generate_seed_points(N_seeds, true, -1, 1, 38);

    // for each frame generate mesh and store it in files
    for (int i = 0; i < 300; i++) {
        
        // update all particles positions
        for (int j = 0; j < N_seeds; j++) {

            // update positions according to velocity
            pts[j].x = pts[j].x + vel[j].x * 0.005;
            pts[j].y = pts[j].y + vel[j].y * 0.005;

            // change velocities at boundary
            if (pts[j].x < 0 || pts[j].x > 1) {
                vel[j].x = -vel[j].x;
                pts[j].x = pts[j].x + 2 * vel[j].x * 0.005;
            }
            if (pts[j].y < 0 || pts[j].y > 1) {
                vel[j].y = -vel[j].y;
                pts[j].y = pts[j].y + 2 * vel[j].y * 0.005;
            }

        }
        

        // construct mesh
        VoronoiMesh vmesh(pts, N_seeds);
        vmesh.construct_mesh();
        vmesh.save_mesh_to_files(i);
        cout << i << endl;

    }

}

// generates points, will generate mesh, stop time and do tests
int main () {

    // generate seeds for mesh
    int N_seeds = 100;
    Point* pts = generate_seed_points(N_seeds, true, 0, 1, 42);
    
    // Get the current time point before the code execution
    chrono::high_resolution_clock::time_point start_time = chrono::high_resolution_clock::now();

    // construct mesh
    VoronoiMesh vmesh(pts, N_seeds);
    vmesh.construct_mesh();

    // Get the current time point after the code execution
    chrono::high_resolution_clock::time_point end_time = chrono::high_resolution_clock::now();

    // Calculate the duration of the code execution
    chrono::microseconds duration = chrono::duration_cast<chrono::microseconds>(end_time - start_time);

    // Output the duration in microseconds
    cout << "Execution time: " << duration.count() << " microseconds" << endl;

    // save mesh to file
    vmesh.save_mesh_to_files(0);


    // generate animation for a moving mesh
    generate_animation_files();

    cout << "done" << endl;
    

}


