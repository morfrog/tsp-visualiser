// Placeholder page until the real UI arrives (see PLAN.md, phases 3-4): draws
// the 30CITY_0 sample and its file-order tour, using the ported core modules.

import { DistanceMatrix } from './core/distances';
import type { City } from './core/geometry';
import { Tour } from './core/tour';
import { parseTsp } from './core/tspfile';

const SAMPLE_30CITY_0 = `30
0.7049470 0.5640130
0.6338390 0.4365670
0.6931670 0.3657340
0.8526570 0.4011050
0.9629810 0.3749810
0.8250370 0.2685020
0.7925960 0.1481370
0.7456280 0.1724600
0.6402480 0.0756249
0.6406450 0.1528370
0.5788140 0.2638020
0.5626700 0.3167820
0.4833220 0.2215340
0.3510540 0.2749110
0.3174230 0.1712700
0.1471600 0.0879849
0.2200690 0.3299050
0.0855434 0.4113590
0.0259407 0.4917140
0.1769460 0.5529340
0.1561630 0.6755270
0.0964385 0.9548940
0.3073520 0.9367050
0.3453170 0.6697290
0.4972990 0.4665060
0.5213480 0.6082340
0.5988650 0.7628710
0.7449870 0.8972440
0.8067570 0.8662980
0.8969390 0.5849180`;

function draw(canvas: HTMLCanvasElement, cities: readonly City[], tour: Tour): void {
  const ctx = canvas.getContext('2d');
  if (!ctx) throw new Error('Canvas 2D not supported');

  const margin = 24;
  const size = canvas.width - margin * 2;
  const px = (c: City) => margin + c.x * size;
  const py = (c: City) => margin + (1 - c.y) * size; // y up

  ctx.clearRect(0, 0, canvas.width, canvas.height);

  ctx.strokeStyle = '#ff5555';
  ctx.lineWidth = 2;
  ctx.beginPath();
  tour.toArray().forEach((i, n) => {
    const c = cities[i];
    if (n === 0) ctx.moveTo(px(c), py(c));
    else ctx.lineTo(px(c), py(c));
  });
  ctx.closePath();
  ctx.stroke();

  ctx.fillStyle = '#ffffff';
  for (const c of cities) {
    ctx.beginPath();
    ctx.arc(px(c), py(c), 5, 0, Math.PI * 2);
    ctx.fill();
  }
}

const viewCanvas = document.querySelector<HTMLCanvasElement>('#view')!;
const statusLine = document.querySelector<HTMLParagraphElement>('#status')!;
const sampleCities = parseTsp(SAMPLE_30CITY_0);
const sampleTour = Tour.identity(sampleCities.length);
draw(viewCanvas, sampleCities, sampleTour);
const tourLength = sampleTour.length(new DistanceMatrix(sampleCities));
statusLine.textContent = `30CITY_0.TSP: ${sampleCities.length} cities, optimal tour length ${tourLength.toFixed(6)}`;
