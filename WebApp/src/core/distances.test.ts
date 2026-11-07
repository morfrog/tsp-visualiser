import { describe, expect, it } from 'vitest';
import { DistanceMatrix } from './distances';
import { Tour } from './tour';
import { parseTsp } from './tspfile';
import { readDataFile } from '../test/data';

describe('DistanceMatrix', () => {
  const d = new DistanceMatrix([
    { x: 0, y: 0 },
    { x: 3, y: 4 },
    { x: 3, y: 0 },
  ]);

  it('is symmetric with a zero diagonal', () => {
    expect(d.size).toBe(3);
    expect(d.get(0, 1)).toBe(5);
    expect(d.get(1, 0)).toBe(5);
    expect(d.get(2, 2)).toBe(0);
  });

  it('computes insertion cost like InsertCost', () => {
    // Putting city 2 between 0 and 1: 3 + 4 - 5.
    expect(d.insertCost(2, 0, 1)).toBe(2);
  });
});

// Data/10SOLNS.TXT lists the optimal tour lengths found for the ten 30-city
// benchmark problems (from Dr Lister's student's solver, see DOCO.DOC). The
// 30CITY_n.TSP files store their cities in that optimal order, so the length
// of the file-order tour must reproduce each value.
describe('optimal tours of the 30-city benchmarks', () => {
  const optimal = readDataFile('10SOLNS.TXT')
    .split(/\r?\n/)
    .map((line) => /Data File No\.\s*(\d+)\s+([\d.]+)/.exec(line))
    .filter((m): m is RegExpExecArray => m !== null)
    .map((m) => ({ file: `30CITY_${m[1]}.TSP`, length: parseFloat(m[2]) }));

  it('lists all ten problems', () => {
    expect(optimal).toHaveLength(10);
  });

  it.each(optimal)('$file has file-order length $length', ({ file, length }) => {
    const cities = parseTsp(readDataFile(file));
    const d = new DistanceMatrix(cities);
    // The listed values are rounded to 4-5 decimal places.
    expect(Math.abs(Tour.identity(cities.length).length(d) - length)).toBeLessThan(5e-5);
  });
});
