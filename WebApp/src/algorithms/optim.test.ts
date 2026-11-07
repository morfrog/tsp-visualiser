import { describe, expect, it } from 'vitest';
import { Ega } from '../core/colors';
import { DistanceMatrix } from '../core/distances';
import { Tour } from '../core/tour';
import { expectValidTour, loadProblem, record } from '../test/helpers';
import { runToEnd } from './frame';
import { nextOneOpt, nextThreeOpt, nextTwoOpt, optimise, type OptMethod } from './optim';

const METHODS: OptMethod[] = ['opt1', 'opt2', 'opt3'];

/** Do segments p1-p2 and q1-q2 cross? */
function crosses(d: DistanceMatrix, a: number, b: number, c: number, e: number): boolean {
  const [p1, p2, q1, q2] = [a, b, c, e].map((i) => d.cities[i]);
  const side = (o: typeof p1, s: typeof p1, t: typeof p1) => (s.x - o.x) * (t.y - o.y) - (s.y - o.y) * (t.x - o.x);
  return side(p1, p2, q1) * side(p1, p2, q2) < 0 && side(q1, q2, p1) * side(q1, q2, p2) < 0;
}

describe.each(METHODS)('%s', (method) => {
  const dist = loadProblem('30CITIES.DI4');
  const start = Tour.identity(dist.size);

  it('shortens the tour, keeps it valid and leaves the start alone', () => {
    const before = start.toArray();
    const { frames, result } = record(optimise(dist, start, method), dist.size);
    expectValidTour(result.tours[0], dist.size);
    expect(result.lengths[0]).toBeLessThan(start.length(dist));
    expect(start.toArray()).toEqual(before);
    expect(frames.length % 3).toBe(0);
    expect(frames.filter((f) => f.endOfStep)).toHaveLength(frames.length / 3);
  });

  it('ends where no move of its kind helps', () => {
    const tour = runToEnd(optimise(dist, start, method)).tours[0];
    const next = { opt1: nextOneOpt, opt2: nextTwoOpt, opt3: nextThreeOpt }[method];
    expect(next(dist, tour)).toBeNull();
  });

  it('makes no moves on an optimal tour', () => {
    const optimal = loadProblem('30CITY_4.TSP');
    const { frames, result } = record(optimise(optimal, Tour.identity(30), method), 30);
    expect(frames).toHaveLength(0);
    expect(result.tours[0].toArray()).toEqual(Tour.identity(30).toArray());
  });

  it('every move shortens the tour', () => {
    const run = optimise(dist, start, method);
    let last = start.length(dist);
    for (;;) {
      const step = run.next();
      if (step.done) break;
      if (step.value.endOfStep) {
        const now = step.value.views[0].tour.length(dist);
        expect(now).toBeLessThan(last);
        last = now;
      }
    }
  });
});

describe('2-opt', () => {
  it('removes every crossing', () => {
    const dist = loadProblem('50CITY_1.TSP');
    const tour = runToEnd(optimise(dist, Tour.identity(dist.size), 'opt2')).tours[0];
    const edges = [...tour.edges()];
    for (let i = 0; i < edges.length; i++) {
      for (let j = i + 2; j < edges.length; j++) {
        if (i === 0 && j === edges.length - 1) continue;
        expect(crosses(dist, ...edges[i], ...edges[j])).toBe(false);
      }
    }
  });

  it('shows the reversed segment in yellow and the swapped edges', () => {
    const dist = loadProblem('30CITIES.DI0');
    const { frames } = record(optimise(dist, Tour.identity(30), 'opt2'), 30);
    const [first, second] = frames;
    expect(first.views[0].edges).toBe('mask');
    const colors = new Set(first.views[0].marks.map((m) => m.color));
    expect(colors).toEqual(new Set([Ega.BLUE, Ega.YELLOW]));
    expect(second.views[0].lines.map((l) => l.color)).toEqual([Ega.BLACK, Ega.BLACK, Ega.LTRED, Ega.LTRED]);
  });
});

describe('1-opt', () => {
  it('turns the cities it touches white, as the original mask did', () => {
    const dist = loadProblem('30CITIES.DI2');
    const { frames } = record(optimise(dist, Tour.identity(30), 'opt1'), 30);
    const last = frames[frames.length - 1].views[0].baseColors;
    expect(last).toContain(Ega.WHITE);
    expect(frames[0].text.map((t) => t.text)).toEqual(
      expect.arrayContaining(['One Opt\nMethod', 'One Opt Method\n']),
    );
  });
});

describe('optimise', () => {
  it('needs at least four cities', () => {
    const three = new DistanceMatrix([
      { x: 0, y: 0 },
      { x: 1, y: 0 },
      { x: 0, y: 1 },
    ]);
    expect(() => runToEnd(optimise(three, Tour.identity(3), 'opt2'))).toThrow(RangeError);
  });
});
