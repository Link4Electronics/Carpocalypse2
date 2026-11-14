#include "oil.h"

#include "finteray.h"
#include "globvars.h"
#include "globvrpb.h"
#include "graphics.h"
#include "loading.h"
#include "network.h"
#include "piping.h"
#include "utility.h"

#include <brender/brender.h>

#include "carpocalypse2_macros.h"
#include "oil.h"

// GLOBAL: CARMA2_HW 0x0065a398
const char* gOil_pixie_names[1] = {
    "OIL.PIX",
};

// GLOBAL: CARMA2_HW 0x00690c88
br_pixelmap* gOil_pixies[1];

// GLOBAL: CARMA2_HW 0x00690c90
tOil_spill_info gOily_spills[32];

// GLOBAL: CARMA2_HW 0x00691710
int gNext_oil_pixie;

// GLOBAL: CARMA2_HW 0x0074a68c
br_material* oily_material = NULL;

#ifdef CARPOCALYPSE2_MATCHING
#endif
#ifdef CARPOCALYPSE2_MATCHING
#endif
#ifdef CARPOCALYPSE2_MATCHING
extern tU32 C2_HOOK_FASTCALL GetTotalTime(void);
extern br_scalar C2_HOOK_STDCALL SRandomBetween(br_scalar pA, br_scalar pB);
extern int gBig_bang;
extern char gSFS_shortest_dist[];
extern int gStart_jumped;
#endif
#ifdef CARPOCALYPSE2_MATCHING
extern const br_scalar gFloat_0058a464;
extern void C2_HOOK_CDECL BrMatrix34PreRotateY();
extern void C2_HOOK_CDECL BrModelUpdate();
extern void C2_HOOK_FASTCALL CheckSingleFace(tFace_ref* pFace, br_vector3* ray_pos, br_vector3* ray_dir, br_vector3* normal, br_scalar* rt, br_vector3* coll_pos);
extern int C2_HOOK_FASTCALL FindFacesInBox(tBounds* bnds, tFace_ref* face_list, int max_face, tWorld_callbacks* pWorld_callbacks);
extern int C2_HOOK_FASTCALL IRandomBetween(int pA, int pB);
extern tNet_message* C2_HOOK_FASTCALL NetBuildGuaranteedMessage(tU8 pNet_message_type, int pOption);
extern int C2_HOOK_FASTCALL NetGuaranteedSendMessageToAllPlayers(tNet_game_details* pDetails, tNet_message* pMessage, void* pNotifyFail);
extern tNet_game_player_info* C2_HOOK_FASTCALL NetPlayerFromCar(tCar_spec *pCar);
extern int C2_HOOK_FASTCALL NormalSideOfPlane(br_vector3* pPoint, br_vector3* pNormal, br_scalar pD);
extern void C2_HOOK_FASTCALL PipeSingleOilSpill(int pIndex, br_matrix34* pMat, br_scalar pFull_size, br_scalar pGrow_rate, tU32 pSpill_time, tU32 pStop_time, tCar_spec* pCar, br_vector3* pOriginal_pos, br_pixelmap* pPixelmap);
extern tNet_game_details* gCurrent_net_game;
extern char gFLOAT_0058a420[];
extern tNet_mode gNet_mode;
extern int gPed_676914;
#endif
#ifdef CARPOCALYPSE2_MATCHING
extern void C2_HOOK_CDECL BrMatrix34ApplyP();
extern int C2_HOOK_FASTCALL PointInSpill(br_vector3* pV, int pSpill);
#endif
#ifdef CARPOCALYPSE2_MATCHING
extern br_scalar gMinimum_yness_before_knackerisation;
#endif
// FUNCTION: CARMA2_HW 0x004a6c50
void C2_NAKED C2_HOOK_FASTCALL ResetOilSpills(void) {
#ifndef CARPOCALYPSE2_MATCHING
    int i;

    for (i = 0; i < (int)CARPOCALYPSE2_ASIZE(gOily_spills); i++) {
        gOily_spills[i].actor->render_style = BR_RSTYLE_NONE;
        gOily_spills[i].car = NULL;
        gOily_spills[i].car_actor = NULL;
        gOily_spills[i].stop_time = 0;
    }
#else
    __asm {
            push esi
            mov eax, offset gOily_spills+0xc
            mov dl, 1
            xor ecx, ecx
    L004a6c5a:
            mov esi, dword ptr [eax - 0xc]
            add eax, 0x54
            cmp eax, offset gSFS_shortest_dist
            mov byte ptr [esi + 0x20], dl
            mov dword ptr [eax - 0x54], ecx
            mov dword ptr [eax - 0x5c], ecx
            mov dword ptr [eax - 0x4c], ecx
            jl L004a6c5a
            pop esi
            ret
            nop
            nop
            nop
            nop
            nop
            nop
            nop
            nop
            nop
            nop
            nop
    }
#endif
}

// FUNCTION: CARMA2_HW 0x004a6c80
void C2_HOOK_FASTCALL QueueOilSpill2(
    tCar_spec* pCar,
    br_actor* pActor,
    br_model* pModel,
    int pSpill_time_offset,
    float pFull_size_min,
    float pFull_size_max,
    float pGrow_size_min,
    float pGrow_size_max,
    float pSize,
    float pField_0x48,
    float pField_0x4c,
    float pField_0x40,
    float pField_0x44,
    br_material* pMaterial,
    int pField_0x50)
{
#ifndef CARPOCALYPSE2_MATCHING
    int i;
    int slot;
    int oldest;
    tU32 time_low;
    tU32 time_high;

    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tOil_spill_info, car, 0xc);
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tOil_spill_info, spill_time, 0x10);
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tOil_spill_info, current_size, 0x1c);
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tOil_spill_info, field_0x20, 0x20);
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tOil_spill_info, grow_rate, 0x24);
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tOil_spill_info, field_0x40, 0x40);
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tOil_spill_info, field_0x44, 0x44);
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tOil_spill_info, field_0x48, 0x48);
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tOil_spill_info, field_0x4c, 0x4c);
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tOil_spill_info, field_0x50, 0x50);
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(br_actor, material, 0x1c);
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(br_actor, render_style, 0x20);

    oldest = 0;
    slot = -1;
    time_low = GetTotalTime();
    time_high = GetTotalTime();

    for (i = 0; i < (int)CARPOCALYPSE2_ASIZE(gOily_spills); i++) {
        if (pCar != NULL && gOily_spills[i].car_actor == pActor && time_low < gOily_spills[i].spill_time + 5000) {
            return;
        }
    }

    for (i = 0; i < (int)CARPOCALYPSE2_ASIZE(gOily_spills); i++) {
        if (gOily_spills[i].car_actor != NULL) {
            if (gOily_spills[i].spill_time < time_high) {
                time_high = gOily_spills[i].spill_time;
                oldest = i;
            }
        } else {
            slot = i;
            break;
        }
    }

    if (slot < 0) {
        slot = oldest;
    }
    gOily_spills[slot].actor->material = pMaterial;
    gOily_spills[slot].car = pCar;
    gOily_spills[slot].car_actor = pActor;
    gOily_spills[slot].car_model = pModel;
    gOily_spills[slot].spill_time = time_low + pSpill_time_offset;
    gOily_spills[slot].field_0x20 = pSize;
    gOily_spills[slot].full_size = SRandomBetween(pFull_size_min, pFull_size_max);
    gOily_spills[slot].grow_rate = SRandomBetween(pGrow_size_min, pGrow_size_max);
    gOily_spills[slot].current_size = pSize;
    gOily_spills[slot].actor->render_style = BR_RSTYLE_NONE;
    gOily_spills[slot].field_0x40 = pField_0x40;
    gOily_spills[slot].field_0x44 = pField_0x44;
    gOily_spills[slot].field_0x48 = pField_0x48;
    gOily_spills[slot].field_0x4c = pField_0x4c;
    gOily_spills[slot].field_0x50 = pField_0x50;
