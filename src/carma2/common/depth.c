#include "depth.h"

extern br_scalar gCamera_yon;

#include "displays.h"
#include "errors.h"
#include "globvars.h"
#include "globvrkm.h"
#include "globvrpb.h"
#include "init.h"
#include "loading.h"
#include "platform.h"
#include "replay.h"
#include "skidmark.h"
#include "spark.h"
#include "tinted.h"
#include "trig.h"
#include "world.h"
#include "utility.h"

#include "carpocalypse2_macros.h"
#include "depth.h"

#include "car.h"
#include "world2.h"
#include "brucetrk.h"
#include "globvars.h"
#include "globvrkm.h"
#include "carpocalypse2_macros.h"

#define ACTOR_CAMERA(ACTOR) ((br_camera*)((ACTOR)->type_data))
static void C2_HOOK_FASTCALL FrobFog(void);
// GLOBAL: CARMA2_HW 0x0079ec44
tSpecial_volume* gDAT_0079ec44;


// GLOBAL: CARMA2_HW 0x0059118c
extern int gDepth_cueing_on;
// GLOBAL: CARMA2_HW 0x0067c4bc
int gDepth_shade_table_power;

// GLOBAL: CARMA2_HW 0x0079ec38
br_pixelmap* gFog_shade_table;

// GLOBAL: CARMA2_HW 0x0067c4a4
int gFog_shade_table_power;

// GLOBAL: CARMA2_HW 0x0079ec24
br_pixelmap* gAcid_shade_table;

// GLOBAL: CARMA2_HW 0x0067c4dc
int gAcid_shade_table_power;

// GLOBAL: CARMA2_HW 0x0079ec28
br_pixelmap* gWater_shade_table;

// GLOBAL: CARMA2_HW 0x0067c4cc
int gWater_shade_table_power;


// GLOBAL: CARMA2_HW 0x0067c4d8
br_actor* gForward_sky_actor;

// GLOBAL: CARMA2_HW 0x0067c4c8
float gSky_width;

// GLOBAL: CARMA2_HW 0x0067c4b4
float gSky_height;

// GLOBAL: CARMA2_HW 0x0067c4d0
float gSky_x_multiplier;

// GLOBAL: CARMA2_HW 0x0067c4d4
float gSky_y_multiplier;

// GLOBAL: CARMA2_HW 0x0067c4a8
int gHas_sky_texture;

// GLOBAL: CARMA2_HW 0x0067c4c0
br_actor* gRearview_sky_actor;

// GLOBAL: CARMA2_HW 0x0067c4ac
br_angle gOld_fov;

// GLOBAL: CARMA2_HW 0x0067c4b8
br_scalar gOld_yon;

// GLOBAL: CARMA2_HW 0x0079ec44
tSpecial_volume* gLast_camera_special_volume;

// GLOBAL: CARMA2_HW 0x00591188
int gSky_on = 1;

// GLOBAL: CARMA2_HW 0x0079ec1c
br_pixelmap* gSwap_sky_texture;

// GLOBAL: CARMA2_HW 0x0079ec20
br_pixelmap* gDepth_shade_table;

// GLOBAL: CARMA2_HW 0x0067c4e0
br_material* gHorizon_material;

// GLOBAL: CARMA2_HW 0x0067c4a0
br_model* gForward_sky_model;

// GLOBAL: CARMA2_HW 0x0074cee8
br_material* gMaterial[2];

// GLOBAL: CARMA2_HW 0x0059118c
int gDepth_cueing_on = 1;

// GLOBAL: CARMA2_HW 0x00591190
tDepth_effect_type gSwap_depth_effect_type = eDepth_effect_none;

// GLOBAL: CARMA2_HW 0x0079ec4c
int gSwap_depth_effect_start;

// GLOBAL: CARMA2_HW 0x0079ec48
int gSwap_depth_effect_end;

// GLOBAL: CARMA2_HW 0x0079ec34
int gSwap_depth_effect_colour_blue;

// GLOBAL: CARMA2_HW 0x0079ec3c
int gSwap_depth_effect_colour_red;

// GLOBAL: CARMA2_HW 0x0079ec40
int gSwap_depth_effect_colour_green;

#ifdef CARPOCALYPSE2_MATCHING
#endif
#ifdef CARPOCALYPSE2_MATCHING
extern br_actor* C2_HOOK_CDECL BrActorAdd();
extern br_actor* C2_HOOK_CDECL BrActorAllocate();
extern br_material* C2_HOOK_CDECL BrMaterialFind();
extern br_model* C2_HOOK_CDECL BrModelAdd();
extern br_model* C2_HOOK_CDECL BrModelAllocate();
extern void C2_HOOK_CDECL BrModelUpdate();
extern br_pixelmap* C2_HOOK_CDECL BrPixelmapAllocate();
extern br_pixelmap* C2_HOOK_CDECL BrTableAdd();
extern void C2_HOOK_FASTCALL DRPixelmapRectangleCopy(br_pixelmap* dst, br_int_16 dx, br_int_16 dy, br_pixelmap* src, br_int_16 sx, br_int_16 sy, br_uint_16 w, br_uint_16 h);
extern void C2_HOOK_FASTCALL DRPixelmapRectangleFill(br_pixelmap* dst, br_int_16 x, br_int_16 y, br_uint_16 w, br_uint_16 h, br_uint_32 colour);
extern void C2_NORETURN C2_HOOK_CDECL FatalError();
extern void C2_HOOK_FASTCALL GenerateSmokeShades(void);
extern void C2_HOOK_FASTCALL PossibleService(void);
extern void __cdecl _ftol();
extern char gFloat_00589c40[];
extern int gNo_fog;
extern br_pixelmap* gScreen;
extern br_angle gSky_image_height;
extern br_angle gSky_image_underground;
extern br_angle gSky_image_width;
extern br_actor* gUniverse_actor;
#endif
// FUNCTION: CARMA2_HW 0x00413f10
intptr_t C2_HOOK_CDECL SwitchCarModel(br_actor* pActor, void* pData) {
    if (pActor->user != NULL) {
        int* index = (int*)pData;
        tUser_crush_data *crush_data = pActor->user;
        br_model *model = crush_data->models[*index];
        if (model != NULL) {
            pActor->model = model;
            pActor->type = BR_ACTOR_MODEL;
            return 0;
        }
    }
    pActor->model = NULL;
    pActor->type = BR_ACTOR_NONE;
    return 0;
}

// FUNCTION: CARMA2_HW 0x00447110
void C2_HOOK_FASTCALL ToggleDepthCueing(void) {
    int enabled;

    enabled = GetDepthCueingOn();
    SetDepthCueingOn(!enabled);
    if (gProgram_state.current_depth_effect.type != eDepth_effect_none) {
        NewTextHeadupSlot(4, 0, 2000, -4, GetMiscString(eMiscString_depth_cueing_on));
    } else if (gSwap_depth_effect_type != eDepth_effect_none) {
        NewTextHeadupSlot(4, 0, 2000, -4, GetMiscString(eMiscString_depth_cueing_off));
    } else {
        NewTextHeadupSlot(4, 0, 2000, -4, GetMiscString(eMiscString_there_is_no_depth_cueing_for_this_race));
    }
}

br_model* C2_HOOK_FASTCALL CreateHorizonModel(br_actor* pCamera) {
    tU8 nbands;
    tU8 band;
    tU8 vertex;
    tU8 stripe;
    br_model* model;

    nbands = 21;
    model = BrModelAllocate(NULL, 88, 6 * nbands);
    model->flags |= BR_MODF_KEEP_ORIGINAL;
    for (band = 0; band < nbands; band++) {
        for (stripe = 0; stripe < 3; stripe++) {
            vertex = 6 * band + 2 * stripe;
            model->faces[vertex + 0].vertices[0] = stripe + 4 * band + 0;
            model->faces[vertex + 0].vertices[1] = stripe + 4 * band + 1;
            model->faces[vertex + 0].vertices[2] = stripe + 4 * band + 5;
            model->faces[vertex + 1].vertices[0] = stripe + 4 * band + 0;
            model->faces[vertex + 1].vertices[1] = stripe + 4 * band + 5;
            model->faces[vertex + 1].vertices[2] = stripe + 4 * band + 4;

            model->faces[vertex + 0].smoothing = 1;
            model->faces[vertex + 1].smoothing = 1;

            model->faces[vertex + 0].material = NULL;
            model->faces[vertex + 1].material = NULL;
        }
    }
    return model;
}

int Log2(int pNumber) {
    int i;
    int bits[16];

    bits[0] = 1;
    bits[1] = 2;
    bits[2] = 4;
    bits[3] = 8;
    bits[4] = 16;
    bits[5] = 32;
    bits[6] = 64;
    bits[7] = 128;
    bits[8] = 256;
    bits[9] = 512;
    bits[10] = 1024;
    bits[11] = 2048;
    bits[12] = 4096;
    bits[13] = 0x2000;
    bits[14] = 0x4000;
    bits[15] = 0x8000;
    for (i = CARPOCALYPSE2_ASIZE(bits) - 1; i >= 0; --i) {
        if (bits[i] & pNumber) {
            return i;
        }
    }
    return 0;
}

// FUNCTION: CARMA2_HW 0x00445860
void C2_HOOK_FASTCALL LoadDepthTable(char* pName, br_pixelmap** pTable, int* pPower) {
    tPath_name the_path;
    int i;
    int j;
    tU8 temp;

#define PTABLE_PIXEL_AT(X, Y) ((tU8*)((*pTable)->pixels))[(X) + (Y) * (*pTable)->row_bytes]

    PathCat(the_path, gApplication_path, "SHADETAB");
    PathCat(the_path, the_path, pName);
    *pTable = DRPixelmapLoad(the_path);
    if (*pTable == NULL) {
        FatalError(kFatalError_CannotLoadDepthCueShadeTable);
    }
    *pPower = Log2((*pTable)->height);
    for (i = 0; i < (*pTable)->width; i++) {
        for (j = 0; j < (*pTable)->height / 2; j++) {
            temp = PTABLE_PIXEL_AT(i, j);
            PTABLE_PIXEL_AT(i, j) = PTABLE_PIXEL_AT(i, ((*pTable)->height - j - 1));
            PTABLE_PIXEL_AT(i, ((*pTable)->height - j - 1)) = temp;
        }
    }

#undef PTABLE_PIXEL_AT
}

br_scalar C2_HOOK_FASTCALL Tan(br_scalar pAngle) {

    return sinf(BrAngleToRadian(pAngle)) / cosf(BrAngleToRadian(pAngle));
}

