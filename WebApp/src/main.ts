// Phase 0 placeholder: proves the build + Pages deploy pipeline by drawing a
// sample map (Data/30CITY_0.TSP) and its city-order tour on the canvas.
// The real UI and algorithms arrive in later phases (see PLAN.md).

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

interface City {
  x: number;
  y: number;
}

function parseTsp(text: string): City[] {
  const [countLine, ...lines] = text.trim().split(/\r?\n/);
  const count = parseInt(countLine, 10);
  return lines.slice(0, count).map((line) => {
    const [x, y] = line.trim().split(/\s+/).map(Number);
    return { x, y };
  });
}

function draw(canvas: HTMLCanvasElement, cities: City[]): number {
  const ctx = canvas.getContext('2d');
  if (!ctx) throw new Error('Canvas 2D not supported');

  const margin = 24;
  const size = canvas.width - margin * 2;
  const px = (c: City) => margin + c.x * size;
  const py = (c: City) => margin + (1 - c.y) * size; // y up, as on the original screen

  ctx.clearRect(0, 0, canvas.width, canvas.height);

  ctx.strokeStyle = '#ff5555';
  ctx.lineWidth = 2;
  ctx.beginPath();
  cities.forEach((c, i) => (i === 0 ? ctx.moveTo(px(c), py(c)) : ctx.lineTo(px(c), py(c))));
  ctx.closePath();
  ctx.stroke();

  ctx.fillStyle = '#ffffff';
  for (const c of cities) {
    ctx.beginPath();
    ctx.arc(px(c), py(c), 5, 0, Math.PI * 2);
    ctx.fill();
  }

  let length = 0;
  for (let i = 0; i < cities.length; i++) {
    const a = cities[i];
    const b = cities[(i + 1) % cities.length];
    length += Math.hypot(a.x - b.x, a.y - b.y);
  }
  return length;
}

const viewCanvas = document.querySelector<HTMLCanvasElement>('#view')!;
const statusLine = document.querySelector<HTMLParagraphElement>('#status')!;
const sampleCities = parseTsp(SAMPLE_30CITY_0);
const tourLength = draw(viewCanvas, sampleCities);
statusLine.textContent = `30CITY_0.TSP: ${sampleCities.length} cities, file-order tour length ${tourLength.toFixed(6)}`;

export {};
