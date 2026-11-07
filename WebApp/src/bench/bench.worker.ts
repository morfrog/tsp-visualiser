// Works out the benchmark tables off the main thread and reports each cell.

import { benchProblems } from './data';
import { runBenchmarks, type CellResult } from './runner';

export type BenchMessage = { type: 'cell'; cell: CellResult } | { type: 'done' } | { type: 'error'; message: string };

self.onmessage = (event: MessageEvent<{ seed: number }>) => {
  const post = (m: BenchMessage) => self.postMessage(m);
  try {
    for (const cell of runBenchmarks(benchProblems(), event.data.seed)) post({ type: 'cell', cell });
    post({ type: 'done' });
  } catch (error) {
    post({ type: 'error', message: error instanceof Error ? error.message : String(error) });
  }
};
