// Shared checks for the algorithm tests.

import { expect } from 'vitest';
import type { Frame, MethodResult, MethodRun } from '../algorithms/frame';
import { DistanceMatrix } from '../core/distances';
import type { Tour } from '../core/tour';
import { parseTsp } from '../core/tspfile';
import { readDataFile } from './data';

export function loadProblem(name: string): DistanceMatrix {
  return new DistanceMatrix(parseTsp(readDataFile(name)));
}

/** The tour visits every city exactly once. */
export function expectValidTour(tour: Tour, n: number): void {
  expect([...tour.toArray()].sort((a, b) => a - b)).toEqual(Array.from({ length: n }, (_, i) => i));
}

export interface Recorded {
  frames: Frame[];
  /** Tour sizes of the first view at each frame (frames hold live tours). */
  sizes: number[];
  result: MethodResult;
}

/** Runs to the end, checking each frame refers only to real cities. */
export function record(run: MethodRun, n: number): Recorded {
  const frames: Frame[] = [];
  const sizes: number[] = [];
  for (;;) {
    const step = run.next();
    if (step.done) return { frames, sizes, result: step.value };
    const frame = step.value;
    for (const view of frame.views) {
      expect(view.baseColors).toHaveLength(n);
      for (const m of view.marks) expect(m.city >= 0 && m.city < n).toBe(true);
      for (const l of view.lines) expect(l.from >= 0 && l.from < n && l.to >= 0 && l.to < n).toBe(true);
    }
    frames.push(frame);
    sizes.push(frame.views[0].tour.size);
  }
}
