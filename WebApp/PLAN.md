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

   **Notes for phase 3:**
   - The original y-axis points down (`_setwindow(0, -0.05, -0.05, 1.05, 1.05)`, invert off),
     with a 0.05 margin. The placeholder page draws y up; fix that in the renderer.
   - Cities are filled circles of radius 0.023 (double when highlighted) with their number in
     black inside; edges LTRED; background black. Palette in `core/colors.ts`.
   - Black lines in a frame mean "erase this edge" (2-opt shows the edges it breaks that way).
   - Frames hold live tours: draw a frame before resuming the generator.
   - 3-opt costs ~9 ms per move at 30 cities, ~37 ms at 100 and ~420 ms at 256. Run methods
     in a Web Worker (at least for run-to-end on large maps) so the page stays responsive.
   - Annealing's end screen offered an overlay of the two tours (`ControlLCLOverlay`); not
     ported yet.
3. **Player and renderer.** Cities, tour and highlights on a scalable canvas with an EGA palette.
   Keys `0`–`9`, Enter, Esc as in the original; play/pause/step buttons; touch support.
4. **Main screen.** File list, solver/method/init lists, annealing parameters, New / Randomise /
   Save / Go buttons with Shift-N/R/S/G and Shift-H, message bar hints, help panel.
5. **Files and storage.** Bundled samples from `Data/`, upload or drag-and-drop, Save as a
   `.TSP` download, "Current" kept in `localStorage`, PNG screenshot instead of the `V` key.
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
