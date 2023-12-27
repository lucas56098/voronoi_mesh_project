#include <iostream>
#include <random>
#include <chrono>
#include <string>
#include <vector>
#include <fstream>
#include <cstdlib>
#include <sys/stat.h>
#include <sys/resource.h>
#include "Point.h"
#include "VoronoiMesh.h"


// ANSI escape codes for text colors
#define RED_TEXT "\033[1;31m"
#define ORANGE_TEXT "\033[1;33m"
#define RESET_COLOR "\033[0m"
#define GREEN_TEXT "\033[1;32m"

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
int get_sort_index(Point pt, int sort_grid_size, int sort_scheme) {

    double nr = static_cast<double>(sort_grid_size);

    int index;

    // sort by x-y modulo grid
    if (sort_scheme == 1) {

        if (static_cast<int>(nr * nr * pt.y)%2==0) {

            index = (sort_grid_size - static_cast<int>(pt.x * nr)) + static_cast<int>(nr * nr * pt.y);

        } else {

            index = static_cast<int>(pt.x * nr) + static_cast<int>(nr * nr * pt.y);

        }

    // sort radially outward
    } else if (sort_scheme == 2) {

        index = static_cast<int>((nr*nr)*sqrt((pt.x-0.5)*(pt.x-0.5) + (pt.y-0.5)*(pt.y-0.5)));

    // sort radially inward
    } else if (sort_scheme == 3) {

        index = static_cast<int>((nr*nr)*(sqrt(0.5) - sqrt((pt.x-0.5)*(pt.x-0.5) + (pt.y-0.5)*(pt.y-0.5))));

    // all other numbers -> do not sort
    } else {

        index = 1;

    }

    return index;
}

// RANDOM POINTS: generates seed points to use for mesh generation
vector<Point> generate_seed_points(int N, bool fixed_random_seed, double min, int max, int rd_seed, bool sort_pts, int sort_precision, int sort_scheme) {
    vector<Point> points;

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
            indices.push_back(get_sort_index(points[i], sort_precision, sort_scheme));
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
        vector<Point> sorted_pts;
        for (int i = 0; i < combined.size(); i++) {
            sorted_pts.push_back(points[combined[i].second]);
        }

        return sorted_pts;
    }

    return points;
}

// UNIFORM POINTS: generates seed points to use for mesh generation
vector<Point> generate_uniform_seed_points(int N_approx, double min, double max) {

    vector<Point> points;
    default_random_engine eng;
    eng = default_random_engine(42);

    double val = 0.0000000000001;

    uniform_real_distribution<double> distr(-val, val);

    for (int i = 0; i<sqrt(N_approx)-1; i++) {

        for (int j = 0; j<sqrt(N_approx)-1; j++) {

            points.push_back(Point(min + max * static_cast<double>(i+1)/static_cast<double>(sqrt(N_approx)) + distr(eng), min + max * static_cast<double>(j+1)/static_cast<double>(sqrt(N_approx))+ distr(eng)));

        }

    }

    return points;

}

// ANIMATION: generates moving mesh and stores it frame by frame in files
void generate_animation_files(int frames, int seeds, bool fixed_seed, int rd_seed) {
    
    // generate initial points and velocities for mesh
    int N_seeds = seeds;
    vector<Point> pts = generate_seed_points(N_seeds, fixed_seed, 0, 1, rd_seed, true, 1000, 1);
    vector<Point> vel = generate_seed_points(N_seeds, fixed_seed, -1, 1, rd_seed, false, 1000, 0);

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
        VoronoiMesh vmesh(pts);
        vmesh.do_point_insertion();
        vmesh.save_mesh_to_files(i);
        cout << fixed << (i+1) << "/" << (frames) << "\r";
        cout.flush();

    }

    cout << endl;

}

