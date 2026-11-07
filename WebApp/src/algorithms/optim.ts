// Port of the iteration (modification) methods in Legacy/OPTIM.C. Each one
// repeatedly finds the single best improving move over the whole tour and
// makes it, until no move shortens the tour:
//   1-opt: move one city to a better edge;
//   2-opt: reverse one segment;
//   3-opt: cut the tour into three segments and reverse any combination.
// Candidates are scanned in the original order and compared strictly, so
// ties go the same way they did in 1997.

import { Ega, Role, type EgaColor } from '../core/colors';
import type { DistanceMatrix } from '../core/distances';
import { Tour } from '../core/tour';
import type { CityMark, Frame, Line, MethodRun, TextUpdate } from './frame';
import { requireCities, withTitle } from './insertion';
import { d, f, say } from './text';

export type OptMethod = 'opt1' | 'opt2' | 'opt3';

/**
 * Stops a run that somehow keeps "improving" forever, which floating-point
 * noise could in principle cause. Generous: a real run on 1000 cities makes
 * far fewer moves.
 */
const MAX_MOVES = 1_000_000;

/** Legacy Same: edge a-b is edge c-d, in either direction. */
function sameEdge(a: number, b: number, c: number, e: number): boolean {
  return (a === e && b === c) || (a === c && b === e);
}

/** A tour being improved, with the Legacy MASK colours it is drawn with. */
class Improvement {
  readonly colors: EgaColor[];

  constructor(
    readonly dist: DistanceMatrix,
    readonly tour: Tour,
  ) {
    // NewProblem starts every city GREY (city_out); 1-opt turns the cities it
    // touches WHITE, and the 2-opt and 3-opt highlights reset them to GREY.
    this.colors = new Array<EgaColor>(dist.size).fill(Role.cityOut);
  }

  frame(marks: CityMark[], text: TextUpdate[], edges: 'plain' | 'mask', lines: Line[] = [], endOfStep = false): Frame {
    return {
      views: [{ tour: this.tour, closed: true, baseColors: this.colors, marks, edges, lines }],
      text,
      endOfStep,
    };
  }

  /** Legacy DrawNorm. */
  norm(text: TextUpdate[] = [], endOfStep = true): Frame {
    return this.frame([], text, 'plain', [], endOfStep);
  }

  /**
   * Colours every city by segment, walking the tour from `startCity` and
   * switching colour at each city in `switches`. Used for the 2-opt and
   * 3-opt highlights (Legacy HiliteTwoOpt, Hilite3Opt).
   */
  segmentMarks(startCity: number, initial: EgaColor, switches: [number, EgaColor][]): CityMark[] {
    const marks: CityMark[] = [];
    const start = this.tour.indexOf(startCity);
    let color = initial;
    for (let i = 0; i < this.tour.size; i++) {
      const city = this.tour.at(start + i);
      for (const [at, next] of switches) if (city === at) color = next;
      marks.push({ city, color });
    }
    return marks;
  }

  result() {
    return { tours: [this.tour], lengths: [this.tour.length(this.dist)] };
  }
}

// ---------------------------------------------------------------- 1-opt

export interface OneOptMove {
  city: number;
  /** Its neighbours now. */
  oldA: number;
  oldB: number;
  /** The edge it moves to. */
  newA: number;
  newB: number;
  /** Change in tour length (negative). */
  improve: number;
}

