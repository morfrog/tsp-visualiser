// The benchmark's problems, bundled from the repository's Data/ folder.

import { parseTsp } from '../core/tspfile';
import { parseOptimal, type BenchProblems } from './runner';

const ordered = import.meta.glob('../../../Data/30CITY_[0-9].TSP', { query: '?raw', import: 'default', eager: true }) as Record<
  string,
  string
>;
const shuffled = import.meta.glob('../../../Data/30CITIES.DI[0-9]', { query: '?raw', import: 'default', eager: true }) as Record<
  string,
  string
>;
const solutions = import.meta.glob('../../../Data/10SOLNS.TXT', { query: '?raw', import: 'default', eager: true }) as Record<
  string,
  string
>;

/** Files in problem order 0..9, by the digit at the end of their name. */
function byDigit(files: Record<string, string>): string[] {
  return Object.entries(files)
    .map(([path, text]) => ({ n: Number(/(\d)(?:\.TSP)?$/i.exec(path)![1]), text }))
    .sort((a, b) => a.n - b.n)
    .map((f) => f.text);
}

export function benchProblems(): BenchProblems {
  return {
    optimal: parseOptimal(Object.values(solutions)[0] ?? ''),
    ordered: byDigit(ordered).map(parseTsp),
    shuffled: byDigit(shuffled).map(parseTsp),
  };
}
