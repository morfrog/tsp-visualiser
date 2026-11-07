// VTTSP web port: starts on the original main screen with the bundled
// sample problems.

import { SAMPLES } from './data/samples';
import { MainScreen } from './ui/mainScreen';
import { initialMenu } from './ui/menuState';

const app = document.querySelector<HTMLElement>('#app')!;
const state = initialMenu(SAMPLES.map((s) => ({ name: s.name, cities: s.cities })));
new MainScreen(app, state).mount();
