// Port of the construction methods in Legacy/INSERT.C: insertion (three
// starts x seven insertion rules) and Nearest Neighbour.
//
// Tie-breaking follows the original exactly, because the benchmark tables in
// DOCO.DOC depend on it:
//   - comparisons are strict (Legacy Lesser/Greater), so the first candidate
//     found wins a tie;
//   - cities not yet in the tour live in `out`, scanned from index 0 up to
//     `remaining`. Removing a city moves the last live entry into its slot
//     (Legacy CityDel), so the scan order changes as the tour grows;
//   - tour edges are scanned from the head (Legacy PathStart/PathNext).

import { Ega, Role, type EgaColor } from '../core/colors';
import type { DistanceMatrix } from '../core/distances';
import { randomInt, type Rng } from '../core/random';
import { Tour } from '../core/tour';
import type { CityMark, Frame, Line, MethodRun, TextUpdate } from './frame';
import { d, f, say } from './text';

export type InsertionStart = 'random' | 'closest' | 'furthest';

export type InsertionRule =
  | 'random'
  | 'cheapest'
  | 'dearest'
  | 'nearest'
  | 'furthest'
  | 'nearest2'
  | 'furthest2';

/** Names as the original printed them (Legacy MethodStr / InitStr). */
export const INSERTION_RULE_NAMES: Record<InsertionRule | 'nearestNeighbour', string> = {
  random: 'Random',
  cheapest: 'Cheapest',
  dearest: 'Dearest',
  nearest: 'Nearest',
  furthest: 'Furthest',
  nearest2: 'Nearest II',
  furthest2: 'Furthest II',
  nearestNeighbour: 'Nearest Neigbour', // sic, as in the original
};

type Better = (value: number, best: number) => boolean;
const lesser: Better = (value, best) => value < best;
const greater: Better = (value, best) => value > best;

/** A tour under construction (Legacy INSPROB). */
class Construction {
  readonly tour = new Tour();
  /** Cities not yet in the tour, live in [0, remaining). Stale entries follow. */
  readonly out: number[];
  remaining: number;
  /** Legacy MASK colours: GREY until a city joins the tour, then WHITE. */
  readonly colors: EgaColor[];

  constructor(readonly dist: DistanceMatrix) {
    const n = dist.size;
    this.out = Array.from({ length: n }, (_, i) => i);
    this.remaining = n;
    this.colors = new Array<EgaColor>(n).fill(Role.cityOut);
  }

  /** Legacy CityDel. */
  takeOut(city: number): void {
    const pos = this.out.indexOf(city);
    this.remaining -= 1;
    this.out[pos] = this.out[this.remaining];
  }

  /** Adds a city to the tour's mask colours (Legacy SetMaskAt city_in). */
  markIn(city: number): void {
    this.colors[city] = Role.cityIn;
  }

  /** Nearest (lesser) or furthest (greater) city not in the tour (Legacy FNeighbour). */
  neighbour(better: Better, city: number): number {
    let best = this.out[0] === city ? this.out[1] : this.out[0];
    let bestDist = this.dist.get(city, best);
    for (let i = 0; i < this.remaining; i++) {
      const c = this.out[i];
      if (c !== city) {
        const dc = this.dist.get(city, c);
        if (better(dc, bestDist)) {
          bestDist = dc;
          best = c;
        }
      }
    }
    return best;
  }

  /** Cheapest edge to put `city` on (Legacy BestInsert). */
  bestInsert(city: number): [number, number] {
    let best: [number, number] = [this.tour.at(0), this.tour.at(1)];
    let bestCost = this.dist.insertCost(city, best[0], best[1]);
    for (let i = 1; i < this.tour.size; i++) {
      const a = this.tour.at(i);
      const b = this.tour.at(i + 1);
      const cost = this.dist.insertCost(city, a, b);
      if (cost < bestCost) {
        bestCost = cost;
        best = [a, b];
      }
    }
    return best;
  }

  /**
   * City whose best placement is cheapest (lesser) or dearest (greater) over
   * all cities and edges (Legacy FNextCost).
   */
  nextByCost(better: Better): number {
    let best = this.out[0];
    let bestCost = this.dist.insertCost(best, this.tour.at(0), this.tour.at(1));
    for (let i = 0; i < this.remaining; i++) {
      const c = this.out[i];
      for (let e = 0; e < this.tour.size; e++) {
        const cost = this.dist.insertCost(c, this.tour.at(e), this.tour.at(e + 1));
        if (better(cost, bestCost)) {
          best = c;
          bestCost = cost;
        }
      }
    }
    return best;
  }

