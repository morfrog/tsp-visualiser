// Test helper: reads the original 1997 data files from the repository's Data/ folder.

import { readFileSync, readdirSync } from 'node:fs';
import { dirname, join } from 'node:path';
import { fileURLToPath } from 'node:url';
import { decodeTspBytes } from '../core/tspfile';

export const DATA_DIR = join(dirname(fileURLToPath(import.meta.url)), '../../../Data');

export function readDataFile(name: string): string {
  return decodeTspBytes(readFileSync(join(DATA_DIR, name)));
}

export function listTspFiles(): string[] {
  return readdirSync(DATA_DIR)
    .filter((f) => f.toUpperCase().endsWith('.TSP'))
    .sort();
}
