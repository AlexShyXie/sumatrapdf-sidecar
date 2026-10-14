// Builds SumatraPDF ng under Fedora (from Windows: in the Fedora WSL distro)
// and copies it to /usr/local/bin, so that `SumatraPDF` runs it.
//
//   bun cmd/fedora-install.ts             # release build, then install
//   bun cmd/fedora-install.ts -dbg        # debug build
//   bun cmd/fedora-install.ts -clean      # clean build

import { fedoraMain } from "./helper/ng-fedora";

await fedoraMain("install");
