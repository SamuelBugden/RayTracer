"""Terminal output: scene summary, progress bar and closing report."""

import sys

RULE = "  " + "-" * 60
BAR_WIDTH = 36


class Report:
    def __init__(self, stream=sys.stdout):
        self.stream = stream
        self._last_percent = None

    def _write(self, text):
        self.stream.write(text)
        self.stream.flush()

    def field(self, label, value):
        self._write(f"  {label:<16}{value}\n")

    def rule(self):
        self._write(RULE + "\n")

    def header(self, scene, workers=None):
        viewplane = scene.viewplane
        self._write("\n  RayTracer\n")
        self.rule()
        self.field("Resolution", f"{viewplane.width} x {viewplane.height} px")
        self.field("Objects", f"{len(scene.spheres)} spheres, {len(scene.planes)} planes")
        self.field("Lights", str(len(scene.lights)))
        self.field("Max bounces", str(scene.max_depth))
        if workers is not None:
            self.field("Workers", str(workers))
        self.rule()

    def progress(self, rows_done, rows_total, seconds):
        """Redraw the bar in place, only when the percentage changes."""
        percent = 100 * rows_done // rows_total
        if percent == self._last_percent:
            return
        self._last_percent = percent

        filled = BAR_WIDTH * rows_done // rows_total
        bar = "=" * filled
        if filled < BAR_WIDTH:
            bar += ">" + " " * (BAR_WIDTH - filled - 1)
        if rows_done < rows_total:
            remaining = seconds * (rows_total - rows_done) / rows_done if rows_done else 0
            timing = f"ETA {remaining:.1f}s"
        else:
            timing = f"done in {seconds:.1f}s"
        self._write(f"\r  Rendering  [{bar}] {percent:>3}%  {timing:<18}")
        if rows_done == rows_total:
            self._write("\n")

    def summary(self, stats):
        self.rule()
        self.field("Render time", f"{stats.seconds:.2f} s")
        self.field("Rays traced", f"{stats.total_rays:,}")
        self.field("  primary", f"{stats.primary_rays:,}")
        self.field("  reflection", f"{stats.reflection_rays:,}")
        self.field("  shadow", f"{stats.shadow_rays:,}")
        if stats.seconds > 0:
            self.field("Throughput", f"{stats.total_rays / stats.seconds / 1e6:.1f} M rays/s")
