# Seam Carving — Content-Aware Image Resizing (C++)

This program implements the seam carving algorithm for image resizing,
as presented in the SIGGRAPH 2007 paper *"Seam Carving for Content-Aware Image 
Resizing"* by Shai Avidan and Ariel Shamir. The algorithm preserves 
important content while removing low-energy seams from the image.

## Build & Run

```bash
make
```

**Option 1 — direct:**
```bash
./seamcarving input.png output.png 400 400
```

**Option 2 — via make:**
```bash
make run input=image.png output=out.jpg width=400 height=400
```

**Clean up generated files:**
```bash
make clean
```

## Notes

- No spaces around `=` when using `make run` (e.g. `width=400`, not `width = 400`)
- `output` can be `.png` or `.jpg`
