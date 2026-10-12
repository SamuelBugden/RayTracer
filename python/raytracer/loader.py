"""Build a Scene from a YAML description. See raytracer/scenes/spheres.yaml for the format."""

from pathlib import Path

import yaml

from .camera import Viewplane
from .material import Material
from .objects import Plane, Sphere
from .scene import Scene


class SceneError(ValueError):
    """The scene file is missing a field or has a bad value."""


def _require(mapping, key, where):
    if not isinstance(mapping, dict):
        raise SceneError(f"{where}: expected a mapping, got {type(mapping).__name__}")
    if key not in mapping:
        raise SceneError(f"{where}: missing '{key}'")
    return mapping[key]


def _material(entry, where):
    spec = _require(entry, "material", where)
    where = f"{where}.material"
    try:
        return Material(
            ambient=float(_require(spec, "ambient", where)),
            diffuse=float(_require(spec, "diffuse", where)),
            specular=float(_require(spec, "specular", where)),
            shininess=float(_require(spec, "shininess", where)),
            colour=_require(spec, "colour", where),
            reflectivity=float(spec.get("reflectivity", 0.0)),
        )
    except SceneError:
        raise
    except (TypeError, ValueError) as error:
        raise SceneError(f"{where}: {error}") from None


def build_scene(description, width=None, height=None):
    """Build a Scene from an already-parsed YAML mapping. width/height override the file."""
    image = description.get("image", {})
    camera = _require(description, "camera", "scene")
    position = _require(camera, "position", "camera")
    look_at = _require(camera, "look_at", "camera")
    try:
        viewplane = Viewplane(
            width or image.get("width", 640),
            height or image.get("height", 480),
            fov=float(camera.get("fov", 55)),
            camera=position,
            look_at=look_at,
            up=camera.get("up", (0, 1, 0)),
        )
    except ValueError as error:
        raise SceneError(f"camera: {error}") from None

    settings = description.get("render", {})
    scene = Scene(
        viewplane,
        lights=description.get("lights", []),
        light_intensity=settings.get("light_intensity", 2.0),
        max_depth=settings.get("max_depth", 5),
    )

    for n, entry in enumerate(description.get("spheres", [])):
        where = f"spheres[{n}]"
        try:
            scene.add_sphere(Sphere(_require(entry, "radius", where), _require(entry, "centre", where),
                                    _material(entry, where), name=entry.get("name")))
        except SceneError:
            raise
        except ValueError as error:
            raise SceneError(f"{where}: {error}") from None

    for n, entry in enumerate(description.get("planes", [])):
        where = f"planes[{n}]"
        try:
            plane = Plane(_require(entry, "normal", where), _require(entry, "point", where),
                          _material(entry, where), name=entry.get("name"))
            if "checker" in entry:
                checker = entry["checker"]
                plane.set_checker(_require(checker, "colour", f"{where}.checker"),
                                  _require(checker, "tile_size", f"{where}.checker"))
        except SceneError:
            raise
        except ValueError as error:
            raise SceneError(f"{where}: {error}") from None
        scene.add_plane(plane)

    return scene


def load_scene(path, width=None, height=None):
    with open(Path(path), encoding="utf-8") as file:
        description = yaml.safe_load(file)
    if not isinstance(description, dict):
        raise SceneError(f"{path}: expected a YAML mapping at the top level")
    return build_scene(description, width, height)
