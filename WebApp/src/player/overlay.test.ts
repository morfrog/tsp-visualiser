import { describe, expect, it } from 'vitest';
import { initialOverlay, setOverlaySpeed, stepOverlay } from './overlay';

describe('overlay motion (Legacy ControlLCLOverlay)', () => {
  it('slides the halves together to the centre, then apart to the edges, and repeats', () => {
    const s = initialOverlay();
    expect(s.left).toBeLessThan(s.right);
    const events: string[] = [];
    for (let i = 0; i < 2000 && events.length < 4; i++) {
      const e = stepOverlay(s);
      if (e !== 'moving') events.push(`${e}@${s.left},${s.right}`);
    }
    expect(events.map((e) => e.split('@')[0])).toEqual(['centre', 'edge', 'centre', 'edge']);
    // At the centre the windows coincide.
    const [l, r] = events[0].split('@')[1].split(',').map(Number);
    expect(Math.abs(r - l)).toBeLessThanOrEqual(1);
  });

  it('digits change the step size but keep the direction', () => {
    const s = initialOverlay();
    setOverlaySpeed(s, 5);
    expect(s).toMatchObject({ factor: 5, rd: -5, ld: 5 });
    stepOverlay(s);
    expect(s.left).toBe(2 + 5);
  });
});
