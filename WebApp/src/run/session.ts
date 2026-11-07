// A running method, either in a Web Worker (in the browser) or on the
// current thread (tests, or browsers without module workers). Both sides use
// the same Stepper, so they behave identically.

import { selectionProblem, startRun } from '../algorithms/catalog';
import { DistanceMatrix } from '../core/distances';
import { AnsiCRandom } from '../core/random';
import { Tour } from '../core/tour';
import type { FromWorker, StartRequest, ToWorker } from './protocol';
import { Stepper, type AdvanceRequest, type StepBatch } from './stepper';

export interface RunSession {
  advance(request?: AdvanceRequest): Promise<StepBatch>;
  /** Stops the run and frees its worker. */
  dispose(): void;
}

/** The selection can't run on this map (too few cities, bad settings). */
export class RunStartError extends Error {
  constructor(message: string) {
    super(message);
    this.name = 'RunStartError';
  }
}

export function createStepper(request: StartRequest): Stepper {
  const problem = selectionProblem(request.cities.length, request.selection);
  if (problem) throw new RunStartError(problem);
  const dist = new DistanceMatrix(request.cities);
  const run = startRun(dist, new Tour(request.tour), request.selection, new AnsiCRandom(request.seed));
  return new Stepper(run);
}

/** Runs on the current thread. */
export class LocalSession implements RunSession {
  private readonly stepper: Stepper;

  constructor(request: StartRequest) {
    this.stepper = createStepper(request);
  }

  advance(request: AdvanceRequest = {}): Promise<StepBatch> {
    return Promise.resolve(this.stepper.advance(request));
  }

  dispose(): void {}
}

/** Runs in a Web Worker. */
export class WorkerSession implements RunSession {
  private nextId = 1;
  private readonly pending = new Map<number, { resolve: (b: StepBatch) => void; reject: (e: Error) => void }>();
  private failure: Error | null = null;

  private constructor(private readonly worker: Worker) {
    worker.onmessage = (event: MessageEvent<FromWorker>) => this.receive(event.data);
    worker.onerror = (event) => this.fail(new Error(event.message || 'The run worker failed.'));
  }

  static start(request: StartRequest): Promise<WorkerSession> {
    const worker = new Worker(new URL('./run.worker.ts', import.meta.url), { type: 'module' });
    const session = new WorkerSession(worker);
    return new Promise((resolve, reject) => {
      worker.onmessage = (event: MessageEvent<FromWorker>) => {
        const message = event.data;
        if (message.type === 'started') {
          worker.onmessage = (e: MessageEvent<FromWorker>) => session.receive(e.data);
          resolve(session);
        } else if (message.type === 'error') {
          // The worker loaded but refused the selection.
          worker.terminate();
          reject(new RunStartError(message.message));
        }
      };
      worker.onerror = (event) => {
        worker.terminate();
        reject(new Error(event.message || 'The run worker failed to start.'));
      };
      const start: ToWorker = { type: 'start', ...request };
      worker.postMessage(start);
    });
  }

  advance(request: AdvanceRequest = {}): Promise<StepBatch> {
    if (this.failure) return Promise.reject(this.failure);
    const id = this.nextId++;
    return new Promise((resolve, reject) => {
      this.pending.set(id, { resolve, reject });
      const message: ToWorker = { type: 'advance', id, ...request };
      this.worker.postMessage(message);
    });
  }

  dispose(): void {
    this.worker.terminate();
    this.fail(new Error('The run was stopped.'));
  }

  private receive(message: FromWorker): void {
    if (message.type === 'batch') {
      this.pending.get(message.id)?.resolve(message.batch);
      this.pending.delete(message.id);
    } else if (message.type === 'error') {
      const error = new Error(message.message);
      if (message.id !== undefined) {
        this.pending.get(message.id)?.reject(error);
        this.pending.delete(message.id);
      } else {
        this.fail(error);
      }
    }
  }

  private fail(error: Error): void {
    this.failure = error;
    for (const { reject } of this.pending.values()) reject(error);
    this.pending.clear();
  }
}

/** Starts a run in a worker if possible, otherwise on this thread. */
export async function startSession(request: StartRequest): Promise<RunSession> {
  if (typeof Worker !== 'undefined') {
    try {
      return await WorkerSession.start(request);
    } catch (error) {
      // A refused selection would fail locally too; anything else means the
      // worker itself couldn't load, so fall back to this thread.
      if (error instanceof RunStartError) throw error;
    }
  }
  return new LocalSession(request);
}
