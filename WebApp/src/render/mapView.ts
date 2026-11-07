// Draws a map window the way Legacy/SCREEN.C and PROBLEM.C did:
//   - the unit square is shown with a 0.05 margin on every side, y pointing
//     down (_setwindow(0, -0.05, -0.05, 1.05, 1.05), invert off);
//   - tour edges are 1-pixel lines, LTRED, or segment-coloured in 'mask' mode
//     (DrawPath / DrawMaskPath);
//   - extra lines are drawn over the edges; black ones erase an edge;
//   - cities are filled ellipses of radius 0.023 (double when highlighted)
//     with their number in black (DrawCity), drawn last.
// The original window was a few hundred EGA pixels; here the canvas matches
// its element's size and the device pixel ratio, so lines stay crisp.

import { EGA_PALETTE, Ega, Role, type EgaColor } from '../core/colors';
import type { City } from '../core/geometry';
import type { ViewSnapshot } from '../run/stepper';

export const CITY_SIZE = 0.023;
const WINDOW_MIN = -0.05;
const WINDOW_SPAN = 1.1;

export interface Viewport {
  /** Left and top of the window in canvas pixels. */
  left: number;
  top: number;
  width: number;
  height: number;
}

/** Unit-square coordinates to canvas pixels (Legacy _setwindow mapping, y down). */
export function toCanvas(vp: Viewport, x: number, y: number): [number, number] {
  return [vp.left + ((x - WINDOW_MIN) / WINDOW_SPAN) * vp.width, vp.top + ((y - WINDOW_MIN) / WINDOW_SPAN) * vp.height];
}

/** Colour of each city: its mark if it has one, otherwise its mask colour. */
export function cityColors(view: ViewSnapshot): EgaColor[] {
  const colors = [...view.baseColors];
  for (const m of view.marks) colors[m.city] = m.color;
  return colors;
}

/** Colour of each tour edge, from the head; the closing edge last (if closed). */
export function edgeColors(view: ViewSnapshot): EgaColor[] {
  const colors = cityColors(view);
  const n = view.tour.length;
  const count = view.closed ? n : n - 1;
  const result: EgaColor[] = [];
  for (let i = 0; i < count; i++) {
    const a = view.tour[i];
    const b = view.tour[(i + 1) % n];
    result.push(view.edges === 'mask' && colors[a] === colors[b] ? colors[a] : Role.line);
  }
  return result;
}

/** Sets a canvas's backing size to its displayed size; returns the scale used. */
export function fitCanvas(canvas: HTMLCanvasElement): number {
  const dpr = window.devicePixelRatio || 1;
  const width = Math.max(1, Math.round(canvas.clientWidth * dpr));
  const height = Math.max(1, Math.round(canvas.clientHeight * dpr));
  if (canvas.width !== width || canvas.height !== height) {
    canvas.width = width;
    canvas.height = height;
  }
  return dpr;
}

export function clear(ctx: CanvasRenderingContext2D, vp?: Viewport): void {
  ctx.fillStyle = EGA_PALETTE[Ega.BLACK];
  if (vp) ctx.fillRect(vp.left, vp.top, vp.width, vp.height);
  else ctx.fillRect(0, 0, ctx.canvas.width, ctx.canvas.height);
}

function line(ctx: CanvasRenderingContext2D, vp: Viewport, a: City, b: City, color: EgaColor, width: number): void {
  const [x1, y1] = toCanvas(vp, a.x, a.y);
  const [x2, y2] = toCanvas(vp, b.x, b.y);
  ctx.strokeStyle = EGA_PALETTE[color];
  ctx.lineWidth = color === Ega.BLACK ? width * 2.5 : width; // a black line must cover the edge
  ctx.beginPath();
  ctx.moveTo(x1, y1);
  ctx.lineTo(x2, y2);
  ctx.stroke();
}

function city(
  ctx: CanvasRenderingContext2D,
  vp: Viewport,
  c: City,
  label: string,
  color: EgaColor,
  size: number,
  showLabel: boolean,
): void {
  const [x, y] = toCanvas(vp, c.x, c.y);
  const rx = (size / WINDOW_SPAN) * vp.width;
  const ry = (size / WINDOW_SPAN) * vp.height;
  ctx.fillStyle = EGA_PALETTE[color];
  ctx.beginPath();
  ctx.ellipse(x, y, rx, ry, 0, 0, Math.PI * 2);
  ctx.fill();
  if (!showLabel) return;
  // Legacy drew the number in black, at the city's size, centred.
  const fontPx = Math.min(rx, ry) * (label.length > 2 ? 1.05 : 1.35);
  if (fontPx < 5) return; // too small to read
  ctx.fillStyle = EGA_PALETTE[Role.text];
  ctx.font = `bold ${fontPx}px system-ui, sans-serif`;
  ctx.textAlign = 'center';
  ctx.textBaseline = 'middle';
  ctx.fillText(label, x, y + fontPx * 0.05);
}

/** Above this many cities the numbers overlap too much to read, so they are left off. */
export const MAX_LABELLED_CITIES = 100;

export interface DrawOptions {
  /** Line width in canvas pixels. */
  lineWidth?: number;
  /**
   * Draw city numbers. Default: when there are at most MAX_LABELLED_CITIES
   * (and they are big enough to read).
   */
  labels?: boolean;
  /** Clear the viewport first. */
  clear?: boolean;
}

/** Draws one frame view: edges, extra lines, then cities (Legacy Hilite* / DrawNorm). */
export function drawView(
  ctx: CanvasRenderingContext2D,
  vp: Viewport,
  cities: readonly City[],
  view: ViewSnapshot,
  options: DrawOptions = {},
): void {
  const width = options.lineWidth ?? 1.5;
  if (options.clear ?? true) clear(ctx, vp);
  ctx.lineCap = 'round';

  const edges = edgeColors(view);
  edges.forEach((color, i) => {
    const a = view.tour[i];
    const b = view.tour[(i + 1) % view.tour.length];
    line(ctx, vp, cities[a], cities[b], color, width);
  });
  for (const l of view.lines) line(ctx, vp, cities[l.from], cities[l.to], l.color, width);

  const colors = cityColors(view);
  const large = new Set(view.marks.filter((m) => m.large).map((m) => m.city));
  const labels = options.labels ?? cities.length <= MAX_LABELLED_CITIES;
  // Every city is drawn, in or out of the tour (Legacy DrawCities).
  for (let i = 0; i < cities.length; i++) {
    city(ctx, vp, cities[i], String(i), colors[i], large.has(i) ? CITY_SIZE * 2 : CITY_SIZE, labels);
  }
}

/** A tour in one colour, as the annealing overlay draws it (Legacy DrawOverlay). */
export function drawOverlayTour(
  ctx: CanvasRenderingContext2D,
  vp: Viewport,
  cities: readonly City[],
  tour: readonly number[],
  color: EgaColor,
  withCities: boolean,
  lineWidth = 1.5,
): void {
  for (let i = 0; i < tour.length; i++) {
    line(ctx, vp, cities[tour[i]], cities[tour[(i + 1) % tour.length]], color, lineWidth);
  }
  if (withCities) {
    for (let i = 0; i < cities.length; i++) city(ctx, vp, cities[i], String(i), Role.cityIn, CITY_SIZE, false);
  }
}
