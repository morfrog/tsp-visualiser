// Port of simulated annealing from Legacy/ANNEAL.C. The original always ran
// 1-opt and 2-opt annealing side by side from the same start tour, in two
// windows, sharing one stream of random numbers (Control1vs2Anneal).
//
// At each temperature T it makes `changes` attempts per method. An attempt
// lists every possible move with its length change dE and probability
// P = 1 if dE <= 0, else exp(-dE / T). A "null change" with P = 1 is added,
// and one is chosen at random in proportion to P. A method stops after a
// whole temperature where every attempt chose the null change. Then T is
// multiplied by the cooling rate, until both stop or T <= 5e-7.

import { Ega, Role, type EgaColor } from '../core/colors';
import type { DistanceMatrix } from '../core/distances';
import { RAND_MAX, type Rng } from '../core/random';
import type { Tour } from '../core/tour';
import type { CityMark, Line, MethodRun, TextUpdate, View } from './frame';
import { requireCities } from './insertion';
import { d, f, say } from './text';

export interface AnnealSettings {
  /** Starting temperature (>= 0). Original default 100. */
  temperature: number;
  /** Multiplier applied after each temperature, 0 < rate < 1. Original default 0.8. */
  coolingRate: number;
  /** Attempts per temperature (>= 1). Original default 50. */
  changesPerTemperature: number;
}

export const DEFAULT_ANNEAL_SETTINGS: AnnealSettings = {
  temperature: 100,
  coolingRate: 0.8,
  changesPerTemperature: 50,
};

/** Temperature below which annealing stops (Control1vs2Anneal). */
const MIN_TEMPERATURE = 0.0000005;

/** Checks settings the way AnnealInitChange validated typed input. */
export function validateAnnealSettings(s: AnnealSettings): string | null {
  if (!(s.temperature >= 0)) return 'Temperature Must Be > 0.0';
  if (!(s.coolingRate > 0 && s.coolingRate < 1)) return '0.0 < cooling rate < 1.0';
  if (!(Number.isInteger(s.changesPerTemperature) && s.changesPerTemperature > 0)) {
    return 'Must be at least one change...';
  }
  return null;
}

/** Legacy GetP. */
function probability(deltaE: number, temperature: number): number {
  return deltaE <= 0 ? 1 : Math.exp(0 - deltaE / temperature);
}

interface OneOptCandidate {
  city: number;
  oldA: number;
  oldB: number;
  newA: number;
  newB: number;
  deltaE: number;
}

interface TwoOptCandidate {
  a: number;
  b: number;
  c: number;
  d: number;
  deltaE: number;
}

/** Every 1-opt move, in the original order (Legacy AnnealOneOpt). */
export function oneOptCandidates(dist: DistanceMatrix, tour: Tour): OneOptCandidate[] {
  const result: OneOptCandidate[] = [];
  for (const city of tour.toArray()) {
    const removed = tour.removeTemporarily(city)!;
    const { prev: oldA, next: oldB } = removed;
    const removal = dist.get(oldA, oldB) - dist.get(city, oldA) - dist.get(city, oldB);
    for (let i = 0; i < tour.size; i++) {
      const newA = tour.at(i);
      const newB = tour.at(i + 1);
      const same = (oldA === newB && oldB === newA) || (oldA === newA && oldB === newB);
      if (!same) {
        result.push({ city, oldA, oldB, newA, newB, deltaE: dist.insertCost(city, newA, newB) + removal });
      }
    }
    tour.restore(removed);
  }
  return result;
}

/** Every 2-opt move, in the original order (Legacy AnnealTwoOpt). */
export function twoOptCandidates(dist: DistanceMatrix, tour: Tour): TwoOptCandidate[] {
  const t = tour.toArray();
  const n = t.length;
  t.push(t[0]);
  const result: TwoOptCandidate[] = [];
  for (let x = 0; x < n - 2; x++) {
    for (let y = x + 2; y < n; y++) {
      const a = t[x];
      const b = t[x + 1];
      const c = t[y];
      const e = t[y + 1];
      if (a !== e) {
        result.push({ a, b, c, d: e, deltaE: dist.get(a, c) + dist.get(b, e) - dist.get(a, b) - dist.get(c, e) });
      }
    }
  }
  return result;
}

/**
 * Picks a candidate index in proportion to its probability, or -1 for the
 * null change (Legacy ApplyTemperature + ChooseChange).
 */
export function chooseChange(deltas: readonly number[], temperature: number, rng: Rng): number {
  const sums = new Array<number>(deltas.length);
  let sum = 0;
  for (let i = 0; i < deltas.length; i++) {
    sum += probability(deltas[i], temperature);
    sums[i] = sum;
  }
  const pick = (rng.rand() / RAND_MAX) * (sum + 1);
  if (pick > sum) return -1;
  for (let i = 0; i < sums.length; i++) if (sums[i] > pick) return i;
  // pick == sum exactly: the original read one past the end; take the last.
  return sums.length - 1;
}

/** One side of the run: a method, its tour and its mask colours. */
class Side {
  readonly colors: EgaColor[];
  length: number;
  done = false;
  nulls = 0;

  constructor(
    readonly dist: DistanceMatrix,
    readonly tour: Tour,
    readonly area: 'left' | 'right',
  ) {
    this.colors = new Array<EgaColor>(dist.size).fill(Role.cityOut);
    this.length = tour.length(dist);
  }

  view(marks: CityMark[] = [], edges: 'plain' | 'mask' = 'plain', lines: Line[] = []): View {
    return { tour: this.tour, closed: true, baseColors: this.colors, marks, edges, lines };
  }
}

