"""Command line entry point: python -m raytracer [scene.yaml] [-o output.png]"""

import argparse
import os
import sys
from pathlib import Path

from .image import save_image
from .loader import SceneError, load_scene
from .report import Report

DEFAULT_SCENE = Path(__file__).resolve().parent / "scenes" / "spheres.yaml"


def main(argv=None):
    parser = argparse.ArgumentParser(prog="raytracer", description="Render a YAML scene to an image.")
    parser.add_argument("scene", nargs="?", default=DEFAULT_SCENE, type=Path,
                        help=f"scene file (default: {DEFAULT_SCENE.name})")
    parser.add_argument("-o", "--output", default="Render.png", type=Path,
                        help="output image; the format follows the extension (default: Render.png)")
    parser.add_argument("--width", type=int, help="override the image width in pixels")
    parser.add_argument("--height", type=int, help="override the image height in pixels")
    parser.add_argument("--max-depth", type=int, help="override the maximum number of reflection bounces")
    parser.add_argument("-j", "--workers", type=int, default=0,
                        help="processes to render with (default: 0, one per CPU)")
    parser.add_argument("-q", "--quiet", action="store_true", help="print nothing except errors")
    args = parser.parse_args(argv)

    try:
        scene = load_scene(args.scene, args.width, args.height)
    except (OSError, SceneError) as error:
        print(f"raytracer: {error}", file=sys.stderr)
        return 1
    if args.max_depth is not None:
        scene.max_depth = args.max_depth

    report = None if args.quiet else Report()
    if report:
        report.header(scene, workers=args.workers or os.cpu_count())
    pixels = scene.render(progress=report.progress if report else None, workers=args.workers)
    if report:
        report.summary(scene.stats)

    try:
        saved = save_image(pixels, args.output)
    except (OSError, ValueError) as error:
        print(f"raytracer: could not write {args.output}: {error}", file=sys.stderr)
        return 1
    if report:
        report.field("Saved to", str(saved))
        print()
    return 0


if __name__ == "__main__":
    sys.exit(main())
