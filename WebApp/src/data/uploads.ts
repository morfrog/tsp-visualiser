// Reading .TSP files the visitor opens or drops onto the main screen.

import type { City } from '../core/geometry';
import { decodeTspBytes, parseTsp, TspParseError } from '../core/tspfile';

/** Larger files are refused before reading. */
export const MAX_FILE_BYTES = 1_000_000;
/** The original warned that 1000 cities pushed it to its limits; 3-opt is O(n^3) per move. */
export const MAX_CITIES = 2000;

export type UploadResult = { ok: true; name: string; cities: City[] } | { ok: false; name: string; error: string };

/** Checks and parses one file's contents. Coordinates must lie in [0, 1], as the original drew. */
export function readTsp(name: string, bytes: ArrayBuffer | Uint8Array): UploadResult {
  if (bytes.byteLength > MAX_FILE_BYTES) return { ok: false, name, error: 'Too big (over 1 MB).' };
  let cities: City[];
  try {
    cities = parseTsp(decodeTspBytes(bytes));
  } catch (error) {
    return { ok: false, name, error: error instanceof TspParseError ? error.message : 'Not a .TSP file.' };
  }
  if (cities.length > MAX_CITIES) return { ok: false, name, error: `Over ${MAX_CITIES} cities.` };
  if (cities.some((c) => c.x < 0 || c.x > 1 || c.y < 0 || c.y > 1)) {
    return { ok: false, name, error: 'Coordinates must be 0 to 1.' };
  }
  return { ok: true, name, cities };
}

/** Reads browser File objects. */
export async function readFiles(files: Iterable<File>): Promise<UploadResult[]> {
  const results: UploadResult[] = [];
  for (const file of files) {
    if (file.size > MAX_FILE_BYTES) {
      results.push({ ok: false, name: file.name, error: 'Too big (over 1 MB).' });
      continue;
    }
    try {
      results.push(readTsp(file.name, await file.arrayBuffer()));
    } catch {
      results.push({ ok: false, name: file.name, error: 'Could not be read.' });
    }
  }
  return results;
}