#else
    __asm {
            sub esp, 8
            push ebx
            push ebp
            push esi
            mov esi, edx
            push edi
            mov dword ptr [esp + 0x10], ecx
            mov dword ptr [esp + 0x14], esi
            xor edi, edi
            or ebp, 0xffffffff
            call GetTotalTime
            mov ebx, eax
            call GetTotalTime
            mov ecx, offset gOily_spills+0x10
    L004a6ca7:
            mov edx, dword ptr [esp + 0x10]
            test edx, edx
            je L004a6cc4
            cmp dword ptr [ecx - 0xc], esi
            jne L004a6cc4
            mov edx, dword ptr [ecx]
            add edx, 0x1388
            cmp ebx, edx
            jb L004a6dbb
    L004a6cc4:
            add ecx, 0x54
            cmp ecx, offset gStart_jumped
            jl L004a6ca7
            xor esi, esi
            mov ecx, offset gOily_spills+0x10
    L004a6cd6:
            mov edx, dword ptr [ecx - 0xc]
            test edx, edx
            je L004a6cf5
            mov edx, dword ptr [ecx]
            cmp edx, eax
            jae L004a6ce7
            mov eax, edx
            mov edi, esi
    L004a6ce7:
            add ecx, 0x54
            inc esi
            cmp ecx, offset gStart_jumped
            jl L004a6cd6
            jmp L004a6cf7
    L004a6cf5:
            mov ebp, esi
    L004a6cf7:
            test ebp, ebp
            jge L004a6cfd
            mov ebp, edi
    L004a6cfd:
            mov eax, ebp
            mov ecx, dword ptr [esp + 0x48]
            shl eax, 3
            sub eax, ebp
            mov edx, dword ptr [esp + 0x10]
            lea esi, [eax + eax*2]
            shl esi, 2
            mov eax, dword ptr [esi + gOily_spills]
            mov dword ptr [eax + 0x1c], ecx
            mov eax, dword ptr [esp + 0x14]
            mov ecx, dword ptr [esp + 0x1c]
            mov dword ptr [esi + gOily_spills+0xc], edx
            mov edx, dword ptr [esp + 0x20]
            mov dword ptr [esi + gOily_spills+0x4], eax
            mov eax, dword ptr [esp + 0x34]
            mov dword ptr [esi + gOily_spills+0x8], ecx
            mov ecx, dword ptr [esp + 0x28]
            add ebx, edx
            mov edx, dword ptr [esp + 0x24]
            push ecx
            mov dword ptr [esi + gOily_spills+0x10], ebx
            push edx
            mov dword ptr [esi + gOily_spills+0x20], eax
            call SRandomBetween
            fstp dword ptr [esi + gOily_spills+0x18]
            mov eax, dword ptr [esp + 0x30]
            mov ecx, dword ptr [esp + 0x2c]
            push eax
            push ecx
            call SRandomBetween
            fstp dword ptr [esi + gOily_spills+0x24]
            mov edx, dword ptr [esp + 0x34]
            mov ecx, dword ptr [esp + 0x40]
            mov dword ptr [esi + gOily_spills+0x1c], edx
            mov eax, dword ptr [esi + gOily_spills]
            mov edx, dword ptr [esp + 0x44]
            mov byte ptr [eax + 0x20], 1
            mov eax, dword ptr [esp + 0x38]
            mov dword ptr [esi + gOily_spills+0x40], ecx
            mov ecx, dword ptr [esp + 0x3c]
            mov dword ptr [esi + gOily_spills+0x44], edx
            mov edx, dword ptr [esp + 0x4c]
            mov dword ptr [esi + gOily_spills+0x48], eax
            mov dword ptr [esi + gOily_spills+0x4c], ecx
            mov dword ptr [esi + gOily_spills+0x50], edx
    L004a6dbb:
            pop edi
            pop esi
            pop ebp
            pop ebx
            add esp, 8
            ret 0x34
    }
#endif
}

// FUNCTION: CARMA2_HW 0x004a6dd0
void C2_HOOK_FASTCALL QueueOilSpill(tCar_spec* pCar) {
    QueueOilSpill2(
        pCar,
        pCar->car_master_actor,
        pCar->car_actor->model,
        500,
        0.35f,
        0.6f,
        3.0e-5f,
        0.0001f,
        0.1f,
        1.5f,
        2.5f,
        0.01f,
        0.15f,
        oily_material,
        0);
}

// FUNCTION: CARMA2_HW 0x004a7460
void C2_HOOK_FASTCALL SetInitialOilStuff(tOil_spill_info* pOil, br_model* pModel) {
    /* Resize the spill in the ground plane without changing vertex heights. */
    pModel->vertices[0].p.v[0] = -pOil->field_0x20;
    pModel->vertices[0].p.v[2] = -pOil->field_0x20;
    pModel->vertices[1].p.v[0] = pOil->field_0x20;
    pModel->vertices[1].p.v[2] = -pOil->field_0x20;
    pModel->vertices[2].p.v[0] = pOil->field_0x20;
    pModel->vertices[2].p.v[2] = pOil->field_0x20;
    pModel->vertices[3].p.v[0] = -pOil->field_0x20;
    pModel->vertices[3].p.v[2] = pOil->field_0x20;
    pOil->actor->render_style = BR_RSTYLE_FACES;
    BrMaterialUpdate(pOil->actor->material, BR_MATU_ALL);
    BrModelUpdate(pModel, BR_MODU_ALL);
}

void C2_HOOK_FASTCALL MungeOilsHeightAeGround(tOil_spill_info* pThe_spill) {

    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tOil_spill_info, actor, 0x0);
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tOil_spill_info, position, 0x34);

    BrVector3Copy(&pThe_spill->actor->t.t.translate.t, &pThe_spill->position);
}

int C2_HOOK_FASTCALL OKToSpillOil(tOil_spill_info* pOil) {
    br_scalar distance;
    br_scalar mr_dotty;
    br_vector3 pos;
    br_vector3 v;
    br_vector3 ray_pos;
    br_vector3 ray_dir;
    br_vector3 normal;
    tCar_spec* car;
    int i;
    int face_count;
    int found_one;
    br_angle angle_to_rotate_by;
    tBounds kev_bounds;
    tFace_ref the_list[10];
    tFace_ref* face_ref;

    car = pOil->car;
    if (car != NULL && car->driver >= 7 && car->damage_units[eDamage_engine].damage_level < 99 && car->damage_units[eDamage_transmission].damage_level < 99) {
        return 0;
    }
    angle_to_rotate_by = IRandomBetween(0, 0xffff);
    BrVector3Set(&kev_bounds.original_bounds.min,
        -pOil->full_size,
        pOil->car_model->bounds.min.v[1] - 1.f / WORLD_SCALE,
        -pOil->full_size);
    BrVector3Set(&kev_bounds.original_bounds.max,
        pOil->full_size,
        pOil->car_model->bounds.max.v[1],
        pOil->full_size);
    BrMatrix34PreRotateY(&pOil->actor->t.t.mat, angle_to_rotate_by);
    kev_bounds.mat = &pOil->car_actor->t.t.mat;
    face_count = FindFacesInBox(&kev_bounds, the_list, CARPOCALYPSE2_ASIZE(the_list), NULL);
    BrVector3Set(&v, .0f, .2f, .0f);
    BrVector3Add(&ray_pos, &pOil->car_actor->t.t.translate.t, &v);
    if (ray_dir.v[1] * ray_dir.v[1] <= 2 * BR_SCALAR_EPSILON) {
        BrVector3Set(&ray_dir, 1.f, 0.f, 0.f);
    } else {
        BrVector3Set(&ray_dir, 0.f, 1.f, 0.f);
    }
    if (face_count == 0) {
        return 0;
    }
    found_one = 0;
    for (i = 0; i < face_count; i++) {
        face_ref = &the_list[i];
        if (!found_one) {
            CheckSingleFace(face_ref, &ray_pos, &ray_dir, &normal, &distance, &pos);
            if (distance < 100.f) {
                found_one = 1;
                BrVector3Copy(&pOil->actor->t.t.look_up.up, &normal);
                BrVector3Set(&v, 0.f, 0.f, 1.f);
                BrVector3Cross(&pOil->actor->t.t.look_up.look, &normal, &v);
                BrVector3Set(&v, -1.f, 0.f, 0.f);
                BrVector3Cross((br_vector3*)pOil->actor->t.t.mat.m[2], &normal, &v);
                BrVector3Copy(&pOil->position, &pos);
                BrMatrix34PreRotateY(&pOil->actor->t.t.mat, angle_to_rotate_by);
            }
        }
    }
    if (!found_one || normal.v[1] < .97f) {
        return 0;
    }
    for (i = 0; i < face_count; i++) {
        face_ref = &the_list[i];
        mr_dotty = BrVector3Dot(&face_ref->normal, &normal);
        if (mr_dotty < .98f && (mr_dotty > .8f || !NormalSideOfPlane(&pOil->actor->t.t.translate.t, &face_ref->normal, face_ref->d))) {
            return 0;
        }
    }
    return 1;
}

