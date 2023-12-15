#include <iostream>
#include <random>
#include <chrono>
#include <string>
#include <vector>
#include <fstream>
#include <sys/resource.h>
#include "Point.h"
#include "VoronoiMesh.h"

// MEMORY: function to print out maximum memory usage
long long get_maxrss_memory() {
    
    // declare a rusage structure to store resource usage information
    struct rusage usage;

    // get resource usage statistics for the current process
    if (getrusage(RUSAGE_SELF, &usage) == 0) {
    
        // print the RSS memory size in megabytes
        long long rssmax = usage.ru_maxrss;
        cout << "max RSS memory size: " << rssmax/1024.0/1024.0 << " MB" << endl;
        return rssmax;

    } else {

        cerr << "Error getting resource usage." << endl;

        return 0;
    }
}

// RANDOM POINTS: function to get a sort index
int get_sort_index(Point pt, int sort_grid_size) {

    double nr = static_cast<double>(sort_grid_size);
    int index = static_cast<int>(pt.x * nr) + static_cast<int>(nr * nr * pt.y);
    return index;
}

// RANDOM POINTS: generates seed points to use for mesh generation
deque<Point> generate_seed_points(int N, bool fixed_random_seed, int min, int max, int rd_seed, bool sort_pts, int sort_precision) {
    deque<Point> points;

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
        points.push_back(Point(x, y));
    }

    // if this is true the points will be sorted
    if (sort_pts) {
        vector<int> indices;
        vector<int> sort_indices;

        // get sort indices
        for (int i = 0; i < points.size(); i++) {
            indices.push_back(get_sort_index(points[i], sort_precision));
            sort_indices.push_back(i);
        }

        // combine data into pairs
        vector<pair<int, int> > combined;
        for (int i = 0; i < indices.size(); ++i) {
            combined.push_back(make_pair(indices[i], sort_indices[i]));
        }    

        // sort combined data by sort indices
        sort(combined.begin(), combined.end());

        // get sorted_pts
        deque<Point> sorted_pts;
        for (int i = 0; i < combined.size(); i++) {
            sorted_pts.push_back(points[combined[i].second]);
        }

        return sorted_pts;
    }

    return points;
}

// ANIMATION: generates moving mesh and stores it frame by frame in files
void generate_animation_files(int frames, int seeds) {
    
    // generate initial points and velocities for mesh
    int N_seeds = seeds;
    deque<Point> pts = generate_seed_points(N_seeds, true, 0, 1, 42, true, 1000);
    deque<Point> vel = generate_seed_points(N_seeds, true, -1, 1, 38, false, 1000);

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
        vmesh.do_point_insertion();
        vmesh.save_mesh_to_files(i);
        cout << i << endl;

    }

}

// ANIMATION: function to generate files for animation of grid construction
void animate_algorithm(int N_seeds, int rd_seed, int algorithm, bool sort) {

    // generate seed points for animation and its indices
    deque<Point> pts = generate_seed_points(N_seeds, true, 0, 1, rd_seed, sort, 1000);
    vector<int> indices;
    for (int i = 0; i<pts.size(); i++) {
        indices.push_back(i);
    }

    // mesh for the halfplane intersection algorithm
    VoronoiMesh vmesh_hp_intersect(pts, N_seeds);

    for (int i = 1; i<N_seeds; i++) {

        // actual points known at frame by point insertion mesh
        deque<Point> actual_pts;
        for (int j = 0; j < i; j++) {
            actual_pts.push_back(pts[j]);
        }
    
        // algorithm 0 : halfplane intersection
        if (algorithm == 0) {

            // construct cell as far as known in that step
            VoronoiCell vcell(pts[i-1], i-1);
            vcell.construct_cell(pts, indices);
            vmesh_hp_intersect.vcells.push_back(vcell);
            vmesh_hp_intersect.save_mesh_to_files(i-1);

        // algorithm != 0 : point insertion
        } else {

            // construct cell as far as known in that step
            VoronoiMesh vmesh(actual_pts, i);
            if (i<=3) {
                vmesh.construct_mesh();
            } else {
                vmesh.do_point_insertion();
            }
            vmesh.save_mesh_to_files(i-1);
        }
        cout << i-1 << endl;

    }

}

