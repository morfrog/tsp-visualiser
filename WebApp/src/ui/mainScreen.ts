// The main screen (Legacy INTERFAC.C: DrawScreen, IntWinManager and main()).
//
// Windows, as on the 640 x 350 original: title bar; file list; map view;
// help window; solver, method and init lists; New, Save, Randomize and Go!
// buttons; message bar. Hovering or focusing a window shows its hint in the
// message bar; choosing an item shows its help. Right-click (or choose it
// again) to change an annealing setting. Shift-N, Shift-S, Shift-R, Shift-G
// and Shift-H work as before; F1 is help and F2 is save.
//
// The Quit button had nothing to do in a browser; its slot holds Help.

import { selectionProblem, SOLVERS } from '../algorithms/catalog';
import { f, d } from '../algorithms/text';
import { EGA_PALETTE, Ega, Role, type EgaColor } from '../core/colors';
import { DistanceMatrix } from '../core/distances';
import { citiesInTourOrder, makeMap, randomizeOrder } from '../core/makemap';
import { AnsiCRandom, clockSeed } from '../core/random';
import { Tour } from '../core/tour';
import { formatTsp } from '../core/tspfile';
import { help, type HelpEntry } from '../data/help';
import { showRun } from '../player/runScreen';
import { drawView, fitCanvas } from '../render/mapView';
import { renderTextGrid, TextGrid } from '../render/textGrid';
import {
  addFile,
  annealSetting,
  changeAnnealSetting,
  initItems,
  methodItems,
  selectedFile,
  selectedInit,
  selectedMethod,
  selectFile,
  selectInit,
  selectionLabel,
  selectMethod,
  selectSolver,
  setCurrent,
  solverId,
  toSelection,
  type ListItem,
  type MenuState,
} from './menuState';

const VERSION = '2.10'; // Legacy VERSION 2.1, printed "%1.2f"
const HELP_ROWS = 10;
const HELP_COLS = 34;
const MIN_NEW_POINTS = 3;
const MAX_NEW_POINTS = 1000;

type WindowId = 'title' | 'files' | 'view' | 'help' | 'solver' | 'method' | 'init' | 'new' | 'save' | 'random' | 'go' | 'helpButton' | 'msg';