void C2_HOOK_FASTCALL MungeOilsHeightAboveGround(tOil_spill_info* pOil) {

    BrVector3Copy(&pOil->actor->t.t.translate.t, &pOil->position);
}

// FUNCTION: CARMA2_HW 0x004a6e50
void C2_NAKED C2_HOOK_FASTCALL ProcessOilSpills(tU32 pFrame_period) {
#ifndef CARPOCALYPSE2_MATCHING
    int i;
    tU32 time;
    br_scalar grow_amount;
    br_scalar this_size;
    br_vector3 v;
    tNet_message* message;

#ifndef CARPOCALYPSE2_MATCHING
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tOil_spill_info, position, 0x34);
    /* FIXME: unconditional assert */
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tNet_message, contents.oil_spill.player, 0x1c);
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tNet_message, contents.oil_spill.full_size, 0x20);
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tNet_message, contents.oil_spill.grow_rate, 0x24);
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tNet_message, contents.oil_spill.current_size, 0x28);
#endif

    time = GetTotalTime();
    for (i = 0; i < CARPOCALYPSE2_ASIZE(gOily_spills); i++) {
        br_model* the_model;
        tOil_spill_info* oil = &gOily_spills[i];

        if (oil->car_actor == NULL) {
            oil->actor->render_style = BR_RSTYLE_NONE;
        } else {
            the_model = oil->actor->model;
            if (oil->actor->render_style == BR_RSTYLE_NONE
                    && time >= oil->spill_time
                    && (oil->car == NULL || (fabsf(oil->car->collision_info->v.v[0]) < 1 / WORLD_SCALE / 100.f
                                          && fabsf(oil->car->collision_info->v.v[1]) < 1 / WORLD_SCALE / 100.f
                                          && fabsf(oil->car->collision_info->v.v[2]) < 1 / WORLD_SCALE / 100.f))) {
                if (gAction_replay_mode) {
                    SetInitialOilStuff(oil, the_model);
                } else {
                    if (!OKToSpillOil(oil)) {
                        oil->car = NULL;
                        oil->car_actor = NULL;
                    } else {
                        oil->spill_time = time;
                        if (gNext_oil_pixie > 0) {
 /* CARPOCALYPSE2_ASIZE(gOil_pixies) */
                            gNext_oil_pixie = 0;
                        }
                        if (oil->car == NULL) {
                            BrVector3Copy(&oil->original_pos, &oil->car_actor->t.t.translate.t);
                        } else {
                            BrVector3Copy(&oil->original_pos, &oil->car->pos);
                        }
                        PipeSingleOilSpill(i,
                            &oil->actor->t.t.mat,
                            oil->full_size,
                            oil->grow_rate,
                            oil->spill_time,
                            oil->stop_time,
                            oil->car,
                            &oil->original_pos,
                            oil->actor->material->colour_map);
                        oil->stop_time = 0;
                        SetInitialOilStuff(oil, the_model);
                        if (gNet_mode == eNet_mode_host && oil->car != NULL) {
                            message = NetBuildGuaranteedMessage(31, 0);
                            message->contents.oil_spill.player = NetPlayerFromCar(oil->car)->ID;
                            message->contents.oil_spill.full_size = oil->full_size;
                            message->contents.oil_spill.grow_rate = oil->grow_rate;
                            message->contents.oil_spill.current_size = oil->current_size;
                            NetGuaranteedSendMessageToAllPlayers(gCurrent_net_game, message, NULL);
                        }
                    }
                }
            } else {
                if (oil->actor->render_style == BR_RSTYLE_FACES
                        && (oil->stop_time == 0 || time < oil->stop_time)) {

                    int grow = 1;
                    if (oil->car != NULL) {
                        BrVector3Sub(&v, &oil->original_pos, &oil->car->pos);
                        grow_amount = BrVector3LengthSquared(&v);
                        grow = oil->stop_time == 0 || grow_amount <= 0.2f;
                    }
                    if (grow) {
                        this_size = oil->field_0x20 + (time - oil->spill_time) * oil->grow_rate;
                        if (this_size >= oil->field_0x20) {
                            oil->actor->render_style = BR_RSTYLE_FACES;
                            if (this_size <= oil->full_size) {
                                the_model->vertices[0].p.v[0] = -this_size;
                                the_model->vertices[0].p.v[2] = -this_size;
                                the_model->vertices[1].p.v[0] =  this_size;
                                the_model->vertices[1].p.v[2] = -this_size;
                                the_model->vertices[2].p.v[0] =  this_size;
                                the_model->vertices[2].p.v[2] =  this_size;
                                the_model->vertices[3].p.v[0] = -this_size;
                                the_model->vertices[3].p.v[2] =  this_size;
                                oil->current_size = this_size;
                            } else {
                                the_model->vertices[0].p.v[0] = -oil->full_size;
                                the_model->vertices[0].p.v[2] = -oil->full_size;
                                the_model->vertices[1].p.v[0] =  oil->full_size;
                                the_model->vertices[1].p.v[2] = -oil->full_size;
                                the_model->vertices[2].p.v[0] =  oil->full_size;
                                the_model->vertices[2].p.v[2] =  oil->full_size;
                                the_model->vertices[3].p.v[0] = -oil->full_size;
                                the_model->vertices[3].p.v[2] =  oil->full_size;
                                oil->current_size = oil->full_size;
                            }
                            BrModelUpdate(the_model, BR_MODU_ALL);
                        } else {
                            oil->actor->render_style = BR_RSTYLE_NONE;
                        }
                    } else {
                        oil->stop_time = time;
                        continue;
                    }
                }
            }
        }
        if (oil->actor->render_style == BR_RSTYLE_FACES) {
            MungeOilsHeightAboveGround(oil);
        }
    }
