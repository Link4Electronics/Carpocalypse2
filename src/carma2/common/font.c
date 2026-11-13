#include "font.h"

#include "errors.h"
#include "loading.h"
#include "utility.h"

#include "carpocalypse2_macros.h"

#include "c2_string.h"
#include "font.h"
#include <stdlib.h>
#include <stdio.h>

#include "drmem.h"
#include "loading1.h"
#include "graphics1.h"
#include "graphics2.h"
#include "utility.h"
#include "errors.h"
#include "graphics3.h"
#include "packfile.h"
#include "loadtif.h"
#include "globvars.h"
#include "platform.h"
#include "carpocalypse2_macros.h"
#include "carpocalypse2_types.h"

#include <string.h>

#define POLYFONT_MATERIAL_STORE_PROPS(MATERIAL, CHARACTER, FONTIDX, COUNTER)  do { (MATERIAL)->user = (void*)(uintptr_t)((CHARACTER) | ((FONTIDX) << 8) | ((COUNTER) << 16)); } while (0)
#define POLYFONT_MATERIAL_GET_CHARACTER(MATERIAL) ((((uintptr_t)(MATERIAL)->user) >> 0) & 0xff)
#define POLYFONT_MATERIAL_GET_FONTIDX(MATERIAL) ((((uintptr_t)(MATERIAL)->user) >> 8) & 0xff)
#define POLYFONT_MATERIAL_GET_COUNTER(MATERIAL) ((((uintptr_t)(MATERIAL)->user) >> 16) & 0xffff)

#define POLYFONT_FONT_CHARACTER(FONT, CHARACTER) ((CHARACTER) + ((FONT) << 8))
#define POLYFONT_PACK_USER(FONT_CHARACTER, GENERATION) ((void*)(uintptr_t)((FONT_CHARACTER) + ((GENERATION) << 16)))
#define POLYFONT_MATERIAL_GET_FONT_CHARACTER(MATERIAL) (((uintptr_t)(MATERIAL)->user) & 0xffff)
#define POLYFONT_MATERIAL_GET_GENERATION(MATERIAL) ((((uintptr_t)(MATERIAL)->user) >> 16) & 0xffff)
typedef struct tPolyPixGlyph {
    int used;
    int w;
    int h;
    br_uint_16* pixels; /* RGB_565, magenta already keyed to 0 */
} tPolyPixGlyph;

typedef struct tPolyPixSet {
    char path[256];
    int ascii_offset;
    tPolyPixGlyph glyphs[256];
    void* res;
} tPolyPixSet;

// GLOBAL: CARMA2_HW 0x00686498
int polyFontMaterialCounter; // FIXME: unknown purpose

// GLOBAL: CARMA2_HW 0x00686480
int currentPolyFontMaterialIdx;

// GLOBAL: CARMA2_HW 0x0076c960
br_pixelmap* gTexture_maps[1024];

// GLOBAL: CARMA2_HW 0x007663e0
tDR_font gFonts[28];

// GLOBAL: CARMA2_HW 0x0059ad30
const char* gFont_names[24] = {
    "TYPEABLE",
    "ORANGHED",
    "BLUEHEAD",
    "GREENHED",
    "MEDIUMHD",
    "TIMER",
    "NEWHITE",
    "NEWRED",
    "NEWBIGGR",
    "GRNDK",
    "GRNLIT",
    "GRYDK",
    "GRYLIT",
    "BUTTIN",
    "BUTTOUT",
    "LITPLAQ",
    "DRKPLAQ",
    "RED1",
    "RED1GLOW",
    "GRN1",
    "GRN1GLOW",
    "RED2",
    "RED2GLOW",
    "OPPOSTAT",
};

// GLOBAL: CARMA2_HW 0x00764f00
br_pixelmap* gPixelmapBuffer[1000];

// GLOBAL: CARMA2_HW 0x007663d0
int gPixelmapBufferSize;

// GLOBAL: CARMA2_HW 0x0076d960
tPolyFont gPoly_fonts[27];

// GLOBAL: CARMA2_HW 0x0068648c
int gSize_font_texture_pages;

// GLOBAL: CARMA2_HW 0x00765ec0
tPolyFontBorderColours gPoly_font_border_colours[27];

// GLOBAL: CARMA2_HW 0x00686488
int gInterface_fonts_loaded;

// GLOBAL: CARMA2_HW 0x00765ea0
int gInitial_count_font_texture_pages = 0;

// GLOBAL: CARMA2_HW 0x00686494
int gInterface_polyfont_texture_pages;

// GLOBAL: CARMA2_HW 0x00686340
br_material* gPoly_font_materials[80];

// GLOBAL: CARMA2_HW 0x0074cf10
br_actor* gString_root_actor;

// GLOBAL: CARMA2_HW 0x0074cae0
br_actor* gPolyfont_glyph_actors[256];

// GLOBAL: CARMA2_HW 0x00686490
int gCount_polyfont_glyph_actors;

// GLOBAL: CARMA2_HW 0x0059ada0
int gDRFont_to_polyfont_mapping[24] = {
    kPolyfont_ingame_tiny_red,
    kPolyfont_ingame_tiny_yellow,
    kPolyfont_ingame_tiny_blue,
    kPolyfont_ingame_tiny_green,
    kPolyfont_ingame_italic_yellow,
    kPolyfont_ingame_big_timer,
    kPolyfont_ingame_medium_blue,
    kPolyfont_ingame_medium_red,
    kPolyfont_ingame_medium_orange,
    kPolyfont_ingame_tiny_green,
    kPolyfont_ingame_medium_green,
    kPolyfont_ingame_tiny_blue,
    kPolyfont_ingame_tiny_red,
    kPolyfont_ingame_tiny_yellow,
    kPolyfont_ingame_tiny_blue,
    kPolyfont_ingame_tiny_blue,
    kPolyfont_ingame_tiny_blue,
    kPolyfont_ingame_italic_red,
    kPolyfont_ingame_italic_red,
    kPolyfont_ingame_italic_green,
    kPolyfont_ingame_italic_green,
    kPolyfont_ingame_italic_red,
    kPolyfont_ingame_italic_red,
    kPolyfont_ingame_medium_blue,
};

// GLOBAL: CARMA2_HW 0x0059ad28
int gRender_poly_text = 1;

static tPolyPixSet* poly_pix_sets[16];

#ifdef CARPOCALYPSE2_MATCHING
extern br_actor* C2_HOOK_CDECL BrActorAdd();
extern br_actor* C2_HOOK_CDECL BrActorRemove();
extern void C2_HOOK_CDECL BrMaterialUpdate();
extern br_model* C2_HOOK_CDECL BrModelAdd();
extern br_model* C2_HOOK_CDECL BrModelAllocate();
extern void C2_HOOK_CDECL BrZbSceneRender();
extern void C2_HOOK_FASTCALL CleanPolyFontDanglers(void);
extern void C2_HOOK_FASTCALL CreatePolyFont(const char* pName, int pSize, int pIndex);
extern void C2_NORETURN C2_HOOK_CDECL FatalError();
extern void C2_HOOK_FASTCALL LoadInterfaceFonts(void);
extern void C2_HOOK_FASTCALL PolyFontText(const char *pText, int pX, int pY, int pFont, tJustification pJust, int pRender);
extern void C2_HOOK_FASTCALL TransparentPolyFontText(const char *pText, int pX, int pY, int pFont, tJustification pJust, int pRender, double pOpacity_factor);
extern void __cdecl _ftol();
extern br_pixelmap* gBack_screen;
extern br_pixelmap* gDepth_buffer;
extern br_actor* gHUD_camera;
extern br_actor* gHUD_root;
extern char gUNK_0058a038[];
#endif
// FUNCTION: CARMA2_HW 0x00463730
int C2_HOOK_FASTCALL PolyFontHeight(int pFont_index) {

    CheckAvailabilityOfThisFont(pFont_index);
    return gPoly_fonts[pFont_index].fontCharacterHeight;
}

// FUNCTION: CARMA2_HW 0x00463760
int C2_HOOK_FASTCALL FindCharacterWidth(br_pixelmap* pMap) {
    int y;
    int x;
    tU16* pixel;
    int height;

    height = pMap->height;
    x = pMap->width - 1;
    for (; x >= 0; x--) {
        pixel = (tU16*)pMap->pixels + x;
        for (y = 0; y < height; y++) {
            if (*(tU16*)pixel != 0) {
                return x + 1;
            }
            pixel =pixel + pMap->row_bytes / 2;
        }
    }
    return 1;
}

// FUNCTION: CARMA2_HW 0x004637d0
int C2_HOOK_FASTCALL CharacterWidth(int pFont_index, tU8 pCharacter) {
    tPolyFont* font;

    if ('a' <= pCharacter && pCharacter <= 'z') {
        pCharacter = pCharacter - 'a' + 'A';
    }

    font = &gPoly_fonts[pFont_index];
    if (gPoly_fonts[pFont_index].glyphs[pCharacter].used) {
        return gPoly_fonts[pFont_index].glyphs[pCharacter].glyph_width;
    }
    return font->widthOfBlank;
}

// FUNCTION: CARMA2_HW 0x00463830
int C2_HOOK_FASTCALL GetSpacing(int pFont_index) {
    tPolyFont* font;

    font = &gPoly_fonts[pFont_index];
    return font->interCharacterSpacing;
}

// FUNCTION: CARMA2_HW 0x00463850
void C2_HOOK_FASTCALL SolidPolyFontTextInABox(int pFont, const char* pText, int pX, int pY, int pWidth, int pHeight, tJustification pJustification, undefined4 pParam_8) {

    CheckAvailabilityOfThisFont(pFont);
    TransparentPolyFontTextInABox(pFont, pText, pX, pY, pWidth, pHeight, pJustification, pParam_8, 1.0);
}

// FUNCTION: CARMA2_HW 0x004638b0
void C2_NAKED C2_HOOK_FASTCALL PolyFontTextInABox(int pFont, const char* pText, int pLeft, int pTop, int pRight, int pBottom, tJustification pJust, int pRender) {
#ifndef CARPOCALYPSE2_MATCHING
    char s[256];
    int s_len;
    int x;
    int y;
    int in_start_put;
    int in_end_put;
    int in_pos;
    int newline;
    int text_x;
    int max_width;

    y = pTop;
    in_start_put = 0;
    text_x = 0;
    CheckAvailabilityOfThisFont(pFont);
    max_width = pRight - pLeft;
    in_pos = 0;
    s_len = 0;
    s[s_len] = '\0';

    while (pText[in_pos] != '\0') {
        tU8 chr;

        chr = pText[in_pos];
        newline = chr == '\r';
        s[s_len] = chr;
        if (newline) {
            s[s_len] = ' ';
        }
        s[s_len + 1] = '\0';
        if (!newline) {
            if (chr >= 'a' && chr <= 'z') {
                chr -= 0x20;
            }
            if (gPoly_fonts[pFont].glyphs[(tU8)chr].used) {
                text_x += gPoly_fonts[pFont].glyphs[(tU8)chr].glyph_width;
            } else {
                text_x += gPoly_fonts[pFont].widthOfBlank;
            }
        }
        in_end_put = in_pos;
        if (text_x > max_width || newline) {
            int len_put;

            while (in_end_put > in_start_put && pText[in_end_put] != ' ') {
                in_end_put -= 1;
            }
            if (in_end_put == in_start_put) {
                in_end_put = in_pos;
            }
            len_put = in_end_put - in_start_put;
            s[len_put + 1] = '\0';
            if (pText[in_end_put] == ' ') {
                in_end_put += 1;
            }
            s[len_put] = '\0';
            in_pos = in_end_put;
            if (pJust == eJust_centre) {
                x = (pRight + pLeft) / 2;
            } else {
                x = pLeft;
            }
            PolyFontText(s, x, y, pFont, pJust, gRender_poly_text);
            while (pText[in_pos] == '\r' || pText[in_pos] == ' ') {
                in_pos++;
            }
            y += PolyFontHeight(pFont);
            text_x = 0;
            in_start_put = in_pos;
            s_len = 0;
        } else {
            s_len++;
            in_pos++;
        }
    }
    if (s_len != 0) {
        int x;
        tJustification justif;

        if (pJust == eJust_centre) {
            x = (pRight + pLeft) / 2;
            justif = eJust_centre;
        } else {
            x = pLeft;
            justif = pJust;
        }
        PolyFontText(s, x, y, pFont, justif, gRender_poly_text);
    }
#else
    __asm {
            sub esp, 0x11c
            mov eax, dword ptr [esp + 0x124]
            push ebx
            mov ebx, ecx
            mov dword ptr [esp + 4], eax
            push ebp
            push esi
            lea eax, [ebx + ebx*4]
            push edi
            xor edi, edi
            mov ebp, edx
            lea ecx, [eax + eax*4]
            mov dword ptr [esp + 0x18], ebx
            mov dword ptr [esp + 0x14], edi
            lea eax, [ebx + ecx*4]
            lea eax, [eax + eax*8]
            shl eax, 3
            mov dword ptr [esp + 0x20], eax
            cmp dword ptr [eax + gPoly_fonts+0x60], edi
            jne L004638f3
            call LoadInterfaceFonts
    L004638f3:
            mov eax, dword ptr [esp + 0x138]
            mov edx, dword ptr [esp + 0x130]
            sub eax, edx
            xor esi, esi
            mov dword ptr [esp + 0x28], eax
            mov al, byte ptr [ebp]
            xor ecx, ecx
            mov byte ptr [esp + 0x2c], 0
            test al, al
            je L00463a7e
    L0046391b:
            mov al, byte ptr [esi + ebp]
            xor edx, edx
            cmp al, 0xd
            mov byte ptr [esp + ecx + 0x2c], al
            jne L00463932
            mov byte ptr [esp + ecx + 0x2c], 0x20
            mov edx, 1
    L00463932:
            test edx, edx
            mov byte ptr [esp + ecx + 0x2d], 0
            jne L00463987
            cmp al, 0x61
            mov byte ptr [esp + 0x1c], al
            jb L0046394d
            cmp al, 0x7a
            ja L0046394d
            add al, 0xe0
            mov byte ptr [esp + 0x1c], al
    L0046394d:
            mov eax, dword ptr [esp + 0x1c]
            and eax, 0xff
            mov ebx, eax
            shl ebx, 3
            sub ebx, eax
            mov dword ptr [esp + 0x24], ebx
            mov ebx, dword ptr [esp + 0x20]
            mov eax, dword ptr [esp + 0x24]
            lea eax, [ebx + eax*4]
            cmp dword ptr [eax + gPoly_fonts+0x6c], 0
            je L0046397d
            mov eax, dword ptr [eax + gPoly_fonts+0x74]
            jmp L00463983
    L0046397d:
            mov eax, dword ptr [ebx + gPoly_fonts+0x54]
    L00463983:
            add dword ptr [esp + 0x14], eax
    L00463987:
            mov eax, dword ptr [esp + 0x14]
            mov ebx, dword ptr [esp + 0x28]
            cmp eax, ebx
            jg L004639a2
            test edx, edx
            jne L004639a2
            mov ebx, dword ptr [esp + 0x18]
            inc ecx
            inc esi
            jmp L00463a6d
    L004639a2:
            cmp esi, edi
            mov eax, esi
            jle L004639b3
    L004639a8:
            cmp byte ptr [eax + ebp], 0x20
            je L004639b3
            dec eax
            cmp eax, edi
            jg L004639a8
    L004639b3:
            cmp eax, edi
            jne L004639b9
            mov eax, esi
    L004639b9:
            mov dl, byte ptr [eax + ebp]
            mov ecx, eax
            sub ecx, edi
            cmp dl, 0x20
            mov esi, eax
            mov byte ptr [esp + ecx + 0x2d], 0
            jne L004639cf
            lea esi, [eax + 1]
    L004639cf:
            mov eax, dword ptr [esp + 0x140]
            mov byte ptr [esp + ecx + 0x2c], 0
            cmp eax, 2
            jne L00463a0b
            mov ecx, dword ptr [gRender_poly_text]
            mov ebx, dword ptr [esp + 0x18]
            mov edx, dword ptr [esp + 0x10]
            push ecx
            mov ecx, dword ptr [esp + 0x134]
            push eax
            mov eax, dword ptr [esp + 0x140]
            push ebx
            add eax, ecx
            push edx
            cdq
            sub eax, edx
            mov edx, eax
            sar edx, 1
            jmp L00463a24
    L00463a0b:
            mov edx, dword ptr [gRender_poly_text]
            mov ebx, dword ptr [esp + 0x18]
            push edx
            mov edx, dword ptr [esp + 0x134]
            push eax
            mov eax, dword ptr [esp + 0x18]
            push ebx
            push eax
    L00463a24:
            lea ecx, [esp + 0x3c]
            call PolyFontText
    L00463a2d:
            mov al, byte ptr [esi + ebp]
            cmp al, 0xd
            je L00463a38
            cmp al, 0x20
            jne L00463a3b
    L00463a38:
            inc esi
            jmp L00463a2d
    L00463a3b:
            mov edi, dword ptr [esp + 0x20]
            mov eax, dword ptr [edi + gPoly_fonts+0x60]
            test eax, eax
            jne L00463a4e
            call LoadInterfaceFonts
    L00463a4e:
            mov eax, dword ptr [edi + gPoly_fonts+0x50]
            mov ecx, dword ptr [esp + 0x10]
            shl eax, 1
            cdq
            sub eax, edx
            mov edi, esi
            sar eax, 1
            add ecx, eax
            mov dword ptr [esp + 0x10], ecx
            xor ecx, ecx
            mov dword ptr [esp + 0x14], ecx
    L00463a6d:
            cmp byte ptr [esi + ebp], 0
            jne L0046391b
            mov edx, dword ptr [esp + 0x130]
    L00463a7e:
            test ecx, ecx
            je L00463ac0
            mov eax, dword ptr [esp + 0x140]
            mov ecx, dword ptr [gRender_poly_text]
            cmp eax, 2
            push ecx
            jne L00463ab0
            mov eax, dword ptr [esp + 0x14]
            mov ecx, dword ptr [esp + 0x13c]
            push 2
            push ebx
            push eax
            lea eax, [ecx + edx]
            cdq
            sub eax, edx
            mov edx, eax
            sar edx, 1
            jmp L00463ab7
    L00463ab0:
            push eax
            mov eax, dword ptr [esp + 0x18]
            push ebx
            push eax
    L00463ab7:
            lea ecx, [esp + 0x3c]
            call PolyFontText
    L00463ac0:
            pop edi
            pop esi
            pop ebp
            pop ebx
            add esp, 0x11c
            ret 0x18
    }
#endif
}

