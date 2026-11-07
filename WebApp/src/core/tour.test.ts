import { describe, expect, it } from 'vitest';
import { DistanceMatrix } from './distances';
import { Tour } from './tour';

describe('Tour basics', () => {
  it('starts from the head and wraps around', () => {
    const t = new Tour([4, 2, 7]);
    expect(t.size).toBe(3);
    expect(t.at(0)).toBe(4);
    expect(t.at(3)).toBe(4);
    expect(t.at(-1)).toBe(7);
    expect(t.next(7)).toBe(4);
    expect(t.prev(4)).toBe(7);
    expect([...t.edges()]).toEqual([
      [4, 2],
      [2, 7],
      [7, 4],
    ]);
    expect(() => t.next(9)).toThrow(RangeError);
  });

  it('identity is the .TSP file order', () => {
    expect(Tour.identity(4).toArray()).toEqual([0, 1, 2, 3]);
  });

  it('clones independently and compares by order from the head', () => {
    const a = new Tour([0, 1, 2]);
    const b = a.clone();
    b.append(3);
    expect(a.toArray()).toEqual([0, 1, 2]);
    expect(a.equals(new Tour([0, 1, 2]))).toBe(true);
    expect(a.equals(new Tour([1, 2, 0]))).toBe(false);
  });
});

describe('insertBetween (Legacy InsertPath)', () => {
  it('inserts after the first node of the matching edge, in either direction', () => {
    const t = new Tour([0, 1, 2]);
    expect(t.insertBetween(9, 1, 2)).toBe(true);
    expect(t.toArray()).toEqual([0, 1, 9, 2]);
    expect(t.insertBetween(8, 9, 1)).toBe(true);
    expect(t.toArray()).toEqual([0, 1, 8, 9, 2]);
  });

  it('inserting on the edge back to the head appends, keeping the head', () => {
    const t = new Tour([0, 1, 2]);
    expect(t.insertBetween(5, 0, 2)).toBe(true);
    expect(t.toArray()).toEqual([0, 1, 2, 5]);
  });

  it('fails if the cities are not adjacent', () => {
    const t = new Tour([0, 1, 2, 3]);
    expect(t.insertBetween(5, 0, 2)).toBe(false);
    expect(t.toArray()).toEqual([0, 1, 2, 3]);
  });

  it('on a two-city tour picks the first edge from the head', () => {
    const t = new Tour([3, 6]);
    t.insertBetween(1, 6, 3);
    expect(t.toArray()).toEqual([3, 1, 6]);
  });
});

describe('remove, removeTemporarily and restore', () => {
  it('removing the head makes the next city the head', () => {
    const t = new Tour([0, 1, 2]);
    expect(t.remove(0)).toBe(true);
    expect(t.toArray()).toEqual([1, 2]);
    expect(t.remove(9)).toBe(false);
  });

  it.each([0, 1, 3])('restores city at position %i exactly', (pos) => {
    const t = new Tour([10, 11, 12, 13]);
    const city = t.at(pos);
    const removed = t.removeTemporarily(city)!;
    expect(t.has(city)).toBe(false);
    expect(t.restore(removed)).toBe(true);
    expect(t.toArray()).toEqual([10, 11, 12, 13]);
  });

  it('reports the neighbours, like TmpPathDel', () => {
    const t = new Tour([10, 11, 12]);
    expect(t.removeTemporarily(10)).toEqual({ city: 10, prev: 12, next: 11, wasHead: true });
    expect(t.removeTemporarily(10)).toBeNull();
  });

  it('restores relative to its neighbours even if other cities moved', () => {
    const t = new Tour([0, 1, 2, 3]);
    const removed = t.removeTemporarily(2)!;
    t.remove(0);
    expect(t.restore(removed)).toBe(true);
    expect(t.toArray()).toEqual([1, 2, 3]);
  });

  it('refuses to restore if the neighbours are no longer adjacent', () => {
    const t = new Tour([0, 1, 2, 3]);
    const removed = t.removeTemporarily(1)!;
    t.insertBetween(9, 0, 2);
    expect(t.restore(removed)).toBe(false);
  });
});

describe('reversals (Legacy Reversal and ReverseSubPath)', () => {
  it('reverses a range in place', () => {
    const t = new Tour([0, 1, 2, 3, 4, 5]);
    t.reverseRange(1, 4);
    expect(t.toArray()).toEqual([0, 4, 3, 2, 1, 5]);
  });

  it('reverses a range that wraps past the end, swapping city numbers', () => {
    const t = new Tour([0, 1, 2, 3, 4, 5]);
    t.reverseRange(4, 1); // nodes 4,5,0,1 hold 4,5,0,1 -> 1,0,5,4
    expect(t.toArray()).toEqual([5, 4, 2, 3, 1, 0]);
  });

  it('reverseSubPath works with the section in either direction', () => {
    const forward = new Tour([0, 1, 2, 3, 4, 5]);
    expect(forward.reverseSubPath(0, 1, 3, 4)).toBe(true);
    expect(forward.toArray()).toEqual([0, 3, 2, 1, 4, 5]);

    const backward = new Tour([0, 1, 2, 3, 4, 5]);
    expect(backward.reverseSubPath(4, 3, 1, 0)).toBe(true);
    expect(backward.toArray()).toEqual([0, 3, 2, 1, 4, 5]);
  });

  it('reverseSubPath refuses a layout that does not match', () => {
    const t = new Tour([0, 1, 2, 3, 4, 5]);
    expect(t.reverseSubPath(5, 1, 3, 4)).toBe(false);
    expect(t.toArray()).toEqual([0, 1, 2, 3, 4, 5]);
  });

  it('tour length reflects a reversal that crosses edges', () => {
    const cities = [
      { x: 0, y: 0 },
      { x: 1, y: 0 },
      { x: 1, y: 1 },
      { x: 0, y: 1 },
    ];
    const d = new DistanceMatrix(cities);
    const t = Tour.identity(4);
    expect(t.length(d)).toBeCloseTo(4);
    t.reverseRange(1, 2); // 0,2,1,3: crosses the square's diagonals
    expect(t.length(d)).toBeCloseTo(2 + 2 * Math.SQRT2);
  });
});