// BENCHMARKING: function to benchmark the mesh generation algorithm, saves times in csv
void do_benchmarking(string output_file, vector<int> seedvalues, bool append, int algorithm) {

    ofstream timing_list;

    // append to file or create new file
    if (append) {
        timing_list =  ofstream("benchmarks/time_" + output_file, ios::app);
    } else {
        timing_list = ofstream("benchmarks/time_" + output_file);
        timing_list << "nr_seeds,time_in_microseconds\n";
    }

    ofstream memory_list;
    memory_list = ofstream("benchmarks/memory_" + output_file);
    memory_list << "nr_seeds,rss_memory_usage_in_bytes\n";

    cout << "Start Benchmarking" << endl;

    // do benchmark for each seedvalue size
    for (int i = 0; i < seedvalues.size(); i++) {
        
        // generate seeds for mesh
        int N_seeds = seedvalues[i];
        deque<Point> pts = generate_seed_points(N_seeds, true, 0, 1, 42, true, 1000);
    
        // get current time point
        chrono::high_resolution_clock::time_point start_time = chrono::high_resolution_clock::now();

        // construct mesh
        VoronoiMesh* vmesh = new VoronoiMesh(pts, N_seeds);
        if (algorithm == 0) {
            vmesh->construct_mesh();
        } else {
            vmesh->do_point_insertion();
        }

        // get current time point
        chrono::high_resolution_clock::time_point end_time = chrono::high_resolution_clock::now();

        // calculate duration of the code execution
        chrono::microseconds duration = chrono::duration_cast<chrono::microseconds>(end_time - start_time);

        // save to file
        timing_list << N_seeds << "," << duration.count() << "\n";

        cout << i << " ->";

        // output the duration in microseconds
        cout << "Seeds: " << N_seeds << "  Execution time: " << duration.count() << " microseconds" << endl;
        memory_list << N_seeds << "," <<  get_maxrss_memory() << "\n";
 
        //vmesh.save_mesh_to_files(0);
        delete vmesh;
    }

    timing_list.close();
    memory_list.close();

    cout << "Benchmarking done" << endl;

}


// MAIN : generate voronoi mesh for given seed number and stop time for that -------------------------------------
int main () {
    
    // generate seeds for mesh
    int N_seeds = 30;

    deque<Point> pts = generate_seed_points(N_seeds, true, 0, 1, 42, true, 1000);

    // get the current time point before the code execution
    chrono::high_resolution_clock::time_point start_time = chrono::high_resolution_clock::now();

    // construct mesh
    VoronoiMesh vmesh(pts, N_seeds);
    //vmesh.construct_mesh();               // <-- O(n^2) scaling half plane intersection
    vmesh.do_point_insertion();             // <-- O(nlogn) scaling point insertion algoithm

    // get the current time point after the code execution
    chrono::high_resolution_clock::time_point end_time = chrono::high_resolution_clock::now();

    // calculate the duration of the code execution
    chrono::microseconds duration = chrono::duration_cast<chrono::microseconds>(end_time - start_time);

    // output the duration in microseconds
    cout << "Execution time: " << duration.count() << " microseconds" << endl;

    // save mesh to file
    vmesh.save_mesh_to_files(0);


// OPTIONAL : do correctness checks ------------------------------------------------------------------------------

    // check mesh for correctness
    bool tests = vmesh.check_mesh();                            // <-- usually take longer then generating the grid
    cout << "all tests: " << boolalpha << tests << endl;


// OPTIONAL : do benchmarking for some seeds ---------------------------------------------------------------------
/*

    // choose seed numbers for which the benchmarking should be done
    vector<int> seedvals;
    //seedvals.push_back(1);
    seedvals.push_back(10);
    seedvals.push_back(30);
    seedvals.push_back(50);
    seedvals.push_back(100);
    seedvals.push_back(200);
    seedvals.push_back(300);
    seedvals.push_back(500);
    seedvals.push_back(1000);
    seedvals.push_back(3000);
    seedvals.push_back(5000);
    seedvals.push_back(10000);
    seedvals.push_back(15000);
    seedvals.push_back(20000);
    seedvals.push_back(30000);
    seedvals.push_back(100000);
    seedvals.push_back(300000);
    seedvals.push_back(500000);
    seedvals.push_back(1000000);

    // name output file
    string output = "for_memory_benchmark.csv";

    // do the benchmarking
    do_benchmarking(output, seedvals, false, 1);  // true or false: append or new file

*/   
// OPTIONAL : generate animations  -------------------------------------------------------------------------------

    // animation for a moving mesh
    //generate_animation_files(300, 30);

    //animate_algorithm(100, 42, 1, false);

// OPTIONAL : print out max rss memory usage of the process ------------------------------------------------------

    // determine maximum memory usage of the whole process
    long long max_memory = get_maxrss_memory();


    cout << "done" << endl;
    return 0;    

}