/** Best single-city move, or null if none shortens the tour (Legacy NextOneOpt). */
export function nextOneOpt(dist: DistanceMatrix, tour: Tour): OneOptMove | null {
  const cities = tour.toArray();
  let best: OneOptMove | null = null;
  let improve = 0;
  for (const city of cities) {
    const removed = tour.removeTemporarily(city)!;
    const { prev: a, next: b } = removed;
    const removal = dist.get(a, b) - dist.get(city, a) - dist.get(city, b);

    // Legacy BestInsert over the tour without the city, from its head.
    let c = tour.at(0);
    let e = tour.at(1);
    let bestCost = dist.insertCost(city, c, e);
    for (let i = 1; i < tour.size; i++) {
      const p = tour.at(i);
      const q = tour.at(i + 1);
      const cost = dist.insertCost(city, p, q);
      if (cost < bestCost) {
        bestCost = cost;
        c = p;
        e = q;
      }
    }

    const change = bestCost + removal;
    if (change < improve && !sameEdge(a, b, c, e)) {
      improve = change;
      best = { city, oldA: a, oldB: b, newA: c, newB: e, improve };
    }
    tour.restore(removed);
  }
  return best;
}

/** Legacy HiliteOneOpt: a city and two neighbours, then they turn WHITE for good. */
function oneOptMarks(a: number, city: number, b: number, color: EgaColor): CityMark[] {
  return [
    { city: a, color },
    { city, color, large: true },
    { city: b, color },
  ];
}

function* oneOptSteps(m: Improvement): Generator<Frame, void, void> {
  let times = 0;
  for (let moves = 0; moves < MAX_MOVES; moves++) {
    const move = nextOneOpt(m.dist, m.tour);
    if (!move) return;
    const { city, oldA, oldB, newA, newB, improve } = move;

    yield m.frame(
      oneOptMarks(oldA, city, oldB, Role.hilite1),
      [
        say(`Run ${times}\n`, 3, Ega.YELLOW),
        say(`${d(city, 2)} from ${d(oldA, 2)},${d(oldB, 2)}\n                \n`, 4, Role.hilite1),
        say('                   \n                   \n', 6, Ega.BLACK),
      ],
      'plain',
      [
        { from: oldA, to: city, color: Role.hilite1 },
        { from: city, to: oldB, color: Role.hilite1 },
      ],
    );
    times++;
    for (const c of [oldA, city, oldB]) m.colors[c] = Role.cityIn;

    yield m.frame(oneOptMarks(newA, city, newB, Role.hilite2), [
      say(`${d(city, 2)} to ${d(newA, 2)},${d(newB, 2)}  `, 5, Role.hilite2),
    ], 'plain');
    for (const c of [newA, city, newB]) m.colors[c] = Role.cityIn;

    m.tour.remove(city);
    m.tour.insertBetween(city, newA, newB);
    yield m.norm([say(`Path Length\nDecrease = ${f(-improve, 8, 6)}`, 6, Ega.YELLOW)]);
  }
}

// ---------------------------------------------------------------- 2-opt

export interface TwoOptMove {
  /** Edges a-b and c-d become a-c and b-d: the segment b..c is reversed. */
  a: number;
  b: number;
  c: number;
  d: number;
  change: number;
}

/** Best segment reversal, or null if none shortens the tour (Legacy NextTwoOpt). */
export function nextTwoOpt(dist: DistanceMatrix, tour: Tour): TwoOptMove | null {
  const t = tour.toArray();
  const n = t.length;
  t.push(t[0]);
  let best: TwoOptMove | null = null;
  let bestChange = 0;
  for (let x = 0; x < n - 2; x++) {
    for (let y = x + 2; y < n; y++) {
      const a = t[x];
      const b = t[x + 1];
      const c = t[y];
      const e = t[y + 1];
      if (a !== e) {
        const change = dist.get(a, c) + dist.get(b, e) - dist.get(a, b) - dist.get(c, e);
        if (change < bestChange) {
          bestChange = change;
          best = { a, b, c, d: e, change };
        }
      }
    }
  }
  return best;
}

/** Legacy HiliteTwoOpt: segment b..c yellow, the rest blue, from the head. */
function twoOptMarks(m: Improvement, move: TwoOptMove): CityMark[] {
  return m.segmentMarks(m.tour.at(0), Role.hilite1, [
    [move.b, Role.hilite2],
    [move.d, Role.hilite1],
  ]);
}