br_scalar C2_HOOK_FASTCALL EdgeU(br_angle pSky, br_angle pView, br_angle pPerfect) {
    br_scalar a;
    br_scalar b;
    br_scalar c;

    a = cosf(BrAngleToRadian(pPerfect));
    b = sinf(BrAngleToRadian(pView));
    c = cosf(BrAngleToRadian(pView));
    return b * a * a / (BrAngleToRadian(pSky) * (1.f + c));
}

br_scalar C2_HOOK_FASTCALL CalculateWrappingMultiplier(br_scalar pValue, br_scalar pYon) {
    br_scalar k;
    br_scalar trunc_k;
    int int_k;

    k = (float)(pYon * 1.3f * CARPOCALYPSE2_TAU / pValue);
    int_k = (int)k;
    if (k - (br_scalar)int_k <= .5f) {
        trunc_k = (br_scalar)int_k;
    } else {
        trunc_k = (br_scalar)int_k + 1.f;
    }
    return (float)(trunc_k / CARPOCALYPSE2_TAU * pValue);
}

// FUNCTION: CARMA2_HW 0x004459e0
void C2_NAKED C2_HOOK_FAKE_THISCALL DoHorizon(br_pixelmap* pRender_buffer, undefined4 pEdx, br_actor* pCamera, br_matrix34* pCamera_to_world) {
#ifndef CARPOCALYPSE2_MATCHING
    br_pixelmap* sky_map;
    br_camera* camera_data;
    double half_fov;
    double horizontal_scale;
    float tangent_half_fov;
    volatile br_angle horizontal_fov;
    br_angle yaw;
    int angle_scale;
    int source_x;
    int source_y;
    int screen_y;
    int copy_width;
    int copied_width;
    int remaining_width;
    int top_height;
    int bottom_height;
    float pitch_cosine;
    float pitch_sine;
    float angle_fraction;
    tU8 top_colour;
    tU8 bottom_colour;

    (void)pEdx;
    copied_width = 0;
    sky_map = gHorizon_material->colour_map;
    camera_data = pCamera->type_data;
    half_fov = (double)((tU16)camera_data->field_of_view >> 1) * (PI / 32768.0);
    tangent_half_fov = (float)(sin(half_fov) / cos(half_fov));
    horizontal_scale = (double)(camera_data->aspect * tangent_half_fov)
            * sky_map->width / pRender_buffer->width;
    horizontal_fov = BrRadianToAngle(atan(horizontal_scale));
    horizontal_fov *= 2;
    angle_scale = 0xfff0 / (int)(1.0f / ((double)horizontal_fov / 65536.0) + 0.5f);

    yaw = BrRadianToAngle(atan2(pCamera_to_world->m[2][0], pCamera_to_world->m[2][2]));
    yaw = (tU16)yaw;
    angle_fraction = (float)((double)angle_scale * (1.0 / 65536.0));
    source_x = (int)(-(((double)yaw * (1.0 / 65536.0)) / angle_fraction) * sky_map->width);
    for (; source_x < 0; source_x += sky_map->width) {
    }
    for (; sky_map->width < source_x; source_x -= sky_map->width) {
    }

    pitch_cosine = sqrt(pCamera_to_world->m[2][0] * pCamera_to_world->m[2][0]
            + pCamera_to_world->m[2][2] * pCamera_to_world->m[2][2]);
    pitch_sine = pCamera_to_world->m[2][1] / pitch_cosine;
    pitch_sine = pitch_sine / tangent_half_fov;
    source_y = ((int)gSky_image_underground * sky_map->height) / (int)gSky_image_height;
    source_y -= (int)(pitch_sine * pRender_buffer->height * 0.5f);
    source_y -= sky_map->height;
    screen_y = source_y;

    if (pRender_buffer->width != 0) {
        do {
            remaining_width = sky_map->width - source_x;
            copy_width = pRender_buffer->width - copied_width;
            if (remaining_width <= copy_width) {
                copy_width = remaining_width;
            }
            DRPixelmapRectangleCopy(pRender_buffer, copied_width - pRender_buffer->origin_x,
                    screen_y, sky_map, source_x - sky_map->origin_x, -sky_map->origin_y,
                    copy_width, sky_map->height);
            copied_width += copy_width;
            source_x = 0;
        } while (copied_width < pRender_buffer->width);
    }

    top_height = screen_y + pRender_buffer->origin_y;
    if (top_height > 0) {
        top_colour = *(tU8*)sky_map->pixels;
        DRPixelmapRectangleFill(pRender_buffer, -pRender_buffer->origin_x, -pRender_buffer->origin_y,
                pRender_buffer->width, top_height, top_colour);
    }

    bottom_height = pRender_buffer->height - screen_y - sky_map->height - pRender_buffer->origin_y;
    if (bottom_height > 0) {
        bottom_colour = *((tU8*)sky_map->pixels + sky_map->row_bytes * (sky_map->height - 1));
        DRPixelmapRectangleFill(pRender_buffer, -pRender_buffer->origin_x,
                screen_y + sky_map->height, pRender_buffer->width, bottom_height, bottom_colour);
    }
#else
    __asm {
            sub esp, 0x10
            mov eax, dword ptr [esp + 0x14]
            mov edx, dword ptr [gHorizon_material]
            push ebx
            push ebp
            mov eax, dword ptr [eax + 0x5c]
            push esi
            mov esi, dword ptr [edx + 0x40]
            push edi
            mov dx, word ptr [eax + 6]
            mov ebx, ecx
            shr dx, 1
            and edx, 0xffff
            xor edi, edi
            mov dword ptr [esp + 0x24], edx
            mov di, word ptr [ebx + 0x34]
            fild dword ptr [esp + 0x24]
            xor ebp, ebp
            mov dword ptr [esp + 0x18], edi
            mov bp, word ptr [esi + 0x34]
            mov dword ptr [esp + 0x1c], 0
            fmul qword ptr [gFloat_00589c40+0x90]
            fld dword ptr [eax + 0x10]
            mov dword ptr [esp + 0x14], ebp
            fld st(1)
            fsin
            fxch st(2)
            fcos
            fxch st(2)
            fst dword ptr [esp + 0x24]
            fxch st(2)
            fst dword ptr [esp + 0x24]
            fdivp st(2), st(0)
            fxch st(1)
            fstp dword ptr [esp + 0x10]
            fmul dword ptr [esp + 0x10]
            fild dword ptr [esp + 0x18]
            fild dword ptr [esp + 0x14]
            fstp dword ptr [esp + 0x24]
            fxch st(1)
            fmul dword ptr [esp + 0x24]
            fld qword ptr [gFloat_00589c40+0x78]
            fxch st(2)
            fdivp st(1), st(0)
            fxch st(1)
            fpatan
            fmul qword ptr [gFloat_00589c40+0x98]
            call _ftol
            shl eax, 1
            and eax, 0xffff
            mov dword ptr [esp + 0x18], eax
            fild dword ptr [esp + 0x18]
            fmul qword ptr [gFloat_00589c40+0xa0]
            fdivr dword ptr [gFloat_00589c40+0x84]
            fsub dword ptr [gFloat_00589c40+0xa8]
            call _ftol
            mov ecx, dword ptr [esp + 0x28]
            fld dword ptr [ecx + 0x18]
            fld dword ptr [ecx + 0x20]
            mov ecx, eax
            mov eax, 0xfff0
            cdq
            idiv ecx
            fpatan
            mov dword ptr [esp + 0x18], eax
            fmul qword ptr [gFloat_00589c40+0x98]
            call _ftol
            mov dx, ax
            mov eax, dword ptr [esp + 0x18]
            and eax, 0xffff
            and edx, 0xffff
            mov dword ptr [esp + 0x14], eax
            mov dword ptr [esp + 0x18], edx
            fild dword ptr [esp + 0x14]
            fild dword ptr [esp + 0x18]
            fmul qword ptr [gFloat_00589c40+0xa0]
            fxch st(1)
            fmul qword ptr [gFloat_00589c40+0xa0]
            fst dword ptr [esp + 0x18]
            fdivp st(1), st(0)
            fchs
            fmul dword ptr [esp + 0x24]
            call _ftol
            test eax, eax
            mov dword ptr [esp + 0x24], eax
            jge L00445b1a
    L00445b12:
            add eax, ebp
            js L00445b12
            mov dword ptr [esp + 0x24], eax
    L00445b1a:
            cmp ebp, eax
            jge L00445b28
    L00445b1e:
            sub eax, ebp
            cmp eax, ebp
            jg L00445b1e
            mov dword ptr [esp + 0x24], eax
    L00445b28:
            mov eax, dword ptr [esp + 0x28]
            xor ebp, ebp
            mov bp, word ptr [esi + 0x36]
            fld dword ptr [eax + 0x20]
            fld dword ptr [eax + 0x18]
            fst dword ptr [esp + 0x28]
            fld st(1)
            fxch st(1)
            fmul dword ptr [esp + 0x28]
            fld dword ptr [eax + 0x1c]
            fxch st(2)
            fmul st(0), st(3)
            xor eax, eax
            mov dword ptr [esp + 0x14], ebp
            mov ax, word ptr [ebx + 0x36]
            faddp st(1), st(0)
            mov dword ptr [esp + 0x18], eax
            fsqrt
            fst dword ptr [esp + 0x28]
            fdivp st(1), st(0)
            fdiv dword ptr [esp + 0x10]
            fild dword ptr [esp + 0x18]
            fmulp st(1), st(0)
            fmul dword ptr [gFloat_00589c40+0xac]
            call _ftol
            xor edx, edx
            mov ecx, eax
            mov dx, word ptr [gSky_image_underground]
            mov eax, edx
            imul eax, ebp
            fstp st(0)
            xor ebp, ebp
            mov bp, word ptr [gSky_image_height]
            cdq
            idiv ebp
            mov ebp, eax
            sub ebp, ecx
            mov ecx, dword ptr [esp + 0x14]
            sub ebp, ecx
            test edi, edi
            mov dword ptr [esp + 0x28], ebp
            jle L00445bf9
            mov ecx, dword ptr [esp + 0x24]
            mov ebp, dword ptr [esp + 0x1c]
    L00445bb0:
            xor eax, eax
            sub edi, ebp
            mov ax, word ptr [esi + 0x34]
            sub eax, ecx
            cmp edi, eax
            jl L00445bc0
            mov edi, eax
    L00445bc0:
            mov dx, word ptr [esi + 0x36]
            mov ax, word ptr [esi + 0x3a]
            sub cx, word ptr [esi + 0x38]
            push edx
            neg ax
            push edi
            push eax
            push ecx
            mov ecx, dword ptr [esp + 0x38]
            mov dx, bp
            push esi
            sub dx, word ptr [ebx + 0x38]
            push ecx
            mov ecx, ebx
            call DRPixelmapRectangleCopy
            add ebp, edi
            xor edi, edi
            mov di, word ptr [ebx + 0x34]
            xor ecx, ecx
            cmp ebp, edi
            jl L00445bb0
            mov ebp, dword ptr [esp + 0x28]
    L00445bf9:
            mov edx, dword ptr [esi + 8]
            mov al, byte ptr [edx]
            mov edx, ebp
            mov byte ptr [esp + 0x28], al
            mov ax, word ptr [ebx + 0x3a]
            movsx ecx, ax
            add edx, ecx
            test edx, edx
            jle L00445c37
            mov ecx, dword ptr [esp + 0x28]
            lea edx, [ebp + eax]
            and ecx, 0xff
            push ecx
            mov cx, word ptr [ebx + 0x34]
            push edx
            mov dx, word ptr [ebx + 0x38]
            neg eax
            push ecx
            push eax
            neg dx
            mov ecx, ebx
            call DRPixelmapRectangleFill
    L00445c37:
            mov di, word ptr [esi + 0x36]
            movsx edx, word ptr [esi + 0x28]
            mov ecx, edi
            and ecx, 0xffff
            lea eax, [ecx - 1]
            imul edx, eax
            mov eax, dword ptr [esi + 8]
            mov dl, byte ptr [edx + eax]
            xor eax, eax
            mov ax, word ptr [ebx + 0x36]
            mov byte ptr [esp + 0x28], dl
            sub eax, ebp
            sub eax, ecx
            movsx ecx, word ptr [ebx + 0x3a]
            sub eax, ecx
            test eax, eax
            jle L00445c8d
            mov edx, dword ptr [esp + 0x28]
            add ebp, edi
            and edx, 0xff
            mov ecx, ebx
            push edx
            mov dx, word ptr [ebx + 0x38]
            push eax
            mov ax, word ptr [ebx + 0x34]
            push eax
            push ebp
            neg dx
            call DRPixelmapRectangleFill
    L00445c8d:
            pop edi
            pop esi
            pop ebp
            pop ebx
            add esp, 0x10
            ret 8
    }
#endif
}

