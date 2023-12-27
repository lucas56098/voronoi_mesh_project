# Voronoi Mesh Project - vmp
Small C++ project to generate a voronoi mesh using a naive halfplane intersection and point insertion algorithm. Optional also some visualisaition and benchmarking tools are available. This project was done during an undergraduate project internship in the Group of Dr. Dylan Nelson at ITA Heidelberg.

## Introduction
Voronoi meshes can be used in many applications. One such application for example are cosmological hydrodynamical simulations 
like [IllustrisTNG](https://www.tng-project.org) based on codes like [AREPO](https://github.com/dnelson86/arepo), where a moving voronoi mesh is used as a grid for the hydrodynamics and therefore needs to be regenerated for every timestep. While moving vornoi meshes in a compromise between SPH and AMR help with shock treatment and also make the code gallilean invariant this of course comes at a substential computing cost. Fast and reliable algorithms for generating voronoi meshes in 3D therefore are very useful to do such simulations. Due to the limited time of my project internship we however sticked to 2D algorithms.
The voronoi mesh generation algorithms can be divided into direct and indirect algorithms, where the indirect algorithms first generate a delunay triangulation and then use the geometric duality to the voronoi tesselation to construct the mesh. While codes like [AREPO](https://github.com/dnelson86/arepo) work using an indirect approach we focused on directly generating a voronoi mesh.
The two algorithms we looked at are a naive halfplane intersection algorithm which at best scales with $ \mathcal{O}(n^2) $

###
This sentence uses `$` delimiters to show math inline:  $\sqrt{3x-1}+(1+x)^2$
## Getting started

## Run options