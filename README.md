# voronoi_mesh_project
Small C++ Project to generate a voronoi mesh in the most naive way possible. Still work in progress...

### Later on
- Improve performance (do profiling)
- Other algorithms and comparison...

![til](./figures/example_voronoi_animation.gif)

### Performance
At the moment the program is still very slow. The further goal is to see wether this can be optimized or is intrinsic to the naive approach. Here you can see some benchmarking on my machine MacBook M1 running the debug and release version. For the third line some memory management is introduced (e.g. all halfplanes and points that are no edges or seed get deleted once cell is constructed)
