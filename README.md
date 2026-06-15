# CS Graphics & Algorithms

A collection of C++ projects exploring computer graphics rendering techniques and 
algorithmic problem solving, built using Eigen for linear algebra.

## Projects

### [Ray Tracer — Phong Shading, Shadows & Reflections](./ray-tracer)
A ray tracer implementing diffuse and specular lighting, hard shadows via shadow rays, 
and single-bounce mirror reflections. Parses NFF scene files and outputs PPM images.

### [Rasterizer — Triangle Rasterization with Depth Buffering](./rasterizer)
A rasterization-based renderer using the bounding-box/edge-function method, 
barycentric interpolation, z-buffering, and per-pixel Phong lighting with smooth 
or flat shading.

### [Seam Carving — Content-Aware Image Resizing](./seam-carving)
An implementation of the SIGGRAPH 2007 seam carving algorithm for content-aware 
image resizing using dynamic programming.

## Additional Work

- **3D Boids Simulation with KD-Tree Optimization** — multi-agent flocking simulation 
  (separation, alignment, cohesion) with spatial indexing for neighbor queries.
- **Laplacian Mesh Smoothing** — iterative mesh smoothing via Laplacian operators 
  on OBJ files.

## Tech

C++ • Eigen • CImg
