// printf-style helpers so the side-panel text matches the original sprintf()s.

import type { EgaColor } from '../core/colors';
import type { TextArea, TextUpdate } from './frame';

/** %Nd */
export function d(value: number, width = 0): string {
  return String(Math.trunc(value)).padStart(width);
}

/** %W.Pf */
export function f(value: number, width: number, precision: number): string {
  return value.toFixed(precision).padStart(width);
}

export function say(
  text: string,
  row: number,
  color: EgaColor,
  col = 1,
  area: TextArea = 'info',
): TextUpdate {
  return { area, row, col, text, color };
}
