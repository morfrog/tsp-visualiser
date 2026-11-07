// The solvers, methods and starts as the main screen listed them, in the same
// order and with the same names and help files (Legacy INTERFAC.C: SetSolver,
// SetMethod, SetInit), and one function to start whichever is selected.

import type { DistanceMatrix } from '../core/distances';
import type { Rng } from '../core/random';
import type { Tour } from '../core/tour';
import { anneal, validateAnnealSettings, type AnnealSettings } from './anneal';
import type { MethodRun } from './frame';
import { insertion, nearestNeighbour, type InsertionRule, type InsertionStart } from './insertion';
import { optimise, type OptMethod } from './optim';

export interface Choice<Id extends string> {
  id: Id;
  /** Name shown in the list. */
  name: string;
  /** Help file in Legacy/HLP. */
  help: string;
  /** Same result every run (SS in DOCO.DOC) rather than different (DS). */
  deterministic: boolean;
}

export type InsertionMethodId = 'nearestNeighbour' | InsertionRule;
export type SolverId = 'insertion' | 'iteration' | 'annealing';

export const INSERTION_METHODS: readonly Choice<InsertionMethodId>[] = [
  { id: 'nearestNeighbour', name: 'N Neighbour', help: 'MNEARN.HLP', deterministic: false },
  { id: 'random', name: 'Random', help: 'MRANDOM.HLP', deterministic: false },
  { id: 'cheapest', name: 'Cheapest', help: 'MCHEAP.HLP', deterministic: true },
  { id: 'dearest', name: 'Dearest', help: 'MDEAR.HLP', deterministic: true },
  { id: 'nearest', name: 'Nearest', help: 'MNEAR.HLP', deterministic: true },
  { id: 'furthest', name: 'Furthest', help: 'MFURTH.HLP', deterministic: true },
  { id: 'nearest2', name: 'Nearest II', help: 'MNEAR2.HLP', deterministic: true },
  { id: 'furthest2', name: 'Furthest II', help: 'MFURTH2.HLP', deterministic: true },
];

export const INSERTION_STARTS: readonly Choice<InsertionStart>[] = [
  { id: 'random', name: 'Random', help: 'IRANDOM.HLP', deterministic: false },
  { id: 'closest', name: 'Closest', help: 'ICLOSE.HLP', deterministic: true },
  { id: 'furthest', name: 'Furthest', help: 'IFURTH.HLP', deterministic: true },
];

export const ITERATION_METHODS: readonly Choice<OptMethod>[] = [
  { id: 'opt1', name: '1 opt', help: '1OPT.HLP', deterministic: true },
  { id: 'opt2', name: '2 opt', help: '2OPT.HLP', deterministic: true },
  { id: 'opt3', name: '3 opt', help: '3OPT.HLP', deterministic: true },
];

export const ANNEALING_METHODS: readonly Choice<'oneVsTwo'>[] = [
  { id: 'oneVsTwo', name: '1 vs 2 opt', help: '1VS2OPT.HLP', deterministic: false },
];

/** Annealing's settings take the place of the start list (Legacy anninit). */
export const ANNEALING_SETTINGS_HELP: Record<keyof AnnealSettings, string> = {
  temperature: 'ITEMP.HLP',
  coolingRate: 'ICOOL.HLP',
  changesPerTemperature: 'IATTMP.HLP',
};

export const SOLVERS: readonly Choice<SolverId>[] = [
  { id: 'insertion', name: 'Insertion', help: 'SINSERT.HLP', deterministic: false },
  { id: 'iteration', name: 'Iteration', help: 'SITERATE.HLP', deterministic: true },
  { id: 'annealing', name: 'Annealing', help: 'SANNEAL.HLP', deterministic: false },
];

export type Selection =
  | { solver: 'insertion'; method: InsertionMethodId; start: InsertionStart }
  | { solver: 'iteration'; method: OptMethod }
  | { solver: 'annealing'; settings: AnnealSettings };

/**
 * Pauses per step, for splitting a timed step's delay between them (Legacy
 * Init2TimeSegs for Nearest Neighbour, Init3TimeSegs for the rest).
 */
export function pausesPerStep(selection: Selection): number {
  return selection.solver === 'insertion' && selection.method === 'nearestNeighbour' ? 2 : 3;
}

/** Why a selection can't run on this map, or null if it can. */
export function selectionProblem(cityCount: number, selection: Selection): string | null {
  const min = selection.solver === 'insertion' ? 3 : 4;
  if (cityCount < min) return `This method needs at least ${min} cities.`;
  if (selection.solver === 'annealing') return validateAnnealSettings(selection.settings);
  return null;
}

/**
 * Starts the selected method (Legacy "Go!"). Insertion builds a new tour and
 * ignores `tour`; iteration and annealing improve a copy of it.
 */
export function startRun(dist: DistanceMatrix, tour: Tour, selection: Selection, rng: Rng): MethodRun {
  switch (selection.solver) {
    case 'insertion':
      return selection.method === 'nearestNeighbour'
        ? nearestNeighbour(dist, rng)
        : insertion(dist, selection.start, selection.method, rng);
    case 'iteration':
      return optimise(dist, tour, selection.method);
    case 'annealing':
      return anneal(dist, tour, selection.settings, rng);
  }
}
