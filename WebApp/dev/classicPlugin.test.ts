import { mkdirSync, mkdtempSync, rmSync, writeFileSync } from 'node:fs';
import { tmpdir } from 'node:os';
import { join } from 'node:path';
import { strFromU8, unzipSync } from 'fflate';
import { afterEach, describe, expect, it } from 'vitest';
import { buildBundle, classicPlugin, originalStatus } from './classicPlugin';

let dirs: string[] = [];
function tempRepo(): string {
  const dir = mkdtempSync(join(tmpdir(), 'vttsp-classic-'));
  dirs.push(dir);
  return dir;
}
afterEach(() => {
  for (const d of dirs) rmSync(d, { recursive: true, force: true });
  dirs = [];
});

describe('originalStatus', () => {
  it('reports what is missing, relative to the repository', () => {
    const repo = tempRepo();
    expect(originalStatus(join(repo, 'Temp/Tsp/PACK'), repo)).toEqual({
      available: false,
      location: 'Temp/Tsp/PACK',
      missing: ['VTTSP.EXE', 'DOS4GW.EXE'],
    });
  });

  it('is available once both programs are present, in any case', () => {
    const repo = tempRepo();
    const pack = join(repo, 'Temp/Tsp/PACK');
    mkdirSync(pack, { recursive: true });
    writeFileSync(join(pack, 'vttsp.exe'), 'MZ');
    writeFileSync(join(pack, 'DOS4GW.EXE'), 'MZ');
    expect(originalStatus(pack, repo)).toMatchObject({ available: true, missing: [] });
  });
});

describe('buildBundle', () => {
  it('packs every file, upper-cased as DOS saw them, with DOSBox settings that start VTTSP', () => {
    const repo = tempRepo();
    const pack = join(repo, 'pack');
    mkdirSync(join(pack, 'hlp'), { recursive: true });
    writeFileSync(join(pack, 'VTTSP.EXE'), 'MZ1');
    writeFileSync(join(pack, 'hlp', 'vttsp.hlp'), 'help');
    const files = unzipSync(buildBundle(pack));
    expect(Object.keys(files).sort()).toEqual(['.jsdos/dosbox.conf', 'HLP/VTTSP.HLP', 'VTTSP.EXE']);
    expect(strFromU8(files['.jsdos/dosbox.conf'])).toMatch(/\[autoexec\][\s\S]*mount c \.[\s\S]*VTTSP\.EXE/);
  });
});

describe('classicPlugin', () => {
  it('only applies to the dev server, never to builds', () => {
    expect(classicPlugin({ repoRoot: tempRepo(), base: '/tsp-visualiser/' }).apply).toBe('serve');
  });
});
