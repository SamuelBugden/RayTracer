"""A small recursive ray tracer: Phong shading, hard shadows and reflections.

Python port of the C++ ray tracer in ../src. Rays are traced in batches with
numpy, but each step mirrors the C++ Scene::Trace.
"""

from .camera import Viewplane
from .material import Material
from .objects import Plane, Sphere
from .scene import RenderStats, Scene

__all__ = ["Material", "Plane", "RenderStats", "Scene", "Sphere", "Viewplane"]