function* twoOptSteps(m: Improvement): Generator<Frame, void, void> {
  for (let moves = 0; moves < MAX_MOVES; moves++) {
    const move = nextTwoOpt(m.dist, m.tour);
    if (!move) return;
    const { a, b, c, d: e } = move;
    const g = (p: number, q: number) => m.dist.get(p, q);
    const dab = g(a, b);
    const dcd = g(c, e);
    const dac = g(a, c);
    const dbd = g(b, e);

    yield m.frame(twoOptMarks(m, move), [
      say(`Paths ${d(b, 2)}->${d(c, 2)}\n`, 3, Role.hilite2),
      say(`And   ${d(a, 2)}->${d(e, 2)}\n`, 4, Role.hilite1),
      say(`Breaking\n${d(a, 2)}->${d(b, 2)} = ${f(dab, 8, 6)}\n${d(c, 2)}->${d(e, 2)} = ${f(dcd, 8, 6)}\n`, 5, Ega.GREEN),
      say(`Adding\n${d(a, 2)}->${d(c, 2)} = ${f(dac, 8, 6)}\n${d(b, 2)}->${d(e, 2)} = ${f(dbd, 8, 6)}\n`, 8, Ega.GREEN),
      say(`Path Length\nDecrease = ${f(0 - (dac - dab + dbd - dcd), 8, 6)}\n`, 11, Ega.YELLOW),
    ], 'mask');

    yield m.frame(twoOptMarks(m, move), [], 'mask', [
      { from: a, to: b, color: Ega.BLACK },
      { from: c, to: e, color: Ega.BLACK },
      { from: a, to: c, color: Role.line },
      { from: b, to: e, color: Role.line },
    ]);

    m.tour.reverseSubPath(a, b, c, e);
    yield m.norm();
  }
}

// ---------------------------------------------------------------- 3-opt

/**
 * A 3-opt move (Legacy Next3Opt's 9-int array). The tour is cut at three
 * edges into segments A = p0..p1, B = p2..p3 and C = p4..p5, joined
 * p1-p2, p3-p4 and p5-p0. Each `reverse` flag reverses one segment.
 */
export interface ThreeOptMove {
  points: [number, number, number, number, number, number];
  reverse: [boolean, boolean, boolean];
  change: number;
}

type Points = ThreeOptMove['points'];

/** Sum of the three joining edges (Legacy SingleChange). */
function joins(dist: DistanceMatrix, p: Points): number {
  return dist.get(p[1], p[2]) + dist.get(p[3], p[4]) + dist.get(p[0], p[5]);
}

/** Swaps the ends of each reversed segment (Legacy SingleRevs). */
function applyReversals(p: Points, r: readonly boolean[]): void {
  if (r[0]) [p[0], p[1]] = [p[1], p[0]];
  if (r[1]) [p[2], p[3]] = [p[3], p[2]];
  if (r[2]) [p[4], p[5]] = [p[5], p[4]];
}

/** Best of the 7 reversal combinations for one set of cuts (Legacy FindBestRevs). */
function bestReversals(dist: DistanceMatrix, p: Points): { reverse: [boolean, boolean, boolean]; change: number } {
  let reverse: [boolean, boolean, boolean] = [false, false, false];
  let bestChange = 0;
  const before = joins(dist, p);
  for (const x of [false, true]) {
    for (const y of [false, true]) {
      for (const z of [false, true]) {
        if (!x && !y && !z) continue; // no reversal, no change
        const r = [x, y, z];
        applyReversals(p, r);
        const change = joins(dist, p) - before;
        applyReversals(p, r);
        if (change < bestChange) {
          reverse = [x, y, z];
          bestChange = change;
        }
      }
    }
  }
  return { reverse, change: bestChange };
}

