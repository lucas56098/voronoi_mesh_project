#include <iostream>
#include <random>
#include "Point.h"
#include "VoronoiMesh.h"
#include <chrono>
#include <string>
#include <vector>
#include <fstream>

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
void generate_animation_files(int frames, int seeds) {
    
    // generate initial points and velocities for mesh
    int N_seeds = seeds;
    Point* pts = generate_seed_points(N_seeds, true, 0, 1, 42);
    Point* vel = generate_seed_points(N_seeds, true, -1, 1, 38);

    // for each frame generate mesh and store it in files
    for (int i = 0; i < frames; i++) {
        
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

// function to benchmark the mesh generation algorithm, saves times in csv
void do_benchmarking(string output_file, vector<int> seedvalues, bool append) {

    ofstream timing_list;

    // append to file or create new file
    if (append) {
        timing_list =  ofstream(output_file, ios::app);
    } else {
        timing_list = ofstream(output_file);
        timing_list << "nr_seeds,time_in_microseconds\n";
    }

    cout << "Start Benchmarking" << endl;

    // do benchmark for each seedvalue size
    for (int i = 0; i < seedvalues.size(); i++) {
        
        // generate seeds for mesh
        int N_seeds = seedvalues[i];
        Point* pts = generate_seed_points(N_seeds, true, 0, 1, 42);
    
        // get current time point
        chrono::high_resolution_clock::time_point start_time = chrono::high_resolution_clock::now();

        // construct mesh
        VoronoiMesh vmesh(pts, N_seeds);
        vmesh.construct_mesh();

        // get current time point
        chrono::high_resolution_clock::time_point end_time = chrono::high_resolution_clock::now();

        // calculate duration of the code execution
        chrono::microseconds duration = chrono::duration_cast<chrono::microseconds>(end_time - start_time);

        // save to file
        timing_list << N_seeds << "," << duration.count() << "\n";

        // output the duration in microseconds
        cout << "Seeds: " << N_seeds << "  Execution time: " << duration.count() << " microseconds" << endl;
    }

    timing_list.close();

    cout << "Benchmarking done" << endl;

}

// generates points, will generate mesh, stop time and do tests
int main () {

// MAIN : generate voronoi mesh for given seed number and stop time for that -------------------------------------

    // generate seeds for mesh
    int N_seeds = 20;
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



// OPTIONAL : do correctness checks ------------------------------------------------------------------------------

    // check mesh for correctness
    bool tests = vmesh.check_mesh();
    cout << "all tests: " << boolalpha << tests << endl;
    

// OPTIONAL : do benchmarking for some seeds ---------------------------------------------------------------------
    /*
    
    // choose seed numbers for which the benchmarking should be done
    vector<int> seedvals;
    seedvals.push_back(1);
    seedvals.push_back(10);
    seedvals.push_back(30);
    seedvals.push_back(50);
    seedvals.push_back(100);
    seedvals.push_back(200);
    seedvals.push_back(300);
    //seedvals.push_back(1000);

    // name output file
    string output = "times_release.csv";

    // do the benchmarking
    do_benchmarking(output, seedvals, true);  // true or false: append or new file
    
    */

// OPTIONAL : generate animation for a moving mesh1 --------------------------------------------------------------

    /*

    generate_animation_files(300, 30);

    */

    cout << "done" << endl;
    

}


