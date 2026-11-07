// The screen shown while a method runs (Legacy INTERFAC.C's Go! branch and
// ControlAnneal): map window(s), text panels, and the finish screens.
//
// Insertion and iteration: one map with the 23 x 20 text panel beside it
// (InsWinManager / OptWinManager). When finished, "F I N I S H E D" and the
// length, then Esc returns the new tour; Esc during the run abandons it.
//
// Annealing: a 3 x 78 top bar, then the 1-opt and 2-opt maps side by side,
// each with a 2 x 38 pane (AnnWinManager). When finished (or stopped), any
// key shows the overlay; Esc returns. Annealing never changed the current
// tour in the original, and doesn't here.

import { pausesPerStep } from '../algorithms/catalog';
import type { TextArea, TextUpdate } from '../algorithms/frame';
import { f, say } from '../algorithms/text';
import { Ega, Role, type EgaColor } from '../core/colors';
import { DistanceMatrix } from '../core/distances';
import { Tour } from '../core/tour';
import { drawView, fitCanvas } from '../render/mapView';
import { renderTextGrid, TextGrid } from '../render/textGrid';
import type { StartRequest } from '../run/protocol';
import { startSession } from '../run/session';
import type { FrameSnapshot, StepBatch, ViewSnapshot } from '../run/stepper';
import { runOverlay } from './overlay';
import { CommandQueue, commandForKey, playback, type Command, type Mode } from './playback';

export interface RunRequest extends StartRequest {
  /** Shown above the run, e.g. the file name. */
  title: string;
}

export interface RunScreenResult {
  /** The new current tour (insertion and iteration only, when finished). */
  tour: number[] | null;
  lengths: number[];
}

interface Layout {
  root: HTMLElement;
  maps: HTMLCanvasElement[];
  overlay: HTMLCanvasElement | null;
  texts: Partial<Record<TextArea, { grid: TextGrid; pre: HTMLElement; box: HTMLElement }>>;
  status: HTMLElement;
  controls: HTMLElement;
}

const INTRO = "Hit Esc to Quit,\nSpeeds '0','1'-'9'\nAny other key \nTo continue..";

function el<K extends keyof HTMLElementTagNameMap>(tag: K, className?: string, text?: string): HTMLElementTagNameMap[K] {
  const e = document.createElement(tag);
  if (className) e.className = className;
  if (text !== undefined) e.textContent = text;
  return e;
}

function textPanel(area: TextArea, rows: number, cols: number, label: string) {
  const pre = el('pre', `text text--${area}`);
  pre.setAttribute('aria-label', label);
  pre.style.setProperty('--cols', String(cols));
  pre.style.setProperty('--rows', String(rows));
  // The box is the size container the panel's font is fitted to.
  const box = el('div', 'textbox');
  box.append(pre);
  return { grid: new TextGrid(rows, cols), pre, box };
}

function buildLayout(container: HTMLElement, annealing: boolean, title: string): Layout {
  const root = el('section', `run ${annealing ? 'run--anneal' : 'run--single'}`);
  root.setAttribute('aria-label', `Running: ${title}`);
  const heading = el('h2', 'run__title', title);
  const status = el('p', 'run__status');
  status.setAttribute('aria-live', 'polite');
  const controls = el('div', 'run__controls');
  const texts: Layout['texts'] = {};
  const maps: HTMLCanvasElement[] = [];
  let overlay: HTMLCanvasElement | null = null;

  if (annealing) {
    texts.top = textPanel('top', 3, 78, 'Annealing status');
    texts.left = textPanel('left', 2, 38, '1-opt annealing');
    texts.right = textPanel('right', 2, 38, '2-opt annealing');
    const pair = el('div', 'run__pair');
    for (const [area, label] of [
      ['left', '1-opt annealing map'],
      ['right', '2-opt annealing map'],
    ] as const) {
      const figure = el('figure', 'run__pane');
      const canvas = el('canvas', 'map map--anneal');
      canvas.setAttribute('role', 'img');
      canvas.setAttribute('aria-label', label);
      maps.push(canvas);
      figure.append(canvas, texts[area]!.box);
      pair.append(figure);
    }
    overlay = el('canvas', 'map map--overlay');
    overlay.setAttribute('role', 'img');
    overlay.setAttribute('aria-label', 'Both tours sliding over each other');
    overlay.hidden = true;
    // The overlay takes the maps' place; the length panes stay below it.
    root.append(heading, texts.top.box, overlay, pair, status, controls);
  } else {
    texts.info = textPanel('info', 23, 20, 'Method information');
    const canvas = el('canvas', 'map map--single');
    canvas.setAttribute('role', 'img');
    canvas.setAttribute('aria-label', 'Map');
    maps.push(canvas);
    const body = el('div', 'run__body');
    body.append(canvas, texts.info.box);
    root.append(heading, body, status, controls);
  }
  container.replaceChildren(root);
  return { root, maps, overlay, texts, status, controls };
}

