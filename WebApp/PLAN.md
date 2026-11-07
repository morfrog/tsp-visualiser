# VTTSP Web Port — Plan

Port of the 1997 DOS program *Visualisation Tool for the Travelling Salesman Problem*
(VTTSP v2.1, Watcom C, `Legacy/`) to a static web app hosted on GitHub Pages.

**Stack:** Vite + TypeScript, HTML Canvas 2D, no UI framework.
**Live URL:** `https://morfrog.github.io/tsp-visualiser/`

## Legacy summary

| Legacy file | Role | Web port |
|---|---|---|
| `INTERFAC.C` | Main program: file list, solver/method/init lists, buttons, message bar, help | `src/ui/*` (plain DOM) |
| `INSERT.C` | Insertion: 3 inits × 7 operations, plus Nearest Neighbour | `src/algorithms/insert.ts` |
| `OPTIM.C` | Iteration: 1-opt, 2-opt, 3-opt | `src/algorithms/optim.ts` |
| `ANNEAL.C` | Simulated annealing (temperature, cooling rate, changes per temp) | `src/algorithms/anneal.ts` |
| `PATH.C` | Tour as a doubly linked list | `src/core/tour.ts` (array based) |
| `FILE.C` | `.TSP` reader/writer, distance matrix | `src/core/tspfile.ts`, `src/core/distances.ts` |
| `PROBLEM.C` | Drawing helpers, masks, `WaitTill` delay/keypress loop | `src/render/*`, `src/player.ts` |
| `SCREEN.C`, `MOUSE.C`, `graph.h`, `conio` | EGA 640×350 graphics, DOS mouse driver, keyboard | Replaced by canvas + DOM events |
| `HLP/*.HLP` | Help text | Bundled text |
| `TSP.C`, `TSPOPT.C`, `TSPANN.C`, `COMPSOLN.C` | Command-line test and benchmark programs | Optional benchmark page |

**Key porting issue:** the algorithms block mid-loop in `WaitTill()`/`getch()` for animation,
speed changes (`0`–`9`, Enter = step mode) and Esc (abort and revert). On the web each algorithm
becomes a generator that `yield`s a frame (highlights + info text). A player drives it with
`requestAnimationFrame`, a speed setting or single steps.

**`.TSP` format:** first line is the city count, then one `x y` pair per line in [0, 1].

## Phases

0. **Scaffold and deploy.** Vite + TS in `WebApp/`, `base: '/tsp-visualiser/'`, GitHub Actions
   workflow deploying `WebApp/dist` to Pages on push to `main`.
   *Manual step:* repo Settings → Pages → Source = **GitHub Actions**.
1. **Core logic.** ✅ Done. `src/core/`: `tspfile.ts` (character-scan reader matching
   `FILE.C`, so CRLF, tabs, exponents, stray bytes all parse), `distances.ts`, `tour.ts`
   (array in linked-list head order, so inserts, deletes and reversals land exactly where
   `PATH.C` put them). Vitest tests in `src/**/*.test.ts` read the real files in `../Data`.
   *Oracle:* the `30CITY_n.TSP` files are stored in optimal tour order, and their file-order
   lengths match `Data/10SOLNS.TXT`. Phase 2 uses these to check the DOCO benchmark ratios:
   the same-result-every-run methods (SS in `DOCO.DOC`) should reproduce them exactly.
2. **Algorithms as generators.** ✅ Done. `src/algorithms/`: `insertion.ts` (3 starts × 7 rules,
   Nearest Neighbour), `optim.ts` (1/2/3-opt), `anneal.ts` (1-opt vs 2-opt side by side),
   `catalog.ts` (menus in the original order with help files, and `startRun`). Each method is a
   generator yielding a `Frame` (`frame.ts`) wherever the C paused in `WaitTill()`: views with
   highlight marks, extra lines and edge colouring, plus text written into a character grid at
   the original rows and columns. `core/random.ts` is a seeded ANSI C `rand()`; DS methods repeat
   for a seed but can't match 1997 runs, which were seeded from the clock.
   *Verified:* all 120 SS insertion results (Closest/Furthest × 6 rules × 10 problems) and all
   30 iteration results match DOCO's tables to 5 decimals. The iteration benchmark started from
   the shuffled `30CITIES.DI0`–`DI9` files.

