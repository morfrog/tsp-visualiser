// Port of the DISTANCES matrix from Legacy/FILE.C (NewMap).

import { distance, type City } from './geometry';

/** Symmetric matrix of distances between every pair of cities. */
export class DistanceMatrix {
  readonly size: number;
  private readonly values: Float64Array;

  constructor(readonly cities: readonly City[]) {
    const n = cities.length;
    this.size = n;
    this.values = new Float64Array(n * n);
    for (let i = 0; i < n; i++) {
      for (let j = i; j < n; j++) {
        const d = distance(cities[i], cities[j]);
        this.values[i * n + j] = d;
        this.values[j * n + i] = d;
      }
    }
  }

  /** Distance between cities a and b (Legacy `dist->array[a][b]`). */
  get(a: number, b: number): number {
    return this.values[a * this.size + b];
  }

  /** Extra length from putting city i between a and b (Legacy InsertCost). */
  insertCost(i: number, a: number, b: number): number {
    return this.get(a, i) + this.get(i, b) - this.get(a, b);
  }
}
