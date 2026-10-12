"""The camera and the grid of pixels it looks through."""

import math

import numpy as np

from .vector import cross, normalise, vec3


class Viewplane:
    """A camera plus the plane of pixels its rays pass through.

    The default constructor builds a look-at camera: the viewplane sits one
    unit in front of the camera, facing look_at, spanning fov degrees
    horizontally. Use Viewplane.axis_aligned for the original fixed setup.
    """

    def __init__(self, width, height, fov, camera, look_at, up=(0, 1, 0)):
        self.width = int(width)
        self.height = int(height)
        if self.width <= 0 or self.height <= 0:
            raise ValueError(f"image size must be positive, got {width} x {height}")
        if not 0 < fov < 180:
            raise ValueError(f"field of view must be between 0 and 180 degrees, got {fov}")
        self.camera = vec3(camera)
        forward = normalise(vec3(look_at) - self.camera)
        self.right = normalise(cross(vec3(up), forward))
        self.up = cross(forward, self.right)
        self.centre = self.camera + forward
        self.plane_width = 2 * math.tan(math.radians(fov) / 2)
        self.plane_height = self.plane_width * self.height / self.width

    @classmethod
    def axis_aligned(cls, width, height, plane_width, plane_height, position, camera):
        """Viewplane centred on position, perpendicular to z, as in the original C++ scene."""
        viewplane = cls.__new__(cls)
        viewplane.width = int(width)
        viewplane.height = int(height)
        viewplane.camera = vec3(camera)
        viewplane.right = vec3(1, 0, 0)
        viewplane.up = vec3(0, 1, 0)
        viewplane.centre = vec3(position)
        viewplane.plane_width = float(plane_width)
        viewplane.plane_height = float(plane_height)
        return viewplane

    def primary_rays(self, first_row, last_row):
        """Rays from the camera through the centre of each pixel in rows [first_row, last_row).

        Returns (origins, directions), each shaped (rows * width, 3), in row-major order.
        """
        pixel_width = self.plane_width / self.width
        pixel_height = self.plane_height / self.height
        i = np.arange(self.width)
        j = np.arange(first_row, last_row)
        x = -self.plane_width / 2 + pixel_width / 2 + i * pixel_width
        y = self.plane_height / 2 - pixel_height / 2 - j * pixel_height

        pixels = (self.centre
                  + x[np.newaxis, :, np.newaxis] * self.right
                  + y[:, np.newaxis, np.newaxis] * self.up).reshape(-1, 3)
        directions = normalise(pixels - self.camera)
        origins = np.broadcast_to(self.camera, directions.shape)
        return origins, directions
