# Voronoi Mesh Project - vmp
Small C++ project to generate a voronoi mesh using a naive halfplane intersection and point insertion algorithm. Optional also some visualisaition and benchmarking tools are available. This work was done during an undergraduate project internship, in the Group of Dr. Dylan Nelson at ITA Heidelberg. If you want to learn more about the algorithms and how they work read the first few segments as well. Otherwise, feel free to just skip to the getting started part.

#### Table of contents
1. [Introduction](#introduction)
2. [Naive halfplane intersection](#naive-halfplane-intersection)
3. [Point insertion](#point-insertion)
4. [Performance and memory usage](#performance-and-memory-usage)
5. [Correctness checks](#correctness-checks)
6. [Getting started](#getting-started)
7. [Run options](#run-options)
8. [Acknowledgements](#acknowledgements)

<p align="left">
  <img src="./figures/readme_figures/example_voronoi_animation.gif" alt="moving_mesh" class = "img-responsive" style="width: 60%;">
</p>

## Introduction
A `VoronoiCell` is a polygonal region surrounding a specific point in a space, encompassing all locations that are closer to that point than to any other point in a given set of points. The edges of that voronoi cell are part of the perpendicular bisectors to the neigbouring points. In the following we will continue calling those perpendicular bisectors `Halfplane` and the midpoint of such a voronoi cell `seed`. The voronoi cells of all points in the set combined are called a `VoronoiMesh`. Voronoi meshes can be used in many applications. One such application for example are cosmological hydrodynamical simulations like [IllustrisTNG](https://www.tng-project.org) based on codes like [AREPO](https://github.com/dnelson86/arepo), where a moving voronoi mesh is used as a grid for the hydrodynamics and therefore needs to be regenerated for every timestep. While moving vornoi meshes, as a compromise between SPH and AMR, help with shock treatment and also make the code gallilean invariant this of course comes at a substential computing cost. Fast and reliable algorithms for generating voronoi meshes in 3D therefore are very useful to do such simulations. Due to the limited time of my project internship we however sticked to 2D algorithms.
Voronoi mesh generation algorithms can be divided into direct and indirect algorithms, where the indirect algorithms first generate a delunay triangulation and then use the geometric duality to the voronoi tesselation to construct the mesh. While codes like [AREPO](https://github.com/dnelson86/arepo) work using an indirect approach we focused on directly generating a voronoi mesh.
The two algorithms we looked at are a naive halfplane intersection algorithm which at best scales with $\mathcal{O}(n^2)$ and a point insertion algorithm that at best scales with $\mathcal{O}(n\log{n})$.

### Folder structure
In the main folder there is the C++ program with its header files and the python file for visualisation. 
- `./figures` : here the outputs of any visualization will be saved (e.g. image, benchmark, mmanim, gganim)
    - `./figures/readme_figures` : here are the images for the readme 
- `./build` : this will be the folder where you will build your executable into
    - `./build/files` : here you will find any saved mesh files stored in edge-, verticie- and seedfiles
    - `./build/benchmarks` : if you do a benchmark here the raw files will be saved. the plots however still are in `./figures`

### Object structure and methods
To get a brief overwiev of the program structure we first look at the objects. There is an `Point` object which basically just stores an x and y valaue. Two of such points can be used to initalize a `Halfplane` (which, strictly speaking, in 2D is just a straight line) which is the perpendicular bisector between the two points. In the halfplane itself the midpoint between the two initializing points and a normed vector pointed along the halfplane are stored. Additionaly the different objects store indices for better finding them in the different data structures but i'll skip over that here.
A `VoronoiCell` is an object consisting of a seedpoint, a vector of verticies (also points),  a vector of edges (the final halfplanes) and a vector of halfplanes needed for the construction which after generation is cleared. The voronoi cells are stored in an `VoronoiMesh` in a vcell vector. Additional to those objects there is a structure called `intersection` to store some informations when intersection two halfplanes (e.g. intersecting point).

There are some geometrical operations which are essential to the algortithms and therefore deserve to be discussed in advance.
- First there is `intersect_two_halfplanes()` where the first halfplane usually is the current halfplane and the second halfplane is the one to intersect. In 2D this reduces to solving a linear equation system evaluating some determinants. For the intersection we store the intersecting point, the second halfplane and the signed distance (relative to normed vector pointed along the halfplane) to the midpoint of the first/current halfplane.
- This way we can find the smallest positive intersection (`find_smallest_pos_intersect()`) by intersecting all the necessary halfplanes with the current halfplane and minimizing for the distance to either the midpoint or the last vertex depending on situation.

## Naive halfplane intersection
<p align="left">
  <img src="./figures/readme_figures/explainer_hp_intersection.gif" alt="hp_intersection_explainer" height = "400" width = "400">
  <img src="./figures/readme_figures/hp_intersection.gif" alt="hp_intersection" height = "400" width = "400">
</p>

The naive halfplane algorithm can be found as the `construct_mesh()` function of the `VoronoiMesh`. It is the slower of the both algorithms but conceptually easier to understand. As one can see in the right gif a cell is generated exactly once and then stays this way the whole time. This is a conceptual difference to point insertion as we will later see. For the halfplane intersection one first determines the halfplane closest to the seed of the cell one wants to construct because this is the only halfplane where one can be sure, that its midpoint will be part of the edge of the cell. This halfplane will defenitely be part of the cell and can be stored. Starting from there one finds the halfplane intersection with the smallest positive relative distance to the midpoint of that first edge by intersecting the first edge with all other halfplanes that exist. The intersecting halfplane will be the next edge that can be stored and their intersection will be a vertex. For the next step finding the next smallest positive intersection the next edge becomes the current index and the vertex becomes the last vertex. This process repeats until one returns to the first halfplane again. The process is also visualised in the left gif. Boundary handling here is reached by just adding four halfplane boundaries to the total list of halfplanes to be checked. This algorithm in total needs on the order of N checks per cell with N cells. Thus this algorithm is $\mathcal{O}(n^2)$. We will check this in the performance benchmark later.

## Point insertion
<p align="left">
  <img src="./figures/readme_figures/explainer_pt_insertion.gif" alt="pt_insertion_explainer" height = "400" width = "400">
  <img src="./figures/readme_figures/unsorted_point_insertion_clipped.gif" alt="pt_insertion" height = "400" width = "400">
</p>

The point insertion algorithm can be found as the `do_point_insertion()` function of the `VoronoiMesh`. It is conceptually a bit more difficult and requires more focus on adapting the neighbouring cells and respecting boundary conditions. As one can see in the right gif this algortihm inserts one point after another to make a new cell and then adapts all cells that need to be changed. For this reason after constructing a cell the cell will likely be changed multiple times later on in the algorithm. Conceptually for this algorithm it is sufficient to look a the process of inserting one point because this process repeats again and again. If we want to insert one point into an already existing mesh we first need to find out in which cell we currently are. For that there exist a `find_cell_index()` function that starts from some specified cell and from there jumps step by step towards the new seedpoint until it can't get closer. The cell it is in then must be the cell where the point is in as well. This process can be optimized largely later on (see. section on presorting points). When we know the cell we are in we can construct the halfpane between the new seedpoint and the seedpoint of the cell we are in. This boundary will be part of the new cell. Now we want to find the smallest possible intersection with all the edges of the cell we are in. With that edge we know which cell will be the next to use for constructing the new cell. The intersection with the edge gives us the vertex and making a halfplane between the next cell seedpoint and the new cell seedpoint gives us the next edge. This process can be repeated until we reach the first cell we started in again. Now that the new cell is constructed we need to adapt the surrounding cells by clipping their edges according to where the intersections have been. The whole process is visualised in the left gif. Boundary handling here is more difficult than in the first algorithm because when we reach a boundary there is no cell we can go into. Therefore we need a special treatment here. The basic rule for that is: stay on the boundary as long as the leave condition is not satisfied. The leave condition is that the halfplane, between the new seedpoint and the seedpoint of the cell we are with our boundary, intersects with the boundary inside of the cell we are checking. 
<p align="left">
  <img src="./figures/readme_figures/boundary1.png" alt="boundary1" style="width: 45%;">
  <img src="./figures/readme_figures/boundary2.png" alt="boundary2" style="width: 45%;">
</p>

In the left image the leave condition would be satisfied while on the right image the leave condition for that cell wouldn't be satisfied. When the boundary is left the algorithms continues normal as before. Given the `find_cell_index()` function is optimized this algorithm scales with $\mathcal{O}(n\log{n})$.

### Presorting points
Presorting the points speeds up the `find_cell_index()` function by first setting start index to the last found cell index. If the points are not sorted this is of course not a good guess, but if the points are spacially closely sorted this is a really good guess and can reduce the number of steps needed to reach the cell, the new seed is in, hugely. Here are a few examples of sorting that is implemented in the command line interface (no sort, modulo sort, inout, outin). The modulo sort is the one with the best performance out of them. Some kind of Peano Hilbert curve or something might be even better but is not implemented here. 
<p align="left">
  <img src="./figures/readme_figures/unsorted_point_insertion.gif" alt="sort1" height = "400" width = "400">
  <img src="./figures/readme_figures/sorted_point_insertion.gif" alt="sort2" height = "400" width = "400">
  <img src="./figures/readme_figures/in_out_point_insertion.gif" alt="sort3" height = "400" width = "400">
  <img src="./figures/readme_figures/out_in_point_insertion.gif" alt="sort4" height = "400" width = "400">
</p>

### Degeneracy
Degeneracies are important to get right for a robust handling of special cases. However an handling for exact degeneracies does not work with the code. For points degenerate to up to 1e-13 the algorithms still runs quite stable but exact leads to problems. Be aware of that. Here an example of an almost uniform grid. The points vary by around 1e-13 form the uniform grid.
<p align="left">
  <img src="./figures/readme_figures/almost_uniform_grid.png" alt="almost_uniform" style="width: 50%;">
</p>

## Performance and memory usage
For peformance benchmarking the time the generation took on my PC (MacBook Pro M1) was plotted as a function of points to generate. If you want to try some benchmarking for yourself feel free to use the `-benchmark` option in the command line interface. As one can see the algorithms scale as expected. In addition also an even more naive hp intersection scaling with $\mathcal{O}(n^3)$ is shown, which, however, is not included in the final code. Also one can see that the sorting of the points according to the modulo sort is the final piece in the puzzle to archieve $\mathcal{O}(n\log{n})$ scaling, because otherwise for very large point sets the `find_cell_index()` function scales worse and it takes many steps to reach the cell where the point is in. Memory-wise some improvements defenitely still can be made but i think it is quite hard to do this without the need to recompute stuff in the program or lose the quick acess to the vertecies. The memory grows approximately linear wich is kind of expected. In addition to that the maximum rss memory usage is still higher than the final mesh size because of the generation algorithms also taking up memory while running.
<p align="left">
  <img src="./figures/readme_figures/example_benchmark.png" alt="benchmark" style="width: 45%;">
  <img src="./figures/readme_figures/example_memory_benchmark.png" alt="memory_benchmark" style="width: 45%;">
</p>

## Correctness checks
Checking the mesh after generation is an important part to verify that the algorithm works as expected. For that we try to check different properies a voronoi mesh should have. We do the following checks: 

- `check_equidistance()`: Test if every vertex has at least three equidistant seedpoints. (Definition of a vertex)
- `check_area()` : Compute the total area of all cells added up. (Should equal 1 up to some finite precision limits)
- `check_neighbours()` : Test if every neigbour of a cell has that cell as a neigbour as well.

Checks can be activated using `-check` as an option in the command line interface. However one should note that for large point sets the mesh generation is way faster than the correctness checks so i would not recommend to use them only if really needed for very huge point sets.

## Getting started
Before starting make sure you have the following installed:

- C++ with a working compiler (only with stdlib)
- CMake (optional one can manually build the project)
- Git (alternatively the git clone in the getting started can be replaced by just downloading the files manually)

For the visualisation in python the following packages are needed:
- argparse, time, tqdm, matplotlib, numpy, PIL, scipy

Start by going into the folder where you want to clone the repository into and do:

```bash
git clone https://github.com/lucas56098/voronoi_mesh_project.git
```

Then go into the voronoi_mesh_project, create a build folder and go into that as well.

```bash
cd voronoi_mesh_project
mkdir build
cd build
```

In that folder build the program using cmake

```bash
cmake ..
cmake --build .
```

The program is now build and can be run using `./vmp`. For example one could try

```bash
./vmp -n 100 -fixed_seed 42 -image
```

If everything works fine, the grid will be generated for 100 seeds and stored into `build/files`. Also the grid will be plotted using python and the final image will be stored as `figures/single_picture.pdf`. It should look something like this:
<p align="left">
  <img src="./figures/readme_figures/getting_started_terminal.png" alt="terminal" style="width: 50%;">
  <img src="./figures/readme_figures/getting_started_plot.png" alt="figure"style="width: 40%;">
</p>

## Run options
Now that we have a working installation here is an overview of what you can do with the command line interface.

`-n [int N_seeds]`                  : specify the number of seedpoints 

`-fixed_seed [int rd_seed]`         : fix the random seed and specify it 

`-sort_option [int sort]`           : specify presorting of points

                     0 - no sort

                     1 - modulo sort (standard option)

                     2 - radially outward

                     3 - radially inward

`-check`                            : check mesh for correctness (for large point sets takes way longer than grid generation)

`-algorithm [int algorithm]`         : specify the algorithm used

                     0 - halfplane intersection O(n^2)

                     1 - point insertion O(nlogn) (standard option)

`-image`                            : plot image of mesh using python matplotlib and save file

The next options are specific and not compatible with all of the options above!

`-benchmark`                        : benchmark algorithm, save benchmarking files and plot time and memory benchmark using python matplotlib

    ! benchmarking is not compatible with -n, -check, -image, - gganim, -mmanim !

`-mmanim [int N_frames] [int fps]`  : moving mesh animation, specify (frames) (fps)

    ! Moving Mesh Animation is not compatible with -sort_option, -check, -algorithm, -image, -benchmark, -gganim !

`-gganim [int fps]`                 : grid generation animation, specify (fps)

    ! Grid Generation Animation is not compatible with -sort_option, -check, -algorithm, -image, -benchmark, -mmanim !


If anything is unclear:

`-h`,`-help`,`--help`               : will show this list


## Acknowledgements
Many thanks do [Dylan](https://github.com/dnelson86) and [Chris](https://github.com/cbyrohl) from the Institute for Theoretical Astrophysics Heidelberg who guided me through this project and spend many hours discussing conceptual problems and brainstorming further ideas. 
It was a lot of fun!