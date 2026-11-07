// A text window as the original used them: a fixed grid of character cells,
// each with a colour. Text is written at a 1-based row and column and simply
// overwrites cells (Legacy DrawText: _settextposition + _outtext, with
// wrapping on). "\n" moves to column 1 of the next row. Nothing is cleared,
// which is why the original wrote trailing spaces to blank out old text.

import { EGA_PALETTE, Ega, type EgaColor } from '../core/colors';

export interface Cell {
  ch: string;
  color: EgaColor;
}

export class TextGrid {
  private cells: Cell[][];

  constructor(
    readonly rows: number,
    readonly cols: number,
  ) {
    this.cells = TextGrid.blank(rows, cols);
  }

  private static blank(rows: number, cols: number): Cell[][] {
    return Array.from({ length: rows }, () => Array.from({ length: cols }, () => ({ ch: ' ', color: Ega.BLACK })));
  }

  clear(): void {
    this.cells = TextGrid.blank(this.rows, this.cols);
  }

  write(text: string, row: number, col: number, color: EgaColor): void {
    let r = row;
    let c = col;
    for (const ch of text) {
      if (ch === '\n') {
        r++;
        c = 1;
        continue;
      }
      if (c > this.cols) {
        r++;
        c = 1;
      }
      if (r >= 1 && r <= this.rows && c >= 1) this.cells[r - 1][c - 1] = { ch, color };
      c++;
    }
  }

  /** Row text, for tests and screen readers. */
  rowText(row: number): string {
    return this.cells[row - 1].map((cell) => cell.ch).join('');
  }

  /** Runs of same-coloured characters per row, for drawing. */
  runs(): { text: string; color: EgaColor }[][] {
    return this.cells.map((row) => {
      const runs: { text: string; color: EgaColor }[] = [];
      for (const cell of row) {
        const last = runs[runs.length - 1];
        // Spaces join any run: their colour is invisible.
        if (last && (last.color === cell.color || cell.ch === ' ')) last.text += cell.ch;
        else runs.push({ text: cell.ch, color: cell.color });
      }
      return runs;
    });
  }
}

/** Draws a grid into a <pre> element, one line per row. */
export function renderTextGrid(grid: TextGrid, pre: HTMLElement): void {
  const fragment = document.createDocumentFragment();
  grid.runs().forEach((row, i) => {
    if (i > 0) fragment.append('\n');
    for (const run of row) {
      const span = document.createElement('span');
      span.textContent = run.text;
      span.style.color = EGA_PALETTE[run.color];
      fragment.append(span);
    }
  });
  pre.replaceChildren(fragment);
}
