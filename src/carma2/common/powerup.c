#include "powerup.h"

#include "car.h"
#include "displays.h"
#include "globvars.h"
#include "graphics1.h"
#include "pedestrn.h"
#include "physics.h"
#include "pratcam.h"
#include "spark.h"
#include "globvrpb.h"
#include "powerups.h"
#include "utility.h"
#include "carpocalypse2_macros.h"

#include "c2_string.h"
// GLOBAL: CARMA2_HW 0x006a0ad4
const char* gPowerup_txt_path;

// GLOBAL: CARMA2_HW 0x007059e0
tHeadup_icon gPickedup_powerups[20];

// GLOBAL: CARMA2_HW 0x00763480
tSlot_info gCost_APO;

// GLOBAL: CARMA2_HW 0x0075b900
tSlot_info gTrade_in_value_APO;

// GLOBAL: CARMA2_HW 0x0074d380
tSlot_info gSubstitution_value_APO;

// GLOBAL: CARMA2_HW 0x00761c60
tSlot_info gPotential_substitution_value_APO;

// GLOBAL: CARMA2_HW 0x0074d1a4
int gNet_powerup_time_replacement;

// GLOBAL: CARMA2_HW 0x007447d8
int gPickup_respawn_min_time_ms;

// GLOBAL: CARMA2_HW 0x007447e8
int gPickup_respawn_max_extra_time_ms;

// GLOBAL: CARMA2_HW 0x007059c0
int gCount_mutant_tail_parts;

// GLOBAL: CARMA2_HW 0x00705b70
tU32 gINT_00705b70;

// GLOBAL: CARMA2_HW 0x00705b78
float gMass_mutant_tail_link;

// GLOBAL: CARMA2_HW 0x00705b74
float gMass_mutant_tail_ball;

// GLOBAL: CARMA2_HW 0x0065ebbc
float gMass_mine = 1.0f;

// GLOBAL: CARMA2_HW 0x006a0454
int gCount_cloaked_cars;

// GLOBAL: CARMA2_HW 0x006a0910
tCar_spec* gCloaked_cars[12];

// GLOBAL: CARMA2_HW 0x006a0a54
tPowerup* gPowerup_array;

// GetNextGoodyTime

// DoPowerupTextHeadup

// QueueIconGoing

// QueueIconComing

// LosePowerupX

// LosePowerup

// LoseKeyboardPowerupN

// KeyboardPowerupFinished

// LoseAllSimilarPowerups
extern int gNumber_of_powerups;

void C2_HOOK_FASTCALL LoseAllSimilarPowerups(tPowerup* pThe_powerup) {
    int i;
    tPowerup* the_powerup;

    for (i = 0, the_powerup = gPowerup_array; i < gNumber_of_powerups; i++, the_powerup++) {
        if (the_powerup != pThe_powerup && the_powerup->got_time != 0
            && (pThe_powerup->group_inclusion & the_powerup->group_inclusion) != 0) {
            LosePowerup(the_powerup);
        }
    }
}

// ExcludedPowerup

