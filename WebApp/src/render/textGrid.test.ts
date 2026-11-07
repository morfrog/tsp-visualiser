import { describe, expect, it } from 'vitest';
import { Ega } from '../core/colors';
import { TextGrid } from './textGrid';

describe('TextGrid', () => {
  it('writes at a 1-based row and column, overwriting without clearing', () => {
    const g = new TextGrid(3, 10);
    g.write('Insert is  12.', 1, 1, Ega.WHITE);
    expect(g.rowText(1)).toBe('Insert is ');
    expect(g.rowText(2)).toBe(' 12.      '); // wrapped past column 10
    g.write('XY', 1, 3, Ega.YELLOW);
    expect(g.rowText(1)).toBe('InXYrt is ');
  });

  it('moves to column 1 of the next row on a newline', () => {
    const g = new TextGrid(3, 8);
    g.write('ab\ncd\n\nlost', 1, 4, Ega.GREEN);
    expect(g.rowText(1)).toBe('   ab   ');
    expect(g.rowText(2)).toBe('cd      ');
    expect(g.rowText(3)).toBe('        ');
  });

  it('blanks text with spaces, as the original did', () => {
    const g = new TextGrid(1, 6);
    g.write('Hello', 1, 1, Ega.WHITE);
    g.write('   ', 1, 2, Ega.BLACK);
    expect(g.rowText(1)).toBe('H   o ');
  });

  it('groups characters into coloured runs, folding spaces into the run before', () => {
    const g = new TextGrid(1, 8);
    g.write('ab', 1, 1, Ega.RED);
    g.write('cd', 1, 4, Ega.BLUE);
    expect(g.runs()[0]).toEqual([
      { text: 'ab ', color: Ega.RED },
      { text: 'cd   ', color: Ega.BLUE },
    ]);
  });

  it('clears', () => {
    const g = new TextGrid(1, 3);
    g.write('abc', 1, 1, Ega.RED);
    g.clear();
    expect(g.rowText(1)).toBe('   ');
  });
});