// FUNCTION: CARMA2_HW 0x00445e20
void C2_NAKED C2_HOOK_FASTCALL MungeSkyModel(br_actor* pCamera, br_model* pModel) {
#ifndef CARPOCALYPSE2_MATCHING
    br_camera* camera_data;
    br_scalar horizon_half_height;
    br_scalar horizon_half_width;
    br_scalar horizon_half_diag;
    br_scalar tan_half_fov;
    br_scalar sky_distance;
    br_angle half_diag_fov;
    tU8 nbands;
    tU8 band;
    tU8 vertex;
    tU8 stripe;
    br_scalar edge_u;
    br_scalar narrow_u;
    br_angle min_angle;
    br_angle angle_range;
    br_angle angle;

    camera_data = pCamera->type_data;
    tan_half_fov = Tan((float)(camera_data->field_of_view / 2));
    sky_distance = camera_data->yon_z - 1.f;
    horizon_half_width = sky_distance * tan_half_fov;
    horizon_half_height = horizon_half_width * camera_data->aspect;
    horizon_half_diag = sqrtf(horizon_half_height * horizon_half_height + horizon_half_width * horizon_half_width);
    half_diag_fov = BrRadianToAngle(atan2f(horizon_half_diag, sky_distance));
    edge_u = EdgeU(gSky_image_width, 2 * half_diag_fov, BR_ANGLE_DEG(10));
    narrow_u = edge_u / 2.f;
    gSky_width = horizon_half_width * 2.f;
    gSky_height = horizon_half_height * 2.f;
    gSky_x_multiplier = CalculateWrappingMultiplier(gSky_width, camera_data->yon_z);
    gSky_y_multiplier = CalculateWrappingMultiplier(gSky_height, camera_data->yon_z);

    for (vertex = 0; vertex < 88; vertex += 4) {
        pModel->vertices[vertex].map.v[0] = -edge_u;
    }
    for (vertex = 1; vertex < 88; vertex += 4) {
        pModel->vertices[vertex].map.v[0] = -narrow_u;
    }
    for (vertex = 2; vertex < 88; vertex += 4) {
        pModel->vertices[vertex].map.v[0] = narrow_u;
    }
    for (vertex = 3; vertex < 88; vertex += 4) {
        pModel->vertices[vertex].map.v[0] = edge_u;
    }
    for (vertex = 0; vertex < 88; vertex += 4) {
        pModel->vertices[vertex].p.v[0] = -horizon_half_diag;
    }
    for (vertex = 1; vertex < 88; vertex += 4) {
        pModel->vertices[vertex].p.v[0] = -(horizon_half_diag / 2.f);
    }
    for (vertex = 2; vertex < 88; vertex += 4) {
        pModel->vertices[vertex].p.v[0] = horizon_half_diag / 2.f;
    }
    for (vertex = 3; vertex < 88; vertex += 4) {
        pModel->vertices[vertex].p.v[0] = horizon_half_diag;
    }
    PossibleService();
    angle_range = -gSky_image_underground - (-BR_ANGLE_DEG(90) - half_diag_fov);
    for (band = 0; band < 2u; band++) {
        vertex = 4 * band;
        angle = -BR_ANGLE_DEG(90) - half_diag_fov + angle_range * band / 2;
        pModel->vertices[vertex].p.v[1] = sinf(BrAngleToRadian(angle)) * sky_distance;
        pModel->vertices[vertex].p.v[2] = -cosf(BrAngleToRadian(angle)) * sky_distance;
    }
    min_angle = -gSky_image_underground;
    angle_range = gSky_image_height;
    nbands = 18;
    for (band = 0; band < nbands; band++) {
        vertex = 4 * band + 8;
        pModel->vertices[vertex].p.v[1] = sinf(BrAngleToRadian(min_angle + angle_range * band / nbands)) * sky_distance;
        pModel->vertices[vertex].p.v[2] = -cosf(BrAngleToRadian(min_angle + angle_range * band / nbands)) * sky_distance;
    }
    min_angle = gSky_image_height - gSky_image_underground;
    angle_range = half_diag_fov + BR_ANGLE_DEG(90) - (gSky_image_height - gSky_image_underground);
    for (band = 0; band <= 1u; band++) {
        vertex = 4 * band + 80;
        angle = min_angle + angle_range * band;
        pModel->vertices[vertex].p.v[1] = sinf(BrAngleToRadian(angle)) * sky_distance;
        pModel->vertices[vertex].p.v[2] = -cosf(BrAngleToRadian(angle)) * sky_distance;
    }
    PossibleService();
    for (band = 0; band <= 21u; ++band) {
        vertex = 4 * band;
        for (stripe = 1; stripe < 4u; ++stripe) {
            pModel->vertices[vertex + stripe].p.v[1] = pModel->vertices[vertex].p.v[1];
            pModel->vertices[vertex + stripe].p.v[2] = pModel->vertices[vertex].p.v[2];
        }
    }

    BrModelUpdate(pModel, BR_MODU_ALL & ~BR_MODU_FACES);
#else
    __asm {
            sub esp, 0x14
            push ebx
            push ebp
            push esi
            push edi
            mov edi, dword ptr [ecx + 0x5c]
            mov esi, edx
            mov ax, word ptr [edi + 6]
            shr ax, 1
            and eax, 0xffff
            mov dword ptr [esp + 0x1c], eax
            fild dword ptr [esp + 0x1c]
            fld dword ptr [edi + 0xc]
            fsub dword ptr [gFloat_00589c40+0x84]
            fxch st(1)
            fmul qword ptr [gFloat_00589c40+0x90]
            fxch st(1)
            fstp dword ptr [esp + 0x14]
            fld dword ptr [edi + 0x10]
            fxch st(1)
            fst dword ptr [esp + 0x1c]
            fld dword ptr [esp + 0x1c]
            fxch st(1)
            fsin
            fxch st(1)
            fcos
            fxch st(1)
            fst dword ptr [esp + 0x1c]
            fxch st(1)
            fst dword ptr [esp + 0x1c]
            fdivp st(1), st(0)
            fmul dword ptr [esp + 0x14]
            fld dword ptr [esp + 0x14]
            fxch st(1)
            fxch st(2)
            fxch st(1)
            fxch st(1)
            fmul st(0), st(2)
            fld st(2)
            fmul st(0), st(3)
            fld st(1)
            fmul st(0), st(2)
            faddp st(1), st(0)
            fsqrt
            fst dword ptr [esp + 0x1c]
            fst dword ptr [esp + 0x18]
            fxch st(1)
            fxch st(2)
            fpatan
            fmul qword ptr [gFloat_00589c40+0x98]
            call _ftol
            mov cx, word ptr [gSky_image_width]
            mov ebp, eax
            mov word ptr [esp + 0x1c], cx
            mov dword ptr [esp + 0x20], ebp
            mov edx, dword ptr [esp + 0x1c]
            lea eax, [ebp + ebp]
            and eax, 0xffff
            and edx, 0xffff
            mov dword ptr [esp + 0x1c], eax
            mov dword ptr [esp + 0x10], edx
            fild dword ptr [esp + 0x1c]
            fld qword ptr [gFloat_00589c40+0xb0]
            fcos
            fxch st(1)
            fmul qword ptr [gFloat_00589c40+0x90]
            fild dword ptr [esp + 0x10]
            fxch st(2)
            fstp dword ptr [esp + 0x1c]
            fld st(0)
            fld st(1)
            fxch st(1)
            fcos
            fxch st(1)
            fsin
            fld dword ptr [esp + 0x1c]
            fxch st(4)
            fmul qword ptr [gFloat_00589c40+0x90]
            fxch st(2)
            fst dword ptr [esp + 0x1c]
            fxch st(4)
            fst dword ptr [esp + 0x1c]
            fmul dword ptr [esp + 0x1c]
            fxch st(2)
            fst dword ptr [esp + 0x1c]
            fxch st(4)
            fsub dword ptr [gFloat_00589c40+0xb8]
            fxch st(1)
            fst dword ptr [esp + 0x1c]
            fxch st(4)
            fmulp st(1), st(0)
            fxch st(1)
            fmulp st(3), st(0)
            fdivp st(2), st(0)
            fxch st(1)
            fstp dword ptr [esp + 0x1c]
            fstp st(0)
            fld st(0)
            fxch st(2)
            fadd st(0), st(0)
            fld dword ptr [esp + 0x1c]
            fxch st(3)
            fadd st(0), st(0)
            fxch st(1)
            fstp dword ptr [gSky_width]
            fstp dword ptr [gSky_height]
            fld dword ptr [edi + 0xc]
            fmul dword ptr [gFloat_00589c40+0xc0]
            fxch st(2)
            fmul dword ptr [gFloat_00589c40+0xac]
            fxch st(2)
            fmul dword ptr [gFloat_00589c40+0xc4]
            fdiv dword ptr [gSky_width]
            fstp st(1)
            fld st(0)
            call _ftol
            mov dword ptr [esp + 0x10], eax
            fild dword ptr [esp + 0x10]
            fxch st(1)
            fsub st(0), st(1)
            fcomp dword ptr [gFloat_00589c40+0xac]
            fnstsw ax
            test ah, 0x41
            jne L00445fad
            fsub dword ptr [gFloat_00589c40+0xb8]
    L00445fad:
            fmul dword ptr [gFloat_00589c40+0xc8]
            fmul dword ptr [gSky_width]
            fstp dword ptr [gSky_x_multiplier]
            fld dword ptr [edi + 0xc]
            fmul dword ptr [gFloat_00589c40+0xc0]
            fmul dword ptr [gFloat_00589c40+0xc4]
            fdiv dword ptr [gSky_height]
            fld st(0)
            call _ftol
            mov dword ptr [esp + 0x10], eax
            fild dword ptr [esp + 0x10]
            fxch st(1)
            fsub st(0), st(1)
            fcomp dword ptr [gFloat_00589c40+0xac]
            fnstsw ax
            test ah, 0x41
            jne L00445ffa
            fsub dword ptr [gFloat_00589c40+0xb8]
    L00445ffa:
            fmul dword ptr [gFloat_00589c40+0xc8]
            xor cl, cl
            xor eax, eax
            fmul dword ptr [gSky_height]
            fstp dword ptr [gSky_y_multiplier]
            fld dword ptr [esp + 0x1c]
            fchs
    L00446016:
            mov edx, dword ptr [esi + 8]
            lea eax, [eax + eax*4]
            add cl, 4
            fst dword ptr [edx + eax*8 + 0xc]
            mov byte ptr [esp + 0x10], cl
            mov eax, dword ptr [esp + 0x10]
            and eax, 0xff
            cmp eax, 0x58
            jl L00446016
            fstp st(0)
            fld st(0)
            mov cl, 1
            mov eax, 1
            fchs
    L00446042:
            mov edx, dword ptr [esi + 8]
            lea eax, [eax + eax*4]
            add cl, 4
            fst dword ptr [edx + eax*8 + 0xc]
            mov byte ptr [esp + 0x10], cl
            mov eax, dword ptr [esp + 0x10]
            and eax, 0xff
            cmp eax, 0x58
            jl L00446042
            fstp st(0)
            mov cl, 2
            mov eax, 2
    L0044606a:
            mov edx, dword ptr [esi + 8]
            lea eax, [eax + eax*4]
            add cl, 4
            fst dword ptr [edx + eax*8 + 0xc]
            mov byte ptr [esp + 0x10], cl
            mov eax, dword ptr [esp + 0x10]
            and eax, 0xff
            cmp eax, 0x58
            jl L0044606a
            fstp st(0)
            mov cl, 3
            mov eax, 3
    L00446092:
            mov edx, dword ptr [esi + 8]
            lea eax, [eax + eax*4]
            fld dword ptr [esp + 0x1c]
            add cl, 4
            fstp dword ptr [edx + eax*8 + 0xc]
            mov byte ptr [esp + 0x10], cl
            mov eax, dword ptr [esp + 0x10]
            and eax, 0xff
            cmp eax, 0x58
            jl L00446092
            fld dword ptr [esp + 0x18]
            xor cl, cl
            xor eax, eax
            fchs
    L004460bf:
            mov edx, dword ptr [esi + 8]
            lea eax, [eax + eax*4]
            add cl, 4
            fst dword ptr [edx + eax*8]
            mov byte ptr [esp + 0x10], cl
            mov eax, dword ptr [esp + 0x10]
            and eax, 0xff
            cmp eax, 0x58
            jl L004460bf
            fstp st(0)
            fld dword ptr [esp + 0x18]
            fmul dword ptr [gFloat_00589c40+0xa8]
            mov cl, 1
            mov eax, 1
    L004460f0:
            mov edx, dword ptr [esi + 8]
            lea eax, [eax + eax*4]
            add cl, 4
            fst dword ptr [edx + eax*8]
            mov byte ptr [esp + 0x10], cl
            mov eax, dword ptr [esp + 0x10]
            and eax, 0xff
            cmp eax, 0x58
            jl L004460f0
            fstp st(0)
            fld dword ptr [esp + 0x18]
            fmul dword ptr [gFloat_00589c40+0xac]
            mov cl, 2
            mov eax, 2
    L00446121:
            mov edx, dword ptr [esi + 8]
            lea eax, [eax + eax*4]
            add cl, 4
            fst dword ptr [edx + eax*8]
            mov byte ptr [esp + 0x10], cl
            mov eax, dword ptr [esp + 0x10]
            and eax, 0xff
            cmp eax, 0x58
            jl L00446121
            fstp st(0)
            mov cl, 3
            mov eax, 3
    L00446148:
            mov edx, dword ptr [esi + 8]
            lea eax, [eax + eax*4]
            fld dword ptr [esp + 0x18]
            add cl, 4
            fstp dword ptr [edx + eax*8]
            mov byte ptr [esp + 0x10], cl
            mov eax, dword ptr [esp + 0x10]
            and eax, 0xff
            cmp eax, 0x58
            jl L00446148
            call PossibleService
            mov ebx, 0xc004
            xor cl, cl
            sub ebx, ebp
            xor edi, edi
            mov ax, bx
            add ax, word ptr [gSky_image_underground]
            neg eax
            mov dword ptr [esp + 0x1c], eax
            and eax, 0xffff
            mov ebp, eax
    L00446191:
            mov eax, edi
            add edi, ebp
            cdq
            sub eax, edx
            mov edx, dword ptr [esi + 8]
            sar eax, 1
            add eax, ebx
            and eax, 0xffff
            mov dword ptr [esp + 0x18], eax
            mov eax, ecx
            fild dword ptr [esp + 0x18]
            and eax, 0xff
            add cl, 4
            fmul qword ptr [gFloat_00589c40+0x90]
            lea eax, [eax + eax*4]
            shl eax, 3
            fld st(0)
            fsin
            cmp cl, 8
            fmul dword ptr [esp + 0x14]
            fstp dword ptr [eax + edx + 4]
            mov edx, dword ptr [esi + 8]
            fcos
            fchs
            fmul dword ptr [esp + 0x14]
            fstp dword ptr [eax + edx + 8]
            jb L00446191
            mov ax, word ptr [gSky_image_height]
            mov bx, word ptr [gSky_image_underground]
            mov word ptr [esp + 0x1c], ax
            xor edi, edi
            mov ebp, dword ptr [esp + 0x1c]
            mov cl, 8
            neg bx
            and ebp, 0xffff
    L00446205:
            mov eax, 0x38e38e39
            imul edi
            sar edx, 2
            mov eax, edx
            add edi, ebp
            shr eax, 0x1f
            add edx, eax
            mov eax, ecx
            add edx, ebx
            and eax, 0xff
            and edx, 0xffff
            add cl, 4
            mov dword ptr [esp + 0x1c], edx
            mov edx, dword ptr [esi + 8]
            fild dword ptr [esp + 0x1c]
            lea eax, [eax + eax*4]
            shl eax, 3
            fmul qword ptr [gFloat_00589c40+0x90]
            cmp cl, 0x50
            fld st(0)
            fsin
            fmul dword ptr [esp + 0x14]
            fstp dword ptr [eax + edx + 4]
            mov edx, dword ptr [esi + 8]
            fcos
            fchs
            fmul dword ptr [esp + 0x14]
            fstp dword ptr [eax + edx + 8]
            jb L00446205
            mov di, word ptr [gSky_image_height]
            mov ebp, dword ptr [esp + 0x20]
            sub di, word ptr [gSky_image_underground]
            mov dl, 0x50
            sub ebp, edi
            add ebp, 0x3ffc
            xor bl, bl
    L0044627f:
            xor ax, ax
            mov ecx, edx
            mov al, bl
            and ecx, 0xff
            imul eax, ebp
            add eax, edi
            add dl, 4
            and eax, 0xffff
            mov dword ptr [esp + 0x20], eax
            mov eax, dword ptr [esi + 8]
            fild dword ptr [esp + 0x20]
            mov dword ptr [esp + 0x1c], eax
            lea eax, [ecx + ecx*4]
            mov ecx, dword ptr [esp + 0x1c]
            fmul qword ptr [gFloat_00589c40+0x90]
            shl eax, 3
            inc bl
            cmp dl, 0x54
            fld st(0)
            fsin
            fxch st(1)
            fcos
            fst dword ptr [esp + 0x20]
            fxch st(1)
            fmul dword ptr [esp + 0x14]
            fxch st(1)
            fchs
            fmul dword ptr [esp + 0x14]
            fxch st(1)
            fstp dword ptr [eax + ecx + 4]
            mov ecx, dword ptr [esi + 8]
            fstp dword ptr [eax + ecx + 8]
            jbe L0044627f
            call PossibleService
            xor bl, bl
    L004462ed:
            mov eax, ebx
            mov edi, 3
            and eax, 0xff
            lea ecx, [eax + eax*4]
            shl ecx, 3
            inc eax
            lea eax, [eax + eax*4]
            shl eax, 3
    L00446306:
            mov edx, dword ptr [esi + 8]
            add eax, 0x28
            dec edi
            mov ebp, dword ptr [edx + ecx + 4]
            mov dword ptr [eax + edx - 0x24], ebp
            mov edx, dword ptr [esi + 8]
            mov ebp, dword ptr [edx + ecx + 8]
            mov dword ptr [eax + edx - 0x20], ebp
            jne L00446306
            add bl, 4
            cmp bl, 0x54
            jbe L004462ed
            push 0x7f7f
            push esi
            call BrModelUpdate
            add esp, 8
            pop edi
            pop esi
            pop ebp
            pop ebx
            add esp, 0x14
            ret
    }
#endif
}