// FUNCTION: CARMA2_HW 0x004d9150
int C2_HOOK_FASTCALL GotPowerupX(tCar_spec* pCar, int pIndex, int pArg3, int pMessage, tU32 pTime) {
    tPowerup* the_powerup;
    int i;
    int ps_power;
    char s[256];
    char* s2;

    if (pIndex >= gNumber_of_powerups || pIndex < 0) {
        return -1;
    }
    the_powerup = &gPowerup_array[pIndex];
    if (the_powerup->type == ePowerup_dummy) {
        return -1;
    }
    if (the_powerup->got_proc == NULL) {
        return -1;
    }
    while (((gProgram_state.sausage_eater_mode || gCount_active_peds < 2)
            && (strstr(the_powerup->message, "Ped") != NULL
                || strstr(the_powerup->message, "ped") != NULL
                || strstr(the_powerup->message, "corpses") != NULL))
        || (gNet_mode != eNet_mode_none && the_powerup->net_type == eNet_powerup_inappropriate)) {
        pIndex = 0;
        the_powerup = &gPowerup_array[pIndex];
    }
    the_powerup->current_value = -1;
    LoseAllSimilarPowerups(the_powerup);
    ps_power = gNet_mode != eNet_mode_none && the_powerup->got_proc == GotTimeOrPower;
    if (the_powerup->message[0] != '\0' && pMessage && !ps_power) {
        s2 = s;
        strcpy(s2, the_powerup->message);
        if (the_powerup->got_proc == FreezeTimer) {
            s2 = strtok(s, "/");
            if (gFreeze_timer) {
                s2 = strtok(NULL, "/");
            }
        }
        NewTextHeadupSlot(4, 0, 3000, -4, s2);
    }
    the_powerup->car = pCar;
    if (the_powerup->got_proc != NULL) {
        pIndex = the_powerup->got_proc(the_powerup, pCar);
    }
    if (pCar->driver <= 5) {
        return pIndex;
    }
    switch (the_powerup->type) {
    case ePowerup_instantaneous:
        break;
    case ePowerup_timed:
        the_powerup->got_time = GetTotalTime();
        if (pArg3) {
            the_powerup->lose_time = the_powerup->got_time + the_powerup->duration;
        } else {
            the_powerup->lose_time = the_powerup->got_time + pTime;
        }
        gProgram_state.current_car.powerups[pIndex] = the_powerup->lose_time;
        break;
    case ePowerup_whole_race:
        the_powerup->got_time = GetTotalTime();
        gProgram_state.current_car.powerups[pIndex] = -1;
        break;
    default:
        break;
    }
    if (the_powerup->prat_cam_event >= 0) {
        PratcamEvent(the_powerup->prat_cam_event);
    }
    /* NOTE: retail also broadcasts a guaranteed "powerup gained" net message
     * here; omitted like the rest of the netcode (cf. CloseDownPowerUps). */
    if (the_powerup->type != ePowerup_instantaneous && the_powerup->icon != NULL) {
        for (i = 0; i < gNumber_of_icons; i++) {
            if (gPickedup_powerups[i].powerup == the_powerup) {
                gPickedup_powerups[i].fizzle_stage = 4;
                return pIndex;
            }
        }
        if (gNumber_of_icons == CARPOCALYPSE2_ASIZE(gPickedup_powerups)) {
            return pIndex;
        }
        gPickedup_powerups[gNumber_of_icons].powerup = the_powerup;
        gPickedup_powerups[gNumber_of_icons].fizzle_stage = 0;
        gPickedup_powerups[gNumber_of_icons].fizzle_direction = 1;
        gPickedup_powerups[gNumber_of_icons].fizzle_start = GetTotalTime();
        gNumber_of_icons++;
    }
    return pIndex;
}

// GotPowerupXX

// GotPowerupEarwig

// FUNCTION: CARMA2_HW 0x004d8d40
void C2_HOOK_FASTCALL GotPowerupEarwig(tCar_spec* pCar, int pIndex, int pMessage, int pSelect_inventory) {
    int i;

    GotPowerupX(pCar, pIndex, 0, pMessage, 0);
    if (pSelect_inventory) {
        for (i = 0; i < CARPOCALYPSE2_ASIZE(gInventory); i++) {
            if (gInventory[i] < 0) {
                break;
            }
        }
        if (i < CARPOCALYPSE2_ASIZE(gInventory)) {
            gInventory[i] = pIndex;
            gInventory_selected = i;
        }
    }
}

// AutoGoody

// GotPowerup

// DRMapFindMeAPixie


// LoadPowerups (implemented in powerups.c)

// InitRepulseEffects

// MungeBoxObject

// MungeShereObject

// InitTail

// ThisObjectHitAMineSoSpamItUpUsingThisPowerupsSettings

// InitMineShit

// InitPowerups (implemented in powerups.c)

// RemoveRepulseEffect

// ResetRepulseEffects

// EmptyInventory

// ResetPowerups

// CloseDownPowerUps (implemented in powerups.c)

