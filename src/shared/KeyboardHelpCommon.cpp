/* Copyright 2026 the SumatraPDF project authors (see AUTHORS file).
   License: GPLv3 */

#include "base/Base.h"
#include "gui/Dpi.h"
#include "gui/UIModels.h"
#include "Settings.h"
#include "AppSettings.h"
#include "DocController.h"
#include "MainWindow.h"
#include "Accelerators.h"
#include "Translations.h"
#include "Commands.h"
#include "KeyboardHelp.h"
#include "KeyboardHelpCommon.h"

bool IsHelpListedCmd(int cmdId) {
    for (const KbSectionDef& definition : kSections) {
        for (const int* id = definition.commands; *id; id++) {
            if (*id == cmdId) {
                return true;
            }
        }
    }
    return false;
}

struct DefaultKeyboardHelpDataSource : KeyboardHelpDataSource {
    Str Translate(Str s) override { return s; }

    TempStr CommandDescriptionTemp(int cmdId) override { return str::DupTemp(GetCommandDescription(cmdId)); }

    TempStr CommandShortcutTemp(int cmdId, int) override {
        for (const auto& e : kFallbackShortcuts) {
            if (e.id == cmdId) {
                return str::DupTemp(Str(e.shortcut));
            }
        }
        return {};
    }
};

static DefaultKeyboardHelpDataSource gDefaultDataSource;

KeyboardHelpDataSource* GetDefaultKeyboardHelpDataSource() {
    return &gDefaultDataSource;
}
