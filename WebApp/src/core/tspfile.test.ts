import { describe, expect, it } from 'vitest';
import { formatTsp, parseTsp, TspParseError } from './tspfile';
import { listTspFiles, readDataFile } from '../test/data';

describe('parseTsp', () => {
  it('reads a count followed by x y lines', () => {
    expect(parseTsp('3\n0.1 0.2\n0.3 0.4\n0.5 0.6\n')).toEqual([
      { x: 0.1, y: 0.2 },
      { x: 0.3, y: 0.4 },
      { x: 0.5, y: 0.6 },
    ]);
  });

  it('copes with the quirks found in the 1997 files', () => {
    const text = '4\r\n' + '.0981 .5942\r\n' + '0.5\t2.5e-01\r\n' + '\r\n' + '1 0\xff\r\n' + '-0 +1';
    expect(parseTsp(text)).toEqual([
      { x: 0.0981, y: 0.5942 },
      { x: 0.5, y: 0.25 },
      { x: 1, y: 0 },
      { x: -0, y: 1 },
    ]);
  });

  it('ignores anything after the first two numbers on a line', () => {
    expect(parseTsp('2 cities\n0.1 0.2 99 extra\n0.3 0.4\n')).toEqual([
      { x: 0.1, y: 0.2 },
      { x: 0.3, y: 0.4 },
    ]);
  });

  it('reads tokens atof cannot convert as 0, like the original', () => {
    expect(parseTsp('1\n- 0.5\n')).toEqual([{ x: 0, y: 0.5 }]);
  });

  it('ignores lines after the stated count', () => {
    expect(parseTsp('1\n0.1 0.2\n0.3 0.4\n')).toHaveLength(1);
  });

  it('rejects empty files, bad counts and short files', () => {
    expect(() => parseTsp('')).toThrow(TspParseError);
    expect(() => parseTsp('0\n')).toThrow(/at least 1/);
    expect(() => parseTsp('3\n0.1 0.2\n0.3')).toThrow(/after 1 of 3/);
  });
});

describe('formatTsp', () => {
  it('writes the count and %8.7f pairs like WriteLocates', () => {
    expect(formatTsp([{ x: 0.1, y: 0.25 }, { x: 1, y: 0 }])).toBe(
      '2\n0.1000000 0.2500000\n1.0000000 0.0000000\n',
    );
  });

  it('round-trips through parseTsp', () => {
    const cities = parseTsp(readDataFile('30CITY_0.TSP'));
    expect(parseTsp(formatTsp(cities))).toEqual(cities);
  });
});

describe('the bundled 1997 data files', () => {
  const files = listTspFiles();

  it('includes the sample problems', () => {
    expect(files).toContain('30CITY_0.TSP');
    expect(files.length).toBeGreaterThanOrEqual(18);
  });

  it.each(files)('%s parses to the stated number of cities in [0, 1]', (name) => {
    const text = readDataFile(name);
    const stated = parseInt(text, 10);
    const cities = parseTsp(text);
    expect(cities).toHaveLength(stated);
    for (const c of cities) {
      expect(c.x).toBeGreaterThanOrEqual(0);
      expect(c.x).toBeLessThanOrEqual(1);
      expect(c.y).toBeGreaterThanOrEqual(0);
      expect(c.y).toBeLessThanOrEqual(1);
    }
  });
});