// FUNCTION: CARMA2_HW 0x00463ad0
void C2_NAKED C2_HOOK_FASTCALL TransparentPolyFontTextInABox(int pFont, const char* pText, int pLeft, int pTop, int pRight, int pBottom, tJustification pJust, undefined4 pUnknown, double pBlend) {
#ifndef CARPOCALYPSE2_MATCHING
    char s[256];
    int s_len;
    int y;
    int in_start_put;
    int in_end_put;
    int str_i;
    int newline;
    int text_x;

    in_start_put = 0;
    text_x = 0;
    if (pText != NULL) {
        str_i = 0;
        s_len = 0;
        s[s_len] = '\0';
        y = pTop;

        while (pText[str_i] != '\0') {
            int chr;

            chr = pText[str_i];
            newline = chr == '\r';
            s[s_len] = chr;
            if (newline) {
                s[s_len] = ' ';
            }
            s[s_len + 1] = '\0';
            if (!newline) {
                text_x += CharacterWidth(pFont, (tU8)chr);
            }
            in_end_put = str_i;
            if (text_x <= pRight - pLeft && !newline) {
                s_len++;
                str_i++;
            } else {
                int len_put;

                /* Avoid breaking words in parts ... */
                while (in_end_put > in_start_put && pText[in_end_put] != ' ') {
                    in_end_put -= 1;
                }
                /* ... but break when the word is too long */
                if (in_end_put == in_start_put) {
                    in_end_put = str_i;
                }
                len_put = in_end_put - in_start_put;
                s[len_put + 1] = '\0';
                if (pText[in_end_put] == ' ') {
                    in_end_put += 1;
                }
                s[len_put] = '\0';
                if (pJust == eJust_centre) {
                    TransparentPolyFontText(s, (pRight + pLeft) / 2, y, pFont, pJust, gRender_poly_text, pBlend);
                } else {
                    TransparentPolyFontText(s, pLeft, y, pFont, pJust, gRender_poly_text, pBlend);
                }
                while (pText[in_end_put] == '\r' || pText[in_end_put] == ' ') {
                    in_end_put++;
                }
                y += (2 * PolyFontHeight(pFont)) / 2;
                text_x = 0;
                in_start_put = in_end_put;
                s_len = 0;
                str_i = in_start_put;
            }
        }
        if (s_len != 0) {

            if (pJust == eJust_centre) {
                TransparentPolyFontText(s, (pRight + pLeft) / 2, y, pFont, pJust, gRender_poly_text, pBlend);
            } else {
                TransparentPolyFontText(s, pLeft, y, pFont, pJust, gRender_poly_text, pBlend);
            }
        }
    }
#else
    __asm {
            sub esp, 0x118
            push ebx
            push ebp
            mov ebp, ecx
            mov eax, edx
            mov ecx, dword ptr [esp + 0x128]
            xor ebx, ebx
            push esi
            cmp eax, ebx
            push edi
            mov dword ptr [esp + 0x18], eax
            mov dword ptr [esp + 0x14], ebx
            mov dword ptr [esp + 0x10], ecx
            je L00463d24
            mov ecx, dword ptr [esp + 0x134]
            mov esi, dword ptr [esp + 0x12c]
            sub ecx, esi
            xor edi, edi
            mov dword ptr [esp + 0x24], ecx
            mov cl, byte ptr [eax]
            xor edx, edx
            mov byte ptr [esp + 0x28], bl
            test cl, cl
            mov dword ptr [esp + 0x20], ebx
            je L00463cca
            jmp L00463b2b
    L00463b27:
            mov eax, dword ptr [esp + 0x18]
    L00463b2b:
            mov al, byte ptr [edi + eax]
            xor esi, esi
            cmp al, 0xd
            mov byte ptr [esp + edx + 0x28], al
            jne L00463b42
            mov byte ptr [esp + edx + 0x28], 0x20
            mov esi, 1
    L00463b42:
            test esi, esi
            mov byte ptr [esp + edx + 0x29], 0
            jne L00463ba7
            cmp al, 0x61
            mov byte ptr [esp + 0x1c], al
            jb L00463b5d
            cmp al, 0x7a
            ja L00463b5d
            add al, 0xe0
            mov byte ptr [esp + 0x1c], al
    L00463b5d:
            lea eax, [ebp + ebp*4]
            lea eax, [eax + eax*4]
            lea eax, [ebp + eax*4]
            lea ecx, [eax + eax*8]
            mov eax, dword ptr [esp + 0x1c]
            and eax, 0xff
            mov ebx, eax
            shl ebx, 3
            shl ecx, 3
            sub ebx, eax
            lea eax, [ecx + ebx*4]
            mov ebx, dword ptr [eax + gPoly_fonts+0x6c]
            test ebx, ebx
            je L00463b93
            mov eax, dword ptr [eax + gPoly_fonts+0x74]
            jmp L00463b99
    L00463b93:
            mov eax, dword ptr [ecx + gPoly_fonts+0x54]
    L00463b99:
            mov ecx, dword ptr [esp + 0x14]
            mov ebx, dword ptr [esp + 0x20]
            add ecx, eax
            mov dword ptr [esp + 0x14], ecx
    L00463ba7:
            mov ecx, dword ptr [esp + 0x24]
            mov eax, dword ptr [esp + 0x14]
            cmp eax, ecx
            jg L00463bbe
            test esi, esi
            jne L00463bbe
            inc edx
            inc edi
            jmp L00463cb5
    L00463bbe:
            cmp edi, ebx
            mov eax, edi
            jle L00463bd5
    L00463bc4:
            mov esi, dword ptr [esp + 0x18]
            cmp byte ptr [eax + esi], 0x20
            je L00463bd9
            dec eax
            cmp eax, ebx
            jg L00463bc4
            jmp L00463bd9
    L00463bd5:
            mov esi, dword ptr [esp + 0x18]
    L00463bd9:
            cmp eax, ebx
            jne L00463bdf
            mov eax, edi
    L00463bdf:
            mov dl, byte ptr [eax + esi]
            mov ecx, eax
            sub ecx, ebx
            cmp dl, 0x20
            mov edi, eax
            mov byte ptr [esp + ecx + 0x29], 0
            jne L00463bf5
            lea edi, [eax + 1]
    L00463bf5:
            mov eax, dword ptr [esp + 0x13c]
            mov edx, dword ptr [esp + 0x148]
            cmp eax, 2
            mov byte ptr [esp + ecx + 0x28], 0
            push edx
            jne L00463c3e
            mov eax, dword ptr [esp + 0x148]
            mov ecx, dword ptr [gRender_poly_text]
            mov edx, dword ptr [esp + 0x14]
            push eax
            mov eax, dword ptr [esp + 0x13c]
            push ecx
            mov ecx, dword ptr [esp + 0x138]
            push 2
            push ebp
            add eax, ecx
            push edx
            cdq
            sub eax, edx
            mov edx, eax
            sar edx, 1
            jmp L00463c5b
    L00463c3e:
            mov ecx, dword ptr [esp + 0x148]
            mov edx, dword ptr [gRender_poly_text]
            push ecx
            push edx
            mov edx, dword ptr [esp + 0x138]
            push eax
            mov eax, dword ptr [esp + 0x20]
            push ebp
            push eax
    L00463c5b:
            lea ecx, [esp + 0x40]
            call TransparentPolyFontText
    L00463c64:
            mov al, byte ptr [edi + esi]
            cmp al, 0xd
            je L00463c6f
            cmp al, 0x20
            jne L00463c72
    L00463c6f:
            inc edi
            jmp L00463c64
    L00463c72:
            lea eax, [ebp + ebp*4]
            lea ecx, [eax + eax*4]
            lea eax, [ebp + ecx*4]
            lea esi, [eax + eax*8]
            shl esi, 3
            mov eax, dword ptr [esi + gPoly_fonts+0x60]
            test eax, eax
            jne L00463c92
            call LoadInterfaceFonts
    L00463c92:
            mov eax, dword ptr [esi + gPoly_fonts+0x50]
            mov ecx, dword ptr [esp + 0x10]
            shl eax, 1
            cdq
            sub eax, edx
            mov ebx, edi
            sar eax, 1
            add ecx, eax
            xor edx, edx
            mov dword ptr [esp + 0x10], ecx
            mov dword ptr [esp + 0x20], ebx
            mov dword ptr [esp + 0x14], edx
    L00463cb5:
            mov eax, dword ptr [esp + 0x18]
            cmp byte ptr [edi + eax], 0
            jne L00463b27
            mov esi, dword ptr [esp + 0x12c]
    L00463cca:
            test edx, edx
            je L00463d24
            mov eax, dword ptr [esp + 0x13c]
            mov ecx, dword ptr [esp + 0x148]
            mov edx, dword ptr [esp + 0x144]
            cmp eax, 2
            push ecx
            push edx
            jne L00463d0b
            mov eax, dword ptr [gRender_poly_text]
            mov edx, dword ptr [esp + 0x13c]
            push eax
            mov ecx, dword ptr [esp + 0x1c]
            lea eax, [edx + esi]
            push 2
            cdq
            sub eax, edx
            push ebp
            mov edx, eax
            push ecx
            sar edx, 1
            jmp L00463d1b
    L00463d0b:
            mov ecx, dword ptr [gRender_poly_text]
            mov edx, dword ptr [esp + 0x18]
            push ecx
            push eax
            push ebp
            push edx
            mov edx, esi
    L00463d1b:
            lea ecx, [esp + 0x40]
            call TransparentPolyFontText
    L00463d24:
            pop edi
            pop esi
            pop ebp
            pop ebx
            add esp, 0x118
            ret 0x20
    }
#endif
}

// FUNCTION: CARMA2_HW 0x00463d40
void C2_HOOK_FASTCALL InitPolyFonts(void) {
    int i;
    tPath_name the_path;
    FILE* f;
    br_actor* actor;
    int red, green, blue;

    C2_HOOK_BUG_ON(CARPOCALYPSE2_ASIZE(gPoly_font_materials) != 80);

    PrintMemoryDump(0, "START OF InitPolyFonts()");

    InitPolyFontMaterials();
    C2_HOOK_BUG_ON(sizeof(gPoly_fonts) != 196344);
    memset(&gPoly_fonts, 0, sizeof(gPoly_fonts));
    gString_root_actor = BrActorAllocate(BR_ACTOR_NONE, NULL);
    if (gString_root_actor == NULL) {
#ifdef CARPOCALYPSE2_FIX_BUGS
        FatalError(kFatalError_OOM_S, "");
#else
        FatalError(kFatalError_OOM_S);
#endif
    }
    gString_root_actor->identifier = "gString_root_actor";
    gString_root_actor->t.type = BR_TRANSFORM_TRANSLATION;
    BrVector3Set(&gString_root_actor->t.t.translate.t, 0.0f, 0.0f, 0.0f);

    C2_HOOK_BUG_ON(CARPOCALYPSE2_ASIZE(gPolyfont_glyph_actors) != 256);
    for (i = 0; i < (int)CARPOCALYPSE2_ASIZE(gPolyfont_glyph_actors); i++) {

        actor = BrActorAllocate(BR_ACTOR_MODEL, NULL);
        gPolyfont_glyph_actors[i] = actor;
        if (actor == NULL) {
#ifdef CARPOCALYPSE2_FIX_BUGS
            FatalError(kFatalError_OOM_S, "");
#else
            FatalError(kFatalError_OOM_S);
#endif
        }
        actor->t.type = BR_TRANSFORM_TRANSLATION;
        actor->identifier = "character_actor";
        BrVector3Set(&actor->t.t.translate.t, 0.0f, 0.0f, 0.0f);
    }

    PathCat(the_path, gApplication_path, "FontCol.TXT");
    f = DRfopen(the_path, "rb");
    if (f != NULL) {
        /**
         * COLOURS FOR POLYGONAL FONTS
         *
         * Each font has four sets of RGB values in the range 0-255. These specify
         * the colour for each corner of every character in that font.
         */

        C2_HOOK_BUG_ON(CARPOCALYPSE2_ASIZE(gPoly_font_border_colours) != 27);
        for (i = 0; i < (int)CARPOCALYPSE2_ASIZE(gPoly_font_border_colours); i++) {

            /* Top/Left */
            GetThreeInts(f, &red, &green, &blue);
            gPoly_font_border_colours[i].tl.r = red;
            gPoly_font_border_colours[i].tl.g = green;
            gPoly_font_border_colours[i].tl.b = blue;

            /* Top/Right */
            GetThreeInts(f, &red, &green, &blue);
            gPoly_font_border_colours[i].tr.r = red;
            gPoly_font_border_colours[i].tr.g = green;
            gPoly_font_border_colours[i].tr.b = blue;

            /* Bottom/Left */
            GetThreeInts(f, &red, &green, &blue);
            gPoly_font_border_colours[i].bl.r = red;
            gPoly_font_border_colours[i].bl.g = green;
            gPoly_font_border_colours[i].bl.b = blue;

            /* Bottom/Right */
            GetThreeInts(f, &red, &green, &blue);
            gPoly_font_border_colours[i].br.r = red;
            gPoly_font_border_colours[i].br.g = green;
            gPoly_font_border_colours[i].br.b = blue;
        }
        fclose(f);
    } else {
        BrFailure("Missing FONTCOL.TXT file");
    }
    ConvertCarIcons(gIcons_pix);

    CreatePolyFontWithTimerFix(kPolyfont_ingame_tiny_yellow, "TINYFONT", 1.0f, 8);
    CreatePolyFontWithTimerFix(kPolyfont_ingame_tiny_blue, "TINYFONT", 1.0f, 8);
    CreatePolyFontWithTimerFix(kPolyfont_ingame_tiny_red, "TINYFONT", 1.0f, 8);
    CreatePolyFontWithTimerFix(kPolyfont_ingame_tiny_green, "TINYFONT", 1.0f, 8);
    CreatePolyFontWithTimerFix(kPolyfont_ingame_big_timer, "BIGYELLOWTIMER", 1.0f, 32);
    CreatePolyFontWithTimerFix(kPolyfont_ingame_medium_red, "MEDIUM_HEADUP", 1.0f, 16);
    CreatePolyFontWithTimerFix(kPolyfont_ingame_medium_blue, "MEDIUM_HEADUP", 1.0f, 16);
    CreatePolyFontWithTimerFix(kPolyfont_ingame_medium_orange, "MEDIUM_HEADUP", 1.0f, 16);
    CreatePolyFontWithTimerFix(kPolyfont_ingame_medium_green, "MEDIUM_HEADUP", 1.0f, 16);
    CreatePolyFontWithTimerFix(kPolyfont_ingame_italic_yellow, "BIG_ITALIC", 1.0f, 32);
    CreatePolyFontWithTimerFix(kPolyfont_ingame_italic_green, "BIG_ITALIC", 1.0f, 32);
    CreatePolyFontWithTimerFix(kPolyfont_ingame_italic_blue, "BIG_ITALIC", 1.0f, 32);
    CreatePolyFontWithTimerFix(kPolyfont_ingame_italic_red, "BIG_ITALIC", 1.0f, 32);
    CreatePolyFontWithTimerFix(kPolyfont_netpos, "NETPOSFONT", 1.0f, 16);

    gInitial_count_font_texture_pages = gSize_font_texture_pages;
    PrintMemoryDump(0, "END OF InitPolyFonts()");
}

