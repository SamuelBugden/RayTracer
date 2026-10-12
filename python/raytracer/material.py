"""Surface properties shared by spheres and planes."""

from dataclasses import dataclass

import numpy as np


@dataclass
class Material:
    """Phong material.

    ambient, diffuse and specular are the k_a, k_d and k_s coefficients,
    shininess is the specular exponent (alpha), colour is RGB in 0-255 and
    reflectivity (0-1) is how much of the reflected ray is blended in.
    """

    ambient: float
    diffuse: float
    specular: float
    shininess: float
    colour: tuple
    reflectivity: float = 0.0

    def __post_init__(self):
        self.colour = np.asarray(self.colour, dtype=np.float64)
        if self.colour.shape != (3,):
            raise ValueError(f"colour needs 3 components, got {self.colour.tolist()!r}")
        if not 0 <= self.reflectivity <= 1:
            raise ValueError(f"reflectivity must be between 0 and 1, got {self.reflectivity}")
