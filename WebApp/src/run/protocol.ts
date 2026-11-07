// Messages between the page and the run worker.

import type { Selection } from '../algorithms/catalog';
import type { City } from '../core/geometry';
import type { AdvanceRequest, StepBatch } from './stepper';

export interface StartRequest {
  cities: City[];
  /** Current tour, used by iteration and annealing. */
  tour: number[];
  selection: Selection;
  seed: number;
}

export type ToWorker =
  | ({ type: 'start' } & StartRequest)
  | ({ type: 'advance'; id: number } & AdvanceRequest);

export type FromWorker =
  | { type: 'started' }
  | { type: 'batch'; id: number; batch: StepBatch }
  | { type: 'error'; id?: number; message: string };
