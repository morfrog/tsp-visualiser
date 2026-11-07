// Temporary setup page for trying the player (phase 3). Phase 4 replaces it
// with the original main screen: file list, solver/method/init lists and
// buttons. Like the original, a finished insertion or iteration run becomes
// the current tour, so methods can be chained (build, then improve).

import { DEFAULT_ANNEAL_SETTINGS } from './algorithms/anneal';
import {
  ANNEALING_METHODS,
  INSERTION_METHODS,
  INSERTION_STARTS,
  ITERATION_METHODS,
  selectionProblem,
  SOLVERS,
  type Choice,
  type Selection,
  type SolverId,
} from './algorithms/catalog';
import { Role, type EgaColor } from './core/colors';
import { DistanceMatrix } from './core/distances';
import { clockSeed } from './core/random';
import { Tour } from './core/tour';
import { SAMPLES } from './data/samples';
import { showRun } from './player/runScreen';
import { drawView, fitCanvas } from './render/mapView';

const app = document.querySelector<HTMLElement>('#app')!;

interface Current {
  sample: number;
  tour: number[];
  /** Which run produced the current tour, if any. */
  madeBy: string | null;
}

const state = {
  current: { sample: Math.max(0, SAMPLES.findIndex((s) => s.name === '30CITY_0.TSP')), tour: [], madeBy: null } as Current,
  solver: 'insertion' as SolverId,
  method: 'nearest2',
  start: 'furthest',
  anneal: { ...DEFAULT_ANNEAL_SETTINGS },
};
state.current.tour = identity(SAMPLES[state.current.sample].cities.length);

let previewObserver: ResizeObserver | null = null;

function identity(n: number): number[] {
  return Array.from({ length: n }, (_, i) => i);
}

function options<T extends string>(
  select: HTMLSelectElement,
  choices: readonly Choice<T>[],
  value: string,
  markRandom = true,
): void {
  select.replaceChildren(
    ...choices.map((c) => {
      const o = document.createElement('option');
      o.value = c.id;
      o.textContent = `${c.name}${markRandom && !c.deterministic ? ' (random)' : ''}`;
      o.selected = c.id === value;
      return o;
    }),
  );
}

function selection(): Selection {
  switch (state.solver) {
    case 'insertion':
      return { solver: 'insertion', method: state.method as never, start: state.start as never };
    case 'iteration':
      return { solver: 'iteration', method: state.method as never };
    case 'annealing':
      return { solver: 'annealing', settings: { ...state.anneal } };
  }
}

