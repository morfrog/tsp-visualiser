// VTTSP web port: starts on the original main screen with the bundled
// sample problems, plus the visitor's own files and Current from last time.

import { SAMPLES } from './data/samples';
import { load, save } from './data/storage';
import { MainScreen } from './ui/mainScreen';
import { initialMenu, toStored } from './ui/menuState';

const app = document.querySelector<HTMLElement>('#app')!;
const state = initialMenu(
  SAMPLES.map((s) => ({ name: s.name, cities: s.cities })),
  load(),
);
new MainScreen(app, state, (s) => save(toStored(s))).mount();

// Classic mode exists only under the dev server (see dev/classicPlugin.ts).
if (import.meta.env.DEV) {
  const footer = document.querySelector('.site-footer');
  const link = document.createElement('a');
  link.href = 'classic.html';
  link.textContent = 'Classic (dev only)';
  footer?.append(' · ', link);
}
