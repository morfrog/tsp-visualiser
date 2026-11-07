import { describe, expect, it } from 'vitest';
import { parseTsp } from '../core/tspfile';
import { readDataFile } from '../test/data';
import { cellCount, matches, parseOptimal, runBenchmarks, tableById, TABLES } from './runner';

const problems = {
  optimal: parseOptimal(readDataFile('10SOLNS.TXT')),
  ordered: Array.from({ length: 10 }, (_, i) => parseTsp(readDataFile(`30CITY_${i}.TSP`))),
  shuffled: Array.from({ length: 10 }, (_, i) => parseTsp(readDataFile(`30CITIES.DI${i}`))),
};

describe('benchmark tables', () => {
  it('cover all of DOCO: Nearest Neighbour, three insertion starts, iteration, annealing', () => {
    expect(TABLES.map((t) => t.section)).toEqual(['2.1.1.1', '2.2.3.1', '2.2.3.2', '2.2.3.3', '3.2.1', '4.1.1']);
    expect(cellCount()).toBe(10 + 3 * 70 + 30 + 30);
  });

  it('mark 150 results as the same every run', () => {
    const deterministic = TABLES.reduce((n, t) => n + t.columns.filter((c) => c.deterministic).length * 10, 0);
    expect(deterministic).toBe(150);
  });

  it('parse the optimal lengths in problem order', () => {
    expect(problems.optimal).toEqual([4.72574, 4.65923, 5.0227, 3.9263, 4.11147, 4.51163, 4.53754, 3.95369, 4.16023, 4.61321]);
  });
});

describe('runBenchmarks', () => {
  it('works out every cell, and every same-every-run cell matches 1997', { timeout: 60_000 }, () => {
    const cells = [...runBenchmarks(problems, 1)];
    expect(cells).toHaveLength(cellCount());
    let checked = 0;
    for (const cell of cells) {
      const table = tableById(cell.table);
      expect(cell.value).toBeGreaterThan(0.9);
      if (table.columns[cell.column].deterministic) {
        checked++;
        expect(matches(cell.value, table.doco[cell.row][cell.column]), `${cell.table} ${cell.row} ${cell.column}`).toBe(true);
      }
    }
    expect(checked).toBe(150);
  });
});
