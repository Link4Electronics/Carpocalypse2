#include "loading3.h"

#include "loading.h"
#include "loading1.h"
#include "graphics2.h"
#include "utility.h"
#include "packfile.h"
#include "carpocalypse2_types.h"
#include "globvars.h"
#include "platform.h"
#include "loadtif.h"
#include "carpocalypse2_macros.h"

#include <string.h>
// GLOBAL: CARMA2_HW 0x0065852c
char* gPedTexturePath = "PIXELMAP";

#ifdef CARPOCALYPSE2_MATCHING
extern void C2_HOOK_FASTCALL AllowOpenToFail(void);
extern void C2_HOOK_FASTCALL DoNotAllowOpenToFail(void);
extern br_pixelmap* C2_HOOK_FASTCALL LoadTiffTexture_Ex(const char* pDirectory, const char* pFile_stem, br_pixelmap* pPalette, int pFlags, int* pError_code);
extern void C2_HOOK_FASTCALL PathCat(char* pDestn_str, const char* pStr_1, const char* pStr_2);
extern void C2_HOOK_FASTCALL PossibleService(void);
extern void __cdecl _chkstk();
extern tPath_name gApplication_path;
extern int gDisableTiffConversion;
extern int gGraf_spec_index;
extern tGraf_spec gGraf_specs[2];
extern char gNet_avail_names[];
extern tPixelFlags gPixelFlags;
extern br_pixelmap* gRender_palette;
#endif
// FUNCTION: CARMA2_HW 0x0048ea40
int C2_HOOK_FASTCALL LoadBunchOfPixies(const char* pathRoot, const char* textureName, br_pixelmap** textureBuffer, size_t bufferCapacity) {
    tPath_name tempPath;
    br_uint_32 nb;

    PathCat(tempPath, pathRoot, textureName);
    // strcat(tempPath, (gPixelFlags & kPixelFlags_16bbp) ? ".P16" : ".P08");
    if (gPixelFlags & kPixelFlags_16bbp) {
        strcat(tempPath, ".P16");
    } else {
        strcat(tempPath, ".P08");
    }
    nb = BrPixelmapLoadMany(tempPath, textureBuffer, (br_uint_16)bufferCapacity);
    if (nb == 0) {
        if (gPixelFlags & kPixelFlags_16bbp) {
            PathCat(tempPath, pathRoot, "PIX16");
        } else {
            PathCat(tempPath, pathRoot, "PIX8");
        }
        PathCat(tempPath, tempPath, textureName);
        strcat(tempPath, ".PIX");
        nb = BrPixelmapLoadMany(tempPath, textureBuffer, (br_uint_16)bufferCapacity);
    }
    if (nb != 0) {
        if (nb == 1 && !gDisableTiffConversion) {
            if (textureBuffer[0]->identifier != NULL) {
                BrResFree(textureBuffer[0]->identifier);
            }
            textureBuffer[0]->identifier = BrResStrDup(textureBuffer[0], textureName);
        }
    }
    return nb;
}

// FUNCTION: CARMA2_HW 0x0048eb80
br_pixelmap* C2_HOOK_FASTCALL DRLdImg(const char* texturePathDir, const char* textureName, br_pixelmap* pPalette, int flags, int* errorCode) {
    br_pixelmap* textures[1000];
    int nb;

    if (gDisableTiffConversion) {
        nb = LoadBunchOfPixies(texturePathDir, textureName, textures, CARPOCALYPSE2_ASIZE(textures));
        if (nb != 0) {
            *errorCode = 0;
            return textures[0];
        } else {
            *errorCode = 1;
            return NULL;
        }
    } else {
        return LoadTiffTexture_Ex(texturePathDir, textureName, pPalette, flags | kLoadTextureFlags_SaveTextureCompressed, errorCode);
    }
}

