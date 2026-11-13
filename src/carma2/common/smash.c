#include "smash.h"
#include "world.h"
#include "oil.h"

#include "utility.h"
#include "globvars.h"
#include "globvrpb.h"
#include "physics.h"
#include "smashing.h"
#include "carpocalypse2_types.h"
#include "carpocalypse2_macros.h"
// GLOBAL: CARMA2_HW 0x006a55c8
tExplosion gExplosions[50];
#ifdef CARPOCALYPSE2_MATCHING
#endif
#ifdef CARPOCALYPSE2_MATCHING
#endif
#ifdef CARPOCALYPSE2_MATCHING
extern void C2_HOOK_CDECL BrMatrix34Identity();
extern void C2_HOOK_FASTCALL DoNetSmash(tQueued_net_smash* pSmash);
extern int C2_HOOK_FASTCALL FUN_004f0d50(const float* pPoint, const float* pNormal, float pPlane_distance, const float* pVertex0, const float* pVertex1, const float* pVertex2);
extern void C2_HOOK_FASTCALL FillInBounds(tBounds* bnds);
extern int C2_HOOK_FASTCALL FindFacesInBox(tBounds* bnds, tFace_ref* face_list, int max_face, tWorld_callbacks* pWorld_callbacks);
extern void C2_HOOK_FASTCALL FlushSmashQueue(int pFlush_powerups);
extern tCar_spec* C2_HOOK_FASTCALL GetCarSpec(tVehicle_type pCategory, int pIndex);
extern float C2_HOOK_FASTCALL SmashEnvironment(tPhysics_object* pObject, undefined4* pArg2, float pArg3, br_vector3* pArg4, br_vector3* pArg5, br_vector3* pArg6, int pArg7, int pArg8);
extern int gCount_host_smashes;
extern int gINT_006a8294;
extern tStored_smash* gNet_host_smashes;
extern tNet_mode gNet_mode;
extern tNet_game_player_info gNet_players[12];
extern int gNumber_of_net_players;
extern tU32 gPHIL_last_physics_tick;
extern tProgram_state gProgram_state;
extern tQueued_net_smash gQueued_net_smashes[50];
extern tWorld_callbacks gWorld_callbacks;
#endif
#ifdef CARPOCALYPSE2_MATCHING
extern void C2_HOOK_FASTCALL ActuallyRepairSmash(tCar_spec* pCar_spec, tCar_crush_smashable_part* pSmashable, int pLevel);
extern tU32 C2_HOOK_FASTCALL GetTotalTime(void);
extern tRepair_animation gRepair_animations[25];
#endif
// FUNCTION: CARMA2_HW 0x004ea880

void C2_HOOK_FASTCALL InitExplosions(void) {
    int i;
    int capacity;
    tExplosion* explosion;

    capacity = (int)CARPOCALYPSE2_ASIZE(gExplosions);
    for (i = 0, explosion = gExplosions; i < capacity; explosion++, i++) {

        explosion->actor = BrActorAllocate(BR_ACTOR_MODEL, NULL);

        explosion->actor->model = BrModelAllocate(NULL, 4, 2);
        explosion->actor->model->faces[0].vertices[0] = 0;
        explosion->actor->model->faces[0].vertices[1] = 2;
        explosion->actor->model->faces[0].vertices[2] = 1;
        explosion->actor->model->faces[0].material = NULL;
        explosion->actor->model->faces[0].smoothing = 1;
        explosion->actor->model->faces[1].vertices[0] = 0;
        explosion->actor->model->faces[1].vertices[1] = 3;
        explosion->actor->model->faces[1].vertices[2] = 2;
        explosion->actor->model->faces[1].material = NULL;
        explosion->actor->model->faces[1].smoothing = 1;
        BrVector3Set(&explosion->actor->model->vertices[0].p, -.5f, -.5f, 0.f);
        BrVector2Set(&explosion->actor->model->vertices[0].map, 0.f, 1.f);
        BrVector3Set(&explosion->actor->model->vertices[1].p, -.5f, .5f, 0.f);
        BrVector2Set(&explosion->actor->model->vertices[1].map, 0.f, 0.f);
        BrVector3Set(&explosion->actor->model->vertices[2].p, .5f, .5f, 0.f);
        BrVector2Set(&explosion->actor->model->vertices[2].map, 1.f, 0.f);
        BrVector3Set(&explosion->actor->model->vertices[3].p, .5f, -.5f, 0.f);
        BrVector2Set(&explosion->actor->model->vertices[3].map, 1.f, 1.f);
        BrModelAdd(explosion->actor->model);

        explosion->actor->material = BrMaterialAllocate("BANG!");
        explosion->actor->material->flags &= ~BR_MATF_LIGHT;
        explosion->actor->material->flags |= BR_MATF_ALWAYS_VISIBLE;
        explosion->actor->material->colour_map = gBack_screen;
        GlorifyMaterial(&explosion->actor->material, 1, kRendererShadingType_AmbientOnly);
        BrMaterialAdd(explosion->actor->material);
        explosion->actor->render_style = BR_RSTYLE_FACES;

        explosion->start = 0;
    }
}

