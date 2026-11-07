import { describe, expect, it } from 'vitest';
import { Ega } from '../core/colors';
import { DistanceMatrix } from '../core/distances';
import { AnsiCRandom } from '../core/random';
import { expectValidTour, loadProblem, record } from '../test/helpers';
import { runToEnd } from './frame';
import { insertion, nearestNeighbour, type InsertionRule, type InsertionStart } from './insertion';

const RULES: InsertionRule[] = ['random', 'cheapest', 'dearest', 'nearest', 'furthest', 'nearest2', 'furthest2'];
const STARTS: InsertionStart[] = ['random', 'closest', 'furthest'];

describe('insertion', () => {
  const dist = loadProblem('50CITY_1.TSP');
  const combos = STARTS.flatMap((start) => RULES.map((rule) => ({ start, rule })));

  it.each(combos)('$start start, $rule rule builds a full tour', ({ start, rule }) => {
    const { frames, sizes, result } = record(insertion(dist, start, rule, new AnsiCRandom(7)), dist.size);
    expectValidTour(result.tours[0], dist.size);
    expect(result.lengths[0]).toBeCloseTo(result.tours[0].length(dist), 10);
    // Three pauses per city inserted, after starting with two cities.
    expect(frames).toHaveLength(3 * (dist.size - 2));
    expect(frames.filter((f) => f.endOfStep)).toHaveLength(dist.size - 2);
    expect(sizes[0]).toBe(2);
    expect(frames.every((f) => f.views[0].closed)).toBe(true);
  });

  it('puts the method title on the first frame only', () => {
    const { frames } = record(insertion(dist, 'closest', 'nearest2', new AnsiCRandom(1)), dist.size);
    expect(frames[0].text[0]).toMatchObject({ row: 1, text: 'Nearest II\nMethod\n', color: Ega.GREEN });
    expect(frames.slice(1).some((f) => f.text.some((t) => t.text.includes('Method')))).toBe(false);
  });

  it('writes the side panel like the original', () => {
    const { frames } = record(insertion(dist, 'furthest', 'cheapest', new AnsiCRandom(1)), dist.size);
    expect(frames[0].text[1].text).toMatch(/^Insert is [ \d]{3}\.  \n\n$/);
    const changes = frames[1].text.map((t) => t.text).join('');
    expect(changes).toMatch(/Between [ \d]{3} and [ \d]{3}\.\n/);
    expect(changes).toMatch(/Increase To Path =\n\d\.\d{5}/);
  });

  it('highlights the city to insert, then the edge it breaks', () => {
    const { frames } = record(insertion(dist, 'closest', 'cheapest', new AnsiCRandom(1)), dist.size);
    const [first, second] = frames;
    expect(first.views[0].marks).toEqual([{ city: expect.any(Number), color: Ega.YELLOW, large: true }]);
    const city = first.views[0].marks[0].city;
    expect(second.views[0].marks[0]).toEqual({ city, color: Ega.YELLOW, large: true });
    expect(second.views[0].lines).toHaveLength(1);
    expect(second.views[0].lines[0].color).toBe(Ega.BLUE);
  });

  it('random methods repeat for the same seed and vary between seeds', () => {
    const tourFor = (seed: number) =>
      runToEnd(insertion(dist, 'random', 'random', new AnsiCRandom(seed))).tours[0].toArray();
    expect(tourFor(3)).toEqual(tourFor(3));
    const distinct = new Set([1, 2, 3, 4, 5].map((s) => tourFor(s).join(',')));
    expect(distinct.size).toBeGreaterThan(1);
  });

  it('needs at least three cities', () => {
    const two = new DistanceMatrix([
      { x: 0, y: 0 },
      { x: 1, y: 1 },
    ]);
    expect(() => runToEnd(insertion(two, 'closest', 'cheapest', new AnsiCRandom(1)))).toThrow(RangeError);
  });

  it('works on the smallest problem', () => {
    const three = new DistanceMatrix([
      { x: 0, y: 0 },
      { x: 1, y: 0 },
      { x: 0, y: 1 },
    ]);
    const result = runToEnd(insertion(three, 'furthest', 'dearest', new AnsiCRandom(1)));
    expectValidTour(result.tours[0], 3);
  });
});

describe('nearest neighbour', () => {
  const dist = loadProblem('30CITY_3.TSP');

  it('builds an open tour from a random city, two pauses per city', () => {
    const { frames, sizes, result } = record(nearestNeighbour(dist, new AnsiCRandom(11)), dist.size);
    expectValidTour(result.tours[0], dist.size);
    expect(frames).toHaveLength(2 * (dist.size - 1));
    expect(sizes[0]).toBe(1);
    expect(frames.every((f) => !f.views[0].closed)).toBe(true);
  });

  it('always moves to the nearest unvisited city', () => {
    const tour = runToEnd(nearestNeighbour(dist, new AnsiCRandom(5))).tours[0].toArray();
    for (let i = 0; i < tour.length - 1; i++) {
      const here = tour[i];
      const unvisited = tour.slice(i + 1);
      const nearest = Math.min(...unvisited.map((c) => dist.get(here, c)));
      expect(dist.get(here, tour[i + 1])).toBe(nearest);
    }
  });

  it('starts from a different city for a different seed', () => {
    const startFor = (seed: number) => runToEnd(nearestNeighbour(dist, new AnsiCRandom(seed))).tours[0].at(0);
    expect(new Set([1, 2, 3, 4, 5, 6].map(startFor)).size).toBeGreaterThan(1);
  });
});
