// Random numbers for the methods that give a different result each run (DS).
//
// The original seeded the C library's rand() from the clock (StartRandom in
// Legacy/SYS.C) and used rand() % n (Random). This uses the ANSI C sample
// rand() generator that most C libraries of the time shipped, seeded
// explicitly, so a run can be repeated by reusing its seed. 1997 runs cannot
// be reproduced exactly: their seeds came from the time of day.

export const RAND_MAX = 32767;

export interface Rng {
  /** Next integer in [0, RAND_MAX] (C rand()). */
  rand(): number;
}

export class AnsiCRandom implements Rng {
  private state: number;

  constructor(seed: number) {
    this.state = seed >>> 0;
  }

  rand(): number {
    this.state = (Math.imul(this.state, 1103515245) + 12345) >>> 0;
    return (this.state >>> 16) & RAND_MAX;
  }
}

/** Integer in [0, n) (Legacy Random). */
export function randomInt(rng: Rng, n: number): number {
  return rng.rand() % n;
}

/** Seed derived from the clock, as StartRandom did (hundredths x seconds). */
export function clockSeed(now: Date = new Date()): number {
  const hundredths = Math.floor(now.getMilliseconds() / 10);
  return (hundredths * now.getSeconds()) % RAND_MAX;
}