// FUNCTION: CARMA2_HW 0x00464090
void C2_HOOK_FASTCALL CreatePolyFontWithTimerFix(int pFont, const char* pName, float pFactor, int pSize) {
    int i;

    CreatePolyFont(pName, pSize, pFont);

    if (pFont == kPolyfont_ingame_big_timer) {
        for (i = 0; i < (int)CARPOCALYPSE2_ASIZE(gPoly_fonts[kPolyfont_ingame_big_timer].glyphs); i++) {
            gPoly_fonts[kPolyfont_ingame_big_timer].glyphs[i].glyph_width = 27;
        }
        gPoly_fonts[kPolyfont_ingame_big_timer].glyphs[58].glyph_width = 11; /* FIXME: What is gyph 58? */
    }
}

br_model* C2_HOOK_FASTCALL CreateFontCharacterModel(int pIndex, int pAscii, int pWidth, int pHeight, const char* pName) {
    br_model* model;

    model = gPoly_fonts[pIndex].model;
    while (model != NULL) {
        if (model->bounds.max.v[0] == pWidth && model->bounds.min.v[1] == -pHeight) {
            return model;
        }
        model = model->user;
    }
    model = BrModelAllocate("String Model", 4, 2);
    model->user = gPoly_fonts[pIndex].glyphs[pIndex].model;
    gPoly_fonts[pIndex].glyphs[pIndex].model = model;
    if (model == NULL) {
        FatalError(kFatalError_CouldNotCreateTexturesPages_S, pName);
    }
    model->faces[0].vertices[0] = 0;
    model->faces[0].vertices[1] = 1;
    model->faces[0].vertices[2] = 2;
    model->faces[1].vertices[0] = 1;
    model->faces[1].vertices[1] = 3;
    model->faces[1].vertices[2] = 2;
    BrVector3Set(&model->vertices[0].p, 0.0f, 0.0f, -1.2f);
    BrVector3Set(&model->vertices[1].p, (float) pWidth, 0.0f, -1.2f);
    BrVector3Set(&model->vertices[2].p, 0.f, (float) -pHeight, -1.2f);
    BrVector3Set(&model->vertices[3].p, (float) pWidth, (float) -pHeight, -1.2f);
    BrVector2Set(&model->vertices[0].map, 0.0f, 0.0f);
    BrVector2Set(&model->vertices[1].map, 1.0f, 0.0f);
    BrVector2Set(&model->vertices[2].map, 0.0f, 1.0f);
    BrVector2Set(&model->vertices[3].map, 1.0f, 1.0f);
    ColourVertices(model, pIndex);
    model->flags &= ~(BR_MODF_KEEP_ORIGINAL | BR_MODF_UPDATEABLE);
    BrModelAdd(model);
    return model;
}

// FUNCTION: CARMA2_HW 0x00464a70
br_pixelmap* C2_HOOK_FASTCALL GetThisFuckingPixelmap(const char* path, const char* glyph_name, int loadFromDisk) {
    tPath_name pathBuffer;
    FILE* f;
    char* str;
    size_t i;

    PathCat(pathBuffer, path, "PIXIES.P16");
    f = PFfopen(pathBuffer, "rb");
    if (f != NULL) {
        PFfclose(f);
        if (loadFromDisk) {
            gPixelmapBufferSize = BrPixelmapLoadMany(pathBuffer, gPixelmapBuffer, CARPOCALYPSE2_ASIZE(gPixelmapBuffer));
        }
        strcpy(pathBuffer, glyph_name);

        str = strchr(pathBuffer, '.');
        *str = '\0';

        for (i = 0; i < gPixelmapBufferSize; i++) {
            if (gPixelmapBuffer[i] != NULL && DRStricmp(gPixelmapBuffer[i]->identifier, pathBuffer) == 0) {
                return gPixelmapBuffer[i];
            }
        }
        return NULL;
    } else {
        PathCat(pathBuffer, path, glyph_name);
        return DRImageLoad(pathBuffer);
    }
}

void C2_HOOK_FASTCALL KillThePixies(void) {
    size_t i;

    for (i = 0; i < gPixelmapBufferSize; i++) {

        if (gPixelmapBuffer[i] != NULL) {
            BrPixelmapFree(gPixelmapBuffer[i]);
            gPixelmapBuffer[i] = NULL;
        }
    }
    gPixelmapBufferSize = 0;
}

// FUNCTION: CARMA2_HW 0x004640d0
__inline void C2_HOOK_FASTCALL CheckAvailabilityOfThisFont(int pFont) {

    if (!gPoly_fonts[pFont].available) {
        LoadInterfaceFonts();
    }
}

// FUNCTION: CARMA2_HW 0x004640f0
void C2_HOOK_FASTCALL DisposeInterfaceFonts(void) {
    int i;
    br_pixelmap* map;

    if (gInterface_fonts_loaded) {

        RemovePolyFont(kPolyfont_hand_green_15pt_unlit);
        RemovePolyFont(kPolyfont_hand_green_15pt_lit);
        RemovePolyFont(kPolyfont_hand_red_15pt_unlit);
        RemovePolyFont(kPolyfont_hand_red_15pt_lit);
        RemovePolyFont(kPolyfont_hand_green_10pt_unlit);
        RemovePolyFont(kPolyfont_hand_green_10pt_lit);
        RemovePolyFont(kPolyfont_serp_red_15pt_lit);
        RemovePolyFont(kPolyfont_serp_red_30pt_lit);
        RemovePolyFont(kPolyfont_serp_green_30pt_unlit);
        RemovePolyFont(kPolyfont_serp_green_30pt_lit);
        RemovePolyFont(kPolyfont_serp_green_38pt_unlit);
        RemovePolyFont(kPolyfont_serp_green_38pt_lit);
        RemovePolyFont(kPolyfont_highlighter);

        for (i = 0; i < gInterface_polyfont_texture_pages; i++) {
            map = gTexture_maps[gInterface_polyfont_texture_pages + i];

            if (map != NULL) {

                BrMapRemove(map);
                BrPixelmapFree(map);
                gTexture_maps[gInterface_polyfont_texture_pages + i] = NULL;
            }
        }
        gInterface_fonts_loaded = 0;
    }
}

// FUNCTION: CARMA2_HW 0x00464290
void C2_HOOK_FASTCALL RemovePolyFont(int pFont) {
    int i;

    for (i = 0; i < (int)CARPOCALYPSE2_ASIZE(gPoly_fonts[pFont].glyphs); i++) {
        tPolyFontGlyph* glyph = &gPoly_fonts[pFont].glyphs[i];

        if (glyph->used) {

            glyph->material = NULL;
            glyph->model = NULL;
            glyph->used = 0;
        }
    }
    gPoly_fonts[pFont].available = 0;
}

// FUNCTION: CARMA2_HW 0x004642d0
void C2_NAKED C2_HOOK_FASTCALL LoadInterfaceFonts(void) {
#ifndef CARPOCALYPSE2_MATCHING

    if (!gInterface_fonts_loaded) {
        gSize_font_texture_pages = gInitial_count_font_texture_pages;
        CreatePolyFontWithTimerFix(kPolyfont_hand_green_15pt_unlit, "HAND15U", 1.0f, 16);
        CreatePolyFontWithTimerFix(kPolyfont_hand_green_15pt_lit, "HAND15", 1.0f, 16);
        CreatePolyFontWithTimerFix(kPolyfont_hand_red_15pt_unlit, "HAND15U", 1.0f, 16);
        CreatePolyFontWithTimerFix(kPolyfont_hand_red_15pt_lit, "HAND15", 1.0f, 16);
        CreatePolyFontWithTimerFix(kPolyfont_hand_green_10pt_unlit, "HAND10U", 1.0f, 16);
        CreatePolyFontWithTimerFix(kPolyfont_hand_green_10pt_lit, "HAND10", 1.0f, 16);
        CreatePolyFontWithTimerFix(kPolyfont_serp_red_15pt_lit, "SERP15", 1.0f, 16);
        CreatePolyFontWithTimerFix(kPolyfont_serp_red_30pt_lit, "SERP30", 1.0f, 32);
        CreatePolyFontWithTimerFix(kPolyfont_serp_green_30pt_unlit, "SERP30U", 1.0f, 32);
        CreatePolyFontWithTimerFix(kPolyfont_serp_green_30pt_lit, "SERP30", 1.0f, 32);
        CreatePolyFontWithTimerFix(kPolyfont_serp_green_38pt_unlit, "SERP38U", 1.0f, 64);
        CreatePolyFontWithTimerFix(kPolyfont_serp_green_38pt_lit, "SERP38", 1.0f, 64);
        CreatePolyFontWithTimerFix(kPolyfont_highlighter, "HAND15lo", 1.0f, 16);
        gInterface_polyfont_texture_pages = gSize_font_texture_pages - gInitial_count_font_texture_pages;
        gInterface_fonts_loaded = 1;
    }
#else
    __asm {
            mov eax, dword ptr [gInterface_fonts_loaded]
            test eax, eax
            jne L004643eb
            mov eax, dword ptr [gInitial_count_font_texture_pages]
            push 0
            mov edx, 0x10
            mov ecx, offset gDRFont_to_polyfont_mapping+0x29c
            mov dword ptr [gSize_font_texture_pages], eax
            call CreatePolyFont
            mov edx, 0x10
            mov ecx, offset gDRFont_to_polyfont_mapping+0x294
            push 1
            call CreatePolyFont
            mov edx, 0x10
            mov ecx, offset gDRFont_to_polyfont_mapping+0x29c
            push 2
            call CreatePolyFont
            mov edx, 0x10
            mov ecx, offset gDRFont_to_polyfont_mapping+0x294
            push 3
            call CreatePolyFont
            mov edx, 0x10
            mov ecx, offset gDRFont_to_polyfont_mapping+0x28c
            push 0xa
            call CreatePolyFont
            mov edx, 0x10
            mov ecx, offset gDRFont_to_polyfont_mapping+0x284
            push 0xb
            call CreatePolyFont
            mov edx, 0x10
            mov ecx, offset gDRFont_to_polyfont_mapping+0x27c
            push 4
            call CreatePolyFont
            mov edx, 0x20
            mov ecx, offset gDRFont_to_polyfont_mapping+0x274
            push 5
            call CreatePolyFont
            mov edx, 0x20
            mov ecx, offset gDRFont_to_polyfont_mapping+0x26c
            push 6
            call CreatePolyFont
            mov edx, 0x20
            mov ecx, offset gDRFont_to_polyfont_mapping+0x274
            push 7
            call CreatePolyFont
            mov edx, 0x40
            mov ecx, offset gDRFont_to_polyfont_mapping+0x264
            push 8
            call CreatePolyFont
            mov edx, offset gDRFont_to_polyfont_mapping+0x25c
            mov ecx, 9
            push 0x40
            push 0x3f800000
            call CreatePolyFontWithTimerFix
            mov edx, offset gDRFont_to_polyfont_mapping+0x250
            mov ecx, 0x1a
            push 0x10
            push 0x3f800000
            call CreatePolyFontWithTimerFix
            mov ecx, dword ptr [gSize_font_texture_pages]
            mov eax, dword ptr [gInitial_count_font_texture_pages]
            sub ecx, eax
            mov dword ptr [gInterface_fonts_loaded], 1
            mov dword ptr [gInterface_polyfont_texture_pages], ecx
    L004643eb:
            ret
    }
#endif
}


#ifndef CARPOCALYPSE2_MATCHING
/* ==== direct PIXIES.P16 parser (x64-safe) ====
 * The retail BrPixelmapLoadMany chunk walker misparses multi-chunk font
 * archives on 64-bit hosts, so CreatePolyFont sources glyphs from this
 * instead: big-endian {id,size} chunks; id 0x3d = pixelmap header
 * (type u8, row_bytes/w/h/origin u16 BE, mip u16, asciz name), id 0x21 =
 * pixel block; names are ASCII codes ("65.PIX" = 'A'); ARGB_4444 pixels
 * with magenta chroma key. */
static tPolyPixSet* PolyPixGet(const char* the_path) {
    int i;
    tPolyPixSet* ps;
    FILE* f;
    long sz;
    tU8* buf;
    tU32 nbFiles;
    tU8* rec;
    tU8* cur;
    tU8* p16 = NULL;
    long p16_len = 0;
    tU32 k;
    char (*names)[256];
    tU8** datas;

    for (i = 0; i < 16; i++) {
        if (poly_pix_sets[i] != NULL && strcmp(poly_pix_sets[i]->path, the_path) == 0) {
            return poly_pix_sets[i];
        }
    }
    for (i = 0; i < 16; i++) {
        if (poly_pix_sets[i] == NULL) break;
    }
    if (i == 16) return NULL;
    ps = BrResAllocate(NULL, sizeof(*ps), BR_MEMORY_APPLICATION);
    if (ps == NULL) return NULL;
    strcpy(ps->path, the_path);
    poly_pix_sets[i] = ps;

    f = PFfopen(the_path, "rb"); /* TWT-aware: reads from mounted archive */
    if (f == NULL) return ps;
    fseek(f, 0, SEEK_END);
    sz = ftell(f);
    fseek(f, 0, SEEK_SET);
    buf = BrMemAllocate(sz, BR_MEMORY_APPLICATION);
    if (buf == NULL || fread(buf, 1, sz, f) != (size_t)sz) {
        BrMemFree(buf);
        PFfclose(f);
        return ps;
    }
    PFfclose(f);
    nbFiles = *(tU32*)(buf + 4);
    rec = buf + 8;
    cur = buf + 8 + nbFiles * 56;
    for (k = 0; k < nbFiles; k++) {
        tU32 fsz = *(tU32*)rec;
        const char* nm = (const char*)(rec + 4);
        if (strcmp(nm, "PIXIES.P16") == 0) {
            p16 = cur;
            p16_len = fsz;
        }
        cur += (fsz + 3u) & ~3u;
        rec += 56;
    }
    if (p16 == NULL) {
        BrMemFree(buf);
        return ps;
    }
    memset(ps->glyphs, 0, sizeof(ps->glyphs));
    ps->ascii_offset = 31;
    /* walk chunks */
    {
        long pos = 0;
        int cur_valid = 0;
        int cur_rb = 0, cur_w = 0, cur_h = 0;
        char cur_name[33];
        while (pos + 8 <= p16_len) {
            tU32 cid = ((tU32)p16[pos] << 24) | ((tU32)p16[pos+1] << 16) | ((tU32)p16[pos+2] << 8) | p16[pos+3];
            tU32 csize = ((tU32)p16[pos+4] << 24) | ((tU32)p16[pos+5] << 16) | ((tU32)p16[pos+6] << 8) | p16[pos+7];
            if (pos + 8 + (long)csize > p16_len) break;
            if (cid == 0x3d && csize >= 13) {
                tU8* p = &p16[pos + 8];
                size_t nl;
                cur_rb = (p[1] << 8) | p[2];
                cur_w = (p[3] << 8) | p[4];
                cur_h = (p[5] << 8) | p[6];
                for (nl = 0; nl < 32 && p[13 + nl] != 0; nl++) cur_name[nl] = (char)p[13 + nl];
                cur_name[nl] = '\0';
                cur_valid = (cur_rb > 0 && cur_w > 0 && cur_h > 0 && nl > 0);
            } else if (cid == 0x21 && cur_valid) {
                tU8* px = &p16[pos + 8 + csize - (long)cur_h * cur_rb];
                int x, y;
                int ascii = atoi(cur_name);
                if (ascii >= 0 && ascii < 256) {
                    tPolyPixGlyph* g = &ps->glyphs[ascii];
                    if (g->pixels == NULL) {
                        g->pixels = BrMemAllocate((cur_w * cur_h) * 2, BR_MEMORY_APPLICATION);
                        if (g->pixels != NULL) {
                            g->w = cur_w;
                            g->h = cur_h;
                            for (y = 0; y < cur_h; y++) {
                                for (x = 0; x < cur_w; x++) {
                                    tU16 v = (tU16)(px[y * cur_rb + x * 2] | (px[y * cur_rb + x * 2 + 1] << 8));
                                    unsigned r = (v >> 8) & 0xF;
                                    unsigned gg = (v >> 4) & 0xF;
                                    unsigned b = v & 0xF;
                                    unsigned a = (v >> 12) & 0xF;
                                    tU16 r5, g6, b5;
                                    if (a == 0 || (r == 15 && gg == 0 && b == 15)) {
                                        g->pixels[y * cur_w + x] = 0;
                                        continue;
                                    }
                                    r5 = (tU16)((r << 1 | r >> 3));
                                    g6 = (tU16)((gg << 2 | gg >> 2));
                                    b5 = (tU16)((b << 1 | b >> 3));
                                    g->pixels[y * cur_w + x] = (tU16)((r5 << 11) | (g6 << 5) | b5);
                                }
                            }
                            g->used = 1;
                        }
                    }
                }
                cur_valid = 0;
            } else if (cid == 0) {
                /* separator chunk, keep walking */
            }
            pos += 8 + csize;
        }
    }
    BrMemFree(buf);
    return ps;
}

