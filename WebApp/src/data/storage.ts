// Keeps the visitor's own files and Current between visits, as the original
// kept them on disk. Browser storage can be missing or refuse (private
// windows, blocked site data, full quota), so every access is guarded and
// the app works the same without it, just forgetting on reload.

import type { City } from '../core/geometry';

export const STORAGE_KEY = 'vttsp:v1';

export interface StoredFile {
  name: string;
  cities: City[];
}

export interface Stored {
  /** Files saved or opened by the visitor, in list order. */
  files: StoredFile[];
  /** The Current file, if one was written. */
  current: City[] | null;
}

/** The parts of Web Storage used here, so tests can pass a stand-in. */
export interface KeyValueStore {
  getItem(key: string): string | null;
  setItem(key: string, value: string): void;
}

export function browserStore(): KeyValueStore | null {
  try {
    return typeof localStorage === 'undefined' ? null : localStorage;
  } catch {
    return null; // accessing localStorage itself can throw
  }
}

function isCity(value: unknown): value is City {
  return (
    typeof value === 'object' &&
    value !== null &&
    Number.isFinite((value as City).x) &&
    Number.isFinite((value as City).y)
  );
}

function cities(value: unknown): City[] | null {
  if (!Array.isArray(value) || !value.every(isCity)) return null;
  return value.map((c) => ({ x: c.x, y: c.y }));
}

/** Reads what was stored; anything missing or malformed is ignored. */
export function load(store: KeyValueStore | null = browserStore()): Stored {
  const empty: Stored = { files: [], current: null };
  if (!store) return empty;
  try {
    const raw = store.getItem(STORAGE_KEY);
    if (!raw) return empty;
    const data = JSON.parse(raw) as { files?: unknown; current?: unknown };
    const files = Array.isArray(data.files)
      ? data.files.flatMap((f: unknown) => {
          const name = (f as StoredFile)?.name;
          const list = cities((f as StoredFile)?.cities);
          return typeof name === 'string' && name && list ? [{ name, cities: list }] : [];
        })
      : [];
    return { files, current: cities(data.current) };
  } catch {
    return empty;
  }
}

/** Writes; returns false if storage is unavailable or full. */
export function save(stored: Stored, store: KeyValueStore | null = browserStore()): boolean {
  if (!store) return false;
  try {
    store.setItem(STORAGE_KEY, JSON.stringify(stored));
    return true;
  } catch {
    return false;
  }
}
