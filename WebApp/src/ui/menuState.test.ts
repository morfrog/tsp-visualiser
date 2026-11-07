import { describe, expect, it } from 'vitest';
import {
  addFile,
  annealItemNames,
  changeAnnealSetting,
  CURRENT,
  initialMenu,
  initItems,
  methodItems,
  selectedFile,
  selectedInit,
  selectedMethod,
  selectFile,
  selectInit,
  selectionLabel,
  selectMethod,
  selectSolver,
  setCurrent,
  toSelection,
} from './menuState';

const city = (x: number, y = 0) => ({ x, y });
const files = () => [
  { name: 'A.TSP', cities: [city(0), city(1), city(2), city(3)] },
  { name: 'B.TSP', cities: [city(5), city(6), city(7), city(8), city(9)] },
];

describe('initialMenu', () => {
  it('lists the files with Current last, a copy of the first', () => {
    const s = initialMenu(files());
    expect(s.files.map((f) => f.name)).toEqual(['A.TSP', 'B.TSP', CURRENT]);
    expect(s.files[2].cities).toEqual(s.files[0].cities);
    expect(s.files[2].cities).not.toBe(s.files[0].cities);
    expect(selectedFile(s).name).toBe('A.TSP');
  });

  it('starts on Insertion with N Neighbour, which has no init list (Legacy defaults)', () => {
    const s = initialMenu(files());
    expect(methodItems(s)[selectedMethod(s)].name).toBe('N Neighbour');
    expect(initItems(s)).toEqual([]);
    expect(selectedInit(s)).toBeNull();
    expect(toSelection(s)).toEqual({ solver: 'insertion', method: 'nearestNeighbour', start: 'random' });
  });
});

describe('choosing', () => {
  it('each solver remembers its own method', () => {
    const s = initialMenu(files());
    selectMethod(s, 6); // Nearest II
    selectSolver(s, 1);
    selectMethod(s, 2); // 3 opt
    expect(toSelection(s)).toEqual({ solver: 'iteration', method: 'opt3' });
    selectSolver(s, 0);
    expect(methodItems(s)[selectedMethod(s)].name).toBe('Nearest II');
  });

  it('insertion offers three starts once a method needs one; iteration offers none', () => {
    const s = initialMenu(files());
    selectMethod(s, 2);
    expect(initItems(s).map((i) => i.name)).toEqual(['Random', 'Closest', 'Furthest']);
    selectInit(s, 2);
    expect(toSelection(s)).toMatchObject({ method: 'cheapest', start: 'furthest' });
    selectSolver(s, 1);
    expect(initItems(s)).toEqual([]);
  });

  it('ignores out-of-range choices', () => {
    const s = initialMenu(files());
    selectFile(s, 9);
    selectMethod(s, 99);
    expect(s.selectedFile).toBe(0);
    expect(selectedMethod(s)).toBe(0);
  });

  it('labels the run', () => {
    const s = initialMenu(files());
    selectMethod(s, 7);
    expect(selectionLabel(s)).toBe('Insertion, Furthest II');
  });
});

describe('annealing settings', () => {
  it('shows them as the original did', () => {
    expect(annealItemNames({ temperature: 100, coolingRate: 0.8, changesPerTemperature: 50 })).toEqual([
      'Temp =100.00',
      'Cool =  0.80',
      'AtTmp=    50',
    ]);
  });

  it('accepts or refuses typed values with the original messages', () => {
    const s = initialMenu(files());
    selectSolver(s, 2);
    selectInit(s, 1);
    expect(changeAnnealSetting(s, '1.5')).toEqual({ ok: false, message: '0.0 < cooling rate < 1.0' });
    expect(s.anneal.coolingRate).toBe(0.8);
    expect(changeAnnealSetting(s, '.5')).toEqual({ ok: true, message: 'Cooling Rate Accepted' });
    expect(s.anneal.coolingRate).toBe(0.5);

    selectInit(s, 2);
    expect(changeAnnealSetting(s, '0')).toMatchObject({ ok: false });
    expect(changeAnnealSetting(s, '20')).toMatchObject({ ok: true, message: 'Changes At Temperature OK' });

    selectInit(s, 0);
    expect(changeAnnealSetting(s, '-3')).toMatchObject({ ok: false, message: 'Temperature Must Be > 0.0' });
    expect(changeAnnealSetting(s, '0')).toMatchObject({ ok: true });
    expect(toSelection(s)).toEqual({
      solver: 'annealing',
      settings: { temperature: 0, coolingRate: 0.5, changesPerTemperature: 20 },
    });
  });
});

describe('Current and saved files', () => {
  it('setCurrent replaces Current and selects it', () => {
    const s = initialMenu(files());
    setCurrent(s, [city(9), city(8), city(7)]);
    expect(selectedFile(s)).toEqual({ name: CURRENT, cities: [city(9), city(8), city(7)] });
  });

  it('lists saved .TSP files before Current, keeping Current selected', () => {
    const s = initialMenu(files());
    setCurrent(s, [city(1), city(2), city(3)]);
    expect(addFile(s, 'MINE.TSP', selectedFile(s).cities)).toBe(true);
    expect(s.files.map((f) => f.name)).toEqual(['A.TSP', 'B.TSP', 'MINE.TSP', CURRENT]);
    expect(selectedFile(s).name).toBe(CURRENT);
  });

  it('replaces a file saved under the same name, and lists only .TSP names', () => {
    const s = initialMenu(files());
    addFile(s, 'a.tsp', [city(4), city(4), city(4)]);
    expect(s.files).toHaveLength(3);
    expect(s.files[0].cities).toHaveLength(3);
    expect(addFile(s, 'notes.txt', [])).toBe(false);
    expect(s.files).toHaveLength(3);
  });
});