static br_pixelmap* PolyPixGetGlyphMap(tPolyPixSet* ps, int ascii) {
    tPolyPixGlyph* g;
    br_pixelmap* pm;
    int x, y;
    if (ps == NULL || ascii < 0 || ascii >= 256 || !ps->glyphs[ascii].used) {
        return NULL;
    }
    g = &ps->glyphs[ascii];
    pm = BrPixelmapAllocate(BR_PMT_RGBA_4444, g->w, g->h, NULL, 0);
    if (pm == NULL) return NULL;
    strcpy(pm->identifier, "polypix");
    for (y = 0; y < g->h; y++) {
        br_uint_16* row = (br_uint_16*)((char*)pm->pixels + y * pm->row_bytes);
        for (x = 0; x < g->w; x++) {
            tU16 src = g->pixels[y * g->w + x];
            tU16 r5 = (src >> 11) & 0x1F;
            tU16 g6 = (src >> 5) & 0x3F;
            tU16 b5 = src & 0x1F;
            /* back to 4444 nibbles: r@8-11, g@4-7, b@0-3, alpha@12-15;
             * src==0 is the magenta chroma key -> fully transparent */
            row[x] = src == 0 ? 0 : (tU16)(0xF000 | ((r5 >> 1) << 8) | ((g6 >> 1) << 4) | (b5 >> 1));
        }
    }
    return pm;
}
#endif

// FUNCTION: CARMA2_HW 0x004643f0
void C2_HOOK_FASTCALL CreatePolyFont(const char* pName, int pSize, int pIndex) {
    int tex_x;
    int tex_y;
    tPath_name the_path;
    char s[256];
    char s2[256];
    int i;
    tTWTVFS twt;
    FILE* f;
    br_pixelmap *blank_map;
    int count_characters;
    tPolyFontGlyph* glyph;
    int ascii;
    br_pixelmap* map;

    PathCat(the_path, gApplication_path, gGraf_specs[gGraf_spec_index].data_dir_name);
    PathCat(the_path, the_path, "FONTS");
    PathCat(the_path, the_path, pName);

    twt = OpenPackFileAndSetTiffLoading(the_path);

    strcpy(s, the_path);
    PathCat(s, s, "FONT.TXT");
    tex_x = 0;
    tex_y = 0;
    f = PFfopen(s, "rt");
    if (f == NULL) {
        FatalError(kFatalError_CannotLoadFontWidthTable_S, pName);
    }

    strcpy(gPoly_fonts[pIndex].name, pName);
    /* number of characters */
    gPoly_fonts[pIndex].numberOfCharacters = count_characters = GetAnInt(f);
    /* inter-character spacing */
    gPoly_fonts[pIndex].interCharacterSpacing = GetAnInt(f);
    /* ASCII offset (value of first character) */
    gPoly_fonts[pIndex].asciiOffset = GetAnInt(f);
    /* font character height */
    gPoly_fonts[pIndex].fontCharacterHeight = GetAnInt(f);
    /* Blank width */
    gPoly_fonts[pIndex].widthOfBlank = GetAnInt(f);
    gPoly_fonts[pIndex].fontSize = pSize;
    gPoly_fonts[pIndex].available = 1;
    C2_HOOK_BUG_ON(CARPOCALYPSE2_ASIZE(gPoly_fonts[pIndex].glyphs) != 256);
    for (i = 0; i < CARPOCALYPSE2_ASIZE(gPoly_fonts[pIndex].glyphs); i++) {
        glyph = &gPoly_fonts[pIndex].glyphs[i];

        glyph->model = NULL;
        glyph->material = NULL;
        glyph->used = 0;
        glyph->index = 0;
        glyph->glyph_width = pSize;
    }
    PFfclose(f);
    strcpy(s, the_path);
    PathCat(s, s, "BLANK.PIX");
    blank_map = GetThisFuckingPixelmap(the_path, "BLANK.PIX", 1);
    if (blank_map == NULL) {
        FatalError(kFatalError_CannotLoadFontImage_S, "BLANK.PIX");
    }

    for (i = 0; i < count_characters; i++) {

        strcpy(s, the_path);
        ascii = gPoly_fonts[pIndex].asciiOffset + i;
        sprintf(s2, "%d.PIX", ascii);
        PathCat(s, s, s2);
#ifndef CARPOCALYPSE2_MATCHING
        {
            static tPolyPixSet* poly_ps;
            if (poly_ps == NULL) poly_ps = PolyPixGet(the_path);
            map = PolyPixGetGlyphMap(poly_ps, ascii);
        }
        if (map == NULL)
#endif
        map = GetThisFuckingPixelmap(the_path, s2, 0);
        if (map != NULL) {
            if (gTexture_maps[gSize_font_texture_pages] == NULL) {
                gTexture_maps[gSize_font_texture_pages] = BrPixelmapAllocate(blank_map->type, 64, 64, NULL, 0);
                if (gTexture_maps[gSize_font_texture_pages] == NULL) {
                    FatalError(kFatalError_CouldNotCreateTexturesPages_S, pName);
                }
                BrMapAdd(gTexture_maps[gSize_font_texture_pages]);
            }
            if (map->type != gTexture_maps[gSize_font_texture_pages]->type) {
                printf("FONT:%s  CHAR:%c (%i)\n", pName, ascii, ascii);
                fflush(stdout);
                BrFailure("BLOODY FONTS :(");
            } else {
#ifndef CARPOCALYPSE2_MATCHING
                /* Tint the glyph with this font slot's border colours
                 * (bilinear across the four corners) before packing, so the
                 * text colour matches the retail vertex-colour modulation
                 * without relying on the software vertex pipeline. */
                {
                    int px, py;
                    tRGBColour* tl = &gPoly_font_border_colours[pIndex].tl;
                    tRGBColour* tr = &gPoly_font_border_colours[pIndex].tr;
                    tRGBColour* bl = &gPoly_font_border_colours[pIndex].bl;
                    tRGBColour* br = &gPoly_font_border_colours[pIndex].br;
                    for (py = 0; py < pSize; py++) {
                        for (px = 0; px < pSize; px++) {
                            tU16 v;
                            int r, g, b;
                            float fx = (pSize > 1) ? (float)px / (pSize - 1) : 0.f;
                            float fy = (pSize > 1) ? (float)py / (pSize - 1) : 0.f;
                            v = *(tU16*)((char*)map->pixels + py * map->row_bytes + px * 2);
                            if ((v >> 12) == 0) {
                                continue;
                            }
                            /* multiply, mirroring the retail vertex-colour
                             * modulation (texel * border colour / 255) */
                            r = (int)((((tl->r * (1-fx) + tr->r * fx) * (1-fy)) + ((bl->r * (1-fx) + br->r * fx) * fy)) * ((v >> 8 & 0xF) * 17) / 255.f);
                            g = (int)((((tl->g * (1-fx) + tr->g * fx) * (1-fy)) + ((bl->g * (1-fx) + br->g * fx) * fy)) * ((v >> 4 & 0xF) * 17) / 255.f);
                            b = (int)((((tl->b * (1-fx) + br->b * fx) * (1-fy)) + ((bl->b * (1-fx) + br->b * fx) * fy)) * ((v & 0xF) * 17) / 255.f);
                            if (r > 255) r = 255;
                            if (g > 255) g = 255;
                            if (b > 255) b = 255;
                            v = (tU16)(0xF000 | ((r >> 4) << 8) | ((g >> 4) << 4) | (b >> 4));
                            *(tU16*)((char*)map->pixels + py * map->row_bytes + px * 2) = v;
                        }
                    }
                }
#endif
                DRPixelmapRectangleCopy(
                        gTexture_maps[gSize_font_texture_pages],
                        tex_x, tex_y,
                        map,
                        0, 0,
                        pSize, pSize);
                BrMapUpdate(gTexture_maps[gSize_font_texture_pages], BR_MAPU_ALL);
                gPoly_fonts[pIndex].glyphs[ascii].index = gSize_font_texture_pages;
                gPoly_fonts[pIndex].glyphs[ascii].used = 1;
                BrVector2Set(&gPoly_fonts[pIndex].glyphs[ascii].texCoord, (float) tex_x / 64.0f, (float)tex_y  / 64.0f);
                gPoly_fonts[pIndex].glyphs[ascii].glyph_width = FindCharacterWidth(map);
                gPoly_fonts[pIndex].glyphs[ascii].material = NULL;
                gPoly_fonts[pIndex].glyphs[ascii].model = CreateFontCharacterModel(pIndex, ascii, pSize, pSize, pName);
                tex_x += pSize;
                if (tex_x >= 64) {
                    tex_y += pSize;
                    tex_x = 0;
                    if (tex_y >= 64) {
                        gSize_font_texture_pages += 1;
                        if (gSize_font_texture_pages >= (int)CARPOCALYPSE2_ASIZE(gTexture_maps)) {
                            FatalError(kFatalError_CouldNotCreateTexturesPages_S, pName);
                        }
                        tex_x = 0;
                        tex_y = 0;
                    }
                }
            }
            if (twt < 0) {
                BrPixelmapFree(map);
            }
        }
    }
    KillThePixies();
    ClosePackFileAndSetTiffLoading(twt);
    gSize_font_texture_pages += 1;
}

// FUNCTION: CARMA2_HW 0x00464b80
C2_NAKED br_model* C2_HOOK_FASTCALL CreateCharacterModel(int width, int height, int texture_id_x, int texture_id_y, const char* pageName) {
#ifndef CARPOCALYPSE2_MATCHING
    br_model* pModel;
    br_scalar f_width;
    br_scalar f_map_x1;
    br_scalar f_map_y1;
    br_scalar f_map_x2;
    br_scalar f_map_y2;

    pModel = BrModelAllocate("String Model", 4, 2);
    if (pModel == NULL) {
        FatalError(kFatalError_CouldNotCreateTexturesPages_S, pageName);
    }
    f_width = (br_scalar)width;
    pModel->faces[0].vertices[0] = 0;
    pModel->faces[0].vertices[1] = 1;
    pModel->faces[0].vertices[2] = 2;
    pModel->faces[1].vertices[0] = 1;
    pModel->faces[1].vertices[1] = 3;
    pModel->faces[1].vertices[2] = 2;
    BrVector3Set(&pModel->vertices[0].p, 0.0f, 0.0f, -1.2f);
    BrVector3Set(&pModel->vertices[1].p, f_width, 0.0f, -1.2f);
    BrVector3Set(&pModel->vertices[2].p, 0.0f, (float)-height, -1.2f);
    BrVector3Set(&pModel->vertices[3].p, f_width, (float)-height, -1.2f);
    f_map_x1 = (br_scalar)texture_id_x / 64.0f;
    f_map_y1 = (br_scalar)texture_id_y / 64.0f;
    f_map_x2 = f_map_x1 + f_width / 64.0f;
    f_map_y2 = f_map_y1 + (br_scalar)height / 64.0f;
    BrVector2Set(&pModel->vertices[0].map, f_map_x1, f_map_y1);
    BrVector2Set(&pModel->vertices[1].map, f_map_x2, f_map_y1);
    BrVector2Set(&pModel->vertices[2].map, f_map_x1, f_map_y2);
    BrVector2Set(&pModel->vertices[3].map, f_map_x2, f_map_y2);
    pModel->flags |= BR_MODF_UPDATEABLE;
    BrModelAdd(pModel);
    return pModel;
#else
    __asm {
            sub esp, 8
            push esi
            push edi
            push 2
            mov edi, edx
            push 4
            mov dword ptr [esp + 0x10], ecx
            push offset gDRFont_to_polyfont_mapping+0x2a4
            mov dword ptr [esp + 0x18], edi
            call BrModelAllocate
            mov esi, eax
            add esp, 0xc
            test esi, esi
            jne L00464bb8
            mov eax, dword ptr [esp + 0x1c]
            push eax
            push 0xb7
            call FatalError
            add esp, 8
    L00464bb8:
            mov ecx, dword ptr [esi + 0xc]
            mov eax, 1
            fild dword ptr [esp + 8]
            mov word ptr [ecx], 0
            mov edx, dword ptr [esi + 0xc]
            neg edi
            mov word ptr [edx + 2], ax
            mov ecx, dword ptr [esi + 0xc]
            mov dword ptr [esp + 0x1c], edi
            push esi
            mov word ptr [ecx + 4], 2
            mov edx, dword ptr [esi + 0xc]
            mov word ptr [edx + 0x28], ax
            mov eax, dword ptr [esi + 0xc]
            mov word ptr [eax + 0x2a], 3
            mov ecx, dword ptr [esi + 0xc]
            mov word ptr [ecx + 0x2c], 2
            mov edx, dword ptr [esi + 8]
            mov dword ptr [edx], 0
            mov eax, dword ptr [esi + 8]
            mov dword ptr [eax + 4], 0
            mov ecx, dword ptr [esi + 8]
            mov eax, 0xbf99999a
            mov dword ptr [ecx + 8], eax
            mov edx, dword ptr [esi + 8]
            fst dword ptr [edx + 0x28]
            mov ecx, dword ptr [esi + 8]
            fild dword ptr [esp + 0x20]
            mov dword ptr [ecx + 0x2c], 0
            mov edx, dword ptr [esi + 8]
            mov dword ptr [edx + 0x30], eax
            mov ecx, dword ptr [esi + 8]
            mov dword ptr [ecx + 0x50], 0
            mov edx, dword ptr [esi + 8]
            fst dword ptr [edx + 0x54]
            mov ecx, dword ptr [esi + 8]
            fld st(1)
            mov dword ptr [ecx + 0x58], eax
            mov edx, dword ptr [esi + 8]
            fstp dword ptr [edx + 0x78]
            mov ecx, dword ptr [esi + 8]
            fstp dword ptr [ecx + 0x7c]
            mov edx, dword ptr [esi + 8]
            fmul dword ptr [gUNK_0058a038+0x8]
            mov dword ptr [edx + 0x80], eax
            mov eax, dword ptr [esi + 8]
            fstp dword ptr [esp + 0x20]
            fild dword ptr [esp + 0x10]
            fmul dword ptr [gUNK_0058a038+0x8]
            fild dword ptr [esp + 0x18]
            fmul dword ptr [gUNK_0058a038+0x8]
            fild dword ptr [esp + 0x1c]
            fmul dword ptr [gUNK_0058a038+0x8]
            fst dword ptr [esp + 0x18]
            fld st(1)
            fstp dword ptr [eax + 0xc]
            mov ecx, dword ptr [esi + 8]
            fst dword ptr [ecx + 0x10]
            mov edx, dword ptr [esi + 8]
            fld st(1)
            fadd dword ptr [esp + 0x20]
            fst dword ptr [esp + 0x20]
            fstp dword ptr [edx + 0x34]
            mov eax, dword ptr [esi + 8]
            fstp dword ptr [eax + 0x38]
            mov ecx, dword ptr [esi + 8]
            fstp dword ptr [ecx + 0x5c]
            mov edx, dword ptr [esi + 8]
            mov ecx, dword ptr [esp + 0x20]
            fadd dword ptr [esp + 0x18]
            fst dword ptr [edx + 0x60]
            mov eax, dword ptr [esi + 8]
            mov dword ptr [eax + 0x84], ecx
            mov edx, dword ptr [esi + 8]
            fstp dword ptr [edx + 0x88]
            or byte ptr [esi + 0x20], 0x80
            call BrModelAdd
            add esp, 4
            mov eax, esi
            pop edi
            pop esi
            add esp, 8
            ret 0xc
            nop
            nop
            nop
            nop
            nop
            nop
    }
#endif
}

