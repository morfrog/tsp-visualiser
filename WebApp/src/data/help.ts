// The help files from Legacy/HLP, bundled at build time. Each file is a
// title line followed by up to nine lines of text (Legacy ReadChoiceHelp:
// ReadLines(in, 1) then ReadLines(in, 9)), shown in the main screen's help
// window when the item is chosen.

export interface HelpEntry {
  title: string;
  /** Up to nine lines. */
  lines: string[];
}

const files = import.meta.glob('../../../Legacy/HLP/*.HLP', { query: '?raw', import: 'default', eager: true }) as Record<
  string,
  string
>;

export function parseHelp(text: string): HelpEntry {
  const all = text.replace(/\r/g, '').split('\n');
  const lines = all.slice(1, 10);
  while (lines.length > 0 && lines[lines.length - 1].trim() === '') lines.pop();
  return { title: all[0] ?? '', lines };
}

const entries = new Map<string, HelpEntry>(
  Object.entries(files).map(([path, text]) => [path.split('/').pop()!.toUpperCase(), parseHelp(text)]),
);

/** Help for a file name such as "MCHEAP.HLP"; a stand-in if it is missing, as the original showed. */
export function help(file: string): HelpEntry {
  return (
    entries.get(file.toUpperCase()) ?? {
      title: file,
      lines: ['Is An Invalid File', 'Ensure it is in the HLP', 'subdirectory'],
    }
  );
}

export const HELP_FILES: readonly string[] = [...entries.keys()].sort();