// NewRepulseRay

// RemoveRepulsiveReferences

// MungeRepulseRays

// AdjustRepulseRay

// ResetRepulseRay

// DrawSinglePowerupIcon

// DrawPowerupIcons2

// DrawKeyPowerups

// ToggleInventory

// RightInventory

// LeftInventory

// DrawPowerups

// MungeKeyboardPowerup

// DoPowerupPeriodics

// CheckRespawnQueue

// MungePowerupStuff

// GotPowerupN

// GotPowerup0

// GotPowerup1

// GotPowerup2

// GotPowerup3

// GotPowerup4

// GotPowerup5

// GotPowerup6

// GotPowerup7

// GotPowerup8

// GotPowerup9

// GotCredits

// GainAPO2

// GainAPOPot2

// GainAPO

// GotTimeOrPower

// GainAPOPotential

// SetPedSpeed

// SetPedSize

// SetPedHeadSize

// SetPedExplode

// SetPedBrittle

// SetInvulnerability

// ResetInvulnerability

// SetFreeRepairs

// ResetFreeRepairs

// SetStupidPedestrians

// ResetStupidPedestrians

// FreezeTimer

// UnfreezeTimer

// DoInstantRepair

// ResetPedSpeed

// ResetPedSize

// ResetPedHeadSize

// ResetPedExplode

// ResetPedBrittle

// SetEngineFactor

// SetUnderwater

// TrashBodywork

// TakeDrugs

// PaletteFuckedUpByDrugs

// TheEffectsOfDrugs

// SetOpponentsSpeed

// SetCopsSpeed

// SetGravity

// SetPinball

// SetWallclimb

// SetBouncey

// SetSuspension

// SetTyreGrip

// SetImmortalPeds

// ResetImmortalPeds

// SetSuicidalPeds

// ResetSuicidalPeds

// SetDamageMultiplier

// FillInMine

// SendMines

extern tCar_spec* gActive_car_list[139];
extern int gNum_active_cars;

// FUNCTION: CARMA2_HW 0x004dd1b0
void C2_HOOK_FASTCALL MinePreCollisionStuff(void) {
    int i;

    for (i = 0; i < (int)CARPOCALYPSE2_ASIZE(gShit_mines); i++) {
        tPhysics_object* obj;

        if (!(gShit_mines[i].flags & 0x1)) {
            continue;
        }
        obj = gShit_mines[i].collision_info;
        if (obj->field_0x261 != 0x31 || obj->message_time != gPHIL_last_physics_tick) {
            continue;
        }
        obj->field_0xf0 = 1;
        SelectCrushNetwork(&obj->field_0x294, obj, 2);
        obj->field_0x261 = 0;
    }
}

// ActivateMine

// PositionMine

// MineSound

// ReceivedMine

// ShitMine

// MineExplode

// TiggerMineToExplode

// HitAShatOutMine

// ClearMineReferencesToCar

// MungeShatMines

// FUNCTION: CARMA2_HW 0x004de000
void C2_HOOK_FASTCALL ARDoShitMine(br_actor* pActor, const br_vector3* pUnused) {
    if (pActor->parent == NULL) {
        BrActorAdd(gNon_track_actor, pActor);
    }
    pActor->render_style = BR_RSTYLE_FACES;
}

// FUNCTION: CARMA2_HW 0x004de020
void C2_HOOK_FASTCALL ARUndoShitMine(br_actor* pActor, const br_vector3* pUnused) {
    if (pActor->parent != NULL) {
        BrActorRemove(pActor);
    }
    pActor->render_style = BR_RSTYLE_NONE;
}

// PissOutOil

// KangerooJump

// AnnihilatePeds

// NapalmPeds

// RepulseOpponents2

// RepulseOpponents

// RepulsePedestrians

// ReceivedRepulse

// ResetEngineFactor

// ResetUnderwater

// PukeDrugsBackUp

// ResetOpponentsSpeed

// ResetCopsSpeed

// ResetGravity

// ResetPinball

// ResetWallclimb

// ResetBouncey