// ResetExplosions

// RemoveAllExplosionettes

// MungeExplosions

// FUNCTION: CARMA2_HW 0x004eace0
void C2_HOOK_FASTCALL CopyExplosionData(int pIndex, tExplosion* pExplosion) {
    gExplosions[pIndex] = *pExplosion;
}

// DoExplosionette

// Explode

// FUNCTION: CARMA2_HW 0x004eb020
void C2_HOOK_FASTCALL RemoveExplosions(tCar_spec* pCar_spec) {
    int i;
    tExplosion* explosion;

    for (i = CARPOCALYPSE2_ASIZE(gExplosions), explosion = gExplosions; i > 0; i--, explosion++) {
        if (explosion->start != 0 && explosion->collision_actor == pCar_spec->collision_info->actor) {
            if (explosion->actor->parent != NULL) {
                BrActorRemove(explosion->actor);
            }
            explosion->finished = 1;
        }
    }
}

// CalcRandomPointBetween

// ApplyInitialMovement

// ApplyTimeSpec

// FindShrapnelFragment

// SetShrapnelFragment

// ShatterObject

// MungeInternalCarGlass

// ActuallySmashPart

// MakeSmashSound

// MungeSmokeFire

// Random34Matrix

// SetInitPosition

// DoGhostPartShrapnel

// DoNonCarShrapnel

// DoShrapnel

// DoAward

// FUNCTION: CARMA2_HW 0x004ecc40
void C2_HOOK_FASTCALL DoSpillage(tSlick_spec* pSlick, br_actor* pActor) {
    QueueOilSpill2(NULL, pActor, pActor->model, pSlick->field_0x0,
        pSlick->field_0x4, pSlick->field_0x8, pSlick->field_0xc, pSlick->field_0x10,
        pSlick->field_0x14, pSlick->field_0x20, pSlick->field_0x24, pSlick->field_0x18,
        pSlick->field_0x1c, pSlick->material, pSlick->material2);
}

// DoSpecial

// GiveNonCarsAShove

// QueueDelayedSideEffect

// DoDelayedNonCar

// DoDelayedSmash

// MungeDelayedSideEffects

// ActivateNonCarSideEffects

// ActivateSmashSideEffects

// DoSideEffects

// DoVariableEffects

// DoConnotations

// DoPowerupRespawnSmash

// DoConnotationsMultiLevel

// SmashItIntoVerySmallPiecesIndeed

// InitSmashQueue

// FlushPowerupQueue

// FlushSmashQueue

// QueueSmashIfNecessary3

// QueueSmashIfNecessary2

// AllocateSmashLevels

// ReadSmashSounds

// ReadShrapnelMovementSpec

// ReadShrapnelInitialPosition

// ReadShrapnelTimeInfo

// NoteChildren

// ReadSmokeFireChance

// LoadActorFromRace

// ReadShrapnelSpec

// ReadSpillData

// ReadExplosion

// FreeExplosion

// ReadSpecialEffectsSpec

// ReadBounds

// ReadNoncarSideEffects

// ReadSmashSideEffects

// ReadSideEffects

// ReadLightingInformation

// ReadAward

// ReadVariableChanges

// ReadConnotations

// ReadSmashLevels

// ReadSmashableData

// ActuallyRepairSmash

// KillFragment

