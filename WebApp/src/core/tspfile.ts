// Port of the .TSP reader and writer from Legacy/FILE.C (NewLocates, ReadDouble,
// SkipLine, WriteLocates).
//
// Format: the first number is the city count, then each city is one line
// holding "x y". The legacy reader scans characters rather than splitting on
// whitespace, which is why it copes with the quirks found in Data/: CRLF
// endings, tabs, exponents ("2.5e-01"), missing leading zeros (".0981"),
// stray bytes and blank lines. This reader follows the same rules:
//   - readNumber skips any characters that cannot start a number, then reads
//     the run of number characters [0-9 . + - E e].
//   - after the count and after each city's y value, the rest of the line is
//     skipped, so anything after the first two numbers on a line is ignored.
//
// The .TSP order of the cities is also a tour: Save writes the cities in tour
// order (Legacy PathAsLocates + WriteLocates), and loading starts with the tour
// 0, 1, 2, ... n-1.

import type { City } from './geometry';

export class TspParseError extends Error {
  constructor(message: string) {
    super(message);
    this.name = 'TspParseError';
  }
}

function isNumberChar(c: string): boolean {
  return (c >= '0' && c <= '9') || c === '.' || c === '-' || c === '+' || c === 'E' || c === 'e';
}

class Reader {
  private pos = 0;

  constructor(private readonly text: string) {}

  /** Legacy ReadDouble. Returns null at end of input instead of looping forever. */
  readNumber(): number | null {
    const t = this.text;
    while (this.pos < t.length && !isNumberChar(t[this.pos])) this.pos++;
    if (this.pos >= t.length) return null;
    const start = this.pos;
    while (this.pos < t.length && isNumberChar(t[this.pos])) this.pos++;
    const token = t.slice(start, this.pos);
    // ReadDouble consumes the terminating character unless it is a newline.
    if (this.pos < t.length && t[this.pos] !== '\n') this.pos++;
    // atof() yields 0 for tokens it cannot convert, such as "-" or ".".
    const value = parseFloat(token);
    return Number.isNaN(value) ? 0 : value;
  }

  /** Legacy SkipLine: move past the next newline. */
  skipLine(): void {
    const nl = this.text.indexOf('\n', this.pos);
    this.pos = nl < 0 ? this.text.length : nl + 1;
  }
}

/** Parses the contents of a .TSP file into cities, in file (tour) order. */
export function parseTsp(text: string): City[] {
  const reader = new Reader(text);
  const countValue = reader.readNumber();
  if (countValue === null) throw new TspParseError('File is empty: expected a city count.');
  const count = Math.trunc(countValue);
  if (count < 1) throw new TspParseError(`City count must be at least 1, got ${countValue}.`);
  reader.skipLine();

  const cities: City[] = [];
  for (let i = 0; i < count; i++) {
    const x = reader.readNumber();
    const y = reader.readNumber();
    if (x === null || y === null) {
      throw new TspParseError(`File ends after ${i} of ${count} cities.`);
    }
    cities.push({ x, y });
    reader.skipLine();
  }
  return cities;
}

/** Formats cities as .TSP text, matching WriteLocates' "%d" and "%8.7f %8.7f". */
export function formatTsp(cities: readonly City[]): string {
  const lines = [String(cities.length)];
  for (const c of cities) lines.push(`${c.x.toFixed(7)} ${c.y.toFixed(7)}`);
  return lines.join('\n') + '\n';
}

/**
 * Decodes raw file bytes. The 1997 files are 8-bit DOS text, so decode as
 * Latin-1: every byte maps to one character and stray bytes cannot fail.
 */
export function decodeTspBytes(bytes: ArrayBuffer | Uint8Array): string {
  return new TextDecoder('latin1').decode(bytes);
}
