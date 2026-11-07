// The 16 EGA colours the 1997 program drew with, by the names Legacy/SCREEN.H
// gives them. Frames refer to colours by these indexes; the renderer maps
// them to RGB with EGA_PALETTE.

export const Ega = {
  BLACK: 0,
  BLUE: 1,
  BLACK2: 2,
  BLUE2: 3,
  RED: 4,
  PURPLE: 5,
  ORANGE: 6,
  WHITE: 7,
  GREY: 8,
  LTBLUE: 9,
  GREEN: 10,
  CYAN: 11,
  LTRED: 12,
  PINK: 13,
  YELLOW: 14,
  LTWHITE: 15,
} as const;

export type EgaColor = (typeof Ega)[keyof typeof Ega];

/** Colour roles from Legacy/SCREEN.H. */
export const Role = {
  normal: Ega.RED,
  lineChange: Ega.BLUE,
  cityHighlight: Ega.YELLOW,
  cityIn: Ega.WHITE,
  cityOut: Ega.GREY,
  text: Ega.BLACK,
  line: Ega.LTRED,
  border: Ega.PURPLE,
  hilite1: Ega.BLUE,
  hilite2: Ega.YELLOW,
  hilite3: Ega.PINK,
} as const;

/**
 * RGB for each index: the standard EGA palette, with the VGA remapping that
 * InitScreen applied (indexes 1 and 3 become full blue, 2 becomes black).
 */
export const EGA_PALETTE: readonly string[] = [
  '#000000', // 0 BLACK
  '#0000ff', // 1 BLUE (remapped)
  '#000000', // 2 BLACK2 (remapped)
  '#0000ff', // 3 BLUE2 (remapped)
  '#aa0000', // 4 RED
  '#aa00aa', // 5 PURPLE
  '#aa5500', // 6 ORANGE
  '#aaaaaa', // 7 WHITE
  '#555555', // 8 GREY
  '#5555ff', // 9 LTBLUE
  '#55ff55', // 10 GREEN
  '#55ffff', // 11 CYAN
  '#ff5555', // 12 LTRED
  '#ff55ff', // 13 PINK
  '#ffff55', // 14 YELLOW
  '#ffffff', // 15 LTWHITE
];