  /**
   * For each tour city, its nearest or furthest city outside (`pick`); then
   * the nearest or furthest of those (`choose`) (Legacy FNextDist).
   */
  nextByDistance(choose: Better, pick: Better): number {
    const head = this.tour.at(0);
    let best = this.neighbour(pick, head);
    let bestDist = this.dist.get(best, head);
    for (let i = 0; i < this.tour.size; i++) {
      const city = this.tour.at(i);
      const candidate = this.neighbour(pick, city);
      const dc = this.dist.get(candidate, city);
      if (choose(dc, bestDist)) {
        best = candidate;
        bestDist = dc;
      }
    }
    return best;
  }

  // ---- Starting tours ----

  /** Legacy RndStartPath. */
  startRandom(rng: Rng): void {
    const n = this.dist.size;
    const a = randomInt(rng, n);
    let b: number;
    do b = randomInt(rng, n);
    while (b === a);
    this.startWith(a, b);
  }

  /** Closest (lesser) or furthest (greater) pair of cities (Legacy FStartPath). */
  startByPair(better: Better): void {
    const n = this.dist.size;
    let a = 0;
    let b = 1;
    let best = this.dist.get(a, b);
    for (let x = 0; x < n; x++) {
      for (let y = 0; y < n; y++) {
        if (x !== y) {
          const dxy = this.dist.get(x, y);
          if (better(dxy, best)) {
            a = x;
            b = y;
            best = dxy;
          }
        }
      }
    }
    this.startWith(a, b);
  }

  /** Legacy NNeighbourStartPath. */
  startNearestNeighbour(rng: Rng): void {
    const a = randomInt(rng, this.dist.size);
    this.takeOut(a);
    this.tour.append(a);
    this.markIn(a);
  }

  private startWith(a: number, b: number): void {
    this.takeOut(a);
    this.takeOut(b);
    this.tour.append(a);
    this.tour.append(b);
    this.markIn(a);
    this.markIn(b);
  }

  // ---- Frames, matching the Legacy Hilite* and DrawNorm calls ----

  frame(
    closed: boolean,
    marks: CityMark[],
    text: TextUpdate[],
    endOfStep = false,
    lines: Line[] = [],
  ): Frame {
    return {
      views: [{ tour: this.tour, closed, baseColors: this.colors, marks, edges: 'plain', lines }],
      text,
      endOfStep,
    };
  }

  /** Legacy HiliteCity: the city about to go in, double size. */
  hiliteCity(city: number, closed: boolean, text: TextUpdate[]): Frame {
    return this.frame(closed, [{ city, color: Role.cityHighlight, large: true }], text);
  }

  /** Legacy HiliteCityGroup: the city, and the edge it will break, in blue. */
  hiliteGroup(city: number, a: number, b: number, text: TextUpdate[]): Frame {
    return this.frame(
      true,
      [
        { city, color: Ega.YELLOW, large: true },
        { city: a, color: Ega.BLUE, large: true },
        { city: b, color: Ega.BLUE, large: true },
      ],
      text,
      false,
      [{ from: a, to: b, color: Ega.BLUE }],
    );
  }

  /** Legacy ShowChanges: the side-panel arithmetic for an insertion. */
  changes(next: number, a: number, b: number, color: EgaColor, row: number): TextUpdate[] {
    const g = (p: number, q: number) => this.dist.get(p, q);
    return [
      say(`Between ${d(a, 3)} and ${d(b, 3)}.\n`, row, color),
      say(`Edge Deleted =\n${d(a, 3)}->${d(b, 3)} is ${f(g(a, b), 6, 5)}\n`, row + 1, color),
      say(
        `Edges Added = \n` +
          `${d(next, 3)}->${d(a, 3)} is ${f(g(next, a), 6, 5)}\n` +
          `${d(next, 3)}->${d(b, 3)} is ${f(g(next, b), 6, 5)}\n`,
        row + 4,
        color,
      ),
      say(`Increase To Path =\n${f(this.dist.insertCost(next, a, b), 6, 5)}`, row + 8, color),
    ];
  }

  result() {
    return { tours: [this.tour], lengths: [this.tour.length(this.dist)] };
  }
}

/** Alternates WHITE and LTWHITE each step so changed text stands out (Legacy `col`). */
function* flicker(): Generator<EgaColor, never, void> {
  for (;;) {
    yield Ega.LTWHITE;
    yield Ega.WHITE;
  }
}

function methodTitle(name: string): TextUpdate {
  return say(`${name}\nMethod\n`, 1, Ega.GREEN);
}

