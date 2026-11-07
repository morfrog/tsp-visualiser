// The sample problems from the repository's Data/ folder, bundled at build
// time. The original listed every *.TSP file in its directory.

import type { City } from '../core/geometry';
import { parseTsp } from '../core/tspfile';

const files = import.meta.glob('../../../Data/*.TSP', { query: '?raw', import: 'default', eager: true }) as Record<
  string,
  string
>;

export interface Sample {
  name: string;
  cities: City[];
}

export const SAMPLES: readonly Sample[] = Object.entries(files)
  .map(([path, text]) => ({ name: path.split('/').pop()!, cities: parseTsp(text) }))
  .sort((a, b) => a.name.localeCompare(b.name, undefined, { numeric: true }));
