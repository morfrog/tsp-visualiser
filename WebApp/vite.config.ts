import { resolve } from 'node:path';
import { defineConfig } from 'vite';
import { classicPlugin } from './dev/classicPlugin';

const base = '/tsp-visualiser/';

// GitHub Pages serves the site from https://morfrog.github.io/tsp-visualiser/
export default defineConfig({
  base,
  server: {
    // The sample problems are bundled from the repository's Data/ folder.
    fs: { allow: ['..'] },
  },
  // Classic mode (the original VTTSP.EXE in js-dos) exists only in `npm run dev`.
  plugins: [classicPlugin({ repoRoot: resolve(__dirname, '..'), base })],
  build: {
    rollupOptions: {
      // classic.html is deliberately not listed, so it is never built.
      input: {
        main: resolve(__dirname, 'index.html'),
        benchmark: resolve(__dirname, 'benchmark.html'),
      },
    },
  },
});
