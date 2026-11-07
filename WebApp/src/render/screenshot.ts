// Saves a picture of the screen, the web version of the original's V key
// (Legacy WriteScreen2File wrote raw pixels to VTTSP000.SCR, VTTSP001.SCR,
// ...). Here the visible screen is redrawn onto one canvas, from its map
// canvases, window borders and text, and offered as VTTSP000.PNG and so on.

let pictureNumber = 0;

/** The next picture's file name: VTTSP000.PNG, VTTSP001.PNG, ... */
export function nextPictureName(): string {
  return `VTTSP${String(pictureNumber++ % 1000).padStart(3, '0')}.PNG`;
}

/** For tests. */
export function resetPictureNumber(): void {
  pictureNumber = 0;
}

function visible(el: Element): boolean {
  const r = el.getBoundingClientRect();
  return r.width > 0 && r.height > 0 && getComputedStyle(el).visibility !== 'hidden';
}

/** Draws `root` as it appears now onto a new canvas. */
export function composeScreen(root: HTMLElement, scale = Math.max(2, window.devicePixelRatio || 1)): HTMLCanvasElement {
  const box = root.getBoundingClientRect();
  const out = document.createElement('canvas');
  out.width = Math.round(box.width * scale);
  out.height = Math.round(box.height * scale);
  const ctx = out.getContext('2d')!;
  ctx.scale(scale, scale);
  ctx.fillStyle = getComputedStyle(document.body).backgroundColor || '#000';
  ctx.fillRect(0, 0, box.width, box.height);

  const at = (r: DOMRect) => ({ x: r.left - box.left, y: r.top - box.top, w: r.width, h: r.height });

  // Backgrounds and borders of windows, buttons and panels.
  for (const el of root.querySelectorAll<HTMLElement>('*')) {
    // Skip what only screen readers see (and the hidden file input), and
    // controls marked as not part of the picture.
    if (!visible(el) || el.closest('.sr-only, [data-no-picture]')) continue;
    const style = getComputedStyle(el);
    const r = at(el.getBoundingClientRect());
    const bg = style.backgroundColor;
    if (bg && bg !== 'rgba(0, 0, 0, 0)' && bg !== 'transparent') {
      ctx.fillStyle = bg;
      ctx.fillRect(r.x, r.y, r.w, r.h);
    }
    const width = parseFloat(style.borderTopWidth);
    if (width > 0 && style.borderTopStyle !== 'none') {
      ctx.strokeStyle = style.borderTopColor;
      ctx.lineWidth = width;
      ctx.strokeRect(r.x + width / 2, r.y + width / 2, r.w - width, r.h - width);
    }
  }

  // Map canvases, as drawn.
  for (const canvas of root.querySelectorAll('canvas')) {
    if (!visible(canvas)) continue;
    const r = at(canvas.getBoundingClientRect());
    const style = getComputedStyle(canvas);
    const inset = parseFloat(style.borderTopWidth) || 0;
    ctx.drawImage(canvas, r.x + inset, r.y + inset, r.w - inset * 2, r.h - inset * 2);
  }

  // Text, one line box at a time, in its own font and colour.
  const walker = document.createTreeWalker(root, NodeFilter.SHOW_TEXT);
  for (let node = walker.nextNode(); node; node = walker.nextNode()) {
    const text = node.textContent ?? '';
    const parent = node.parentElement;
    if (!parent || !text.trim() || !visible(parent) || parent.closest('.sr-only, [data-no-picture]')) continue;
    const style = getComputedStyle(parent);
    ctx.font = `${style.fontWeight} ${style.fontSize} ${style.fontFamily}`;
    ctx.fillStyle = style.color;
    ctx.textBaseline = 'middle';
    const range = document.createRange();
    range.selectNodeContents(node);
    const rects = [...range.getClientRects()];
    if (rects.length === 1) {
      const r = at(rects[0]);
      ctx.fillText(text, r.x, r.y + r.h / 2);
    } else if (rects.length > 1) {
      // Wrapped text: draw each character where it landed.
      for (let i = 0; i < text.length; i++) {
        range.setStart(node, i);
        range.setEnd(node, i + 1);
        const rect = range.getClientRects()[0];
        if (rect && text[i] !== ' ') {
          const r = at(rect);
          ctx.fillText(text[i], r.x, r.y + r.h / 2);
        }
      }
    }
  }
  return out;
}

/** Composes the screen and offers it as the next VTTSPnnn.PNG download. */
export function savePicture(root: HTMLElement): void {
  const name = nextPictureName();
  composeScreen(root).toBlob((blob) => {
    if (!blob) return;
    const url = URL.createObjectURL(blob);
    const a = document.createElement('a');
    a.href = url;
    a.download = name;
    document.body.append(a);
    a.click();
    a.remove();
    setTimeout(() => URL.revokeObjectURL(url), 1000);
  }, 'image/png');
}
