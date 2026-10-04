# Changelog

Single source of truth for release notes. Entries are bilingual: English first,
then Chinese.

Release flow: accumulate changes under `## Unreleased`, then rename that section
to `## <version>` in the commit you tag. Pushing tag `v<version>` triggers
`.github/workflows/release.yml`, which copies this section into the GitHub
release notes. A tag without a matching section fails the build.

## Unreleased

## v3.7-sidecar.4

### External-change sync hardened

- **External changes now sync while the document is open.** A 2-second poll picks up edits made to the sidecar file (JSON or Markdown) and reloads them live, with a toast summarizing what changed. Polling pauses while you're editing (annotation selected, popup open, mouse down) so it never interferes.
- **Ctrl+S is merge-aware.** If the file changed since the last sync, saving reloads first, then writes the merged session back in one go.
- **Malformed callouts no longer break the file.** A half-deleted callout is skipped with a log entry; the rest import normally.
- **Typora round-trip fix.** Copy-pasting a callout in Typora's rendered view wraps `[[...]]` values in an invisible `<a>` anchor; import now strips it and the file self-heals on next save. (Copy from source-code mode to avoid it.)

### New: export annotations to PDF

- `Ctrl+K` → "Save Annotations to a new PDF...": writes the whole session — including sidecar annotations — into a standalone PDF copy. The original PDF and sidecar are untouched. Saving onto the original file name is supported (reopening won't duplicate annotations).


### 外部变更同步加固

- **文档开着也能同步外部改动了。** 2 秒轮询监测 sidecar 文件（JSON 或 Markdown），有变化即实时重载并弹提示。编辑期间（选中批注、弹窗、按住鼠标）轮询自动暂停，互不干扰。
- **Ctrl+S 会合并。** 文件与上次同步点不一致时，先重载外部变更再一并写回，单次完成。
- **残缺 callout 不再拖垮整个文件。** 删一半的 callout 跳过并记日志，其余正常导入。
- **Typora 粘贴容错。** 渲染视图里复制 callout 会给 `[[...]]` 值套上看不见的 `<a>` 标签，导入时自动剥除，下次保存自愈。（从源代码模式复制可避免。）

### 新增：批注导出为 PDF

- `Ctrl+K` → "Save Annotations to a new PDF..."：把当前会话的全部批注（含 sidecar 批注）写进一份独立 PDF 副本。原 PDF 和 sidecar 不动。也支持写回原文件名（重开不会出现双份批注）。


## 3.7-sidecar.3 - 2026-10-03

### Sidecar: external change detection

- Automatically detects external modifications to sidecar files (OneDrive sync, other viewers) via a 2-second fingerprint poll and reloads them into the open document.
- Local unsaved annotations are preserved across reloads; saving never blindly overwrites external edits (fingerprint re-checked before write).
- Change notifications now show accurate added / removed / updated counts, file name instead of full path, and the save location (central folder / next to PDF). No-op changes (e.g. OneDrive touching mtime) stay silent.

### Sidecar：外部变更检测

- 通过 2 秒指纹轮询自动检测 sidecar 文件的外部修改（OneDrive 同步、其他阅读器），并自动重载到当前文档。
- 重载时保留本地未保存的批注；保存前重新校验指纹，绝不盲目覆盖外部编辑。
- 通知改进：准确显示新增 / 删除 / 更新的批注数量，显示文件名而非完整路径，并注明保存位置（中央文件夹 / PDF 旁）。无实质变化的变更（如 OneDrive 仅触碰修改时间）不再弹窗。