function renderSetup(): void {
  const sample = SAMPLES[state.current.sample];
  const dist = new DistanceMatrix(sample.cities);
  const length = new Tour(state.current.tour).length(dist);

  app.innerHTML = `
    <section class="setup" aria-label="Choose a run">
      <div class="setup__map">
        <canvas class="map map--preview" role="img" aria-label="Current tour"></canvas>
        <p class="setup__caption"></p>
      </div>
      <form class="setup__form">
        <label>Map <select name="sample"></select></label>
        <label>Solver <select name="solver"></select></label>
        <label>Method <select name="method"></select></label>
        <label class="only-insertion">Initialisation <select name="start"></select></label>
        <fieldset class="only-annealing">
          <legend>Annealing settings</legend>
          <label>Temperature <input name="temperature" type="number" min="0" step="any"></label>
          <label>Cooling rate <input name="coolingRate" type="number" min="0" max="1" step="any"></label>
          <label>Changes at each temperature <input name="changes" type="number" min="1" step="1"></label>
        </fieldset>
        <label>Random seed <input name="seed" type="number" min="0" step="1"></label>
        <p class="setup__problem" role="alert"></p>
        <div class="setup__buttons">
          <button type="submit" class="btn btn--go">Go!</button>
          <button type="button" class="btn" name="reset">Reset tour</button>
        </div>
        <p class="setup__note">Temporary setup screen. The original main screen arrives in the next phase.</p>
      </form>
    </section>`;

  const form = app.querySelector<HTMLFormElement>('.setup__form')!;
  const field = <T extends HTMLElement>(name: string) => form.elements.namedItem(name) as T;

  const sampleSelect = field<HTMLSelectElement>('sample');
  sampleSelect.replaceChildren(
    ...SAMPLES.map((s, i) => {
      const o = document.createElement('option');
      o.value = String(i);
      o.textContent = `${s.name} (${s.cities.length} cities)`;
      o.selected = i === state.current.sample;
      return o;
    }),
  );
  options(field<HTMLSelectElement>('solver'), SOLVERS, state.solver, false);
  const methods = { insertion: INSERTION_METHODS, iteration: ITERATION_METHODS, annealing: ANNEALING_METHODS }[state.solver];
  if (!methods.some((m) => m.id === state.method)) state.method = methods[0].id;
  options(field<HTMLSelectElement>('method'), methods, state.method);
  options(field<HTMLSelectElement>('start'), INSERTION_STARTS, state.start);
  field<HTMLInputElement>('temperature').value = String(state.anneal.temperature);
  field<HTMLInputElement>('coolingRate').value = String(state.anneal.coolingRate);
  field<HTMLInputElement>('changes').value = String(state.anneal.changesPerTemperature);
  field<HTMLInputElement>('seed').value = String(clockSeed());
  form.classList.toggle('is-insertion', state.solver === 'insertion');
  form.classList.toggle('is-annealing', state.solver === 'annealing');

  const caption = app.querySelector<HTMLElement>('.setup__caption')!;
  caption.textContent = `${sample.name}: ${sample.cities.length} cities, tour length ${length.toFixed(6)}${
    state.current.madeBy ? ` (after ${state.current.madeBy})` : ''
  }`;
  const canvas = app.querySelector<HTMLCanvasElement>('.map--preview')!;
  const drawPreview = () => {
    fitCanvas(canvas);
    drawView(
      canvas.getContext('2d')!,
      { left: 0, top: 0, width: canvas.width, height: canvas.height },
      sample.cities,
      {
        tour: state.current.tour,
        closed: true,
        baseColors: new Array<EgaColor>(sample.cities.length).fill(state.current.madeBy ? Role.cityIn : Role.cityOut),
        marks: [],
        edges: 'plain',
        lines: [],
      },
      { lineWidth: Math.max(1, canvas.width / 460) },
    );
  };
  // One observer at a time: an old one fires when its canvas is removed.
  previewObserver?.disconnect();
  previewObserver = new ResizeObserver(() => {
    if (canvas.isConnected) drawPreview();
  });
  previewObserver.observe(canvas);

  const problem = app.querySelector<HTMLElement>('.setup__problem')!;
  const check = () => {
    problem.textContent = selectionProblem(sample.cities.length, selection()) ?? '';
  };
  check();

  form.addEventListener('change', (event) => {
    const target = event.target as HTMLInputElement | HTMLSelectElement;
    switch (target.name) {
      case 'sample':
        state.current = { sample: Number(target.value), tour: [], madeBy: null };
        state.current.tour = identity(SAMPLES[state.current.sample].cities.length);
        return renderSetup();
      case 'solver':
        state.solver = target.value as SolverId;
        return renderSetup();
      case 'method':
        state.method = target.value;
        break;
      case 'start':
        state.start = target.value;
        break;
      case 'temperature':
        state.anneal.temperature = Number(target.value);
        break;
      case 'coolingRate':
        state.anneal.coolingRate = Number(target.value);
        break;
      case 'changes':
        state.anneal.changesPerTemperature = Number(target.value);
        break;
    }
    check();
  });

  field<HTMLButtonElement>('reset').addEventListener('click', () => {
    state.current.tour = identity(sample.cities.length);
    state.current.madeBy = null;
    renderSetup();
  });

  form.addEventListener('submit', async (event) => {
    event.preventDefault();
    if (selectionProblem(sample.cities.length, selection())) return check();
    const seed = Math.trunc(Number(field<HTMLInputElement>('seed').value)) || 0;
    const methodName = methods.find((m) => m.id === state.method)?.name ?? '';
    const result = await showRun(app, {
      title: `${sample.name}: ${SOLVERS.find((s) => s.id === state.solver)!.name}, ${methodName}`,
      cities: sample.cities,
      tour: state.current.tour,
      selection: selection(),
      seed,
    });
    if (result.tour) {
      state.current.tour = result.tour;
      state.current.madeBy = methodName.trim();
    }
    renderSetup();
  });
}

renderSetup();