// FUNCTION: CARMA2_HW 0x00464cf0
br_material* C2_HOOK_FASTCALL CreatePolyMaterial(int pTexture_id) {
    br_material *material;

    material = BrMaterialAllocate("Character Material");
    if (material == NULL) {
        BrFailure("Could not create font material");
    }
    material->flags = BR_MATF_FORCE_FRONT | BR_MATF_ALWAYS_VISIBLE | BR_MATF_SMOOTH | BR_MATF_PRELIT;
    material->colour_map = gTexture_maps[pTexture_id];
    BrMaterialAdd(material);
    return material;
}

// FUNCTION: CARMA2_HW 0x00464d40
void C2_HOOK_FASTCALL ColourVertices(br_model* pModel, int pFont_index) {

    if (pModel == NULL) {
        return;
    }
    pModel->vertices[0].red = gPoly_font_border_colours[pFont_index].tl.r;
    pModel->vertices[0].grn = gPoly_font_border_colours[pFont_index].tl.g;
    pModel->vertices[0].blu = gPoly_font_border_colours[pFont_index].tl.b;
    pModel->vertices[1].red = gPoly_font_border_colours[pFont_index].tr.r;
    pModel->vertices[1].grn = gPoly_font_border_colours[pFont_index].tr.g;
    pModel->vertices[1].blu = gPoly_font_border_colours[pFont_index].tr.b;
    pModel->vertices[2].red = gPoly_font_border_colours[pFont_index].bl.r;
    pModel->vertices[2].grn = gPoly_font_border_colours[pFont_index].bl.g;
    pModel->vertices[2].blu = gPoly_font_border_colours[pFont_index].bl.b;
    pModel->vertices[3].red = gPoly_font_border_colours[pFont_index].br.r;
    pModel->vertices[3].grn = gPoly_font_border_colours[pFont_index].br.g;
    pModel->vertices[3].blu = gPoly_font_border_colours[pFont_index].br.b;
}

// FUNCTION: CARMA2_HW 0x00464df0
void C2_HOOK_FASTCALL SolidPolyFontText(const char* pText, int pX, int pY, int pFont, tJustification pJust, int pRender) {

    CheckAvailabilityOfThisFont(pFont);
    TransparentPolyFontText(pText, pX, pY, pFont, pJust, pRender, 1.0);
}

void C2_HOOK_FASTCALL InitPolyFontMaterials(void) {
    int i;

    for (i = 0; i < (int)CARPOCALYPSE2_ASIZE(gPoly_font_materials); i++) {

        gPoly_font_materials[i] = CreatePolyMaterial(0);
    }
}

// FUNCTION: CARMA2_HW 0x00465250
br_material* C2_HOOK_FASTCALL GetPolyFontMaterial(int pFont_index, char pChar) {
    tU8 uchar;
    br_material* material;
    float f_size;
    tU32 packed_font_character;

    // GLOBAL: CARMA2 0x00686480
    static int poly_font_material_index;

    // GLOBAL: CARMA2 0x00686498
    static tU16 poly_font_material_generation;

    C2_HOOK_BUG_ON(sizeof(tPolyFont) != 0x1c68);
    C2_HOOK_BUG_ON(sizeof(tPolyFontGlyph) != 0x1c);

    uchar = pChar;
    material = gPoly_fonts[pFont_index].glyphs[uchar].material;
    packed_font_character = POLYFONT_FONT_CHARACTER(pFont_index, uchar);
    if (material != NULL && POLYFONT_MATERIAL_GET_FONT_CHARACTER(material) == packed_font_character) {
        material->user = POLYFONT_PACK_USER(packed_font_character, poly_font_material_generation);
        return material;
    }
    // Find "oldest" material, and re-use
    do {
        poly_font_material_index += 1;
        poly_font_material_generation += 1;
        if (poly_font_material_index >= (int)CARPOCALYPSE2_ASIZE(gPoly_font_materials)) {
            poly_font_material_index = 0;
        }
    } while ((poly_font_material_generation - POLYFONT_MATERIAL_GET_GENERATION(gPoly_font_materials[poly_font_material_index])) < (int)CARPOCALYPSE2_ASIZE(gPoly_font_materials));
    material = gPoly_font_materials[poly_font_material_index];
    gPoly_fonts[pFont_index].glyphs[uchar].material = material;
    material->colour_map = gTexture_maps[gPoly_fonts[pFont_index].glyphs[uchar].index];
    material->user = POLYFONT_PACK_USER(packed_font_character, poly_font_material_generation);;
    f_size = (float)gPoly_fonts[pFont_index].fontSize;
    BrMatrix23Scale(&material->map_transform, f_size / 64.0f, f_size / 64.0f);
    material->map_transform.m[2][0] = gPoly_fonts[pFont_index].glyphs[uchar].texCoord.v[0];
    material->map_transform.m[2][1] = gPoly_fonts[pFont_index].glyphs[uchar].texCoord.v[1];
    material->extra_prim = NULL;
    BrMaterialUpdate(material, BR_MATU_ALL);
    return material;
}