// FUNCTION: CARMA2_HW 0x0048ec20
C2_NAKED br_pixelmap* C2_HOOK_FASTCALL RealLoadPixelmap(const char* pPath_name) {
#ifndef CARPOCALYPSE2_MATCHING
    const char* ext;
    tPath_name path;
    char texture_name[256];
    int error;
    br_pixelmap* pm;

    ext = strrchr(pPath_name, '.');
    if (ext == NULL) {
        ext = pPath_name + strlen(pPath_name);
    }
    if (ext - pPath_name == 4) {
        if (memcmp(pPath_name, "none", 4) == 0) {
            return NULL;
        }
    }
    PossibleService();
    PathCat(path, gApplication_path, gGraf_specs[gGraf_spec_index].data_dir_name);
    PathCat(path, path, "PIXELMAP");
    strcpy(texture_name, pPath_name);
    texture_name[strlen(texture_name) - 4] = '\0';
    AllowOpenToFail();
    pm = DRLdImg(path, texture_name, gRender_palette, gPixelFlags, &error);
    DoNotAllowOpenToFail();
    if (pm == NULL) {
        PathCat(path, gApplication_path, gPedTexturePath);
        return DRLdImg(path, texture_name, gRender_palette, gPixelFlags, &error);
    }
    return pm;
#else
    __asm {
            mov eax, 0x11a4
            call _chkstk
            push ebx
            push esi
            push edi
            mov ebx, ecx
            push 0x2e
            push ebx
            call strrchr
            add esp, 8
            test eax, eax
            jne L0048ec4c
            mov edi, ebx
            or ecx, 0xffffffff
            repne scasb
            not ecx
            dec ecx
            mov eax, ecx
            add eax, ebx
    L0048ec4c:
            sub eax, ebx
            cmp eax, 4
            jne L0048ec6c
            mov ecx, eax
            mov edi, offset gNet_avail_names+0x7b8
            mov esi, ebx
            xor eax, eax
            repe cmpsb
            jne L0048ec6c
            pop edi
            pop esi
            pop ebx
            add esp, 0x11a4
            ret
    L0048ec6c:
            call PossibleService
            mov eax, dword ptr [gGraf_spec_index]
            lea ecx, [eax + eax*2]
            lea edx, [eax + ecx*4]
            lea ecx, [esp + 0x10]
            mov eax, dword ptr [edx*4 + gGraf_specs+0x1c]
            mov edx, offset gApplication_path
            push eax
            call PathCat
            lea edx, [esp + 0x10]
            lea ecx, [esp + 0x10]
            push offset gNet_avail_names+0x4c0
            call PathCat
            mov edi, ebx
            or ecx, 0xffffffff
            xor eax, eax
            lea edx, [esp + 0x110]
            repne scasb
            not ecx
            sub edi, ecx
            mov eax, ecx
            mov esi, edi
            mov edi, edx
            lea edx, [esp + 0x110]
            shr ecx, 2
            rep movsd
            mov ecx, eax
            xor eax, eax
            and ecx, 3
            sub edx, 4
            rep movsb
            lea edi, [esp + 0x110]
            or ecx, 0xffffffff
            repne scasb
            not ecx
            dec ecx
            mov byte ptr [ecx + edx], al
            call AllowOpenToFail
            mov edx, dword ptr [gDisableTiffConversion]
            mov eax, dword ptr [gPixelFlags]
            mov ecx, dword ptr [gRender_palette]
            test edx, edx
            je L0048ed40
            lea eax, [esp + 0x210]
            push 0x3e8
            push eax
            lea edx, [esp + 0x118]
            lea ecx, [esp + 0x18]
            call LoadBunchOfPixies
            test eax, eax
            je L0048ed34
            mov esi, dword ptr [esp + 0x210]
            mov dword ptr [esp + 0xc], 0
            jmp L0048ed5b
    L0048ed34:
            mov dword ptr [esp + 0xc], 1
            xor esi, esi
            jmp L0048ed5b
    L0048ed40:
            lea edx, [esp + 0xc]
            or al, 0x40
            push edx
            push eax
            push ecx
            lea edx, [esp + 0x11c]
            lea ecx, [esp + 0x1c]
            call LoadTiffTexture_Ex
            mov esi, eax
    L0048ed5b:
            call DoNotAllowOpenToFail
            test esi, esi
            jne L0048edec
            mov eax, dword ptr [gPedTexturePath]
            mov edx, offset gApplication_path
            push eax
            lea ecx, [esp + 0x14]
            call PathCat
            mov edx, dword ptr [gDisableTiffConversion]
            mov eax, dword ptr [gPixelFlags]
            mov ecx, dword ptr [gRender_palette]
            test edx, edx
            je L0048edd1
            lea ecx, [esp + 0x210]
            push 0x3e8
            push ecx
            lea edx, [esp + 0x118]
            lea ecx, [esp + 0x18]
            call LoadBunchOfPixies
            test eax, eax
            jne L0048edbe
            mov eax, esi
            pop edi
            pop esi
            pop ebx
            add esp, 0x11a4
            ret
    L0048edbe:
            mov esi, dword ptr [esp + 0x210]
            mov eax, esi
            pop edi
            pop esi
            pop ebx
            add esp, 0x11a4
            ret
    L0048edd1:
            lea edx, [esp + 0xc]
            or al, 0x40
            push edx
            push eax
            push ecx
            lea edx, [esp + 0x11c]
            lea ecx, [esp + 0x1c]
            call LoadTiffTexture_Ex
            mov esi, eax
    L0048edec:
            mov eax, esi
            pop edi
            pop esi
            pop ebx
            add esp, 0x11a4
            ret
    }
#endif
}

