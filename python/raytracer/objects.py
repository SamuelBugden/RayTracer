"""Scene geometry. Every method takes a batch of rays (origins and unit
directions, each shaped (N, 3)) and answers for all of them at once."""

import numpy as np

from .vector import cross, dot, normalise, vec3

# Distance returned for rays that miss, so the nearest hit is a plain argmin
MISS = np.inf


class Sphere:
    def __init__(self, radius, centre, material, name=None):
        if radius <= 0:
            raise ValueError(f"sphere radius must be positive, got {radius}")
        self.radius = float(radius)
        self.centre = vec3(centre)
        self.material = material
        self.name = name

    def distance(self, origins, directions):
        """Distance along each ray to the nearest intersection in front of its origin, or MISS."""
        offset = origins - self.centre
        a = dot(directions, directions)
        b = 2 * dot(directions, offset)
        c = dot(offset, offset) - self.radius ** 2
        discriminant = b * b - 4 * a * c

        hits = discriminant >= 0
        root = np.sqrt(np.where(hits, discriminant, 0))
        near = (-b - root) / (2 * a)
        far = (-b + root) / (2 * a)
        return np.where(hits & (near > 0), near, np.where(hits & (far > 0), far, MISS))

    def normal_at(self, points):
        return normalise(points - self.centre)

    def colour_at(self, points):
        return np.broadcast_to(self.material.colour, points.shape)

    def __repr__(self):
        return f"Sphere(radius={self.radius}, centre={self.centre.tolist()})"


class Plane:
    def __init__(self, normal, point, material, name=None):
        self.normal = normalise(vec3(normal))
        self.point = vec3(point)
        self.material = material
        self.name = name
        self.checker_colour = None
        self.tile_size = 0.0

    def set_checker(self, colour, tile_size):
        """Alternate the material colour with a second colour in square tiles."""
        if tile_size <= 0:
            raise ValueError(f"tile size must be positive, got {tile_size}")
        self.checker_colour = vec3(colour)
        self.tile_size = float(tile_size)

    def distance(self, origins, directions):
        """Distance along each ray to the plane, or MISS if parallel or pointing away."""
        denominator = dot(directions, self.normal)
        facing = np.abs(denominator) > 1e-12
        with np.errstate(divide="ignore", invalid="ignore"):
            t = dot(self.point - origins, self.normal) / denominator
        return np.where(facing & (t > 0), t, MISS)

    def normal_at(self, points):
        return np.broadcast_to(self.normal, points.shape)

    def colour_at(self, points):
        colours = np.broadcast_to(self.material.colour, points.shape)
        if self.checker_colour is None:
            return colours
        # Two axes lying in the plane; count whole tiles along each
        helper = vec3(1, 0, 0) if abs(self.normal[0]) < 0.9 else vec3(0, 1, 0)
        axis_u = normalise(cross(self.normal, helper))
        axis_v = cross(self.normal, axis_u)
        offset = points - self.point
        tile_u = np.floor(dot(offset, axis_u) / self.tile_size)
        tile_v = np.floor(dot(offset, axis_v) / self.tile_size)
        odd = (tile_u + tile_v) % 2 != 0
        return np.where(odd[:, np.newaxis], self.checker_colour, colours)

    def __repr__(self):
        return f"Plane(normal={self.normal.tolist()}, point={self.point.tolist()})"
