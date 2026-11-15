#include "cutscene.h"

#include "drmem.h"
#include "errors.h"
#include "flicplay.h"
#include "globvars.h"
#include "globvrbm.h"
#include "graphics.h"
#include "input.h"
#include "sound.h"
#include "utility.h"

extern void C2_HOOK_FASTCALL FUN_00461980(int pSound_ID, tU32 pSound_time);

#include <brender/brender.h>

#include "carpocalypse2_macros.h"
#include "carpocalypse2_types.h"

#include "platform.h"

#include "c2_string.h"
#include "smack.h"
#include "cutscene.h"

// DoSCILogo

// DoStainlessLogo

// PlaySmackerFile

// DoOpeningAnimation

// DoEndRaceAnimation

// DoGameCompletedAnimation

// StartLoadingScreen
#define SMACKER_INTERPOLATE
#define SMACKER_ANTIALIASING
// GLOBAL: CARMA2_HW 0x0079eca4
br_actor* gSmackerActor;

// GLOBAL: CARMA2_HW 0x0079ecac
br_model* gSmackerModel;

// GLOBAL: CARMA2_HW 0x0079eca8
br_pixelmap* gSmackerCurrentPalette;

// GLOBAL: CARMA2_HW 0x0079ec9c
br_pixelmap* gSmackerBackPalette;

// GLOBAL: CARMA2_HW 0x0079eca0
br_pixelmap* gSmackerPrevBackPalette;

// GLOBAL: CARMA2_HW 0x0079ec60
br_material* gSmackMaterials[15];


// GLOBAL: CARMA2_HW 0x0067c3b0
void* gSmackerBuffer;
#ifdef CARPOCALYPSE2_MATCHING
extern int C2_HOOK_FASTCALL AnyKeyDown(void);
extern br_actor* C2_HOOK_CDECL BrActorAdd();
extern br_actor* C2_HOOK_CDECL BrActorAllocate();
extern void C2_HOOK_CDECL BrActorFree();
extern br_actor* C2_HOOK_CDECL BrActorRemove();
extern br_pixelmap* C2_HOOK_CDECL BrMapAdd();
extern br_pixelmap* C2_HOOK_CDECL BrMapRemove();
extern void C2_HOOK_CDECL BrMapUpdate();
extern br_material* C2_HOOK_CDECL BrMaterialAdd();
extern br_material* C2_HOOK_CDECL BrMaterialAllocate();
extern br_material* C2_HOOK_CDECL BrMaterialRemove();
extern void C2_HOOK_CDECL BrMaterialUpdate();
extern void* C2_HOOK_CDECL BrMemAllocate();
extern br_model* C2_HOOK_CDECL BrModelAdd();
extern br_model* C2_HOOK_CDECL BrModelAllocate();
extern void C2_HOOK_CDECL BrModelFree();
extern br_model* C2_HOOK_CDECL BrModelRemove();
extern br_pixelmap* C2_HOOK_CDECL BrPixelmapAllocate();
extern void C2_HOOK_CDECL BrPixelmapFill();
extern void C2_HOOK_CDECL BrPixelmapFree();
extern void C2_HOOK_CDECL BrZbSceneRender();
extern void C2_HOOK_FASTCALL ClearEntireScreen(void);
extern br_pixelmap* C2_HOOK_FASTCALL DRPixelmapAllocate(br_uint_8 pType, br_uint_16 pW, br_uint_16 pH, void* pPixels, int pFlags);
extern void C2_HOOK_FASTCALL DRSetPalette(br_pixelmap* pThe_palette);
extern void C2_HOOK_FASTCALL FadePaletteDown(void);
extern int GetRegisterSourceLocation(char* pSource_location);
extern int C2_HOOK_FASTCALL PDCheckDriveExists(const char* pThe_path);
extern void C2_HOOK_FASTCALL PDEnterDebugger(const char* pStr);
extern tU32 C2_HOOK_FASTCALL PDGetTotalTime(void);
extern void C2_HOOK_FASTCALL PDScreenBufferSwap(int pRendering_area_only);
extern int C2_HOOK_FASTCALL PDServiceSystem(tU32 pTime_since_last_call);
extern void C2_HOOK_FASTCALL PathCat(char* pDestn_str, const char* pStr_1, const char* pStr_2);
extern void C2_HOOK_FASTCALL StopMusic(void);
extern void C2_HOOK_CDECL dr_dprintf();
extern br_actor* g2d_camera;
extern tPath_name gApplication_path;
extern br_pixelmap* gBack_screen;
extern br_pixelmap* gDepth_buffer;
extern const char* gDir_separator;
extern void* gHWnd;
extern int gNbPixelBits;
extern int gNoCutscenes;
extern int gSound_override;
#endif
#ifdef CARPOCALYPSE2_MATCHING
extern char gData_00589274[];
extern char gData_00589278[];
extern char gData_0058927c[];
extern char gData_00589280[];
extern char gData_00589284[];
extern char gData_00589288[];
extern char gData_0058928c[];
extern char gData_00589290[];
extern char gData_00589294[];
extern char gData_00589298[];
extern char gData_0058929c[];
extern char gData_005892a0[];
extern char gData_005892a4[];
extern char gData_0058ff80[];
extern char gFloat_00589b4c[];
extern char gFloat_0058fe0c[];
extern char gFloat_0058fe20[];
extern char gFloat_0058fe40[];
extern char gFloat_0058fe90[];
extern char gFloat_0058fec4[];
extern char gFloat_0058fedc[];
extern char gFloat_0058fef8[];
extern char gFloat_0058ff14[];
extern char gFloat_0058ff54[];
extern char gString_0058fe88[];
extern char gString_0058fea8[];
extern char gString_0058feb8[];
#endif
#ifdef CARPOCALYPSE2_MATCHING
extern char gMatchData_00589274[];
extern char gMatchData_00589278[];
extern char gMatchData_0058927c[];
extern char gMatchData_00589280[];
extern char gMatchData_00589284[];
extern char gMatchData_00589288[];
extern char gMatchData_0058928c[];
extern char gMatchData_00589290[];
extern char gMatchData_00589294[];
extern char gMatchData_00589298[];
extern char gMatchData_0058929c[];
extern char gMatchData_005892a0[];
extern char gMatchData_005892a4[];
extern char gMatchData_0058ff80[];
extern char gMatchFloat_00589b4c[];
extern char gMatchFloat_0058fe0c[];
extern char gMatchFloat_0058fe20[];
extern char gMatchFloat_0058fe40[];
extern char gMatchFloat_0058fe90[];
extern char gMatchFloat_0058fec4[];
extern char gMatchFloat_0058fedc[];
extern char gMatchFloat_0058fef8[];
extern char gMatchFloat_0058ff14[];
extern char gMatchFloat_0058ff54[];
extern char gMatchString_0058fe88[];
extern char gMatchString_0058fea8[];
extern char gMatchString_0058feb8[];
#endif
// FUNCTION: CARMA2_HW 0x0043e750
void C2_HOOK_FASTCALL DoSCILogo(void) {
}

// FUNCTION: CARMA2_HW 0x0043e760
void C2_HOOK_FASTCALL DoStainlessLogo(void) {
}

