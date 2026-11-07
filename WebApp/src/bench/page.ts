// The benchmark page: reruns DOCO.DOC's tables and sets each result beside
// the 1997 one.

import { clockSeed } from '../core/random';
import type { BenchMessage } from './bench.worker';
import { cellCount, matches, TABLES, type CellResult, type TableSpec } from './runner';

const root = document.querySelector<HTMLElement>('#bench')!;

function el<K extends keyof HTMLElementTagNameMap>(tag: K, className?: string, text?: string): HTMLElementTagNameMap[K] {
  const e = document.createElement(tag);
  if (className) e.className = className;
  if (text !== undefined) e.textContent = text;
  return e;
}

const fmt = (v: number) => v.toFixed(5);
const mean = (values: number[]) => values.reduce((a, b) => a + b, 0) / values.length;

interface TableView {
  spec: TableSpec;
  cells: HTMLTableCellElement[][];
  averages: HTMLTableCellElement[];
  values: (number | undefined)[][];
}

function buildTable(spec: TableSpec): { section: HTMLElement; view: TableView } {
  const section = el('section', 'bench__table');
  section.setAttribute('aria-labelledby', `t-${spec.id}`);
  const heading = el('h2', undefined, spec.title);
  heading.id = `t-${spec.id}`;
  section.append(heading, el('p', 'bench__note', `DOCO ${spec.section}. ${spec.note}`));

  const scroller = el('div', 'bench__scroll');
  scroller.tabIndex = 0;
  scroller.setAttribute('role', 'region');
  scroller.setAttribute('aria-label', `${spec.title} results`);
  const table = el('table');
  const head = table.createTHead().insertRow();
  head.append(el('th', undefined, 'File'));
  for (const c of spec.columns) {
    const th = el('th', undefined, c.name);
    if (!c.deterministic) th.append(el('span', 'bench__varies', ' (varies)'));
    head.append(th);
  }
  const body = table.createTBody();
  const cells: HTMLTableCellElement[][] = [];
  spec.doco.forEach((row, r) => {
    const tr = body.insertRow();
    const th = el('th', undefined, `30city_${r}`);
    th.scope = 'row';
    tr.append(th);
    cells.push(
      row.map((doco) => {
        const td = tr.insertCell();
        td.append(el('span', 'bench__ours', '…'), el('span', 'bench__doco', `1997: ${fmt(doco)}`));
        return td;
      }),
    );
  });
  const foot = table.createTFoot().insertRow();
  const avg = el('th', undefined, 'Average');
  avg.scope = 'row';
  foot.append(avg);
  const averages = spec.columns.map((_, c) => {
    const td = foot.insertCell();
    td.append(el('span', 'bench__ours', '…'), el('span', 'bench__doco', `1997: ${fmt(mean(spec.doco.map((row) => row[c])))}`));
    return td;
  });
  scroller.append(table);
  section.append(scroller);
  return { section, view: { spec, cells, averages, values: spec.doco.map((row) => row.map(() => undefined)) } };
}

function render(): void {
  root.replaceChildren();
  root.append(el('h1', undefined, 'VTTSP Benchmarks'));
  root.append(
    el(
      'p',
      'bench__intro',
      "The tables from the 1997 algorithmic report (DOCO.DOC), rerun now in your browser with the ported methods. Each result is the method's tour length divided by the optimal length, so 1.02 means 2% longer than the best possible tour. Methods that give the same answer every time should match 1997 exactly; the ones that use random numbers vary, so compare their averages.",
    ),
  );

  const controls = el('form', 'bench__controls');
  const label = el('label', undefined, 'Random seed ');
  const seed = el('input');
  seed.type = 'number';
  seed.min = '0';
  seed.step = '1';
  seed.value = String(clockSeed());
  label.append(seed);
  const run = el('button', 'btn btn--go', 'Run benchmarks');
  run.type = 'submit';
  controls.append(label, run);
  const status = el('p', 'bench__status');
  status.setAttribute('role', 'status');
  root.append(controls, status);

  const views = new Map<string, TableView>();
  for (const spec of TABLES) {
    const { section, view } = buildTable(spec);
    views.set(spec.id, view);
    root.append(section);
  }

  let worker: Worker | null = null;
  controls.addEventListener('submit', (event) => {
    event.preventDefault();
    worker?.terminate();
    for (const v of views.values()) {
      v.values = v.spec.doco.map((row) => row.map(() => undefined));
      v.cells.flat().forEach((td) => {
        td.className = '';
        td.querySelector('.bench__ours')!.textContent = '…';
      });
      v.averages.forEach((td) => (td.querySelector('.bench__ours')!.textContent = '…'));
    }
    const total = cellCount();
    let done = 0;
    let checked = 0;
    let matched = 0;
    run.disabled = true;
    status.textContent = `Working… 0 of ${total} results.`;
    const started = performance.now();

    worker = new Worker(new URL('./bench.worker.ts', import.meta.url), { type: 'module' });
    worker.onmessage = (e: MessageEvent<BenchMessage>) => {
      const m = e.data;
      if (m.type === 'cell') {
        const result = show(views, m.cell);
        done++;
        if (result !== null) {
          checked++;
          if (result) matched++;
        }
        status.textContent = `Working… ${done} of ${total} results.`;
      } else if (m.type === 'done') {
        run.disabled = false;
        const seconds = ((performance.now() - started) / 1000).toFixed(1);
        status.textContent = `Done in ${seconds} s. ${matched} of ${checked} same-every-time results match 1997 exactly.`;
        status.classList.toggle('is-bad', matched !== checked);
        worker?.terminate();
        worker = null;
      } else {
        run.disabled = false;
        status.textContent = `Stopped: ${m.message}`;
        status.classList.add('is-bad');
      }
    };
    worker.postMessage({ seed: Math.trunc(Number(seed.value)) || 0 });
  });

  run.click();
}

/** Fills in a cell; returns whether it matched 1997, or null if it isn't checked. */
function show(views: Map<string, TableView>, cell: CellResult): boolean | null {
  const view = views.get(cell.table)!;
  const doco = view.spec.doco[cell.row][cell.column];
  const deterministic = view.spec.columns[cell.column].deterministic;
  const td = view.cells[cell.row][cell.column];
  view.values[cell.row][cell.column] = cell.value;
  const ours = td.querySelector('.bench__ours')!;
  ours.textContent = fmt(cell.value);
  let result: boolean | null = null;
  if (deterministic) {
    result = matches(cell.value, doco);
    td.className = result ? 'is-match' : 'is-mismatch';
    ours.textContent += result ? ' ✓' : ' ✗';
    td.title = result ? 'Matches 1997' : 'Differs from 1997';
  }
  const column = view.values.map((row) => row[cell.column]);
  if (column.every((v) => v !== undefined)) {
    view.averages[cell.column].querySelector('.bench__ours')!.textContent = fmt(mean(column as number[]));
  }
  return result;
}

render();
