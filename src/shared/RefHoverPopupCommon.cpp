/* Copyright 2026 the SumatraPDF project authors (see AUTHORS file).
   License: GPLv3 */

#include "base/Base.h"
#include "base/Pixmap.h"
#include "gui/Dpi.h"
#include "gui/UIModels.h"
#include "DocController.h"
#include "EngineBase.h"
#include "Settings.h"
#include "DisplayMode.h"
#include "DisplayModel.h"
#include "RefHover.h"
#include "RefHoverPopupCommon.h"

static bool PopupClientToPagePt(RefHoverState* s, int clientX, int clientY, PointF& ptOut) {
    if (!s || !s->hitEngine || s->displayed.destPage <= 0) {
        return false;
    }
    float zoom = s->displayed.baseZoom * s->displayed.userZoom;
    if (zoom <= 0.f) {
        return false;
    }
    int border = DpiScale(kRefHoverBorder);
    // When a column-wrap continuation is stitched below displayed.region in
    // the bitmap (see RefHoverRender.cpp's StackPixmapsVertically), a click
    // there falls outside what displayed.region maps to — the formula below
    // would silently produce a page point in the wrong place. Reject clicks
    // past the primary crop's rendered height rather than mis-hit-test.
    float regionPixH = s->displayed.region.dy * zoom;
    if ((float)(clientY - border) > regionPixH) {
        return false;
    }
    ptOut.x = s->displayed.region.x + ((float)(clientX - border) / zoom);
    ptOut.y = s->displayed.region.y + ((float)(clientY - border) / zoom);
    return true;
}

static IPageDestination* LaunchLinkAtPagePt(RefHoverState* s, PointF pagePt) {
    if (!s || !s->hitEngine || s->displayed.destPage <= 0) {
        return nullptr;
    }
    IPageElement* el = s->hitEngine->GetElementAtPos(s->displayed.destPage, pagePt);
    if (!el || !el->Is(kindPageElementDest)) {
        return nullptr;
    }
    IPageDestination* dest = el->AsLink();
    if (!dest || !IsLaunchLinkKind(dest->GetKind())) {
        return nullptr;
    }
    return dest;
}

IPageDestination* LaunchLinkAtPopupPt(RefHoverState* s, int clientX, int clientY) {
    PointF pagePt;
    if (!PopupClientToPagePt(s, clientX, clientY, pagePt)) {
        return nullptr;
    }
    return LaunchLinkAtPagePt(s, pagePt);
}

bool RefHoverRerenderDisplayedRegion(RefHoverState* s, EngineBase* engine, int page, RectF region) {
    if (!s || !engine || page <= 0) {
        return false;
    }
    float zoom = s->displayed.baseZoom * s->displayed.userZoom;
    if (zoom <= 0.f) {
        return false;
    }
    s->displayed.destPage = page;
    s->displayed.region = region;
    RefHoverState::RenderRequest req;
    req.pageNo = page;
    req.zoom = zoom;
    req.region = region;
    RefHoverRequestRender(s, engine, req);
    return true;
}

// Canvas wiring entry points (RefHoverCanvas.cpp) — keep Canvas.cpp thin.
bool RefHoverIsInternalLink(IPageElement* el, DisplayModel* dm) {
    if (!el || !el->Is(kindPageElementDest)) {
        return false;
    }
    IPageDestination* dest = el->AsLink();
    if (!dest) {
        return false;
    }
    if (IsLaunchLinkKind(dest->GetKind())) {
        return false;
    }
    int destPage = PageDestGetPageNo(dest);
    if (dm && dm->ValidPageNo(destPage)) {
        return true;
    }
    // chaptered doc: destPage stays -1 until clicked, but a dest with a
    // chapter to resolve lazily is still an internal link
    return dm && destPage < 1 && dest->loc.chapter >= 1;
}

// Scroll the popup's rendered region by a wheel notch. Positive delta scrolls
// toward earlier content (up); negative scrolls toward later content (down).
// Rolls over to the previous / next page when the viewport hits a page edge
// (continuous scrolling). Popup window keeps its initial size; only the
// rendered region's Y (and possibly page number) changes.
bool RefHoverWheelScroll(RefHoverState* s, EngineBase* engine, int wheelDelta) {
    if (!s || !RefHoverPopupShown(s) || s->displayed.destPage <= 0 || !engine) {
        return false;
    }
    float zoom = s->displayed.baseZoom * s->displayed.userZoom;
    if (zoom <= 0.f) {
        return false;
    }
    int pageCount = engine->PageCount();
    int page = s->displayed.destPage;
    RectF region = s->displayed.region;
    RectF mediabox = engine->PageMediabox(page);
    if (mediabox.dx <= 0.f || mediabox.dy <= 0.f) {
        return false;
    }

    float scrollStep = (float)DpiScale(kRefHoverScrollStepPx);
    float scrollPt = scrollStep * ((float)wheelDelta / (float)kWheelDelta) / zoom;
    float newY = region.y - scrollPt;

    if (newY < 0.f) {
        if (page > 1) {
            float overflow = -newY;
            page--;
            mediabox = engine->PageMediabox(page);
            newY = mediabox.dy - region.dy - overflow;
            newY = std::max(newY, 0.f);
        } else {
            newY = 0.f;
        }
    } else if (newY + region.dy > mediabox.dy) {
        if (page < pageCount) {
            float overflow = (newY + region.dy) - mediabox.dy;
            page++;
            mediabox = engine->PageMediabox(page);
            newY = overflow;
            if (newY + region.dy > mediabox.dy) {
                newY = mediabox.dy - region.dy;
            }
            newY = std::max(newY, 0.f);
        } else {
            newY = mediabox.dy - region.dy;
            newY = std::max(newY, 0.f);
        }
    }

    if (page == s->displayed.destPage && newY == region.y) {
        return false;
    }
    region.y = newY;
    region.dy = std::min(region.dy, mediabox.dy);
    if (region.x + region.dx > mediabox.dx) {
        region.x = mediabox.dx - region.dx;
        if (region.x < 0.f) {
            region.x = 0.f;
            region.dx = mediabox.dx;
        }
    }

    return RefHoverRerenderDisplayedRegion(s, engine, page, region);
}
