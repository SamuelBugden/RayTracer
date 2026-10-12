"""Vector helpers. Vectors are numpy arrays whose last axis has length 3,
so the same functions work on a single vector or on a batch of N vectors."""

import numpy as np


def vec3(x, y=None, z=None):
    """Build a float64 vector from three numbers or from any length-3 sequence."""
    if y is None:
        values = np.asarray(x, dtype=np.float64)
    else:
        values = np.array([x, y, z], dtype=np.float64)
    if values.shape != (3,):
        raise ValueError(f"expected 3 components, got {values.tolist()!r}")
    return values


def dot(a, b):
    """Dot product along the last axis."""
    return np.sum(a * b, axis=-1)


def magnitude(v):
    return np.sqrt(dot(v, v))


def normalise(v):
    """Unit vector(s) pointing the same way as v."""
    return v / magnitude(v)[..., np.newaxis]


def cross(a, b):
    return np.cross(a, b)


def reflect(direction, normal):
    """Mirror direction(s) about normal(s): d - 2(d.n)n."""
    return direction - 2 * dot(direction, normal)[..., np.newaxis] * normal
