import { describe, expect, it } from 'vitest';
import type { RunSession } from '../run/session';
import type { StepBatch } from '../run/stepper';
import { CommandQueue, commandForKey, MS_PER_SPEED_DIGIT, playback, type Mode } from './playback';

/** A fake run of `total` frames, `perStep` frames per step. */
function fakeSession(total: number, perStep = 3) {
  let produced = 0;
  const requests: (number | 'budget')[] = [];
  let disposed = false;
  const session: RunSession = {
    advance: async (request = {}) => {
      const count = request.budgetMs !== undefined ? 5 : (request.frames ?? 1);
      requests.push(request.budgetMs !== undefined ? 'budget' : count);
      const frames = Math.min(count, total - produced);
      produced += frames;
      const batch: StepBatch = {
        frame: frames > 0 ? { views: [], endOfStep: produced % perStep === 0 } : null,
        text: [],
        frames,
      };
      if (produced >= total) batch.outcome = { tours: [[0, 1, 2]], lengths: [1] };
      return batch;
    },
    dispose: () => {
      disposed = true;
    },
  };
  return { session, requests, isDisposed: () => disposed, produced: () => produced };
}

function deps(session: RunSession, commands: CommandQueue, clock: { t: number }, modes: Mode[] = []) {
  return {
    session,
    pausesPerStep: 3,
    commands,
    show: () => {},
    nextTick: async () => {
      clock.t += 16;
    },
    now: () => clock.t,
    onModeChange: (m: Mode) => modes.push(m),
  };
}

const tick = () => new Promise((r) => setTimeout(r, 0));

describe('commandForKey', () => {
  it('maps keys the way DoInput did', () => {
    expect(commandForKey('Escape')).toEqual({ kind: 'abort' });
    expect(commandForKey('Enter')).toEqual({ kind: 'keyMode' });
    expect(commandForKey('0')).toEqual({ kind: 'fast' });
    expect(commandForKey('7')).toEqual({ kind: 'speed', digit: 7 });
    expect(commandForKey('x')).toEqual({ kind: 'continue' });
    expect(commandForKey(' ')).toEqual({ kind: 'continue' });
    expect(commandForKey('Shift')).toBeNull();
  });
});

describe('CommandQueue', () => {
  it('hands a pushed command to a waiter, or queues it', async () => {
    const q = new CommandQueue();
    const waiting = q.wait();
    q.push({ kind: 'continue' });
    expect(await waiting).toEqual({ kind: 'continue' });
    q.push({ kind: 'fast' });
    expect(q.poll()).toEqual({ kind: 'fast' });
    expect(q.poll()).toBeNull();
  });

  it('times out with null', async () => {
    const q = new CommandQueue();
    expect(await q.wait(0)).toBeNull();
    expect(await q.wait(5)).toBeNull();
  });
});

describe('playback', () => {
  it('starts by waiting for a key at every pause', async () => {
    const fake = fakeSession(6);
    const commands = new CommandQueue();
    const done = playback(deps(fake.session, commands, { t: 0 }));
    await tick();
    expect(fake.produced()).toBe(1);
    commands.push({ kind: 'continue' });
    await tick();
    expect(fake.produced()).toBe(2);
    for (let i = 0; i < 4; i++) {
      commands.push({ kind: 'continue' });
      await tick();
    }
    const result = await done;
    expect(result.aborted).toBe(false);
    expect(result.outcome?.tours[0]).toEqual([0, 1, 2]);
    expect(fake.isDisposed()).toBe(true);
  });

  it("'0' runs to the end in time-budgeted batches", async () => {
    const fake = fakeSession(40);
    const commands = new CommandQueue();
    const modes: Mode[] = [];
    const done = playback(deps(fake.session, commands, { t: 0 }, modes));
    await tick();
    commands.push({ kind: 'fast' });
    const result = await done;
    expect(result.outcome).toBeDefined();
    expect(fake.requests.slice(1).every((r) => r === 'budget')).toBe(true);
    expect(modes.map((m) => m.kind)).toEqual(['key', 'fast']);
  });

  it('Esc stops the run and frees the session', async () => {
    const fake = fakeSession(100);
    const commands = new CommandQueue();
    const done = playback(deps(fake.session, commands, { t: 0 }));
    await tick();
    commands.push({ kind: 'abort' });
    const result = await done;
    expect(result).toMatchObject({ aborted: true });
    expect(result.lastFrame).not.toBeNull();
    expect(fake.isDisposed()).toBe(true);
  });

  it('a digit plays with digit x 0.5 s per step, split across its pauses', async () => {
    const fake = fakeSession(6);
    const commands = new CommandQueue();
    const modes: Mode[] = [];
    const clock = { t: 0 };
    commands.push({ kind: 'speed', digit: 1 });
    const started = Date.now();
    const result = await playback(deps(fake.session, commands, clock, modes), { kind: 'key' });
    expect(result.outcome).toBeDefined();
    expect(modes[1]).toEqual({ kind: 'timed', digit: 1, stepMs: MS_PER_SPEED_DIGIT });
    // The fake clock never advances, so each wait lasts its full share.
    expect(Date.now() - started).toBeGreaterThanOrEqual(400);
  });
});