// ANIMATION: function to generate files for animation of grid construction
void animate_algorithm(int N_seeds, int rd_seed, int algorithm, bool sort, int sort_scheme) {

    // generate seed points for animation and its indices
    vector<Point> pts = generate_seed_points(N_seeds, true, 0, 1, rd_seed, sort, sqrt(N_seeds), sort_scheme);
    vector<int> indices;
    for (int i = 0; i<pts.size(); i++) {
        indices.push_back(i);
    }

    // mesh for the halfplane intersection algorithm
    VoronoiMesh vmesh_hp_intersect(pts);

    for (int i = 1; i<N_seeds; i++) {

        // actual points known at frame by point insertion mesh
        vector<Point> actual_pts;
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
            VoronoiMesh vmesh(actual_pts);
            if (i<=3) {
                vmesh.construct_mesh();
            } else {
                vmesh.do_point_insertion();
            }
            vmesh.save_mesh_to_files(i-1);
        }
        cout << fixed << i << "/" << N_seeds-1 << "\r";
        cout.flush();

    }

    cout << endl;

}

// BENCHMARKING: function to benchmark the mesh generation algorithm, saves times in csv
void do_benchmarking(string output_file, vector<int> seedvalues, bool append, int algorithm, bool sort, int sort_scheme, bool fixed_seed, int rd_seed) {

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

    cout << "Start Benchmarking: 0 to " << seedvalues.size()-1 << endl;

    // do benchmark for each seedvalue size
    for (int i = 0; i < seedvalues.size(); i++) {
        
        // generate seeds for mesh
        int N_seeds = seedvalues[i];
        vector<Point> pts = generate_seed_points(N_seeds, fixed_seed, 0, 1, rd_seed, sort, sqrt(N_seeds), sort_scheme);
    
        // get current time point
        chrono::high_resolution_clock::time_point start_time = chrono::high_resolution_clock::now();

        // construct mesh
        VoronoiMesh* vmesh = new VoronoiMesh(pts);
        //VoronoiMesh vmesh(pts);
        if (algorithm == 0) {
            vmesh->construct_mesh();
            //vmesh.construct_mesh();
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

        long long total_size = vmesh->calculate_mesh_memory(true);
        cout << "manual mesh capacity: " << total_size/1024.0/1024.0 << "MB" << endl;
 
        //vmesh.save_mesh_to_files(0);
        delete vmesh;
    }

    timing_list.close();
    memory_list.close();

    cout << "Benchmarking done" << endl;

}

// CLI: test wether part of command line input is integer
bool is_integer(const string& str) {
    try {
        // Use std::stoi for string to integer conversion with error checking
        stoi(str);
        return true;  // If no exception is thrown, it's a valid integer
    } catch (const invalid_argument& e) {
        // std::invalid_argument is thrown if no conversion could be performed
        return false;
    } catch (const out_of_range& e) {
        // std::out_of_range is thrown if the converted value would fall out of the range of the result type
        return false;
    }
}

// MAIN :  -------------------------------------------------------------------------------------------------------
int main (int argc, char *argv[]) {

    // check if directories exist
    const char* dir1 = "files";
    const char* dir2 = "benchmarks";
    struct stat sb;
 
    if (!(stat(dir1, &sb) == 0)) {
        int result = system("mkdir files");
    }
    if (!(stat(dir2, &sb) == 0)) {
        int result = system("mkdir benchmarks");
    }
 


    // Standard Values for CLI options
    int N_seeds = 20;
    bool fixed_seed = false;
    int rd_seed = 42;
    bool sort = true;
    int sort_scheme = 1;
    bool check_option = false;
    int run_option = 0;         // run options: 0: normal_mesh, 1: benchmark, 2: moving mesh animation, 3: grid generation animation
    int algorithm = 1;      // 1: pt_insertion, 0: hp_intersection, rest: also pt_insertion
    bool image_condition = false;
    bool need_help = false;
    int frames = 100;
    int fps = 20;


    // READ OUT CLI to start program with correct options
    // loop through all cli arguments
    for (int i = 0; i < argc; i++) {
        bool found_command = false;

        // option for seed number
        if (strcmp(argv[i], "-n") == 0 && argc > i+1) {
            found_command = true;
            if (is_integer(argv[i+1])) {
                N_seeds = stoi(argv[i+1]);
                cout << GREEN_TEXT << "CLI OPTION: " << RESET_COLOR << "Seed number = " << N_seeds << endl;
            } else {
                cout << RED_TEXT << "CLI ERROR: " << RESET_COLOR << "Specified seed number is not an integer: " << argv[i] << " " << argv[i+1] << endl;
                cout << setw(11) << "" << "Continuing with standard value for -n: " << N_seeds << endl;
            }
        } else if (strcmp(argv[i], "-n") == 0 && argc <= i+1) {
            found_command = true;
            cout << RED_TEXT << "CLI ERROR: " << RESET_COLOR << "Called -n but not specified seed number. Use: -n (your_seed_number) instead" << endl;
        }

        // option to fix random seed
        if (strcmp(argv[i], "-fixed_seed") == 0 && argc > i+1) {
            found_command = true;
            if (is_integer(argv[i+1])) {
                rd_seed = stoi(argv[i+1]);
                fixed_seed = true;
                cout << GREEN_TEXT << "CLI OPTION: " << RESET_COLOR << "fixed seed = " << rd_seed << endl;
            } else {
                fixed_seed = true;
                cout << ORANGE_TEXT << "CLI WARNING: " << RESET_COLOR << "Fixed random seed but not specified any number" << endl;
                cout << setw(13) << "" << "Continuing with standard value for -fixed_seed: 42" << endl;
            }
        } else if (strcmp(argv[i], "-fixed_seed") == 0 && argc <= i+1) {
            found_command = true;
            fixed_seed = true;
            cout << ORANGE_TEXT << "CLI WARNING: " << RESET_COLOR << "Fixed random seed but not specified any number" << endl;
            cout << setw(13) << "" << "Continuing with standard value for -fixed_seed: 42" << endl;
        }

        // option to sort random seeds
        if (strcmp(argv[i], "-sort_option") == 0 && argc > i+1) {
            found_command = true;
            if (is_integer(argv[i+1])) {
                sort_scheme = stoi(argv[i+1]);
                if (sort_scheme == 0) {
                    sort = false;
                } else {
                    sort = true;
                }
                cout << GREEN_TEXT << "CLI OPTION: " << RESET_COLOR << "Sort Option = " << sort << endl;
            } else {
                cout << RED_TEXT << "CLI ERROR: " << RESET_COLOR << "Sort scheme is not a valid integer. Use: -sort (0, 1, 2, 3) where" << 
                        endl << setw(11) << "" << "0:no sort, 1: modulo sort, 2: radially outward, 3: radially inward" << endl;
                cout << setw(11) << "" << "Continuing with standard sort option: 1 -> modulo sort" << endl;
            }
        } else if (strcmp(argv[i], "-sort_option") == 0 && argc <= i+1) {
            found_command = true;
            cout << RED_TEXT << "CLI ERROR: " << RESET_COLOR << "did not specify an sort scheme after using -sort. Use: -sort (0, 1, 2, 3) where" << 
                    endl << setw(11) << "" << "0:no sort, 1: modulo sort, 2: radially outward, 3: radially inward" << endl;
            cout << setw(11) << "" << "Continuing with standard sort option: 1 -> modulo sort" << endl;
        }

        // option to check the mesh after generation
        if (strcmp(argv[i], "-check") == 0) {
            found_command = true;
            check_option = true;
            cout << GREEN_TEXT << "CLI OPTION: " << RESET_COLOR << "Check" << endl;
        }

        // option to change algorithm
        if (strcmp(argv[i], "-algorithm") == 0 && argc > i+1) {
            found_command = true;
            if (is_integer(argv[i+1])) {
                algorithm = stoi(argv[i+1]);
                cout << GREEN_TEXT << "CLI OPTION: " << RESET_COLOR << "Algorithm = " << algorithm << endl;
            } else {
                cout << RED_TEXT << "CLI ERROR: " << RESET_COLOR << "Specified algorithm is not an integer: " << argv[i] << " " << argv[i+1] << endl;
                cout << setw(11) << "" << "Continuing with standard algorithm: point_insertion " << endl;
            }
        } else if (strcmp(argv[i], "-algorithm") == 0 && argc <= i+1) {
            found_command = true;
            cout << RED_TEXT << "CLI ERROR: " << RESET_COLOR << "Called -algorithm but not specified it. Use: -algorithm (your_algorithm_number) instead" << endl;
            cout << setw(11) << "" << "Continuing with standard algorithm: point_insertion " << endl;
        }

        // option to directly plot image of generated mesh
        if (strcmp(argv[i], "-image") == 0) {
            found_command = true;
            image_condition = true;
            cout << GREEN_TEXT << "CLI OPTION: " << RESET_COLOR << "Image" << endl;
        }

        // option to do benchmarks
        if (strcmp(argv[i], "-benchmark") == 0) {
            found_command = true;
            run_option = 1;
            bool correct_benchmark = true;

            for (int j = 0; j<argc; j++) {
                if(strcmp(argv[j], "-check") == 0 || strcmp(argv[j], "-n") == 0 || strcmp(argv[j], "-image") == 0) {
                    cout << RED_TEXT << "CLI ERROR: " << RESET_COLOR << "Benchmark is not compatible with -check, -n, -image. Check wether you specified any of these" << endl;
                    correct_benchmark = false;
                }
            }
            if (correct_benchmark) {
                cout << GREEN_TEXT << "CLI OPTION: " << RESET_COLOR << "Benchmark" << endl;
            }

        }

        // option to animate moving mesh
        if (strcmp(argv[i], "-mmanim") == 0 && argc > i+2) {
            found_command = true;
            run_option = 2;
            bool correct_benchmark = true;

            if (is_integer(argv[i+1]) && is_integer(argv[i+2])) {
                frames = atoi(argv[i+1]);
                fps = atoi(argv[i+2]);
            } else {
                cout << RED_TEXT << "CLI ERROR: " << RESET_COLOR << "Specified numbers are not integer: " << argv[i] << " " << argv[i+1] << " " << argv[i+2] << endl;
                correct_benchmark = false;
            }

            for (int j = 0; j<argc; j++) {
                if (strcmp(argv[i], "-sort_option") == 0 || strcmp(argv[i], "-check") == 0 || strcmp(argv[i], "-algorithm") == 0 || strcmp(argv[i], "-image") == 0 || strcmp(argv[i], "-benchmark") == 0 || strcmp(argv[i], "-gganim") == 0) {
                    cout << RED_TEXT << "CLI ERROR: " << RESET_COLOR << "Moving Mesh Animation is not compatible with -sort_option, -check, -algorithm, -image, -benchmark, -gganim. Check wether you specified any of these" << endl;
                    correct_benchmark = false;
                }
            }
            if (correct_benchmark) {
                cout << GREEN_TEXT << "CLI OPTION: " << RESET_COLOR << "Moving Mesh Animation" << endl;
            }

        } else if (strcmp(argv[i], "-mmanim") == 0 && argc <= i+2) {
            found_command = true;
            cout << RED_TEXT << "CLI ERROR: " << RESET_COLOR << "Did not specify number of frames and fps. Use: -mmanim num_frames fps" << endl;
        }



        // option to animate grid generation
        if (strcmp(argv[i], "-gganim") == 0 && argc > i+1) {
            found_command = true;
            run_option = 3;
            bool correct_benchmark = true;

            if (is_integer(argv[i+1])) {
                fps = atoi(argv[i+1]);

            } else {
                cout << RED_TEXT << "CLI ERROR: " << RESET_COLOR << "Specified number is not an integer: " << argv[i] << " " << argv[i+1] << endl;
                correct_benchmark = false;
            }

            for (int j = 0; j<argc; j++) {
                if (strcmp(argv[i], "-sort_option") == 0 || strcmp(argv[i], "-check") == 0 || strcmp(argv[i], "-algorithm") == 0 || strcmp(argv[i], "-image") == 0 || strcmp(argv[i], "-benchmark") == 0) {
                    cout << RED_TEXT << "CLI ERROR: " << RESET_COLOR << "Grid Generation Animation is not compatible with -sort_option, -check, -algorithm, -image, -benchmark, -mmanim. Check wether you specified any of these" << endl;
                    correct_benchmark = false;
                }
            }
            if (correct_benchmark) {
                cout << GREEN_TEXT << "CLI OPTION: " << RESET_COLOR << "Grid Generation Animation" << endl;
            }

        } else if (strcmp(argv[i], "-gganim") == 0 && argc <= i+1) {
            found_command = true;
            cout << RED_TEXT << "CLI ERROR: " << RESET_COLOR << "Did not specify fps. Use: -gganim fps" << endl;
        }



        // option to get help
        if (strcmp(argv[i], "-help") == 0 || strcmp(argv[i], "--help") == 0 || strcmp(argv[i], "-h") == 0) {
            found_command = true;
            need_help = true;
            cout << "Voronoi Mesh Project - vmp" << endl << endl;
            cout << "Program to generate voronoi meshes using different algorithms, visualize and benchmark them. \nWritten during project internship at ITA Heidelberg by Lucas Schleuss" << endl << endl;
            cout << "-n                 : specify the number of seedpoints" << endl;
            cout << "-fixed_seed        : fix the random seed and specify it" << endl;
            cout << "-sort_option       : specify presorting of points" << endl;
            cout << setw(21) << "" << "0 - no sort" << endl;
            cout << setw(21) << "" << "1 - modulo sort (standard option)" << endl;
            cout << setw(21) << "" << "2 - radially outward" << endl;
            cout << setw(21) << "" << "3 - radially inward" << endl;
            cout << "-check             : check mesh for correctness (for large point sets takes way longer than grid generation)" << endl;
            cout << "-algorithm          : specify the algorithm used" << endl;
            cout << setw(21) << "" << "0 - halfplane intersection O(n^2)" << endl;
            cout << setw(21) << "" << "1 - point insertion O(nlogn) (standard option)" << endl;
            cout << "-image             : plot image of mesh using python matplotlib and save file" << endl;
            cout << "-benchmark         : benchmark algorithm, save benchmarking files and plot time and memory benchmark using python matplotlib" << endl;
            cout << setw(21) << "" << "! benchmarking is not compatible with -n, -check, -image, - gganim, -mmanim !" << endl;
            cout << "-mmanim            : moving mesh animation, specify (frames) (fps)" << endl;
            cout << setw(21) << "" << "! Moving Mesh Animation is not compatible with -sort_option, -check, -algorithm, -image, -benchmark, -gganim !" << endl;
            cout << "-gganim            : grid generation animation, specify (fps)" << endl;
            cout << setw(21) <<  "" << "! Grid Generation Animation is not compatible with -sort_option, -check, -algorithm, -image, -benchmark, -mmanim !" << endl;
            
            cout << "-h, -help, --help  : show this window and exit" << endl;

        }

        // error message for unknown commands
        if (!found_command && argv[i][0] == '-') {
            cout << RED_TEXT << "CLI ERROR: " << RESET_COLOR << "Unknown command found: " << argv[i] << " <- please remove or correct!" << endl;
        }

    }


    // GENERATE MESH: generate voronoi mesh for given seed number and stop time for that
    if (run_option == 0 && !need_help) {

        cout << "generating points..." << endl;

        vector<Point> pts = generate_seed_points(N_seeds, fixed_seed, 0, 1, rd_seed, sort, sqrt(N_seeds), sort_scheme);
        //vector<Point> pts = generate_uniform_seed_points(N_seeds, 0, 1);

        cout << "start timer..." << endl;

        // get the current time point before the code execution
        chrono::high_resolution_clock::time_point start_time = chrono::high_resolution_clock::now();

        // construct mesh
        VoronoiMesh vmesh(pts);
         if (algorithm == 0) {
            vmesh.construct_mesh();         // <-- O(n^2) scaling half plane intersection
        } else {
            vmesh.do_point_insertion();     // <-- O(nlogn) scaling point insertion algoithm
        }     

        // get the current time point after the code execution
        chrono::high_resolution_clock::time_point end_time = chrono::high_resolution_clock::now();

        // calculate the duration of the code execution
        chrono::microseconds duration = chrono::duration_cast<chrono::microseconds>(end_time - start_time);

        cout << "end timer..." << endl;

        // output the duration in microseconds
        cout << "Execution time: " << duration.count() << " microseconds" << endl;

        // save mesh to file
        cout << "saving mesh to files..." << endl;
        vmesh.save_mesh_to_files(0);

        // OPTIONAL : do correctness checks 
        if (check_option) {
            // check mesh for correctness
            bool tests = vmesh.check_mesh();                            // <-- usually take longer then generating the grid
            if (tests) {
                cout << "all tests: " << boolalpha << GREEN_TEXT << tests << RESET_COLOR << endl;
            } else {
                cout << "all tests: " << boolalpha << RED_TEXT << tests << RESET_COLOR << endl;
            }
        }

        // OPTIONAL : print out max rss memory usage of the processs
        long long max_memory = get_maxrss_memory();

        long long total_capacity = vmesh.calculate_mesh_memory(true);
        cout << "manually calculated mesh capacity: " << total_capacity/1024.0/1024.0 << "MB" << endl;

        // Show Image
        if (image_condition) {
            int result = system("python3 ../visualisation.py -program 0 ");
        }


    }

    
// OPTIONAL : do benchmarking for some seeds ---------------------------------------------------------------------

    if (run_option == 1 && !need_help) {
    
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
        if (algorithm == 1) {
            seedvals.push_back(15000);
            seedvals.push_back(20000);
            seedvals.push_back(30000);
            seedvals.push_back(100000);
            seedvals.push_back(300000);
            seedvals.push_back(500000);
            seedvals.push_back(1000000);
        }
        

        //seedvals.push_back(10);
        //for (int i = 10000; i< 500000; i+= 10000) {
        //    seedvals.push_back(i);
        //}

        // name output file
        string output = "benchmark.csv";

        // do the benchmarking
        do_benchmarking(output, seedvals, false, algorithm, sort, sort_scheme, fixed_seed, rd_seed);  // first true or false: append or new file

        // Show Benchmarking plots
        int result = system("python3 ../visualisation.py -program 1 ");


        }


// OPTIONAL : generate animations  -------------------------------------------------------------------------------

    // animation for a moving mesh
    if (run_option == 2) {
        generate_animation_files(frames, N_seeds, fixed_seed, rd_seed);

        // Create a named std::string
        string commandString = "python3 ../visualisation.py -program 2 -num_frames " + to_string(frames) + " -fps " + to_string(fps);

        // Use c_str() on the named string
        const char* command = commandString.c_str();

        int result = system(command);
    }

    // grid generation animation
    if (run_option == 3) {

        animate_algorithm(N_seeds, rd_seed, algorithm, sort, sort_scheme);

        // Create a named std::string
        string commandString = "python3 ../visualisation.py -program 3 -num_frames " + to_string(N_seeds) + " -fps " + to_string(fps);

        // Use c_str() on the named string
        const char* command = commandString.c_str();

        int result = system(command);

    }

    cout << "done" << endl;

    return 0;    

}