// FUNCTION: CARMA2_HW 0x0043e770
void C2_NAKED C2_HOOK_FASTCALL PlaySmackerFile(const char* pSmack_name) {
#ifndef CARPOCALYPSE2_MATCHING
    int mati;
    int i;
    int j;
    int k;
    int currentFrame;
    tU32 timeStart, timeEnd;
    int paletteChangeCount;
    int frameCount;
    int frameStart;
    char *palette_pixels;
    int vertex_start;
    int face_start;
    int smackBufferFlags;
    int smackBlitFlags;
    tPath_name the_path;
    HSMACK smackHandle;
    HSMACKBLIT smackBlitHandle;
    int paletteChanged;
    br_pixelmap *tmpPalette;
    br_scalar l, r;
    br_scalar b, u;
    br_uint_32 paletteColour;
    int keyPressed;
    int buffer_start;
    int current_buffer_start;
    char *material_pixels;
    br_uint_8 *current_buffer;

    smackBlitHandle = NULL;
    paletteChangeCount = 0;
    frameCount = 0;
    if (gSound_override) {
        return;
    }
    if (gNoCutscenes) {
        return;
    }
    palette_pixels = NULL; /* Added by carpocalypse2 */
    if (gBack_screen->type == BR_PMT_INDEX_8) {
        smackBufferFlags = 0;
        palette_pixels = gCurrent_palette->pixels;
        smackBlitFlags = 0; /** Added by carpocalypse2 */
    } else if (gBack_screen->type == BR_PMT_RGB_555) {
        smackBufferFlags = SMACKBUFFER555;
        smackBlitFlags = SMACKBUFFER555 | SMACKBLIT2X;
    } else if (gBack_screen->type == BR_PMT_RGB_565) {
        smackBufferFlags = SMACKBUFFER565;
        smackBlitFlags = SMACKBUFFER565 | SMACKBLIT2X;
    } else {
        PDEnterDebugger("Unexpected backscreen format for Smacker player");
        return;
    }

#define SMACK_WIDTH     320
#define SMACK_HEIGHT    200

    if (gSmackerBuffer == NULL) {
        gSmackerBuffer = BrMemAllocate(SMACK_WIDTH * SMACK_HEIGHT, kMem_misc);
    }

    StopMusic();
    FadePaletteDown();
    ClearEntireScreen();

    SmackUseMMX(1);
    SmackSoundUseDirectSound(NULL);

    PathCat(the_path, gApplication_path, "CUTSCENE");
    PathCat(the_path, the_path, pSmack_name);
    dr_dprintf("Trying to open smack file '%s'", the_path);

    smackHandle = SmackOpen(the_path, SMACKTRACKS, SMACKPRELOADALL);
    if (smackHandle == NULL) {
        smackHandle = SmackOpen(the_path, SMACKTRACKS, -1);
    }
    if (smackHandle == NULL) {
        dr_dprintf("Unable to open smack file - attempt to load smack from CD...");
        if (!PDReadSourceLocation(the_path)) {
            dr_dprintf("Can't get CD directory name");
        } else {
            if (the_path[strlen(the_path) - 1] != gDir_separator[0]) {
                strcat(the_path, gDir_separator);
            }
            strcat(the_path, "DATA");
            PathCat(the_path, the_path, "CUTSCENE");
            PathCat(the_path, the_path, pSmack_name);
            dr_dprintf("Got CD path: '%s'", the_path);
            if (PDCheckDriveExists(the_path)) {
                smackHandle = SmackOpen(the_path, SMACKTRACKS, SMACKPRELOADALL);
                if (smackHandle == NULL) {
                    smackHandle = SmackOpen(the_path, SMACKTRACKS, -1);
                }
            }
        }
    }
    if (smackHandle == NULL) {
        dr_dprintf("Smack file '%s' failed to open", pSmack_name);
        PDServiceSystem(0);
        return;
    }
    dr_dprintf("Smack file opened OK");
    timeStart = PDGetTotalTime();
    if (gNbPixelBits == 16) {
        gSmackerActor = BrActorAllocate(BR_ACTOR_MODEL, NULL);
        gSmackerActor->render_style = BR_RSTYLE_FACES;
        gSmackerCurrentPalette = DRPixelmapAllocate(BR_PMT_RGBX_888, 1, 256, NULL, 0);
        gSmackerBackPalette = DRPixelmapAllocate(BR_PMT_RGBX_888, 1, 256, NULL, 0);
        BrMapAdd(gSmackerCurrentPalette);

#define SMACKER_NMATERIALS 15
#define SMACKER_NVERTICES (4 * SMACKER_NMATERIALS)  // 60
#define SMACKER_NFACES (2 * SMACKER_NMATERIALS)  // 30

        gSmackerModel = BrModelAllocate("Smack Model", SMACKER_NVERTICES, SMACKER_NFACES);
        if (gSmackerModel == NULL) {
            PDEnterDebugger("No Smack model");
            BrActorFree(gSmackerActor);
            gSmackerActor = NULL;
            BrMapRemove(gSmackerCurrentPalette);
            BrPixelmapFree(gSmackerCurrentPalette);
            goto load_failed;
        }
        for (i = 0; i < CARPOCALYPSE2_ASIZE(gSmackMaterials); i++) {
            gSmackMaterials[i] = BrMaterialAllocate("Devious Smack Material");
            if (gSmackMaterials[i] == NULL) {
                PDEnterDebugger("No Smack material");
                BrActorFree(gSmackerActor);
                gSmackerActor = NULL;
                BrMapRemove(gSmackerCurrentPalette);
                BrPixelmapFree(gSmackerCurrentPalette);
                goto load_failed;
            }
            gSmackMaterials[i]->colour_map = BrPixelmapAllocate(BR_PMT_INDEX_8, 64, 64, NULL, 0);
            gSmackMaterials[i]->colour_map->map = gSmackerCurrentPalette;
            gSmackMaterials[i]->flags = BR_MATF_ALWAYS_VISIBLE | BR_MATF_FORCE_FRONT;
#if defined(SMACKER_INTERPOLATE)
            gSmackMaterials[i]->flags |= BR_MATF_MAP_INTERPOLATION;
#endif
#if defined(SMACKER_ANTIALIASING)
            gSmackMaterials[i]->flags |= BR_MATF_MAP_ANTIALIASING;
#endif
            BrMapAdd(gSmackMaterials[i]->colour_map->map);
            BrMapAdd(gSmackMaterials[i]->colour_map);
            BrMaterialAdd(gSmackMaterials[i]);
        }
        for (mati = 0, i = 0, vertex_start = 0, face_start = 0; mati < CARPOCALYPSE2_ASIZE(gSmackMaterials); i++) {
            b = (float) (-128 * i - 48);
            u = (float) (-128 * (i + 1) - 48);
            for (j = 0; j < 5; mati++, j++, vertex_start += 4, face_start += 2) {  // 5 == SMACK_WIDTH / 64
                l = (float) (128 * j);
                r = (float) (128 * (j + 1));
                gSmackerModel->vertices[vertex_start + 0].p.v[0] = l;
                gSmackerModel->vertices[vertex_start + 0].p.v[1] = u;
                gSmackerModel->vertices[vertex_start + 0].p.v[2] = -2.f;
                gSmackerModel->vertices[vertex_start + 0].map.v[0] = 0.0078125f;
                gSmackerModel->vertices[vertex_start + 0].map.v[1] = 0.9921875f;

                gSmackerModel->vertices[vertex_start + 1].p.v[0] = r;
                gSmackerModel->vertices[vertex_start + 1].p.v[1] = u;
                gSmackerModel->vertices[vertex_start + 1].p.v[2] = -2.f;
                gSmackerModel->vertices[vertex_start + 1].map.v[0] = 0.9921875f;
                gSmackerModel->vertices[vertex_start + 1].map.v[1] = 0.9921875f;

                gSmackerModel->vertices[vertex_start + 2].p.v[0] = r;
                gSmackerModel->vertices[vertex_start + 2].p.v[1] = b;
                gSmackerModel->vertices[vertex_start + 2].p.v[2] = -2.f;
                gSmackerModel->vertices[vertex_start + 2].map.v[0] = 0.9921875f;
                gSmackerModel->vertices[vertex_start + 2].map.v[1] = 0.0078125f;

                gSmackerModel->vertices[vertex_start + 3].p.v[0] = l;
                gSmackerModel->vertices[vertex_start + 3].p.v[1] = b;
                gSmackerModel->vertices[vertex_start + 3].p.v[2] = -2.f;
                gSmackerModel->vertices[vertex_start + 3].map.v[0] = 0.0078125f;
                gSmackerModel->vertices[vertex_start + 3].map.v[1] = 0.0078125f;

                gSmackerModel->faces[face_start + 0].vertices[0] = vertex_start + 0;
                gSmackerModel->faces[face_start + 0].vertices[1] = vertex_start + 3;
                gSmackerModel->faces[face_start + 0].vertices[2] = vertex_start + 1;
                gSmackerModel->faces[face_start + 0].smoothing = 0;
                gSmackerModel->faces[face_start + 0].material = gSmackMaterials[mati];
                gSmackerModel->faces[face_start + 0].flags = 0;

                gSmackerModel->faces[face_start + 1].vertices[0] = vertex_start + 1;
                gSmackerModel->faces[face_start + 1].vertices[1] = vertex_start + 3;
                gSmackerModel->faces[face_start + 1].vertices[2] = vertex_start + 2;
                gSmackerModel->faces[face_start + 1].smoothing = 0;
                gSmackerModel->faces[face_start + 1].material = gSmackMaterials[mati];
                gSmackerModel->faces[face_start + 1].flags = 0;
            }
        }
        BrModelAdd(gSmackerModel);
        gSmackerActor->model = gSmackerModel;
        BrActorAdd(g2d_camera, gSmackerActor);
        load_failed:
        if (gSmackerActor != NULL) {
            if (SmackBufferOpen(gHWnd, SMACKFRAMERATE, SMACK_WIDTH, SMACK_HEIGHT, SMACK_WIDTH, SMACK_WIDTH) == 0) {
                PDEnterDebugger("sbuf");
            }
            SmackToBuffer(smackHandle, 0, 0, SMACK_WIDTH, SMACK_WIDTH, gSmackerBuffer, 0);
            if (smackHandle->Frames != 0) {
                for (currentFrame = 1; currentFrame != smackHandle->Frames; currentFrame++) {
                    frameStart = PDGetTotalTime();
                    if (smackHandle->NewPalette) {
                        BrMapRemove(gSmackerCurrentPalette);
                        gSmackerPrevBackPalette = gSmackerBackPalette;
                        tmpPalette = gSmackerCurrentPalette;
                        gSmackerCurrentPalette = gSmackerBackPalette;
                        gSmackerBackPalette = tmpPalette;
                        for (i = 0; i < 256; i++) {
                            paletteColour = BR_COLOUR_RGB(smackHandle->Palette[3 * i], smackHandle->Palette[3 * i + 1],
                                                          smackHandle->Palette[3 * i + 2]);
                            if (paletteColour == 0) {
                                paletteColour = BR_COLOUR_RGB(4, 4, 4);
                            }
                            ((br_uint_32 *) gSmackerCurrentPalette->pixels)[i] = paletteColour;
                        }
                        BrMapAdd(gSmackerCurrentPalette);
                        for (i = 0; i < CARPOCALYPSE2_ASIZE(gSmackMaterials); i++) {
                            gSmackMaterials[i]->colour_map->map = gSmackerCurrentPalette;
                            BrMapUpdate(gSmackMaterials[i]->colour_map->map, BR_MAPU_ALL);
                            BrMapUpdate(gSmackMaterials[i]->colour_map, BR_MAPU_ALL);
                            BrMaterialUpdate(gSmackMaterials[i], BR_MATU_ALL);
                        }
                    }
                    SmackDoFrame(smackHandle);
                    BrPixelmapFill(gDepth_buffer, -1);
                    for (i = 0, buffer_start = 0;
                         i < CARPOCALYPSE2_ASIZE(gSmackMaterials); buffer_start += 64 * SMACK_WIDTH) {
                        current_buffer_start = buffer_start;
                        for (j = 0; j < 5; j++, current_buffer_start += 64, i++) {
                            material_pixels = gSmackMaterials[i]->colour_map->pixels;
                            current_buffer = (br_uint_8 *) gSmackerBuffer + current_buffer_start;
                            for (k = 0; k < 64; k++) {
                                memcpy(material_pixels + 64 * k, current_buffer + 320 * k, 64);
                            }
                            BrMapUpdate(gSmackMaterials[i]->colour_map, BR_MAPU_ALL);
                            BrMaterialUpdate(gSmackMaterials[i], BR_MATU_ALL);
                        }
                    }
                    BrZbSceneRender(g2d_camera, g2d_camera, gBack_screen, gDepth_buffer);
                    PDScreenBufferSwap(0);
                    PDServiceSystem(0);
                    if (currentFrame != smackHandle->Frames) {
                        SmackNextFrame(smackHandle);
                    }
                    keyPressed = 0;
                    while (SmackWait(smackHandle) && PDGetTotalTime() < frameStart + 67u) {
                        keyPressed = AnyKeyDown();
                        if (keyPressed) {
                            break;
                        }
                    }
                    if (keyPressed) {
                        break;
                    }
                }
            }
            if (gSmackerActor != NULL) {
                for (i = 0; i < CARPOCALYPSE2_ASIZE(gSmackMaterials); i++) {
                    BrMapRemove(gSmackMaterials[i]->colour_map);
                    BrPixelmapFree(gSmackMaterials[i]->colour_map);
                    gSmackMaterials[i]->colour_map = NULL;
                    BrMaterialRemove(gSmackMaterials[i]);
                    gSmackMaterials[i] = NULL;
                }
                BrModelRemove(gSmackerModel);
                BrActorRemove(gSmackerActor);
                BrModelFree(gSmackerModel);
                BrActorFree(gSmackerActor);
            }
        }
        FadePaletteDown();
        ClearEntireScreen();
    } else if (gBack_screen->type == BR_PMT_RGB_565 || gBack_screen->type == BR_PMT_RGB_555) {
        smackBlitHandle = SmackBlitOpen(smackBlitFlags);
        if (smackBlitHandle != NULL) {
            if (smackHandle->Frames != 0) {
                for (currentFrame = 1; currentFrame != smackHandle->Frames; currentFrame++) {
                    frameStart = PDGetTotalTime();
                    frameCount++;
                    if (gBack_screen->type == BR_PMT_RGB_565 ||
                        gBack_screen->type == BR_PMT_RGB_555) {
                        SmackToBuffer(smackHandle, 0, 0, 320, 200, gSmackerBuffer, 0);
                    } else {
                        SmackToBuffer(smackHandle, 0, 0, gBack_screen->row_bytes,
                                      gBack_screen->height, gBack_screen->pixels, smackBufferFlags);
                    }
                    paletteChanged = 0;
                    if (smackHandle->NewPalette) {
                        paletteChanged = 1;
                        paletteChangeCount += 1;
                        if (gBack_screen->type == BR_PMT_RGB_565 ||
                            gBack_screen->type == BR_PMT_RGB_555) {
                            SmackBlitSetPalette(smackBlitHandle, smackHandle->Palette, smackHandle->PalType);
                        } else {
                            /* Check added by carpocalypse2 */
                            if (palette_pixels == NULL) {
                                abort();
                            }
                            for (i = 0; i < 256; i++) {
                                ((br_uint_32 *) palette_pixels)[i] = BR_COLOUR_RGB(smackHandle->Palette[3 * i + 0],
                                                                                   smackHandle->Palette[3 * i + 1],
                                                                                   smackHandle->Palette[3 * i + 2]);
                            }
                        }
                    }
                    SmackDoFrame(smackHandle);
                    if (gBack_screen->type == BR_PMT_RGB_565 ||
                        gBack_screen->type == BR_PMT_RGB_555) {
                        SmackBlit(smackBlitHandle, gBack_screen->pixels, gBack_screen->row_bytes, 0,
                                  40, gSmackerBuffer, 320, 0, 0, 320, 200);
                    }
                    if (currentFrame != smackHandle->Frames) {
                        SmackNextFrame(smackHandle);
                    }
                    if (paletteChanged) {
                        DRSetPalette(gCurrent_palette);
                    }
                    PDScreenBufferSwap(0);
                    PDServiceSystem(0);
                    keyPressed = 0;
                    while (SmackWait(smackHandle) && PDGetTotalTime() < frameStart + 67u) {
                        keyPressed = AnyKeyDown();
                        if (keyPressed) {
                            break;
                        }
                    }
                    if (keyPressed) {
                        break;
                    }
                }
            }
            FadePaletteDown();
            ClearEntireScreen();
            SmackBlitClose(smackBlitHandle);
        }
    }
    if (smackHandle != NULL) {
        SmackClose(smackHandle);
    }
    timeEnd = PDGetTotalTime();
    dr_dprintf("Frames %d, time %d, pal changes %d, pal changes per sec %f, av fps %f",
               frameCount, 1000 * (timeEnd - timeStart), paletteChangeCount,
               (float) paletteChangeCount / (timeEnd - timeStart), (float) frameCount / (timeEnd - timeStart));
    PDServiceSystem(0);
#else
    __asm {
            sub esp, 0x12c
            mov eax, dword ptr [gSound_override]
            push ebx
            push ebp
            push esi
            push edi
            xor edi, edi
            mov esi, ecx
            cmp eax, edi
            mov dword ptr [esp + 0x20], esi
            mov dword ptr [esp + 0x2c], edi
            mov dword ptr [esp + 0x28], edi
            mov dword ptr [esp + 0x24], edi
            jne L0043f2da
            cmp dword ptr [gNoCutscenes], edi
            jne L0043f2da
            mov ecx, dword ptr [gBack_screen]
            xor eax, eax
            mov al, byte ptr [ecx + 0x2c]
            sub eax, 3
            je L0043e7f6
            dec eax
            je L0043e7e4
            dec eax
            je L0043e7d2
            mov ecx, offset gMatchData_0058ff80
            call PDEnterDebugger
            pop edi
            pop esi
            pop ebp
            pop ebx
            add esp, 0x12c
            ret
    L0043e7d2:
            mov dword ptr [esp + 0x1c], 0xc0000000
            mov dword ptr [esp + 0x10], 0xc0000002
            jmp L0043e807
    L0043e7e4:
            mov dword ptr [esp + 0x1c], 0x80000000
            mov dword ptr [esp + 0x10], 0x80000002
            jmp L0043e807
    L0043e7f6:
            mov edx, dword ptr [gCurrent_palette]
            mov dword ptr [esp + 0x1c], edi
            mov eax, dword ptr [edx + 8]
            mov dword ptr [esp + 0x30], eax
    L0043e807:
            cmp dword ptr [gSmackerBuffer], edi
            jne L0043e826
            push 0xfd
            push 0xfa00
            call BrMemAllocate
            add esp, 8
            mov dword ptr [gSmackerBuffer], eax
    L0043e826:
            call StopMusic
            call FadePaletteDown
            call ClearEntireScreen
            push 1
            call dword ptr [gMatchData_00589278]
            push edi
            call dword ptr [gMatchData_0058927c]
            mov edx, offset gApplication_path
            lea ecx, [esp + 0x3c]
            push 0x58ff74
            call PathCat
            lea edx, [esp + 0x3c]
            lea ecx, [esp + 0x3c]
            push esi
            call PathCat
            lea ecx, [esp + 0x3c]
            push ecx
            push offset gMatchFloat_0058ff54
            call dr_dprintf
            mov ebx, dword ptr [gMatchData_00589280]
            add esp, 8
            lea edx, [esp + 0x3c]
            push 0x200
            push 0xfe000
            push edx
            call ebx
            mov ebp, eax
            cmp ebp, edi
            mov dword ptr [esp + 0x18], ebp
            jne L0043e9cc
            push -1
            lea eax, [esp + 0x40]
            push 0xfe000
            push eax
            call ebx
            mov ebp, eax
            cmp ebp, edi
            mov dword ptr [esp + 0x18], ebp
            jne L0043e9cc
            push offset gMatchFloat_0058ff14
            call dr_dprintf
            add esp, 4
            lea ecx, [esp + 0x3c]
            call GetRegisterSourceLocation
            test eax, eax
            je L0043e9bf
            lea edi, [esp + 0x3c]
            or ecx, 0xffffffff
            xor eax, eax
            repne scasb
            not ecx
            dec ecx
            je L0043e91d
            mov edi, dword ptr [gDir_separator]
            mov cl, byte ptr [esp + ecx + 0x3b]
            cmp cl, byte ptr [edi]
            je L0043e91d
            or ecx, 0xffffffff
            xor eax, eax
            repne scasb
            not ecx
            sub edi, ecx
            lea edx, [esp + 0x3c]
            mov esi, edi
            mov edi, edx
            mov edx, ecx
            or ecx, 0xffffffff
            repne scasb
            mov ecx, edx
            dec edi
            shr ecx, 2
            rep movsd
            mov ecx, edx
            and ecx, 3
            rep movsb
    L0043e91d:
            mov edi, 0x58ff0c
            or ecx, 0xffffffff
            xor eax, eax
            lea edx, [esp + 0x3c]
            repne scasb
            not ecx
            sub edi, ecx
            push 0x58ff74
            mov esi, edi
            mov edi, edx
            mov edx, ecx
            or ecx, 0xffffffff
            repne scasb
            mov ecx, edx
            dec edi
            shr ecx, 2
            rep movsd
            mov ecx, edx
            lea edx, [esp + 0x40]
            and ecx, 3
            rep movsb
            lea ecx, [esp + 0x40]
            call PathCat
            lea edx, [esp + 0x3c]
            lea ecx, [esp + 0x3c]
            mov eax, dword ptr [esp + 0x20]
            push eax
            call PathCat
            lea ecx, [esp + 0x3c]
            push ecx
            push offset gMatchFloat_0058fef8
            call dr_dprintf
            add esp, 8
            lea ecx, [esp + 0x3c]
            call PDCheckDriveExists
            test eax, eax
            je L0043e9cc
            push 0x200
            lea edx, [esp + 0x40]
            push 0xfe000
            push edx
            call ebx
            mov ebp, eax
            test ebp, ebp
            mov dword ptr [esp + 0x18], ebp
            jne L0043e9d4
            push -1
            lea eax, [esp + 0x40]
            push 0xfe000
            push eax
            call ebx
            mov ebp, eax
            mov dword ptr [esp + 0x18], ebp
            jmp L0043e9cc
    L0043e9bf:
            push offset gMatchFloat_0058fedc
            call dr_dprintf
            add esp, 4
    L0043e9cc:
            test ebp, ebp
            je L0043f2c1
    L0043e9d4:
            push offset gMatchFloat_0058fec4
            call dr_dprintf
            add esp, 4
            call PDGetTotalTime
            mov dword ptr [esp + 0x34], eax
            mov eax, dword ptr [gNbPixelBits]
            cmp eax, 0x10
            jne L0043f057
            push 0
            push 1
            call BrActorAllocate
            add esp, 8
            mov dword ptr [gSmackerActor], eax
            mov edx, 1
            mov cl, 7
            push 0
            push 0
            push 0x100
            mov byte ptr [eax + 0x20], 4
            call DRPixelmapAllocate
            mov edx, 1
            mov cl, 7
            push 0
            push 0
            push 0x100
            mov dword ptr [gSmackerCurrentPalette], eax
            call DRPixelmapAllocate
            mov ecx, dword ptr [gSmackerCurrentPalette]
            mov dword ptr [gSmackerBackPalette], eax
            push ecx
            call BrMapAdd
            add esp, 4
            push 0x1e
            push 0x3c
            push offset gMatchString_0058feb8
            call BrModelAllocate
            mov esi, eax
            add esp, 0xc
            test esi, esi
            jne L0043eaa8
            mov ecx, offset gMatchString_0058fea8
            call PDEnterDebugger
            mov edx, dword ptr [gSmackerActor]
            push edx
            call BrActorFree
            mov eax, dword ptr [gSmackerCurrentPalette]
            add esp, 4
            mov dword ptr [gSmackerActor], esi
            push eax
            call BrMapRemove
            mov ecx, dword ptr [gSmackerCurrentPalette]
            add esp, 4
            push ecx
            call BrPixelmapFree
            add esp, 4
            jmp L0043ed3b
    L0043eaa8:
            mov edi, offset gSmackMaterials
            mov ebp, 0x20000
            mov ebx, 0x10000
    L0043eab7:
            push offset gMatchFloat_0058fe90
            call BrMaterialAllocate
            add esp, 4
            mov dword ptr [edi], eax
            test eax, eax
            je L0043f2e5
            push 0
            push 0
            push 0x40
            push 0x40
            push 3
            call BrPixelmapAllocate
            mov edx, dword ptr [edi]
            add esp, 0x14
            mov dword ptr [edx + 0x40], eax
            mov eax, dword ptr [edi]
            mov edx, dword ptr [gSmackerCurrentPalette]
            mov ecx, dword ptr [eax + 0x40]
            mov dword ptr [ecx + 0x10], edx
            mov eax, dword ptr [edi]
            mov dword ptr [eax + 0x20], 0x2800
            mov eax, dword ptr [edi]
            mov edx, dword ptr [eax + 0x20]
            or edx, ebp
            mov dword ptr [eax + 0x20], edx
            mov eax, dword ptr [edi]
            mov ecx, dword ptr [eax + 0x20]
            or ecx, ebx
            mov dword ptr [eax + 0x20], ecx
            mov ecx, dword ptr [edi]
            mov edx, dword ptr [ecx + 0x40]
            mov eax, dword ptr [edx + 0x10]
            push eax
            call BrMapAdd
            mov ecx, dword ptr [edi]
            add esp, 4
            mov edx, dword ptr [ecx + 0x40]
            push edx
            call BrMapAdd
            mov eax, dword ptr [edi]
            add esp, 4
            push eax
            call BrMaterialAdd
            add edi, 4
            add esp, 4
            cmp edi, offset gSmackerBackPalette
            jl L0043eab7
            xor ecx, ecx
            xor edx, edx
            mov dword ptr [esp + 0x14], ecx
            mov ebp, offset gSmackMaterials
    L0043eb56:
            mov eax, dword ptr [esp + 0x14]
            xor edi, edi
            shl eax, 7
            mov ebx, eax
            mov eax, 0xffffff50
            sub eax, ebx
            mov dword ptr [esp + 0x1c], edi
            mov dword ptr [esp + 0x10], eax
            add eax, 0x80
            fild dword ptr [esp + 0x10]
            mov dword ptr [esp + 0x10], eax
            lea eax, [edx + edx*4]
            fild dword ptr [esp + 0x10]
            shl eax, 3
    L0043eb87:
            mov ebx, dword ptr [esi + 8]
            shl edi, 7
            mov dword ptr [esp + 0x10], edi
            add edi, 0x80
            fild dword ptr [esp + 0x10]
            mov dword ptr [esp + 0x10], edi
            fst dword ptr [eax + ebx]
            mov ebx, dword ptr [esi + 8]
            fld st(2)
            fstp dword ptr [eax + ebx + 4]
            mov ebx, dword ptr [esi + 8]
            fild dword ptr [esp + 0x10]
            mov dword ptr [eax + ebx + 8], 0xc0000000
            mov ebx, dword ptr [esi + 8]
            mov dword ptr [eax + ebx + 0xc], 0x3c000000
            mov ebx, dword ptr [esi + 8]
            mov dword ptr [eax + ebx + 0x10], 0x3f7e0000
            mov edi, dword ptr [esi + 8]
            fst dword ptr [eax + edi + 0x28]
            mov edi, dword ptr [esi + 8]
            fld st(3)
            fstp dword ptr [eax + edi + 0x2c]
            mov ebx, dword ptr [esi + 8]
            mov edi, 0xc0000000
            mov dword ptr [eax + ebx + 0x30], edi
            mov ebx, dword ptr [esi + 8]
            mov dword ptr [eax + ebx + 0x34], 0x3f7e0000
            mov ebx, dword ptr [esi + 8]
            mov dword ptr [eax + ebx + 0x38], 0x3f7e0000
            mov ebx, dword ptr [esi + 8]
            fstp dword ptr [eax + ebx + 0x50]
            mov ebx, dword ptr [esi + 8]
            fld st(1)
            fstp dword ptr [eax + ebx + 0x54]
            mov ebx, dword ptr [esi + 8]
            mov dword ptr [eax + ebx + 0x58], edi
            mov ebx, dword ptr [esi + 8]
            mov dword ptr [eax + ebx + 0x5c], 0x3f7e0000
            mov ebx, dword ptr [esi + 8]
            mov dword ptr [eax + ebx + 0x60], 0x3c000000
            mov ebx, dword ptr [esi + 8]
            fstp dword ptr [eax + ebx + 0x78]
            mov ebx, dword ptr [esi + 8]
            fst dword ptr [eax + ebx + 0x7c]
            mov ebx, dword ptr [esi + 8]
            mov dword ptr [eax + ebx + 0x80], edi
            mov ebx, dword ptr [esi + 8]
            mov edi, 0x3c000000
            mov dword ptr [eax + ebx + 0x84], edi
            mov ebx, dword ptr [esi + 8]
            mov dword ptr [eax + ebx + 0x88], edi
            mov edi, dword ptr [esi + 0xc]
            mov word ptr [ecx + edi], dx
            mov ebx, dword ptr [esi + 0xc]
            lea edi, [edx + 3]
            mov word ptr [ecx + ebx + 2], di
            mov ebx, dword ptr [esi + 0xc]
            lea edi, [edx + 1]
            mov word ptr [ecx + ebx + 4], di
            mov edi, dword ptr [esi + 0xc]
            mov ebx, dword ptr [ebp]
            mov dword ptr [ecx + edi + 8], ebx
            mov edi, dword ptr [esi + 0xc]
            xor ebx, ebx
            mov word ptr [ecx + edi + 6], bx
            mov edi, dword ptr [esi + 0xc]
            mov byte ptr [ecx + edi + 0x10], bl
            mov ebx, dword ptr [esi + 0xc]
            lea edi, [edx + 1]
            mov word ptr [ecx + ebx + 0x28], di
            mov ebx, dword ptr [esi + 0xc]
            lea edi, [edx + 3]
            mov word ptr [ecx + ebx + 0x2a], di
            mov ebx, dword ptr [esi + 0xc]
            lea edi, [edx + 2]
            mov word ptr [ecx + ebx + 0x2c], di
            mov edi, dword ptr [esi + 0xc]
            mov ebx, dword ptr [ebp]
            mov dword ptr [ecx + edi + 0x30], ebx
            mov edi, dword ptr [esi + 0xc]
            mov word ptr [ecx + edi + 0x2e], 0
            mov edi, dword ptr [esi + 0xc]
            add edx, 4
            add eax, 0xa0
            add ebp, 4
            mov byte ptr [ecx + edi + 0x38], 0
            mov edi, dword ptr [esp + 0x1c]
            inc edi
            add ecx, 0x50
            cmp edi, 5
            mov dword ptr [esp + 0x1c], edi
            jl L0043eb87
            mov edi, dword ptr [esp + 0x14]
            fstp st(0)
            inc edi
            cmp ebp, offset gSmackerBackPalette
            fstp st(0)
            mov dword ptr [esp + 0x14], edi
            jl L0043eb56
            push esi
            call BrModelAdd
            mov ecx, dword ptr [gSmackerActor]
            mov dword ptr [gSmackerModel], esi
            add esp, 4
            mov dword ptr [ecx + 0x18], esi
            mov edx, dword ptr [gSmackerActor]
            mov eax, dword ptr [g2d_camera]
            push edx
            push eax
            call BrActorAdd
            add esp, 8
    L0043ed3b:
            mov eax, dword ptr [gSmackerActor]
            test eax, eax
            je L0043f211
            mov ecx, dword ptr [gHWnd]
            push 0xc8
            push 0x140
            push 0xc8
            push 0x140
            push 0x80
            push ecx
            call dword ptr [gMatchData_00589284]
            test eax, eax
            jne L0043ed7c
            mov ecx, offset gMatchString_0058fe88
            call PDEnterDebugger
    L0043ed7c:
            mov edx, dword ptr [gSmackerBuffer]
            mov esi, dword ptr [esp + 0x18]
            push 0
            push edx
            push 0xc8
            push 0x140
            push 0
            push 0
            push esi
            call dword ptr [gMatchData_00589288]
            mov ecx, dword ptr [esi + 0xc]
            mov eax, 1
            cmp ecx, eax
            mov dword ptr [esp + 0x14], eax
            jb L0043efca
    L0043edb2:
            call PDGetTotalTime
            mov esi, dword ptr [esp + 0x18]
            mov dword ptr [esp + 0x30], eax
            mov eax, dword ptr [esi + 0x68]
            test eax, eax
            je L0043ee8e
            mov eax, dword ptr [gSmackerCurrentPalette]
            push eax
            call BrMapRemove
            mov eax, dword ptr [gSmackerBackPalette]
            mov ecx, dword ptr [gSmackerCurrentPalette]
            mov dword ptr [gSmackerPrevBackPalette], eax
            mov dword ptr [gSmackerCurrentPalette], eax
            add esp, 4
            xor eax, eax
            mov dword ptr [gSmackerBackPalette], ecx
            add esi, 0x6d
    L0043edf8:
            xor ecx, ecx
            xor edx, edx
            mov ch, byte ptr [esi - 1]
            mov dl, byte ptr [esi + 1]
            mov cl, byte ptr [esi]
            shl ecx, 8
            or ecx, edx
            jne L0043ee10
            mov ecx, 0x40404
    L0043ee10:
            mov edx, dword ptr [gSmackerCurrentPalette]
            add eax, 4
            add esi, 3
            cmp eax, 0x400
            mov edx, dword ptr [edx + 8]
            mov dword ptr [edx + eax - 4], ecx
            jl L0043edf8
            mov eax, dword ptr [gSmackerCurrentPalette]
            push eax
            call BrMapAdd
            add esp, 4
            mov esi, offset gSmackMaterials
    L0043ee3d:
            mov ecx, dword ptr [esi]
            mov eax, dword ptr [gSmackerCurrentPalette]
            push 0xfff
            mov edx, dword ptr [ecx + 0x40]
            mov dword ptr [edx + 0x10], eax
            mov ecx, dword ptr [esi]
            mov edx, dword ptr [ecx + 0x40]
            mov eax, dword ptr [edx + 0x10]
            push eax
            call BrMapUpdate
            mov ecx, dword ptr [esi]
            add esp, 8
            mov edx, dword ptr [ecx + 0x40]
            push 0xfff
            push edx
            call BrMapUpdate
            mov eax, dword ptr [esi]
            add esp, 8
            push 0x7fff
            push eax
            call BrMaterialUpdate
            add esi, 4
            add esp, 8
            cmp esi, offset gSmackerBackPalette
            jl L0043ee3d
    L0043ee8e:
            mov ecx, dword ptr [esp + 0x18]
            push ecx
            call dword ptr [gMatchData_0058928c]
            mov edx, dword ptr [gDepth_buffer]
            push -1
            push edx
            call BrPixelmapFill
            add esp, 8
            mov dword ptr [esp + 0x1c], 0
            mov ebp, offset gSmackMaterials
    L0043eeb7:
            mov eax, dword ptr [esp + 0x1c]
            mov dword ptr [esp + 0x10], 5
            mov dword ptr [esp + 0x20], eax
    L0043eec7:
            mov ecx, dword ptr [gSmackerBuffer]
            mov edx, dword ptr [esp + 0x20]
            mov eax, dword ptr [ebp]
            add edx, ecx
            mov ebx, 0x40
            mov ecx, dword ptr [eax + 0x40]
            mov eax, dword ptr [ecx + 8]
    L0043eee1:
            mov esi, edx
            mov edi, eax
            mov ecx, 0x10
            add eax, 0x40
            add edx, 0x140
            dec ebx
            rep movsd
            jne L0043eee1
            mov edx, dword ptr [ebp]
            push 0xfff
            mov eax, dword ptr [edx + 0x40]
            push eax
            call BrMapUpdate
            mov ecx, dword ptr [ebp]
            add esp, 8
            push 0x7fff
            push ecx
            call BrMaterialUpdate
            mov edx, dword ptr [esp + 0x28]
            mov eax, dword ptr [esp + 0x18]
            add esp, 8
            add edx, 0x40
            add ebp, 4
            dec eax
            mov dword ptr [esp + 0x20], edx
            mov dword ptr [esp + 0x10], eax
            jne L0043eec7
            mov ecx, dword ptr [esp + 0x1c]
            add ecx, 0x5000
            cmp ebp, offset gSmackerBackPalette
            mov dword ptr [esp + 0x1c], ecx
            jl L0043eeb7
            mov edx, dword ptr [gDepth_buffer]
            mov eax, dword ptr [gBack_screen]
            push edx
            push eax
            mov eax, dword ptr [g2d_camera]
            push eax
            push eax
            call BrZbSceneRender
            add esp, 0x10
            xor ecx, ecx
            call PDScreenBufferSwap
            xor ecx, ecx
            call PDServiceSystem
            mov edi, dword ptr [esp + 0x18]
            mov ebp, dword ptr [esp + 0x14]
            cmp ebp, dword ptr [edi + 0xc]
            je L0043ef8e
            push edi
            call dword ptr [gMatchData_00589274]
    L0043ef8e:
            mov ebx, dword ptr [gMatchData_00589290]
    L0043ef94:
            call AnyKeyDown
            mov esi, eax
            push edi
            call ebx
            test esi, esi
            jne L0043efca
            test eax, eax
            je L0043efb6
            call PDGetTotalTime
            mov ecx, dword ptr [esp + 0x30]
            add ecx, 0x43
            cmp eax, ecx
            jb L0043ef94
    L0043efb6:
            test esi, esi
            jne L0043efca
            mov eax, dword ptr [edi + 0xc]
            inc ebp
            cmp ebp, eax
            mov dword ptr [esp + 0x14], ebp
            jbe L0043edb2
    L0043efca:
            mov eax, dword ptr [gSmackerActor]
            test eax, eax
            je L0043f211
            mov esi, offset gSmackMaterials
            xor edi, edi
    L0043efde:
            mov edx, dword ptr [esi]
            mov eax, dword ptr [edx + 0x40]
            push eax
            call BrMapRemove
            mov ecx, dword ptr [esi]
            add esp, 4
            mov edx, dword ptr [ecx + 0x40]
            push edx
            call BrPixelmapFree
            mov eax, dword ptr [esi]
            add esp, 4
            mov dword ptr [eax + 0x40], edi
            mov ecx, dword ptr [esi]
            push ecx
            call BrMaterialRemove
            mov dword ptr [esi], edi
            add esi, 4
            add esp, 4
            cmp esi, offset gSmackerBackPalette
            jl L0043efde
            mov edx, dword ptr [gSmackerModel]
            push edx
            call BrModelRemove
            mov eax, dword ptr [gSmackerActor]
            add esp, 4
            push eax
            call BrActorRemove
            mov ecx, dword ptr [gSmackerModel]
            add esp, 4
            push ecx
            call BrModelFree
            mov edx, dword ptr [gSmackerActor]
            add esp, 4
            push edx
            call BrActorFree
            add esp, 4
            jmp L0043f211
    L0043f057:
            mov eax, dword ptr [gBack_screen]
            mov cl, byte ptr [eax + 0x2c]
            cmp cl, 5
            je L0043f069
            cmp cl, 4
            jne L0043f089
    L0043f069:
            mov eax, dword ptr [esp + 0x10]
            push eax
            call dword ptr [gMatchData_00589294]
            test eax, eax
            mov dword ptr [esp + 0x2c], eax
            je L0043f22a
            mov eax, dword ptr [gBack_screen]
            mov ebp, dword ptr [esp + 0x18]
    L0043f089:
            mov edx, dword ptr [ebp + 0xc]
            mov ecx, 1
            cmp edx, ecx
            mov dword ptr [esp + 0x14], ecx
            jb L0043f211
            mov edi, dword ptr [esp + 0x18]
            mov ebp, dword ptr [gMatchData_00589288]
            jmp L0043f0ae
    L0043f0a9:
            mov eax, dword ptr [gBack_screen]
    L0043f0ae:
            mov esi, dword ptr [esp + 0x24]
            mov cl, byte ptr [eax + 0x2c]
            inc esi
            cmp cl, 5
            mov dword ptr [esp + 0x24], esi
            je L0043f0db
            cmp cl, 4
            je L0043f0db
            mov ecx, dword ptr [esp + 0x1c]
            mov edx, dword ptr [eax + 8]
            push ecx
            push edx
            movsx edx, word ptr [eax + 0x28]
            xor ecx, ecx
            mov cx, word ptr [eax + 0x36]
            push ecx
            push edx
            jmp L0043f0ed
    L0043f0db:
            mov eax, dword ptr [gSmackerBuffer]
            push 0
            push eax
            push 0xc8
            push 0x140
    L0043f0ed:
            push 0
            push 0
            push edi
            call ebp
            mov eax, dword ptr [edi + 0x68]
            xor ebx, ebx
            test eax, eax
            je L0043f162
            mov ecx, dword ptr [gBack_screen]
            mov edx, dword ptr [esp + 0x28]
            inc edx
            mov ebx, 1
            mov al, byte ptr [ecx + 0x2c]
            mov dword ptr [esp + 0x28], edx
            cmp al, 5
            je L0043f14c
            cmp al, 4
            je L0043f14c
            mov esi, dword ptr [esp + 0x30]
            lea eax, [edi + 0x6e]
            mov ebp, 0x100
    L0043f128:
            xor edx, edx
            xor ecx, ecx
            mov dh, byte ptr [eax - 2]
            mov cl, byte ptr [eax]
            mov dl, byte ptr [eax - 1]
            add eax, 3
            shl edx, 8
            or edx, ecx
            mov dword ptr [esi], edx
            add esi, 4
            dec ebp
            jne L0043f128
            mov ebp, dword ptr [gMatchData_00589288]
            jmp L0043f162
    L0043f14c:
            mov edx, dword ptr [edi + 0x370]
            mov ecx, dword ptr [esp + 0x2c]
            lea eax, [edi + 0x6c]
            push edx
            push eax
            push ecx
            call dword ptr [gMatchData_00589298]
    L0043f162:
            push edi
            call dword ptr [gMatchData_0058928c]
            mov eax, dword ptr [gBack_screen]
            mov cl, byte ptr [eax + 0x2c]
            cmp cl, 5
            je L0043f17b
            cmp cl, 4
            jne L0043f1ad
    L0043f17b:
            mov edx, dword ptr [gSmackerBuffer]
            push 0xc8
            movsx ecx, word ptr [eax + 0x28]
            push 0x140
            push 0
            push 0
            push 0x140
            push edx
            mov edx, dword ptr [eax + 8]
            mov eax, dword ptr [esp + 0x44]
            push 0x28
            push 0
            push ecx
            push edx
            push eax
            call dword ptr [gMatchData_0058929c]
    L0043f1ad:
            mov ecx, dword ptr [esp + 0x14]
            mov eax, dword ptr [edi + 0xc]
            cmp ecx, eax
            je L0043f1bf
            push edi
            call dword ptr [gMatchData_00589274]
    L0043f1bf:
            test ebx, ebx
            je L0043f1ce
            mov ecx, dword ptr [gCurrent_palette]
            call DRSetPalette
    L0043f1ce:
            xor ecx, ecx
            call PDScreenBufferSwap
            xor ecx, ecx
            call PDServiceSystem
            call PDGetTotalTime
            mov ebx, dword ptr [gMatchData_00589290]
    L0043f1e7:
            call AnyKeyDown
            mov esi, eax
            push edi
            call ebx
            test esi, esi
            jne L0043f211
            test eax, eax
            jne L0043f1e7
            test esi, esi
            jne L0043f211
            mov eax, dword ptr [esp + 0x14]
            mov ecx, dword ptr [edi + 0xc]
            inc eax
            cmp eax, ecx
            mov dword ptr [esp + 0x14], eax
            jbe L0043f0a9
    L0043f211:
            call FadePaletteDown
            call ClearEntireScreen
            mov eax, dword ptr [esp + 0x2c]
            test eax, eax
            je L0043f22a
            push eax
            call dword ptr [gMatchData_005892a0]
    L0043f22a:
            call PDGetTotalTime
            mov edx, dword ptr [esp + 0x18]
            mov esi, eax
            push edx
            call dword ptr [gMatchData_005892a4]
            mov ecx, esi
            sub esp, 8
            mov eax, dword ptr [esp + 0x3c]
            mov dword ptr [esp + 0x40], 0
            sub ecx, eax
            mov edx, dword ptr [esp + 0x30]
            mov dword ptr [esp + 0x3c], ecx
            lea eax, [eax + eax*4]
            fild qword ptr [esp + 0x3c]
            fild dword ptr [esp + 0x2c]
            lea eax, [eax + eax*4]
            sub esp, 8
            fdiv st(0), st(1)
            lea eax, [eax + eax*4]
            shl eax, 3
            mov ecx, eax
            lea eax, [esi + esi*4]
            lea eax, [eax + eax*4]
            fmul dword ptr [gMatchFloat_00589b4c]
            fstp qword ptr [esp + 8]
            fild dword ptr [esp + 0x38]
            fdiv st(0), st(1)
            fmul dword ptr [gMatchFloat_00589b4c]
            _emit 0xdd
        _emit 0x1c
        _emit 0x24
            push edx
            lea edx, [eax + eax*4]
            mov eax, dword ptr [esp + 0x38]
            shl edx, 3
            sub edx, ecx
            push edx
            push eax
            push offset gMatchFloat_0058fe40
            fstp st(0)
            call dr_dprintf
            add esp, 0x20
            xor ecx, ecx
            call PDServiceSystem
            pop edi
            pop esi
            pop ebp
            pop ebx
            add esp, 0x12c
            ret
    L0043f2c1:
            mov ecx, dword ptr [esp + 0x20]
            push ecx
            push offset gMatchFloat_0058fe20
            call dr_dprintf
            add esp, 8
            xor ecx, ecx
            call PDServiceSystem
    L0043f2da:
            pop edi
            pop esi
            pop ebp
            pop ebx
            add esp, 0x12c
            ret
    L0043f2e5:
            mov ecx, offset gMatchFloat_0058fe0c
            call PDEnterDebugger
            mov ecx, dword ptr [gSmackerActor]
            push ecx
            call BrActorFree
            mov edx, dword ptr [gSmackerCurrentPalette]
            add esp, 4
            mov dword ptr [gSmackerActor], 0
            push edx
            call BrMapRemove
            mov eax, dword ptr [gSmackerCurrentPalette]
            add esp, 4
            push eax
            call BrPixelmapFree
            add esp, 4
            jmp L0043ed3b
            nop
            nop
            nop
            nop
            nop
            nop
    }
#endif
}