// FUNCTION: CARMA2_HW 0x00447250
void C2_HOOK_FASTCALL MungeForwardSky(void) {
    MungeSkyModel(gCamera, gForward_sky_model);
    gHas_sky_texture = gProgram_state.default_depth_effect.sky_texture != NULL;
}

// FUNCTION: CARMA2_HW 0x00446890
void C2_HOOK_FASTCALL ReduceDepthStart(void) {
    char s[256];

    if (gProgram_state.current_depth_effect.start > 3) {
        gProgram_state.current_depth_effect.start--;
    }
    sprintf(s, "Depth start reduced to %d", gProgram_state.current_depth_effect.start);
    NewTextHeadupSlot(4, 0, 500, -1, s);
    gProgram_state.default_depth_effect.start = gProgram_state.current_depth_effect.start;
}

// FUNCTION: CARMA2_HW 0x004468f0
void C2_HOOK_FASTCALL IncreaseDepthStart(void) {
    char s[256];

    if (gProgram_state.current_depth_effect.start < 13) {
        gProgram_state.current_depth_effect.start++;
    }
    sprintf(s, "Depth start increased to %d", gProgram_state.current_depth_effect.start);
    NewTextHeadupSlot(4, 0, 500, -1, s);
    gProgram_state.default_depth_effect.start = gProgram_state.current_depth_effect.start;
}

