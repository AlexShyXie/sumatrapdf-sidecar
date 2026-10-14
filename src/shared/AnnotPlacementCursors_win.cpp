/* Copyright 2026 the SumatraPDF project authors (see AUTHORS file).
   License: GPLv3 */

#include "base/Base.h"
#include "base/Win.h"
#include "gui/Dpi.h"
#include "gui/UIModels.h"
#include "Settings.h"
#include "AppSettings.h"
#include "DocController.h"
#include "EngineBase.h"
#include "Theme.h"
#include "TextSelection.h"
#include "SumatraPDF.h"
#include "MainWindow.h"
#include "Toolbar.h"
#include "SvgIcons.h"
#include "AnnotPlacementCursors.h"

HCURSOR gCursorTextAnnotationPlacement = nullptr;

static int gCursorTextAnnotationPlacementDx = 0;

static int gCursorTextAnnotationPlacementDy = 0;

static Color gCursorTextAnnotationPlacementColor = 0;

HCURSOR gCursorInkAnnotationPlacement = nullptr;

static int gCursorInkAnnotationPlacementDx = 0;

static int gCursorInkAnnotationPlacementDy = 0;

static Color gCursorInkAnnotationPlacementColor = 0;

HCURSOR GetTextAnnotationPlacementCursor() {
    int dx = std::max(ToolbarIconSize(), DpiGetSystemMetrics(SM_CXCURSOR));
    int dy = std::max(ToolbarIconSize(), DpiGetSystemMetrics(SM_CYCURSOR));
    Color color = ThemeWindowTextColor();
    if (gCursorTextAnnotationPlacement && dx == gCursorTextAnnotationPlacementDx &&
        dy == gCursorTextAnnotationPlacementDy && color == gCursorTextAnnotationPlacementColor) {
        return gCursorTextAnnotationPlacement;
    }
    HCURSOR cursor = CreateSvgPlacementCursor(gIconAnnotText, dx, dy, color, 0, 0);
    if (!cursor) {
        return gCursorTextAnnotationPlacement;
    }
    if (gCursorTextAnnotationPlacement) {
        DestroyCursor(gCursorTextAnnotationPlacement);
    }
    gCursorTextAnnotationPlacement = cursor;
    gCursorTextAnnotationPlacementDx = dx;
    gCursorTextAnnotationPlacementDy = dy;
    gCursorTextAnnotationPlacementColor = color;
    return gCursorTextAnnotationPlacement;
}

HCURSOR GetInkAnnotationPlacementCursor() {
    int dx = std::max(ToolbarIconSize(), DpiGetSystemMetrics(SM_CXCURSOR));
    int dy = std::max(ToolbarIconSize(), DpiGetSystemMetrics(SM_CYCURSOR));
    Color color = ThemeWindowTextColor();
    if (gCursorInkAnnotationPlacement && dx == gCursorInkAnnotationPlacementDx &&
        dy == gCursorInkAnnotationPlacementDy && color == gCursorInkAnnotationPlacementColor) {
        return gCursorInkAnnotationPlacement;
    }
    DWORD hotspotX = (DWORD)((4 * dx) / 24);
    DWORD hotspotY = (DWORD)((20 * dy) / 24);
    HCURSOR cursor = CreateSvgPlacementCursor(gIconEditAnnotations, dx, dy, color, hotspotX, hotspotY);
    if (!cursor) {
        return gCursorInkAnnotationPlacement;
    }
    if (gCursorInkAnnotationPlacement) {
        DestroyCursor(gCursorInkAnnotationPlacement);
    }
    gCursorInkAnnotationPlacement = cursor;
    gCursorInkAnnotationPlacementDx = dx;
    gCursorInkAnnotationPlacementDy = dy;
    gCursorInkAnnotationPlacementColor = color;
    return cursor;
}

void DeleteAnnotationPlacementCursors() {
    if (gCursorTextAnnotationPlacement) {
        DestroyCursor(gCursorTextAnnotationPlacement);
    }
    gCursorTextAnnotationPlacement = nullptr;
    gCursorTextAnnotationPlacementDx = 0;
    gCursorTextAnnotationPlacementDy = 0;
    gCursorTextAnnotationPlacementColor = 0;

    if (gCursorInkAnnotationPlacement) {
        DestroyCursor(gCursorInkAnnotationPlacement);
    }
    gCursorInkAnnotationPlacement = nullptr;
    gCursorInkAnnotationPlacementDx = 0;
    gCursorInkAnnotationPlacementDy = 0;
    gCursorInkAnnotationPlacementColor = 0;
}
