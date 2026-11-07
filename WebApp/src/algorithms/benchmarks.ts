// Benchmark tables from Documentation/DOCO.DOC (1997), as ratios of the
// method's tour length to the optimal length in Data/10SOLNS.TXT, for the
// ten problems 30CITY_0.TSP .. 30CITY_9.TSP.
//
// Only methods that give the same result every run (SS in DOCO) can be
// checked: the Closest and Furthest starts with the six deterministic
// insertion rules, and the three iteration methods. Random start, the Random
// rule, Nearest Neighbour and annealing used clock-seeded random numbers.

import type { InsertionRule } from './insertion';

/** Column order of DOCO's insertion tables. */
export const INSERTION_TABLE_RULES: readonly InsertionRule[] = [
  'random',
  'cheapest',
  'dearest',
  'nearest',
  'furthest',
  'nearest2',
  'furthest2',
];

/** DOCO 2.2.3.2, Table for Closest Initialisation. Rows are 30CITY_0..9. */
export const DOCO_CLOSEST: readonly (readonly number[])[] = [
  [1.03576, 1.12789, 1.0144, 1.16921, 1.18019, 1.02021, 1.0144],
  [1.15038, 1.20081, 1.04158, 1.19206, 1.29712, 1.00447, 1.04158],
  [1.04313, 1.06058, 1.00856, 1.14947, 1.12885, 1.02818, 1.00856],
  [1.00501, 1.11204, 1.01714, 1.25344, 1.10207, 1.02163, 1.01714],
  [1.08704, 1.1121, 1.01566, 1.19927, 1.21973, 1.06153, 1.02636],
  [1.03818, 1.10865, 1.03679, 1.13531, 1.24948, 1.02778, 1.07377],
  [1.04444, 1.19678, 1.17089, 1.19678, 1.25348, 1.05422, 1.08568],
  [1.01093, 1.21041, 1.06764, 1.22047, 1.18839, 1.06299, 1.06668],
  [1.07159, 1.06456, 1.13347, 1.15981, 1.01254, 1.01499, 1.04544],
  [1.02574, 1.10003, 1.05545, 1.06517, 1.10865, 1.03236, 1.04904],
];

/** DOCO 2.2.3.3, Table for Furthest Initialisation. Rows are 30CITY_0..9. */
export const DOCO_FURTHEST: readonly (readonly number[])[] = [
  [1.07792, 1.04274, 1.02272, 1.17656, 1.11377, 1.02021, 1.0144],
  [1.07869, 1.04204, 1.03114, 1.04204, 1.10615, 1.02648, 1.04158],
  [1.03481, 1.1062, 1.00856, 1.02357, 1.12693, 1.02652, 1.00513],
  [1.02112, 1.05942, 1.01714, 1.05942, 1.13067, 1.03618, 1.01714],
  [1.02373, 1.08399, 1.01566, 1.10339, 1.10581, 1.01297, 1.02636],
  [1.03321, 1.06566, 1.03955, 1.06073, 1.11339, 1.07911, 1.07377],
  [1.06571, 1.07703, 1.0189, 1.05837, 1.08366, 1.0189, 1.08568],
  [1.00199, 1.08765, 1.00096, 1.04694, 1.08906, 1.0, 1.0],
  [1.11949, 1.03389, 1.02372, 1.02052, 1.0708, 1.019, 1.01254],
  [1.03129, 1.02175, 1.05886, 1.00772, 1.01974, 1.05222, 1.04904],
];

/** DOCO 3.2.1, Table Of Iteration Methods: 1-opt, 2-opt, 3-opt. Rows are 30CITY_0..9. */
export const DOCO_ITERATION: readonly (readonly number[])[] = [
  [1.01275, 1.03027, 1.0005],
  [1.1202, 1.0, 1.0],
  [1.12272, 1.00339, 1.0],
  [1.31058, 1.01112, 1.0],
  [1.35593, 1.01297, 1.0],
  [1.17676, 1.07681, 1.0212],
  [1.11552, 1.01894, 1.0],
  [1.02133, 1.00069, 1.0],
  [1.0, 1.0, 1.00369],
  [1.12983, 1.02536, 1.0],
];
