#include "polyfont.h"

#include "drmem.h"
#include "errors.h"
#include "font.h"
#include "globvars.h"
#include "grafdata.h"
#include "graphics.h"
#include "loading.h"
#include "platform.h"
#include "utility.h"

#include "carpocalypse2_types.h"
#include "brender/br_types.h"

#include "c2_string.h"

#include "carpocalypse2_macros.h"
int NearestPowerOfTwo(int pValue);
// FUNCTION: CARMA2_HW 0x00497610
void C2_HOOK_FASTCALL UpdateMapAndSaveOptions(void) {
    InitMap();
    SaveOptions();
}

// FUNCTION: CARMA2_HW 0x00464e40
void C2_HOOK_FASTCALL RenderPolyTextLine(const char *pText, int pX, int pY, int pFont, tJustification pJust, int pRender) {
    int text_len;
    int i;
    int draw_x;
    int draw_y;

    CheckAvailabilityOfThisFont(pFont);
    if (pRender) {
        CleanPolyFontDanglers();
    }
    switch (pJust) {
    case eJust_right:
        pX -= PolyFontTextWidth(pFont, pText);
        break;
    case eJust_centre:
        pX -= PolyFontTextWidth(pFont, pText) / 2;
        break;
    case eJust_left:
        break;
    }

    text_len = strlen(pText);
    if (gCount_polyfont_glyph_actors + text_len >= CARPOCALYPSE2_ASIZE(gPolyfont_glyph_actors)) {
        /* line is not drawn */
        /* FIXME: log debug message */
        return;
    }

    draw_x = pX;
    draw_y = pY;
    for (i = 0; i < text_len; i++) {
        tU8 c = pText[i];
        br_actor* actor;

        if (c == '\r') {
            draw_x = pX;
            draw_y += gPoly_fonts[pFont].fontCharacterHeight;
            continue;
        }
        if (c >= 'a' && c <= 'z') {
            c -= 'a' - 'A';
        }
        if (!gPoly_fonts[pFont].glyphs[c].used) {
            draw_x += gPoly_fonts[pFont].widthOfBlank;
            continue;
        }
        actor = gPolyfont_glyph_actors[gCount_polyfont_glyph_actors];
        actor->model = gPoly_fonts[pFont].glyphs[c].model;
        actor->material = GetPolyFontMaterial(pFont, c);
        BrActorAdd(gString_root_actor, actor);
        BrVector3Set(&actor->t.t.translate.t, (float)draw_x, (float)-draw_y, -1.1f);
        gCount_polyfont_glyph_actors++;
        draw_x += gPoly_fonts[pFont].glyphs[c].glyph_width + gPoly_fonts[pFont].interCharacterSpacing;
    }
    if (pRender) {
        int original_origin_x;
        int original_origin_y;
        int original_width;
        int original_height;
        int original_base_x;
        int original_base_y;

        BrActorAdd(gHUD_root, gString_root_actor);
        original_origin_x = gBack_screen->origin_x;
        original_origin_y = gBack_screen->origin_y;
        original_width = gBack_screen->width;
        original_height = gBack_screen->height;
        original_base_x = gBack_screen->base_x;
        original_base_y = gBack_screen->base_y;
        gBack_screen->origin_x = 0;
        gBack_screen->origin_y = 0;
        gBack_screen->base_x = 0;
        gBack_screen->base_y = 0;
        gBack_screen->width = 640;
        gBack_screen->height = 480;
        BrZbSceneRender(gHUD_root, gHUD_camera, gBack_screen, gDepth_buffer);
        gBack_screen->origin_x = original_origin_x;
        gBack_screen->origin_y = original_origin_y;
        gBack_screen->width = original_width;
        gBack_screen->height = original_height;
        gBack_screen->base_x = original_base_x;
        gBack_screen->base_y = original_base_y;
        BrActorRemove(gString_root_actor);
        CleanPolyFontDanglers();
    }
}