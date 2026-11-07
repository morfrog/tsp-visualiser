// What the main screen has chosen, and the rules for changing it (Legacy
// INTERFAC.C: SetFile, SetSolver, SetMethod, SetInit, AnnealInitChange).
//
// As in the original, a map's city order is its tour: selecting a file
// starts from the tour in file order, and a finished run, New and Randomize
// all write the result to the "Current" file, listed last, and select it.

import { DEFAULT_ANNEAL_SETTINGS, validateAnnealSettings, type AnnealSettings } from '../algorithms/anneal';
import {
  ANNEALING_METHODS,
  ANNEALING_SETTINGS_HELP,
  INSERTION_METHODS,
  INSERTION_STARTS,
  ITERATION_METHODS,
  SOLVERS,
  type Choice,
  type Selection,
  type SolverId,
} from '../algorithms/catalog';
import { f, d } from '../algorithms/text';
import type { City } from '../core/geometry';
import type { Stored } from '../data/storage';

export const CURRENT = 'Current';

export interface MapFile {
  name: string;
  cities: City[];
  /** Saved or opened by the visitor (kept between visits), not a bundled sample. */
  user?: boolean;
}

export interface MenuState {
  /** Listed files; the last is always Current. */
  files: MapFile[];
  /** The bundled samples, to restore one a user file had replaced. */
  samples: MapFile[];
  selectedFile: number;
  solver: number;
  /** Each solver remembers its own method (Legacy insmethodnum etc.). */
  methods: Record<SolverId, number>;
  /** Insertion and annealing each remember their own init. */
  inits: { insertion: number; annealing: number };
  anneal: AnnealSettings;
}

export interface ListItem {
  name: string;
  help: string;
}

const METHODS: Record<SolverId, readonly Choice<string>[]> = {
  insertion: INSERTION_METHODS,
  iteration: ITERATION_METHODS,
  annealing: ANNEALING_METHODS,
};

const ANNEAL_KEYS: readonly (keyof AnnealSettings)[] = ['temperature', 'coolingRate', 'changesPerTemperature'];

/**
 * The files found plus Current (Legacy SetFile INITIALISE): the bundled
 * samples, then the visitor's stored files (one with a sample's name replaces
 * it, as saving over a file did), then Current, as stored or else a copy of
 * the first file.
 */
export function initialMenu(samples: readonly MapFile[], stored: Stored = { files: [], current: null }): MenuState {
  const copy = (file: MapFile): MapFile => ({ ...file, cities: file.cities.map((c) => ({ ...c })) });
  const listed = samples.map((file) => copy({ name: file.name, cities: file.cities }));
  for (const file of stored.files) {
    const user = copy({ name: file.name, cities: file.cities, user: true });
    const i = listed.findIndex((f) => sameName(f.name, file.name));
    if (i >= 0) listed[i] = user;
    else listed.push(user);
  }
  const first = listed[0]?.cities ?? [];
  const current = stored.current ?? first;
  return {
    files: [...listed, { name: CURRENT, cities: current.map((c) => ({ ...c })) }],
    samples: samples.map(copy),
    selectedFile: 0,
    solver: 0,
    methods: { insertion: 0, iteration: 0, annealing: 0 },
    inits: { insertion: 0, annealing: 0 },
    anneal: { ...DEFAULT_ANNEAL_SETTINGS },
  };
}

function sameName(a: string, b: string): boolean {
  return a.toUpperCase() === b.toUpperCase();
}

/** What to keep between visits: the visitor's files and Current. */
export function toStored(state: MenuState): Stored {
  const last = state.files.length - 1;
  return {
    files: state.files.slice(0, last).filter((f) => f.user).map((f) => ({ name: f.name, cities: f.cities })),
    current: state.files[last].cities,
  };
}

export function solverId(state: MenuState): SolverId {
  return SOLVERS[state.solver].id;
}

export function selectedFile(state: MenuState): MapFile {
  return state.files[state.selectedFile];
}

export function methodItems(state: MenuState): ListItem[] {
  return METHODS[solverId(state)].map((m) => ({ name: m.name, help: m.help }));
}

export function selectedMethod(state: MenuState): number {
  return state.methods[solverId(state)];
}

/** Annealing settings as the init list showed them (Legacy SetInit INITIALISE). */
export function annealItemNames(s: AnnealSettings): string[] {
  return [
    `Temp =${f(s.temperature, 6, 2)}`,
    `Cool =${f(s.coolingRate, 6, 2)}`,
    `AtTmp=${d(s.changesPerTemperature, 6)}`,
  ];
}

/**
 * The init list: insertion's three starts (none for Nearest Neighbour, which
 * picks its own), none for iteration, annealing's three settings.
 */
export function initItems(state: MenuState): ListItem[] {
  switch (solverId(state)) {
    case 'insertion':
      if (INSERTION_METHODS[state.methods.insertion].id === 'nearestNeighbour') return [];
      return INSERTION_STARTS.map((s) => ({ name: s.name, help: s.help }));
    case 'iteration':
      return [];
    case 'annealing':
      return annealItemNames(state.anneal).map((name, i) => ({ name, help: ANNEALING_SETTINGS_HELP[ANNEAL_KEYS[i]] }));
  }
}