// FUNCTION: CARMA2_HW 0x0048ec00
br_pixelmap* C2_HOOK_FASTCALL LoadPixelmap(const char* pPath_name) {
    br_pixelmap *pm;

    pm = RealLoadPixelmap(pPath_name);
    EnsurePixelmapAllowed(pm, 0);
    return pm;
}

// FUNCTION: CARMA2_HW 0x0048ee00
br_pixelmap* C2_HOOK_FASTCALL LoadPixelmapAllow(const char* pPath_name) {
    br_pixelmap *pm;

    pm = RealLoadPixelmap(pPath_name);
    EnsurePixelmapAllowed(pm, 1);
    return pm;
}

// FUNCTION: CARMA2_HW 0x0048ee20
br_pixelmap* C2_HOOK_FASTCALL LoadPixelmapForcedSize(const char* pPath_name, int pWidth, int pHeight) {
    br_pixelmap* pm;
    br_pixelmap* new_pm;

    pm = RealLoadPixelmap(pPath_name);
    EnsurePixelmapAllowed(pm, 0);
    if (pm == NULL) {
        return NULL;
    }
    if ((int)pm->width > pWidth || (int)pm->height > pHeight) {
        PDFatalError("Bugger - LoadPixelmapForcedSize() needs size to be bigger than original\n");
    }
    new_pm = BrPixelmapAllocate(pm->type, pWidth, pHeight, NULL, 0);
    if (new_pm == NULL) {
        PDFatalError("Bugger - LoadPixelmapForcedSize() could not clone pixelmap\n");
    }
    BrPixelmapRectangleCopy(new_pm, 0, 0, pm, 0, 0, pm->width, pm->height);
    BrPixelmapFree(pm);
    return new_pm;
}

// FUNCTION: CARMA2_HW 0x0048eec0
int C2_HOOK_FASTCALL LoadInterfacePixelmaps(const char *pPath_name, br_pixelmap** pMaps, int pCapacity) {
    tPath_name path;
    size_t count;

    strcpy(path, pPath_name);
    strcpy(gCurrent_load_directory, "INTRFACE");
    count = LoadTextureTryAllLocations(path, pMaps, pCapacity);
    DoNotAllowOpenToFail();
    return count;
}

// FUNCTION: CARMA2_HW 0x0048ef40
br_pixelmap* C2_HOOK_FASTCALL LoadShadeTable(const char* pName) {
    tPath_name the_path;

    PathCat(the_path, gApplication_path, "SHADETAB");
    PathCat(the_path, the_path, pName);
    return BrPixelmapLoad(the_path);
}

// FUNCTION: CARMA2_HW 0x0048ef80
br_material* C2_HOOK_FASTCALL LoadMaterial(const char* pThe_path) {
    tPath_name the_path;

    PossibleService();
    PathCat(the_path, gApplication_path, "MATERIAL");
    PathCat(the_path, the_path, pThe_path);
    return BrMaterialLoad(the_path);
}

