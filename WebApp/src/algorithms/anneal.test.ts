import { describe, expect, it } from 'vitest';
import { AnsiCRandom, RAND_MAX, type Rng } from '../core/random';
import { Tour } from '../core/tour';
import { expectValidTour, loadProblem, record } from '../test/helpers';
import {
  anneal,
  chooseChange,
  DEFAULT_ANNEAL_SETTINGS,
  oneOptCandidates,
  twoOptCandidates,
  validateAnnealSettings,
} from './anneal';
import { runToEnd } from './frame';

const fixed = (value: number): Rng => ({ rand: () => value });

describe('candidate lists', () => {
  const dist = loadProblem('30CITY_0.TSP');
  const tour = Tour.identity(30);

  it('match the original array sizes', () => {
    // MethodMaxSize: n(n-2) for 1-opt, PlusFactorial(n-2) - 1 for 2-opt.
    expect(oneOptCandidates(dist, tour)).toHaveLength(30 * 28);
    expect(twoOptCandidates(dist, tour)).toHaveLength((28 * 29) / 2 - 1);
    expect(tour.toArray()).toEqual(Tour.identity(30).toArray());
  });

  it('have the true length change for each move', () => {
    for (const m of oneOptCandidates(dist, tour).slice(0, 50)) {
      const t = tour.clone();
      t.remove(m.city);
      t.insertBetween(m.city, m.newA, m.newB);
      expect(t.length(dist) - tour.length(dist)).toBeCloseTo(m.deltaE, 10);
    }
    for (const m of twoOptCandidates(dist, tour).slice(0, 50)) {
      const t = tour.clone();
      t.reverseSubPath(m.a, m.b, m.c, m.d);
      expect(t.length(dist) - tour.length(dist)).toBeCloseTo(m.deltaE, 10);
    }
  });
});

describe('chooseChange', () => {
  it('chooses in proportion to probability, with a null change of weight 1', () => {
    // Three improving moves (P = 1 each): sum 3, pick = r * 4 / RAND_MAX.
    expect(chooseChange([-1, -1, -1], 1, fixed(0))).toBe(0);
    expect(chooseChange([-1, -1, -1], 1, fixed(Math.floor(RAND_MAX * 0.3)))).toBe(1);
    expect(chooseChange([-1, -1, -1], 1, fixed(Math.floor(RAND_MAX * 0.6)))).toBe(2);
    expect(chooseChange([-1, -1, -1], 1, fixed(RAND_MAX))).toBe(-1);
  });

  it('almost never takes a much worse move when cold', () => {
    expect(chooseChange([5], 0.01, fixed(1))).toBe(-1);
  });
});

describe('validateAnnealSettings', () => {
  it('accepts the defaults and rejects what the original rejected', () => {
    expect(validateAnnealSettings(DEFAULT_ANNEAL_SETTINGS)).toBeNull();
    expect(validateAnnealSettings({ ...DEFAULT_ANNEAL_SETTINGS, temperature: -1 })).toMatch(/Temperature/);
    expect(validateAnnealSettings({ ...DEFAULT_ANNEAL_SETTINGS, coolingRate: 1 })).toMatch(/cooling/);
    expect(validateAnnealSettings({ ...DEFAULT_ANNEAL_SETTINGS, changesPerTemperature: 0 })).toMatch(/change/);
  });
});

describe('anneal', () => {
  const dist = loadProblem('30CITIES.DI5');
  const start = Tour.identity(30);
  const quick = { temperature: 1, coolingRate: 0.5, changesPerTemperature: 10 };

  it('runs 1-opt and 2-opt side by side to valid tours', () => {
    const { frames, result } = record(anneal(dist, start, quick, new AnsiCRandom(3)), 30);
    expect(result.tours).toHaveLength(2);
    result.tours.forEach((t) => expectValidTour(t, 30));
    result.lengths.forEach((l, i) => expect(l).toBeCloseTo(result.tours[i].length(dist), 10));
    expect(frames.every((f) => f.views.length === 2)).toBe(true);
    expect(start.toArray()).toEqual(Tour.identity(30).toArray());
  });

  it('keeps the running length in step with the tour', () => {
    const { frames, result } = record(anneal(dist, start, quick, new AnsiCRandom(3)), 30);
    const shown = frames
      .flatMap((f) => f.text)
      .filter((t) => t.area === 'right' && t.col === 8)
      .map((t) => parseFloat(t.text));
    if (shown.length > 0) expect(shown[shown.length - 1]).toBeCloseTo(result.lengths[1], 5);
  });

  it('repeats for the same seed', () => {
    const tours = (seed: number) => runToEnd(anneal(dist, start, quick, new AnsiCRandom(seed))).tours.map((t) => t.toArray());
    expect(tours(9)).toEqual(tours(9));
  });

  it('labels the screen like the original', () => {
    const { frames } = record(anneal(dist, start, quick, new AnsiCRandom(1)), 30);
    const first = frames[0].text;
    expect(first).toEqual(
      expect.arrayContaining([
        expect.objectContaining({ area: 'top', text: '1opt vs 2opt Annealing Run...' }),
        expect.objectContaining({
          area: 'top',
          text: 'Temperature=   1.000000  Cooling_Rate=0.500000  #Changes=  1 OF  10.',
        }),
        expect.objectContaining({ area: 'left', text: 'Length=            Accepted=       %' }),
      ]),
    );
  });

  it('only labels the screen at temperature 0, as the original loop never started', () => {
    const { frames, result } = record(anneal(dist, start, { ...quick, temperature: 0 }, new AnsiCRandom(1)), 30);
    expect(frames).toHaveLength(1);
    expect(frames[0].text.map((t) => t.area)).toEqual(['top', 'top', 'left', 'right']);
    expect(result.tours[0].toArray()).toEqual(start.toArray());
  });

  it('rejects invalid settings', () => {
    expect(() => runToEnd(anneal(dist, start, { ...quick, coolingRate: 2 }, new AnsiCRandom(1)))).toThrow(RangeError);
  });
});
