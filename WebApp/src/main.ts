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