// FUNCTION: CARMA2_HW 0x0048efd0
br_model* C2_HOOK_FASTCALL LoadModel(const char* pName) {
    tPath_name the_path;

    PossibleService();
    PathCat(the_path, gApplication_path, "MODELS");
    PathCat(the_path, the_path, pName);
    return BrModelLoad(the_path);
}

// FUNCTION: CARMA2_HW 0x0048f020
br_actor* C2_HOOK_FASTCALL LoadActor(const char* pName) {
    tPath_name the_path;
    br_actor* actor;

    PossibleService();
    PathCat(the_path, gApplication_path, "ACTORS");
    PathCat(the_path, the_path, pName);
    actor = BrActorLoad(the_path);
    if (actor == NULL) {
        PathCat(the_path, gRace_path, pName);
        actor = BrActorLoad(the_path);
    }
    return actor;
}

// FUNCTION: CARMA2_HW 0x0048f090
void C2_HOOK_FASTCALL DRLoadPalette(const char* pPath_name) {
    br_pixelmap* palette_array[100];
    int number_of_palettes;
    int i;

    number_of_palettes = BrPixelmapLoadMany(pPath_name, palette_array, CARPOCALYPSE2_ASIZE(palette_array));
    for (i = 0; i < number_of_palettes; i++) {
        palette_array[i]->row_bytes = (palette_array[i]->row_bytes + 3) & ~0x3;
        palette_array[i]->base_x = 0;
        palette_array[i]->base_y = 0;
    }
    BrTableAddMany(palette_array, number_of_palettes);
}

// FUNCTION: CARMA2_HW 0x0048f100
void C2_HOOK_FASTCALL DRLoadShadeTable(const char* pPath_name) {
    br_pixelmap* table_array[100];
    int number_of_tables;
    int i;

    number_of_tables = BrPixelmapLoadMany(pPath_name, table_array, CARPOCALYPSE2_ASIZE(table_array));
    for (i = 0; i < number_of_tables; i++) {
        table_array[i]->row_bytes = (table_array[i]->row_bytes + 3) & ~0x3;
        table_array[i]->base_x = 0;
        table_array[i]->base_y = 0;
    }
    BrTableAddMany(table_array, number_of_tables);
}

// FUNCTION: CARMA2_HW 0x0048f170
void C2_HOOK_FASTCALL DRLoadPixelmaps(const char* pPath_name) {
    br_pixelmap* pixelmaps[100];
    int number_of_pixelmaps;
    int i;

    PossibleService();
    number_of_pixelmaps = DRPixelmapLoadMany(pPath_name, pixelmaps, CARPOCALYPSE2_ASIZE(pixelmaps));
    for (i = number_of_pixelmaps - 1; i >= 0; i--) {
        pixelmaps[i]->origin_x = 0;
        pixelmaps[i]->origin_y = 0;
    }
    BrMapAddMany(pixelmaps, number_of_pixelmaps);
}

// FUNCTION: CARMA2_HW 0x0048f1d0
void C2_HOOK_FASTCALL DRLoadMaterials(const char* pPath_name) {
    br_material* material_array[100];
    int i;
    int number_of_materials;

    PossibleService();
    number_of_materials = BrMaterialLoadMany(pPath_name, material_array, CARPOCALYPSE2_ASIZE(material_array));
    for (i = 0; i < number_of_materials; i++) {
        material_array[i]->flags &= ~BR_MATF_LIGHT;
    }
    BrMaterialAddMany(material_array, number_of_materials);
}

// FUNCTION: CARMA2_HW 0x0048f230
void C2_HOOK_FASTCALL DRLoadModels(const char* pPath_name) {
    int i;
    br_model* model_array[100];
    int number_of_models;

    PossibleService();
    number_of_models = BrModelLoadMany(pPath_name, model_array, CARPOCALYPSE2_ASIZE(model_array));
    WhitenVertexRGB(model_array, number_of_models);
    for (i = 0; i < number_of_models; i++) {
        model_array[i]->flags = BR_MODF_UPDATEABLE;
    }
    BrModelAddMany(model_array, number_of_models);
}

