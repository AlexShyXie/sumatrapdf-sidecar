# Changelog

Single source of truth for release notes. Entries are bilingual: English first,
then Chinese.

Release flow: accumulate changes under `## Unreleased`, then rename that section
to `## <version>` in the commit you tag. Pushing tag `v<version>` triggers
`.github/workflows/release.yml`, which copies this section into the GitHub
release notes. A tag without a matching section fails the build.

## Unreleased

## v3.7-sidecar.5

### Annotation sidecar changes

- **Empty file attachments are now persisted.** A FileAttachment annotation   placed without a file attached ("empty paperclip") is saved to the sidecar   and restored after reopening — position, icon, contents and timestamps   are kept. Use *Attach File* in the annotation toolbar to add the file   later. Attachments whose embedded file exists but cannot be extracted are   still skipped and reported as `(N skipped)`, so real data loss is never   silently converted into an empty icon.
- **Markdown is now the only sidecar format.** Saving always produces a    `.md` file (Obsidian-style callouts), editable directly in Obsidian.    The `.md` format is no longer experimental.
- **Legacy `.json` sidecars: read-only fallback.** Existing `.json`    sidecars are still imported when opening a PDF, but are never written    again and are left untouched — the next save creates a sibling `.md`    file. No data is lost during the transition.
- **Removed setting:** `Annotations.separateSaveAsMd`. With Markdown as    the only format this switch has no effect. `Annotations.separateSave`    is unchanged and still controls whether the sidecar feature is enabled.  
- 
### 批注 sidecar 变更

- **空文件附件现在会被持久化。** 放下了 FileAttachment 注解但尚未挂文件的  「空回形针」会正常写入 sidecar 并在重开文档后还原——位置、图标、内容  与时间戳全部保留。之后可通过注解工具栏的 *Attach File* 随时补挂文件。  嵌入文件存在但读取失败的附件仍会跳过并计入 `(N skipped)`，避免把真实的  数据丢失静默保存成一个空图标。
- **Markdown 成为唯一的 sidecar 格式。** 保存一律产出 `.md` 文件  （Obsidian 风格 callout），可在 Obsidian 中直接编辑。  `.md` 格式不再是「实验性」。
- **旧 `.json` sidecar：只读回退。** 打开 PDF 时仍会导入已有的 `.json`  sidecar，但不再写出该格式，原文件保留不动——下次保存会生成同名的  `.md` 文件，迁移过程零数据丢失。
- **删除设置项：** `Annotations.separateSaveAsMd`。Markdown 成为唯一格式后该开关已无意义。`Annotations.separateSave` 保持不变，仍单独控制 sidecar 功能开关。


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
