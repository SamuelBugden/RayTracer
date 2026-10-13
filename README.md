# RayTracer
C++ Ray Tracer with Phong shading, shadows and recursive reflections

![Example render](Spheres%20Output.png)

## Build and run
```
g++ -O2 -o raytracer src/*.cpp
./raytracer Render.bmp
```
The scene is defined in `src/Main.cpp`. The output path is the first argument (default `Render.bmp`).

A Python 3 port lives in [`python/`](python/README.md).

## VS Code dev container
The `.devcontainer/` folder sets up Python 3.12, numpy/PyYAML/Pillow, g++ and gdb, with the C++, Python and YAML extensions.

1. Install Docker and the VS Code **Dev Containers** extension.
2. Open this folder in VS Code and choose **Reopen in Container** (or run *Dev Containers: Reopen in Container* from the command palette).

Once it's built, use **Terminal → Run Task** for:

| Task | What it does |
| --- | --- |
| C++: build | Compiles `src/` to `build/raytracer` (also `Ctrl+Shift+B`) |
| C++: render | Builds, then renders to `images/cpp_render.bmp` |
| Python: render | Renders the YAML scene to `images/python_render.png` |
| Python: render (preview, 640x480) | A quicker, smaller render |
| Python: tests | Runs the unittest suite (also in the Testing panel) |

The **Run and Debug** panel has *C++: debug render* (gdb) and *Python: debug render* configurations. Open a render from the Explorer to preview it.
