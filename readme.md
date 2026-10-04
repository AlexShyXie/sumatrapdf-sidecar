[![Build](https://github.com/AlexShyXie/sumatrapdf-sidecar/actions/workflows/build.yml/badge.svg?branch=add_sidecar)](https://github.com/sumatrapdfreader/sumatrapdf/actions/workflows/build.yml)

## SumatraPDF Reader with sidecar

SumatraPDF is a multi-format (PDF, EPUB, MOBI, CBZ, CBR, FB2, CHM, XPS, DjVu) reader
for Windows under (A)GPLv3 license, with some code under BSD license (see
AUTHORS).

More Information:
* [Website](https://www.sumatrapdfreader.org/free-pdf-reader)
* [Manual](https://www.sumatrapdfreader.org/manual)
* [Developer Information](https://www.sumatrapdfreader.org/docs/Contribute-to-SumatraPDF)



[简体中文](readme-zh.md) | English
A modified version of SumatraPDF: annotations are not written into the PDF, but saved to a JSON file stored alongside it.

Upstream repository: [sumatrapdfreader/SumatraPDF](https://github.com/sumatrapdfreader/SumatraPDF). This repository is forked from the official master, with changes concentrated in `src/Sidecar.cpp` (a new file) and 6 mounting points marked with `// SIDECAR:` comments.

A PDF annotation sidecar (companion file) means annotation data is not written into the PDF itself, but saved as a separate file with the same name in the same directory (e.g., `book.pdf` → `book.json`). When opening the PDF, the sidecar file is loaded automatically; when editing annotations, only this small (a few KB) file is rewritten, leaving the PDF untouched — especially useful for large PDFs stored on sync services like OneDrive, since a single annotation edit won't trigger a full re-upload of a 600 MB file. The trade-off: other PDF readers won't show the annotations, because the data isn't inside the PDF.

## Why
> **SumatraPDF** originally supported saving annotations as a same-named `.smx` plain-text sidecar, last supported in version **3.2**;
> **Okular** early on stored annotations in a hidden XML file, last supported in version **1.2**.
> Both projects actively removed this feature around 2018 and explicitly stated it would not return. The core reasons for dropping it were threefold: sidecar files easily become disconnected after renaming or "Save As", creating a perceived "data loss"; the plain-text format couldn't carry complex annotation types; and inconsistent coordinate units across formats made maintenance costly. The maturing of embedded annotations in standard PDF ultimately made this standalone storage approach obsolete.
The industry consensus is that annotations should travel with the file, but for some scenarios this is precisely a disaster: my PDF library contains hundreds-of-megabytes single-file scanned documents, synced on OneDrive. Once SumatraPDF's annotation feature is enabled, drawing a single line rewrites the entire PDF — a 600 MB file, and OneDrive has to re-upload the whole thing even for a one-byte change; disabling annotations means giving up half the value of the reader.
That's why I prefer the sidecar approach: open a PDF with annotations and the PDF file itself is untouched — its modification time stays at zero. All annotation data lives in the adjacent `.json` file.

## How It Works
- **Import**: When opening a PDF, if a `.json` file with the same name exists in the same directory (e.g., `book.pdf` → `book.json`), annotations are loaded automatically without touching the PDF itself.
- **Auto-save**: After annotation changes, a 2-second debounce writes to disk — JSON only.
- **Ctrl+S**: With sidecar enabled, Ctrl+S saves the JSON and no longer pops up "Save As copy". If external changes are detected before saving, it first reloads and merges, then writes (notice: "External changes detected before saving, reloading first").
- **External change detection**: While the document is open, if the sidecar is modified externally (Typora, another device syncing in), a 2-second polling detects it, reloads automatically, and shows a notice.
- **Central folder mode** (optional): After setting `Annotations.centralFolder`, all sidecars are stored in one place, named as `parentFolderName/fileName.json`. Suitable for libraries spread across multiple folders, or when you want to sync your annotation library separately.
Settings (AdvancedSettings):

| Setting | Default | Description |
| --------------------------- | ------- | ----------------------------------------- |
| `Annotations.separateSave` | false | Enable the sidecar feature |
| `Annotations.centralFolder` | (empty) | Central directory for storing annotations |
| `Annotations.separateSaveAsMd` | false | Save sidecar as Markdown instead of JSON (**experimental**, see below) |

```ini
Annotations [
    ....
    SeparateSave = true
    CentralFolder = E:\Downloads\Claw
    SeparateSaveAsMd = true
]
```

## Supported Annotation Types
16 types, with full round-trip property support:
Text (sticky note), FreeText (text box), Highlight, Underline, Squiggly, StrikeOut, Line, Square, Circle, Polygon, PolyLine, Ink, Caret, Redact, Stamp, FileAttachment
FreeText supports font, font size, text color, alignment, bold, italic, underline, and transparent background. For all types, colors (including none/transparent), opacity, borders, author, and timestamps are preserved.
Stamp and FileAttachment carry binary payloads (images, embedded files), which the JSON never holds: payloads are written to an `assets/` folder next to the JSON, named by content hash (`assets/<hash>.<ext>`) and referenced by relative path. Identical payloads are deduplicated, and asset files no longer referenced by any sidecar in the folder are deleted on save. Sidecar files from the previous format (no asset fields) still load.

## Export Annotations to PDF
Other readers can't see the annotations, but there's an exit: **Ctrl+K opens the command palette → "Save Annotations to a new PDF..."**.

All annotations of the current session (those imported from the sidecar plus ones created this session) are injected into an independent PDF copy, readable by any viewer. The original PDF and sidecar remain untouched; after exporting, keep using the original document as usual.

## Markdown sidecar (experimental 🧪)
With `SeparateSaveAsMd = true`, annotations are saved as `book.md` instead of `book.json`: each annotation is an Obsidian callout, editable with any Markdown editor.

```markdown
---
sumatrapdf_sidecar: 2
generator: SumatraPDF-sidecar/2-md
file: book.pdf
---
# mybooknote
> [!Note]
> type: highlight
> page: 11
> rect: [58.4,695.2,299.6,708.4]
> quads: [[58.4,708.4,299.6,695.2,58.4,695.2,299.6,708.4]]
> text: highlighted words
> contents: highlighted words
> author: AlexShy
```

Rules:
- **Everything outside callouts is yours.** Headings, prose, ordinary quote blocks — SumatraPDF preserves them verbatim on read/write, touching only the machine lines in `key: value` form inside callouts. Annotation data and annotation notes now live in the same file.
- **The `contents` line is always present.** When there's no annotation note, it is filled with the highlighted text; when both are empty it's `contents: ""` — fill it in by hand in Obsidian, and it takes effect when the document is reopened.
- **Two-way sync, in real time.** Editing the md while the document is open is the intended workflow: within 2 seconds after saving, it reloads automatically and shows a notice — externally added: "Loaded N annotation(s)"; externally deleted: "Removed N annotation(s) deleted externally"; modified or mixed: "Reloaded N annotation(s)". Annotations newly created within the debounce window but not yet written to disk are preserved (notice tail: ", N local unsaved kept"), then written back. Hand-writing a valid callout (not recommended — coordinates are hard to compute precisely) also becomes a real annotation.
- **Automatic migration.** When enabled, `.md` is read first, falling back to `.json` if absent; the next save writes `.md`, and the old `.json` is left untouched and no longer updated.
- **Atomic writes.** The md is mixed with your notes, so saving goes through a temp file + replace — a mid-save crash won't corrupt the file.
- **Tolerance for hand edits.** If a callout is half-deleted leaving stray machine lines: that entry is skipped, the rest imports normally, and the stray lines stay in the file for you to clean up by hand. Copy-pasting a callout in Typora's rendered view wraps `[[...]]` values in invisible `<a>` tags (wiki-link conversion, invisible even in source mode): these are stripped automatically on import, and the file self-heals on the next save; to avoid it entirely, copy from source mode.
- **Querying**: front matter is YAML; `key:: value` double-colon syntax inside callouts is compatible with Dataview.
⚠️ **Experimental notice**: the format may still change (a version field is included to keep old files readable); validation in single-user scenarios is limited.

## Known Limitations
- **No conflict merging.** If two machines modify the same sidecar simultaneously, the last save overwrites the earlier one. Fine for single-user use.

- **Editing the md while the document is open: file wins on conflict.** If the same annotation is changed on both sides within the debounce window (2 seconds by default), the reload takes the file version and local changes are silently dropped. How to avoid: wait for the debounce flush before editing the md, or after editing the md, wait for the notice before touching annotations in the app.

- Large annotations have defensive limits: 512 vertices for polygons/polylines, 64 strokes for ink, with at most 2048 points per stroke. Anything beyond these limits is truncated.

- Other readers won't see the annotations when opening this PDF — the data lives in the JSON, not in the PDF. This is by design (use the command palette to export when sharing is needed).

  
## License
GPLv3, same as upstream.
