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
void C2_HOOK_FASTCALL RepairSmash(tCar_spec* pCar_spec, tCar_crush_buffer_entry* pSmash_data) {
    tCar_crush_smashable_part* smashable = pSmash_data->smashables;
    int i;

    for (i = 0; i < pSmash_data->count_smashables; i++, smashable++) {
        if (smashable->field_0x4c != 0) {
            ActuallyRepairSmash(pCar_spec, smashable, smashable->field_0x4c - 1);
            AddRepairAnimationToQueue(pCar_spec, smashable);
        }
    }
}

// TotallyRepairSmash

void C2_HOOK_FASTCALL InitGlassFragments(void) {
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

static void C2_HOOK_FASTCALL InitDecals(void) {
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

    MungeGlassFragments2(1);
    i = CARPOCALYPSE2_ASIZE(gDecals);
    do {
        tDecal* decal = &gDecals[CARPOCALYPSE2_ASIZE(gDecals) - i];
        if (decal->time != 0) {
            BrActorRemove(decal->actor);
        }
        decal->time = 0;
        i--;
    } while (i != 0);
    i = CARPOCALYPSE2_ASIZE(gExplosions);
    do {
        if (gExplosions[CARPOCALYPSE2_ASIZE(gExplosions) - i].actor->parent != NULL) {
            BrActorRemove(gExplosions[CARPOCALYPSE2_ASIZE(gExplosions) - i].actor);
        }
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
void C2_HOOK_FASTCALL QueueNetSmash(tQueued_net_smash* pSmash) {
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
}

// FUNCTION: CARMA2_HW 0x004f6130
void C2_HOOK_FASTCALL DoNetSmash(tQueued_net_smash* pSmash) {

    NOT_IMPLEMENTED();
}

// ReceivedSmashEnvironment

// DoQueuedNetSmashes

// FindTheBastard

// ReceivedSmashShraps

// MungeSmashEdgeTriggers