// FUNCTION: CARMA2_HW 0x0048f290
void C2_HOOK_FASTCALL DRLoadActors(const char* pPath_name) {
    int i;
    br_actor* actor_array[100];
    int number_of_actors;

    PossibleService();
    number_of_actors = BrActorLoadMany(pPath_name, actor_array, CARPOCALYPSE2_ASIZE(actor_array));
    for (i = 0; i < number_of_actors; i++) {
        gActor_array[gNumber_of_actors] = actor_array[i];
        gNumber_of_actors++;
    }
}

// FUNCTION: CARMA2_HW 0x0048f2e0
void C2_HOOK_FASTCALL DRLoadLights(const char* pPath_name) {
    br_actor* actor;
    br_light* light;

    actor = BrActorAllocate(BR_ACTOR_LIGHT, NULL);
    light = actor->type_data;
    light->type = BR_LIGHT_DIRECT;
    light->colour = BR_COLOUR_RGB(gLighting_data.directional.red, gLighting_data.directional.green, gLighting_data.directional.blue);
    light->attenuation_c = 1.0f;
    BrMatrix34RotateX(&actor->t.t.mat, BR_ANGLE_DEG(-60));
    BrMatrix34PostRotateY(&actor->t.t.mat, BR_ANGLE_DEG(30));
    gLight_array[gNumber_of_lights] = actor;
    gNumber_of_lights++;
    EnableLights();
}

// FUNCTION: CARMA2_HW 0x0048f360
void C2_HOOK_FASTCALL LoadInFiles(const char* pThe_path, const char* pArchive_name, tPDForEveryFileRecurse_cbfn pAction_routine) {
    tPath_name the_path;
    tTWTVFS twt;

    PathCat(the_path, pThe_path, pArchive_name);
    twt = OpenPackFileAndSetTiffLoading(the_path);
    PFForEveryFile(the_path, pAction_routine);
    ClosePackFileAndSetTiffLoading(twt);
}

// FUNCTION: CARMA2_HW 0x0048f3b0
int C2_HOOK_FASTCALL TestForOriginalCarmaCDinDrive(void) {
    struct {
        tPath_name path_txt;
        tPath_name cd_data;
        tPath_name cd_cutscene;
        tPath_name cd;
    } paths;
    FILE* f;
    char paths_txt_first_char;

    paths.path_txt[0] = '\0';
    strcpy(paths.path_txt, gApplication_path);
    strcat(paths.path_txt, gDir_separator);
    strcat(paths.path_txt, "PATHS.TXT");
    if (PDCheckDriveExists(paths.path_txt)) {
        f = PFfopen(paths.path_txt, "rt");
        if (f != NULL) {

            paths_txt_first_char = PFfgetc(f);
            PFungetc(paths_txt_first_char, f);

            paths.cd[0] = '\0';
            GetALineAndDontArgue(f, paths.cd);
            PFfclose(f);
            strcpy(paths.cd_data, paths.cd);
            if (strchr(gDir_separator, paths.cd_data[strlen(paths.cd_data) - 1]) == NULL) {
                strcat(paths.cd_data, gDir_separator);
            }
            strcat(paths.cd_data, "DATA");

            if (DRStricmp(paths.cd, gApplication_path) != 0) {
                strcpy(paths.cd_cutscene, paths.cd_data);
                if (strchr(gDir_separator, paths.cd_cutscene[strlen(paths.cd_cutscene) - 1]) == NULL) {
                    strcat(paths.cd_cutscene, gDir_separator);
                }
                strcat(paths.cd_cutscene, "CUTSCENE");

                if (PDCheckDriveExists2(paths.cd_data, "GENERAL.TXT", 100)
                        && PDCheckDriveExists2(paths.cd, "CARMA.EXE", 1000000)
                        && PDCheckDriveExists2(paths.cd_cutscene, "MIX_INTR.SMK", 10000000)) {
                    if (paths_txt_first_char != '@') {
                        EncodeFile(paths.path_txt);
                    }
                    return 1;
                }
            }
        }
    }
    return 0;
}

// FUNCTION: CARMA2_HW 0x0048f7f0
void C2_HOOK_FASTCALL SetDefaultPixelmapFolderName(void) {

    gPedTexturePath = "PIXELMAP";
}