import { describe, expect, it } from 'vitest';
import { AnsiCRandom, clockSeed, randomInt, RAND_MAX } from './random';

describe('AnsiCRandom', () => {
  it('matches the ANSI C sample rand() sequence for seed 1', () => {
    const rng = new AnsiCRandom(1);
    expect([rng.rand(), rng.rand(), rng.rand(), rng.rand()]).toEqual([16838, 5758, 10113, 17515]);
  });

  it('repeats for the same seed and stays in range', () => {
    const a = new AnsiCRandom(42);
    const b = new AnsiCRandom(42);
    for (let i = 0; i < 1000; i++) {
      const v = a.rand();
      expect(v).toBe(b.rand());
      expect(v).toBeGreaterThanOrEqual(0);
      expect(v).toBeLessThanOrEqual(RAND_MAX);
    }
  });

  it('randomInt gives rand() % n', () => {
    const rng = new AnsiCRandom(1);
    expect(randomInt(rng, 30)).toBe(16838 % 30);
  });

  it('clockSeed multiplies hundredths by seconds, like StartRandom', () => {
    expect(clockSeed(new Date(2026, 0, 1, 12, 0, 7, 250))).toBe(25 * 7);
    expect(clockSeed(new Date(2026, 0, 1, 12, 0, 0, 990))).toBe(0);
  });
});
