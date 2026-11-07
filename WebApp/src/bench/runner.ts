// Reruns DOCO.DOC's benchmark tables, cell by cell, as Legacy TSP.C, TSPOPT.C
// and TSPANN.C did in their "-3" benchmark modes: each method on each of the
// ten 30-city problems, as a ratio of its tour length to the optimal length.

import { anneal, DEFAULT_ANNEAL_SETTINGS } from '../algorithms/anneal';
import {
  DOCO_ANNEALING,
  DOCO_CLOSEST,
  DOCO_FURTHEST,
  DOCO_ITERATION,
  DOCO_NEAREST_NEIGHBOUR,
  DOCO_RANDOM,
  INSERTION_TABLE_RULES,
} from '../algorithms/benchmarks';
import { runToEnd } from '../algorithms/frame';
import { INSERTION_RULE_NAMES, insertion, nearestNeighbour, type InsertionStart } from '../algorithms/insertion';
import { optimise, type OptMethod } from '../algorithms/optim';
import { DistanceMatrix } from '../core/distances';
import type { City } from '../core/geometry';
import { AnsiCRandom } from '../core/random';
import { Tour } from '../core/tour';

export interface BenchProblems {
  /** Optimal tour lengths from Data/10SOLNS.TXT. */
  optimal: readonly number[];
  /** 30CITY_0..9.TSP, stored in optimal tour order. */
  ordered: readonly (readonly City[])[];
  /** 30CITIES.DI0..9, the same cities shuffled (the iteration benchmark's start). */
  shuffled: readonly (readonly City[])[];
}

export interface TableSpec {
  id: string;
  title: string;
  /** Which DOCO section it reproduces. */
  section: string;
  /** What each row started from. */
  note: string;
  columns: { name: string; deterministic: boolean }[];
  /** DOCO's values, [row][column]. */
  doco: readonly (readonly number[])[];
}

export interface CellResult {
  table: string;
  row: number;
  column: number;
  value: number;
}

/** DOCO printed 5 decimals and the optimal lengths are rounded too. */
export const MATCH_TOLERANCE = 2e-5;

const STARTS: { id: InsertionStart; name: string; section: string; doco: readonly (readonly number[])[] }[] = [
  { id: 'random', name: 'Random', section: '2.2.3.1', doco: DOCO_RANDOM },
  { id: 'closest', name: 'Closest', section: '2.2.3.2', doco: DOCO_CLOSEST },
  { id: 'furthest', name: 'Furthest', section: '2.2.3.3', doco: DOCO_FURTHEST },
];

const OPT_METHODS: OptMethod[] = ['opt1', 'opt2', 'opt3'];

export const TABLES: readonly TableSpec[] = [
  {
    id: 'nearestNeighbour',
    title: 'Nearest Neighbour',
    section: '2.1.1.1',
    note: 'Starts from a random city, so results vary from run to run.',
    columns: [{ name: 'NearN', deterministic: false }],
    doco: DOCO_NEAREST_NEIGHBOUR.map((v) => [v]),
  },
  ...STARTS.map((start) => ({
    id: `insertion-${start.id}`,
    title: `Insertion, ${start.name} initialisation`,
    section: start.section,
    note:
      start.id === 'random'
        ? 'The random start pair makes every column vary from run to run.'
        : 'Only the Random rule varies; every other column should match 1997 exactly.',
    columns: INSERTION_TABLE_RULES.map((rule) => ({
      name: INSERTION_RULE_NAMES[rule],
      deterministic: start.id !== 'random' && rule !== 'random',
    })),
    doco: start.doco,
  })),
  {
    id: 'iteration',
    title: 'Iteration',
    section: '3.2.1',
    note: 'Started from the shuffled copies 30CITIES.DI0 to DI9. Every column should match exactly.',
    columns: ['1 opt', '2 opt', '3 opt'].map((name) => ({ name, deterministic: true })),
    doco: DOCO_ITERATION,
  },
  {
    id: 'annealing',
    title: 'Simulated annealing',
    section: '4.1.1',
    note: `Started from the optimal tours, with temperature ${DEFAULT_ANNEAL_SETTINGS.temperature}, cooling rate ${DEFAULT_ANNEAL_SETTINGS.coolingRate} and ${DEFAULT_ANNEAL_SETTINGS.changesPerTemperature} changes per temperature (DOCO does not record its settings). Results vary from run to run.`,
    columns: [
      { name: '1 opt', deterministic: false },
      { name: '2 opt', deterministic: false },
      { name: '1 opt / 2 opt', deterministic: false },
    ],
    doco: DOCO_ANNEALING,
  },
];

export function tableById(id: string): TableSpec {
  const table = TABLES.find((t) => t.id === id);
  if (!table) throw new Error(`No table ${id}`);
  return table;
}

/** Cells in the order they are worked out. */
export function cellCount(): number {
  return TABLES.reduce((sum, t) => sum + t.columns.length * t.doco.length, 0);
}

/**
 * Works out every cell. Random methods take their seed from `seed` plus the
 * cell's position, so a seed reproduces a whole run of the page.
 */
export function* runBenchmarks(problems: BenchProblems, seed: number): Generator<CellResult, void, void> {
  const rows = problems.ordered.length;
  let n = 0;
  const rng = () => new AnsiCRandom(seed + n++);
  const ordered = problems.ordered.map((cities) => new DistanceMatrix(cities));
  const ratio = (length: number, row: number) => length / problems.optimal[row];

  for (let row = 0; row < rows; row++) {
    const r = runToEnd(nearestNeighbour(ordered[row], rng()));
    yield { table: 'nearestNeighbour', row, column: 0, value: ratio(r.lengths[0], row) };
  }

  for (const start of STARTS) {
    for (let row = 0; row < rows; row++) {
      for (let column = 0; column < INSERTION_TABLE_RULES.length; column++) {
        const r = runToEnd(insertion(ordered[row], start.id, INSERTION_TABLE_RULES[column], rng()));
        yield { table: `insertion-${start.id}`, row, column, value: ratio(r.lengths[0], row) };
      }
    }
  }

  for (let row = 0; row < rows; row++) {
    const cities = problems.shuffled[row];
    const dist = new DistanceMatrix(cities);
    for (let column = 0; column < OPT_METHODS.length; column++) {
      const r = runToEnd(optimise(dist, Tour.identity(cities.length), OPT_METHODS[column]));
      yield { table: 'iteration', row, column, value: ratio(r.lengths[0], row) };
    }
  }

  for (let row = 0; row < rows; row++) {
    const dist = ordered[row];
    const r = runToEnd(anneal(dist, Tour.identity(dist.size), DEFAULT_ANNEAL_SETTINGS, rng()));
    const [one, two] = r.lengths.map((l) => ratio(l, row));
    yield { table: 'annealing', row, column: 0, value: one };
    yield { table: 'annealing', row, column: 1, value: two };
    yield { table: 'annealing', row, column: 2, value: one / two };
  }
}

export function matches(value: number, doco: number): boolean {
  return Math.abs(value - doco) < MATCH_TOLERANCE;
}

/** Parses Data/10SOLNS.TXT ("Data File No. 0   4.72574"). */
export function parseOptimal(text: string): number[] {
  return text
    .split(/\r?\n/)
    .map((line) => /Data File No\.\s*(\d+)\s+([\d.]+)/.exec(line))
    .filter((m): m is RegExpExecArray => m !== null)
    .sort((a, b) => Number(a[1]) - Number(b[1]))
    .map((m) => parseFloat(m[2]));
}
