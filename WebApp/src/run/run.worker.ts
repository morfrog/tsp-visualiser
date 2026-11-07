// Runs a method off the main thread, so slow searches (3-opt on hundreds of
// cities takes about half a second per move) never freeze the page.

import { createStepper } from './session';
import type { FromWorker, ToWorker } from './protocol';
import type { Stepper } from './stepper';

let stepper: Stepper | null = null;

function post(message: FromWorker): void {
  self.postMessage(message);
}

self.onmessage = (event: MessageEvent<ToWorker>) => {
  const message = event.data;
  try {
    if (message.type === 'start') {
      stepper = createStepper(message);
      post({ type: 'started' });
    } else {
      if (!stepper) throw new Error('No run started.');
      post({ type: 'batch', id: message.id, batch: stepper.advance(message) });
    }
  } catch (error) {
    post({
      type: 'error',
      id: message.type === 'advance' ? message.id : undefined,
      message: error instanceof Error ? error.message : String(error),
    });
  }
};
