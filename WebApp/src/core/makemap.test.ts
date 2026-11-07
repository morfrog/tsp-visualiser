import { describe, expect, it } from 'vitest';
import { citiesInTourOrder, makeMap, randomizeOrder } from './makemap';
import { AnsiCRandom, RAND_MAX } from './random';

describe('makeMap (Legacy MakeMap)', () => {
  it('places cities in the unit square, rounded to the 6 decimals the original wrote', () => {
    const cities = makeMap(50, new AnsiCRandom(1));
    expect(cities).toHaveLength(50);
    for (const c of cities) {
      for (const v of [c.x, c.y]) {
        expect(v).toBeGreaterThanOrEqual(0);
        expect(v).toBeLessThanOrEqual(1);
        expect(Number(v.toFixed(6))).toBe(v);
      }
    }
  });

  it('draws x then y from rand() / RAND_MAX', () => {
    const [first] = makeMap(1, new AnsiCRandom(1));
    expect(first.x).toBe(Number((16838 / RAND_MAX).toFixed(6)));
    expect(first.y).toBe(Number((5758 / RAND_MAX).toFixed(6)));
  });
});

describe('randomizeOrder (Legacy rndfile)', () => {
  const cities = Array.from({ length: 30 }, (_, i) => ({ x: i / 30, y: 0 }));

  it('keeps the same cities in a different order', () => {
    const shuffled = randomizeOrder(cities, new AnsiCRandom(3));
    expect([...shuffled].sort((a, b) => a.x - b.x)).toEqual(cities);
    expect(shuffled).not.toEqual(cities);
  });

  it('repeats for a seed and leaves the input alone', () => {
    const before = cities.map((c) => ({ ...c }));
    expect(randomizeOrder(cities, new AnsiCRandom(9))).toEqual(randomizeOrder(cities, new AnsiCRandom(9)));
    expect(cities).toEqual(before);
  });
});

describe('citiesInTourOrder (Legacy PathAsLocates)', () => {
  it('reorders the cities so file order is the tour', () => {
    const cities = [{ x: 0, y: 0 }, { x: 1, y: 0 }, { x: 2, y: 0 }];
    expect(citiesInTourOrder(cities, [2, 0, 1])).toEqual([cities[2], cities[0], cities[1]]);
  });
});