function button(label: string, onClick: () => void, title?: string): HTMLButtonElement {
  const b = el('button', 'btn', label);
  b.type = 'button';
  if (title) b.title = title;
  b.addEventListener('click', onClick);
  return b;
}

function describeMode(mode: Mode): string {
  switch (mode.kind) {
    case 'key':
      return 'Paused at each step. Press any key, tap the map, or choose Step.';
    case 'timed':
      return `Playing at speed ${mode.digit}: ${(mode.stepMs / 1000).toFixed(1)} s per step.`;
    case 'fast':
      return 'Running to the end…';
  }
}

/** Runs the selected method on screen; resolves when the user leaves the run screen. */
export async function showRun(container: HTMLElement, request: RunRequest): Promise<RunScreenResult> {
  const annealing = request.selection.solver === 'annealing';
  const layout = buildLayout(container, annealing, request.title);
  const cities = request.cities;
  const commands = new CommandQueue();
  let keyHandler: (key: string) => void = (key) => {
    const c = commandForKey(key);
    if (c) commands.push(c);
  };
  const onKeyDown = (event: KeyboardEvent) => {
    if (event.ctrlKey || event.metaKey || event.altKey) return;
    if (event.target instanceof HTMLSelectElement || event.target instanceof HTMLInputElement) return;
    // Let Enter/Space activate a focused button normally.
    if ((event.key === 'Enter' || event.key === ' ') && event.target instanceof HTMLButtonElement) return;
    keyHandler(event.key);
    if (event.key === ' ' || event.key === 'Enter' || event.key === 'Escape') event.preventDefault();
  };
  window.addEventListener('keydown', onKeyDown);

  // Latest frame drawn (a holder, so closures can update it).
  const latest: { frame: FrameSnapshot | null } = { frame: null };
  const draw = () => {
    if (!latest.frame) return;
    latest.frame.views.forEach((view, i) => {
      const canvas = layout.maps[i];
      if (!canvas || canvas.hidden) return;
      fitCanvas(canvas);
      const ctx = canvas.getContext('2d')!;
      drawView(ctx, { left: 0, top: 0, width: canvas.width, height: canvas.height }, cities, view, {
        lineWidth: Math.max(1, canvas.width / 460),
      });
    });
  };
  const resize = new ResizeObserver(() => draw());
  layout.maps.forEach((m) => resize.observe(m));

  const write = (updates: readonly TextUpdate[]) => {
    const touched = new Set<TextArea>();
    for (const u of updates) {
      const panel = layout.texts[u.area];
      if (!panel) continue;
      panel.grid.write(u.text, u.row, u.col, u.color);
      touched.add(u.area);
    }
    for (const area of touched) renderTextGrid(layout.texts[area]!.grid, layout.texts[area]!.pre);
  };
  const show = (frame: FrameSnapshot | null, text: readonly TextUpdate[]) => {
    write(text);
    if (frame) {
      latest.frame = frame;
      draw();
    }
  };

  const setControls = (buttons: HTMLElement[]) => layout.controls.replaceChildren(...buttons);
  const push = (c: Command) => () => commands.push(c);
  const speedSelect = () => {
    const select = el('select', 'speed');
    select.setAttribute('aria-label', 'Play speed');
    select.append(el('option', undefined, 'Play at speed…'));
    for (let digit = 1; digit <= 9; digit++) {
      const o = el('option', undefined, `${digit} (${(digit * 0.5).toFixed(1)} s per step)`);
      o.value = String(digit);
      select.append(o);
    }
    select.addEventListener('change', () => {
      if (select.value) commands.push({ kind: 'speed', digit: Number(select.value) });
      select.selectedIndex = 0;
    });
    return select;
  };
  layout.controls.replaceChildren(
    button('Step', push({ kind: 'continue' }), 'Next step (any key)'),
    speedSelect(),
    button('Run to end', push({ kind: 'fast' }), "Run without stopping ('0')"),
    button('Pause', push({ kind: 'keyMode' }), 'Stop at every step (Enter)'),
    button('Stop', push({ kind: 'abort' }), 'Abandon the run (Esc)'),
  );
  layout.maps.forEach((m) => m.addEventListener('click', () => commands.push({ kind: 'continue' })));

  const startView: ViewSnapshot = {
    tour: request.tour,
    closed: true,
    baseColors: new Array<EgaColor>(cities.length).fill(Role.cityOut),
    marks: [],
    edges: 'plain',
    lines: [],
  };

  const cleanup = () => {
    window.removeEventListener('keydown', onKeyDown);
    resize.disconnect();
  };

  try {
    // Legacy drew the current tour and the key help, then started straight away.
    show({ views: annealing ? [startView, startView] : [startView], endOfStep: false }, annealing ? [] : [say(INTRO, 19, Ega.BLUE)]);

    let session;
    try {
      session = await startSession(request);
    } catch (error) {
      layout.status.textContent = error instanceof Error ? error.message : String(error);
      await waitForKey(layout, commands, (h) => (keyHandler = h), [button('Back', () => commands.push({ kind: 'abort' }))]);
      return { tour: null, lengths: [] };
    }

    const result = await playback({
      session,
      pausesPerStep: pausesPerStep(request.selection),
      commands,
      show: (batch: StepBatch) => show(batch.frame, batch.text),
      nextTick: () => new Promise((r) => requestAnimationFrame(() => r())),
      now: () => performance.now(),
      onModeChange: (mode) => (layout.status.textContent = describeMode(mode)),
    });

    if (!annealing) {
      if (result.aborted || !result.outcome) return { tour: null, lengths: [] };
      const tour = result.outcome.tours[0];
      const length = new Tour(tour).length(new DistanceMatrix(cities));
      const base = latest.frame?.views[0].baseColors ?? startView.baseColors;
      show({ views: [{ ...startView, tour, baseColors: base }], endOfStep: true }, [
        say('F I N I S H E D', 16, Ega.LTRED),
        say(`Length Is ${f(length, 8, 6)}`, 18, Ega.YELLOW),
        say('                  \nHit Esc           \nto Quit...        \n                  ', 19, Ega.LTRED),
      ]);
      layout.status.textContent = `Finished. Tour length ${length.toFixed(6)}.`;
      commands.clear();
      await waitForKey(layout, commands, (h) => (keyHandler = h), [button('Done', () => commands.push({ kind: 'abort' }), 'Esc')], true);
      return { tour, lengths: [length] };
    }

    // Annealing: show both final tours, then offer the overlay.
    const tours = result.outcome?.tours ?? latest.frame?.views.map((v) => v.tour) ?? [request.tour, request.tour];
    const dist = new DistanceMatrix(cities);
    const lengths = tours.map((t) => new Tour(t).length(dist));
    show(
      { views: tours.map((t, i) => ({ ...startView, tour: t, baseColors: latest.frame?.views[i]?.baseColors ?? startView.baseColors })), endOfStep: true },
      [say('Hit Esc To Quit, Any Key For Overlay.', 1, Ega.YELLOW, 35, 'top')],
    );
    layout.status.textContent = `${result.aborted ? 'Stopped' : 'Finished'}. 1-opt ${lengths[0].toFixed(6)}, 2-opt ${lengths[1].toFixed(6)}.`;
    commands.clear();
    const choice = await waitForKey(layout, commands, (h) => (keyHandler = h), [
      button('Overlay', () => commands.push({ kind: 'continue' }), 'Any key'),
      button('Done', () => commands.push({ kind: 'abort' }), 'Esc'),
    ]);
    if (choice === 'abort') return { tour: null, lengths };

    write([
      say("Hit Esc To Quit, Speeds '1'-'9', '0' Pause.", 1, Ega.YELLOW, 35, 'top'),
      say(`Length=${lengths[0].toFixed(6)}  `, 1, Ega.YELLOW, 1, 'left'),
      say(`Length=${lengths[1].toFixed(6)}  `, 1, Ega.LTRED, 1, 'right'),
    ]);
    for (const map of layout.maps) map.hidden = true;
    layout.overlay!.hidden = false;
    layout.status.textContent = 'Overlay: 1-opt in yellow, 2-opt in red.';
    const stop = new AbortController();
    let overlayKeys: ((key: string) => void) | null = null;
    keyHandler = (key) => {
      if (key === 'Escape') stop.abort();
      else overlayKeys?.(key);
    };
    const overlayButtons = [
      button('Pause', () => overlayKeys?.('0'), "'0'"),
      button('Faster', () => overlayKeys?.('5'), "'5'"),
      button('Slower', () => overlayKeys?.('1'), "'1'"),
      button('Done', () => stop.abort(), 'Esc'),
    ];
    setControls(overlayButtons);
    await runOverlay(
      layout.overlay!,
      cities,
      { tour: tours[0], color: Ega.YELLOW },
      { tour: tours[1], color: Ega.LTRED },
      stop.signal,
      (handler) => {
        overlayKeys = handler;
        return () => (overlayKeys = null);
      },
    );
    return { tour: null, lengths };
  } finally {
    cleanup();
  }
}

/**
 * Shows `buttons` and waits for a key. With `escOnly`, only Esc (or a button
 * that sends 'abort') finishes, as on the original's finish screen.
 */
async function waitForKey(
  layout: Layout,
  commands: CommandQueue,
  setKeyHandler: (h: (key: string) => void) => void,
  buttons: HTMLButtonElement[],
  escOnly = false,
): Promise<'abort' | 'continue'> {
  layout.controls.replaceChildren(...buttons);
  buttons[0]?.focus({ preventScroll: true });
  setKeyHandler((key) => {
    if (key === 'Escape') commands.push({ kind: 'abort' });
    else if (!escOnly && key.length === 1) commands.push({ kind: 'continue' });
  });
  for (;;) {
    const c = await commands.wait();
    if (c?.kind === 'abort') return 'abort';
    if (!escOnly && c) return 'continue';
  }
}
