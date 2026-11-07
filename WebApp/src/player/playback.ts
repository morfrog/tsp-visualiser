// Playback control, ported from Legacy/PROBLEM.C (TotalDelay, WaitTill,
// DoInput) and kept free of the DOM so it can be tested.
//
// The original had three modes, switched by keys at any pause:
//   - wait for a key at every pause (the default when a run starts; Enter);
//   - timed: digit d sets d x 50 clock ticks (d x 0.5 s) per step, split
//     evenly between the step's pauses ('1'-'9');
//   - run to the end without waiting ('0').
// Esc stops the run. While waiting, any key ends the wait at once.

import type { RunOutcome, StepBatch, FrameSnapshot } from '../run/stepper';
import type { RunSession } from '../run/session';

export type Command =
  | { kind: 'continue' }
  | { kind: 'abort' }
  | { kind: 'fast' }
  | { kind: 'keyMode' }
  | { kind: 'speed'; digit: number };

export type Mode = { kind: 'key' } | { kind: 'timed'; digit: number; stepMs: number } | { kind: 'fast' };

/** One digit step: 50 ticks of Watcom's 100-per-second clock(). */
export const MS_PER_SPEED_DIGIT = 500;

/** Work per animation frame when running to the end. */
export const FAST_BUDGET_MS = 12;

/** Maps a key to what the original did with it (Legacy DoInput). */
export function commandForKey(key: string): Command | null {
  if (key === 'Escape') return { kind: 'abort' };
  if (key === 'Enter') return { kind: 'keyMode' };
  if (key === '0') return { kind: 'fast' };
  if (/^[1-9]$/.test(key)) return { kind: 'speed', digit: Number(key) };
  // Any other key just continues; ignore modifiers and navigation keys.
  if (key.length === 1) return { kind: 'continue' };
  return null;
}

export function modeFor(command: Command, current: Mode): Mode {
  switch (command.kind) {
    case 'fast':
      return { kind: 'fast' };
    case 'keyMode':
      return { kind: 'key' };
    case 'speed':
      return { kind: 'timed', digit: command.digit, stepMs: command.digit * MS_PER_SPEED_DIGIT };
    default:
      return current;
  }
}

/** Commands from keys and buttons, waited on by playback. */
export class CommandQueue {
  private queue: Command[] = [];
  private waiter: ((c: Command | null) => void) | null = null;

  push(command: Command): void {
    if (this.waiter) {
      const resolve = this.waiter;
      this.waiter = null;
      resolve(command);
    } else {
      this.queue.push(command);
    }
  }

  /** Next queued command, without waiting. */
  poll(): Command | null {
    return this.queue.shift() ?? null;
  }

  /** Waits for a command, or until timeoutMs passes (then null). */
  wait(timeoutMs?: number, setTimer = setTimeout, clearTimer = clearTimeout): Promise<Command | null> {
    const queued = this.poll();
    if (queued) return Promise.resolve(queued);
    if (timeoutMs !== undefined && timeoutMs <= 0) return Promise.resolve(null);
    return new Promise((resolve) => {
      let timer: ReturnType<typeof setTimeout> | undefined;
      this.waiter = (c) => {
        if (timer !== undefined) clearTimer(timer);
        resolve(c);
      };
      if (timeoutMs !== undefined) {
        timer = setTimer(() => {
          this.waiter = null;
          resolve(null);
        }, timeoutMs);
      }
    });
  }

  clear(): void {
    this.queue = [];
  }
}

export interface PlaybackDeps {
  session: RunSession;
  /** Pauses per step for this method, to split a timed step's delay. */
  pausesPerStep: number;
  commands: CommandQueue;
  /** Applies a batch's text and draws its frame. */
  show(batch: StepBatch): void;
  /** Resolves on the next animation frame. */
  nextTick(): Promise<void>;
  now(): number;
  onModeChange?(mode: Mode): void;
}

export interface PlaybackResult {
  outcome?: RunOutcome;
  aborted: boolean;
  /** The last frame drawn, so an aborted run can still show where it got to. */
  lastFrame: FrameSnapshot | null;
}

/** Plays a run to its end or until stopped. */
export async function playback(deps: PlaybackDeps, initial: Mode = { kind: 'key' }): Promise<PlaybackResult> {
  let mode = initial;
  let lastFrame: FrameSnapshot | null = null;
  let stepStart = deps.now();
  let pausesInStep = 0;
  deps.onModeChange?.(mode);

  /** Applies a command; returns true if the run should stop. */
  const handle = (command: Command): boolean => {
    if (command.kind === 'abort') return true;
    const next = modeFor(command, mode);
    if (next !== mode) {
      mode = next;
      deps.onModeChange?.(mode);
      // Legacy re-split the delay (Init3TimeSegs) from the step's start.
    }
    return false;
  };

  const stop = (): PlaybackResult => {
    deps.session.dispose();
    return { aborted: true, lastFrame };
  };

  for (;;) {
    const batch = await deps.session.advance(mode.kind === 'fast' ? { budgetMs: FAST_BUDGET_MS } : { frames: 1 });
    deps.show(batch);
    if (batch.frame) lastFrame = batch.frame;
    if (batch.outcome) {
      deps.session.dispose();
      return { outcome: batch.outcome, aborted: false, lastFrame };
    }
    pausesInStep += batch.frames;

    if (mode.kind === 'key') {
      const command = await deps.commands.wait();
      if (command && handle(command)) return stop();
    } else if (mode.kind === 'timed') {
      const deadline = stepStart + (Math.min(pausesInStep, deps.pausesPerStep) * mode.stepMs) / deps.pausesPerStep;
      const command = await deps.commands.wait(deadline - deps.now());
      // Any key ends the wait early (Legacy WaitTill returned on kbhit()).
      if (command && handle(command)) return stop();
    } else {
      await deps.nextTick();
      for (let c = deps.commands.poll(); c; c = deps.commands.poll()) if (handle(c)) return stop();
    }

    if (batch.frame?.endOfStep || pausesInStep >= deps.pausesPerStep) {
      stepStart = deps.now();
      pausesInStep = 0;
    }
  }
}
