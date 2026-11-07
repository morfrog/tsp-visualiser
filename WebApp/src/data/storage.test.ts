import { describe, expect, it } from 'vitest';
import { load, save, STORAGE_KEY, type KeyValueStore } from './storage';

function memory(initial: Record<string, string> = {}): KeyValueStore & { data: Record<string, string> } {
  const data = { ...initial };
  return {
    data,
    getItem: (k) => (k in data ? data[k] : null),
    setItem: (k, v) => {
      data[k] = v;
    },
  };
}

const stored = {
  files: [{ name: 'MINE.TSP', cities: [{ x: 0.1, y: 0.2 }, { x: 0.3, y: 0.4 }, { x: 0.5, y: 0.6 }] }],
  current: [{ x: 0.9, y: 0.8 }],
};

describe('storage', () => {
  it('round-trips files and Current', () => {
    const store = memory();
    expect(save(stored, store)).toBe(true);
    expect(load(store)).toEqual(stored);
  });

  it('starts empty when nothing is stored or storage is missing', () => {
    expect(load(memory())).toEqual({ files: [], current: null });
    expect(load(null)).toEqual({ files: [], current: null });
    expect(save(stored, null)).toBe(false);
  });

  it('ignores damaged or unexpected data', () => {
    expect(load(memory({ [STORAGE_KEY]: '{not json' }))).toEqual({ files: [], current: null });
    const odd = JSON.stringify({
      files: [
        { name: 'OK.TSP', cities: [{ x: 0, y: 0 }] },
        { name: '', cities: [] },
        { name: 'BAD.TSP', cities: [{ x: 'a', y: 0 }] },
        null,
      ],
      current: 'nope',
    });
    expect(load(memory({ [STORAGE_KEY]: odd }))).toEqual({
      files: [{ name: 'OK.TSP', cities: [{ x: 0, y: 0 }] }],
      current: null,
    });
  });

  it('reports a store that refuses to write (full, or blocked)', () => {
    const refusing: KeyValueStore = {
      getItem: () => {
        throw new Error('blocked');
      },
      setItem: () => {
        throw new Error('QuotaExceededError');
      },
    };
    expect(save(stored, refusing)).toBe(false);
    expect(load(refusing)).toEqual({ files: [], current: null });
  });
});
