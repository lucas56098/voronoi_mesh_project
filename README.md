# voronoi_mesh_project
Small C++ Project to generate a voronoi mesh in the most naive way possible. Still work in progress...

### Todos
- Write Point insertion algorithm
- Change Point insertion algorithm to O(nlogn) scaling
- Other algorithms and comparison...

![til](./figures/example_voronoi_animation.gif)

### Time performance
The initial version of the program was still very slow (debug and release) with an O(n^3) scaling. The rest of the benchmarks are in release mode. After improving some initial memory problems the O(n^3) version is still quite slow and had memory problems after 15000 Pts. The newer version of the algorithm has an O(n^2) scaling and is no longer memory limited. 

![Image](./figures/example_benchmark.png)

### Memory usage
Memory usage for the O(n^2) algorithm. One can see that the program is no longer memory limited in any reasonable computing time.

![Image](./figures/example_memory_benchmark.png)
