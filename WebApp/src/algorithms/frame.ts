// What an algorithm hands the player at each pause.
//
// The 1997 algorithms drew straight to the screen and then blocked in
// WaitTill() for a delay or a key press. Here each algorithm is a generator
// that yields a Frame at every point where the original paused, and returns
// its result when finished. The player (phase 3) decides how long to wait
// between frames, steps one frame per key press, or aborts by simply not
// resuming the generator.
//
// Frames hold live references: `tour` and `baseColors` keep changing as the
// algorithm runs, so draw a frame before resuming the generator.

import type { EgaColor } from '../core/colors';
import type { Tour } from '../core/tour';

/** A city drawn in a highlight colour, optionally at double size. */
export interface CityMark {
  city: number;
  color: EgaColor;
  large?: boolean;
}

/** An extra line between two cities, drawn after the tour's edges. Black erases. */
export interface Line {
  from: number;
  to: number;
  color: EgaColor;
}

/** One map window: a tour plus highlights. */
export interface View {
  tour: Tour;
  /** Draw the edge from the last city back to the head. */
  closed: boolean;
  /** Each city's normal colour (Legacy MASK), indexed by city number. */
  baseColors: readonly EgaColor[];
  /** Cities drawn differently in this frame only. */
  marks: readonly CityMark[];
  /**
   * How tour edges are coloured. 'plain': all LTRED (Legacy DrawPath).
   * 'mask': an edge takes its cities' colour when both ends share it, else
   * LTRED (Legacy DrawMaskPath), which shows the 2-opt and 3-opt segments.
   */
  edges: 'plain' | 'mask';
  lines: readonly Line[];
}

/** Text areas: the side panel, or the annealing screen's top bar and panes. */
export type TextArea = 'info' | 'top' | 'left' | 'right';

/**
 * Text written into an area's character grid at a 1-based row and column, as
 * Legacy ShowText did. Characters overwrite what is there; nothing is
 * cleared, so trailing spaces matter. "\n" moves to column 1 of the next row.
 */
export interface TextUpdate {
  area: TextArea;
  row: number;
  col: number;
  text: string;
  color: EgaColor;
}

export interface Frame {
  /** One view for insertion and iteration; two (1-opt, 2-opt) for annealing. */
  views: readonly View[];
  text: readonly TextUpdate[];
  /** This frame finishes one step (one city inserted, one move made). */
  endOfStep: boolean;
}

export interface MethodResult {
  /** Final tour, or tours for annealing (1-opt, then 2-opt). */
  tours: Tour[];
  /** Length of each tour. */
  lengths: number[];
}

export type MethodRun = Generator<Frame, MethodResult, void>;

/** Runs a method to the end without drawing, as the benchmark modes did. */
export function runToEnd(run: MethodRun): MethodResult {
  for (;;) {
    const step = run.next();
    if (step.done) return step.value;
  }
}