#else
    __asm {
            sub esp, 0x3b4
            push ebx
            push ebp
            push esi
            push edi
            call GetTotalTime
            mov dword ptr [esp + 0x10], eax
            mov dword ptr [esp + 0x14], 0
            mov esi, offset gOily_spills+0xc
    L004a6e70:
            mov eax, dword ptr [esi - 8]
            lea edi, [esi - 0xc]
            test eax, eax
            je L004a7410
            mov ecx, dword ptr [edi]
            mov bl, byte ptr [ecx + 0x20]
            mov edx, dword ptr [ecx + 0x18]
            cmp bl, 1
            mov dword ptr [esp + 0x24], edx
            jne L004a729c
            mov eax, dword ptr [esp + 0x10]
            mov ebp, dword ptr [esi + 4]
            cmp eax, ebp
            jb L004a729c
            mov eax, dword ptr [esi]
            test eax, eax
            je L004a6eea
            mov ebp, dword ptr [eax + 8]
            fld dword ptr [ebp + 0x68]
            fabs
            fcomp dword ptr [gFLOAT_0058a420+0x10]
            fnstsw ax
            test ah, bl
            je L004a729c
            fld dword ptr [ebp + 0x6c]
            fabs
            fcomp dword ptr [gFLOAT_0058a420+0x10]
            fnstsw ax
            test ah, bl
            je L004a729c
            fld dword ptr [ebp + 0x70]
            fabs
            fcomp dword ptr [gFLOAT_0058a420+0x10]
            fnstsw ax
            test ah, bl
            je L004a729c
    L004a6eea:
            mov eax, dword ptr [gPed_676914]
            test eax, eax
            je L004a6eff
            mov ecx, edi
            call SetInitialOilStuff
            jmp L004a7416
    L004a6eff:
            mov eax, dword ptr [esi]
            test eax, eax
            je L004a6f26
            cmp dword ptr [eax + 0xc], 7
            jl L004a6f26
            mov edx, dword ptr [eax + 0x4e0]
            mov ecx, 0x63
            cmp edx, ecx
            jge L004a6f26
            cmp dword ptr [eax + 0x50c], ecx
            jl L004a728a
    L004a6f26:
            mov edx, 0xffff
            xor ecx, ecx
            call IRandomBetween
            fld dword ptr [esi + 0xc]
            mov ecx, dword ptr [esi - 4]
            mov edx, dword ptr [edi]
            fchs
            fst dword ptr [esp + 0x64]
            fld dword ptr [ecx + 0x38]
            fsub qword ptr [gFLOAT_0058a420+0x18]
            fld dword ptr [esi + 0xc]
            fld dword ptr [esi + 0xc]
            fxch st(2)
            fstp dword ptr [esp + 0x68]
            fxch st(2)
            fstp dword ptr [esp + 0x6c]
            fxch st(1)
            fstp dword ptr [esp + 0x70]
            fld dword ptr [ecx + 0x44]
            mov ebp, eax
            add edx, 0x2c
            fstp dword ptr [esp + 0x74]
            push ebp
            push edx
            fstp dword ptr [esp + 0x80]
            call BrMatrix34PreRotateY
            mov eax, dword ptr [esi - 8]
            add esp, 8
            add eax, 0x2c
            lea edx, [esp + 0xa4]
            push 0
            push 0xa
            lea ecx, [esp + 0x68]
            mov dword ptr [esp + 0x68], eax
            call FindFacesInBox
            mov ecx, dword ptr [esi - 8]
            mov edx, eax
            fld dword ptr [esp + 0x68]
            mov eax, dword ptr [ecx + 0x50]
            mov dword ptr [esp + 0x40], edx
            mov dword ptr [esp + 0x54], eax
            fld dword ptr [ecx + 0x54]
            fsub dword ptr [gFLOAT_0058a420+0x20]
            fxch st(1)
            fsub dword ptr [esp + 0x74]
            fxch st(1)
            fstp dword ptr [esp + 0x58]
            mov ecx, dword ptr [ecx + 0x58]
            fst dword ptr [esp + 0x2c]
            fmul dword ptr [esp + 0x2c]
            mov dword ptr [esp + 0x5c], ecx
            fsqrt
            fcom dword ptr [gFLOAT_0058a420+0x24]
            fnstsw ax
            test ah, 0x41
            jne L004a700d
            fdivr qword ptr [gFLOAT_0058a420+0x28]
            mov dword ptr [esp + 0x38], 0
            mov eax, dword ptr [esp + 0x38]
            mov ecx, eax
            mov dword ptr [esp + 0x28], eax
            mov dword ptr [esp + 0x30], ecx
            fld st(0)
            fstp st(1)
            fmul dword ptr [esp + 0x2c]
            fstp dword ptr [esp + 0x2c]
            jmp L004a7027
    L004a700d:
            fstp st(0)
            mov dword ptr [esp + 0x28], 0x3f800000
            mov dword ptr [esp + 0x2c], 0
            mov dword ptr [esp + 0x30], 0
    L004a7027:
            test edx, edx
            je L004a728a
            xor ebx, ebx
            lea ecx, [esp + 0xa4]
            test edx, edx
            mov dword ptr [esp + 0x3c], ecx
            jle L004a7114
            mov dword ptr [esp + 0x34], edx
    L004a7048:
            test ebx, ebx
            jne L004a70f6
            lea edx, [esp + 0x48]
            lea eax, [esp + 0x44]
            push edx
            push eax
            lea edx, [esp + 0x20]
            lea eax, [esp + 0x30]
            push edx
            push eax
            lea edx, [esp + 0x64]
            call CheckSingleFace
            fld dword ptr [esp + 0x44]
            fcomp dword ptr [gFLOAT_0058a420+0x38]
            fnstsw ax
            test ah, 1
            je L004a70f6
            mov ecx, dword ptr [edi]
            mov edx, dword ptr [esp + 0x18]
            push ebp
            mov ebx, 1
            mov dword ptr [ecx + 0x38], edx
            mov eax, dword ptr [edi]
            mov ecx, dword ptr [esp + 0x20]
            mov dword ptr [eax + 0x3c], ecx
            mov edx, dword ptr [edi]
            mov eax, dword ptr [esp + 0x24]
            mov dword ptr [edx + 0x40], eax
            mov ecx, dword ptr [edi]
            fld dword ptr [esp + 0x20]
            fstp dword ptr [ecx + 0x2c]
            fld dword ptr [esp + 0x1c]
            mov edx, dword ptr [edi]
            xor eax, eax
            fchs
            fstp dword ptr [edx + 0x30]
            mov ecx, dword ptr [edi]
            mov dword ptr [ecx + 0x34], eax
            mov edx, dword ptr [edi]
            mov dword ptr [edx + 0x44], eax
            mov eax, dword ptr [edi]
            fld dword ptr [esp + 0x24]
            fchs
            fstp dword ptr [eax + 0x48]
            fld dword ptr [esp + 0x20]
            mov ecx, dword ptr [edi]
            fstp dword ptr [ecx + 0x4c]
            mov edx, dword ptr [esp + 0x4c]
            mov eax, dword ptr [esp + 0x50]
            mov ecx, dword ptr [esp + 0x54]
            mov dword ptr [edi + 0x34], edx
            mov edx, dword ptr [edi]
            mov dword ptr [edi + 0x38], eax
            add edx, 0x2c
            mov dword ptr [edi + 0x3c], ecx
            push edx
            call BrMatrix34PreRotateY
            add esp, 8
    L004a70f6:
            mov ecx, dword ptr [esp + 0x3c]
            mov eax, dword ptr [esp + 0x34]
            add ecx, 0x50
            dec eax
            mov dword ptr [esp + 0x3c], ecx
            mov dword ptr [esp + 0x34], eax
            jne L004a7048
            mov edx, dword ptr [esp + 0x40]
    L004a7114:
            test ebx, ebx
            je L004a728a
            fld dword ptr [esp + 0x1c]
            fcomp dword ptr [gFLOAT_0058a420+0x3c]
            fnstsw ax
            test ah, 1
            jne L004a728a
            xor ebx, ebx
            test edx, edx
            jle L004a71a8
            lea ebp, [esp + 0xdc]
    L004a713e:
            _emit 0xd9
        _emit 0x45
        _emit 0x00
            fld dword ptr [ebp - 4]
            fld dword ptr [ebp + 4]
            fxch st(2)
            fmul dword ptr [esp + 0x1c]
            fxch st(1)
            fmul dword ptr [esp + 0x18]
            fxch st(2)
            fmul dword ptr [esp + 0x20]
            fxch st(2)
            faddp st(1), st(0)
            fxch st(1)
            faddp st(1), st(0)
            lea edx, [ebp - 4]
            fcom dword ptr [gFLOAT_0058a420+0x40]
            fnstsw ax
            test ah, 1
            je L004a719a
            fcomp dword ptr [gFloat_0058a464]
            fnstsw ax
            test ah, 0x41
            je L004a728a
            mov eax, dword ptr [ebp + 0xc]
            mov ecx, dword ptr [edi]
            push eax
            add ecx, 0x50
            call NormalSideOfPlane
            test eax, eax
            je L004a728a
            jmp L004a719c
    L004a719a:
            fstp st(0)
    L004a719c:
            mov eax, dword ptr [esp + 0x40]
            inc ebx
            add ebp, 0x50
            cmp ebx, eax
            jl L004a713e
    L004a71a8:
            mov eax, dword ptr [gNext_oil_pixie]
            mov ecx, dword ptr [esp + 0x10]
            cmp eax, 1
            mov dword ptr [esi + 4], ecx
            jl L004a71c3
            mov dword ptr [gNext_oil_pixie], 0
    L004a71c3:
            mov eax, dword ptr [esi]
            lea ecx, [esi + 0x1c]
            test eax, eax
            je L004a71e3
            add eax, 0x4c
            mov edx, ecx
            mov ebx, dword ptr [eax]
            mov dword ptr [edx], ebx
            mov ebx, dword ptr [eax + 4]
            mov dword ptr [edx + 4], ebx
            mov eax, dword ptr [eax + 8]
            mov dword ptr [edx + 8], eax
            jmp L004a71fb
    L004a71e3:
            mov edx, dword ptr [esi - 8]
            mov eax, ecx
            add edx, 0x50
            mov ebx, dword ptr [edx]
            mov dword ptr [eax], ebx
            mov ebx, dword ptr [edx + 4]
            mov dword ptr [eax + 4], ebx
            mov edx, dword ptr [edx + 8]
            mov dword ptr [eax + 8], edx
    L004a71fb:
            mov eax, dword ptr [edi]
            mov edx, dword ptr [eax + 0x1c]
            mov edx, dword ptr [edx + 0x40]
            push edx
            mov edx, dword ptr [esi + 8]
            push ecx
            mov ecx, dword ptr [esi]
            push ecx
            mov ecx, dword ptr [esi + 4]
            push edx
            mov edx, dword ptr [esi + 0x18]
            push ecx
            mov ecx, dword ptr [esi + 0xc]
            push edx
            push ecx
            mov ecx, dword ptr [esp + 0x30]
            lea edx, [eax + 0x2c]
            call PipeSingleOilSpill
            mov ecx, edi
            mov dword ptr [esi + 8], 0
            mov edx, dword ptr [esp + 0x24]
            call SetInitialOilStuff
            cmp dword ptr [gNet_mode], 2
            jne L004a7416
            cmp dword ptr [esi], 0
            je L004a7416
            xor edx, edx
            mov cl, 0x1f
            call NetBuildGuaranteedMessage
            mov ecx, dword ptr [esi]
            mov ebp, eax
            call NetPlayerFromCar
            mov edx, dword ptr [eax + 0x24]
            mov dword ptr [ebp + 0x1c], edx
            mov eax, dword ptr [esi + 0xc]
            mov dword ptr [ebp + 0x20], eax
            mov ecx, dword ptr [esi + 0x18]
            mov dword ptr [ebp + 0x24], ecx
            mov edx, dword ptr [esi + 0x10]
            mov dword ptr [ebp + 0x28], edx
            mov ecx, dword ptr [gCurrent_net_game]
            push 0
            mov edx, ebp
            call NetGuaranteedSendMessageToAllPlayers
            jmp L004a7416
    L004a728a:
            mov dword ptr [esi], 0
            mov dword ptr [esi - 8], 0
            jmp L004a7416
    L004a729c:
            cmp bl, 4
            jne L004a7416
            mov ebx, dword ptr [esi + 8]
            test ebx, ebx
            je L004a72b6
            cmp dword ptr [esp + 0x10], ebx
            jae L004a7416
    L004a72b6:
            mov eax, dword ptr [esi]
            test eax, eax
            je L004a731d
            fld dword ptr [esi + 0x1c]
            fld dword ptr [esi + 0x20]
            fld dword ptr [esi + 0x24]
            fxch st(2)
            fsub dword ptr [eax + 0x4c]
            fxch st(1)
            fsub dword ptr [eax + 0x50]
            fxch st(2)
            fsub dword ptr [eax + 0x54]
            fxch st(1)
            fxch st(2)
            fxch st(1)
            test ebx, ebx
            jne L004a7317
            fld st(2)
            fld st(2)
            fxch st(1)
            fmul st(0), st(4)
            fld st(2)
            fxch st(2)
            fmul st(0), st(4)
            fxch st(2)
            fmul st(0), st(3)
            fxch st(2)
            faddp st(1), st(0)
            fxch st(1)
            faddp st(1), st(0)
            fcomp dword ptr [gFloat_0058a464+0x4]
            fstp st(0)
            fnstsw ax
            fstp st(0)
            test ah, 0x41
            fstp st(0)
            jne L004a731d
            mov eax, dword ptr [esp + 0x10]
            mov dword ptr [esi + 8], eax
            jmp L004a7436
    L004a7317:
            fstp st(0)
            fstp st(0)
            fstp st(0)
    L004a731d:
            mov eax, dword ptr [esp + 0x10]
            mov ebx, dword ptr [esi + 4]
            sub eax, ebx
            mov dword ptr [esp + 0x24], eax
            fild dword ptr [esp + 0x24]
            fmul dword ptr [esi + 0x18]
            fadd dword ptr [esi + 0x14]
            fcom dword ptr [esi + 0x14]
            fnstsw ax
            test ah, 1
            je L004a7349
            fstp st(0)
            mov byte ptr [ecx + 0x20], 1
            jmp L004a7416
    L004a7349:
            mov byte ptr [ecx + 0x20], 4
            fcom dword ptr [esi + 0xc]
            fnstsw ax
            test ah, 0x41
            jne L004a73c1
            mov ecx, dword ptr [edx + 8]
            push 0x7fff
            fstp st(0)
            fld dword ptr [esi + 0xc]
            fchs
            _emit 0xd9
        _emit 0x19
            fld dword ptr [esi + 0xc]
            mov eax, dword ptr [edx + 8]
            push edx
            fchs
            fstp dword ptr [eax + 8]
            mov ecx, dword ptr [edx + 8]
            mov eax, dword ptr [esi + 0xc]
            mov dword ptr [ecx + 0x28], eax
            mov ecx, dword ptr [edx + 8]
            fld dword ptr [esi + 0xc]
            fchs
            fstp dword ptr [ecx + 0x30]
            mov eax, dword ptr [edx + 8]
            mov ecx, dword ptr [esi + 0xc]
            mov dword ptr [eax + 0x50], ecx
            mov eax, dword ptr [edx + 8]
            mov ecx, dword ptr [esi + 0xc]
            mov dword ptr [eax + 0x58], ecx
            mov eax, dword ptr [edx + 8]
            fld dword ptr [esi + 0xc]
            fchs
            fstp dword ptr [eax + 0x78]
            mov ecx, dword ptr [edx + 8]
            mov eax, dword ptr [esi + 0xc]
            mov dword ptr [ecx + 0x80], eax
            mov ecx, dword ptr [esi + 0xc]
            mov dword ptr [esi + 0x10], ecx
            call BrModelUpdate
            add esp, 8
            jmp L004a7416
    L004a73c1:
            mov eax, dword ptr [edx + 8]
            push 0x7fff
            fld st(0)
            fchs
            _emit 0xd9
        _emit 0x10
            mov ecx, dword ptr [edx + 8]
            push edx
            fst dword ptr [ecx + 8]
            mov eax, dword ptr [edx + 8]
            fld st(1)
            fstp dword ptr [eax + 0x28]
            mov ecx, dword ptr [edx + 8]
            fst dword ptr [ecx + 0x30]
            mov eax, dword ptr [edx + 8]
            fld st(1)
            fstp dword ptr [eax + 0x50]
            mov ecx, dword ptr [edx + 8]
            fld st(1)
            fstp dword ptr [ecx + 0x58]
            mov eax, dword ptr [edx + 8]
            fstp dword ptr [eax + 0x78]
            mov ecx, dword ptr [edx + 8]
            fst dword ptr [ecx + 0x80]
            fstp dword ptr [esi + 0x10]
            call BrModelUpdate
            add esp, 8
            jmp L004a7416
    L004a7410:
            mov edx, dword ptr [edi]
            mov byte ptr [edx + 0x20], 1
    L004a7416:
            mov eax, dword ptr [edi]
            cmp byte ptr [eax + 0x20], 4
            jne L004a7436
            lea ecx, [esi + 0x28]
            mov edx, eax
            add edx, 0x50
            mov eax, dword ptr [ecx]
            mov dword ptr [edx], eax
            mov eax, dword ptr [ecx + 4]
            mov dword ptr [edx + 4], eax
            mov ecx, dword ptr [ecx + 8]
            mov dword ptr [edx + 8], ecx
    L004a7436:
            mov edx, dword ptr [esp + 0x14]
            add esi, 0x54
            inc edx
            cmp esi, offset gSFS_shortest_dist
            mov dword ptr [esp + 0x14], edx
            jl L004a6e70
            pop edi
            pop esi
            pop ebp
            pop ebx
            add esp, 0x3b4
            ret
    }
