// The main screen's New and Randomize buttons.

import type { City } from './geometry';
import { RAND_MAX, randomInt, type Rng } from './random';

/**
 * A map of `size` cities placed uniformly at random in the unit square
 * (Legacy MakeMap). The original wrote each coordinate as "%8.6f" and read it
 * back, so coordinates are rounded to 6 decimals here too.
 */
export function makeMap(size: number, rng: Rng): City[] {
  const coordinate = () => Number(((rng.rand() / RAND_MAX) * 1.0).toFixed(6));
  return Array.from({ length: size }, () => {
    const x = coordinate();
    const y = coordinate();
    return { x, y };
  });
}

/**
 * The same cities in a random order (Legacy rndfile). Since a map's order is
 * its tour, this gives a random tour. The original shuffled twice, swapping a
 * random pick into the last slot of the whole array each time.
 */
export function randomizeOrder(cities: readonly City[], rng: Rng): City[] {
  const n = cities.length;
  const order = Array.from({ length: n }, (_, i) => i);
  for (let times = 0; times < 2; times++) {
    for (let x = 0; x < n; x++) {
      const pos = randomInt(rng, n - x);
      const tmp = order[n - 1];
      order[n - 1] = order[pos];
      order[pos] = tmp;
    }
  }
  return order.map((i) => ({ ...cities[i] }));
}

/** Cities reordered so the file order is `tour` (Legacy PathAsLocates). */
export function citiesInTourOrder(cities: readonly City[], tour: readonly number[]): City[] {
  return tour.map((i) => ({ ...cities[i] }));
}
