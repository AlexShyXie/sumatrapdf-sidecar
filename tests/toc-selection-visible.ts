// The bookmark of the current page is selected before the Bookmarks pane has a
// size. It must be in view once the pane is shown; it used to sit one row
// above the top, so a document opened on page 1 hid its first bookmark.
//
// Run: bun tests/toc-selection-visible.ts [--no-build]

import { mkdirSync, rmSync, writeFileSync } from "node:fs";
import { join } from "node:path";
import { ControlCommand } from "./control";
import { assemblePdf, runStandalone, tmpPath, USE_NG } from "./util";
import { launchControlled, killAndWait } from "./win-automation";
import { findChildWindow, sleep, treeGetSelection, treeIsItemInView } from "./winapi";

const N_CHAPTERS = 40;
const N_SECTIONS = 3;

// one page per chapter; every chapter has expanded sections
function makePdf(): string {
  const pageObj = (i: number) => 3 + i;
  const outlines = 3 + N_CHAPTERS;
  const chapterObj = (i: number) => outlines + 1 + i * (1 + N_SECTIONS);
  const sectionObj = (i: number, k: number) => chapterObj(i) + 1 + k;

  const objs: string[] = [];
  objs.push(`<< /Type /Catalog /Pages 2 0 R /Outlines ${outlines} 0 R /PageMode /UseOutlines >>`);
  const kids = Array.from({ length: N_CHAPTERS }, (_, i) => `${pageObj(i)} 0 R`).join(" ");
  objs.push(`<< /Type /Pages /Count ${N_CHAPTERS} /Kids [${kids}] >>`);
  for (let i = 0; i < N_CHAPTERS; i++) {
    objs.push(`<< /Type /Page /Parent 2 0 R /MediaBox [0 0 612 792] >>`);
  }
  const first = chapterObj(0);
  const last = chapterObj(N_CHAPTERS - 1);
  objs.push(`<< /Type /Outlines /Count ${N_CHAPTERS * (1 + N_SECTIONS)} /First ${first} 0 R /Last ${last} 0 R >>`);
  for (let i = 0; i < N_CHAPTERS; i++) {
    const dest = `/Dest [${pageObj(i)} 0 R /XYZ null null null]`;
    let ch = `<< /Title (Chapter ${i + 1}) /Parent ${outlines} 0 R ${dest}`;
    if (i > 0) {
      ch += ` /Prev ${chapterObj(i - 1)} 0 R`;
    }
    if (i < N_CHAPTERS - 1) {
      ch += ` /Next ${chapterObj(i + 1)} 0 R`;
    }
    ch += ` /First ${sectionObj(i, 0)} 0 R /Last ${sectionObj(i, N_SECTIONS - 1)} 0 R /Count ${N_SECTIONS} >>`;
    objs.push(ch);
    for (let k = 0; k < N_SECTIONS; k++) {
      let sec = `<< /Title (Section ${i + 1}.${k + 1}) /Parent ${chapterObj(i)} 0 R ${dest}`;
      if (k > 0) {
        sec += ` /Prev ${sectionObj(i, k - 1)} 0 R`;
      }
      if (k < N_SECTIONS - 1) {
        sec += ` /Next ${sectionObj(i, k + 1)} 0 R`;
      }
      objs.push(sec + ` >>`);
    }
  }
  return assemblePdf(objs);
}

async function checkOpenedAt(pdf: string, appdata: string, pageNo: number): Promise<void> {
  const { proc, client, frame } = await launchControlled(["-appdata", appdata, "-page", String(pageNo), pdf]);
  try {
    await client.waitForRenderIdle();

    const deadline = Date.now() + 4000;
    let state = "bookmarks tree did not appear";
    while (Date.now() < deadline) {
      if (USE_NG) {
        state = String((await client.request(ControlCommand.TestUiState, ["count"]))[1] ?? "");
        if (/tocSelShown=1/.test(state)) {
          return;
        }
      } else {
        const tree = findChildWindow(frame, "SysTreeView32");
        const sel = tree ? treeGetSelection(tree) : 0n;
        if (sel !== 0n) {
          state = "selected bookmark is scrolled out of view";
          if (treeIsItemInView(tree, sel)) {
            return;
          }
        }
      }
      await sleep(40);
    }
    throw new Error(`toc-selection-visible: page ${pageNo}: ${state}`);
  } finally {
    client.close();
    await killAndWait(proc);
  }
}

export async function testit(): Promise<void> {
  const pdf = tmpPath("toc-selection-visible.pdf");
  writeFileSync(pdf, makePdf(), "latin1");

  const appdata = tmpPath("toc-selection-visible-appdata");
  rmSync(appdata, { recursive: true, force: true });
  mkdirSync(appdata, { recursive: true });
  writeFileSync(
    join(appdata, "SumatraPDF-settings.txt"),
    "ShowToc = true\nShowFavorites = false\nRestoreSession = false\nCheckForUpdates = false\n",
  );

  // page 1 selects the first bookmark, the middle page one that needs a scroll
  await checkOpenedAt(pdf, appdata, 1);
  await checkOpenedAt(pdf, appdata, N_CHAPTERS / 2);
}

if (import.meta.main) {
  await runStandalone(testit);
}