// FUNCTION: CARMA2_HW 0x00446950
void C2_HOOK_FASTCALL ReduceDepthEnd(void) {
    char s[256];

    if (gProgram_state.current_depth_effect.end < 13) {
        gProgram_state.current_depth_effect.end++;
    }
    sprintf(s, "Depth end reduced to %d", gProgram_state.current_depth_effect.end);
    NewTextHeadupSlot(4, 0, 500, -1, s);
    gProgram_state.default_depth_effect.end = gProgram_state.current_depth_effect.end;
}

// FUNCTION: CARMA2_HW 0x004469b0
void C2_HOOK_FASTCALL IncreaseDepthEnd(void) {
    char s[256];

    if (gProgram_state.current_depth_effect.end > 0) {
        gProgram_state.current_depth_effect.end--;
    }
    sprintf(s, "Depth end increased to %d", gProgram_state.current_depth_effect.end);
    NewTextHeadupSlot(4, 0, 500, -1, s);
    gProgram_state.default_depth_effect.end = gProgram_state.current_depth_effect.end;
}

// FUNCTION: CARMA2_HW 0x00446a10
void C2_HOOK_FASTCALL AssertYons(void) {
    br_camera* camera_ptr;
    int i;

    for (i = 0; i < (int)CARPOCALYPSE2_ASIZE(gCamera_list); i++) {
        camera_ptr = gCamera_list[i]->type_data;
        camera_ptr->yon_z = gYon_multiplier * gCamera_yon;
    }
}

// FUNCTION: CARMA2_HW 0x00447280
void C2_HOOK_FASTCALL MungeRearviewSky(void) {
}

// FUNCTION: CARMA2_HW 0x00447290
void C2_HOOK_FASTCALL SkyTextureChanged(void) {
    BrMaterialUpdate(gHorizon_material, BR_MATU_COLOURMAP);
}

// FUNCTION: CARMA2_HW 0x004472b0
void C2_HOOK_FASTCALL SetSkyColour(br_colour c) {
    gHorizon_material->colour = c;
}

// FUNCTION: CARMA2_HW 0x004472c0
void C2_HOOK_FASTCALL FUN_004472c0(br_model* pModel, tPMFMCB* pCallback) {
    tU16 f;
    br_material* possible_mat;
    br_material* new_mat;

    new_mat = NULL;
    for (f = 0; f < pModel->nfaces; f++) {
        if (pModel->faces[f].material != NULL) {
            possible_mat = pCallback(pModel, f);
            if (possible_mat != NULL) {
                pModel->faces[f].material = possible_mat;
                new_mat = possible_mat;
            }
        }
    }
    if (new_mat != NULL) {
        BrModelUpdate(pModel, BR_MODU_ALL);
    }
}

// FUNCTION: CARMA2_HW 0x00447220
void C2_HOOK_FASTCALL ChangeDepthEffect(void) {
    gProgram_state.current_depth_effect.type = gProgram_state.default_depth_effect.type;
    gProgram_state.current_depth_effect.sky_texture = gProgram_state.default_depth_effect.sky_texture;
    gProgram_state.current_depth_effect.start = gProgram_state.default_depth_effect.start;
    gProgram_state.current_depth_effect.end = gProgram_state.default_depth_effect.end;
}

// FUNCTION: CARMA2_HW 0x00445cb0
void C2_HOOK_FASTCALL DepthEffectSky(br_pixelmap* pRender_buffer, br_pixelmap* pDepth_buffer, br_actor* pCamera, br_matrix34* pCamera_to_world) {
    br_angle yaw;
    br_actor* actor;

    yaw = BrRadianToAngle(atan2(pCamera_to_world->m[2][0], pCamera_to_world->m[2][2]));
    if (gProgram_state.cockpit_on || (gAction_replay_camera_mode * gAction_replay_mode)
            || gAction_replay_camera_mode == kActionReplayCameraMode_Internal || gAdapt_sky_model_for_cockpit) {

        if (gRendering_mirror) {
            actor = gRearview_sky_actor;
        } else {
            actor = gForward_sky_actor;
            if (gOld_fov != ACTOR_CAMERA(gCamera)->field_of_view || gOld_yon != ACTOR_CAMERA(gCamera)->yon_z) {
                gOld_fov = ACTOR_CAMERA(gCamera)->field_of_view;
                gOld_yon = ACTOR_CAMERA(gCamera)->yon_z;
                MungeSkyModel(gCamera, gForward_sky_model);
            }
        }
        BrMatrix34RotateY(&actor->t.t.mat, yaw);
        BrVector3Copy(&actor->t.t.translate.t, (br_vector3*)pCamera_to_world->m[3]);
        gHorizon_material->map_transform.m[0][0] = 1.f;
        gHorizon_material->map_transform.m[0][1] = 0.f;
        gHorizon_material->map_transform.m[1][0] = 0.f;
        gHorizon_material->map_transform.m[1][1] = 1.f;
        gHorizon_material->map_transform.m[2][0] = -BrFixedToFloat(yaw) / BrFixedToFloat(gSky_image_width);
        gHorizon_material->map_transform.m[2][1] = 0.f;
        BrMaterialUpdate(gHorizon_material, BR_MATU_ALL);
        actor->render_style = BR_RSTYLE_FACES;
        BrZbsSceneRenderAdd(actor);
        actor->render_style = BR_RSTYLE_NONE;
    }
}

// FUNCTION: CARMA2_HW 0x00446680
void C2_HOOK_FASTCALL DoWobbleCamera(br_actor* pCamera) {
    float f_time;

    // GLOBAL: CARMA2_HW 0x00591194
    static float mag00 = 0.02f;
    // GLOBAL: CARMA2_HW 0x00591198
    static  float mag01 = 0.02f;
    // GLOBAL: CARMA2_HW 0x0059119c
    static float mag02 = 0.02f;
    // GLOBAL: CARMA2_HW 0x005911a0
    static float mag10 = 0.15f;
    // GLOBAL: CARMA2_HW 0x005911a4
    static float mag11 = 0.05f;
    // GLOBAL: CARMA2_HW 0x005911a8
    static float mag12 = 0.02f;
    // GLOBAL: CARMA2_HW 0x0067c4e4
    static float mag20;
    // GLOBAL: CARMA2_HW 0x0067c4e8
    static float mag21;
    // GLOBAL: CARMA2_HW 0x0067c4ec
    static float mag22;
    // GLOBAL: CARMA2_HW 0x005911ac
    static float period00 = 3000.f;
    // GLOBAL: CARMA2_HW 0x005911b0
    static float period01 = 3000.f;
    // GLOBAL: CARMA2_HW 0x005911b4
    static float period02 = 4000.f;
    // GLOBAL: CARMA2_HW 0x005911b8
    static float period10 = 2200.f;
    // GLOBAL: CARMA2_HW 0x005911bc
    static float period11 = 3300.f;
    // GLOBAL: CARMA2_HW 0x005911c0
    static float period12 = 3100.f;
    // GLOBAL: CARMA2_HW 0x005911c4
    static float period20 = 2800.f;
    // GLOBAL: CARMA2_HW 0x005911c8
    static float period21 = 2500.f;
    // GLOBAL: CARMA2_HW 0x005911cc
    static float period22 = 3900.f;

    f_time = (float)GetTotalTime();
    pCamera->t.t.mat.m[0][0] += FastScalarSin((int)fmod(f_time / period00 * 360.f, 360.f)) * mag00;
    pCamera->t.t.mat.m[0][1] += FastScalarSin((int)fmod(f_time / period01 * 360.f, 360.f)) * mag01;
    pCamera->t.t.mat.m[0][2] += FastScalarSin((int)fmod(f_time / period02 * 360.f, 360.f)) * mag02;
    pCamera->t.t.mat.m[1][0] += FastScalarSin((int)fmod(f_time / period10 * 360.f, 360.f)) * mag10;
    pCamera->t.t.mat.m[1][1] += FastScalarSin((int)fmod(f_time / period11 * 360.f, 360.f)) * mag11;
    pCamera->t.t.mat.m[1][2] += FastScalarSin((int)fmod(f_time / period12 * 360.f, 360.f)) * mag12;
    pCamera->t.t.mat.m[2][0] += FastScalarSin((int)fmod(f_time / period20 * 360.f, 360.f)) * mag20;
    pCamera->t.t.mat.m[2][1] += FastScalarSin((int)fmod(f_time / period21 * 360.f, 360.f)) * mag21;
    pCamera->t.t.mat.m[2][2] += FastScalarSin((int)fmod(f_time / period22 * 360.f, 360.f)) * mag22;
}