// ResetSuspension

// ResetDamageMultiplier

// ResetTyreGrip

// PickAtRandom

// GotVouchers

// MungeVouchers

// SetInstantHandbrake

// ResetInstantHandbrake

// SetDancingPeds

// SetPanickingPeds

// SetLowGravityPeds

// SetPissedPeds

// SetGhostPeds

// ResetDancingPeds

// ResetPanickingPeds

// ResetLowGravityPeds

// ResetPissedPeds

// ResetGhostPeds

// DoBouncey

// HitMine

// SetMassMultiplier

// ResetMassMultiplier

// ShowPedestrians

// HidePedestrians

// SetElectroBastard

// ResetElectroBastard

// SetMutantCorpses

// ResetMutantCorpses

// ReceivedPowerup

// SendCurrentPowerups

// LoseAllLocalPowerups

// LoseDodgyPowerups

// GetPowerupCount

// GetPowerupPosition

// ARPowerupGot

// ARPowerupLose

// SetPissed

// ResetPissed

// PissedEffect

// RenderSpinningPowerup

// RenderChangingPowerup

// SetSpinningPowerup

// DuplicateModel

// SetChangingPowerup

// PedValium

// PowerupCancel

// GrowATail

// CutOffTail

// RemoveTail (implemented in powerups.c)

// FUNCTION: CARMA2_HW 0x004e0710
void C2_HOOK_FASTCALL DoInitialTail(void) {
    tPhysics_object* collision_info;

    for (collision_info = gMutant_tail_first_collision_info; collision_info != NULL; collision_info = collision_info->child) {
        collision_info->actor->render_style = BR_RSTYLE_FACES;
    }
}

// FUNCTION: CARMA2_HW 0x004e0730
void C2_HOOK_FASTCALL UndoInitialTail(void) {
    tPhysics_object* collision_info;

    for (collision_info = gMutant_tail_first_collision_info; collision_info != NULL; collision_info = collision_info->child) {
        collision_info->actor->render_style = BR_RSTYLE_NONE;
    }
}

// MayQueuePowerupRespawn

// FUNCTION: CARMA2_HW 0x004e07d0
void C2_HOOK_FASTCALL ARDoPowerupRespawn(br_actor* pActor, int pUnused) {
    pActor->render_style = BR_RSTYLE_FACES;
}

// FUNCTION: CARMA2_HW 0x004e07e0
void C2_HOOK_FASTCALL ARUndoPowerupRespawn(br_actor* pActor, int pUnused) {
    pActor->render_style = BR_RSTYLE_NONE;
}

// FUNCTION: CARMA2_HW 0x004e07f0
void C2_HOOK_FASTCALL PrintPowerupIconIn3D(int pX, int pY, tHeadup_icon* pIcon, tPowerup* pPowerup, int pScale, tU32 pTime) {
    float f;
    float scale;
    br_fixed_ls prev_opacity;

    if (pIcon == NULL) {
        pPowerup->icon_actor->render_style = BR_RSTYLE_FACES;
        BrVector3Set(&pIcon->icon_actor->t.t.translate.t, (float)pX + 16.0f, (float)-(pY + 16), 0.0f);
        RenderThisHeadup(pIcon->icon_actor);
        return;
    }
    if (pIcon->icon_actor != NULL) {
        pIcon->icon_actor->render_style = BR_RSTYLE_FACES;
        if (pScale != 0) {
            f = (float)(pTime - pIcon->fizzle_start) / 600.0f;
            if (pIcon->fizzle_direction > 0) {
                f = 1.0f - f;
                scale = 1.0f + 5.0f * f;
                BrMatrix34Scale(&pIcon->icon_actor->t.t.mat, scale, scale, 1.0f);
            } else {
                BrMatrix34Scale(&pIcon->icon_actor->t.t.mat, 1.01f - f, 1.01f - f, 1.0f);
            }
            BrMatrix34PostRotateZ(&pIcon->icon_actor->t.t.mat, (br_angle)((1.0f - f / 2.0f) * 65536.0f));
            pIcon->icon_actor->material->extra_prim[1].v.x = BR_FIXED_INT((1.0f - f) * 255.0f);
            BrMaterialUpdate(pIcon->icon_actor->material, BR_MATU_EXTRA_PRIM);
        } else {
            prev_opacity = pIcon->icon_actor->material->extra_prim[1].v.x;
            pIcon->icon_actor->material->extra_prim[1].v.x = BR_FIXED_INT(255);
            if (prev_opacity != pIcon->icon_actor->material->extra_prim[1].v.x) {
                BrMaterialUpdate(pIcon->icon_actor->material, BR_MATU_EXTRA_PRIM);
            }
            BrMatrix34Identity(&pIcon->icon_actor->t.t.mat);
        }
        BrVector3Set(&pIcon->icon_actor->t.t.translate.t, (float)pX + 16.0f, (float)-(pY + 16), 0.0f);
        RenderThisHeadup(pIcon->icon_actor);
    }
}