export function selectedInit(state: MenuState): number | null {
  const id = solverId(state);
  if (initItems(state).length === 0) return null;
  return id === 'annealing' ? state.inits.annealing : state.inits.insertion;
}

export function selectFile(state: MenuState, index: number): void {
  if (index >= 0 && index < state.files.length) state.selectedFile = index;
}

export function selectSolver(state: MenuState, index: number): void {
  if (index >= 0 && index < SOLVERS.length) state.solver = index;
}

export function selectMethod(state: MenuState, index: number): void {
  if (index >= 0 && index < methodItems(state).length) state.methods[solverId(state)] = index;
}

export function selectInit(state: MenuState, index: number): void {
  if (index < 0 || index >= initItems(state).length) return;
  if (solverId(state) === 'annealing') state.inits.annealing = index;
  else state.inits.insertion = index;
}

/** The selected annealing setting: its key and the title used to prompt for it. */
export function annealSetting(state: MenuState): keyof AnnealSettings {
  return ANNEAL_KEYS[state.inits.annealing];
}

/**
 * Sets the selected annealing value from typed text (Legacy AnnealInitChange).
 * Returns the message the original showed and whether it was accepted.
 */
export function changeAnnealSetting(state: MenuState, typed: string): { ok: boolean; message: string } {
  const key = annealSetting(state);
  // atof/atoi: a leading number, else 0.
  const value = key === 'changesPerTemperature' ? parseInt(typed, 10) || 0 : parseFloat(typed) || 0;
  const next = { ...state.anneal, [key]: value };
  const problem = validateAnnealSettings(next);
  const messages: Record<keyof AnnealSettings, [string, string]> = {
    temperature: ['Temperature Accepted', 'Temperature Must Be > 0.0'],
    coolingRate: ['Cooling Rate Accepted', '0.0 < cooling rate < 1.0'],
    changesPerTemperature: ['Changes At Temperature OK', 'Must be at least one change...'],
  };
  if (problem) return { ok: false, message: messages[key][1] };
  state.anneal = next;
  return { ok: true, message: messages[key][0] };
}

/** Replaces Current and selects it. */
export function setCurrent(state: MenuState, cities: City[]): void {
  const last = state.files.length - 1;
  state.files[last] = { name: CURRENT, cities };
  state.selectedFile = last;
}

/**
 * Adds or replaces the visitor's file. Like the original's directory scan,
 * saved files are only listed if their names end in .TSP; opened files are
 * listed whatever their name (`anyName`). They go before Current.
 */
export function addFile(state: MenuState, name: string, cities: City[], anyName = false): boolean {
  if (!anyName && !/\.tsp$/i.test(name)) return false;
  const copy: MapFile = { name, cities: cities.map((c) => ({ ...c })), user: true };
  const last = state.files.length - 1;
  const existing = state.files.findIndex((file, i) => i < last && sameName(file.name, name));
  if (existing >= 0) {
    state.files[existing] = copy;
  } else {
    state.files.splice(last, 0, copy);
    if (state.selectedFile === last) state.selectedFile++; // keep Current selected
  }
  return true;
}

/** Index of a listed file by name (not Current), or -1. */
export function fileIndex(state: MenuState, name: string): number {
  return state.files.findIndex((file, i) => i < state.files.length - 1 && sameName(file.name, name));
}

/**
 * Forgets one of the visitor's files. A sample it had replaced comes back.
 * Returns false for samples and Current, which can't be removed.
 */
export function removeFile(state: MenuState, index: number): boolean {
  const last = state.files.length - 1;
  const file = state.files[index];
  if (index < 0 || index >= last || !file.user) return false;
  const sample = state.samples.find((s) => sameName(s.name, file.name));
  if (sample) {
    state.files[index] = { name: sample.name, cities: sample.cities.map((c) => ({ ...c })) };
  } else {
    state.files.splice(index, 1);
    if (state.selectedFile > index) state.selectedFile--;
    else if (state.selectedFile === index) state.selectedFile = Math.min(index, state.files.length - 1);
  }
  return true;
}

/** The method to run (Legacy Go!). */
export function toSelection(state: MenuState): Selection {
  switch (solverId(state)) {
    case 'insertion':
      return {
        solver: 'insertion',
        method: INSERTION_METHODS[state.methods.insertion].id,
        start: INSERTION_STARTS[state.inits.insertion].id,
      };
    case 'iteration':
      return { solver: 'iteration', method: ITERATION_METHODS[state.methods.iteration].id };
    case 'annealing':
      return { solver: 'annealing', settings: { ...state.anneal } };
  }
}

/** "Insertion, Nearest II" etc., for the run screen's title. */
export function selectionLabel(state: MenuState): string {
  return `${SOLVERS[state.solver].name}, ${methodItems(state)[selectedMethod(state)].name}`;
}
