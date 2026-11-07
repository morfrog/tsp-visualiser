import { describe, expect, it } from 'vitest';
import { Ega, Role } from '../core/colors';
import type { ViewSnapshot } from '../run/stepper';
import { cityColors, edgeColors, toCanvas } from './mapView';

const view = (over: Partial<ViewSnapshot> = {}): ViewSnapshot => ({
  tour: [0, 1, 2, 3],
  closed: true,
  baseColors: [Ega.GREY, Ega.GREY, Ega.WHITE, Ega.WHITE],
  marks: [],
  edges: 'plain',
  lines: [],
  ...over,
});

describe('toCanvas', () => {
  const vp = { left: 10, top: 20, width: 110, height: 220 };

  it('maps the unit square with a 0.05 margin and y pointing down', () => {
    expect(toCanvas(vp, -0.05, -0.05)).toEqual([10, 20]);
    expect(toCanvas(vp, 1.05, 1.05)[0]).toBeCloseTo(120);
    expect(toCanvas(vp, 1.05, 1.05)[1]).toBeCloseTo(240);
    const [, top] = toCanvas(vp, 0.5, 0);
    const [, bottom] = toCanvas(vp, 0.5, 1);
    expect(top).toBeLessThan(bottom);
  });
});

describe('colours', () => {
  it('marks override the mask colour', () => {
    expect(cityColors(view({ marks: [{ city: 1, color: Ega.YELLOW }] }))).toEqual([
      Ega.GREY,
      Ega.YELLOW,
      Ega.WHITE,
      Ega.WHITE,
    ]);
  });

  it('plain edges are all LTRED; open tours skip the closing edge', () => {
    expect(edgeColors(view())).toEqual([Role.line, Role.line, Role.line, Role.line]);
    expect(edgeColors(view({ closed: false }))).toHaveLength(3);
  });

  it('mask edges take a shared end colour, otherwise LTRED (DrawMaskPath)', () => {
    const v = view({
      edges: 'mask',
      marks: [
        { city: 0, color: Ega.BLUE },
        { city: 1, color: Ega.BLUE },
        { city: 2, color: Ega.YELLOW },
        { city: 3, color: Ega.YELLOW },
      ],
    });
    expect(edgeColors(v)).toEqual([Ega.BLUE, Role.line, Ega.YELLOW, Role.line]);
  });
});