// FUNCTION: CARMA2_HW 0x004e09d0
br_actor* C2_HOOK_FASTCALL CreateBillBoard(br_pixelmap* pTexture) {
    br_actor* actor;
    br_material* material;
    br_model* model;
    int width;
    int height;
    br_scalar left;
    br_scalar right;
    br_scalar top;
    br_scalar bottom;

    width = pTexture->width;
    height = pTexture->height;
    model = BrModelAllocate("Billboard Model", 4, 2);
    material = BrMaterialAllocate("Billboard Material");
    actor = BrActorAllocate(BR_ACTOR_MODEL, NULL);
    actor->identifier = "Billboard Actor";
    actor->model = model;
    actor->material = material;
    actor->render_style = BR_RSTYLE_FACES;
    model->faces[0].vertices[0] = 0;
    model->faces[0].vertices[1] = 1;
    model->faces[0].vertices[2] = 2;
    model->faces[1].vertices[0] = 2;
    model->faces[1].vertices[1] = 3;
    model->faces[1].vertices[2] = 0;
    model->faces[0].material = NULL;
    model->faces[1].material = NULL;
    left = (float)-(width / 2);
    top = (float)+(height / 2);
    BrVector3Set(&model->vertices[0].p, left,  top, -2.0f);
    bottom = (float)-(height / 2);
    BrVector3Set(&model->vertices[1].p, left, bottom, -2.0f);
    right = (float)+(width / 2);
    BrVector3Set(&model->vertices[2].p, right, bottom, -2.0f);
    BrVector3Set(&model->vertices[3].p, right,  top, -2.0f);
    BrVector2Set(&model->vertices[0].map, 0.0f, 0.0f);
    BrVector2Set(&model->vertices[1].map, 0.0f, 1.0f);
    BrVector2Set(&model->vertices[2].map, 1.0f, 1.0f);
    BrVector2Set(&model->vertices[3].map, 1.0f, 0.0f);
    material->colour = 0;
    material->colour_map = pTexture;
    material->flags = BR_MATF_ALWAYS_VISIBLE | BR_MATF_FORCE_FRONT;
    model->flags |= BR_MODF_KEEP_ORIGINAL;
    BrMapAdd(pTexture);
    BrMaterialAdd(material);
    BrModelAdd(model);
    return actor;
}

// FUNCTION: CARMA2_HW 0x004e0c00
void C2_HOOK_FASTCALL SetDefaultPowerupFilename(void) {
    gPowerup_txt_path = "POWERUP.TXT";
}

// FUNCTION: CARMA2_HW 0x004e0c40
int C2_HOOK_FASTCALL TurnOnCloaking(tPowerup* pPowerup, tCar_spec* pCar) {

    if (gNet_mode != eNet_mode_none && !IsCarCloaked(pCar)) {
        BlendifyCar(pCar);
        MasterDisableCarFunks(pCar);
        gCloaked_cars[gCount_cloaked_cars++] = pCar;
    }
    return pPowerup - gPowerup_array;
}

