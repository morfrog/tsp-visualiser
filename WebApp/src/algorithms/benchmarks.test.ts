// Reproduces the 1997 benchmark tables in DOCO.DOC exactly (to the 5
// decimals printed), which shows the ported methods make the same choices,
// including tie-breaks, as the original C.

import { describe, expect, it } from 'vitest';
import { DistanceMatrix } from '../core/distances';
import { AnsiCRandom } from '../core/random';
import { Tour } from '../core/tour';
import { parseTsp } from '../core/tspfile';
import { readDataFile } from '../test/data';
import { DOCO_CLOSEST, DOCO_FURTHEST, DOCO_ITERATION, INSERTION_TABLE_RULES } from './benchmarks';
import { runToEnd } from './frame';
import { insertion, type InsertionStart } from './insertion';
import { optimise, type OptMethod } from './optim';

const optimal = readDataFile('10SOLNS.TXT')
  .split(/\r?\n/)
  .map((line) => /Data File No\.\s*(\d+)\s+([\d.]+)/.exec(line))
  .filter((m): m is RegExpExecArray => m !== null)
  .map((m) => parseFloat(m[2]));

const problems = Array.from({ length: 10 }, (_, i) => i);

/** DOCO printed ratios to 5 decimals; the optimal lengths are also rounded. */
function expectRatio(length: number, problem: number, expected: number): void {
  expect(Math.abs(length / optimal[problem] - expected)).toBeLessThan(2e-5);
}

describe.each([
  ['closest', DOCO_CLOSEST],
  ['furthest', DOCO_FURTHEST],
] as const)('DOCO insertion table, %s start', (start: InsertionStart, table) => {
  const cases = problems.flatMap((p) =>
    INSERTION_TABLE_RULES.map((rule, column) => ({ p, rule, column })).filter((c) => c.rule !== 'random'),
  );

  it.each(cases)('30CITY_$p with $rule insertion', ({ p, rule, column }) => {
    const dist = new DistanceMatrix(parseTsp(readDataFile(`30CITY_${p}.TSP`)));
    const result = runToEnd(insertion(dist, start, rule, new AnsiCRandom(1)));
    expectRatio(result.lengths[0], p, table[p][column]);
  });
});

// The iteration benchmark started from the shuffled copies 30CITIES.DI0..9,
// not from the optimal-order 30CITY files.
describe('DOCO iteration table', () => {
  const methods: OptMethod[] = ['opt1', 'opt2', 'opt3'];
  const cases = problems.flatMap((p) => methods.map((method, column) => ({ p, method, column })));

  it.each(cases)('30CITIES.DI$p with $method', ({ p, method, column }) => {
    const cities = parseTsp(readDataFile(`30CITIES.DI${p}`));
    const dist = new DistanceMatrix(cities);
    const result = runToEnd(optimise(dist, Tour.identity(cities.length), method));
    expectRatio(result.lengths[0], p, DOCO_ITERATION[p][column]);
  });
});