// FUNCTION: CARMA2_HW 0x00446340
void C2_HOOK_FASTCALL DoSpecialCameraEffect(br_actor* pCamera, br_matrix34* pCamera_to_world) {
    float f_time;
    // GLOBAL: CARMA2_HW 0x005911d0
    static float mag00 = 0.03f;
    // GLOBAL: CARMA2_HW 0x005911d4
    static float mag01 = 0.03f;
    // GLOBAL: CARMA2_HW 0x005911d8
    static float mag02 = 0.03f;
    // GLOBAL: CARMA2_HW 0x005911dc
    static float mag10 = 0.2f;
    // GLOBAL: CARMA2_HW 0x005911e0
    static float mag11 = 0.07f;
    // GLOBAL: CARMA2_HW 0x005911e4
    static float mag12 = 0.03f;
    // GLOBAL: CARMA2_HW 0x005911e8
    static float mag20 = 0.02f;
    // GLOBAL: CARMA2_HW 0x005911ec
    static float mag21 = 0.03f;
    // GLOBAL: CARMA2_HW 0x005911f0
    static float mag22 = 0.01f;
    // GLOBAL: CARMA2_HW 0x005911f4
    static float period00 = 550.f;
    // GLOBAL: CARMA2_HW 0x005911f8
    static float period01 = 700.f;
    // GLOBAL: CARMA2_HW 0x005911fc
    static float period02 = 200.f;
    // GLOBAL: CARMA2_HW 0x00591200
    static float period10 = 100.f;
    // GLOBAL: CARMA2_HW 0x00591204
    static float period11 = 1300.f;
    // GLOBAL: CARMA2_HW 0x00591208
    static float period12 = 500.f;
    // GLOBAL: CARMA2_HW 0x0059120c
    static float period20 = 800.f;
    // GLOBAL: CARMA2_HW 0x00591210
    static float period21 = 1500.f;
    // GLOBAL: CARMA2_HW 0x00591214
    static float period22 = 300.f;

    UpdateTintedPolys();
    if (gOn_drugs && !gAction_replay_mode) {
        TurnTintedPolyOn(gHud_tinted2);
        f_time = (float)(tS32)PDGetTotalTime();
        pCamera->t.t.mat.m[0][0] += FastScalarSin((int)fmod(f_time / period00 * 360.f, 360.f)) * mag00;
        pCamera->t.t.mat.m[0][1] += FastScalarSin((int)fmod(f_time / period01 * 360.f, 360.f)) * mag01;
        pCamera->t.t.mat.m[0][2] += FastScalarSin((int)fmod(f_time / period02 * 360.f, 360.f)) * mag02;
        pCamera->t.t.mat.m[1][0] += FastScalarSin((int)fmod(f_time / period10 * 360.f, 360.f)) * mag10;
        pCamera->t.t.mat.m[1][1] += FastScalarSin((int)fmod(f_time / period11 * 360.f, 360.f)) * mag11;
        pCamera->t.t.mat.m[1][2] += FastScalarSin((int)fmod(f_time / period12 * 360.f, 360.f)) * mag12;
        pCamera->t.t.mat.m[2][0] += FastScalarSin((int)fmod(f_time / period20 * 360.f, 360.f)) * mag20;
        pCamera->t.t.mat.m[2][1] += FastScalarSin((int)fmod(f_time / period21 * 360.f, 360.f)) * mag21;
        pCamera->t.t.mat.m[2][2] += FastScalarSin((int)fmod(f_time / period22 * 360.f, 360.f)) * mag22;
    } else {
        TurnTintedPolyOff(gHud_tinted2);
        gLast_camera_special_volume = FindSpecialVolume((br_vector3*)pCamera_to_world->m[3], gLast_camera_special_volume, 0);
        if (gLast_camera_special_volume != NULL) {
            if (gLast_camera_special_volume->camera_special_effect_index == 0) {
                DoWobbleCamera(pCamera);
            }
        }
        if (gHas_sky_texture && gSky_on) {
            if ((gProgram_state.current_depth_effect.sky_texture != NULL) == (gLast_camera_special_volume != NULL && gLast_camera_special_volume->sky_col == -2)) {
                ToggleSkyQuietly();
            }
        }
    }
    SetTintedFromSpecialVolume(gHud_tinted1, (br_vector3*)pCamera_to_world->m[3]);
    if (InWater(gHud_tinted1)) {
        TurnTintedPolyOn(gHud_tinted1);
        DoWobbleCamera(pCamera);
    } else {
        TurnTintedPolyOff(gHud_tinted1);
    }
}
// Log2

// CalculateWrappingMultiplier

br_scalar C2_HOOK_FASTCALL DepthCueingShiftToDistance(int pDistance) {

    return gCamera_yon * (br_scalar)pow(10.0, 0.1 * (double)pDistance);
}

// FUNCTION: CARMA2_HW 0x004451a0
void C2_HOOK_FASTCALL FogAccordingToGPSCDE(br_material* pMaterial) {
    int start;
    int end;
    int red;
    int green;
    int blue;

    red = gProgram_state.current_depth_effect.colour.red;
    green = gProgram_state.current_depth_effect.colour.green;
    blue = gProgram_state.current_depth_effect.colour.blue;
    start = gProgram_state.current_depth_effect.start;
    end = gProgram_state.current_depth_effect.end;

    switch (gProgram_state.current_depth_effect.type) {
    case eDepth_effect_none:
        pMaterial->flags &= ~BR_MATF_FOG_LOCAL;
        break;
    case eDepth_effect_darkness:
        pMaterial->fog_min = DepthCueingShiftToDistance(-start);
        pMaterial->fog_max = DepthCueingShiftToDistance(end);
        pMaterial->fog_colour = 0;
        pMaterial->flags |= BR_MATF_FOG_LOCAL;
        break;
    case eDepth_effect_fog:
        pMaterial->fog_min = DepthCueingShiftToDistance(-start);
        pMaterial->fog_max = DepthCueingShiftToDistance(end);
        pMaterial->fog_colour = BR_COLOUR_RGB(0xf8, 0xf8, 0xf8);
        pMaterial->flags |= BR_MATF_FOG_LOCAL;
        break;
    case eDepth_effect_colour:
        pMaterial->fog_min = DepthCueingShiftToDistance(-start);
        pMaterial->fog_max = DepthCueingShiftToDistance(end);
        pMaterial->fog_colour = BR_COLOUR_RGB(red, green, blue);
        pMaterial->flags |= BR_MATF_FOG_LOCAL;
        break;
    }
    BrMaterialUpdate(pMaterial, BR_MATU_ALL);
}

void C2_HOOK_FASTCALL FogCars(void) {
    int i;

    for (i = 0; i < gCurrent_race.number_of_racers; i++) {
        int j;
        tCar_spec* car = gCurrent_race.opponent_list[i].car_spec;

        for (j = 0; j < car->count_detail_levels; j++) {
            SwitchCarModels(car, j);
            ProcessMaterials(car->car_model_actor, FogAccordingToGPSCDE);
        }
        SwitchCarModels(car, 0);
    }
}

static void C2_HOOK_FASTCALL FrobFog(void) {
    int i;
    br_material* material;

    if (gTrack_actor != NULL) {
        ProcessMaterials(gTrack_actor, FogAccordingToGPSCDE);
    }
    if (gNon_track_actor != NULL) {
        ProcessMaterials(gNon_track_actor, FogAccordingToGPSCDE);
    }
    FogCars();

    material = BrMaterialFind("GIBSLICK");
    if (material != NULL) {
        FogAccordingToGPSCDE(material);
    }
    material = BrMaterialFind("PEDSMEAR");
    if (material != NULL) {
        FogAccordingToGPSCDE(material);
    }
    for (i = 0; i < (int)CARPOCALYPSE2_ASIZE(gMaterial); i++) {
        FogAccordingToGPSCDE(gMaterial[i]);
    }
    for (i = 0; i < (int)CARPOCALYPSE2_ASIZE(gCurrent_race.material_modifiers); i++) {
        if (gCurrent_race.material_modifiers[i].skid_mark_material != NULL) {
            FogAccordingToGPSCDE(gCurrent_race.material_modifiers[i].skid_mark_material);
        }
    }
    FogAccordingToGPSCDE(gDefault_track_material);
}

// FUNCTION: CARMA2_HW 0x00445500
void C2_NAKED C2_HOOK_FASTCALL MungeSkyVs(br_model* pModel, br_material* pMaterial) {
#ifndef CARPOCALYPSE2_MATCHING
    int i;
    float tex_y;
    float f_height;
    int height;
    int vertex_idx;

    if (pMaterial->colour_map == NULL) {
        return;
    }
    if (pMaterial->flags & BR_MATF_MAP_INTERPOLATION) {
        height = pMaterial->colour_map->height - 2;
        tex_y = 1.0f / (float)pMaterial->colour_map->height;
    } else {
        height = pMaterial->colour_map->height;
        tex_y = 0.0f;
    }
    for (i = 0; i < 12; i++) {
        pModel->vertices[i].map.v[1] = 1.0f - tex_y;
    }
    for (i = 80; i < 88; i++) {
        pModel->vertices[i].map.v[1] = tex_y;
    }
    f_height = (float)height;
    vertex_idx = 12;
    for (i = 0; i < 17; vertex_idx+=4, i++) {
        int j;

        pModel->vertices[vertex_idx].map.v[1] = tex_y + (float)(17 - i) * f_height / 18.0f / pMaterial->colour_map->height;
        for (j = 1; j < 4; j++) {
            pModel->vertices[vertex_idx + j].map.v[1] = pModel->vertices[vertex_idx].map.v[1];
        }
    }
#else
    __asm {
            sub esp, 0xc
            push edi
            mov edi, edx
            mov dword ptr [esp + 0xc], edi
            mov eax, dword ptr [edi + 0x40]
            test eax, eax
            je L0044560f
            mov edx, dword ptr [edi + 0x20]
            push esi
            push ebp
            push ebx
            test edx, 0x20000
            je L00445540
            xor edx, edx
            mov dx, word ptr [eax + 0x36]
            mov dword ptr [esp + 0x10], edx
            add edx, -2
            fild dword ptr [esp + 0x10]
            mov dword ptr [esp + 0x10], edx
            fdivr qword ptr [gFloat_00589c40+0x78]
            jmp L00445550
    L00445540:
            fld dword ptr [gFloat_00589c40+0x80]
            xor edx, edx
            mov dx, word ptr [eax + 0x36]
            mov dword ptr [esp + 0x10], edx
    L00445550:
            fld dword ptr [gFloat_00589c40+0x84]
            fsub st(0), st(1)
            xor eax, eax
            mov edx, 0xc
    L0044555f:
            mov esi, dword ptr [ecx + 8]
            add eax, 0x28
            dec edx
            fst dword ptr [eax + esi - 0x18]
            jne L0044555f
            fstp st(0)
            mov dl, 0x50
            mov eax, 0x50
    L00445575:
            mov esi, dword ptr [ecx + 8]
            lea eax, [eax + eax*4]
            inc dl
            fst dword ptr [esi + eax*8 + 0x10]
            mov byte ptr [esp + 0x14], dl
            mov eax, dword ptr [esp + 0x14]
            and eax, 0xff
            cmp eax, 0x58
            jl L00445575
            fild dword ptr [esp + 0x10]
            mov dl, 0xc
            mov dword ptr [esp + 0x10], 0x11
            jmp L004455a7
    L004455a3:
            mov edi, dword ptr [esp + 0x18]
    L004455a7:
            fild dword ptr [esp + 0x10]
            mov edi, dword ptr [edi + 0x40]
            xor ebx, ebx
            mov eax, edx
            mov bx, word ptr [edi + 0x36]
            and eax, 0xff
            fmul st(0), st(1)
            mov dword ptr [esp + 0x14], ebx
            mov edi, dword ptr [ecx + 8]
            lea esi, [eax + eax*4]
            mov ebx, 3
            fmul dword ptr [gFloat_00589c40+0x88]
            shl esi, 3
            inc eax
            fidiv dword ptr [esp + 0x14]
            lea eax, [eax + eax*4]
            shl eax, 3
            fsubr st(0), st(2)
            fstp dword ptr [esi + edi + 0x10]
    L004455e6:
            mov edi, dword ptr [ecx + 8]
            add eax, 0x28
            dec ebx
            mov ebp, dword ptr [edi + esi + 0x10]
            mov dword ptr [eax + edi - 0x18], ebp
            jne L004455e6
            mov esi, dword ptr [esp + 0x10]
            add dl, 4
            dec esi
            cmp dl, 0x50
            mov dword ptr [esp + 0x10], esi
            jb L004455a3
            fstp st(0)
            pop ebx
            pop ebp
            fstp st(0)
            pop esi
    L0044560f:
            pop edi
            add esp, 0xc
            ret
    }
#endif
}

