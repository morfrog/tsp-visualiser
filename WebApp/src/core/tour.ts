// Port of PATH from Legacy/PATH.C.
//
// The legacy tour is a circular doubly linked list with a `head` node. Here it
// is an array holding the cities in order from the head, so index 0 is the
// head and the last element links back to it. Each operation keeps the array
// in exactly the order the linked list would have, so the algorithms ported
// in later phases scan edges and break ties the same way they did in 1997:
//   - edges are visited from the head: (c[0],c[1]), ..., (c[n-1],c[0]);
//   - insertBetween puts the new city after the first node of the first
//     matching edge;
//   - deleting the head makes the next city the head;
//   - reversals swap city numbers between nodes, as Reversal() does.

import type { DistanceMatrix } from './distances';

/** A city removed with removeTemporarily, and where to put it back. */
export interface RemovedCity {
  city: number;
  /** Neighbours it sat between (Legacy TmpPathDel's A and B). */
  prev: number;
  next: number;
  /** It was the head (Legacy `washead`), so restore makes it the head again. */
  wasHead: boolean;
}

export class Tour {
  private cities: number[];

  constructor(cities: readonly number[] = []) {
    this.cities = [...cities];
  }

  /** Tour 0, 1, ..., n-1: the order cities appear in a .TSP file (LocatesAsPath). */
  static identity(n: number): Tour {
    return new Tour(Array.from({ length: n }, (_, i) => i));
  }

  get size(): number {
    return this.cities.length;
  }

  /** City at a position, counted from the head. Wraps around. */
  at(index: number): number {
    const n = this.cities.length;
    return this.cities[((index % n) + n) % n];
  }

  indexOf(city: number): number {
    return this.cities.indexOf(city);
  }

  has(city: number): boolean {
    return this.cities.includes(city);
  }

  /** City after `city` in the tour. Throws if `city` is not in the tour. */
  next(city: number): number {
    return this.at(this.requireIndex(city) + 1);
  }

  /** City before `city` in the tour. Throws if `city` is not in the tour. */
  prev(city: number): number {
    return this.at(this.requireIndex(city) - 1);
  }

  /** Cities in order from the head (Legacy PathToArray without the circular copy). */
  toArray(): number[] {
    return [...this.cities];
  }

  clone(): Tour {
    return new Tour(this.cities);
  }

  /** Same cities in the same order from the head. */
  equals(other: Tour): boolean {
    return (
      this.cities.length === other.cities.length && this.cities.every((c, i) => c === other.cities[i])
    );
  }

  /** Edges from the head, ending with the edge back to the head. */
  *edges(): Generator<[number, number]> {
    const n = this.cities.length;
    for (let i = 0; i < n; i++) yield [this.cities[i], this.cities[(i + 1) % n]];
  }

  /** Total closed-tour length (Legacy PathLength). */
  length(dist: DistanceMatrix): number {
    let total = 0;
    for (const [a, b] of this.edges()) total += dist.get(a, b);
    return total;
  }

  /** Adds a city just before the head, closing onto it (Legacy InsertPathEnd). */
  append(city: number): void {
    this.cities.push(city);
  }

  /**
   * Puts `city` on the edge between `a` and `b`, in either direction
   * (Legacy InsertPath). Returns false if a and b are not adjacent.
   */
  insertBetween(city: number, a: number, b: number): boolean {
    const n = this.cities.length;
    for (let i = 0; i < n; i++) {
      const here = this.cities[i];
      const after = this.cities[(i + 1) % n];
      if ((here === a && after === b) || (here === b && after === a)) {
        this.cities.splice(i + 1, 0, city);
        return true;
      }
    }
    return false;
  }

  /** Removes a city (Legacy DeletePath). Returns false if it is not in the tour. */
  remove(city: number): boolean {
    const i = this.cities.indexOf(city);
    if (i < 0) return false;
    this.cities.splice(i, 1);
    return true;
  }

  /**
   * Takes a city out so it can be put back unchanged (Legacy TmpPathDel), for
   * trying a move. Returns null if the city is not in the tour.
   */
  removeTemporarily(city: number): RemovedCity | null {
    const index = this.cities.indexOf(city);
    if (index < 0) return null;
    const removed = {
      city,
      prev: this.at(index - 1),
      next: this.at(index + 1),
      wasHead: index === 0,
    };
    this.cities.splice(index, 1);
    return removed;
  }

  /**
   * Puts back a city taken out with removeTemporarily (Legacy TmpPathIns):
   * relinks it between its old neighbours, and makes it the head again if it
   * was the head. Returns false if those neighbours are no longer adjacent.
   */
  restore(removed: RemovedCity): boolean {
    const n = this.cities.length;
    const i = this.cities.indexOf(removed.prev);
    if (i < 0 || this.at(i + 1) !== removed.next) return false;
    if (i === n - 1) {
      // The gap is the edge back to the head.
      if (removed.wasHead) this.cities.unshift(removed.city);
      else this.cities.push(removed.city);
    } else {
      this.cities.splice(i + 1, 0, removed.city);
    }
    return true;
  }

  /**
   * Reverses the cities from position `from` forward to position `to`,
   * wrapping past the end if needed, by swapping city numbers in place the
   * way Legacy Reversal() does. Positions are array indexes.
   */
  reverseRange(from: number, to: number): void {
    const n = this.cities.length;
    const span = (((to - from) % n) + n) % n; // nodes from `from` to `to`, minus one
    const swaps = Math.floor((span + 1) / 2);
    let i = from;
    let j = to;
    for (let s = 0; s < swaps; s++) {
      const tmp = this.cities[i];
      this.cities[i] = this.cities[j];
      this.cities[j] = tmp;
      i = (i + 1) % n;
      j = (j - 1 + n) % n;
    }
  }

  /**
   * Reverses the section subl..subr that sits between keepl and keepr
   * (Legacy ReverseSubPath). Returns false if keepl, subl..subr, keepr are not
   * laid out that way in either direction.
   */
  reverseSubPath(keepl: number, subl: number, subr: number, keepr: number): boolean {
    const a = this.cities.indexOf(subl);
    const b = this.cities.indexOf(subr);
    if (a < 0 || b < 0) return false;
    if (this.at(a - 1) === keepl && this.at(b + 1) === keepr) {
      this.reverseRange(a, b);
    } else if (this.at(a + 1) === keepl && this.at(b - 1) === keepr) {
      this.reverseRange(b, a);
    } else {
      return false;
    }
    return true;
  }

  private requireIndex(city: number): number {
    const i = this.cities.indexOf(city);
    if (i < 0) throw new RangeError(`City ${city} is not in the tour.`);
    return i;
  }
}
