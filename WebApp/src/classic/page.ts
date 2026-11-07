// Classic mode page (development only): the original 1997 VTTSP.EXE running
// in js-dos. The dev server's classic plugin (dev/classicPlugin.ts) serves
// the original files if they are present locally; nothing of the original is
// ever built or deployed.

const JS_DOS_VERSION = '8.5.3';
const JS_DOS = `https://cdn.jsdelivr.net/npm/js-dos@${JS_DOS_VERSION}/dist`;

interface OriginalStatus {
  available: boolean;
  location: string;
  missing: string[];
}

declare global {
  interface Window {
    Dos?: (element: HTMLElement, options: Record<string, unknown>) => unknown;
  }
}

const root = document.querySelector<HTMLElement>('#classic')!;

function el<K extends keyof HTMLElementTagNameMap>(tag: K, className?: string, text?: string): HTMLElementTagNameMap[K] {
  const e = document.createElement(tag);
  if (className) e.className = className;
  if (text !== undefined) e.textContent = text;
  return e;
}

function load(tag: 'script' | 'link', url: string): Promise<void> {
  return new Promise((resolve, reject) => {
    const e = document.createElement(tag);
    if (e instanceof HTMLScriptElement) e.src = url;
    else {
      e.rel = 'stylesheet';
      e.href = url;
    }
    e.onload = () => resolve();
    e.onerror = () => reject(new Error(`Could not load ${url}`));
    document.head.append(e);
  });
}

async function start(): Promise<void> {
  root.append(el('h1', undefined, 'VTTSP Classic'));
  const note = el('p', 'classic__note');
  root.append(note);

  if (!import.meta.env.DEV) {
    note.textContent = 'Classic mode is only available when developing (npm run dev).';
    return;
  }

  const base = import.meta.env.BASE_URL;
  let status: OriginalStatus;
  try {
    status = (await (await fetch(`${base}classic/status.json`)).json()) as OriginalStatus;
  } catch {
    note.textContent = 'The dev server did not answer. Is this running under npm run dev?';
    return;
  }
  if (!status.available) {
    note.textContent = `The original program isn't here. Put ${status.missing.join(' and ')} (and the HLP folder and .TSP files) in ${status.location}, or set VTTSP_ORIGINAL_DIR, then reload.`;
    return;
  }

  note.textContent = `The original 1997 program, running in DOSBox (js-dos ${JS_DOS_VERSION}), from ${status.location}. Click the screen to capture the mouse. Development only: never deployed.`;
  const screen = el('div', 'classic__screen');
  root.append(screen);
  try {
    await Promise.all([load('link', `${JS_DOS}/js-dos.css`), load('script', `${JS_DOS}/js-dos.js`)]);
    window.Dos!(screen, {
      url: `${base}classic/vttsp.jsdos`,
      pathPrefix: `${JS_DOS}/emulators/`,
      autoStart: true,
      kiosk: true,
    });
  } catch (error) {
    note.textContent = error instanceof Error ? error.message : String(error);
  }
}

void start();

export {};
