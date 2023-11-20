# voronoi_mesh_project
Small C++ Project to generate a voronoi mesh in the most naive way possible. Still work in progress...

### Later on
- Further improve performance (do further profiling)
- Other algorithms and comparison...

![til](./figures/example_voronoi_animation.gif)

### Performance
At the moment the program is still very slow. The further goal is to see wether this can be optimized or is intrinsic to the naive approach. Here you can see some benchmarking on my machine (MacBook M1) running the debug and release version. For the green line some memory management is introduced (e.g. all halfplanes and points that are no edges or seed get deleted once cell is fully constructed). For the red line the algorithm was improved at its core to improve from ~O(n^3) scaling to ~O(n^2) scaling. The main difference is, that the halfplanes don't get intersected all at the beginning of construct cell (~n-steps). Instead only for the current halfplane the intersections are calculated when needed (steps proportional to nr of edges of cell << n). This also allows to delete the intersections from memory once the vertex is identified. Another improvement is that for some lists that aren't accessed frequently but store huge objects (like the vcells list) the data type was changed from vector to deque which prevents copying lots of data. Of course this slows down the correctness checks slightly. At the moment the program runs stable up to around 15000 points on my machine. After that some memory limit is reached. I guess ill have to further improve that.

![Image](./figures/example_benchmark.png)
