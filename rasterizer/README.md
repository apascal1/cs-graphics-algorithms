# Rasterizer — Triangle Rasterization with Depth Buffering (C++)

This program implements a rasterization-based renderer (as opposed to ray tracing), using:

- Perspective projection via camera and projection matrices
- Triangle rasterization using the bounding-box + edge function method
- Barycentric coordinate interpolation for depth, position, and normals
- Depth buffering (z-buffer) for correct visibility
- Smooth shading via interpolated vertex normals (when available), with flat shading as a fallback
- Per-pixel Phong lighting (ambient, diffuse, and specular)

It parses NFF scene files supporting:
- `b` — background color
- `v` — camera (from/at/up/angle/hither/resolution)
- `l` — point lights
- `f r g b Kd Ks e Kt ir` — material properties
- `p n` / `pp n` — polygons, triangulated as a fan (pp includes per-vertex normals for smooth shading)

Output is a PPM (P3) image.

## Build & Run

A Makefile is provided:

```bash
make
make run
```

## Notes

Shading mode (smooth vs. flat) is controlled by the `smoothShading` flag in `main()`, and falls back to flat shading automatically if a triangle has no vertex normals.
