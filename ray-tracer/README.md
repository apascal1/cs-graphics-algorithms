# Ray Tracer — Phong Shading, Shadows & Reflections (C++)

This program extends a basic ray tracer to add:

- Diffuse (Lambert) lighting
- Blinn–Phong specular highlights
- Hard shadows via shadow rays
- Single-bounce mirror reflections scaled by Ks

It parses NFF scene files supporting:
- `b` — background color
- `v` — camera (from/at/up/angle/hither/resolution)
- `l` — point lights
- `f r g b Kd Ks e Kt ir` — material properties (this project uses r, g, b, Kd, Ks, e)
- `p n` / `pp n` — polygons, triangulated as a fan (pp normals ignored for flat shading)

Output is a PPM (P3) image.

## Build & Run

A Makefile is provided:

```bash
make
make run
```