#endif
}

// FUNCTION: CARMA2_HW 0x004a74e0
int C2_HOOK_FASTCALL GetOilSpillCount(void) {
    return CARPOCALYPSE2_ASIZE(gOily_spills);
}

// FUNCTION: CARMA2_HW 0x004a74f0
void C2_HOOK_FASTCALL GetOilSpillDetails(int pIndex, br_actor** pActor, br_scalar* pSize) {
    if (gOily_spills[pIndex].car_actor != NULL) {
        *pActor = gOily_spills[pIndex].actor;
        *pSize = gOily_spills[pIndex].full_size;
    } else {
        *pActor = NULL;
    }
}

#pragma inline_depth(0)
// FUNCTION: CARMA2_HW 0x004a7530
void C2_NAKED C2_HOOK_FASTCALL GetOilFrictionFactors(tCar_spec* pCar, br_scalar* pFl, br_scalar* pFr, br_scalar* pRl, br_scalar* pRr) {
#ifndef CARPOCALYPSE2_MATCHING
    int i;
    br_vector3 wheel_world;

    *pFl = 1.f;
    *pFr = 1.f;
    *pRl = 1.f;
    *pRr = 1.f;

    if (pCar != NULL && pCar->driver > 5) {
        if (pCar->shadow_intersection_flags != 0) {
            for (i = 0; i < (int)CARPOCALYPSE2_ASIZE(gOily_spills); i++) {
                float oil_min = gOily_spills[i].field_0x48;
                float oil_max = gOily_spills[i].field_0x4c;
                if (oil_max != 0.0f && ((1 << i) & pCar->shadow_intersection_flags)) {
                    float friction_min = gOily_spills[i].field_0x40;
                    float friction_max = gOily_spills[i].field_0x44;
                    BrMatrix34ApplyP(&wheel_world, &pCar->wpos[2], &pCar->car_master_actor->t.t.mat);
                    if (PointInSpill(&wheel_world, i)) {
                        pCar->oil_remaining[2] = SRandomBetween(oil_min, oil_max);
                        pCar->maybe_blood_remaining[2] = SRandomBetween(friction_min, friction_max);
                        pCar->field_0x16fc[2] = (br_material*)gOily_spills[i].field_0x50;
                    }
                    BrMatrix34ApplyP(&wheel_world, &pCar->wpos[3], &pCar->car_master_actor->t.t.mat);
                    if (PointInSpill(&wheel_world, i)) {
                        pCar->oil_remaining[3] = SRandomBetween(oil_min, oil_max);
                        pCar->maybe_blood_remaining[3] = SRandomBetween(friction_min, friction_max);
                        pCar->field_0x16fc[3] = (br_material*)gOily_spills[i].field_0x50;
                    }
                    BrMatrix34ApplyP(&wheel_world, &pCar->wpos[0], &pCar->car_master_actor->t.t.mat);
                    if (PointInSpill(&wheel_world, i)) {
                        pCar->oil_remaining[0] = SRandomBetween(oil_min, oil_max);
                        pCar->maybe_blood_remaining[0] = SRandomBetween(friction_min, friction_max);
                        pCar->field_0x16fc[0] = (br_material*)gOily_spills[i].field_0x50;
                    }
                    BrMatrix34ApplyP(&wheel_world, &pCar->wpos[1], &pCar->car_master_actor->t.t.mat);
                    if (PointInSpill(&wheel_world, i)) {
                        pCar->oil_remaining[1] = SRandomBetween(oil_min, oil_max);
                        pCar->maybe_blood_remaining[1] = SRandomBetween(friction_min, friction_max);
                        pCar->field_0x16fc[1] = (br_material*)gOily_spills[i].field_0x50;
                    }
                }
            }
        }
        if (pCar->oil_remaining[2] != 0.0f) {
            *pFl = pCar->maybe_blood_remaining[2];
        }
        if (pCar->oil_remaining[3] != 0.0f) {
            *pFr = pCar->maybe_blood_remaining[3];
        }
        if (pCar->oil_remaining[0] != 0.0f) {
            *pRl = pCar->maybe_blood_remaining[0];
        }
        if (pCar->oil_remaining[1] != 0.0f) {
            *pRr = pCar->maybe_blood_remaining[1];
        }
    }
#else
    __asm {
            sub esp, 0x24
            mov eax, dword ptr [esp + 0x28]
            push ebx
            push ebp
            push esi
            mov esi, ecx
            mov dword ptr [edx], 0x3f800000
            mov ecx, dword ptr [esp + 0x38]
            mov dword ptr [eax], 0x3f800000
            mov eax, dword ptr [esp + 0x3c]
            push edi
            mov dword ptr [ecx], 0x3f800000
            mov dword ptr [esp + 0x24], edx
            test esi, esi
            mov dword ptr [eax], 0x3f800000
            je L004a77c2
            cmp dword ptr [esi + 0xc], 5
            jle L004a77c2
            mov eax, dword ptr [esi + 0x1594]
            test eax, eax
            je L004a774a
            xor ebx, ebx
            mov edi, offset gOily_spills+0x50
            mov dword ptr [esp + 0x10], ebx
    L004a758c:
            fld dword ptr [edi - 4]
            mov ecx, dword ptr [edi - 8]
            fst dword ptr [esp + 0x20]
            fcomp dword ptr [gFLOAT_0058a420+0x30]
            mov dword ptr [esp + 0x1c], ecx
            fnstsw ax
            test ah, 0x40
            jne L004a772e
            mov eax, dword ptr [esi + 0x1594]
            mov edx, 1
            mov ecx, ebx
            shl edx, cl
            test edx, eax
            je L004a772e
            mov eax, dword ptr [edi - 0x10]
            mov edx, dword ptr [esi + 0x10]
            mov ecx, dword ptr [edi - 0xc]
            mov dword ptr [esp + 0x18], eax
            add edx, 0x2c
            mov dword ptr [esp + 0x14], ecx
            lea eax, [esi + 0x123c]
            push edx
            lea ecx, [esp + 0x2c]
            push eax
            push ecx
            call BrMatrix34ApplyP
            add esp, 0xc
            mov edx, ebx
            lea ecx, [esp + 0x28]
            call PointInSpill
            mov ebx, dword ptr [esp + 0x1c]
            mov ebp, dword ptr [esp + 0x20]
            test eax, eax
            je L004a762c
            push ebp
            push ebx
            call SRandomBetween
            fstp dword ptr [esi + 0x16d4]
            mov edx, dword ptr [esp + 0x14]
            mov eax, dword ptr [esp + 0x18]
            push edx
            push eax
            call SRandomBetween
            fstp dword ptr [esi + 0x16e4]
            mov ecx, dword ptr [edi]
            mov dword ptr [esi + 0x1704], ecx
    L004a762c:
            mov edx, dword ptr [esi + 0x10]
            lea eax, [esi + 0x1248]
            add edx, 0x2c
            lea ecx, [esp + 0x28]
            push edx
            push eax
            push ecx
            call BrMatrix34ApplyP
            mov edx, dword ptr [esp + 0x1c]
            add esp, 0xc
            lea ecx, [esp + 0x28]
            call PointInSpill
            test eax, eax
            je L004a7682
            push ebp
            push ebx
            call SRandomBetween
            fstp dword ptr [esi + 0x16d8]
            mov edx, dword ptr [esp + 0x14]
            mov eax, dword ptr [esp + 0x18]
            push edx
            push eax
            call SRandomBetween
            fstp dword ptr [esi + 0x16e8]
            mov ecx, dword ptr [edi]
            mov dword ptr [esi + 0x1708], ecx
    L004a7682:
            mov edx, dword ptr [esi + 0x10]
            lea eax, [esi + 0x1224]
            add edx, 0x2c
            lea ecx, [esp + 0x28]
            push edx
            push eax
            push ecx
            call BrMatrix34ApplyP
            mov edx, dword ptr [esp + 0x1c]
            add esp, 0xc
            lea ecx, [esp + 0x28]
            call PointInSpill
            test eax, eax
            je L004a76d8
            push ebp
            push ebx
            call SRandomBetween
            fstp dword ptr [esi + 0x16cc]
            mov edx, dword ptr [esp + 0x14]
            mov eax, dword ptr [esp + 0x18]
            push edx
            push eax
            call SRandomBetween
            fstp dword ptr [esi + 0x16dc]
            mov ecx, dword ptr [edi]
            mov dword ptr [esi + 0x16fc], ecx
    L004a76d8:
            mov edx, dword ptr [esi + 0x10]
            lea eax, [esi + 0x1230]
            add edx, 0x2c
            lea ecx, [esp + 0x28]
            push edx
            push eax
            push ecx
            call BrMatrix34ApplyP
            mov edx, dword ptr [esp + 0x1c]
            add esp, 0xc
            lea ecx, [esp + 0x28]
            call PointInSpill
            test eax, eax
            je L004a772e
            push ebp
            push ebx
            call SRandomBetween
            fstp dword ptr [esi + 0x16d0]
            mov edx, dword ptr [esp + 0x14]
            mov eax, dword ptr [esp + 0x18]
            push edx
            push eax
            call SRandomBetween
            fstp dword ptr [esi + 0x16e0]
            mov ecx, dword ptr [edi]
            mov dword ptr [esi + 0x1700], ecx
    L004a772e:
            mov ebx, dword ptr [esp + 0x10]
            add edi, 0x54
            inc ebx
            cmp edi, offset gMinimum_yness_before_knackerisation
            mov dword ptr [esp + 0x10], ebx
            jl L004a758c
            mov edx, dword ptr [esp + 0x24]
    L004a774a:
            fld dword ptr [esi + 0x16d4]
            fcomp dword ptr [gFLOAT_0058a420+0x30]
            fnstsw ax
            test ah, 0x40
            jne L004a7765
            mov eax, dword ptr [esi + 0x16e4]
            mov dword ptr [edx], eax
    L004a7765:
            fld dword ptr [esi + 0x16d8]
            fcomp dword ptr [gFLOAT_0058a420+0x30]
            fnstsw ax
            test ah, 0x40
            jne L004a7784
            mov edx, dword ptr [esp + 0x38]
            mov ecx, dword ptr [esi + 0x16e8]
            mov dword ptr [edx], ecx
    L004a7784:
            fld dword ptr [esi + 0x16cc]
            fcomp dword ptr [gFLOAT_0058a420+0x30]
            fnstsw ax
            test ah, 0x40
            jne L004a77a3
            mov ecx, dword ptr [esp + 0x3c]
            mov eax, dword ptr [esi + 0x16dc]
            mov dword ptr [ecx], eax
    L004a77a3:
            fld dword ptr [esi + 0x16d0]
            fcomp dword ptr [gFLOAT_0058a420+0x30]
            fnstsw ax
            test ah, 0x40
            jne L004a77c2
            mov eax, dword ptr [esp + 0x40]
            mov edx, dword ptr [esi + 0x16e0]
            mov dword ptr [eax], edx
    L004a77c2:
            pop edi
            pop esi
            pop ebp
            pop ebx
            add esp, 0x24
            ret 0xc
    }
#endif
}
#pragma inline_depth()

