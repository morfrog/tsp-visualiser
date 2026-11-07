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
1. **Core logic.** `.TSP` parser, distances, tour, path length. Vitest unit tests. The
   same-result-every-run methods (SS in `DOCO.DOC`) on `30CITY_*.TSP` should reproduce the DOCO
   benchmark ratios.
2. **Algorithms as generators.** Insertion, Nearest Neighbour, 1/2/3-opt, annealing. Seeded PRNG
   (methods that vary per run, DS, won't match 1997 runs exactly).
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
npm run build     # type-check + production build to dist/
npm run preview   # serve dist/ locally
```