// FUNCTION: CARMA2_HW 0x00445340
void C2_HOOK_FASTCALL InstantDepthChange(tDepth_effect_type pType, br_pixelmap* pSky_texture, int pStart, int pEnd, int pRed, int pGreen, int pBlue, int pParam_8) {

    if (pType == eDepth_effect_none) {
        pStart = 3;
        pEnd = 3;
    }
    gProgram_state.current_depth_effect.sky_texture = pSky_texture;

    gHorizon_material->colour_map = pSky_texture;
    BrMaterialUpdate(gHorizon_material, BR_MATU_ALL);
    MungeSkyVs(gForward_sky_model, gHorizon_material);

    gProgram_state.current_depth_effect.type = pType;
    gProgram_state.current_depth_effect.start = pStart;
    gProgram_state.current_depth_effect.end = pEnd;
    gProgram_state.current_depth_effect.colour.red = pRed;
    gProgram_state.current_depth_effect.colour.green = pGreen;
    gProgram_state.current_depth_effect.colour.blue = pBlue;

    gProgram_state.default_depth_effect.type = pType;
    gProgram_state.default_depth_effect.start = pStart;
    gProgram_state.default_depth_effect.end = pEnd;
    gProgram_state.default_depth_effect.colour.red = pRed;
    gProgram_state.default_depth_effect.colour.green = pGreen;
    gProgram_state.default_depth_effect.colour.blue = pBlue;
    if (gNo_fog && pParam_8) {
        FrobFog();
    }
}

// Tan

// EdgeU

// MungeSkyModel

// CreateHorizonModel

// LoadDepthTable

// FUNCTION: CARMA2_HW 0x00445620
void C2_NAKED C2_HOOK_FASTCALL InitDepthEffects(void) {
#ifndef CARPOCALYPSE2_MATCHING
#ifdef CARPOCALYPSE2_FIX_BUGS
    /* The original function was stubbed out ("no-op for Linux boot"), which left
     * gForward_sky_model, gHorizon_material and the sky actors all NULL. Downstream
     * code (MungeSkyModel via LoadCar->AdjustRenderScreenSize->MungeForwardSky,
     * InstantDepthChange, DepthEffectSky) dereferences these unconditionally, so
     * the game would crash on the first race. Build the horizon geometry, material
     * and sky actors for real. Shade tables are loaded from DATA/SHADETAB. */
    br_material* horizon_material;
    br_actor* forward_actor;
    br_actor* rearview_actor;

    LoadDepthTable("DEPTHCUE.TAB", &gDepth_shade_table, &gDepth_shade_table_power);
    LoadDepthTable("FOG.TAB", &gFog_shade_table, &gFog_shade_table_power);
    LoadDepthTable("ACIDFOG.TAB", &gAcid_shade_table, &gAcid_shade_table_power);

    horizon_material = BrMaterialAllocate("Horizon");
    horizon_material->flags |= BR_MATF_MAP_INTERPOLATION;
    /* The sky is a self-illuminated backdrop: BRender's default material (which
     * BrMaterialAllocate copies) is lit, and the vertex shader would modulate
     * the sky texture by the material colour, which the game keeps at 0 (the
     * net-game SetSkyColour path). The original HORIZON.MAT is unlit, so drop
     * BR_MATF_LIGHT or the whole sky renders black. */
    horizon_material->flags &= ~BR_MATF_LIGHT;
    horizon_material->colour = 0;
    BrMaterialAdd(horizon_material);
    gHorizon_material = horizon_material;

    gForward_sky_model = CreateHorizonModel(gCamera);
    BrModelAdd(gForward_sky_model);

    forward_actor = BrActorAllocate(BR_ACTOR_MODEL, NULL);
    forward_actor->model = gForward_sky_model;
    forward_actor->material = horizon_material;
    gForward_sky_actor = forward_actor;

    rearview_actor = BrActorAllocate(BR_ACTOR_MODEL, NULL);
    rearview_actor->model = gForward_sky_model;
    rearview_actor->material = horizon_material;
    gRearview_sky_actor = rearview_actor;

    gSky_image_width = BR_ANGLE_DEG(360);
    gSky_image_height = BR_ANGLE_DEG(90);
    gSky_image_underground = BR_ANGLE_DEG(90);
#else
    br_model* sky_model;
    int row;
    int col;
    int u_base;
    int v_base;
    tU8 band;

    LoadDepthTable("DEPTHCUE.TAB", &gDepth_shade_table, &gDepth_shade_table_power);
    LoadDepthTable("FOG.TAB", &gFog_shade_table, &gFog_shade_table_power);
    LoadDepthTable("ACIDFOG.TAB", &gAcid_shade_table, &gAcid_shade_table_power);
    LoadDepthTable("BLUEGIT.TAB", &gWater_shade_table, &gWater_shade_table_power);
    GenerateSmokeShades();

    gHorizon_material = BrMaterialFind("HORIZON.MAT");
    if (gHorizon_material == NULL) {
        FatalError(kFatalError_CannotFindSkyMaterial_S);
    }

    if (gScreen->type == BR_PMT_INDEX_8 && gNo_fog == 0) {
        gHorizon_material->index_blend = BrPixelmapAllocate(BR_PMT_INDEX_8, 0x100, 0x100, NULL, 0);
        BrTableAdd(gHorizon_material->index_blend);
        for (row = 0; row < 0x10000; row += 0x100) {
            for (col = 0; col < 0x100; col++) {
                ((tU8*)gHorizon_material->index_blend->pixels)[row + col] = (tU8)col;
            }
        }
        gHorizon_material->flags |= BR_MATF_PERSPECTIVE;
    }

    gHorizon_material->flags |= BR_MATF_MAP_INTERPOLATION;

    sky_model = BrModelAllocate(NULL, 0x58, 0x7e);
    sky_model->flags |= BR_MODF_KEEP_ORIGINAL;

    band = 0;
    {
        int vtxidx;
        int cur;
        tU8 stripe;

        vtxidx = 0;
        for (band = 0; band < 0x15; band++) {
            u_base = 4 * band;
            v_base = u_base + 4;
            cur = vtxidx;
            for (stripe = 0; stripe < 3; stripe++) {
                *((tU16*)((tU8*)sky_model->faces + cur)) = (tU16)(u_base + stripe);
                *((tU16*)((tU8*)sky_model->faces + cur + 2)) = (tU16)(u_base + stripe + 1);
                *((tU16*)((tU8*)sky_model->faces + cur + 4)) = (tU16)(u_base + stripe + 5);
                *((tU16*)((tU8*)sky_model->faces + cur + 0x28)) = (tU16)(u_base + stripe);
                *((tU16*)((tU8*)sky_model->faces + cur + 0x2a)) = (tU16)(u_base + stripe + 1);
                *((tU16*)((tU8*)sky_model->faces + cur + 0x2c)) = (tU16)(v_base + stripe);
                *((tU16*)((tU8*)sky_model->faces + cur + 6)) = 1;
                *((tU16*)((tU8*)sky_model->faces + cur + 0x2e)) = 1;
                *((tU32*)((tU8*)sky_model->faces + cur + 8)) = 0;
                *((tU32*)((tU8*)sky_model->faces + cur + 0x30)) = 0;
                cur += 0x50;
            }
            vtxidx = cur;
        }
    }

    gForward_sky_model = sky_model;
    BrModelAdd(gForward_sky_model);

    gForward_sky_actor = BrActorAllocate(BR_ACTOR_MODEL, NULL);
    gForward_sky_actor->model = gForward_sky_model;
    gForward_sky_actor->material = gHorizon_material;
    gForward_sky_actor->render_style = BR_RSTYLE_NONE;
    BrActorAdd(gUniverse_actor, gForward_sky_actor);
#endif
#else
    __asm {
            sub esp, 0xc
            mov edx, offset gDepth_shade_table
            mov ecx, offset gSwap_depth_effect_type+0xcc
            push ebx
            push ebp
            push esi
            push edi
            push offset gDepth_shade_table_power
            call LoadDepthTable
            mov edx, offset gFog_shade_table
            mov ecx, offset gSwap_depth_effect_type+0xc4
            push offset gFog_shade_table_power
            call LoadDepthTable
            mov edx, offset gAcid_shade_table
            mov ecx, offset gSwap_depth_effect_type+0xb8
            push offset gAcid_shade_table_power
            call LoadDepthTable
            mov edx, offset gWater_shade_table
            mov ecx, offset gSwap_depth_effect_type+0xac
            push offset gWater_shade_table_power
            call LoadDepthTable
            call GenerateSmokeShades
            push offset gSwap_depth_effect_type+0xa0
            call BrMaterialFind
            add esp, 4
            mov dword ptr [gHorizon_material], eax
            test eax, eax
            jne L004456a1
            push 0x59
            call FatalError
            mov eax, dword ptr [gHorizon_material]
            add esp, 4
    L004456a1:
            mov ecx, dword ptr [gScreen]
            cmp byte ptr [ecx + 0x2c], 3
            jne L00445727
            mov ecx, dword ptr [gNo_fog]
            test ecx, ecx
            jne L00445727
            push 0
            push 0
            push 0x100
            push 0x100
            push 3
            call BrPixelmapAllocate
            mov edx, dword ptr [gHorizon_material]
            add esp, 0x14
            mov dword ptr [edx + 0x4c], eax
            mov eax, dword ptr [gHorizon_material]
            mov ecx, dword ptr [eax + 0x4c]
            push ecx
            call BrTableAdd
            add esp, 4
            xor ecx, ecx
    L004456eb:
            xor eax, eax
    L004456ed:
            mov edx, dword ptr [gHorizon_material]
            mov edx, dword ptr [edx + 0x4c]
            mov edx, dword ptr [edx + 8]
            add edx, eax
            mov byte ptr [edx + ecx], al
            inc eax
            cmp eax, 0x100
            jl L004456ed
            add ecx, 0x100
            cmp ecx, 0x10000
            jl L004456eb
            mov eax, dword ptr [gHorizon_material]
            mov ecx, dword ptr [eax + 0x20]
            or ecx, 0x20
            mov dword ptr [eax + 0x20], ecx
            mov eax, dword ptr [gHorizon_material]
    L00445727:
            mov ebp, dword ptr [eax + 0x20]
            push 0x7e
            or ebp, 0x20000
            push 0x58
            push 0
            mov dword ptr [eax + 0x20], ebp
            call BrModelAllocate
            or byte ptr [eax + 0x20], 2
            add esp, 0xc
            mov byte ptr [esp + 0x13], 0
            xor edi, edi
    L0044574c:
            movzx dx, byte ptr [esp + 0x13]
            xor bl, bl
            lea ecx, [edx*4]
            lea edx, [edx*4 + 4]
            mov dword ptr [esp + 0x14], ecx
            mov dword ptr [esp + 0x18], edx
            mov ecx, edi
    L0044576c:
            mov edx, dword ptr [esp + 0x14]
            mov edi, dword ptr [eax + 0xc]
            movzx si, bl
            add edx, esi
            mov word ptr [ecx + edi], dx
            mov ebp, dword ptr [eax + 0xc]
            lea edi, [edx + 1]
            mov word ptr [ecx + ebp + 2], di
            mov ebp, dword ptr [eax + 0xc]
            lea edi, [edx + 5]
            mov word ptr [ecx + ebp + 4], di
            mov ebp, dword ptr [eax + 0xc]
            mov word ptr [ecx + ebp + 0x28], dx
            mov edx, dword ptr [eax + 0xc]
            mov word ptr [ecx + edx + 0x2a], di
            mov edx, dword ptr [esp + 0x18]
            add edx, esi
            mov esi, dword ptr [eax + 0xc]
            mov word ptr [ecx + esi + 0x2c], dx
            mov esi, dword ptr [eax + 0xc]
            mov edx, 1
            mov word ptr [ecx + esi + 6], dx
            mov esi, dword ptr [eax + 0xc]
            mov word ptr [ecx + esi + 0x2e], dx
            mov edx, dword ptr [eax + 0xc]
            xor esi, esi
            inc bl
            mov dword ptr [ecx + edx + 8], esi
            mov edx, dword ptr [eax + 0xc]
            mov dword ptr [ecx + edx + 0x30], esi
            add ecx, 0x50
            cmp bl, 3
            jb L0044576c
            mov dl, byte ptr [esp + 0x13]
            mov edi, ecx
            inc dl
            cmp dl, 0x15
            mov byte ptr [esp + 0x13], dl
            jb L0044574c
            push eax
            mov dword ptr [gForward_sky_model], eax
            call BrModelAdd
            add esp, 4
            push esi
            push 1
            call BrActorAllocate
            mov ecx, dword ptr [gForward_sky_model]
            mov dword ptr [gForward_sky_actor], eax
            mov dword ptr [eax + 0x18], ecx
            mov edx, dword ptr [gForward_sky_actor]
            mov eax, dword ptr [gHorizon_material]
            add esp, 8
            mov dword ptr [edx + 0x1c], eax
            mov ecx, dword ptr [gForward_sky_actor]
            mov byte ptr [ecx + 0x20], 1
            mov edx, dword ptr [gForward_sky_actor]
            mov eax, dword ptr [gUniverse_actor]
            push edx
            push eax
            call BrActorAdd
            add esp, 8
            pop edi
            pop esi
            pop ebp
            pop ebx
            add esp, 0xc
            ret
    }
#endif
}