// GLOBAL: CARMA2_HW 0x0058a464
const br_scalar gFloat_0058a464 = 0.8f;

// GLOBAL: CARMA2_HW 0x0058a46c
const br_scalar gFloat_0058a46c = 0.14492753148078918f;

// GLOBAL: CARMA2_HW 0x0058a470
const br_scalar gFloat_0058a470 = 0.1f;

// FUNCTION: CARMA2_HW 0x004a77d0
int C2_NAKED C2_HOOK_FASTCALL PointInSpill(br_vector3* pPoint, int pIndex) {
#ifndef CARPOCALYPSE2_MATCHING
    tOil_spill_info* oil;
    br_actor* actor;
    br_scalar range_squared;
    br_scalar delta;

    oil = &gOily_spills[pIndex];
    actor = oil->actor;
    delta = pPoint->v[0] * gFloat_0058a46c - actor->t.t.mat.m[3][0];
    range_squared = oil->current_size * oil->current_size;
    range_squared = range_squared * gFloat_0058a464;
    if (delta * delta >= range_squared) {
        return 0;
    }
    delta = pPoint->v[2] * gFloat_0058a46c - actor->t.t.mat.m[3][2];
    if (delta * delta >= range_squared) {
        return 0;
    }
    delta = pPoint->v[1] * gFloat_0058a46c - actor->t.t.mat.m[3][1];
    if (fabsf(delta) >= gFloat_0058a470) {
        return 0;
    }
    return 1;
#else
    __asm {
            push ecx
            mov eax, edx
            _emit 0xd9
        _emit 0x01
            shl eax, 3
            fmul dword ptr [gFloat_0058a46c]
            sub eax, edx
            lea eax, [eax + eax*2]
            shl eax, 2
            fld dword ptr [eax + gOily_spills+0x1c]
            fxch st(1)
            fld st(1)
            mov edx, dword ptr [eax + gOily_spills]
            fmulp st(2), st(0)
            fsub dword ptr [edx + 0x50]
            fxch st(1)
            fmul dword ptr [gFloat_0058a464]
            fxch st(1)
            _emit 0xd9
        _emit 0x54
        _emit 0x24
        _emit 0x00
            _emit 0xd8
        _emit 0x4c
        _emit 0x24
        _emit 0x00
            fcomp st(1)
            fnstsw ax
            test ah, 1
            je L004a7855
            fld dword ptr [ecx + 8]
            fmul dword ptr [gFloat_0058a46c]
            fsub dword ptr [edx + 0x58]
            fld st(0)
            fmul st(0), st(1)
            fcomp st(2)
            fnstsw ax
            fstp st(0)
            test ah, 1
            fstp st(0)
            je L004a7857
            fld dword ptr [ecx + 4]
            fmul dword ptr [gFloat_0058a46c]
            fsub dword ptr [edx + 0x54]
            fabs
            fcomp dword ptr [gFloat_0058a470]
            fnstsw ax
            test ah, 1
            je L004a7857
            mov eax, 1
            pop ecx
            ret
    L004a7855:
            fstp st(0)
    L004a7857:
            xor eax, eax
            pop ecx
            ret
    }
#endif
}