// FUNCTION: CARMA2_HW 0x0043f330
void C2_HOOK_FASTCALL DoOpeningAnimation(void) {
    PlaySmackerFile("INTERPLAYGO.SMK");
    PlaySmackerFile("MIX_INTR.SMK");
    WaitForNoKeys();
}

// FUNCTION: CARMA2_HW 0x0043f350
void C2_HOOK_FASTCALL DoNewGameAnimation(void) {
}

// FUNCTION: CARMA2_HW 0x0043f360
void C2_HOOK_FASTCALL DoGoToRaceAnimation(void) {
}

// FUNCTION: CARMA2_HW 0x0043f370
void C2_HOOK_FASTCALL DoEndRaceAnimation(void) {

    FadePaletteDown();
}

// FUNCTION: CARMA2_HW 0x0043f380
void C2_HOOK_FASTCALL DoGameOverAnimation(void) {
}

// FUNCTION: CARMA2_HW 0x0043f390
void C2_HOOK_FASTCALL DoGameCompletedAnimation(void) {

    PlaySmackerFile("TOPRANK.SMK");
    WaitForNoKeys();
}

// FUNCTION: CARMA2_HW 0x0043f3a0
void C2_HOOK_FASTCALL DoFeatureUnavailableInDemo(void) {
    float sound_delay;

    PrintMemoryDump(0, "BEFORE DEMO-ONLY SCREEN");
    SuspendPendingFlic();
    FadePaletteDown();
    sound_delay = gFlic_sound_delay_not_in_demo;
    gProgram_state.cut_scene = 1;
    DRS3LoadSample(0x2136);
    FUN_00461980(0x2136, PDGetTotalTime() - (int)(sound_delay * -1000.0f));
    SetNonFatalAllocationErrors();
    RunFlic(7);
    ResetNonFatalAllocationErrors();
    FUN_004835d0();
    FadePaletteDown();
    ClearEntireScreen();
    DRS3ReleaseSound(0x2136);
    FUN_00461980(0, 0);
    gProgram_state.cut_scene = 0;
    FadePaletteDown();
    PrintMemoryDump(0, "AFTER DEMO-ONLY SCREEN");
}

// FUNCTION: CARMA2_HW 0x0043f5c0
void C2_HOOK_FASTCALL StartLoadingScreen(void) {

    SwitchToLoresMode();
    StartMusicTrack(9998);
    PossibleService();
    if (gProgram_state.sausage_eater_mode) {
        SplashScreenWith("LOADSCRN.PIX");
    } else {
        SplashScreenWith("LOADSCRN.PIX");
    }
}