/** Character filters for typed input (Legacy IsIntChar, IsDoubleChar, IsFileChar). */
const FILTERS = {
  int: /^[0-9+-]$/,
  double: /^[0-9.+\-Ee]$/,
  file: /^[^\\/:*?"<>|]$/,
};

/** The original's help text, with the mouse and Quit lines updated for the web. */
const VTTSP_HELP: HelpEntry = {
  title: 'VTTSP - Help (Shift-H)',
  lines: [
    'Click = Select  Right-click =',
    'Change (annealing settings)',
    'Button    Quick    Target',
    'New       Shift-N  Current',
    'Randomize Shift-R  Current',
    'Save      Shift-S  File Entered',
    'HOW TO USE: Make Your File,TSP,',
    'Method & Init Selections and',
    'then Press Go! (Shift-G)',
  ],
};

function el<K extends keyof HTMLElementTagNameMap>(tag: K, className?: string, text?: string): HTMLElementTagNameMap[K] {
  const e = document.createElement(tag);
  if (className) e.className = className;
  if (text !== undefined) e.textContent = text;
  return e;
}

/** A label with its first letter green and the rest blue, like the original buttons. */
function hotkeyLabel(button: HTMLButtonElement, text: string): void {
  const first = el('span', 'hot', text[0]);
  const rest = el('span', 'cold', text.slice(1));
  button.replaceChildren(first, rest);
}

export class MainScreen {
  private root!: HTMLElement;
  private helpGrid = new TextGrid(HELP_ROWS, HELP_COLS);
  private helpPre!: HTMLElement;
  private helpBox!: HTMLElement;
  private msg!: HTMLElement;
  private canvas!: HTMLCanvasElement;
  private lists!: Record<'files' | 'solver' | 'method' | 'init', HTMLElement>;
  private observer: ResizeObserver | null = null;
  private prompt: HTMLInputElement | null = null;
  private cancelPrompt: (() => void) | null = null;
  private busy = false;
  private readonly onKeyDown = (e: KeyboardEvent) => this.handleKey(e);

  constructor(
    private readonly container: HTMLElement,
    private readonly state: MenuState,
  ) {}

  mount(): void {
    this.build();
    this.renderLists();
    this.drawMap();
    window.addEventListener('keydown', this.onKeyDown);
  }

  unmount(): void {
    window.removeEventListener('keydown', this.onKeyDown);
    this.observer?.disconnect();
    this.observer = null;
  }

  // ---------------------------------------------------------------- layout

  private build(): void {
    const root = el('section', 'main-screen');
    root.setAttribute('aria-label', 'VTTSP main screen');

    const win = (id: WindowId, tag: keyof HTMLElementTagNameMap = 'div') => {
      const w = el(tag as 'div', `win win--${id}`);
      w.dataset.win = id;
      w.addEventListener('pointerenter', () => this.hint(id));
      w.addEventListener('focusin', () => this.hint(id));
      return w;
    };

    const title = win('title');
    title.append(el('h1', 'win__title', 'Visualisation Tool For The Travelling Salesman Problem'));

    const files = win('files');
    files.append(el('h2', 'sr-only', 'Files'));
    const fileList = el('div', 'list');
    fileList.setAttribute('role', 'group');
    fileList.setAttribute('aria-label', 'Files');
    files.append(fileList);

    const view = win('view');
    this.canvas = el('canvas', 'map map--view');
    this.canvas.setAttribute('role', 'img');
    view.append(this.canvas);

    const helpWin = win('help');
    this.helpBox = el('div', 'textbox help-box');
    this.helpPre = el('pre', 'text text--help');
    this.helpPre.setAttribute('aria-live', 'polite');
    this.helpPre.style.setProperty('--cols', String(HELP_COLS));
    this.helpPre.style.setProperty('--rows', String(HELP_ROWS));
    this.helpBox.append(this.helpPre);
    helpWin.append(this.helpBox);

    const listWin = (id: 'solver' | 'method' | 'init', label: string) => {
      const w = win(id);
      const list = el('div', 'list');
      list.setAttribute('role', 'group');
      list.setAttribute('aria-label', label);
      w.append(list);
      return [w, list] as const;
    };
    const [solverWin, solverList] = listWin('solver', 'Solver');
    const [methodWin, methodList] = listWin('method', 'Method');
    const [initWin, initList] = listWin('init', 'Initialisation');

    const buttons = el('div', 'win-buttons');
    const btn = (id: WindowId, label: string, action: () => void, shortcut: string) => {
      const b = win(id, 'button') as unknown as HTMLButtonElement;
      b.type = 'button';
      b.classList.add('win-btn');
      hotkeyLabel(b, label);
      b.setAttribute('aria-label', `${label} (${shortcut})`);
      b.setAttribute('aria-keyshortcuts', shortcut);
      b.addEventListener('click', action);
      return b;
    };
    buttons.append(
      btn('new', 'New', () => this.newMap(), 'Shift+N'),
      btn('save', 'Save', () => this.save(), 'Shift+S'),
      btn('random', 'Randomize', () => this.randomize(), 'Shift+R'),
      btn('go', 'GO!', () => void this.go(), 'Shift+G'),
    );
    const helpButton = btn('helpButton', 'Help', () => this.showHelp(VTTSP_HELP), 'Shift+H');

    this.msg = win('msg');
    this.msg.setAttribute('role', 'status');

    root.append(title, files, view, helpWin, solverWin, methodWin, initWin, buttons, helpButton, this.msg);
    this.container.replaceChildren(root);
    this.root = root;
    this.lists = { files: fileList, solver: solverList, method: methodList, init: initList };

    this.observer = new ResizeObserver(() => {
      if (this.canvas.isConnected) this.drawMap();
    });
    this.observer.observe(this.canvas);
    this.about();
  }

  private renderList(
    list: HTMLElement,
    items: readonly ListItem[] | readonly string[],
    selected: number | null,
    onChoose: (index: number, event: MouseEvent) => void,
    onChange?: (index: number) => void,
  ): void {
    list.replaceChildren(
      ...items.map((item, i) => {
        const name = typeof item === 'string' ? item : item.name;
        const b = el('button', `list__item${i === selected ? ' is-selected' : ''}`, name);
        b.type = 'button';
        b.setAttribute('aria-pressed', String(i === selected));
        b.addEventListener('click', (e) => onChoose(i, e));
        if (onChange) {
          b.addEventListener('contextmenu', (e) => {
            e.preventDefault();
            onChange(i);
          });
        }
        return b;
      }),
    );
  }

  private renderLists(): void {
    // Lists are rebuilt on every change; keep keyboard focus on the same item.
    const active = document.activeElement;
    const focused = (Object.keys(this.lists) as (keyof typeof this.lists)[])
      .map((name) => ({ name, index: [...this.lists[name].children].indexOf(active as Element) }))
      .find((f) => f.index >= 0);
    this.fillLists();
    if (focused) {
      const items = this.lists[focused.name].children;
      (items[Math.min(focused.index, items.length - 1)] as HTMLElement | undefined)?.focus({ preventScroll: true });
    }
  }

  private fillLists(): void {
    const s = this.state;
    this.renderList(
      this.lists.files,
      s.files.map((file) => file.name),
      s.selectedFile,
      (i) => {
        selectFile(s, i);
        this.renderLists();
        this.drawMap();
        this.statistics();
      },
    );
    this.renderList(this.lists.solver, SOLVERS.map((c) => ({ name: c.name, help: c.help })), s.solver, (i) => {
      selectSolver(s, i);
      this.renderLists();
      this.showHelp(help(SOLVERS[s.solver].help));
    });
    const methods = methodItems(s);
    this.renderList(this.lists.method, methods, selectedMethod(s), (i) => {
      selectMethod(s, i);
      this.renderLists();
      this.showHelp(help(methods[i].help));
    });
    const inits = initItems(s);
    const annealing = solverId(s) === 'annealing';
    this.renderList(
      this.lists.init,
      inits,
      selectedInit(s),
      (i) => {
        const again = annealing && selectedInit(s) === i;
        selectInit(s, i);
        this.renderLists();
        // Choosing an annealing setting again changes it (for touch screens).
        if (again) this.editAnnealSetting();
        else this.showHelp(help(inits[i].help));
      },
      annealing
        ? (i) => {
            selectInit(s, i);
            this.renderLists();
            this.editAnnealSetting();
          }
        : undefined,
    );
    this.canvas.setAttribute('aria-label', `Map of ${selectedFile(s).name}`);
  }

  private drawMap(): void {
    const file = selectedFile(this.state);
    fitCanvas(this.canvas);
    const ctx = this.canvas.getContext('2d')!;
    drawView(
      ctx,
      { left: 0, top: 0, width: this.canvas.width, height: this.canvas.height },
      file.cities,
      {
        tour: file.cities.map((_, i) => i),
        closed: true,
        baseColors: new Array<EgaColor>(file.cities.length).fill(Role.cityOut),
        marks: [],
        edges: 'plain',
        lines: [],
      },
      { lineWidth: Math.max(1, this.canvas.width / 300) },
    );
  }

  // ---------------------------------------------------------------- help window

  private writeHelp(updates: { text: string; row: number; color: EgaColor; col?: number }[], clear = true): void {
    this.closePrompt();
    if (clear) this.helpGrid.clear();
    for (const u of updates) this.helpGrid.write(u.text, u.row, u.col ?? 1, u.color);
    renderTextGrid(this.helpGrid, this.helpPre);
  }

  /** Legacy ViewAbout, without the personal details. */
  private about(): void {
    this.writeHelp([
      { text: `VTTSP V${VERSION} 1997`, row: 1, col: 8, color: Ega.GREEN },
      { text: 'Visualisation Tool for the', row: 3, col: 4, color: Ega.GREEN },
      { text: 'Travelling Salesman Problem', row: 4, col: 4, color: Ega.GREEN },
      { text: 'Web port of the DOS program.', row: 6, col: 4, color: Ega.GREEN },
      { text: 'Shift-H for help.', row: 8, col: 4, color: Ega.YELLOW },
    ]);
  }

  /** Legacy ViewStatistics. */
  private statistics(): void {
    const file = selectedFile(this.state);
    const length = file.cities.length > 1 ? Tour.identity(file.cities.length).length(new DistanceMatrix(file.cities)) : 0;
    this.writeHelp([
      { text: `Statistics Of ${file.name}`, row: 1, color: Ega.YELLOW },
      { text: `Path Length = ${f(length, 8, 6)}`, row: 4, color: Ega.GREEN },
      { text: `Cities      = ${d(file.cities.length, 4)}`, row: 6, color: Ega.GREEN },
    ]);
  }

  /** Legacy DrawChoiceHelp. */
  private showHelp(entry: HelpEntry): void {
    this.writeHelp([
      { text: entry.title, row: 1, color: Ega.YELLOW },
      { text: entry.lines.join('\n'), row: 2, color: Ega.LTRED },
    ]);
  }

  private hint(id: WindowId): void {
    const s = this.state;
    const solver = SOLVERS[s.solver].name;
    const messages: Record<WindowId, string> = {
      title: 'Visualisation Tool for the TSP, 1997',
      files: 'Choose A File',
      view: `View of ${selectedFile(s).name}`,
      help: 'Help on this program                     (Shift-H)',
      solver: 'Choose a TSP method',
      method: `Choose an ${solver} Operation Method`,
      init:
        solverId(s) === 'annealing'
          ? 'Annealing Inits Click = Select, Click again or Right-click = Change.'
          : `Choose an ${solver} Initialisation Method`,
      new: 'Creates a new random map - to Current    (Shift-N)',
      save: 'Saves the selected map - to a file       (Shift-S)',
      random: 'Randomizes the selected map - to Current (Shift-R)',
      go: 'Start with the current settings          (Shift-G)',
      helpButton: 'Help on this program                     (Shift-H)',
      msg: 'This is the message box',
    };
    this.message(messages[id], Ega.GREEN);
  }

  private message(text: string, color: EgaColor): void {
    this.msg.textContent = text;
    this.msg.style.color = EGA_PALETTE[color];
  }

  /**
   * Reads typed text into the help window at a row and column, as Legacy
   * ReadString did: Enter accepts, Esc aborts, only `filter` characters count.
   * Resolves to the text, null if aborted (Esc, or focus left the input), or
   * undefined if another action replaced the prompt (the caller then does nothing).
   */
  private ask(
    row: number,
    col: number,
    maxLength: number,
    filter: RegExp,
    label: string,
  ): Promise<string | null | undefined> {
    this.closePrompt();
    const input = el('input', 'help-input');
    input.type = 'text';
    input.maxLength = maxLength;
    input.autocomplete = 'off';
    input.spellcheck = false;
    input.setAttribute('aria-label', label);
    input.style.setProperty('--row', String(row));
    input.style.setProperty('--col', String(col));
    if (filter === FILTERS.int) input.inputMode = 'numeric';
    else if (filter === FILTERS.double) input.inputMode = 'decimal';
    this.helpBox.append(input);
    this.prompt = input;
    input.focus();

    return new Promise((resolve) => {
      // Settle once, before removing the input: removing a focused element
      // fires its blur handler straight away, which would otherwise win.
      let settled = false;
      const done = (value: string | null | undefined) => {
        if (settled) return;
        settled = true;
        resolve(value);
        if (this.prompt === input) {
          this.prompt = null;
          this.cancelPrompt = null;
        }
        input.remove();
      };
      // Another action replacing this prompt settles it silently (undefined).
      this.cancelPrompt = () => done(undefined);
      // Drop characters the filter refuses, whether typed, pasted or composed.
      input.addEventListener('input', () => {
        const kept = [...input.value].filter((ch) => filter.test(ch)).join('');
        if (kept !== input.value) input.value = kept;
      });
      input.addEventListener('keydown', (e) => {
        e.stopPropagation();
        if (e.key === 'Enter') {
          e.preventDefault();
          done(input.value);
        } else if (e.key === 'Escape') {
          e.preventDefault();
          done(null);
        }
      });
      input.addEventListener('blur', () => done(null));
    });
  }

  private closePrompt(): void {
    this.cancelPrompt?.();
  }

  // ---------------------------------------------------------------- buttons

  /** Legacy NewBtnPress. */
  private async newMap(): Promise<void> {
    this.writeHelp([
      { text: 'New...', row: 1, color: Ega.GREEN },
      { text: ':Please Enter', row: 2, color: Ega.YELLOW },
      { text: ':The Number Of Points (Integer) \n:', row: 3, color: Ega.YELLOW },
    ]);
    const typed = await this.ask(4, 2, 10, FILTERS.int, 'Number of points');
    if (typed === undefined) return;
    if (typed === null) return this.writeHelp([{ text: 'Operation Aborted', row: 6, color: Ega.GREEN }], false);
    const count = parseInt(typed, 10) || 0;
    if (count < MIN_NEW_POINTS || count > MAX_NEW_POINTS) {
      return this.writeHelp(
        [
          { text: `:${typed}`, row: 4, color: Ega.LTRED },
          { text: `Use ${MIN_NEW_POINTS} to ${MAX_NEW_POINTS} points.`, row: 6, color: Ega.LTRED },
        ],
        false,
      );
    }
    setCurrent(this.state, makeMap(count, new AnsiCRandom(clockSeed())));
    this.renderLists();
    this.drawMap();
    this.writeHelp([{ text: `:${typed}`, row: 4, color: Ega.LTRED }, { text: 'Map Created..', row: 6, color: Ega.GREEN }], false);
  }

  /** Legacy SaveBtnPress: saves the selected map, here as a download. */
  private async save(): Promise<void> {
    this.writeHelp([{ text: 'Save As :\n:', row: 1, color: Ega.YELLOW }]);
    const answer = await this.ask(2, 2, 20, FILTERS.file, 'File name');
    if (answer === undefined) return;
    const typed = answer?.trim();
    if (!typed) return this.writeHelp([{ text: 'Save Aborted..', row: 7, color: Ega.LTRED }], false);
    const file = selectedFile(this.state);
    downloadText(typed, formatTsp(file.cities));
    const listed = addFile(this.state, typed, file.cities);
    this.renderLists();
    this.writeHelp(
      [
        { text: `:${typed}`, row: 2, color: Ega.LTRED },
        { text: `Map Saved As ${typed}...`, row: 7, color: Ega.GREEN },
        ...(listed ? [] : [{ text: 'Only .TSP files are listed.', row: 9, color: Ega.YELLOW }]),
      ],
      false,
    );
  }

  /** Legacy RndBtnPress. */
  private randomize(): void {
    setCurrent(this.state, randomizeOrder(selectedFile(this.state).cities, new AnsiCRandom(clockSeed())));
    this.renderLists();
    this.drawMap();
    this.statistics();
  }

  /** Legacy AnnealInitChange. */
  private async editAnnealSetting(): Promise<void> {
    const key = annealSetting(this.state);
    const entry = help(
      { temperature: 'ITEMP.HLP', coolingRate: 'ICOOL.HLP', changesPerTemperature: 'IATTMP.HLP' }[key],
    );
    this.writeHelp([{ text: `Please Enter The\n:${entry.title}\n:`, row: 1, color: Ega.YELLOW }]);
    const typed = await this.ask(3, 2, 60, key === 'changesPerTemperature' ? FILTERS.int : FILTERS.double, entry.title);
    if (typed === undefined) return;
    if (typed === null) return this.writeHelp([{ text: 'Operation Aborted', row: 4, color: Ega.GREEN }], false);
    const { ok, message } = changeAnnealSetting(this.state, typed);
    this.renderLists();
    this.writeHelp(
      [
        { text: `:${typed}`, row: 3, color: Ega.LTRED },
        { text: message, row: 4, color: ok ? Ega.GREEN : Ega.LTRED },
      ],
      false,
    );
  }

  /** Legacy Go!: runs the selection, then saves a finished tour to Current. */
  private async go(): Promise<void> {
    if (this.busy) return;
    const s = this.state;
    const file = selectedFile(s);
    const selection = toSelection(s);
    const problem = selectionProblem(file.cities.length, selection);
    if (problem) return this.message(problem, Ega.LTRED);

    this.busy = true;
    this.unmount();
    try {
      const result = await showRun(this.container, {
        title: `${file.name}: ${selectionLabel(s)}`,
        cities: file.cities,
        tour: file.cities.map((_, i) => i),
        selection,
        seed: clockSeed(),
      });
      if (result.tour) {
        // A finished insertion or iteration run is saved to Current and selected.
        setCurrent(s, citiesInTourOrder(file.cities, result.tour));
      } else {
        // Otherwise the original still redrew the screen with SetFile(INITIALISE),
        // which copied the selected map to Current, keeping the selection.
        const keep = s.selectedFile;
        setCurrent(s, file.cities.map((c) => ({ ...c })));
        s.selectedFile = keep;
      }
    } finally {
      this.busy = false;
      // Remounting shows the About panel, as DrawScreen did.
      this.mount();
      this.root.querySelector<HTMLElement>('.win--go')?.focus({ preventScroll: true });
    }
  }

  // ---------------------------------------------------------------- keys

  private handleKey(e: KeyboardEvent): void {
    if (this.busy || this.prompt || e.ctrlKey || e.metaKey || e.altKey) return;
    if (e.target instanceof HTMLInputElement || e.target instanceof HTMLSelectElement) return;
    const actions: Record<string, () => void> = {
      N: () => void this.newMap(),
      S: () => void this.save(),
      R: () => this.randomize(),
      G: () => void this.go(),
      H: () => this.showHelp(VTTSP_HELP),
      F1: () => this.showHelp(VTTSP_HELP),
      F2: () => void this.save(),
    };
    // The original compared getch() with capitals: Shift + letter (or Caps Lock).
    const key = e.key.length === 1 && e.shiftKey ? e.key.toUpperCase() : e.key;
    const action = actions[key];
    if (action) {
      e.preventDefault();
      action();
    }
  }
}

/** Offers text as a file download. */
function downloadText(name: string, text: string): void {
  const url = URL.createObjectURL(new Blob([text], { type: 'text/plain' }));
  const a = el('a');
  a.href = url;
  a.download = name;
  document.body.append(a);
  a.click();
  a.remove();
  setTimeout(() => URL.revokeObjectURL(url), 1000);
}
