"""Scene assembly and the recursive trace."""

import os
import time
from concurrent.futures import ProcessPoolExecutor
from dataclasses import dataclass

import numpy as np

from .objects import MISS, Plane, Sphere
from .vector import dot, magnitude, normalise, reflect, vec3

# Secondary rays start this far off the surface so they don't immediately re-hit it
EPSILON = 1e-6

# Roughly how many primary rays to trace per batch: big enough for numpy to be
# efficient, small enough to keep memory modest and the progress bar moving
RAYS_PER_BATCH = 1 << 16


@dataclass
class RenderStats:
    seconds: float = 0.0
    primary_rays: int = 0
    reflection_rays: int = 0
    shadow_rays: int = 0

    @property
    def total_rays(self):
        return self.primary_rays + self.reflection_rays + self.shadow_rays

    def add(self, other):
        self.primary_rays += other.primary_rays
        self.reflection_rays += other.reflection_rays
        self.shadow_rays += other.shadow_rays


class Scene:
    def __init__(self, viewplane, lights=(), light_intensity=2.0, max_depth=5):
        self.viewplane = viewplane
        self.spheres = []
        self.planes = []
        self.lights = [vec3(light) for light in lights]
        self.light_intensity = float(light_intensity)
        self.max_depth = int(max_depth)
        self.stats = RenderStats()

    #------------------------------------------------------------ Scene population

    def add_sphere(self, sphere):
        if not isinstance(sphere, Sphere):
            raise TypeError(f"expected a Sphere, got {type(sphere).__name__}")
        self.spheres.append(sphere)

    def add_plane(self, plane):
        if not isinstance(plane, Plane):
            raise TypeError(f"expected a Plane, got {type(plane).__name__}")
        self.planes.append(plane)

    def add_light(self, position):
        self.lights.append(vec3(position))

    @property
    def objects(self):
        # Spheres first, matching the C++ search order when two hits tie
        return self.spheres + self.planes

    #------------------------------------------------------------ Ray-scene calculations

    def nearest_surface(self, origins, directions):
        """Distance to, and index into self.objects of, the nearest hit for each ray."""
        distances = np.stack([obj.distance(origins, directions) for obj in self.objects])
        index = np.argmin(distances, axis=0)
        return distances[index, np.arange(len(index))], index

    def in_shadow(self, points, light):
        """Whether a sphere sits between each point and the light.

        Only spheres cast shadows: the planes form the room's walls.
        """
        self.stats.shadow_rays += len(points)
        to_light = light - points
        distance_to_light = magnitude(to_light)
        directions = to_light / distance_to_light[:, np.newaxis]
        blocked = np.zeros(len(points), dtype=bool)
        for sphere in self.spheres:
            blocked |= sphere.distance(points, directions) < distance_to_light
        return blocked

    def trace(self, origins, directions, depth=0):
        """Colour (0-255 RGB) seen along each ray, following reflections up to max_depth bounces."""
        colours = np.zeros((len(directions), 3))
        if not self.objects:
            return colours
        distance, index = self.nearest_surface(origins, directions)
        hit = distance != MISS
        if not hit.any():
            return colours
        origins, directions, distance, index = origins[hit], directions[hit], distance[hit], index[hit]
        points = origins + distance[:, np.newaxis] * directions

        #-------------------------------------------------------- Gather each hit's surface properties
        objects = self.objects
        normals = np.empty_like(points)
        base_colour = np.empty_like(points)
        for k, obj in enumerate(objects):
            mask = index == k
            if mask.any():
                normals[mask] = obj.normal_at(points[mask])
                base_colour[mask] = obj.colour_at(points[mask])
        ambient, diffuse_k, specular_k, shininess, reflectivity = (
            np.array([getattr(obj.material, field) for obj in objects])[index]
            for field in ("ambient", "diffuse", "specular", "shininess", "reflectivity"))

        # Shade the side of the surface the ray arrived from
        normals = np.where((dot(normals, directions) > 0)[:, np.newaxis], -normals, normals)
        reflected = reflect(directions, normals)
        surface_points = points + EPSILON * normals

        #-------------------------------------------------------- Phong shading, with the total light intensity shared between the lights
        diffuse = np.zeros(len(points))
        specular = np.zeros(len(points))
        if self.lights:
            intensity = self.light_intensity / len(self.lights)
            for light in self.lights:
                lit = ~self.in_shadow(surface_points, light)
                to_light = normalise(light - points)
                diffuse += np.where(lit, intensity * diffuse_k * np.maximum(0, dot(to_light, normals)), 0)
                specular += np.where(lit, specular_k * np.maximum(0, dot(to_light, reflected)) ** shininess, 0)
        local = np.minimum(255, base_colour * (ambient + diffuse)[:, np.newaxis] + 255 * specular[:, np.newaxis])

        #-------------------------------------------------------- Reflections: trace the reflected rays and blend them in
        bounce = (reflectivity > 0) & (depth < self.max_depth)
        if bounce.any():
            self.stats.reflection_rays += int(bounce.sum())
            bounced = self.trace(surface_points[bounce], reflected[bounce], depth + 1)
            k = reflectivity[bounce][:, np.newaxis]
            local[bounce] = (1 - k) * local[bounce] + k * bounced

        colours[hit] = local
        return colours

    #------------------------------------------------------------ Image render

    def render_rows(self, first_row, last_row):
        """Render rows [first_row, last_row) to uint8 pixels, with the ray counts it took."""
        self.stats = RenderStats()
        origins, directions = self.viewplane.primary_rays(first_row, last_row)
        self.stats.primary_rays = len(directions)
        colours = self.trace(origins, directions)
        pixels = np.clip(colours, 0, 255).astype(np.uint8).reshape(-1, self.viewplane.width, 3)
        return pixels, self.stats

    def render(self, progress=None, workers=1):
        """Render the scene to an (height, width, 3) uint8 RGB array.

        workers > 1 renders blocks of rows in that many processes (0 means one per CPU).
        progress, if given, is called as progress(rows_done, rows_total, seconds_elapsed).
        """
        width, height = self.viewplane.width, self.viewplane.height
        image = np.zeros((height, width, 3), dtype=np.uint8)
        workers = workers or os.cpu_count() or 1
        rows_per_batch = max(1, RAYS_PER_BATCH // width)
        if workers > 1:
            # Several batches per worker so they finish at about the same time
            rows_per_batch = max(1, min(rows_per_batch, height // (workers * 4)))
        batches = [(first, min(height, first + rows_per_batch)) for first in range(0, height, rows_per_batch)]
        totals = RenderStats()
        rows_done = 0

        start = time.perf_counter()
        if progress:
            progress(0, height, 0.0)

        def store(batch, result):
            nonlocal rows_done
            pixels, stats = result
            image[batch[0]:batch[1]] = pixels
            totals.add(stats)
            rows_done += batch[1] - batch[0]
            if progress:
                progress(rows_done, height, time.perf_counter() - start)

        if workers == 1:
            for batch in batches:
                store(batch, self.render_rows(*batch))
        else:
            with ProcessPoolExecutor(max_workers=workers, initializer=_init_worker, initargs=(self,)) as pool:
                for batch, result in zip(batches, pool.map(_render_rows_in_worker, batches)):
                    store(batch, result)

        totals.seconds = time.perf_counter() - start
        self.stats = totals
        return image


#---------------------------------------------------------------- Worker processes each get their own copy of the scene

_worker_scene = None


def _init_worker(scene):
    global _worker_scene
    _worker_scene = scene


def _render_rows_in_worker(batch):
    return _worker_scene.render_rows(*batch)