void C2_HOOK_FASTCALL AddRepairAnimationToQueue(tCar_spec* pCar_spec, tCar_crush_smashable_part* pSmashable) {
    tRepair_animation* p;
    int i;
    int index;

    index = -1;
    i = 0;
    for (p = gRepair_animations; p < &gRepair_animations[CARPOCALYPSE2_ASIZE(gRepair_animations)]; p++, i++) {
        if (p->field_0x0 == pSmashable) {
            index = i;
            break;
        }
    }
    if (index < 0) {
        i = 0;
        for (p = gRepair_animations; p < &gRepair_animations[CARPOCALYPSE2_ASIZE(gRepair_animations)]; p++, i++) {
            if (p->field_0x0 == NULL) {
                index = i;
                break;
            }
        }
    }
    if (index >= 0) {
        gRepair_animations[index].field_0x0 = pSmashable;
        gRepair_animations[index].field_0x4 = GetTotalTime();
        gRepair_animations[index].field_0x8 = pCar_spec;
    }
}

// FUNCTION: CARMA2_HW 0x004ef910
void C2_NAKED C2_HOOK_FASTCALL RepairSmash(tCar_spec* pCar_spec, tCar_crush_buffer_entry* pSmash_data) {
#ifndef CARPOCALYPSE2_MATCHING
    tCar_crush_smashable_part* smashable = pSmash_data->smashables;
    int i;

    for (i = 0; i < pSmash_data->count_smashables; i++, smashable++) {
        if (smashable->field_0x4c != 0) {
            ActuallyRepairSmash(pCar_spec, smashable, smashable->field_0x4c - 1);
            AddRepairAnimationToQueue(pCar_spec, smashable);
        }
    }
#else
    __asm {
            push ecx
            push ebx
            push ebp
            push esi
            mov esi, edx
            xor ebp, ebp
            push edi
            cmp word ptr [esi + 0x38], bp
            mov edi, dword ptr [esi + 0x3c]
            mov dword ptr [esp + 0x10], esi
            mov ebx, ecx
            jbe L004ef9b3
    L004ef92c:
            mov eax, dword ptr [edi + 0x4c]
            test eax, eax
            je L004ef9a1
            dec eax
            mov edx, edi
            push eax
            mov ecx, ebx
            call ActuallyRepairSmash
            or edx, 0xffffffff
            xor eax, eax
            mov ecx, offset gRepair_animations
    L004ef948:
            cmp dword ptr [ecx], edi
            je L004ef95a
            add ecx, 0xc
            inc eax
            cmp ecx, offset gRepair_animations+0x12c
            jl L004ef948
            jmp L004ef95c
    L004ef95a:
            mov edx, eax
    L004ef95c:
            test edx, edx
            jge L004ef980
            xor eax, eax
            mov ecx, offset gRepair_animations
    L004ef967:
            cmp dword ptr [ecx], 0
            je L004ef97a
            add ecx, 0xc
            inc eax
            cmp ecx, offset gRepair_animations+0x12c
            jl L004ef967
            jmp L004ef97c
    L004ef97a:
            mov edx, eax
    L004ef97c:
            test edx, edx
            jl L004ef9a1
    L004ef980:
            lea esi, [eax + eax*2]
            shl esi, 2
            mov dword ptr [esi + gRepair_animations], edi
            call GetTotalTime
            mov dword ptr [esi + gRepair_animations+0x4], eax
            mov dword ptr [esi + gRepair_animations+0x8], ebx
            mov esi, dword ptr [esp + 0x10]
    L004ef9a1:
            xor eax, eax
            inc ebp
            mov ax, word ptr [esi + 0x38]
            add edi, 0x7c
            cmp ebp, eax
            jl L004ef92c
    L004ef9b3:
            pop edi
            pop esi
            pop ebp
            pop ebx
            pop ecx
            ret
    }
#endif
}

// TotallyRepairSmash

