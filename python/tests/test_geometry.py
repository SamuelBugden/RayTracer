import unittest

import numpy as np

from raytracer import Material, Plane, Sphere, Viewplane
from raytracer.objects import MISS
from raytracer.vector import normalise, reflect, vec3

MATTE = Material(ambient=0.1, diffuse=0.9, specular=0.0, shininess=1, colour=(200, 100, 50))


def rays(*pairs):
    origins = np.array([p[0] for p in pairs], dtype=float)
    directions = normalise(np.array([p[1] for p in pairs], dtype=float))
    return origins, directions


class VectorTests(unittest.TestCase):
    def test_normalise_batch(self):
        v = normalise(np.array([[3.0, 0, 4], [0, 2, 0]]))
        np.testing.assert_allclose(v, [[0.6, 0, 0.8], [0, 1, 0]])

    def test_reflect_off_floor(self):
        np.testing.assert_allclose(reflect(vec3(1, -1, 0), vec3(0, 1, 0)), [1, 1, 0])

    def test_vec3_rejects_wrong_length(self):
        with self.assertRaises(ValueError):
            vec3([1, 2])


class SphereTests(unittest.TestCase):
    def setUp(self):
        self.sphere = Sphere(1, (0, 0, 5), MATTE)

    def test_distance_to_near_side(self):
        t = self.sphere.distance(*rays(((0, 0, 0), (0, 0, 1))))
        np.testing.assert_allclose(t, [4])

    def test_miss_and_behind(self):
        t = self.sphere.distance(*rays(((0, 2, 0), (0, 0, 1)), ((0, 0, 10), (0, 0, 1))))
        self.assertTrue(np.all(t == MISS))

    def test_from_inside_hits_far_side(self):
        t = self.sphere.distance(*rays(((0, 0, 5), (0, 0, 1))))
        np.testing.assert_allclose(t, [1])

    def test_normal_points_outward(self):
        np.testing.assert_allclose(self.sphere.normal_at(np.array([[0, 0, 4.0]])), [[0, 0, -1]])

    def test_rejects_bad_radius(self):
        with self.assertRaises(ValueError):
            Sphere(0, (0, 0, 0), MATTE)


class PlaneTests(unittest.TestCase):
    def setUp(self):
        self.floor = Plane((0, 2, 0), (0, -1, 0), MATTE)

    def test_normal_is_normalised(self):
        np.testing.assert_allclose(self.floor.normal, [0, 1, 0])

    def test_distance(self):
        t = self.floor.distance(*rays(((0, 1, 0), (0, -1, 0)), ((0, 1, 0), (0, 1, 0)), ((0, 1, 0), (1, 0, 0))))
        self.assertAlmostEqual(t[0], 2)
        self.assertEqual(t[1], MISS)  # pointing away
        self.assertEqual(t[2], MISS)  # parallel

    def test_checker_alternates(self):
        self.floor.set_checker((0, 0, 0), 1)
        colours = self.floor.colour_at(np.array([[0.5, -1, 0.5], [1.5, -1, 0.5], [1.5, -1, 1.5]]))
        self.assertFalse(np.array_equal(colours[0], colours[1]))
        np.testing.assert_array_equal(colours[0], colours[2])
        self.assertEqual({tuple(c) for c in colours}, {(200, 100, 50), (0, 0, 0)})


class ViewplaneTests(unittest.TestCase):
    def test_centre_ray_points_at_target(self):
        viewplane = Viewplane(3, 3, 60, camera=(1, 2, 3), look_at=(4, 2, 7))
        _, directions = viewplane.primary_rays(1, 2)
        np.testing.assert_allclose(directions[1], normalise(vec3(3, 0, 4)))

    def test_rows_run_top_to_bottom_and_columns_left_to_right(self):
        viewplane = Viewplane(4, 2, 90, camera=(0, 0, 0), look_at=(0, 0, 1))
        _, directions = viewplane.primary_rays(0, 2)
        grid = directions.reshape(2, 4, 3)
        self.assertGreater(grid[0, 0, 1], grid[1, 0, 1])  # top row looks higher
        self.assertLess(grid[0, 0, 0], grid[0, 3, 0])     # left column looks further left

    def test_field_of_view(self):
        viewplane = Viewplane(2, 1, 90, camera=(0, 0, 0), look_at=(0, 0, 1))
        self.assertAlmostEqual(viewplane.plane_width, 2)

    def test_axis_aligned_matches_original_layout(self):
        viewplane = Viewplane.axis_aligned(1, 1, 5, 5, position=(0, -5, 5), camera=(0, -5, 1))
        _, directions = viewplane.primary_rays(0, 1)
        np.testing.assert_allclose(directions, [[0, 0, 1]])

    def test_rejects_bad_settings(self):
        with self.assertRaises(ValueError):
            Viewplane(0, 10, 60, (0, 0, 0), (0, 0, 1))
        with self.assertRaises(ValueError):
            Viewplane(10, 10, 180, (0, 0, 0), (0, 0, 1))


if __name__ == "__main__":
    unittest.main()
