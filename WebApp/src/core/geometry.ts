// Port of the POINT struct and Dist() from Legacy/FILE.C.

/** A city location. Coordinates in the original .TSP files lie in [0, 1]. */
export interface City {
  x: number;
  y: number;
}

/** Euclidean distance between two cities (Legacy `Dist`). */
export function distance(a: City, b: City): number {
  const dx = b.x - a.x;
  const dy = b.y - a.y;
  return Math.sqrt(dx * dx + dy * dy);
}