__inline void C2_HOOK_FASTCALL InitGlassFragments(void) {
    int i;
    tSmash_vertex* smash_vertex;

    C2_HOOK_BUG_ON(CARPOCALYPSE2_ASIZE(gSmash_glass_fragments) != 200);
    C2_HOOK_BUG_ON(CARPOCALYPSE2_ASIZE(gDecals) != 50);

    for (i = CARPOCALYPSE2_ASIZE(gSmash_glass_fragments), smash_vertex = gSmash_glass_fragments; i > 0; i--, smash_vertex++) {
        smash_vertex->actor = BrActorAllocate(BR_ACTOR_MODEL, NULL);
        smash_vertex->actor->model = BrModelAllocate(NULL, 3, 1);
        smash_vertex->actor->model->faces[0].vertices[0] = 0;
        smash_vertex->actor->model->faces[0].vertices[1] = 1;
        smash_vertex->actor->model->faces[0].vertices[2] = 2;
        smash_vertex->actor->model->flags |= BR_MODF_UPDATEABLE;
        BrModelAdd(smash_vertex->actor->model);
        smash_vertex->model = smash_vertex->actor->model;
        smash_vertex->actor->render_style = BR_RSTYLE_FACES;
        smash_vertex->end_time = 0;
    }
}

static __inline void C2_HOOK_FASTCALL InitDecals(void) {
    int i;
    tDecal* smash_quad;

    for (i = CARPOCALYPSE2_ASIZE(gDecals), smash_quad = gDecals; i > 0; i--, smash_quad++) {
        smash_quad->actor = BrActorAllocate(BR_ACTOR_MODEL, NULL);
        smash_quad->actor->model = BrModelAllocate(NULL, 4, 2);
        smash_quad->actor->model->faces[0].vertices[0] = 0;
        smash_quad->actor->model->faces[0].vertices[1] = 1;
        smash_quad->actor->model->faces[0].vertices[2] = 3;
        smash_quad->actor->model->faces[1].vertices[0] = 0;
        smash_quad->actor->model->faces[1].vertices[1] = 3;
        smash_quad->actor->model->faces[1].vertices[2] = 2;
        smash_quad->actor->model->flags |= BR_MODF_UPDATEABLE;
        BrModelAdd(smash_quad->actor->model);
        smash_quad->actor->render_style = BR_RSTYLE_FACES;
        smash_quad->time = 0;
    }
}

// RemoveAllDecals

// ReadPowerupSmash

// FUNCTION: CARMA2_HW 0x004efe00
void C2_HOOK_FASTCALL InitSmashing(void) {
    InitGlassFragments();
    InitDecals();
}

// VerySimpleRotate

// DoFragMovement

// MungeGlassFragments2

// MungeAnimationRepairs

// MungeGlassFragments

// KillAllGlassFragments

// FUNCTION: CARMA2_HW 0x004f02b0
void C2_HOOK_FASTCALL CleanUpSmashStuff(void) {
    int i;
    tDecal* decal;
    tExplosion* explosion;

    MungeGlassFragments2(1);
    i = (int)CARPOCALYPSE2_ASIZE(gDecals);
    decal = gDecals;
    do {
        if (decal->time != 0) {
            BrActorRemove(decal->actor);
        }
        decal->time = 0;
        decal++;
        i--;
    } while (i != 0);
    i = (int)CARPOCALYPSE2_ASIZE(gExplosions);
    explosion = gExplosions;
    do {
        if (explosion->actor->parent != NULL) {
            BrActorRemove(explosion->actor);
        }
        explosion++;
        i--;
    } while (i != 0);
}

// FUNCTION: CARMA2_HW 0x004f6020
void C2_HOOK_FASTCALL TurnOnSmashDecal(int pIndex) {
    BrActorAdd(gOther_selfs[0], gDecals[pIndex].actor);
}

// FUNCTION: CARMA2_HW 0x004f6040
void C2_HOOK_FASTCALL TurnOffSmashDecal(int pIndex) {
    BrActorRemove(gDecals[pIndex].actor);
}

// FUNCTION: CARMA2_HW 0x004f0310
void C2_HOOK_FASTCALL DisposeSmashData(tCar_crush_buffer_entry* pCrush_data) {
    int i;

    if (pCrush_data->smashables != NULL) {
        for (i = 0; i < pCrush_data->count_smashables; i++) {
            if (pCrush_data->smashables[i].field_0x70 != NULL) {
                BrMemFree(pCrush_data->smashables[i].field_0x70);
            }
            if (pCrush_data->smashables[i].field_0x78 != NULL) {
                BrMemFree(pCrush_data->smashables[i].field_0x78);
            }
            if (pCrush_data->smashables[i].levels != NULL) {
                BrMemFree(pCrush_data->smashables[i].levels);
            }
        }
        BrMemFree(pCrush_data->smashables);
    }
}