/** Legacy ControlRand. */
function* randomRule(c: Construction, rng: Rng): Generator<Frame, void, void> {
  const total = c.remaining;
  const colors = flicker();
  while (c.remaining > 0) {
    const col = colors.next().value;
    const next = c.out[randomInt(rng, c.remaining)];
    yield c.hiliteCity(next, true, [say(`Insert is ${d(next, 3)}.  \n\n`, 4, col)]);

    c.takeOut(next);
    const [a, b] = c.bestInsert(next);
    yield c.hiliteGroup(next, a, b, c.changes(next, a, b, col, 5));

    c.tour.insertBetween(next, a, b);
    c.markIn(next);
    const done = Math.trunc((100 * (total - c.remaining)) / total);
    yield c.frame(true, [], [say(`${d(done, 3)}% Complete.\n\n`, 16, Ega.LTBLUE)], true);
  }
}

/** Legacy ControlDist: nearest, furthest, nearest II and furthest II. */
function* distanceRule(c: Construction, choose: Better, pick: Better): Generator<Frame, void, void> {
  const colors = flicker();
  while (c.remaining > 0) {
    const col = colors.next().value;
    const next = c.nextByDistance(choose, pick);
    yield c.hiliteCity(next, true, [say(`Insert is ${d(next, 3)}.  \n\n`, 4, col)]);

    c.takeOut(next);
    const [a, b] = c.bestInsert(next);
    yield c.hiliteGroup(next, a, b, c.changes(next, a, b, col, 5));

    c.tour.insertBetween(next, a, b);
    c.markIn(next);
    yield c.frame(true, [], [], true);
  }
}

/** Legacy ControlCost: cheapest and dearest. */
function* costRule(c: Construction, better: Better): Generator<Frame, void, void> {
  const colors = flicker();
  while (c.remaining > 0) {
    const col = colors.next().value;
    const next = c.nextByCost(better);
    const [a, b] = c.bestInsert(next);
    yield c.hiliteCity(next, true, [say(`Insert is ${d(next, 3)}.  \n\n`, 4, col)]);
    yield c.hiliteGroup(next, a, b, c.changes(next, a, b, col, 5));

    c.takeOut(next);
    c.tour.insertBetween(next, a, b);
    c.markIn(next);
    yield c.frame(true, [], [], true);
  }
}

/** Legacy ControlNearN: the tour stays open while it grows from the end. */
function* nearestNeighbourRule(c: Construction): Generator<Frame, void, void> {
  let last = c.tour.at(-1);
  while (c.remaining > 0) {
    const next = c.neighbour(lesser, last);
    yield c.hiliteCity(next, false, [
      say(`Last Was ${last}.\n\nThis Is ${next}.`, 4, Ega.WHITE),
    ]);

    c.takeOut(next);
    c.tour.append(next);
    c.markIn(next);
    yield c.frame(false, [], [], true);
    last = next;
  }
}

/**
 * Builds a tour by insertion (Legacy ControlAllIns). Needs at least 3 cities.
 * `rng` is used by the random start and the random rule.
 */
export function* insertion(
  dist: DistanceMatrix,
  start: InsertionStart,
  rule: InsertionRule,
  rng: Rng,
): MethodRun {
  requireCities(dist, 3);
  const c = new Construction(dist);
  if (start === 'closest') c.startByPair(lesser);
  else if (start === 'furthest') c.startByPair(greater);
  else c.startRandom(rng);

  const title = methodTitle(INSERTION_RULE_NAMES[rule]);
  const body = (() => {
    switch (rule) {
      case 'random':
        return randomRule(c, rng);
      case 'cheapest':
        return costRule(c, lesser);
      case 'dearest':
        return costRule(c, greater);
      case 'nearest':
        return distanceRule(c, lesser, lesser);
      case 'furthest':
        return distanceRule(c, greater, lesser);
      case 'nearest2':
        return distanceRule(c, lesser, greater);
      case 'furthest2':
        return distanceRule(c, greater, greater);
    }
  })();
  yield* withTitle(body, [title]);
  return c.result();
}

/** Builds a tour by Nearest Neighbour from a random start city (Legacy ControlAllIns, InsNearN). */
export function* nearestNeighbour(dist: DistanceMatrix, rng: Rng): MethodRun {
  requireCities(dist, 3);
  const c = new Construction(dist);
  c.startNearestNeighbour(rng);
  yield* withTitle(nearestNeighbourRule(c), [methodTitle(INSERTION_RULE_NAMES.nearestNeighbour)]);
  return c.result();
}

/**
 * Adds a method's title text to its first frame. The originals printed the
 * title before the loop without pausing for it.
 */
export function* withTitle(
  frames: Generator<Frame, void, void>,
  title: readonly TextUpdate[],
): Generator<Frame, void, void> {
  let first = true;
  for (const frame of frames) {
    yield first ? { ...frame, text: [...title, ...frame.text] } : frame;
    first = false;
  }
}

export function requireCities(dist: DistanceMatrix, min: number): void {
  if (dist.size < min) throw new RangeError(`This method needs at least ${min} cities.`);
}
