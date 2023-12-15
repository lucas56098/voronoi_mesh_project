# voronoi_mesh_project
Small C++ Project to generate a voronoi mesh using half plane intersection and point insertion. Still work in progress...

### Todos
- Manually calculate memory usage and compare
- Different sorting of points?

![til](./figures/example_voronoi_animation.gif)

### Time performance
The initial version of the program was still very slow with an O(n^3) scaling. The newer version of the algorithm had an O(n^2) scaling and is no longer memory limited. This is the expected scaling for half plane insertion. The next algorithm is point insertion and has a way better scaling. When one additionally sorts the poins before generating the grid the scaling is O(nlogn). Here the program again becomes memory limited again, because it is way faster than before.

![Image](./figures/example_benchmark.png)

### Memory usage
Memory usage for the point insertion algorithm. One can see, that the memory scales linearly. The memory limit could be pushed back by improving the program structure, but honestly i don't see much obvious and easy to implement memory improvement while still knowing the seeds, verticies and edges without recomputing lots of stuff. 

![Image](./figures/example_memory_benchmark.png)