function oneOptView(side: Side, a: number, city: number, b: number, color: EgaColor, withLines: boolean): View {
  const marks = [
    { city: a, color },
    { city, color, large: true },
    { city: b, color },
  ];
  const lines = withLines
    ? [
        { from: a, to: city, color },
        { from: city, to: b, color },
      ]
    : [];
  return side.view(marks, 'plain', lines);
}

function twoOptView(side: Side, m: TwoOptCandidate, withLines: boolean): View {
  const marks: CityMark[] = [];
  let color: EgaColor = Role.hilite1;
  for (let i = 0; i < side.tour.size; i++) {
    const city = side.tour.at(i);
    if (city === m.b) color = Role.hilite2;
    else if (city === m.d) color = Role.hilite1;
    marks.push({ city, color });
  }
  const lines = withLines
    ? [
        { from: m.a, to: m.b, color: Ega.BLACK },
        { from: m.c, to: m.d, color: Ega.BLACK },
        { from: m.a, to: m.c, color: Role.line },
        { from: m.b, to: m.d, color: Role.line },
      ]
    : [];
  return side.view(marks, 'mask', lines);
}

/**
 * Runs 1-opt and 2-opt annealing side by side from `start` (Legacy
 * ControlAnneal + Control1vs2Anneal). The start tour is copied, not changed.
 * The result has the 1-opt tour first, then the 2-opt tour.
 */
export function* anneal(
  dist: DistanceMatrix,
  start: Tour,
  settings: AnnealSettings,
  rng: Rng,
): MethodRun {
  requireCities(dist, 4);
  const problem = validateAnnealSettings(settings);
  if (problem) throw new RangeError(problem);
  const { coolingRate: cooling, changesPerTemperature: changes } = settings;
  let temp = settings.temperature;

  const one = new Side(dist, start.clone(), 'left');
  const two = new Side(dist, start.clone(), 'right');

  // Text waiting to go out with the next frame; starts with the screen's labels.
  let pending: TextUpdate[] = [
    say('1opt vs 2opt Annealing Run...', 1, Ega.GREEN, 1, 'top'),
    say(
      `Temperature=${f(temp, 11, 6)}  Cooling_Rate=${f(cooling, 8, 6)}  #Changes=${d(1, 3)} OF ${d(changes, 3)}.`,
      3,
      Ega.RED,
      1,
      'top',
    ),
    say('Length=            Accepted=       %', 1, Ega.RED, 1, 'left'),
    say('Length=            Accepted=       %', 1, Ega.RED, 1, 'right'),
  ];

  while (temp > MIN_TEMPERATURE && !(one.done && two.done)) {
    one.nulls = 0;
    two.nulls = 0;
    for (let x = 0; x < changes; x++) {
      // Choose: 1-opt draws its random number before 2-opt, as in the original.
      let move1: OneOptCandidate | null = null;
      let move2: TwoOptCandidate | null = null;
      const views1: View[] = [one.view(), two.view()];
      if (!one.done) {
        const candidates = oneOptCandidates(dist, one.tour);
        const i = chooseChange(candidates.map((c) => c.deltaE), temp, rng);
        if (i < 0) one.nulls++;
        else move1 = candidates[i];
        views1[0] = move1 ? oneOptView(one, move1.oldA, move1.city, move1.oldB, Role.hilite1, true) : one.view();
      }
      if (!two.done) {
        const candidates = twoOptCandidates(dist, two.tour);
        const i = chooseChange(candidates.map((c) => c.deltaE), temp, rng);
        if (i < 0) two.nulls++;
        else move2 = candidates[i];
        views1[1] = move2 ? twoOptView(two, move2, false) : two.view();
      }
      yield { views: views1, text: [...pending, say(d(x + 1, 3), 3, Ega.LTRED, 58, 'top')], endOfStep: false };
      pending = [];
      if (move1) for (const c of [move1.oldA, move1.city, move1.oldB]) one.colors[c] = Role.cityIn;

      // Show where each move goes (before making it, as the frame holds the
      // live tour), with the new lengths, then make the moves.
      const views2: View[] = [one.view(), two.view()];
      const text2: TextUpdate[] = [];
      if (move1) {
        views2[0] = oneOptView(one, move1.newA, move1.city, move1.newB, Role.hilite2, false);
        one.length += move1.deltaE;
        text2.push(say(one.length.toFixed(6), 1, Ega.LTRED, 8, 'left'));
      }
      if (move2) {
        views2[1] = twoOptView(two, move2, true);
        two.length += move2.deltaE;
        text2.push(say(two.length.toFixed(6), 1, Ega.LTRED, 8, 'right'));
      }
      yield { views: views2, text: text2, endOfStep: false };
      if (move1) {
        for (const c of [move1.newA, move1.city, move1.newB]) one.colors[c] = Role.cityIn;
        one.tour.remove(move1.city);
        one.tour.insertBetween(move1.city, move1.newA, move1.newB);
      }
      if (move2) two.tour.reverseSubPath(move2.a, move2.b, move2.c, move2.d);

      yield { views: [one.view(), two.view()], text: [], endOfStep: true };
    }

    // End of a temperature: shown with the next frame, as the original did
    // not pause here.
    if (one.nulls >= changes) one.done = true;
    if (two.nulls >= changes) two.done = true;
    if (!(one.done && two.done)) {
      temp *= cooling;
      pending.push(say(f(temp, 11, 6), 3, Ega.LTRED, 13, 'top'));
    }
    for (const side of [one, two]) {
      const accepted = side.done ? 0 : (100 * (changes - side.nulls)) / changes;
      pending.push(say(f(accepted, 8, 3), 1, Ega.LTRED, 28, side.area));
    }
  }
  if (pending.length > 0) yield { views: [one.view(), two.view()], text: pending, endOfStep: true };

  return { tours: [one.tour, two.tour], lengths: [one.tour.length(dist), two.tour.length(dist)] };
}