// DoBackwardShrapnel

// InitRepairAnimations

// SplondificatalizeIdentifier

// ReadSmashReplace

// ReadSmashTexture

// ReadSmashableEnvironment

// DisposeSmashEnvNonCars

// CloneMemory

// CloneEnvSmash

// FUNCTION: CARMA2_HW 0x004f0a80
void C2_HOOK_FASTCALL FreeConnotations(tConnotations* pConnotations, int pKeep_actors) {
    int i;
    int j;
    tExplosion_animation_group* group;

    for (i = 0; i < pConnotations->count_shrapnel; i++) {
        if (pConnotations->shrapnel[i].type == kShrapnelType_GhostParts && !pKeep_actors) {
            for (j = 0; j < pConnotations->shrapnel[i].type_info.ghost.count_actors; j++) {
                BrActorFree(pConnotations->shrapnel[i].type_info.ghost.actors[j]);
            }
            BrMemFree(pConnotations->shrapnel[i].type_info.ghost.actors);
        } else if (pConnotations->shrapnel[i].type == kShrapnelType_NonCars && !pKeep_actors) {
            for (j = 0; j < pConnotations->shrapnel[i].type_info.noncar.count_actors; j++) {
                if (pConnotations->shrapnel[i].type_info.noncar.actors[j]->parent == NULL) {
                    BrActorFree(pConnotations->shrapnel[i].type_info.noncar.actors[j]);
                }
            }
            BrMemFree(pConnotations->shrapnel[i].type_info.noncar.actors);
        }
    }
    if (pConnotations->special_effects.explosion_animation.count_groups != 0) {
        for (i = 0, group = pConnotations->special_effects.explosion_animation.groups; i < pConnotations->special_effects.explosion_animation.count_groups; i++, group++) {
            BrMemFree(group->frames);
        }
        BrMemFree(pConnotations->special_effects.explosion_animation.groups);
    }
    if (pConnotations->side_effects.activations.count_activations != 0) {
        BrMemFree(pConnotations->side_effects.activations.activations);
    }
    if (pConnotations->side_effects.side_effects.count_side_effects != 0) {
        BrMemFree(pConnotations->side_effects.side_effects.side_effects);
    }
}

// FreeEnvSmash

// DisposeSmashableEnvironment

// MungeSmashMaterialNames

// ExtrapolateTextureCoordinates

// ExtrapolateGeometryCoordinates2

// ExtrapolateGeometryCoordinates

// ExtractDeTiledFaces

// AddDecal

// InsertQuad

// SpawnSmashAndMungeFaces

// DistanceFromFaceND

// PointInFace

// FindFaceWithPoint

// ProcessTexture

// ProcessMesh

// ProcessModel

// BashModel

// SendNetSmash

// SendAllSmashes

// HitPowerup

// SmashEnvironment

// BashObject

// NonCarSnapSmashCallback

// SmashFaceMustBeUpdateable

// MungeMaterialCB

// MungeMaterial

// ReinitSmashing

// LinkSmashies

// FUNCTION: CARMA2_HW 0x004f5a10
static intptr_t C2_HOOK_CDECL RepairModels(br_actor* pActor, void* pUser) {
    tUser_crush_data* user_crush = pActor->user;
    tCar_crush_buffer_entry* crush_data;

    if (user_crush != NULL) {
        crush_data = user_crush->crush_data;
        if (crush_data != NULL) {
            tCar_crush_smashable_part* smashable = crush_data->smashables;
            int i;

            for (i = 0; i < crush_data->count_smashables; i++, smashable++) {
                if (smashable->field_0x4c != 0) {
                    ActuallyRepairSmash(pUser, smashable, 0);
                }
            }
        }
    }
    return 0;
}

// FUNCTION: CARMA2_HW 0x004f5a00
intptr_t C2_HOOK_FASTCALL RepairAllSmashes(tCar_spec* pCar_spec) {
    return DRActorEnumRecurse(pCar_spec->car_master_actor, RepairModels, pCar_spec);
}

// SmashEverythingCB

// SmashEverything

// DetermineTilitudinality

// SetSmashableModel

// LoadWastedExplosion