// FUNCTION: CARMA2_HW 0x004e0cb0
void C2_HOOK_FASTCALL RemoveFromCloakingList(tCar_spec* pCar) {
    int i;

    if (gNet_mode != eNet_mode_none) {
        for (i = 0; i < gCount_cloaked_cars; i++) {

            if (gCloaked_cars[i] == pCar) {
#ifdef CARPOCALYPSE2_FIX_BUGS
                memmove(&gCloaked_cars[i], &gCloaked_cars[i + 1], (gCount_cloaked_cars - i - 1) * sizeof(tCar_spec*));
#else
                memcpy(&gCloaked_cars[i], &gCloaked_cars[i + 1], (CARPOCALYPSE2_ASIZE(gCloaked_cars) - i - 1) * sizeof(tCar_spec*));
#endif
                gCount_cloaked_cars -= 1;
                break;
            }
        }
    }
}

// FUNCTION: CARMA2_HW 0x004e0d10
int C2_HOOK_FASTCALL IsCarCloaked(tCar_spec* pCar) {
    int i;

    if (gNet_mode != eNet_mode_none) {
        for (i = 0; i < gCount_cloaked_cars; i++) {
            if (gCloaked_cars[i] == pCar) {
                return 1;
            }
        }
    }
    return 0;
}

// FUNCTION: CARMA2_HW 0x004e0d50
void C2_HOOK_FASTCALL TurnOffCloaking(tPowerup* pPowerup, tCar_spec* pCar) {

    if (gNet_mode != eNet_mode_none) {
        UnBlendifyCar(pCar);
        RemoveFromCloakingList(pCar);
        MasterEnableCarFunks(pCar);
        RestoreCarPixelmaps(pCar);
    }
}

// FUNCTION: CARMA2_HW 0x004e0de0
void C2_HOOK_FASTCALL PeriodicCloaking(void) {
    int i;

    if (gNet_mode != eNet_mode_none) {

        for (i = 0; i < gCount_cloaked_cars; i++) {
            DoCamouflageThing(gCloaked_cars[i]);
        }
    }
}
// FUNCTION: CARMA2_HW 0x004df200
void C2_HOOK_FASTCALL FUN_004df200(tPowerup* pPowerup, tCar_spec* pCar) {

    if (pPowerup->got_proc != NULL) {
        ((void (C2_HOOK_FASTCALL *)(tPowerup*, tCar_spec*))pPowerup->got_proc)(pPowerup, pCar);
    }
}

// FUNCTION: CARMA2_HW 0x004df210
void C2_HOOK_FASTCALL FUN_004df210(tPowerup* pPowerup, tCar_spec* pCar) {
    if (pPowerup->lose_proc != NULL) {
        pPowerup->lose_proc(pPowerup, pCar);
    }
}

// FUNCTION: CARMA2_HW 0x004d9670
void C2_HOOK_FASTCALL QueueIconGoing(tPowerup* pPowerup, tHeadup_icon* pHeadup_icons, int* pCount_icons) {
    int i;

    for (i = 0; i < *pCount_icons; i++) {
        if (pHeadup_icons[i].powerup == pPowerup) {
            pHeadup_icons[i].fizzle_stage = 3;
            pHeadup_icons[i].fizzle_direction = -1;
            pHeadup_icons[i].fizzle_start = GetTotalTime();
            break;
        }
    }
}

// FUNCTION: CARMA2_HW 0x004e0ba0
void C2_HOOK_FASTCALL DisposeBillBoard(br_actor* pActor) {

    if (pActor != NULL) {
        if (pActor->material != NULL) {
            BrMaterialRemove(pActor->material);
            BrMaterialFree(pActor->material);
        }
        if (pActor->model != NULL) {
            BrModelRemove(pActor->model);
            BrModelFree(pActor->model);
        }
        if (pActor->parent != NULL) {
            BrActorRemove(pActor);
        }
        BrActorFree(pActor);
    }
}

// FUNCTION: CARMA2_HW 0x004e0e20
void C2_HOOK_FASTCALL FUN_004e0e20(void) {

    // empty
}
