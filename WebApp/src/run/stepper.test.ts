import { describe, expect, it } from 'vitest';
import { insertion } from '../algorithms/insertion';
import { AnsiCRandom } from '../core/random';
import { loadProblem } from '../test/helpers';
import { LocalSession } from './session';
import { Stepper } from './stepper';

const dist = loadProblem('30CITY_2.TSP');
const run = () => insertion(dist, 'furthest', 'cheapest', new AnsiCRandom(1));

describe('Stepper', () => {
  it('gives one frame at a time as plain data', () => {
    const s = new Stepper(run());
    const batch = s.advance();
    expect(batch.frames).toBe(1);
    expect(batch.frame!.views[0].tour).toEqual(expect.any(Array));
    expect(batch.frame!.views[0].tour).toHaveLength(2);
    expect(JSON.parse(JSON.stringify(batch))).toEqual(batch);
  });

  it('snapshots, so later frames do not change earlier ones', () => {
    const s = new Stepper(run());
    const first = s.advance().frame!;
    s.advance({ frames: 10 });
    expect(first.views[0].tour).toHaveLength(2);
  });

  it('keeps every frame\'s text when running ahead', () => {
    const oneByOne = new Stepper(run());
    const texts: string[] = [];
    for (let i = 0; i < 12; i++) texts.push(...oneByOne.advance().text.map((t) => t.text));
    const batch = new Stepper(run()).advance({ frames: 12 });
    expect(batch.frames).toBe(12);
    expect(batch.text.map((t) => t.text)).toEqual(texts);
  });

  it('runs within a time budget and reports the outcome at the end', () => {
    let clock = 0;
    const s = new Stepper(run(), () => (clock += 1));
    const batch = s.advance({ budgetMs: 5 });
    expect(batch.frames).toBeGreaterThan(0);
    expect(batch.frames).toBeLessThan(10);
    const end = s.advance({ budgetMs: 1e9 });
    expect(end.outcome!.tours[0]).toHaveLength(30);
    expect(s.finished).toBe(true);
    expect(s.advance()).toMatchObject({ frame: null, frames: 0, outcome: end.outcome });
  });
});

describe('LocalSession', () => {
  it('refuses a selection that cannot run', () => {
    const cities = dist.cities.slice(0, 3);
    expect(
      () => new LocalSession({ cities: [...cities], tour: [0, 1, 2], selection: { solver: 'iteration', method: 'opt2' }, seed: 1 }),
    ).toThrow(/at least 4/);
  });

  it('runs a selection to the end', async () => {
    const session = new LocalSession({
      cities: [...dist.cities],
      tour: Array.from({ length: 30 }, (_, i) => i),
      selection: { solver: 'insertion', method: 'nearest', start: 'closest' },
      seed: 1,
    });
    const batch = await session.advance({ budgetMs: 1e9 });
    expect(batch.outcome?.tours[0]).toHaveLength(30);
  });
});