// FUNCTION: CARMA2_HW 0x004f5f60
void C2_HOOK_FASTCALL ChangeSmashModel(br_actor* pActor, br_model* pOld_model, br_model* pNew_model) {
    (void)pOld_model;
    pActor->model = pNew_model;
}

// FUNCTION: CARMA2_HW 0x004f5f70
void C2_HOOK_FASTCALL TurnOnSmashModel(br_actor* pActor) {
    pActor->render_style = BR_RSTYLE_FACES;
}

// FUNCTION: CARMA2_HW 0x004f5f80
void C2_HOOK_FASTCALL TurnOffSmashModel(br_actor* pActor) {
    pActor->render_style = BR_RSTYLE_NONE;
}

// FUNCTION: CARMA2_HW 0x004f5f90
void C2_HOOK_FASTCALL SmashRemoveSomeFaces(br_model* pModel, int pCount) {
    pModel->nfaces -= pCount;
    DRModelUpdateAndKevificateMaterials(pModel, BR_MODU_ALL);
}

// SmashRestoreSomeFaces

// TurnOnSmashDecal

// TurnOffSmashDecal

// ChangeSmashTexture

// FUNCTION: CARMA2_HW 0x004f60b0
void C2_NAKED C2_HOOK_FASTCALL QueueNetSmash(tQueued_net_smash* pSmash) {
#ifndef CARPOCALYPSE2_MATCHING
    int i;
    int is_host;
    tQueued_net_smash* pQueued_smash;
    tQueued_net_smash* pHost_smash;

    if (gPHIL_last_physics_tick == 0 || gPHIL_last_physics_tick >= pSmash->field_0x4) {
        DoNetSmash(pSmash);
        return;
    }

    i = 0;
    pQueued_smash = gQueued_net_smashes;
    do {
        if (pQueued_smash->field_0x4 == 0) {
            goto queue_net_smash_found;
        }
        i++;
        pQueued_smash++;
    } while (i < 50);
    return;
queue_net_smash_found:

    is_host = gNet_mode == eNet_mode_host;
    *pQueued_smash = *pSmash;

    if (is_host && gCount_host_smashes < 1000) {
        pHost_smash = (tQueued_net_smash*)((tU8*)gNet_host_smashes + gCount_host_smashes * 0x38);
        gCount_host_smashes++;
        *pHost_smash = *pSmash;
    }
#else
    __asm {
            mov eax, dword ptr [gPHIL_last_physics_tick]
            push esi
            test eax, eax
            push edi
            mov edx, ecx
            je L004f611d
            cmp eax, dword ptr [edx + 4]
            jae L004f611d
            xor eax, eax
            mov edi, offset gQueued_net_smashes
    L004f60c9:
            mov ecx, dword ptr [edi + 4]
            test ecx, ecx
            je L004f60dc
            inc eax
            add edi, 0x38
            cmp eax, 0x32
            jl L004f60c9
            pop edi
            pop esi
            ret
    L004f60dc:
            mov eax, dword ptr [gNet_mode]
            mov ecx, 0xe
            mov esi, edx
            cmp eax, 2
            rep movsd
            jne L004f6124
            mov eax, dword ptr [gCount_host_smashes]
            cmp eax, 0x3e8
            jge L004f6124
            mov esi, dword ptr [gNet_host_smashes]
            mov ecx, eax
            shl ecx, 3
            sub ecx, eax
            inc eax
            mov dword ptr [gCount_host_smashes], eax
            lea edi, [esi + ecx*8]
            mov ecx, 0xe
            mov esi, edx
            rep movsd
            pop edi
            pop esi
            ret
    L004f611d:
            mov ecx, edx
            call DoNetSmash
    L004f6124:
            pop edi
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
    }
#endif
}

