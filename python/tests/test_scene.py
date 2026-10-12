import tempfile
import unittest
from pathlib import Path

import numpy as np
from PIL import Image

from raytracer import Material, Plane, Scene, Sphere, Viewplane
from raytracer.__main__ import DEFAULT_SCENE, main
from raytracer.loader import SceneError, build_scene, load_scene
from raytracer.vector import normalise


def one_ray(origin, direction):
    return np.array([origin], dtype=float), normalise(np.array([direction], dtype=float))


def camera():
    return Viewplane(8, 6, 60, camera=(0, 0, 0), look_at=(0, 0, 1))


class ShadingTests(unittest.TestCase):
    def test_empty_scene_is_black(self):
        scene = Scene(camera(), lights=[(0, 0, 0)])
        np.testing.assert_array_equal(scene.trace(*one_ray((0, 0, 0), (0, 0, 1))), [[0, 0, 0]])

    def test_head_on_light_gives_ambient_plus_diffuse(self):
        material = Material(ambient=0.2, diffuse=0.5, specular=0.0, shininess=1, colour=(100, 100, 100))
        scene = Scene(camera(), lights=[(0, 0, 0)], light_intensity=1.0)
        scene.add_sphere(Sphere(1, (0, 0, 5), material))
        np.testing.assert_allclose(scene.trace(*one_ray((0, 0, 0), (0, 0, 1))), [[70, 70, 70]])

    def test_specular_highlight_adds_white(self):
        material = Material(ambient=0.0, diffuse=0.0, specular=0.5, shininess=10, colour=(100, 0, 0))
        scene = Scene(camera(), lights=[(0, 0, 0)])
        scene.add_sphere(Sphere(1, (0, 0, 5), material))
        np.testing.assert_allclose(scene.trace(*one_ray((0, 0, 0), (0, 0, 1))), [[127.5, 127.5, 127.5]])

    def test_sphere_casts_shadow(self):
        floor = Plane((0, 1, 0), (0, 0, 0), Material(0.2, 0.8, 0, 1, (100, 100, 100)))
        blocker = Sphere(1, (0, 5, 0), Material(0.2, 0.8, 0, 1, (0, 0, 0)))
        scene = Scene(camera(), lights=[(0, 10, 0)], light_intensity=1.0)
        scene.add_plane(floor)
        lit = scene.trace(*one_ray((0, 1, 0), (0, -1, 0)))
        scene.add_sphere(blocker)
        # Look at the floor from below the blocker; only ambient light remains
        shadowed = scene.trace(*one_ray((0, 0.5, 0.01), (0, -1, 0)))
        np.testing.assert_allclose(lit, [[100, 100, 100]])
        np.testing.assert_allclose(shadowed, [[20, 20, 20]])

    def test_mirror_shows_what_it_reflects(self):
        mirror = Plane((0, 0, -1), (0, 0, 10), Material(0, 0, 0, 1, (0, 0, 0), reflectivity=1.0))
        behind = Plane((0, 0, 1), (0, 0, -10), Material(1, 0, 0, 1, (10, 20, 30)))
        scene = Scene(camera(), lights=[(0, 0, 0)])
        scene.add_plane(mirror)
        scene.add_plane(behind)
        np.testing.assert_allclose(scene.trace(*one_ray((0, 0, 0), (0, 0, 1))), [[10, 20, 30]])

    def test_max_depth_limits_bounces(self):
        mirror = Material(0, 0, 0, 1, (0, 0, 0), reflectivity=1.0)
        scene = Scene(camera(), lights=[(0, 0, 0)], max_depth=3)
        scene.add_plane(Plane((0, 0, -1), (0, 0, 1), mirror))
        scene.add_plane(Plane((0, 0, 1), (0, 0, -1), mirror))
        scene.trace(*one_ray((0, 0, 0), (0, 0, 1)))
        self.assertEqual(scene.stats.reflection_rays, 3)


class RenderTests(unittest.TestCase):
    def test_parallel_render_matches_serial(self):
        scene = load_scene(DEFAULT_SCENE, width=48, height=36)
        serial = scene.render(workers=1)
        serial_rays = scene.stats.total_rays
        parallel = scene.render(workers=2)
        np.testing.assert_array_equal(serial, parallel)
        self.assertEqual(scene.stats.total_rays, serial_rays)
        self.assertEqual(scene.stats.primary_rays, 48 * 36)

    def test_command_line_writes_image(self):
        with tempfile.TemporaryDirectory() as folder:
            output = Path(folder) / "out" / "render.png"
            status = main([str(DEFAULT_SCENE), "-o", str(output), "--width", "32", "--height", "24", "-j", "1", "-q"])
            self.assertEqual(status, 0)
            with Image.open(output) as image:
                self.assertEqual(image.size, (32, 24))
                self.assertGreater(np.asarray(image).mean(), 20)


class LoaderTests(unittest.TestCase):
    MINIMAL = {
        "camera": {"position": [0, 0, 0], "look_at": [0, 0, 1]},
        "spheres": [{"radius": 1, "centre": [0, 0, 5],
                     "material": {"ambient": 0.1, "diffuse": 0.9, "specular": 0.5, "shininess": 20, "colour": [255, 0, 0]}}],
    }

    def test_bundled_scene(self):
        scene = load_scene(DEFAULT_SCENE)
        self.assertEqual((len(scene.spheres), len(scene.planes), len(scene.lights)), (7, 6, 3))
        self.assertEqual((scene.viewplane.width, scene.viewplane.height), (1600, 1200))
        self.assertIsNotNone(scene.planes[0].checker_colour)

    def test_size_override_and_defaults(self):
        scene = build_scene(self.MINIMAL, width=10, height=5)
        self.assertEqual((scene.viewplane.width, scene.viewplane.height), (10, 5))
        self.assertEqual(scene.max_depth, 5)
        self.assertEqual(scene.spheres[0].material.reflectivity, 0)

    def test_missing_field_names_its_location(self):
        description = {"camera": {"position": [0, 0, 0], "look_at": [0, 0, 1]},
                       "spheres": [{"radius": 1, "centre": [0, 0, 5], "material": {"ambient": 0.1}}]}
        with self.assertRaisesRegex(SceneError, r"^spheres\[0\]\.material: missing 'diffuse'$"):
            build_scene(description)

    def test_bad_value_is_reported(self):
        description = dict(self.MINIMAL, planes=[{"normal": [0, 1], "point": [0, 0, 0],
                                                   "material": self.MINIMAL["spheres"][0]["material"]}])
        with self.assertRaisesRegex(SceneError, r"^planes\[0\]: expected 3 components"):
            build_scene(description)


if __name__ == "__main__":
    unittest.main()
