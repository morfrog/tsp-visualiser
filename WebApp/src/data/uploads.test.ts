import { describe, expect, it } from 'vitest';
import { readDataFile } from '../test/data';
import { MAX_CITIES, MAX_FILE_BYTES, readTsp } from './uploads';

const bytes = (text: string) => new TextEncoder().encode(text);

describe('readTsp', () => {
  it('reads a .TSP file, quirks and all', () => {
    const result = readTsp('X.TSP', bytes(readDataFile('256BENCH.TSP')));
    expect(result.ok && result.cities).toHaveLength(256);
  });

  it('reads files with other names, like the shuffled 30CITIES.DI0', () => {
    const result = readTsp('30CITIES.DI0', bytes(readDataFile('30CITIES.DI0')));
    expect(result).toMatchObject({ ok: true, name: '30CITIES.DI0' });
  });

  it('explains what is wrong with a bad file', () => {
    expect(readTsp('E.TSP', bytes(''))).toMatchObject({ ok: false, error: expect.stringMatching(/empty/) });
    expect(readTsp('S.TSP', bytes('3\n0.1 0.2\n'))).toMatchObject({ ok: false, error: expect.stringMatching(/ends/) });
    expect(readTsp('O.TSP', bytes('1\n2.5 0.5\n'))).toMatchObject({ ok: false, error: 'Coordinates must be 0 to 1.' });
  });

  it('refuses very large files and maps', () => {
    expect(readTsp('B.TSP', new Uint8Array(MAX_FILE_BYTES + 1))).toMatchObject({ ok: false, error: /1 MB/ });
    const many = [String(MAX_CITIES + 1), ...Array.from({ length: MAX_CITIES + 1 }, () => '0.5 0.5')].join('\n');
    expect(readTsp('M.TSP', bytes(many))).toMatchObject({ ok: false, error: `Over ${MAX_CITIES} cities.` });
  });
});