// FUNCTION: CARMA2_HW 0x004f6130
void C2_NAKED C2_HOOK_FASTCALL DoNetSmash(tQueued_net_smash* pSmash) {
#ifndef CARPOCALYPSE2_MATCHING

    NOT_IMPLEMENTED();
#else
    __asm {
            sub esp, 0x6c0
            push ebx
            push ebp
            push esi
            mov esi, ecx
            push edi
            mov dword ptr [esp + 0x18], esi
            mov ecx, dword ptr [esi + 8]
            cmp ecx, -1
            je L004f619c
            mov eax, dword ptr [gNet_mode]
            test eax, eax
            je L004f6170
            cmp ecx, dword ptr [gNumber_of_net_players]
            jge L004f616c
            lea eax, [ecx + ecx*2]
            lea eax, [eax + eax*8]
            shl eax, 1
            sub eax, ecx
            mov eax, dword ptr [eax*4 + gNet_players+0xc8]
            jmp L004f618f
    L004f616c:
            xor eax, eax
            jmp L004f618f
    L004f6170:
            test ecx, 0xffffff00
            jne L004f617f
            mov eax, offset gProgram_state+0xac
            jmp L004f618f
    L004f617f:
            mov edx, ecx
            and edx, 0xff
            sar ecx, 8
            call GetCarSpec
    L004f618f:
            test eax, eax
            je L004f619c
            mov ecx, dword ptr [eax + 8]
            mov dword ptr [esp + 0x10], ecx
            jmp L004f61a4
    L004f619c:
            mov dword ptr [esp + 0x10], 0
    L004f61a4:
            mov eax, dword ptr [esp + 0x10]
            test eax, eax
            je L004f62d1
            lea eax, [esp + 0x60]
            lea edx, [esp + 0x60]
            push eax
            mov dword ptr [esp + 0x20], edx
            call BrMatrix34Identity
            mov ecx, dword ptr [esi + 0x10]
            mov edx, dword ptr [esi + 0x14]
            lea eax, [esi + 0x10]
            mov dword ptr [esp + 0x88], ecx
            mov dword ptr [esp + 0x18], eax
            mov eax, dword ptr [esi + 0x18]
            add esp, 4
            lea ecx, [esp + 0x1c]
            mov dword ptr [esp + 0x88], edx
            mov dword ptr [esp + 0x8c], eax
            mov dword ptr [esp + 0x20], 0xbd4ccccd
            mov dword ptr [esp + 0x24], 0xbd4ccccd
            mov dword ptr [esp + 0x28], 0xbd4ccccd
            mov dword ptr [esp + 0x2c], 0x3d4ccccd
            mov dword ptr [esp + 0x30], 0x3d4ccccd
            mov dword ptr [esp + 0x34], 0x3d4ccccd
            call FillInBounds
            lea edx, [esp + 0x90]
            lea ecx, [esp + 0x1c]
            push offset gWorld_callbacks
            push 0x14
            call FindFacesInBox
            mov ebp, eax
            xor edi, edi
            test ebp, ebp
            lea ebx, [esp + 0x90]
            jle L004f62d1
            lea esi, [esp + 0xa0]
    L004f6254:
            mov eax, dword ptr [esi + 0x34]
            lea ecx, [esi + 0xc]
            push ecx
            mov ecx, dword ptr [esp + 0x18]
            lea edx, [esi - 0xc]
            push esi
            push edx
            push eax
            lea edx, [esi + 0x24]
            call FUN_004f0d50
            test eax, eax
            jne L004f6287
            inc edi
            add ebx, 0x50
            add esi, 0x50
            cmp edi, ebp
            jl L004f6254
            pop edi
            pop esi
            pop ebp
            pop ebx
            add esp, 0x6c0
            ret
    L004f6287:
            mov eax, dword ptr [esp + 0x18]
            push 1
            mov ecx, dword ptr [eax + 0x34]
            and ecx, 1
            mov dword ptr [gINT_006a8294], ecx
            mov edx, dword ptr [eax + 0x34]
            shr edx, 1
            and edx, 1
            lea ecx, [eax + 0x28]
            push edx
            lea edx, [eax + 0x1c]
            push ecx
            mov ecx, dword ptr [esp + 0x20]
            push edx
            mov edx, dword ptr [eax + 0xc]
            push ecx
            mov ecx, dword ptr [esp + 0x24]
            push edx
            mov edx, ebx
            call SmashEnvironment
            xor ecx, ecx
            fstp st(0)
            call FlushSmashQueue
            mov dword ptr [gINT_006a8294], 0
    L004f62d1:
            pop edi
            pop esi
            pop ebp
            pop ebx
            add esp, 0x6c0
            ret
            nop
            nop
            nop
            nop
    }
#endif
}

// ReceivedSmashEnvironment

// DoQueuedNetSmashes

// FindTheBastard

// ReceivedSmashShraps

// MungeSmashEdgeTriggers