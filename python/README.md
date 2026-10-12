# RayTracer (Python 3)

A Python 3 port of the C++ ray tracer in [`../src`](../src): Phong shading, hard shadows from
spheres, recursive reflections, a look-at camera and chequered planes. Rendering the bundled
scene gives the same pixels as the C++ version.

The ray tracing is all written here. The only dependencies are numpy (vector maths over batches of
rays), PyYAML (scene files) and Pillow (writing the image file).

![Example render](../Spheres%20Output.png)

## Setup
```
cd python
pip install -r requirements.txt     # or: pip install .  (adds a `raytracer` command)
```

## Rendering
```
python -m raytracer                              # bundled scene -> Render.png
python -m raytracer raytracer/scenes/spheres.yaml -o room.bmp
python -m raytracer --width 640 --height 480 -j 1
```

| Option | Meaning |
| --- | --- |
| `scene` | YAML scene file (default: the bundled `raytracer/scenes/spheres.yaml`) |
| `-o`, `--output` | Output image; format follows the extension (`.png`, `.bmp`, `.jpg`, ...) |
| `--width`, `--height` | Override the image size from the scene file |
| `--max-depth` | Override the number of reflection bounces |
| `-j`, `--workers` | Processes to render with; `0` (default) uses one per CPU |
| `-q`, `--quiet` | No progress output |

## Scene files
[`raytracer/scenes/spheres.yaml`](raytracer/scenes/spheres.yaml) is the room from `src/Main.cpp` and documents every
field. In short:

```yaml
image:  {width: 1600, height: 1200}
camera: {position: [0, 16, -8], look_at: [0, 3, 38], fov: 55}
render: {max_depth: 5, light_intensity: 2.0}
lights: [[-18, 28, 12]]
spheres:
  - radius: 5
    centre: [-9, 5, 42]
    material: {ambient: 0.15, diffuse: 0.6, specular: 1.0, shininess: 120,
               colour: [225, 225, 230], reflectivity: 0.85}
planes:
  - normal: [0, 1, 0]
    point: [0, 0, 0]
    material: {ambient: 0.15, diffuse: 0.8, specular: 0.3, shininess: 40, colour: [205, 205, 200]}
    checker: {colour: [95, 95, 100], tile_size: 5}
```

Scenes can also be built in code:

```python
from raytracer import Material, Plane, Scene, Sphere, Viewplane
from raytracer.image import save_image

scene = Scene(Viewplane(640, 480, 55, camera=(0, 5, -10), look_at=(0, 2, 10)), lights=[(-10, 20, 0)])
scene.add_sphere(Sphere(2, (0, 2, 10), Material(0.15, 0.9, 0.6, 60, (200, 30, 30), reflectivity=0.2)))
scene.add_plane(Plane((0, 1, 0), (0, 0, 0), Material(0.15, 0.8, 0.3, 40, (205, 205, 200))))
save_image(scene.render(), "red.png")
```

## Layout
| Module | C++ equivalent | Role |
| --- | --- | --- |
| `vector.py` | `Vector3D` | Dot, cross, normalise and reflect over batches of vectors |
| `material.py` | sphere/plane fields | Phong coefficients, colour and reflectivity |
| `objects.py` | `Sphere`, `Plane` | Ray intersection distances, normals, checker colours |
| `camera.py` | `Viewplane` | Look-at camera and primary rays |
| `scene.py` | `Scene` | Nearest hit, shadows, recursive `trace`, parallel `render` |
| `report.py` | `Scene::Engage` output | Scene summary, progress bar, closing report |
| `loader.py` | `Main.cpp` | Builds a `Scene` from YAML |
| `image.py` | EasyBMP | Saves the pixel array |

Rays are traced a block of rows at a time as numpy arrays. `Scene.trace` recurses on the subset of
rays that hit a reflective surface, so each bounce only does work for the rays that need it.

## Tests
```
python -m unittest discover -s tests -t .
```