// FUNCTION: CARMA2_HW 0x004a6e20
void C2_NAKED C2_HOOK_FASTCALL MungeIndexedOilsHeightAboveGround(int pIndex) {
#ifndef CARPOCALYPSE2_MATCHING
    MungeOilsHeightAboveGround(&gOily_spills[pIndex]);
#else
    __asm {
            mov eax, ecx
            shl eax, 3
            sub eax, ecx
            lea eax, [eax + eax*2]
            mov edx, dword ptr [eax*4 + gOily_spills]
            lea eax, [eax*4 + gOily_spills]
            add edx, 0x50
            lea ecx, [eax + 0x34]
            mov eax, dword ptr [eax + 0x34]
            mov dword ptr [edx], eax
            mov eax, dword ptr [ecx + 4]
            mov dword ptr [edx + 4], eax
            mov ecx, dword ptr [ecx + 8]
            mov dword ptr [edx + 8], ecx
            ret
    }
#endif
}
// FUNCTION: CARMA2_HW 0x004a6a10
void C2_HOOK_FASTCALL InitOilSpills(void) {
    int i;
    br_model* the_model;

    C2_HOOK_BUG_ON(sizeof(tOil_spill_info) != 0x54);

    for (i = 0; i < (int)CARPOCALYPSE2_ASIZE(gOil_pixie_names); i++) {
        gOil_pixies[i] = LoadPixelmap(gOil_pixie_names[i]);
        BrMapAdd(gOil_pixies[i]);
    }

    for (i = 0; i < (int)CARPOCALYPSE2_ASIZE(gOily_spills); i++) {
        oily_material = BrMaterialAllocate(NULL);
        BrMaterialAdd(oily_material);
        oily_material->flags |= BR_MATF_LIGHT;
        oily_material->flags |= BR_MATF_PERSPECTIVE;
        oily_material->flags |= BR_MATF_SMOOTH;
        oily_material->ka = 0.99f;
        oily_material->kd = 0.0f;
        oily_material->ks = 0.0f;
        oily_material->power = 0.0f;
        oily_material->index_base = 0;
        oily_material->index_range = 0;
        oily_material->colour_map = gOil_pixies[0];
        BrMatrix23Identity(&oily_material->map_transform);
        oily_material->index_shade = BrTableFind("IDENTITY.TAB");
        GlorifyMaterial(&oily_material, 1, kRendererShadingType_Specular);
        BrMaterialUpdate(oily_material, BR_MATU_ALL);
        the_model = BrModelAllocate(NULL, 4, 2);
        the_model->flags |= BR_MODF_KEEP_ORIGINAL;

        the_model->faces[0].vertices[0] = 2;
        the_model->faces[0].vertices[1] = 1;
        the_model->faces[0].vertices[2] = 0;
        the_model->faces[0].material = NULL;
        the_model->faces[0].smoothing = 1;

        the_model->faces[1].vertices[0] = 3;
        the_model->faces[1].vertices[1] = 2;
        the_model->faces[1].vertices[2] = 0;
        the_model->faces[1].material = NULL;
        the_model->faces[1].smoothing = 1;

        BrVector3Set(&the_model->vertices[0].p, -1.f, 0.f, -1.f);
        BrVector2Set(&the_model->vertices[0].map, 0.f, 1.f);
        BrVector3Set(&the_model->vertices[1].p, 1.f, 0.f, 1.f);
        BrVector2Set(&the_model->vertices[1].map, 0.f, 0.f);
        BrVector3Set(&the_model->vertices[2].p, 1.f, 0.f, -1.f);
        BrVector2Set(&the_model->vertices[2].map, 1.f, 0.f);
        BrVector3Set(&the_model->vertices[3].p, -1.f, 0.f, 1.f);
        BrVector2Set(&the_model->vertices[3].map, 1.f, 1.f);
        gOily_spills[i].actor = BrActorAllocate(BR_ACTOR_MODEL, NULL);
        gOily_spills[i].actor->model = the_model;
        gOily_spills[i].actor->render_style = BR_RSTYLE_NONE;
        gOily_spills[i].actor->material = oily_material;
        BrActorAdd(gOther_selfs[2], gOily_spills[i].actor);
    }
}

