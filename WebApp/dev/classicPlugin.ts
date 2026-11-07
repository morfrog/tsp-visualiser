// Classic mode, for development only: runs the original 1997 VTTSP.EXE in
// the browser through js-dos (DOSBox compiled to WebAssembly).
//
// The original program is not part of this repository and is never
// published: it contains personal details, and DOS4GW.EXE is third-party
// software. If the files are present at ORIGINAL_DIR (by default
// Temp/Tsp/PACK in the repository, which git ignores), the dev server
// packs them into a js-dos bundle on request. `vite build` never runs this
// plugin, and classic.html is not a build input, so nothing of the original
// reaches dist/. The deploy workflow also checks that.

import { existsSync, readdirSync, readFileSync, statSync } from 'node:fs';
import { join, relative, resolve } from 'node:path';
import { strToU8, zipSync, type Zippable } from 'fflate';
import type { Plugin } from 'vite';

/** Where the original files are expected, relative to the repository root. */
export const DEFAULT_ORIGINAL_DIR = 'Temp/Tsp/PACK';

/** Files the original needs to start. */
export const REQUIRED_FILES = ['VTTSP.EXE', 'DOS4GW.EXE'];

/** DOSBox settings: start the program straight away, as typing VTTSP at the C:\ prompt. */
const DOSBOX_CONF = `[sdl]
autolock=true

[cpu]
core=auto
cycles=auto

[autoexec]
mount c .
c:
VTTSP.EXE
`;

export interface OriginalStatus {
  available: boolean;
  /** Where the files are looked for, relative to the repository root. */
  location: string;
  missing: string[];
}

function listFiles(dir: string): string[] {
  return readdirSync(dir).flatMap((name) => {
    const path = join(dir, name);
    return statSync(path).isDirectory() ? listFiles(path) : [path];
  });
}

export function originalStatus(dir: string, repoRoot: string): OriginalStatus {
  const location = relative(repoRoot, dir).replace(/\\/g, '/') || '.';
  const present = existsSync(dir) ? new Set(readdirSync(dir).map((n) => n.toUpperCase())) : new Set<string>();
  const missing = REQUIRED_FILES.filter((f) => !present.has(f));
  return { available: missing.length === 0, location, missing };
}

/** Every file in `dir` (paths relative to it, upper case as DOS saw them) plus the DOSBox settings. */
export function buildBundle(dir: string): Uint8Array {
  const files: Zippable = { '.jsdos/dosbox.conf': strToU8(DOSBOX_CONF) };
  for (const path of listFiles(dir)) {
    const name = relative(dir, path).replace(/\\/g, '/').toUpperCase();
    files[name] = readFileSync(path);
  }
  return zipSync(files);
}

export function classicPlugin(options: { repoRoot: string; base: string }): Plugin {
  const dir = resolve(options.repoRoot, process.env.VTTSP_ORIGINAL_DIR ?? DEFAULT_ORIGINAL_DIR);
  const prefix = `${options.base.replace(/\/$/, '')}/classic`;
  return {
    name: 'vttsp-classic',
    apply: 'serve', // dev server only; never part of a build
    configureServer(server) {
      server.middlewares.use(`${prefix}/status.json`, (_req, res) => {
        res.setHeader('Content-Type', 'application/json');
        res.setHeader('Cache-Control', 'no-store');
        res.end(JSON.stringify(originalStatus(dir, options.repoRoot)));
      });
      server.middlewares.use(`${prefix}/vttsp.jsdos`, (_req, res) => {
        const status = originalStatus(dir, options.repoRoot);
        if (!status.available) {
          res.statusCode = 404;
          res.end(`Original VTTSP files not found in ${status.location}`);
          return;
        }
        res.setHeader('Content-Type', 'application/zip');
        res.setHeader('Cache-Control', 'no-store');
        res.end(buildBundle(dir));
      });
    },
  };
}
