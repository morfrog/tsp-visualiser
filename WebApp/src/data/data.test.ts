import { describe, expect, it } from 'vitest';
import { INSERTION_METHODS, ITERATION_METHODS, SOLVERS } from '../algorithms/catalog';
import { help, HELP_FILES, parseHelp } from './help';
import { SAMPLES } from './samples';

describe('bundled help files', () => {
  it('includes all 22 help files from Legacy/HLP', () => {
    expect(HELP_FILES).toHaveLength(22);
    for (const c of [...SOLVERS, ...INSERTION_METHODS, ...ITERATION_METHODS]) {
      expect(help(c.help).title).not.toBe(c.help);
    }
  });

  it('reads a title and up to nine lines, as ReadChoiceHelp did', () => {
    expect(help('MCHEAP.HLP')).toEqual({
      title: 'Cheapest Method',
      lines: [
        'This method chooses as its next',
        'insert the point which if',
        'inserted will make the increase',
        'to path length minimal. The point',
        'is inserted in the place which',
        'minimises the increase to path',
        'length. It stops when all points',
        'have been inserted.',
      ],
    });
    const long = parseHelp(['T', ...Array.from({ length: 12 }, (_, i) => `line ${i}`)].join('\r\n'));
    expect(long.lines).toHaveLength(9);
  });

  it('stands in for a missing file the way the original did', () => {
    expect(help('NOPE.HLP')).toEqual({
      title: 'NOPE.HLP',
      lines: ['Is An Invalid File', 'Ensure it is in the HLP', 'subdirectory'],
    });
  });
});

describe('bundled samples', () => {
  it('includes every .TSP file in Data/, sorted', () => {
    expect(SAMPLES.map((s) => s.name)).toContain('30CITY_0.TSP');
    expect(SAMPLES).toHaveLength(18);
    expect(SAMPLES.find((s) => s.name === '256BENCH.TSP')?.cities).toHaveLength(256);
  });
});