// ResetOilSpills

// QueueOilSpill2

// QueueOilSpill

// OKToSpillOil

// MungeOilsHeightAboveGround

// MungeIndexedOilsHeightAboveGround

// SetInitialOilStuff

// ProcessOilSpills

// GetOilSpillCount

// GetOilSpillDetails

// PointInSpill

// GetOilFrictionFactors

// AdjustOilSpill

// FUNCTION: CARMA2_HW 0x004a7860
void C2_HOOK_FASTCALL AdjustOilSpill(int pIndex, br_matrix34* pMat, br_scalar pFull_size, br_scalar pGrow_rate, tU32 pSpill_time, tU32 pStop_time, tCar_spec* pCar, br_vector3* pOriginal_pos, br_pixelmap* pPixelmap) {
    (void)pStop_time;
    (void)pPixelmap;
    BrMatrix34Copy(&gOily_spills[pIndex].actor->t.t.mat, pMat);
    gOily_spills[pIndex].full_size = pFull_size;
    gOily_spills[pIndex].grow_rate = pGrow_rate;
    gOily_spills[pIndex].spill_time = pSpill_time;
    gOily_spills[pIndex].stop_time = 0;
    gOily_spills[pIndex].car = pCar;
    BrVector3Copy(&gOily_spills[pIndex].original_pos, pOriginal_pos);
    gOily_spills[pIndex].actor->render_style = BR_RSTYLE_NONE;
}

// FUNCTION: CARMA2_HW 0x004a78e0
void C2_HOOK_FASTCALL ReceivedOilSpill(tNet_message_chunk* pContents) {
    tCar_spec* car;

    car = NetCarFromPlayerID(pContents->oil_spill.player);
    if (car == NULL) {
        return;
    }
    QueueOilSpill2(car, car->car_master_actor, car->car_actor->model, 500,
        0.35f, 0.6f, 0.00003f, 0.0001f, 0.1f, 1.5f, 2.5f, 0.01f, 0.15f,
        oily_material, NULL);
}

// FUNCTION: CARMA2_HW 0x004a7940
void C2_NAKED C2_HOOK_FASTCALL RemoveCarsOilSlicks(tCar_spec* pCar) {
#ifndef CARPOCALYPSE2_MATCHING
    int i;

    for (i = 0; i < (int)CARPOCALYPSE2_ASIZE(gOily_spills); i++) {
        if (gOily_spills[i].car_actor != NULL && gOily_spills[i].car == pCar) {
            gOily_spills[i].car = NULL;
            gOily_spills[i].car_actor = NULL;
        }
    }
#else
    __asm {
            mov eax, offset gOily_spills+0x4
            xor edx, edx
    L004a7947:
            cmp dword ptr [eax], edx
            je L004a7955
            cmp dword ptr [eax + 8], ecx
            jne L004a7955
            mov dword ptr [eax + 8], edx
            mov dword ptr [eax], edx
    L004a7955:
            add eax, 0x54
            cmp eax, offset gBig_bang
            jl L004a7947
            ret
    }
#endif
}
