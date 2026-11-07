// Drives a method's generator and turns its frames into plain data that can
// cross a worker boundary (frames themselves hold live Tour objects).
//
// The page asks for either one frame (stepping, timed play) or as much work
// as fits in a time budget (run to end). For a budget, only the last frame
// is kept, but the text from every frame in between is kept in order, so the
// side panel ends up exactly as if each frame had been drawn.

import type { CityMark, Frame, Line, MethodRun, TextUpdate, View } from '../algorithms/frame';
import type { EgaColor } from '../core/colors';

export interface ViewSnapshot {
  tour: number[];
  closed: boolean;
  baseColors: EgaColor[];
  marks: CityMark[];
  edges: View['edges'];
  lines: Line[];
}

export interface FrameSnapshot {
  views: ViewSnapshot[];
  endOfStep: boolean;
}

export interface RunOutcome {
  tours: number[][];
  lengths: number[];
}

export interface StepBatch {
  /** The last frame produced, or null if none was (the run had already ended). */
  frame: FrameSnapshot | null;
  /** Text from every frame produced, in order. */
  text: TextUpdate[];
  /** Frames produced in this batch. */
  frames: number;
  /** Set once the method has finished. */
  outcome?: RunOutcome;
}

export interface AdvanceRequest {
  /** Produce at most this many frames (default 1)... */
  frames?: number;
  /** ...or keep going until this much time has passed. */
  budgetMs?: number;
}

export function snapshotView(view: View): ViewSnapshot {
  return {
    tour: view.tour.toArray(),
    closed: view.closed,
    baseColors: [...view.baseColors],
    marks: view.marks.map((m) => ({ ...m })),
    edges: view.edges,
    lines: view.lines.map((l) => ({ ...l })),
  };
}

export function snapshotFrame(frame: Frame): FrameSnapshot {
  return { views: frame.views.map(snapshotView), endOfStep: frame.endOfStep };
}

export class Stepper {
  private outcome: RunOutcome | undefined;

  constructor(
    private readonly run: MethodRun,
    private readonly now: () => number = () => performance.now(),
  ) {}

  get finished(): boolean {
    return this.outcome !== undefined;
  }

  advance(request: AdvanceRequest = {}): StepBatch {
    const text: TextUpdate[] = [];
    if (this.outcome) return { frame: null, text, frames: 0, outcome: this.outcome };

    const budget = request.budgetMs;
    const maxFrames = budget === undefined ? (request.frames ?? 1) : Infinity;
    const deadline = budget === undefined ? Infinity : this.now() + budget;

    let last: Frame | null = null;
    let frames = 0;
    while (frames < maxFrames && (frames === 0 || this.now() < deadline)) {
      const step = this.run.next();
      if (step.done) {
        this.outcome = {
          tours: step.value.tours.map((t) => t.toArray()),
          lengths: [...step.value.lengths],
        };
        break;
      }
      last = step.value;
      text.push(...last.text);
      frames++;
    }
    return {
      frame: last ? snapshotFrame(last) : null,
      text,
      frames,
      outcome: this.outcome,
    };
  }
}
