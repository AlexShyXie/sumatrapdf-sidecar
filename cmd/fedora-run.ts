// Builds SumatraPDF ng under Fedora (from Windows: in the Fedora WSL distro)
// and runs it. Installs the packages and bun the build needs.
//
//   bun cmd/fedora-run.ts                 # release build, then run
//   bun cmd/fedora-run.ts -dbg            # debug build
//   bun cmd/fedora-run.ts -clean          # clean build
//   bun cmd/fedora-run.ts -- file.pdf     # args for SumatraPDF

import { fedoraMain } from "./helper/ng-fedora";

await fedoraMain("run");
