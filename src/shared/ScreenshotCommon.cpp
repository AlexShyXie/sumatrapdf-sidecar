/* Copyright 2022 the SumatraPDF project authors (see AUTHORS file).
   License: GPLv3 */

#include "base/Base.h"
#if defined(SUMATRA_NG)
#include "VirtKeys.h"
#endif
#include "base/File.h"
#include "gui/UIModels.h"
#include "Notifications.h"
#include "AppTools.h"
#include "ShortcutParse.h"
#include "Theme.h"
#include "SumatraConfig.h"
#include "Commands.h"
#include "Accelerators.h"
#include "Settings.h"
#include "AppSettings.h"
#include "MainWindow.h"
#include "SumatraPDF.h"
#include "Translations.h"
#include "Screenshot.h"
#include "AppHelpersCommon.h"
#include "ScreenshotCommon.h"

// find custom shortcut key string for CmdScreenshot, or empty if none
Str FindScreenshotShortcut() {
    // check gSettings->shortcuts first (may have been updated at runtime)
    for (Shortcut* sc : *gSettings->shortcuts) {
        if (str::EqI(sc->cmd, StrL("CmdScreenshot")) && len(sc->key) > 0) {
            return sc->key;
        }
    }
    // fall back to custom commands (built at startup)
    auto* curr = gFirstCustomCommand;
    while (curr) {
        if (curr->origId == CmdScreenshot && len(curr->key) > 0) {
            return curr->key;
        }
        curr = curr->next;
    }
    return {};
}

// serialize VK code + modifiers to a shortcut string like "Ctrl+Shift+F5"
TempStr SerializeHotkeyTemp(uint vk, bool ctrl, bool shift, bool alt, bool altGr) {
    str::Builder s;
    if (altGr) {
        s.Append(StrL("AltGr+"));
    } else {
        if (ctrl) {
            s.Append(StrL("Ctrl+"));
        }
        if (alt) {
            s.Append(StrL("Alt+"));
        }
    }
    if (shift) {
        s.Append(StrL("Shift+"));
    }
    bool isAlphaNumKey = (vk >= 'A' && vk <= 'Z') || (vk >= '0' && vk <= '9');
    if (vk >= VK_F1 && vk <= VK_F24) {
        s.Append(fmt("F%d", (int)(vk - VK_F1 + 1)));
    } else if (isAlphaNumKey) {
        s.AppendChar((char)vk);
    } else if (vk == VK_SNAPSHOT) {
        s.Append(StrL("PrtSc"));
    } else if (vk == VK_RETURN) {
        s.Append(StrL("Return"));
    } else if (vk == VK_LEFT) {
        s.Append(StrL("Left"));
    } else if (vk == VK_RIGHT) {
        s.Append(StrL("Right"));
    } else if (vk == VK_UP) {
        s.Append(StrL("Up"));
    } else if (vk == VK_DOWN) {
        s.Append(StrL("Down"));
    } else if (vk == VK_DELETE) {
        s.Append(StrL("Delete"));
    } else if (vk == VK_INSERT) {
        s.Append(StrL("Insert"));
    } else if (vk == VK_HOME) {
        s.Append(StrL("Home"));
    } else if (vk == VK_END) {
        s.Append(StrL("End"));
    } else if (vk == VK_PRIOR) {
        s.Append(StrL("PageUp"));
    } else if (vk == VK_NEXT) {
        s.Append(StrL("PageDown"));
    } else if (vk == VK_SPACE) {
        s.Append(StrL("Space"));
    } else if (vk == VK_PAUSE) {
        s.Append(StrL("Pause"));
    } else if (vk == VK_SCROLL) {
        s.Append(StrL("ScrollLock"));
    } else if (vk >= VK_NUMPAD0 && vk <= VK_NUMPAD9) {
        s.Append(fmt("Numpad%d", (int)(vk - VK_NUMPAD0)));
    } else {
        // unknown key
        return {};
    }
    return ToStrTemp(s);
}