// FUNCTION: CARMA2_HW 0x00464e40
void C2_NAKED C2_HOOK_FASTCALL PolyFontText(const char *pText, int pX, int pY, int pFont, tJustification pJust, int pRender) {
#ifndef CARPOCALYPSE2_MATCHING
    int text_len;
    int i;
    float draw_x;
    int draw_y;

    if (!gPoly_fonts[pFont].available) {
        CheckAvailabilityOfThisFont(pFont);
    }
    if (pRender) {
        CleanPolyFontDanglers();
    }
    switch (pJust) {
#ifdef CARPOCALYPSE2_FIX_BUGS
    case eJust_left:
        break;
#endif
    case eJust_centre:
        draw_y = 0;
        if (!gPoly_fonts[pFont].available) {
            CheckAvailabilityOfThisFont(pFont);
        }
        text_len = (int)strlen(pText);
        for (i = 0; i < text_len; i++) {
            tU8 c = pText[i];
            if (c >= 'a' && c <= 'z') {
                c = c - 'a' + 'A';
            }
            if (gPoly_fonts[pFont].glyphs[c].used) {
                draw_y += gPoly_fonts[pFont].glyphs[c].glyph_width;
            } else {
                draw_y += gPoly_fonts[pFont].widthOfBlank;
            }
        }
        if (text_len > 1) {
            pX -= ((text_len - 1) * gPoly_fonts[pFont].interCharacterSpacing + draw_y) / 2;
        } else {
            pX -= draw_y / 2;
        }
        break;
    case eJust_right:
        draw_y = 0;
        if (!gPoly_fonts[pFont].available) {
            CheckAvailabilityOfThisFont(pFont);
        }
        text_len = (int)strlen(pText);
        for (i = 0; i < text_len; i++) {
            tU8 c = pText[i];
            if (c >= 'a' && c <= 'z') {
                c = c - 'a' + 'A';
            }
            if (gPoly_fonts[pFont].glyphs[c].used) {
                draw_y += gPoly_fonts[pFont].glyphs[c].glyph_width;
            } else {
                draw_y += gPoly_fonts[pFont].widthOfBlank;
            }
        }
        if (text_len > 1) {
            pX -= (text_len - 1) * gPoly_fonts[pFont].interCharacterSpacing + draw_y;
        } else {
            pX -= draw_y;
        }
        break;
    }

    text_len = strlen(pText);
    if (gCount_polyfont_glyph_actors + text_len < (int)CARPOCALYPSE2_ASIZE(gPolyfont_glyph_actors)) {
        draw_x = pX;
        draw_y = pY;
        for (i = 0; i < text_len; i++) {
            tU8 c = pText[i];

            if (c == '\r') {
                draw_x = pX;
                draw_y += gPoly_fonts[pFont].fontCharacterHeight;
                continue;
            }
            if (c >= 'a' && c <= 'z') {
                c = c - 'a' + 'A';
            }
            if (gPoly_fonts[pFont].glyphs[c].used) {
                gPolyfont_glyph_actors[gCount_polyfont_glyph_actors]->model = gPoly_fonts[pFont].glyphs[c].model;
                gPolyfont_glyph_actors[gCount_polyfont_glyph_actors]->material = GetPolyFontMaterial(pFont, c);
                BrActorAdd(gString_root_actor, gPolyfont_glyph_actors[gCount_polyfont_glyph_actors]);
                BrVector3Set(&gPolyfont_glyph_actors[gCount_polyfont_glyph_actors]->t.t.translate.t, (float)draw_x, (float)-draw_y, -1.1f);
                gCount_polyfont_glyph_actors += 1;
                draw_x += gPoly_fonts[pFont].glyphs[c].glyph_width + gPoly_fonts[pFont].interCharacterSpacing;
            } else {
                draw_x += gPoly_fonts[pFont].widthOfBlank;
            }
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
#else
    __asm {
            sub esp, 0x10
            push ebx
            mov ebx, dword ptr [esp + 0x1c]
            push ebp
            push esi
            lea eax, [ebx + ebx*4]
            xor esi, esi
            mov ebp, ecx
            push edi
            lea eax, [eax + eax*4]
            mov dword ptr [esp + 0x10], edx
            mov dword ptr [esp + 0x14], ebp
            lea ebx, [ebx + eax*4]
            lea ebx, [ebx + ebx*8]
            shl ebx, 3
            cmp dword ptr [ebx + gPoly_fonts+0x60], esi
            jne L00464e73
            call LoadInterfaceFonts
    L00464e73:
            cmp dword ptr [esp + 0x30], esi
            je L00464e7e
            call CleanPolyFontDanglers
    L00464e7e:
            mov eax, dword ptr [esp + 0x2c]
            dec eax
            je L00464f37
            dec eax
            jne L00464fc4
            mov eax, dword ptr [ebx + gPoly_fonts+0x60]
            mov dword ptr [esp + 0x18], esi
            cmp eax, esi
            jne L00464ea3
            call LoadInterfaceFonts
    L00464ea3:
            mov edi, ebp
            or ecx, 0xffffffff
            xor eax, eax
            xor edx, edx
            repne scasb
            not ecx
            dec ecx
            test ecx, ecx
            jle L00464efc
    L00464eb5:
            mov al, byte ptr [edx + ebp]
            cmp al, 0x61
            mov byte ptr [esp + 0x2c], al
            jb L00464eca
            cmp al, 0x7a
            ja L00464eca
            add al, 0xe0
            mov byte ptr [esp + 0x2c], al
    L00464eca:
            mov eax, dword ptr [esp + 0x2c]
            and eax, 0xff
            mov edi, eax
            shl edi, 3
            sub edi, eax
            lea eax, [ebx + edi*4]
            mov edi, dword ptr [eax + gPoly_fonts+0x6c]
            test edi, edi
            je L00464eef
            mov eax, dword ptr [eax + gPoly_fonts+0x74]
            jmp L00464ef5
    L00464eef:
            mov eax, dword ptr [ebx + gPoly_fonts+0x54]
    L00464ef5:
            add esi, eax
            inc edx
            cmp edx, ecx
            jl L00464eb5
    L00464efc:
            cmp ecx, 1
            jle L00464f1f
            mov eax, dword ptr [ebx + gPoly_fonts+0x48]
            dec ecx
            imul eax, ecx
            add eax, esi
            mov ecx, dword ptr [esp + 0x10]
            cdq
            sub eax, edx
            sar eax, 1
            neg eax
            add ecx, eax
            jmp L00464fc0
    L00464f1f:
            mov eax, dword ptr [esp + 0x18]
            mov ecx, dword ptr [esp + 0x10]
            add eax, esi
            cdq
            sub eax, edx
            sar eax, 1
            neg eax
            add ecx, eax
            jmp L00464fc0
    L00464f37:
            mov eax, dword ptr [ebx + gPoly_fonts+0x60]
            mov dword ptr [esp + 0x18], esi
            cmp eax, esi
            jne L00464f4a
            call LoadInterfaceFonts
    L00464f4a:
            mov edi, ebp
            or ecx, 0xffffffff
            xor eax, eax
            xor edx, edx
            repne scasb
            not ecx
            dec ecx
            test ecx, ecx
            jle L00464fa3
    L00464f5c:
            mov al, byte ptr [edx + ebp]
            cmp al, 0x61
            mov byte ptr [esp + 0x2c], al
            jb L00464f71
            cmp al, 0x7a
            ja L00464f71
            add al, 0xe0
            mov byte ptr [esp + 0x2c], al
    L00464f71:
            mov eax, dword ptr [esp + 0x2c]
            and eax, 0xff
            mov edi, eax
            shl edi, 3
            sub edi, eax
            lea eax, [ebx + edi*4]
            mov edi, dword ptr [eax + gPoly_fonts+0x6c]
            test edi, edi
            je L00464f96
            mov eax, dword ptr [eax + gPoly_fonts+0x74]
            jmp L00464f9c
    L00464f96:
            mov eax, dword ptr [ebx + gPoly_fonts+0x54]
    L00464f9c:
            add esi, eax
            inc edx
            cmp edx, ecx
            jl L00464f5c
    L00464fa3:
            cmp ecx, 1
            jle L00464fb4
            mov eax, dword ptr [ebx + gPoly_fonts+0x48]
            dec ecx
            imul eax, ecx
            jmp L00464fb8
    L00464fb4:
            mov eax, dword ptr [esp + 0x18]
    L00464fb8:
            mov ecx, dword ptr [esp + 0x10]
            add eax, esi
            sub ecx, eax
    L00464fc0:
            mov dword ptr [esp + 0x10], ecx
    L00464fc4:
            mov edi, ebp
            or ecx, 0xffffffff
            xor eax, eax
            mov edx, dword ptr [gCount_polyfont_glyph_actors]
            repne scasb
            not ecx
            dec ecx
            lea eax, [ecx + edx]
            cmp eax, 0x100
            jge L0046523c
            fild dword ptr [esp + 0x10]
            test ecx, ecx
            fstp dword ptr [esp + 0x10]
            mov edx, dword ptr [esp + 0x10]
            mov dword ptr [esp + 0x2c], edx
            jle L0046513a
            mov edi, dword ptr [esp + 0x24]
            mov dword ptr [esp + 0x18], ecx
            mov ebp, 0xbf8ccccd
    L00465009:
            mov eax, dword ptr [esp + 0x14]
            mov dl, byte ptr [eax]
            inc eax
            cmp dl, 0xd
            mov byte ptr [esp + 0x24], dl
            mov dword ptr [esp + 0x14], eax
            jne L00465032
            mov eax, dword ptr [esp + 0x10]
            mov dword ptr [esp + 0x2c], eax
            mov eax, dword ptr [ebx + gPoly_fonts+0x50]
            add edi, eax
            jmp L0046512b
    L00465032:
            cmp dl, 0x61
            jb L00465043
            cmp dl, 0x7a
            ja L00465043
            add dl, 0xe0
            mov byte ptr [esp + 0x24], dl
    L00465043:
            mov eax, dword ptr [esp + 0x24]
            and eax, 0xff
            mov esi, eax
            shl esi, 3
            sub esi, eax
            shl esi, 2
            mov eax, dword ptr [ebx + esi + gPoly_fonts+0x6c]
            test eax, eax
            je L0046511d
            mov ecx, dword ptr [gCount_polyfont_glyph_actors]
            mov eax, dword ptr [ecx*4 + gPolyfont_glyph_actors]
            mov ecx, dword ptr [esi + ebx + gPoly_fonts+0x64]
            mov dword ptr [eax + 0x18], ecx
            mov ecx, dword ptr [esp + 0x28]
            call GetPolyFontMaterial
            mov edx, dword ptr [gCount_polyfont_glyph_actors]
            mov ecx, dword ptr [edx*4 + gPolyfont_glyph_actors]
            mov dword ptr [ecx + 0x1c], eax
            mov edx, dword ptr [gCount_polyfont_glyph_actors]
            mov ecx, dword ptr [gString_root_actor]
            mov eax, dword ptr [edx*4 + gPolyfont_glyph_actors]
            push eax
            push ecx
            call BrActorAdd
            mov edx, dword ptr [gCount_polyfont_glyph_actors]
            mov ecx, dword ptr [esp + 0x34]
            add esp, 8
            mov eax, dword ptr [edx*4 + gPolyfont_glyph_actors]
            mov edx, edi
            neg edx
            mov dword ptr [eax + 0x50], ecx
            mov eax, dword ptr [gCount_polyfont_glyph_actors]
            mov dword ptr [esp + 0x1c], edx
            fild dword ptr [esp + 0x1c]
            mov ecx, dword ptr [eax*4 + gPolyfont_glyph_actors]
            fstp dword ptr [ecx + 0x54]
            mov edx, dword ptr [gCount_polyfont_glyph_actors]
            mov eax, dword ptr [edx*4 + gPolyfont_glyph_actors]
            mov dword ptr [eax + 0x58], ebp
            mov ecx, dword ptr [ebx + gPoly_fonts+0x48]
            mov edx, dword ptr [ebx + esi + gPoly_fonts+0x74]
            mov eax, dword ptr [gCount_polyfont_glyph_actors]
            add ecx, edx
            inc eax
            mov dword ptr [esp + 0x1c], ecx
            mov dword ptr [gCount_polyfont_glyph_actors], eax
            fild dword ptr [esp + 0x1c]
            fadd dword ptr [esp + 0x2c]
            fstp dword ptr [esp + 0x2c]
            jmp L0046512b
    L0046511d:
            fild dword ptr [ebx + gPoly_fonts+0x54]
            fadd dword ptr [esp + 0x2c]
            fstp dword ptr [esp + 0x2c]
    L0046512b:
            mov eax, dword ptr [esp + 0x18]
            dec eax
            mov dword ptr [esp + 0x18], eax
            jne L00465009
    L0046513a:
            mov eax, dword ptr [esp + 0x30]
            test eax, eax
            je L0046523c
            mov edx, dword ptr [gString_root_actor]
            mov eax, dword ptr [gHUD_root]
            push edx
            push eax
            call BrActorAdd
            mov eax, dword ptr [gBack_screen]
            xor esi, esi
            xor edi, edi
            xor ebx, ebx
            movsx ecx, word ptr [eax + 0x38]
            mov si, word ptr [eax + 0x34]
            mov di, word ptr [eax + 0x36]
            movsx edx, word ptr [eax + 0x3a]
            mov bx, word ptr [eax + 0x30]
            mov dword ptr [esp + 0x30], ecx
            xor ebp, ebp
            xor ecx, ecx
            mov bp, word ptr [eax + 0x32]
            mov word ptr [eax + 0x38], cx
            mov eax, dword ptr [gBack_screen]
            mov dword ptr [esp + 0x38], edx
            add esp, 8
            mov word ptr [eax + 0x3a], cx
            mov edx, dword ptr [gBack_screen]
            mov word ptr [edx + 0x30], cx
            mov eax, dword ptr [gBack_screen]
            mov word ptr [eax + 0x32], cx
            mov ecx, dword ptr [gBack_screen]
            mov word ptr [ecx + 0x34], 0x280
            mov edx, dword ptr [gBack_screen]
            mov word ptr [edx + 0x36], 0x1e0
            mov eax, dword ptr [gDepth_buffer]
            mov ecx, dword ptr [gBack_screen]
            mov edx, dword ptr [gHUD_camera]
            push eax
            mov eax, dword ptr [gHUD_root]
            push ecx
            push edx
            push eax
            call BrZbSceneRender
            mov ecx, dword ptr [gBack_screen]
            add esp, 0x10
            mov word ptr [ecx + 0x34], si
            mov edx, dword ptr [gBack_screen]
            mov cx, word ptr [esp + 0x28]
            mov word ptr [edx + 0x36], di
            mov eax, dword ptr [gBack_screen]
            mov word ptr [eax + 0x38], cx
            mov edx, dword ptr [gBack_screen]
            mov ax, word ptr [esp + 0x30]
            mov word ptr [edx + 0x3a], ax
            mov ecx, dword ptr [gBack_screen]
            mov word ptr [ecx + 0x30], bx
            mov edx, dword ptr [gBack_screen]
            mov word ptr [edx + 0x32], bp
            mov eax, dword ptr [gString_root_actor]
            push eax
            call BrActorRemove
            add esp, 4
            call CleanPolyFontDanglers
    L0046523c:
            pop edi
            pop esi
            pop ebp
            pop ebx
            add esp, 0x10
            ret 0x10
    }
#endif
}

// FUNCTION: CARMA2_HW 0x00465380
void C2_NAKED C2_HOOK_FASTCALL TransparentPolyFontText(const char *pText, int pX, int pY, int pFont, tJustification pJust, int pRender, double pOpacity_factor) {
#ifndef CARPOCALYPSE2_MATCHING
    int text_len;
    int i;
    float draw_x;
    int draw_y;
    br_token_value tvs[3];

    CheckAvailabilityOfThisFont(pFont);
    if (pRender) {
        CleanPolyFontDanglers();
    }
    switch (pJust) {
#ifdef CARPOCALYPSE2_FIX_BUGS
    case eJust_left:
        break;
#endif
    case eJust_centre:
        pX -= PolyFontTextWidth(pFont, pText) / 2;
        break;
    case eJust_right:
        pX -= PolyFontTextWidth(pFont, pText);
        break;
    }

    text_len = strlen(pText);
    if (gCount_polyfont_glyph_actors + text_len < (int)CARPOCALYPSE2_ASIZE(gPolyfont_glyph_actors)) {
        tvs[0].t = BRT_BLEND_B;
        tvs[0].v.b = 1;
        tvs[1].t = BRT_OPACITY_X;
        tvs[1].v.x = BR_FIXED_INT(255.0 * pOpacity_factor);
        tvs[2].t = BR_NULL_TOKEN;
        tvs[2].v.u32 = 0;

        draw_x = (float)pX;
        draw_y = pY;
        for (i = 0; i < text_len; i++) {
            tU8 c = pText[i];

            if (c == '\r') {
                draw_x = (float)pX;
                draw_y += gPoly_fonts[pFont].fontCharacterHeight;
                continue;
            }
            if (c >= 'a' && c <= 'z') {
                c = c - 'a' + 'A';
            }
            if (!gPoly_fonts[pFont].glyphs[c].used) {
                draw_x += (float)gPoly_fonts[pFont].widthOfBlank;
                continue;
            }
            gPolyfont_glyph_actors[gCount_polyfont_glyph_actors]->model = gPoly_fonts[pFont].glyphs[c].model;
            gPolyfont_glyph_actors[gCount_polyfont_glyph_actors]->material = GetPolyFontMaterial(pFont, c);
            gPolyfont_glyph_actors[gCount_polyfont_glyph_actors]->material->extra_prim = tvs;
            BrMaterialUpdate(gPolyfont_glyph_actors[gCount_polyfont_glyph_actors]->material, BR_MATU_ALL);
            BrActorAdd(gString_root_actor, gPolyfont_glyph_actors[gCount_polyfont_glyph_actors]);
            BrVector3Set(&gPolyfont_glyph_actors[gCount_polyfont_glyph_actors]->t.t.translate.t, draw_x, (float)-draw_y, -1.1f);
            gCount_polyfont_glyph_actors += 1;
            draw_x += gPoly_fonts[pFont].glyphs[c].glyph_width + gPoly_fonts[pFont].interCharacterSpacing;
        }
        if (pRender) {
            int original_origin_x;
            int original_origin_y;
            int original_base_x;
            int original_base_y;

            BrActorAdd(gHUD_root, gString_root_actor);
            original_origin_x = gBack_screen->origin_x;
            original_origin_y = gBack_screen->origin_y;
            original_base_x = gBack_screen->base_x;
            original_base_y = gBack_screen->base_y;
            gBack_screen->origin_x = 0;
            gBack_screen->origin_y = 0;
            gBack_screen->base_x = 0;
            gBack_screen->base_y = 0;
            BrZbSceneRender(gHUD_root, gHUD_camera, gBack_screen, gDepth_buffer);
            gBack_screen->origin_x = original_origin_x;
            gBack_screen->origin_y = original_origin_y;
            gBack_screen->base_x = original_base_x;
            gBack_screen->base_y = original_base_y;
            BrActorRemove(gString_root_actor);
            CleanPolyFontDanglers();
        }
    }
#else
    __asm {
            sub esp, 0x24
            push ebx
            push ebp
            mov ebp, ecx
            push esi
            mov ecx, dword ptr [esp + 0x38]
            xor esi, esi
            push edi
            mov dword ptr [esp + 0x10], edx
            lea eax, [ecx + ecx*4]
            mov dword ptr [esp + 0x14], ebp
            lea eax, [eax + eax*4]
            lea eax, [ecx + eax*4]
            lea ebx, [eax + eax*8]
            shl ebx, 3
            cmp dword ptr [ebx + gPoly_fonts+0x60], esi
            jne L004653b3
            call LoadInterfaceFonts
    L004653b3:
            cmp dword ptr [esp + 0x44], esi
            je L004653be
            call CleanPolyFontDanglers
    L004653be:
            mov eax, dword ptr [esp + 0x40]
            dec eax
            je L00465477
            dec eax
            jne L00465504
            mov eax, dword ptr [ebx + gPoly_fonts+0x60]
            mov dword ptr [esp + 0x18], esi
            cmp eax, esi
            jne L004653e3
            call LoadInterfaceFonts
    L004653e3:
            mov edi, ebp
            or ecx, 0xffffffff
            xor eax, eax
            xor edx, edx
            repne scasb
            not ecx
            dec ecx
            test ecx, ecx
            jle L0046543c
    L004653f5:
            mov al, byte ptr [edx + ebp]
            cmp al, 0x61
            mov byte ptr [esp + 0x40], al
            jb L0046540a
            cmp al, 0x7a
            ja L0046540a
            add al, 0xe0
            mov byte ptr [esp + 0x40], al
    L0046540a:
            mov eax, dword ptr [esp + 0x40]
            and eax, 0xff
            mov edi, eax
            shl edi, 3
            sub edi, eax
            lea eax, [ebx + edi*4]
            mov edi, dword ptr [eax + gPoly_fonts+0x6c]
            test edi, edi
            je L0046542f
            mov eax, dword ptr [eax + gPoly_fonts+0x74]
            jmp L00465435
    L0046542f:
            mov eax, dword ptr [ebx + gPoly_fonts+0x54]
    L00465435:
            add esi, eax
            inc edx
            cmp edx, ecx
            jl L004653f5
    L0046543c:
            cmp ecx, 1
            jle L0046545f
            mov eax, dword ptr [ebx + gPoly_fonts+0x48]
            dec ecx
            imul eax, ecx
            add eax, esi
            mov ecx, dword ptr [esp + 0x10]
            cdq
            sub eax, edx
            sar eax, 1
            neg eax
            add ecx, eax
            jmp L00465500
    L0046545f:
            mov eax, dword ptr [esp + 0x18]
            mov ecx, dword ptr [esp + 0x10]
            add eax, esi
            cdq
            sub eax, edx
            sar eax, 1
            neg eax
            add ecx, eax
            jmp L00465500
    L00465477:
            mov eax, dword ptr [ebx + gPoly_fonts+0x60]
            mov dword ptr [esp + 0x18], esi
            cmp eax, esi
            jne L0046548a
            call LoadInterfaceFonts
    L0046548a:
            mov edi, ebp
            or ecx, 0xffffffff
            xor eax, eax
            xor edx, edx
            repne scasb
            not ecx
            dec ecx
            test ecx, ecx
            jle L004654e3
    L0046549c:
            mov al, byte ptr [edx + ebp]
            cmp al, 0x61
            mov byte ptr [esp + 0x40], al
            jb L004654b1
            cmp al, 0x7a
            ja L004654b1
            add al, 0xe0
            mov byte ptr [esp + 0x40], al
    L004654b1:
            mov eax, dword ptr [esp + 0x40]
            and eax, 0xff
            mov edi, eax
            shl edi, 3
            sub edi, eax
            lea eax, [ebx + edi*4]
            mov edi, dword ptr [eax + gPoly_fonts+0x6c]
            test edi, edi
            je L004654d6
            mov eax, dword ptr [eax + gPoly_fonts+0x74]
            jmp L004654dc
    L004654d6:
            mov eax, dword ptr [ebx + gPoly_fonts+0x54]
    L004654dc:
            add esi, eax
            inc edx
            cmp edx, ecx
            jl L0046549c
    L004654e3:
            cmp ecx, 1
            jle L004654f4
            mov eax, dword ptr [ebx + gPoly_fonts+0x48]
            dec ecx
            imul eax, ecx
            jmp L004654f8
    L004654f4:
            mov eax, dword ptr [esp + 0x18]
    L004654f8:
            mov ecx, dword ptr [esp + 0x10]
            add eax, esi
            sub ecx, eax
    L00465500:
            mov dword ptr [esp + 0x10], ecx
    L00465504:
            mov edi, ebp
            or ecx, 0xffffffff
            xor eax, eax
            repne scasb
            not ecx
            dec ecx
            mov esi, ecx
            mov ecx, dword ptr [gCount_polyfont_glyph_actors]
            lea edx, [esi + ecx]
            cmp edx, 0x100
            jge L004657a0
            fld qword ptr [esp + 0x48]
            fmul qword ptr [gUNK_0058a038+0x10]
            mov dword ptr [esp + 0x1c], 0x85
            mov dword ptr [esp + 0x20], 1
            mov dword ptr [esp + 0x24], 0xbe
            call _ftol
            fild dword ptr [esp + 0x10]
            shl eax, 0x10
            mov dword ptr [esp + 0x28], eax
            xor eax, eax
            fstp dword ptr [esp + 0x48]
            mov ecx, dword ptr [esp + 0x48]
            cmp esi, eax
            mov dword ptr [esp + 0x2c], eax
            mov dword ptr [esp + 0x30], eax
            mov dword ptr [esp + 0x40], ecx
            jle L004656e6
            mov edi, dword ptr [esp + 0x38]
            mov dword ptr [esp + 0x10], esi
            mov ebp, 0xbf8ccccd
    L00465584:
            mov eax, dword ptr [esp + 0x14]
            mov dl, byte ptr [eax]
            inc eax
            cmp dl, 0xd
            mov byte ptr [esp + 0x38], dl
            mov dword ptr [esp + 0x14], eax
            jne L004655ad
            mov edx, dword ptr [esp + 0x48]
            mov eax, dword ptr [ebx + gPoly_fonts+0x50]
            mov dword ptr [esp + 0x40], edx
            add edi, eax
            jmp L004656d7
    L004655ad:
            cmp dl, 0x61
            jb L004655be
            cmp dl, 0x7a
            ja L004655be
            add dl, 0xe0
            mov byte ptr [esp + 0x38], dl
    L004655be:
            mov eax, dword ptr [esp + 0x38]
            and eax, 0xff
            mov esi, eax
            shl esi, 3
            sub esi, eax
            shl esi, 2
            mov eax, dword ptr [ebx + esi + gPoly_fonts+0x6c]
            test eax, eax
            je L004656c9
            mov eax, dword ptr [gCount_polyfont_glyph_actors]
            mov ecx, dword ptr [eax*4 + gPolyfont_glyph_actors]
            mov eax, dword ptr [esi + ebx + gPoly_fonts+0x64]
            mov dword ptr [ecx + 0x18], eax
            mov ecx, dword ptr [esp + 0x3c]
            call GetPolyFontMaterial
            mov ecx, dword ptr [gCount_polyfont_glyph_actors]
            push 0x7fff
            mov edx, dword ptr [ecx*4 + gPolyfont_glyph_actors]
            mov dword ptr [edx + 0x1c], eax
            mov eax, dword ptr [gCount_polyfont_glyph_actors]
            mov ecx, dword ptr [eax*4 + gPolyfont_glyph_actors]
            lea eax, [esp + 0x20]
            mov edx, dword ptr [ecx + 0x1c]
            mov dword ptr [edx + 0x58], eax
            mov ecx, dword ptr [gCount_polyfont_glyph_actors]
            mov edx, dword ptr [ecx*4 + gPolyfont_glyph_actors]
            mov eax, dword ptr [edx + 0x1c]
            push eax
            call BrMaterialUpdate
            mov ecx, dword ptr [gCount_polyfont_glyph_actors]
            mov eax, dword ptr [gString_root_actor]
            add esp, 8
            mov edx, dword ptr [ecx*4 + gPolyfont_glyph_actors]
            push edx
            push eax
            call BrActorAdd
            mov ecx, dword ptr [gCount_polyfont_glyph_actors]
            mov eax, dword ptr [esp + 0x48]
            add esp, 8
            mov edx, dword ptr [ecx*4 + gPolyfont_glyph_actors]
            mov ecx, edi
            neg ecx
            mov dword ptr [edx + 0x50], eax
            mov edx, dword ptr [gCount_polyfont_glyph_actors]
            mov dword ptr [esp + 0x18], ecx
            fild dword ptr [esp + 0x18]
            mov eax, dword ptr [edx*4 + gPolyfont_glyph_actors]
            fstp dword ptr [eax + 0x54]
            mov ecx, dword ptr [gCount_polyfont_glyph_actors]
            mov edx, dword ptr [ecx*4 + gPolyfont_glyph_actors]
            mov dword ptr [edx + 0x58], ebp
            mov eax, dword ptr [ebx + gPoly_fonts+0x48]
            add eax, dword ptr [ebx + esi + gPoly_fonts+0x74]
            mov dword ptr [esp + 0x18], eax
            mov eax, dword ptr [gCount_polyfont_glyph_actors]
            fild dword ptr [esp + 0x18]
            inc eax
            mov dword ptr [gCount_polyfont_glyph_actors], eax
            fadd dword ptr [esp + 0x40]
            fstp dword ptr [esp + 0x40]
            jmp L004656d7
    L004656c9:
            fild dword ptr [ebx + gPoly_fonts+0x54]
            fadd dword ptr [esp + 0x40]
            fstp dword ptr [esp + 0x40]
    L004656d7:
            mov eax, dword ptr [esp + 0x10]
            dec eax
            mov dword ptr [esp + 0x10], eax
            jne L00465584
    L004656e6:
            mov eax, dword ptr [esp + 0x44]
            test eax, eax
            je L004657a0
            mov ecx, dword ptr [gString_root_actor]
            mov edx, dword ptr [gHUD_root]
            push ecx
            push edx
            call BrActorAdd
            mov eax, dword ptr [gBack_screen]
            xor esi, esi
            xor edi, edi
            xor ecx, ecx
            movsx ebx, word ptr [eax + 0x38]
            movsx ebp, word ptr [eax + 0x3a]
            mov si, word ptr [eax + 0x30]
            mov di, word ptr [eax + 0x32]
            mov word ptr [eax + 0x38], cx
            mov eax, dword ptr [gBack_screen]
            add esp, 8
            mov word ptr [eax + 0x3a], cx
            mov edx, dword ptr [gBack_screen]
            mov word ptr [edx + 0x30], cx
            mov eax, dword ptr [gBack_screen]
            mov word ptr [eax + 0x32], cx
            mov ecx, dword ptr [gDepth_buffer]
            mov edx, dword ptr [gBack_screen]
            mov eax, dword ptr [gHUD_camera]
            push ecx
            mov ecx, dword ptr [gHUD_root]
            push edx
            push eax
            push ecx
            call BrZbSceneRender
            mov edx, dword ptr [gBack_screen]
            add esp, 0x10
            mov word ptr [edx + 0x38], bx
            mov eax, dword ptr [gBack_screen]
            mov word ptr [eax + 0x3a], bp
            mov ecx, dword ptr [gBack_screen]
            mov word ptr [ecx + 0x30], si
            mov edx, dword ptr [gBack_screen]
            mov word ptr [edx + 0x32], di
            mov eax, dword ptr [gString_root_actor]
            push eax
            call BrActorRemove
            add esp, 4
            call CleanPolyFontDanglers
    L004657a0:
            pop edi
            pop esi
            pop ebp
            pop ebx
            add esp, 0x24
            ret 0x18
    }
#endif
}

void C2_HOOK_FASTCALL DrawDigitAt(const br_pixelmap* pSource, br_pixelmap* pDest, br_uint_16 pX, br_uint_16 pY, br_uint_16 pDigit_height, int pDigit) {

    DRPixelmapRectangleMaskedCopy(pDest, pX, pY, pSource, 0, pDigit * pDigit_height ,pSource->width, pDigit_height);
}

// FUNCTION: CARMA2_HW 0x004657c0
void C2_HOOK_FASTCALL DrawNumberAt(const br_pixelmap* pSrc, br_pixelmap* pDest, int pX, int pY, int pX_pitch, int pY_pitch, int pValue, int pDigit_count, int pLeading_zeroes) {
    int i;
    int digit;

    for (i = pDigit_count - 1; i >= 0; i--) {
        digit = pValue % 10;
        pValue /= 10;
        if (pValue != 0 || pLeading_zeroes || i == pDigit_count - 1) {
            DrawDigitAt(pSrc, pDest, pX + pX_pitch * i, pY, pY_pitch, digit);
        }
    }
}

// FUNCTION: CARMA2_HW 0x00465850
void C2_HOOK_FASTCALL LoadFont(int pFont_ID) {
    int i;
    FILE* file;
    tPath_name the_path;

    if (gFonts[pFont_ID].images != NULL) {
        return;
    }
    PathCat(the_path, gApplication_path, gGraf_specs[gGraf_spec_index].data_dir_name);
    PathCat(the_path, the_path, "FONTS");
    PathCat(the_path, the_path, gFont_names[pFont_ID]);

#ifndef CARPOCALYPSE2_MATCHING
    /* Load font from TWT archive: mount <name>.TWT then load PIXIES.P16 */
    {
        tPath_name pix_path;
        tTWTVFS twt;
        extern tTWTVFS C2_HOOK_FASTCALL OpenPackFileAndSetTiffLoading(const char* path);
        extern void C2_HOOK_FASTCALL ClosePackFileAndSetTiffLoading(tTWTVFS twt);
        br_pixelmap* maps[100];
        int count;
        int biggest = -1;
        int bigsz = 0;

        twt = OpenPackFileAndSetTiffLoading(the_path);
        if (twt >= 0) {
            PathCat(pix_path, the_path, "PIXIES.P16");
            count = BrPixelmapLoadMany(pix_path, maps, 100);
            ClosePackFileAndSetTiffLoading(twt);

            for (i = 0; i < count; i++) {
                if (maps[i] != NULL && maps[i]->width * maps[i]->height > bigsz) {
                    bigsz = maps[i]->width * maps[i]->height;
                    biggest = i;
                }
            }
            if (biggest >= 0) {
                gFonts[pFont_ID].images = maps[biggest];
            }
        }
    }
    if (gFonts[pFont_ID].images == NULL) {
        /* Try TIFF from FONTS/tiffrgb/ */
        tPath_name tif_path;
        tPath_name base_path;
        int tif_err;

        strcpy(base_path, the_path);
        the_path[strlen(the_path) - (int)strlen(gFont_names[pFont_ID]) - 1] = '\0';
        PathCat(tif_path, the_path, "tiffrgb");
        PathCat(tif_path, tif_path, gFont_names[pFont_ID]);
        strcat(tif_path, ".tif");
        gFonts[pFont_ID].images = LoadTiff16bit(tif_path, 0, &tif_err);
        if (gFonts[pFont_ID].images != NULL) {
            strcpy(the_path, base_path);
            strcat(the_path, ".PIX");
        }
    }
    if (gFonts[pFont_ID].images == NULL) {
        /* Fallback: try direct .PIX load */
        strcat(the_path, ".PIX");
        gFonts[pFont_ID].images = BrPixelmapLoad(the_path);
    }
    if (gFonts[pFont_ID].images == NULL) {
        FatalError(kFatalError_CannotLoadFontImage_S, gFont_names[pFont_ID]);
    }
    gFonts[pFont_ID].images->row_bytes = (gFonts[pFont_ID].images->row_bytes + 3) & ~3;
    if (the_path[strlen(the_path) - 4] != '.') {
        strcat(the_path, ".PIX");
    }
#else
    strcat(the_path, ".PIX");
#endif

    if (gFonts[pFont_ID].file_read_once) {
        return;
    }
    the_path[strlen(the_path) - 3] = '\0';
    strcat(the_path, "TXT");
    file = DRfopen(the_path, "rt");
    if (file == NULL) {
        FatalError(kFatalError_CannotLoadFontWidthTable_S, gFont_names[pFont_ID]);
    }
    gFonts[pFont_ID].height = GetAnInt(file);
    gFonts[pFont_ID].width = GetAnInt(file);
    gFonts[pFont_ID].spacing = GetAnInt(file);
    gFonts[pFont_ID].offset = GetAnInt(file);
    gFonts[pFont_ID].num_entries = GetAnInt(file);
    gFonts[pFont_ID].id = pFont_ID;
    if (gFonts[pFont_ID].width <= 0) {
        for (i = 0; i < gFonts[pFont_ID].num_entries; i++) {
            gFonts[pFont_ID].width_table[i] = GetAnInt(file);
        }
    }
    PFfclose(file);
    gFonts[pFont_ID].file_read_once = 1;
}

// FUNCTION: CARMA2_HW 0x00465a00
void C2_HOOK_FASTCALL DisposeFont(int pFont_ID) {

    if (gFonts[pFont_ID].images != NULL) {
        BrPixelmapFree(gFonts[pFont_ID].images);
        gFonts[pFont_ID].images = NULL;
        gFonts[pFont_ID].file_read_once = 0;
    }
}

// FUNCTION: CARMA2_HW 0x00465a40
void C2_HOOK_FASTCALL InitDRFonts(void) {
    int i;

    for (i = 0; i < (int)CARPOCALYPSE2_ASIZE(gFonts); i++) {
        gFonts[i].images = NULL;
        gFonts[i].file_read_once = 0;
        gFonts[i].id = 11;
    }
}

// FUNCTION: CARMA2_HW 0x00466210
int C2_HOOK_FASTCALL GetPolyFontIndexToReplaceDRfontWith(const tDR_font* pFont) {

    return gDRFont_to_polyfont_mapping[pFont->id];
}

// FUNCTION: CARMA2_HW 0x00465a70
void C2_HOOK_FASTCALL DRPixelmapText(br_pixelmap* pPixelmap, int pX, int pY, const tDR_font* pFont, const char* pText, int pRight_edge) {
#ifndef CARPOCALYPSE2_MATCHING
    /* Direct software text renderer: reads glyph pixels from the font
     * pixelmap and writes them onto pPixelmap. Bypasses the 3D pipeline
     * so text works without a full BRender renderer. */
    if (pFont == NULL || pFont->images == NULL || pFont->images->pixels == NULL) {
        return;
    }

    {
        br_pixelmap* font_pm = pFont->images;
        int char_h = pFont->height;
        int font_stride = font_pm->row_bytes / 2; /* in u16 units for P16 */
        int x, y, i;
        /* Carma fonts stack the glyphs vertically in a column (one
         * char_h-tall band per glyph); wide maps instead lay the glyphs
         * out in a horizontal strip. */
        int vertical = (font_pm->height > font_pm->width);
        /* In the vertical layout each glyph is offset one pixel to the
         * right inside its char_h-tall band. */
        int col_off = (vertical && font_pm->width > 1) ? 1 : 0;

        for (i = 0; pText[i] != '\0'; i++) {
            unsigned char c = pText[i];
            int char_index;
            int char_x;
            int char_w;

            if (c >= 'a' && c <= 'z') c = c - 'a' + 'A';
            char_index = c - pFont->offset;
            if (char_index < 0 || char_index >= pFont->num_entries) {
                pX += pFont->spacing > 0 ? pFont->spacing : 8;
                continue;
            }

            /* Per-character width from width table or default */
            if (pFont->width > 0) {
                char_w = pFont->width;
            } else if (char_index < (int)CARPOCALYPSE2_ASIZE(pFont->width_table)) {
                char_w = pFont->width_table[char_index];
            } else {
                char_w = 8;
            }
            if (char_w <= 0) char_w = 8;

            char_x = 0;
            {
                int ci;
                for (ci = 0; ci < char_index; ci++) {
                    int w = (pFont->width > 0) ? pFont->width :
                        (ci < (int)CARPOCALYPSE2_ASIZE(pFont->width_table) ? pFont->width_table[ci] : 8);
                    if (w <= 0) w = 8;
                    char_x += w;
                }
            }

            {
                int fy_base = vertical ? char_index * char_h : 0;
                int fx_base = vertical ? col_off : char_x;
                int e;
                for (e = 0; e < char_h && pY + e < pPixelmap->height; e++) {
                    int fy = fy_base + e;
                    if (fy < 0 || fy >= font_pm->height) continue;
                    for (x = 0; x < char_w && pX + x < pPixelmap->width; x++) {
                        int fx = fx_base + x;
                        if (fx < 0 || fx >= font_pm->width) continue;

                        {
                            br_uint_16 src_pixel = ((br_uint_16*)font_pm->pixels)[fy * font_stride + fx];
                            /* Skip transparent/black pixels */
                            if (src_pixel == 0) continue;

                            int dx = pX + x;
                            int dy = pY + e;
                            if (dx >= 0 && dx < pPixelmap->width && dy >= 0 && dy < pPixelmap->height) {
                                ((br_uint_16*)pPixelmap->pixels)[dy * (pPixelmap->row_bytes / 2) + dx] = src_pixel;
                            }
                        }
                    }
                }
            }

            pX += char_w + pFont->spacing;
        }
    }
#else
    PolyFontText(pText, pX, pY, GetPolyFontIndexToReplaceDRfontWith(pFont), eJust_left, gRender_poly_text);
#endif
}

// FUNCTION: CARMA2_HW 0x00465aa0
void C2_NAKED C2_HOOK_FASTCALL DRPixelmapCleverText(br_pixelmap* pPixelmap, int pX, int pY, tDR_font* pFont, const char* pText, int pRight_edge) {
#ifndef CARPOCALYPSE2_MATCHING
    int i;
    char s[512];
    int s_end;
    int x_text;
    int x_cursor;
    int len_text;

    s_end = -1;
    x_text = pX;
    x_cursor = pX;
    len_text = strlen(pText);
    for (i = 0; i < len_text; i++) {

        if (pText[i] < 0) {
            if (s_end >= 0) {
                s[s_end + 1] = '\0';
                PolyFontText(s, x_text, pY, GetPolyFontIndexToReplaceDRfontWith(pFont), eJust_left, gRender_poly_text);
                s_end = -1;
                x_text = x_cursor;
            }
            pY -= (PolyFontHeight(GetPolyFontIndexToReplaceDRfontWith(&gFonts[-pText[i]])) - PolyFontHeight(GetPolyFontIndexToReplaceDRfontWith(pFont))) / 2;
            pFont = &gFonts[-pText[i]];
        } else {
            s_end += 1;
            s[s_end] = pText[i];
            x_cursor += CharacterWidth(GetPolyFontIndexToReplaceDRfontWith(pFont), pText[i]) + GetSpacing(GetPolyFontIndexToReplaceDRfontWith(pFont));
        }
    }
    if (s_end >= 0) {
        s[s_end + 1] = '\0';
        PolyFontText(s, x_text, pY, GetPolyFontIndexToReplaceDRfontWith(pFont), eJust_left, gRender_poly_text);
    }
#else
    __asm {
            sub esp, 0x214
            push ebx
            push ebp
            mov dword ptr [esp + 8], edx
            mov dword ptr [esp + 0xc], edx
            mov edx, dword ptr [esp + 0x228]
            push esi
            push edi
            mov edi, edx
            or ecx, 0xffffffff
            xor eax, eax
            or ebx, 0xffffffff
            mov ebp, edx
            repne scasb
            not ecx
            dec ecx
            mov dword ptr [esp + 0x20], ebp
            test ecx, ecx
            jle L00465c59
            mov edi, dword ptr [esp + 0x22c]
            mov dword ptr [esp + 0x1c], ecx
    L00465ae1:
            mov al, byte ptr [ebp]
            test al, al
            jge L00465bc2
            test ebx, ebx
            jl L00465b28
            mov eax, dword ptr [gRender_poly_text]
            mov byte ptr [esp + ebx + 0x25], 0
            mov ecx, dword ptr [edi + 0x1c]
            push eax
            mov eax, dword ptr [esp + 0x22c]
            push 0
            mov edx, dword ptr [ecx*4 + gDRFont_to_polyfont_mapping]
            lea ecx, [esp + 0x2c]
            push edx
            mov edx, dword ptr [esp + 0x20]
            push eax
            call PolyFontText
            or ebx, 0xffffffff
            mov ecx, dword ptr [esp + 0x10]
            mov dword ptr [esp + 0x14], ecx
    L00465b28:
            movsx eax, byte ptr [ebp]
            mov edx, eax
            mov ebp, offset gFonts
            shl edx, 3
            sub edx, eax
            lea eax, [eax + edx*4]
            shl eax, 5
            sub ebp, eax
            mov ecx, dword ptr [ebp + 0x1c]
            mov eax, dword ptr [ecx*4 + gDRFont_to_polyfont_mapping]
            lea ecx, [eax + eax*4]
            lea edx, [ecx + ecx*4]
            lea eax, [eax + edx*4]
            lea esi, [eax + eax*8]
            shl esi, 3
            mov eax, dword ptr [esi + gPoly_fonts+0x60]
            test eax, eax
            jne L00465b68
            call LoadInterfaceFonts
    L00465b68:
            mov eax, dword ptr [edi + 0x1c]
            mov esi, dword ptr [esi + gPoly_fonts+0x50]
            mov eax, dword ptr [eax*4 + gDRFont_to_polyfont_mapping]
            lea ecx, [eax + eax*4]
            lea ecx, [ecx + ecx*4]
            lea eax, [eax + ecx*4]
            lea edi, [eax + eax*8]
            shl edi, 3
            mov eax, dword ptr [edi + gPoly_fonts+0x60]
            test eax, eax
            jne L00465b96
            call LoadInterfaceFonts
    L00465b96:
            mov ecx, dword ptr [edi + gPoly_fonts+0x50]
            mov eax, esi
            sub eax, ecx
            mov ecx, dword ptr [esp + 0x228]
            cdq
            sub eax, edx
            mov edi, ebp
            mov ebp, dword ptr [esp + 0x20]
            sar eax, 1
            neg eax
            add ecx, eax
            mov dword ptr [esp + 0x228], ecx
            jmp L00465c43
    L00465bc2:
            movsx ecx, al
            inc ebx
            cmp cl, 0x61
            mov byte ptr [esp + 0x18], cl
            mov byte ptr [esp + ebx + 0x24], cl
            mov edx, dword ptr [edi + 0x1c]
            mov eax, dword ptr [edx*4 + gDRFont_to_polyfont_mapping]
            jb L00465be9
            cmp cl, 0x7a
            ja L00465be9
            sub cl, 0x20
            mov byte ptr [esp + 0x18], cl
    L00465be9:
            lea ecx, [eax + eax*4]
            lea ecx, [ecx + ecx*4]
            lea ecx, [eax + ecx*4]
            lea edx, [ecx + ecx*8]
            mov ecx, dword ptr [esp + 0x18]
            and ecx, 0xff
            mov esi, ecx
            shl esi, 3
            shl edx, 3
            sub esi, ecx
            lea ecx, [edx + esi*4]
            mov esi, dword ptr [ecx + gPoly_fonts+0x6c]
            test esi, esi
            je L00465c1e
            mov ecx, dword ptr [ecx + gPoly_fonts+0x74]
            jmp L00465c24
    L00465c1e:
            mov ecx, dword ptr [edx + gPoly_fonts+0x54]
    L00465c24:
            lea esi, [eax + eax*4]
            lea edx, [esi + esi*4]
            lea eax, [eax + edx*4]
            lea eax, [eax + eax*8]
            mov edx, dword ptr [eax*8 + gPoly_fonts+0x48]
            mov eax, dword ptr [esp + 0x10]
            add edx, ecx
            add eax, edx
            mov dword ptr [esp + 0x10], eax
    L00465c43:
            mov eax, dword ptr [esp + 0x1c]
            inc ebp
            dec eax
            mov dword ptr [esp + 0x20], ebp
            mov dword ptr [esp + 0x1c], eax
            jne L00465ae1
            jmp L00465c60
    L00465c59:
            mov edi, dword ptr [esp + 0x22c]
    L00465c60:
            test ebx, ebx
            jl L00465c91
            mov eax, dword ptr [gRender_poly_text]
            mov byte ptr [esp + ebx + 0x25], 0
            mov ecx, dword ptr [edi + 0x1c]
            push eax
            mov eax, dword ptr [esp + 0x22c]
            push 0
            mov edx, dword ptr [ecx*4 + gDRFont_to_polyfont_mapping]
            lea ecx, [esp + 0x2c]
            push edx
            mov edx, dword ptr [esp + 0x20]
            push eax
            call PolyFontText
    L00465c91:
            pop edi
            pop esi
            pop ebp
            pop ebx
            add esp, 0x214
            ret 0x10
            nop
            nop
    }
#endif
}

// FUNCTION: CARMA2_HW 0x00465ca0
int C2_HOOK_FASTCALL PolyFontTextWidth(int pFont, const char* pText) {
    int len;
    int i;
    int text_width;
    int spacing;

    text_width = 0;
    spacing = 0;
    CheckAvailabilityOfThisFont(pFont);
    len = (int)strlen(pText);
    for (i = 0; i < len; i++) {
        text_width += CharacterWidth(pFont, pText[i]);
    }
    if (len > 1) {
        spacing = (len - 1) * gPoly_fonts[pFont].interCharacterSpacing;
    }
    return text_width + spacing;
}

// FUNCTION: CARMA2_HW 0x00465d50
int C2_HOOK_FASTCALL DRTextWidth(const tDR_font* pFont, const char* pText) {

    return PolyFontTextWidth(GetPolyFontIndexToReplaceDRfontWith(pFont), pText);
}

// FUNCTION: CARMA2_HW 0x00465e10
int C2_NAKED C2_HOOK_FASTCALL DRTextCleverWidth(const tDR_font* pFont, const char* pText) {
#ifndef CARPOCALYPSE2_MATCHING
    int polyfont;
    int i;
    int len;
    int result;

    polyfont = gDRFont_to_polyfont_mapping[pFont->id];
    result = 0;
    len = (int)strlen(pText);

    for (i = 0; i < len; i++) {
        tS8 c;
        c = pText[i];
        if (c < 0) {
            polyfont = gDRFont_to_polyfont_mapping[gFonts[-c].id];
        } else {
            int inter;
            inter = 0;
            if (i < len - 1) {
                inter = gPoly_fonts[polyfont].interCharacterSpacing;
            }
            if (c >= 'a' && c <= 'z') {
                c = c - 'a' + 'A';
            }
            if (gPoly_fonts[polyfont].glyphs[c].used) {
                result += gPoly_fonts[polyfont].glyphs[c].glyph_width + inter;
            } else {
                result += gPoly_fonts[polyfont].widthOfBlank + inter;
            }
        }
    }
    return result;
#else
    __asm {
            sub esp, 0xc
            mov eax, dword ptr [ecx + 0x1c]
            push ebx
            push esi
            push edi
            mov esi, dword ptr [eax*4 + gDRFont_to_polyfont_mapping]
            mov edi, edx
            or ecx, 0xffffffff
            xor eax, eax
            repne scasb
            not ecx
            dec ecx
            xor ebx, ebx
            test ecx, ecx
            mov dword ptr [esp + 0xc], 0
            mov dword ptr [esp + 0x14], edx
            jle L00465ef6
            push ebp
    L00465e43:
            mov dl, byte ptr [edx]
            test dl, dl
            jge L00465e6b
            movsx eax, dl
            mov edx, eax
            shl edx, 3
            sub edx, eax
            lea eax, [eax + edx*4]
            mov edx, offset gFonts+0x1c
            shl eax, 5
            sub edx, eax
            mov eax, dword ptr [edx]
            mov esi, dword ptr [eax*4 + gDRFont_to_polyfont_mapping]
            jmp L00465ee3
    L00465e6b:
            lea eax, [ecx - 1]
            cmp ebx, eax
            jge L00465e87
            lea eax, [esi + esi*4]
            lea eax, [eax + eax*4]
            lea eax, [esi + eax*4]
            lea eax, [eax + eax*8]
            mov edi, dword ptr [eax*8 + gPoly_fonts+0x48]
            jmp L00465e89
    L00465e87:
            xor edi, edi
    L00465e89:
            mov al, dl
            cmp al, 0x61
            mov byte ptr [esp + 0x14], al
            jb L00465e9d
            cmp al, 0x7a
            ja L00465e9d
            add al, 0xe0
            mov byte ptr [esp + 0x14], al
    L00465e9d:
            lea eax, [esi + esi*4]
            lea edx, [eax + eax*4]
            lea eax, [esi + edx*4]
            lea edx, [eax + eax*8]
            mov eax, dword ptr [esp + 0x14]
            and eax, 0xff
            mov ebp, eax
            shl ebp, 3
            shl edx, 3
            sub ebp, eax
            lea eax, [edx + ebp*4]
            mov ebp, dword ptr [eax + gPoly_fonts+0x6c]
            test ebp, ebp
            je L00465ed1
            mov eax, dword ptr [eax + gPoly_fonts+0x74]
            jmp L00465ed7
    L00465ed1:
            mov eax, dword ptr [edx + gPoly_fonts+0x54]
    L00465ed7:
            mov edx, dword ptr [esp + 0x10]
            add eax, edi
            add edx, eax
            mov dword ptr [esp + 0x10], edx
    L00465ee3:
            mov edx, dword ptr [esp + 0x18]
            inc ebx
            inc edx
            cmp ebx, ecx
            mov dword ptr [esp + 0x18], edx
            jl L00465e43
            pop ebp
    L00465ef6:
            mov eax, dword ptr [esp + 0xc]
            pop edi
            pop esi
            pop ebx
            add esp, 0xc
            ret
    }
#endif
}

static __inline int IntLineWidth(const tDR_font* pFont, const char* pText);

// FUNCTION: CARMA2_HW 0x00465f10
void C2_HOOK_FASTCALL DRPixelmapCentredText(br_pixelmap* pPixelmap, int pX, int pY, const tDR_font* pFont, const char* pText) {
    int width_over_2;

    width_over_2 = DRTextWidth(pFont, pText) / 2;
    DRPixelmapText(pPixelmap, pX - width_over_2, pY, pFont, pText, pX + width_over_2);
}

static __inline int IntLineWidth(const tDR_font* pFont, const char* pText) {
    int polyfont;
    int len;
    int i;
    int result;
    tU8 c;

    polyfont = gDRFont_to_polyfont_mapping[pFont->id];
    CheckAvailabilityOfThisFont(polyfont);
    len = (int)strlen(pText);
    result = 0;
    for (i = 0; i < len; i++) {
        c = pText[i];
        if (c >= 'a' && c <= 'z') {
            c = c - 'a' + 'A';
        }
        if (gPoly_fonts[polyfont].glyphs[c].used) {
            result += gPoly_fonts[polyfont].glyphs[c].glyph_width;
        } else {
            result += gPoly_fonts[polyfont].widthOfBlank;
        }
    }
    if (len > 1) {
        result += (len - 1) * gPoly_fonts[polyfont].interCharacterSpacing;
    }
    return result;
}

// FUNCTION: CARMA2_HW 0x00466000
void C2_HOOK_FASTCALL OoerrIveGotTextInMeBoxMissus(int pFont_index, const char* pText, br_pixelmap* pPixelmap, int pLeft, int pTop, int pRight, int pBottom, int pCentred) {

    if (pCentred) {
        PolyFontTextInABox(GetPolyFontIndexToReplaceDRfontWith(&gFonts[pFont_index]), pText, pLeft, pTop, pRight, pBottom, eJust_centre, gRender_poly_text);
    } else {
        PolyFontTextInABox(GetPolyFontIndexToReplaceDRfontWith(&gFonts[pFont_index]), pText, pLeft, pTop, pRight, pBottom, eJust_left, gRender_poly_text);
    }
}

// FUNCTION: CARMA2_HW 0x00466050
br_font* C2_HOOK_FASTCALL LoadBRFont(const char* pName) {
    FILE* f;
    tPath_name the_path;
    br_font* the_font;
    tU32 data_size;
    int i;

    C2_HOOK_BUG_ON(sizeof(br_font) != 24);

    PathCat(the_path, gApplication_path, gGraf_specs[gGraf_spec_index].data_dir_name);
    PathCat(the_path, the_path, "FONTS");
    PathCat(the_path, the_path, pName);
    f = DRfopen(the_path, "rb");
    PossibleService();
    the_font = BrMemAllocate(sizeof(br_font), kMem_br_font);

    // we read 0x18 bytes as that is the size of the struct in 32 bit code.
    PFfread(the_font, 0x18, 1, f);
#if !defined(C2_BIG_ENDIAN)
    the_font->flags = BrSwap32(the_font->flags);

    // swap endianness
    the_font->glyph_x = the_font->glyph_x >> 8 | the_font->glyph_x << 8;
    the_font->glyph_y = the_font->glyph_y >> 8 | the_font->glyph_y << 8;
    the_font->spacing_x = the_font->spacing_x >> 8 | the_font->spacing_x << 8;
    the_font->spacing_y = the_font->spacing_y >> 8 | the_font->spacing_y << 8;
#endif

    // data_size = 256 * sizeof(br_int_8);
    the_font->width = BrMemAllocate(256 * sizeof(br_int_8), kMem_br_font_wid);
    PFfread(the_font->width, 256 * sizeof(br_int_8), 1, f);
    // data_size = 256 * sizeof(br_uint_16);
    the_font->encoding = BrMemAllocate(256 * sizeof(br_uint_16), kMem_br_font_enc);
    PFfread(the_font->encoding, 256 * sizeof(br_uint_16), 1, f);
#if !defined(C2_BIG_ENDIAN)
    for (i = 0; i < 256; i++) {
        the_font->encoding[i] = the_font->encoding[i] >> 8 | the_font->encoding[i] << 8;
    }
#endif
    PossibleService();
    PFfread(&data_size, sizeof(tU32), 1, f);
#if !defined(C2_BIG_ENDIAN)
    data_size = BrSwap32(data_size);
#endif
    PossibleService();
    the_font->glyphs = BrMemAllocate(data_size, kMem_br_font_glyphs);
    PFfread(the_font->glyphs, data_size, 1u, f);
    PFfclose(f);
    return the_font;
}

// FUNCTION: CARMA2_HW 0x004661f0
br_pixelmap* C2_HOOK_FASTCALL FUN_004661f0(void) {
    br_pixelmap* pm;

    pm = LoadPixelmap("CHRMFONT.PIX");
    if (pm == NULL) {
        FatalError(kFatalError_CannotLoadChromeFontImage);
    }
    return pm;
}

// FUNCTION: CARMA2_HW 0x00466220
br_model* C2_HOOK_FASTCALL CloneModel(br_model* pModel) {
    br_model* new_model;
    br_face* face;
    int i;

    new_model = BrModelAllocate(pModel->identifier, pModel->nvertices, pModel->nfaces);
    memcpy(new_model->vertices, pModel->vertices, pModel->nvertices * sizeof(br_vertex));
    memcpy(new_model->faces, pModel->faces, pModel->nfaces * sizeof(br_face));
    i = 0;
    face = new_model->faces;
    if (i < new_model->nfaces) {
        do {
            face->material = NULL;
            i++;
            face++;
        } while (i < new_model->nfaces);
    }
    new_model->flags = pModel->flags;
    BrModelAdd(new_model);
    return new_model;
}

// FUNCTION: CARMA2_HW 0x004657b0
void C2_HOOK_FASTCALL FUN_004657b0(void) {

}