// DoHorizon

// DoDepthCue

// DoFog

// DepthEffect

// DepthEffectSky

// DoWobbleCamera

// DoDrugWobbleCamera

// DoSubaquaCam

// DoSpecialCameraEffect

// AssertYons

// FUNCTION: CARMA2_HW 0x00446a40
void C2_HOOK_FASTCALL IncreaseYon(void) {
    char s[256];
    int yon;

    gCamera_yon += 5.f;
    AssertYons();
    yon = (int)((br_camera*)gCamera_list[1]->type_data)->yon_z;
    sprintf(s, GetMiscString(eMiscString_yon_increased_to_D), yon);
    NewTextHeadupSlot(4, 0, 2000, -4, s);
}

// FUNCTION: CARMA2_HW 0x00446ad0
void C2_HOOK_FASTCALL DecreaseYon(void) {
    char s[256];
    int yon;

    gCamera_yon -= 5.f;
    if (!(gCamera_yon >= 5.f)) {
        gCamera_yon = 5.f;
    }
    AssertYons();
    yon = (int)((br_camera*)gCamera_list[1]->type_data)->yon_z;
    sprintf(s, GetMiscString(eMiscString_yon_decreased_to_D), yon);
    NewTextHeadupSlot(4, 0, 2000, -4, s);
}

// FUNCTION: CARMA2_HW 0x00446b70
void C2_HOOK_STDCALL SetYon(br_scalar pYon) {
    int i;
    br_camera* camera_ptr;

    if (pYon < 5.0f) {
        pYon = 5.0f;
    }

    for (i = 0; i < CARPOCALYPSE2_ASIZE(gCamera_list); i++) {
        if (gCamera_list[i] != NULL) {
            camera_ptr = gCamera_list[i]->type_data;
            camera_ptr->yon_z = pYon;
        }
    }
    gCamera_yon = pYon;
}

// FUNCTION: CARMA2_HW 0x00446bb0
br_scalar C2_HOOK_STDCALL GetYon(void) {
    return gCamera_yon;
}

// FUNCTION: CARMA2_HW 0x00446bc0
void C2_HOOK_STDCALL IncreaseAngle(void) {
    br_camera* camera_ptr;
    int i;
    char s[256];

    for (i = 0; i < (int)CARPOCALYPSE2_ASIZE(gCamera_list); i++) {
        camera_ptr = gCamera_list[i]->type_data;
        camera_ptr->field_of_view += BrDegreeToAngle(2.5);
        if (camera_ptr->field_of_view > BrDegreeToAngle(170)) {
            camera_ptr->field_of_view = BrDegreeToAngle(170);
        }
        sprintf(s, "Camera angle increased to %d", gProgram_state.current_depth_effect.end);
        NewTextHeadupSlot(4, 0, 500, -1, s);
    }
}

// FUNCTION: CARMA2_HW 0x00446c40
void C2_HOOK_STDCALL DecreaseAngle(void) {
    br_camera* camera_ptr;
    int i;
    char s[256];

    for (i = 0; i < (int)CARPOCALYPSE2_ASIZE(gCamera_list); i++) {
        camera_ptr = gCamera_list[i]->type_data;
        camera_ptr->field_of_view -= BrDegreeToAngle(2.5);
        if (camera_ptr->field_of_view < BrDegreeToAngle(10)) {
            camera_ptr->field_of_view = BrDegreeToAngle(10);
        }
        sprintf(s, "Camera angle decreased to %d", gProgram_state.current_depth_effect.end);
        NewTextHeadupSlot(4, 0, 500, -1, s);
    }
}

// FUNCTION: CARMA2_HW 0x00446e00
int C2_HOOK_FASTCALL GetSkyTextureOn(void) {
    return gSky_on;
}

// FUNCTION: CARMA2_HW 0x00446e10
void C2_HOOK_FASTCALL SetSkyTextureOn(int skyTextureOn) {
    if (gSky_on != skyTextureOn) {
        ToggleSkyQuietly();
    }
    gSky_on = skyTextureOn;
}

// FUNCTION: CARMA2_HW 0x00446e80
void C2_HOOK_FASTCALL ToggleSkyQuietly(void) {
    br_pixelmap* temp;

    temp = gProgram_state.current_depth_effect.sky_texture;
    gProgram_state.current_depth_effect.sky_texture = gSwap_sky_texture;
    gSwap_sky_texture = temp;
    gProgram_state.default_depth_effect.sky_texture = gProgram_state.current_depth_effect.sky_texture;
    if (gHorizon_material != NULL && gProgram_state.default_depth_effect.sky_texture != NULL) {
        gHorizon_material->colour_map = gProgram_state.current_depth_effect.sky_texture;
        BrMaterialUpdate(gHorizon_material, BR_MATU_ALL);
        MungeSkyVs(gForward_sky_model, gHorizon_material);
    }
}

// FUNCTION: CARMA2_HW 0x00446ee0
void C2_HOOK_FASTCALL ToggleSky(void) {
    SetSkyTextureOn(!GetSkyTextureOn());
    if (gProgram_state.current_depth_effect.sky_texture != NULL) {
        NewTextHeadupSlot(4, 0, 2000, -4, GetMiscString(eMiscString_sky_texture_on));
    } else if (gSwap_sky_texture != NULL) {
        NewTextHeadupSlot(4, 0, 2000, -4, GetMiscString(eMiscString_sky_texture_off));
    } else {
        NewTextHeadupSlot(4, 0, 2000, -4, GetMiscString(eMiscString_no_sky_texture_for_this_race));
    }
}

// FUNCTION: CARMA2_HW 0x00446f90
int C2_HOOK_FASTCALL GetDepthCueingOn(void) {
    return gDepth_cueing_on;
}

// FUNCTION: CARMA2_HW 0x00446fa0
void C2_HOOK_FASTCALL SetDepthCueingOn(int pOn) {
    if (gDepth_cueing_on != pOn && gHorizon_material != NULL) {
        ToggleDepthCueingQuietly();
    }
    gDepth_cueing_on = pOn;
}

// FUNCTION: CARMA2_HW 0x00447070
void C2_HOOK_FASTCALL ToggleDepthCueingQuietly(void) {
    tDepth_effect_type temp_type;
    int temp_start;
    int temp_end;
    int temp_red;
    int temp_green;
    int temp_blue;

#pragma inline_depth(0)
    temp_type = gProgram_state.current_depth_effect.type;
    temp_start = gProgram_state.current_depth_effect.start;
    temp_end = gProgram_state.current_depth_effect.end;
    temp_red = gProgram_state.current_depth_effect.colour.red;
    temp_green = gProgram_state.current_depth_effect.colour.green;
    temp_blue = gProgram_state.current_depth_effect.colour.blue;
    InstantDepthChange(
        gSwap_depth_effect_type,
        gProgram_state.current_depth_effect.sky_texture,
        gSwap_depth_effect_start,
        gSwap_depth_effect_end,
        gSwap_depth_effect_colour_red,
        gSwap_depth_effect_colour_green,
        gSwap_depth_effect_colour_blue,
        gProgram_state.racing);
#pragma inline_depth()
    gSwap_depth_effect_type = temp_type;
    gSwap_depth_effect_start = temp_start;
    gSwap_depth_effect_end = temp_end;
    gSwap_depth_effect_colour_red = temp_red;
    gSwap_depth_effect_colour_green = temp_green;
    gSwap_depth_effect_colour_blue = temp_blue;
}

// ToggleDepthCueing

// ChangeDepthEffect

// MungeForwardSky

// MungeRearviewSky

// SkyTextureChanged

// SetSkyColour
