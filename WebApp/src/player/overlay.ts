// The annealing end screen's overlay (Legacy ControlLCLOverlay): the 1-opt
// tour (yellow) and 2-opt tour (red) start in the left and right halves of a
// 639 x 240 window, slide towards each other until they sit exactly on top of
// one another, hold for a second, slide apart to the edges, and repeat.
// Digits set the speed (pixels per move), '0' pauses, Esc ends.

import type { EgaColor } from '../core/colors';
import type { City } from '../core/geometry';
import { clear, drawOverlayTour, fitCanvas, type Viewport } from '../render/mapView';

export const OVERLAY_WIDTH = 639;
export const OVERLAY_HEIGHT = 240;
const CENTRE_HOLD_MS = 1000;

/** The two half-windows' positions, in original EGA pixels (Legacy SWIN l/r). */
export interface OverlayState {
  left: number;
  right: number;
  /** Pixels each window moves per move; sign gives direction. */
  ld: number;
  rd: number;
  factor: number;
}

export type OverlayEvent = 'moving' | 'centre' | 'edge';

export function initialOverlay(): OverlayState {
  // NewWin(1, ..., width/2) and NewWin(width/2+1, ...), then one MoveWin each.
  return { left: 1 + 1, right: Math.floor(OVERLAY_WIDTH / 2) + 1 - 1, ld: 1, rd: -1, factor: 1 };
}

/** One move, then the original's checks for reaching the centre or the edges. */
export function stepOverlay(s: OverlayState): OverlayEvent {
  s.left += s.ld;
  s.right += s.rd;
  const f = s.factor;
  let event: OverlayEvent = 'moving';
  if (Math.abs(s.right - s.left) < f) event = 'centre';
  else if (s.left + f >= s.right) event = 'centre';
  else if (s.left <= f + 1) event = 'edge';
  else if (s.rd < 0 && s.left <= f * 2) event = 'edge';
  if (event !== 'moving') {
    // Turn round (Legacy: ld = rd; rd *= -1).
    s.ld = s.rd;
    s.rd = -s.rd;
  }
  return event;
}

/** Digit d: move d pixels at a time, keeping direction. */
export function setOverlaySpeed(s: OverlayState, digit: number): void {
  s.factor = digit;
  s.rd = (s.rd < 0 ? -1 : 1) * digit;
  s.ld = -s.rd;
}

export interface OverlayTour {
  tour: readonly number[];
  color: EgaColor;
}

/**
 * Runs the overlay on a canvas until `signal` aborts. Keys arrive through
 * `onKey` registration so the caller owns event handling.
 */
export function runOverlay(
  canvas: HTMLCanvasElement,
  cities: readonly City[],
  one: OverlayTour,
  two: OverlayTour,
  signal: AbortSignal,
  subscribe: (handler: (key: string) => void) => () => void,
): Promise<void> {
  const ctx = canvas.getContext('2d')!;
  const s = initialOverlay();
  // Legacy delay(270 / n) between moves, in ms.
  const moveMs = Math.max(2, Math.floor(270 / cities.length));
  let paused = false;
  let holdUntil = 0;
  let last = performance.now();
  let debt = 0;

  const draw = (atCentre: boolean) => {
    const dpr = fitCanvas(canvas);
    const sx = canvas.width / OVERLAY_WIDTH;
    clear(ctx);
    // Each half-window is 318 pixels wide inside its border (Legacy SetWin).
    const half = Math.floor(OVERLAY_WIDTH / 2) - 1;
    const vp = (x: number): Viewport => ({ left: x * sx, top: 0, width: half * sx, height: canvas.height });
    const width = 1.5 * dpr;
    if (atCentre) {
      // Both drawn in the left window: exactly overlaid.
      drawOverlayTour(ctx, vp(s.left), cities, one.tour, one.color, false, width);
      drawOverlayTour(ctx, vp(s.left), cities, two.tour, two.color, true, width);
    } else {
      drawOverlayTour(ctx, vp(s.left), cities, one.tour, one.color, true, width);
      drawOverlayTour(ctx, vp(s.right), cities, two.tour, two.color, true, width);
    }
  };

  return new Promise((resolve) => {
    const unsubscribe = subscribe((key) => {
      if (key === '0') paused = !paused;
      else if (/^[1-9]$/.test(key)) {
        paused = false;
        setOverlaySpeed(s, Number(key));
      } else if (paused) paused = false;
    });
    const finish = () => {
      unsubscribe();
      resolve();
    };
    signal.addEventListener('abort', finish, { once: true });

    const tick = (now: number) => {
      if (signal.aborted) return;
      const dt = now - last;
      last = now;
      if (!paused && now >= holdUntil) {
        debt += dt;
        let moves = Math.min(40, Math.floor(debt / moveMs));
        debt -= moves * moveMs;
        let centre = false;
        while (moves-- > 0) {
          const event = stepOverlay(s);
          if (event === 'centre') {
            centre = true;
            holdUntil = now + CENTRE_HOLD_MS;
            debt = 0;
            break;
          }
        }
        draw(centre);
      } else if (now < holdUntil) {
        draw(true);
      } else {
        draw(false);
      }
      requestAnimationFrame(tick);
    };
    draw(false);
    requestAnimationFrame(tick);
  });
}
