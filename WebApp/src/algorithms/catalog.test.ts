import { existsSync } from 'node:fs';
import { join } from 'node:path';
import { describe, expect, it } from 'vitest';
import { AnsiCRandom } from '../core/random';
import { Tour } from '../core/tour';
import { expectValidTour, loadProblem } from '../test/helpers';
import { DEFAULT_ANNEAL_SETTINGS } from './anneal';
import {
  ANNEALING_METHODS,
  ANNEALING_SETTINGS_HELP,
  INSERTION_METHODS,
  INSERTION_STARTS,
  ITERATION_METHODS,
  SOLVERS,
  startRun,
  type Selection,
} from './catalog';
import { runToEnd } from './frame';

const HELP_DIR = join(__dirname, '../../../Legacy/HLP');

describe('catalog', () => {
  it('lists the menus in the original order', () => {
    expect(SOLVERS.map((s) => s.name)).toEqual(['Insertion', 'Iteration', 'Annealing']);
    expect(INSERTION_METHODS.map((m) => m.name)).toEqual([
      'N Neighbour', 'Random', 'Cheapest', 'Dearest', 'Nearest', 'Furthest', 'Nearest II', 'Furthest II',
    ]);
    expect(INSERTION_STARTS.map((s) => s.name)).toEqual(['Random', 'Closest', 'Furthest']);
    expect(ITERATION_METHODS.map((m) => m.name)).toEqual(['1 opt', '2 opt', '3 opt']);
  });

  it('points every entry at a help file that exists', () => {
    const files = [
      ...[SOLVERS, INSERTION_METHODS, INSERTION_STARTS, ITERATION_METHODS, ANNEALING_METHODS].flat().map((c) => c.help),
      ...Object.values(ANNEALING_SETTINGS_HELP),
    ];
    for (const file of files) expect(existsSync(join(HELP_DIR, file)), file).toBe(true);
  });

  it('starts every selection', () => {
    const dist = loadProblem('12CITY_3.TSP');
    const tour = Tour.identity(dist.size);
    const selections: Selection[] = [
      ...INSERTION_METHODS.flatMap((m) =>
        INSERTION_STARTS.map((s): Selection => ({ solver: 'insertion', method: m.id, start: s.id })),
      ),
      ...ITERATION_METHODS.map((m): Selection => ({ solver: 'iteration', method: m.id })),
      { solver: 'annealing', settings: { ...DEFAULT_ANNEAL_SETTINGS, temperature: 1, changesPerTemperature: 5 } },
    ];
    for (const selection of selections) {
      const result = runToEnd(startRun(dist, tour, selection, new AnsiCRandom(1)));
      result.tours.forEach((t) => expectValidTour(t, dist.size));
    }
  });
});
