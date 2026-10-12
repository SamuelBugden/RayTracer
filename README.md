# RayTracer
C++ Ray Tracer with Phong shading, shadows and recursive reflections

![Example render](Spheres%20Output.png)

## Build and run
```
g++ -O2 -o raytracer src/*.cpp
./raytracer Render.bmp
```
The scene is defined in `src/Main.cpp`. The output path is the first argument (default `Render.bmp`).