3. **Player and renderer.** ✅ Done.
   - `run/`: methods run in a Web Worker (`run.worker.ts`, falling back to the main thread).
     `Stepper` turns frames into plain snapshots and serves one frame, or as many as fit in a
     time budget (keeping all their text), for run-to-end.
   - `render/mapView.ts`: canvas drawing as `SCREEN.C`/`PROBLEM.C` did: y down, 0.05 margins,
     ellipse cities of radius 0.023 with black numbers (left off above 100 cities), LTRED or
     segment-coloured edges, black lines erase. Canvases match the device pixel ratio and the
     original windows' shapes on a 4:3 screen.
   - `render/textGrid.ts`: text panels as character grids of the original sizes (23 × 20 side
     panel; annealing 3 × 78 top bar and 2 × 38 panes), drawn as HTML text.
   - `player/playback.ts`: `WaitTill`/`DoInput` as a testable controller. Starts paused at
     each step; any key continues, digit d plays at d × 0.5 s per step (Watcom `clock()` at
     100 ticks/s), `0` runs to the end, Enter pauses again, Esc stops.
   - `player/runScreen.ts`: run screens and finish screens ("F I N I S H E D", Esc to return
     the new tour). Annealing ends with the sliding overlay (`player/overlay.ts`, port of
     `ControlLCLOverlay`). Buttons and map taps mirror the keys.
   - Samples come from `Data/*.TSP` via `import.meta.glob` (`data/samples.ts`); the Vite dev
     server is allowed to read `..` for that.
4. **Main screen.** ✅ Done. `ui/mainScreen.ts` lays out `IntWinManager`'s windows on the
   original 640 × 350 grid (stacked on phones): title, file list, map view, help window, solver,
   method and init lists, New / Save / Randomize / GO! buttons, message bar. `ui/menuState.ts`
   holds the choices and rules (`SetFile`, `SetSolver`, `SetMethod`, `SetInit`), tested apart
   from the page.
   - As in the original, a map's city order is its tour. A finished insertion or iteration run,
     New (`MakeMap`) and Randomize (`rndfile`) write to Current and select it. Any other return
     from a run copies the selected map to Current (`DrawScreen` → `SetFile(INITIALISE)`).
   - Hover or focus shows each window's hint in the message bar. Choosing an item shows its
     help from `Legacy/HLP` (`data/help.ts`); choosing a file shows its statistics.
   - Shift-N/S/R/G/H, F1 and F2 as before. Typed input is read in the help window like
     `ReadString` (Enter accepts, Esc aborts, character filters).
   - Annealing settings change by right-click, as before, or by choosing the selected one again
     (for touch screens), with the original's checks and messages.
   - Save downloads the selected map as a `.TSP` file and lists it (`.TSP` names only, as the
     original's directory scan did) for this visit. Keeping it across visits is phase 5.
   - Changes from the original: Quit's slot holds Help; button text uses light blue rather than
     BLUE, which is hard to read on black. Personal details (name, student number, institution,
     coordinator) are left out of the title, hints and About panel.
5. **Files and storage.** Upload or drag-and-drop `.TSP` files into the list; keep saved files
   and "Current" in `localStorage`; PNG screenshot instead of the `V` key. (Bundled samples
   and Save as a download are done.)
6. **Extras (optional).** Benchmark page recreating the DOCO tables; "Classic mode" running the
   original `VTTSP.EXE` in js-dos. Quit button dropped.

## Development

```bash
cd WebApp
npm install
npm run dev       # local dev server
npm test          # run the Vitest suite once
npm run build     # type-check + production build to dist/
npm run preview   # serve dist/ locally
```

CI (`.github/workflows/deploy-pages.yml`) runs the tests and the build on Node 22, then deploys.