/** Best 3-opt move, or null if none shortens the tour (Legacy Next3Opt). */
export function nextThreeOpt(dist: DistanceMatrix, tour: Tour): ThreeOptMove | null {
  const t = tour.toArray();
  const n = t.length;
  t.push(t[0]);
  let best: ThreeOptMove | null = null;
  let bestChange = 0;
  const p: Points = [0, 0, 0, 0, 0, 0];
  for (let x = 0; x < n - 2; x++) {
    p[1] = t[x];
    p[2] = t[x + 1];
    for (let y = x + 1; y < n - 1; y++) {
      p[3] = t[y];
      p[4] = t[y + 1];
      for (let z = y + 1; z < n; z++) {
        p[5] = t[z];
        p[0] = t[z + 1];
        const { reverse, change } = bestReversals(dist, p);
        if (change < bestChange) {
          bestChange = change;
          best = { points: [...p] as Points, reverse, change };
        }
      }
    }
  }
  return best;
}

/** Makes a 3-opt move, updating `points` to the new segment ends (Legacy Make3OptTransition). */
function makeThreeOpt(tour: Tour, move: ThreeOptMove): void {
  const p = move.points;
  if (move.reverse[0]) {
    tour.reverseSubPath(p[5], p[0], p[1], p[2]);
    [p[0], p[1]] = [p[1], p[0]];
  }
  if (move.reverse[1]) {
    tour.reverseSubPath(p[1], p[2], p[3], p[4]);
    [p[2], p[3]] = [p[3], p[2]];
  }
  if (move.reverse[2]) {
    tour.reverseSubPath(p[3], p[4], p[5], p[0]);
    [p[4], p[5]] = [p[5], p[4]];
  }
}

/** Legacy Hilite3Opt: segments A, B, C in blue, yellow and pink. */
function threeOptMarks(m: Improvement, p: Points): CityMark[] {
  return m.segmentMarks(p[0], Role.hilite1, [
    [p[2], Role.hilite2],
    [p[4], Role.hilite3],
  ]);
}

function* threeOptSteps(m: Improvement): Generator<Frame, void, void> {
  for (let moves = 0; moves < MAX_MOVES; moves++) {
    // Legacy Control3Opt read through a NULL pointer here when no move was
    // left; under DOS4GW that happened to end the loop. Stop cleanly instead.
    const move = nextThreeOpt(m.dist, m.tour);
    if (!move) return;
    const p = move.points;
    const rev = (r: boolean) => (r ? 'Rev' : 'No ');

    yield m.frame(threeOptMarks(m, p), [
      say(`${d(p[0], 2)}->${d(p[1], 2)} ${rev(move.reverse[0])}\n`, 5, Role.hilite1),
      say(`${d(p[2], 2)}->${d(p[3], 2)} ${rev(move.reverse[1])}\n`, 6, Role.hilite2),
      say(`${d(p[4], 2)}->${d(p[5], 2)} ${rev(move.reverse[2])}\n`, 7, Role.hilite3),
    ], 'mask');

    makeThreeOpt(m.tour, move);
    yield m.frame(threeOptMarks(m, p), [], 'mask');
    yield m.norm();
  }
}

/**
 * Improves `start` with 1-, 2- or 3-opt (Legacy ControlAllOpt). The start
 * tour is copied, not changed. Needs at least 4 cities.
 */
export function* optimise(dist: DistanceMatrix, start: Tour, method: OptMethod): MethodRun {
  requireCities(dist, 4);
  const m = new Improvement(dist, start.clone());
  // ControlAllOpt printed "X Opt\nMethod", then each Control* overwrote row 1.
  const titles: Record<OptMethod, [string, string, () => Generator<Frame, void, void>]> = {
    opt1: ['One Opt\nMethod', 'One Opt Method\n', () => oneOptSteps(m)],
    opt2: ['Two Opt\nMethod', 'Two Opt Method\n', () => twoOptSteps(m)],
    opt3: ['Three Opt\nMethod', 'Three Opt Method', () => threeOptSteps(m)],
  };
  const [first, second, steps] = titles[method];
  yield* withTitle(steps(), [say(first, 1, Ega.GREEN), say(second, 1, Ega.GREEN)]);
  return m.result();
}
