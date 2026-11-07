import { defineConfig } from 'vite';

// GitHub Pages serves the site from https://morfrog.github.io/tsp-visualiser/
export default defineConfig({
  base: '/tsp-visualiser/',
  server: {
    // The sample problems are bundled from the repository's Data/ folder.
    fs: { allow: ['..'] },
  },
});
