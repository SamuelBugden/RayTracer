"""Saving rendered images. The format follows the file extension (.png, .bmp, .jpg, ...)."""

from pathlib import Path

from PIL import Image


def save_image(pixels, path):
    """Write an (height, width, 3) uint8 RGB array to path."""
    path = Path(path)
    if path.parent != Path("."):
        path.parent.mkdir(parents=True, exist_ok=True)
    Image.fromarray(pixels, mode="RGB").save(path)
    return path
