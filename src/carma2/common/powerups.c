#include "powerups.h"

#include "animation.h"
#include "car.h"
#include "controls.h"
#include "crush.h"
#include "displays.h"
#include "errors.h"
#include "explosions.h"
#include "funk.h"
#include "globvars.h"
#include "globvrpb.h"
#include "grafdata.h"
#include "graphics.h"
#include "init.h"
#include "input.h"
#include "joystick.h"
#include "loading.h"
#include "network.h"
#include "oil.h"
#include "opponent.h"
#include "pedestrn.h"
#include "physics.h"
#include "piping.h"
#include "platform.h"

#include "raycast.h"

#include "shrapnel.h"
#include "skidmark.h"
#include "smashing.h"
#include "sound.h"
#include "spark.h"
#include "utility.h"
#include "world.h"

#include "brender/brender.h"

#include "carpocalypse2_macros.h"

#include "c2_string.h"
#include "carpocalypse2_types.h"

#include <math.h>

#include <stdlib.h>

// GLOBAL: CARMA2_HW 0x0058abf0
const double gDOUBLE_0058abf0 = 0.45;

// GLOBAL: CARMA2_HW 0x0058abf8
const double gDOUBLE_0058abf8 = 0.99;

extern float gPedestrian_speed_factor;
extern int gExploding_pedestrians;
extern int gPed_dismemberfest;
extern int gBlind_pedestrians;
extern int gImmortal_peds;
extern int gPeds_suicidal;
extern int gDancing_peds;
extern int gPanicking_peds;
extern int gDrunk_pedestrians;
extern float gPed_gravity_multiplier;
extern float gMutant_speed;
// GLOBAL: CARMA2_HW 0x0058abe0
const double gDouble_0058abe0 = 6.9;

// GLOBAL: CARMA2_HW 0x0058abe8
const double gDouble_0058abe8 = 6.283185307179586;

// GLOBAL: CARMA2_HW 0x0058ac10
float gFLOAT_0058ac10 = 2.384185791015625e-7f;

// GLOBAL: CARMA2_HW 0x0058ac18
double gDOUBLE_0058ac18 = 1.0;

// GLOBAL: CARMA2_HW 0x0058ac20
float gFLOAT_0058ac20 = 1.0f;

// GLOBAL: CARMA2_HW 0x0058ac48
const double gDOUBLE_0058ac48 = 1.4285714285714286;

// GLOBAL: CARMA2_HW 0x0058ac40
const float gFLOAT_0058ac40 = 0.1f;

// GLOBAL: CARMA2_HW 0x0058ac8c
const float gFLOAT_0058ac8c = 25.0f;

// GLOBAL: CARMA2_HW 0x0058ac88
const float gFLOAT_0058ac88 = 0.025f;

// GLOBAL: CARMA2_HW 0x0058ac80
double gDOUBLE_0058ac80 = 0.1;

// GLOBAL: CARMA2_HW 0x0058ac00
const float gFLOAT_0058ac00 = 0.5f;

// GLOBAL: CARMA2_HW 0x0058ac0c
const float gFLOAT_0058ac0c = 0.0f;

// GLOBAL: CARMA2_HW 0x0058ac50
const float gBounce_rate_const = 1000.0f;

// GLOBAL: CARMA2_HW 0x0065feb0
const char* gRepeatability_names[4] = {
    "none",
    "singleshot",
    "repeated",
    "doitregardless",
};

// GLOBAL: CARMA2_HW 0x0065ebb0
const char* gFizzle_names[3] = {
    "CIRCLES",
    "SQUARES",
    "DIAMONDS",
};

// GLOBAL: CARMA2_HW 0x006a52d0
tSmashable_item_spec gPowerup_pickup_smashable;

// GLOBAL: CARMA2_HW 0x006a7ce0
tSmashable_item_spec gPowerup_respawn_smashable;

// GLOBAL: CARMA2_HW 0x006a3660
tSmashable_item_spec gUnknown_smashable_006a3660;

// GLOBAL: CARMA2_HW 0x006a0ac0
br_pixelmap* gFizzle_in[3];

// GLOBAL: CARMA2_HW 0x006a0950
int gFizzle_height;

// GLOBAL: CARMA2_HW 0x006a0ad0
int gNumber_of_powerups;

// GLOBAL: CARMA2_HW 0x006a0a54
extern tPowerup* gPowerup_array;

// GLOBAL: CARMA2_HW 0x0058ac60
const double gDOUBLE_0058ac60 = -1000.0;

// GLOBAL: CARMA2_HW 0x0058ac68
const double gDOUBLE_0058ac68 = 0.15;

// GLOBAL: CARMA2_HW 0x0058ac78
const double gDOUBLE_0058ac78 = 1000.0;

// GLOBAL: CARMA2_HW 0x0058ac70
const double gDOUBLE_0058ac70 = -0.15;



extern tNet_mode gNet_mode;
extern int gRecovery_voucher_count;
extern int gIt_or_fox;
extern int gThis_net_player_index;
extern br_material* oily_material;

void C2_HOOK_FASTCALL AddVouchers(int amount);
void C2_HOOK_FASTCALL RecalculateCarMassMomentOfInertia(tCar_spec* car);
// GLOBAL: CARMA2_HW 0x0074d1a4
extern int gNet_powerup_time_replacement;

void C2_HOOK_FASTCALL ScalePeds(br_vector3* scale);
void C2_HOOK_STDCALL ScalePedHeads(float head_scale);
void C2_HOOK_FASTCALL MakePedsEthereal(void);
void C2_HOOK_FASTCALL MakePedsNotEthereal(void);
int C2_HOOK_FASTCALL FindPedestrianNearCar(tCar_spec* car, br_scalar radius);
int C2_HOOK_FASTCALL NapalmPedestrianNearCar(tCar_spec* car);
int C2_HOOK_FASTCALL DoPedestrianRepulsifictor(tCar_spec* pCar, float pF0, float pF1, float pF2, float pF3);
// GLOBAL: CARMA2_HW 0x006a0948
int gNumber_of_powerup_respawn_specs;

// GLOBAL: CARMA2_HW 0x006a0a64
tPowerup_respawn_spec* gPowerup_respawn_specs;

// GLOBAL: CARMA2_HW 0x006a0ad8
void* gDrug_trip_data;

static void C2_HOOK_FASTCALL PaletteFuckedUpByDrugs(br_pixelmap* pPixelmap, int pOffset) {
    int i;

    (void)pPixelmap;
    if (pOffset == 0) {
        memcpy(gRender_palette->pixels, gDrug_trip_data, 256 * sizeof(tU32));
    }
    for (i = 1; i < 224; i++) {
        ((tU32*)gRender_palette->pixels)[i] = ((tU32*)gDrug_trip_data)[(i + pOffset) & 0xff];
    }
    ((tU32*)gRender_palette->pixels)[0] = ((tU32*)gDrug_trip_data)[0];
    for (i = 224; i < 256; i++) {
        ((tU32*)gRender_palette->pixels)[i] = ((tU32*)gDrug_trip_data)[i];
    }
}

#ifdef CARPOCALYPSE2_MATCHING
#endif
#ifdef CARPOCALYPSE2_MATCHING
#endif
#ifdef CARPOCALYPSE2_MATCHING
extern void C2_HOOK_FASTCALL ARPowerupLose(tPowerup* pPowerup, tCar_spec* pCar);
extern void C2_HOOK_FASTCALL AddRepulseLink(tU32 pTime, tCar_spec* pCar, undefined4 pField_0x24, br_vector3* pOrigin, br_vector3* pVictim, float pField_0x18, float pField_0x1c);
extern br_actor* C2_HOOK_CDECL BrActorRemove();
extern void C2_HOOK_FASTCALL CrashEarnings(tCar_spec* pCar1, tCar_spec* pCar2);
extern void C2_HOOK_FASTCALL DRPixelmapText(br_pixelmap* pPixelmap, int pX, int pY, const tDR_font* pFont, const char* pText, int pRight_edge);
extern int C2_HOOK_FASTCALL DRS3StartSound2(tS3_outlet *pOutlet, tS3_sound_id pSound, int pRepeats, int pLeft_volume, int pRight_volume, int pLeft_pitch, int pRight_pitch);
extern int C2_HOOK_FASTCALL DRS3StartSound3D(tS3_outlet* pOutlet, int pSound_id, const br_vector3* pInitial_position, const br_vector3* pInitial_velocity, int pRepeats, int pVolume, int pPitch, int pSpeed);
extern void C2_HOOK_FASTCALL DamageUnit(tCar_spec* pCar, tDamage_type pDamage_type, int pMagnitude);
extern void C2_HOOK_CDECL DimRectangle();
extern void C2_HOOK_FASTCALL DoCamouflageThing(tCar_spec* pCar);
extern void C2_HOOK_FASTCALL DoDamage(tCar_spec *pCar, tDamage_type pDamage_type, int pMagnitude);
extern void C2_HOOK_FASTCALL DoPowerupRespawnSmash(br_actor *pActor);
extern void C2_HOOK_FASTCALL EarnCredits(int pAmount);
extern float C2_HOOK_STDCALL FRandomBetween(float pA, float pB);
extern float C2_HOOK_STDCALL FRandomPosNeg(float pN);
extern void C2_HOOK_FASTCALL FUN_004d9d00(tPhysics_object* pObject, int pMode);
extern int C2_HOOK_FASTCALL GetCarCount(tVehicle_type pCategory);
extern tCar_spec* C2_HOOK_FASTCALL GetCarSpec(tVehicle_type pCategory, int pIndex);
extern const char* C2_HOOK_FASTCALL GetMiscString(int pIndex);
extern tU32 C2_HOOK_FASTCALL GetTotalTime(void);
extern void C2_HOOK_FASTCALL GotPowerupEarwig(tCar_spec* pCar, int pIndex, int pMessage, int pSelect_inventory);
extern int C2_HOOK_FASTCALL GotPowerupX(tCar_spec* pCar, int pIndex, int pArg3, int pMessage, tU32 pTime);
extern int C2_HOOK_FASTCALL IRandomBetween(int pA, int pB);
extern int C2_HOOK_FASTCALL IsItReallyOKThatWeDontMakeAnyEffortToProtectAnySadFuckersOutThereThatDontWishToSeeInnocentPeopleBlownToBitsByHighExplosiveMinesAndShells(void);
extern int C2_HOOK_FASTCALL KeyIsDown(int pKey_index);
extern void C2_HOOK_FASTCALL KeyboardPowerupFinished(tPowerup* pPowerup, int pSelect_new);
extern void C2_HOOK_FASTCALL MungeRepulseRays(tU32 pTime);
extern tNet_message* C2_HOOK_FASTCALL NetBuildGuaranteedMessage(tU8 pNet_message_type, int pOption);
extern int C2_HOOK_FASTCALL NetGuaranteedSendMessageToAllPlayers(tNet_game_details* pDetails, tNet_message* pMessage, void* pNotifyFail);
extern tNet_game_player_info* C2_HOOK_FASTCALL NetPlayerFromCar(tCar_spec *pCar);
extern int C2_HOOK_FASTCALL NewTextHeadupSlot(int pSlot_index, int pFlash_rate, int pLifetime, int pFont_index, const char* pText);
extern tU32 C2_HOOK_FASTCALL PDGetTotalTime(void);
extern int C2_HOOK_FASTCALL PHILAddActiveObject(tPhysics_object* pInfo, undefined4* pArg2, const br_vector3* pArg3, const br_vector3* pArg4);
extern void C2_HOOK_FASTCALL PHILRemoveFromParentChain(tPhysics_object* pObject);
extern int C2_HOOK_CDECL PHILSetObjectProperty();
extern int C2_HOOK_FASTCALL PercentageChance(int pC);
extern void C2_HOOK_FASTCALL PipeSinglePowerupRespawn(br_actor* pActor, int pPowerup_id);
extern int C2_HOOK_FASTCALL SetPedFall(tPedestrian* pPed, int pSpeed, int pMove1, int pMove2, tPhysics_object* pCollision_info);
extern void C2_HOOK_FASTCALL SetThisPedPhysicing(tPedestrian* pPed);
extern void C2_HOOK_FASTCALL TotallySpamTheModel(tCar_spec* pCar_spec, float pDamage);
extern void __cdecl _ftol();
extern char _CIfmod[];
extern br_uint_8 bl;
extern br_pixelmap* gBack_screen;
extern tCar_spec* gCloaked_cars[12];
extern int gCount_cloaked_cars;
extern int gCountdown;
extern int gCredit_multiplier;
extern int gCrush_deferred;
extern tGraf_data* gCurrent_graf_data;
extern tNet_game_details* gCurrent_net_game;
extern tDR_font gFonts[28];
extern tU32 gFrame_period;
extern tU32 gINT_00705b70;
extern int gInitial_powerup_slots[3];
extern int gInventory_cycling;
extern tU32 gInventory_timeout;
extern tPlayer_ID gLocal_net_ID;
extern br_actor* gNon_track_actor;
extern tU32 gPHIL_last_physics_tick;
extern tHeadup_icon gPickedup_powerups[20];
extern int gPickup_respawn_max_extra_time_ms;
extern int gPickup_respawn_min_time_ms;
extern float gPinball_factor;
extern tSlot_info gPotential_substitution_value_APO;
extern tProgram_state gProgram_state;
extern tU32 gRace_finished;
extern undefined4 gUnknown_00705b80[20];
extern tS3_outlet* gXXX_outlet;
extern br_vector3 gZero_v__car;
#endif
#ifdef CARPOCALYPSE2_MATCHING
extern tPhysics_joint* C2_HOOK_FASTCALL AllocatePhysicsJoint(int pCount_limits, int pType);
extern void C2_HOOK_FASTCALL BRPM_convert(br_pixelmap* pMap, int pPixel_type);
extern void C2_HOOK_FASTCALL BlendifyMaterial(br_material* pMaterial, int pPercent);
extern br_actor* C2_HOOK_CDECL BrActorAllocate();
extern br_pixelmap* C2_HOOK_CDECL BrMapAdd();
extern br_pixelmap* C2_HOOK_CDECL BrMapFind();
extern br_material* C2_HOOK_CDECL BrMaterialAdd();
extern br_material* C2_HOOK_CDECL BrMaterialAllocate();
extern void C2_HOOK_CDECL BrMaterialUpdate();
extern void C2_HOOK_CDECL BrMatrix34ApplyP();
extern void C2_HOOK_CDECL BrMatrix34Copy();
extern void C2_HOOK_CDECL BrMatrix34Translate();
extern void* C2_HOOK_CDECL BrMemAllocate();
extern br_model* C2_HOOK_CDECL BrModelAdd();
extern br_model* C2_HOOK_CDECL BrModelAllocate();
extern br_model* C2_HOOK_CDECL BrModelFind();
extern void C2_HOOK_CDECL BrVector3SetFloat();
extern void C2_HOOK_FASTCALL ClosePackFileAndSetTiffLoading(tTWTVFS twt);
extern void C2_HOOK_FASTCALL ConfigureShitMine(tShit_mine* pMine, tPowerup* pPowerup, tCar_spec* pCar);
extern br_actor* C2_HOOK_FASTCALL CreateBillBoard(br_pixelmap* pTexture);
extern int C2_HOOK_FASTCALL DRTextWidth(const tDR_font* pFont, const char* pText);
extern FILE* C2_HOOK_FASTCALL DRfopen(const char* pFilename, const char* pMode);
extern tFace_ref* C2_HOOK_FASTCALL FUN_004226d0(const br_vector3* pRay_pos, const br_vector3* pRay_dir, br_vector3* pHit_normal, br_scalar* pDistance, br_vector3* pHit_position, tFace_ref* pFaces, int pFace_count);
extern void C2_NORETURN C2_HOOK_CDECL FatalError();
extern char * C2_HOOK_FASTCALL GetALineAndDontArgue(FILE* pF, char* pS);
extern int C2_HOOK_FASTCALL GetALineAndInterpretCommand(FILE* pF, const char** pString_list, int pCount);
extern br_scalar C2_HOOK_FASTCALL GetAScalar(FILE* pF);
extern void C2_HOOK_FASTCALL GetAString(FILE* pF, char* pString);
extern int C2_HOOK_FASTCALL GetAnInt(FILE* pF);
extern int C2_HOOK_FASTCALL GetHierarchyNetworkSize(tPhysics_object* pObject);
extern int C2_HOOK_FASTCALL GetHierarchyNetworkStuff(tPhysics_object* pObject, tU8* pNet_data, int pRemaining);
extern void C2_HOOK_FASTCALL GetPairOfInts(FILE* pF, int* pF1, int* pF2);
extern void C2_HOOK_FASTCALL InitMineShit(void);
extern void C2_HOOK_FASTCALL InitializeMineMotion(tShit_mine* pMine, tCar_spec* pCar);
extern void C2_HOOK_FASTCALL LoadAllImagesInDirectory(tBrender_storage* pStorage_space, const char* path);
extern void C2_HOOK_FASTCALL LoadAllStuffInDirectory(tBrender_storage* pStorage, const char* pPath, tRendererShadingType pShading);
extern br_pixelmap* C2_HOOK_FASTCALL LoadPixelmap(const char* pPath_name);
extern tAdd_to_storage_result C2_HOOK_FASTCALL LoadSingleSound(tBrender_storage* pStorage_space, int pSound_id);
extern tPhysics_object* C2_HOOK_FAKE_THISCALL MungeBoxObject(br_model* pModel, undefined4 pArg2, float pMass);
extern tPhysics_object* C2_HOOK_FAKE_THISCALL MungeSphereObject(br_model* pModel, float pWeight);
extern tNet_message_chunk* C2_HOOK_FASTCALL NetGetToPlayerContents(tNet_message_type pType, tU32 pSize_decider, tPlayer_ID pPlayer_id);
extern tTWTVFS C2_HOOK_FASTCALL OpenPackFileAndSetTiffLoading(const char* path);
extern int C2_HOOK_FASTCALL PFfclose(FILE* pFile);
extern void C2_HOOK_FASTCALL PathCat(char* pDestn_str, const char* pStr_1, const char* pStr_2);
extern void C2_HOOK_FASTCALL PhysicsAddObject(tPhysics_object* pParent, tPhysics_object* pChild);
extern void C2_HOOK_FASTCALL PipeSingleMatrix34Type66(br_actor* pActor);
extern void C2_HOOK_FASTCALL PlayExclusiveFFBEffect(const char* pEffect_name, int pArg2);
extern void C2_HOOK_FASTCALL PrintPowerupIconIn3D(int pX, int pY, tHeadup_icon* pIcon, tPowerup* pPowerup, int pScale, tU32 pTime);
extern void C2_HOOK_FASTCALL ReadExplosion(FILE *pF, tExplosion_animation* pAnimation);
extern void C2_HOOK_FASTCALL ReadNonCarCuboidActivation(FILE* pF, tNon_car_cuboid_activations* pNon_car_cuboid_activations);
extern void C2_HOOK_FASTCALL ReadShrapnelSideEffects(FILE* pF, tShrapnel_side_effects* pShrapnel_side_effects);
extern void C2_HOOK_FASTCALL ReadShrapnelSpec(FILE* pF, tShrapnel_spec* pShrapnel_spec, int* pShrapnel_count);
extern void C2_HOOK_FASTCALL ReadSpillData(FILE *pF,tSlick_spec *pSlick_spec);
extern int C2_HOOK_FASTCALL SelectCrushNetwork(undefined4* pField, tPhysics_object* pObject, int pMode);
extern int gSensible_place_collect_active; // also written by TestForObjectInSensiblePlace (physics.c);
extern void C2_HOOK_FASTCALL TimerString(tU32 pTime, char* pStr, undefined4 pArg3, int pFudge_colon, int pFloat);
extern tPath_name gApplication_path;
extern br_actor* gCamera;
extern float gCamera_angle;
extern float gCosine_array[64];
extern int gCount_mutant_tail_parts;
extern int gDRFont_to_polyfont_mapping[24];
extern int gDisable_sound;
extern tFace_ref gFace_list__car[300];
extern int gGraf_spec_index;
extern tGraf_spec gGraf_specs[2];
extern tPhysics_object* gList_collision_infos;
extern float gMass_mine;
extern float gMass_mutant_tail_ball;
extern float gMass_mutant_tail_link;
extern tBrender_storage gMisc_storage_space;
extern tNet_game_player_info gNet_players[12];
extern char gNext_crush_actor_id[];
extern int gNumber_of_net_players;
extern char gPed_flags_0058f2e4[];
extern char gPowerup_respawn_explosion[];
extern char gPowerup_respawn_field_0x50[];
extern char gPowerup_respawn_sound_count[];
extern char gPowerup_respawn_sound_ids[];
extern char gPowerup_respawn_spill_time_offset[];
extern const char* gPowerup_txt_path;
extern tPhysics_object* gSensible_place_blocker;
extern tBrender_storage gTrack_storage_space;
extern tWorld_callbacks gWorld_callbacks;
#endif
// FUNCTION: CARMA2_HW 0x004dedf0
void C2_HOOK_FASTCALL SendCurrentPowerups(void) {
    tCar_spec* car;
    tNet_message_chunk* message;
    int locals[3];
    int player_id;
    int net_player_index;
    int powerup_index;
    int* powerup_time;
    int current_time;
    tNet_game_player_info* net_player;

    player_id = locals[2];
    locals[1] = 0;
    do {
        locals[0] = locals[1] == 0 ? 1 : GetCarCount(locals[1]);
        locals[2] = 0;
        if (locals[0] > 0) {
            do {
                if (locals[1] == 0) {
                    car = &gProgram_state.current_car;
                } else {
                    car = GetCarSpec(locals[1], locals[2]);
                }
                net_player_index = 0;
                if (gNumber_of_net_players > 0) {
                    net_player = gNet_players;
                    while (net_player_index < gNumber_of_net_players) {
                        if (net_player->car == car) {
                            player_id = gNet_players[net_player_index].ID;
                            break;
                        }
                        net_player_index++;
                        net_player++;
                    }
                }
                powerup_index = 0;
                if (gNumber_of_powerups > 0) {
                    powerup_time = car->powerups;
                    do {
                        if (*powerup_time != 0) {
                            message = NetStartBroadcastContents(eNet_message_chunk_type_powerup, 0);
                            if (message != NULL) {
                                message->powerup.field_0x10 = 1;
                                message->powerup.player_id = player_id;
                                message->powerup.powerup_index = powerup_index;
                                current_time = GetTotalTime();
                                message->powerup.time_remaining = *powerup_time - current_time;
                                NetBroadcastContents(message);
                            }
                        }
                        powerup_index++;
                        powerup_time++;
                    } while (powerup_index < gNumber_of_powerups);
                }
                locals[2]++;
            } while (locals[2] < locals[0]);
        }
        locals[1]++;
    } while (locals[1] <= 1);
}

// GLOBAL: CARMA2_HW 0x006a0a68
int gPissed_next_time;

// Amplitudes, periods, and phase offsets, each with twelve reserved slots.

// GLOBAL: CARMA2_HW 0x00705be4
float gPissed_flicker[36];

// GLOBAL: CARMA2_HW 0x00705c14
float gPissed_flicker_frames[2];

// GLOBAL: CARMA2_HW 0x00705c20
float gPissed_flicker_limit;

// GLOBAL: CARMA2_HW 0x00705c74
float gPissed_hold_time;

// GLOBAL: CARMA2_HW 0x0058ac90
double gConst_kangeroo_force_factor = 0.14492753623188406;

// GLOBAL: CARMA2_HW 0x006a0a50
tU8* gRace_powerup_respawn_bools;

// GLOBAL: CARMA2_HW 0x006a0a60
int gNet_powerup_time_between_goodies_min;

// GLOBAL: CARMA2_HW 0x006a090c
int gNet_powerup_time_between_goodies_max;

// GLOBAL: CARMA2_HW 0x006a0450
int gNet_count_powerup_goodies;

// GLOBAL: CARMA2_HW 0x006a094c
int* gNet_powerup_goodies;

// GLOBAL: CARMA2_HW 0x0065e910
tGot_proc* gGot_procs[56] = {
    GotCredits,
    SetPedSpeed,
    SetPedSize,
    SetPedExplode,
    PickAtRandom,
    SetInvulnerability,
    SetFreeRepairs,
    DoInstantRepair,
    FreezeTimer,
    SetEngineFactor,
    SetUnderwater,
    GotTimeOrPower,
    TrashBodywork,
    TakeDrugs,
    SetOpponentsSpeed,
    SetCopsSpeed,
    SetGravity,
    SetPinball,
    SetWallclimb,
    SetBouncey,
    SetSuspension,
    SetTyreGrip,
    SetDamageMultiplier,
    SetImmortalPeds,
    SetStupidPedestrians,
    SetSuicidalPedestrians,
    GotVouchers,
    SetMassMultiplier,
    SetInstantHandbrake,
    ShowPedestrians,
    HitMine,
    SetElectroBastard,
    SetPedHeadSize,
    SetMutantCorpses,
    ShitMine,
    PissOutOil,
    KangerooJump,
    AnnihilatePeds,
    RepulseOpponents,
    SetPedBrittle,
    SetGhostPeds,
    SetDancingPeds,
    SetPanickingPeds,
    SetLowGravityPeds,
    SetPissedPeds,
    RepulsePedestrians,
    GainAPO,
    GainAPOPotential,
    SetPissed,
    NapalmPeds,
    PedValium,
    PowerupCancel,
    GrowATail,
    CutOffTail,
    TurnOnCloaking,
    NULL,
};

// GLOBAL: CARMA2_HW 0x0065e9f0
tLose_proc* gLose_procs[56] = {
    NULL,
    ResetPedSpeed,
    ResetPedSize,
    ResetPedExplode,
    NULL,
    ResetInvulnerability,
    ResetFreeRepairs,
    NULL,
    UnfreezeTimer,
    ResetEngineFactor,
    ResetUnderwater,
    NULL,
    NULL,
    PukeDrugsBackUp,
    ResetOpponentsSpeed,
    ResetCopsSpeed,
    ResetGravity,
    ResetPinball,
    ResetWallclimb,
    ResetBouncey,
    ResetSuspension,
    ResetTyreGrip,
    ResetDamageMultiplier,
    ResetImmortalPeds,
    ResetStupidPedestrians,
    ResetSuicidalPeds,
    NULL,
    ResetMassMultiplier,
    ResetInstantHandbrake,
    HidePedestrians,
    NULL,
    ResetElectroBastard,
    ResetPedHeadSize,
    ResetMutantCorpses,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    ResetPedBrittle,
    ResetGhostPeds,
    ResetDancingPeds,
    ResetPanickingPeds,
    ResetLowGravityPeds,
    ResetPissedPeds,
    NULL,
    NULL,
    NULL,
    ResetPissed,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    TurnOffCloaking,
    NULL,
};

// GLOBAL: CARMA2_HW 0x0065ead0
tPeriodic_proc* gPeriodic_procs[56] = {
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    TheEffectsOfDrugs,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    DoBouncey,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    MungeVouchers,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    PissedEffect,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
};

// GLOBAL: CARMA2_HW 0x00705560
tShit_mine gShit_mines[20];

// GLOBAL: CARMA2_HW 0x006a0960
tRepulse_link gRepulse_links[6];

// GLOBAL: CARMA2_HW 0x006a0acc
tPhysics_joint* gMutant_tail_first_joint;

// GLOBAL: CARMA2_HW 0x006a0908
tPhysics_object* gMutant_tail_first_collision_info;

// GLOBAL: CARMA2_HW 0x00705540
int gMutant_tail_state;

// GLOBAL: CARMA2_HW 0x006a0ae0
br_model* gModel_powerup_armor;

// GLOBAL: CARMA2_HW 0x006a0ae4
br_model* gModel_powerup_power;

// GLOBAL: CARMA2_HW 0x006a0ae8
br_model* gModel_powerup_offence;

// GLOBAL: CARMA2_HW 0x006a0a70
int gInventory[20];

// GLOBAL: CARMA2_HW 0x006a0a6c
int gINT_006a0a6c;

// GLOBAL: CARMA2_HW 0x006a0a5c
int gINT_006a0a5c;

// GLOBAL: CARMA2_HW 0x006a0458
tRespawn_powerup gRespawn_powerups[100];

// GLOBAL: CARMA2_HW 0x006a0a58
int gInventory_selected = 0;

// GLOBAL: CARMA2_HW 0x006a0944
int gNumber_of_icons;

// GLOBAL: CARMA2_HW 0x006a0aec
tU32 gNext_goody_time;

// GLOBAL: CARMA2_HW 0x006a0958
undefined4 gDAT_006a0958;

// FUNCTION: CARMA2_HW 0x004df1e0
int C2_HOOK_FASTCALL GetPowerupCount(void) {
    return 0;
}

// FUNCTION: CARMA2_HW 0x004df1f0
void C2_HOOK_FASTCALL GetPowerupPosition(int pIndex, br_vector3* pPosition) {
}

void C2_HOOK_FASTCALL InitRepulseEffects(void) {
    br_pixelmap* repulse;
    int i;
    int j;

    repulse = LoadPixelmap("REPULSE.PIX");
    if (repulse == NULL) {
        FatalError(kFatalError_CantFindFile_S, "REPULSE.PIX");
    }
    BrMapAdd(repulse);

    C2_HOOK_BUG_ON(CARPOCALYPSE2_ASIZE(gRepulse_links) != 6);
    C2_HOOK_BUG_ON(sizeof(tRepulse_link) != 0x28);

    for (i = 0; i < CARPOCALYPSE2_ASIZE(gRepulse_links); i++) {
        tRepulse_link *link = &gRepulse_links[i];
        br_model *model;
        br_material *material;

        link->actor = BrActorAllocate(BR_ACTOR_MODEL, NULL);
        model = BrModelAllocate(NULL, 102, 100);
        model->flags |= BR_MODF_UPDATEABLE;
        material = BrMaterialAllocate("");
        material->colour_map = repulse;
        material->flags &= ~BR_MATF_LIGHT;
        material->flags |= BR_MATF_TWO_SIDED;
        BrMaterialAdd(material);
        BlendifyMaterial(material, 100);
        BrMaterialUpdate(material, BR_MATU_ALL);
        for (j = 0; j < model->nfaces / 2; j++) {
            model->faces[j].material = material;
            model->faces[j].vertices[0] = j + 0;
            model->faces[j].vertices[1] = j + 1;
            model->faces[j].vertices[2] = j + 1 + model->nvertices / 2;
        }
        for (j = model->nfaces / 2; j < model->nfaces; j++) {
            model->faces[j].material = material;
            model->faces[j].vertices[0] = j + 1;
            model->faces[j].vertices[1] = j + 2;
            model->faces[j].vertices[2] = j + 1 - model->nvertices / 2;
        }
        for (j = 0; j < model->nvertices / 2; j++) {
            model->vertices[j].p.v[0] = (float) (j % 2);
            model->vertices[j].p.v[1] = 0.f;
        }
        for (j = model->nvertices / 2; j < model->nvertices; j++) {
            model->vertices[j].p.v[0] = (float) (j % 2);
            model->vertices[j].p.v[1] = 1.f;
        }
        link->actor->model = model;
        link->actor->render_style = BR_RSTYLE_FACES;
        link->material = material;
        BrModelAdd(model);
    }
}

void C2_HOOK_FASTCALL InitTail(void) {
    int i;
    tPhysics_object* parent_collision_info;
    tPath_name the_path;
    tTWTVFS twt;

    parent_collision_info = NULL;
    PathCat(the_path, gApplication_path, "TAIL");
    twt = OpenPackFileAndSetTiffLoading(the_path);
    LoadAllStuffInDirectory(&gMisc_storage_space, the_path, kRendererShadingType_Specular);
    ClosePackFileAndSetTiffLoading(twt);
    for (i = 0; i < gCount_mutant_tail_parts; i++) {
        int is_link;
        tPhysics_object *collision_info;
        br_model *model;

        is_link = i != gCount_mutant_tail_parts - 1;
        if (is_link) {
            model = BrModelFind("TAILLINK");
            collision_info = MungeBoxObject(model CARPOCALYPSE2_THISCALL_EDX, gMass_mutant_tail_link);
        } else {
            model = BrModelFind("TAILBALL");
            collision_info = MungeSphereObject(model, gMass_mutant_tail_ball);
        }

        C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tPhysics_object, physics_joint1, 0x188);

        collision_info->flags_0x238 = 0x20;
        collision_info->physics_joint1 = AllocatePhysicsJoint(1, kMem_physics_joint);
        collision_info->physics_joint1->type = eJoint_ball_n_socket;
        BrVector3SetFloat(&collision_info->physics_joint1->field_0x08,
                          0.f, 0.f, model->bounds.min.v[2] * (is_link ? 0.99f : 0.45f));
        if (i == 0) {
            gMutant_tail_first_joint = collision_info->physics_joint1;
            gMutant_tail_first_collision_info = collision_info;

            C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tPhysics_joint, type, 0x00);
            C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tPhysics_joint, friction, 0x04);
            C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tPhysics_joint, field_0x08, 0x08);
            C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tPhysics_joint, hinge_axis2, 0x20);
            C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tPhysics_joint, hinge_axis3, 0x2c);
            C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tPhysics_joint, hinge_axis, 0x38);
            C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tPhysics_joint, count_limits, 0x50);
            C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tPhysics_joint, limits, 0x54);

            collision_info->physics_joint1->count_limits = 1;
            BrVector3SetFloat(&collision_info->physics_joint1->limits[0].child, 0.f, 0.f, 1.f);
            BrVector3SetFloat(&collision_info->physics_joint1->limits[0].parent, 0.f, 0.f, 1.f);
        } else {
            BrVector3SetFloat(&collision_info->physics_joint1->field_0x14,
                              0.f, 0.f, model->bounds.max.v[2] * (is_link ? 0.99f : 0.45f));
            PhysicsAddObject(parent_collision_info, collision_info);
        }
        BrVector3SetFloat(&collision_info->physics_joint1->hinge_axis2, 0.f, 0.f, 1.f);
        BrVector3SetFloat(&collision_info->physics_joint1->hinge_axis3, 0.f, 1.f, 0.f);
        BrVector3SetFloat(&collision_info->physics_joint1->hinge_axis, 1.f, 0.f, 0.f);
        BrMatrix34Translate(&collision_info->transform_matrix, 0.f, 0.f, 0.f);
        BrMatrix34Translate(&collision_info->actor->t.t.mat, 0.f, 0.f, 0.f);
        parent_collision_info = collision_info;
    }
    gMutant_tail_state = 0;
}

void C2_HOOK_FASTCALL RemoveRepulseEffect(tRepulse_link* pRepulse) {

    pRepulse->time = 0;
    if (pRepulse->actor->parent == gNon_track_actor) {
        BrActorRemove(pRepulse->actor);
    }
}

void C2_HOOK_FASTCALL ResetRepulseEffects(void) {
    int i;

    C2_HOOK_BUG_ON(CARPOCALYPSE2_ASIZE(gRepulse_links) != 6);

    for (i = 0; i < (int)CARPOCALYPSE2_ASIZE(gRepulse_links); i++) {
        RemoveRepulseEffect(&gRepulse_links[i]);
    }
}

// FUNCTION: CARMA2_HW 0x004da600
void C2_HOOK_FASTCALL EmptyInventory(void) {
    int i;

    C2_HOOK_BUG_ON(CARPOCALYPSE2_ASIZE(gInventory) != 20);

    for (i = 0; i < CARPOCALYPSE2_ASIZE(gInventory); i++) {
        gInventory[i] = -1;
    }
    gInventory_selected = -1;
    gINT_006a0a6c = 0;
    gINT_006a0a5c = 0;
}

// FUNCTION: CARMA2_HW 0x004da630
void C2_NAKED C2_HOOK_FASTCALL ResetPowerups(void) {
#ifndef CARPOCALYPSE2_MATCHING
    int i;

    C2_HOOK_BUG_ON(CARPOCALYPSE2_ASIZE(gRespawn_powerups) != 100);
    C2_HOOK_BUG_ON(sizeof(gRespawn_powerups[0]) != 0xc);

    i = (gNet_mode == eNet_mode_none) ? 0 : 1;
    gRace_powerup_respawn_bools = gPowerup_respawn_specs[i].bools;

    {
        tPowerup* powerup;
        for (i = 0, powerup = gPowerup_array; i < gNumber_of_powerups; i++, powerup++) {
            powerup->got_time = 0;
            powerup->current_value = -1;
        }
    }
    for (i = 0; i < (int)CARPOCALYPSE2_ASIZE(gRespawn_powerups); i++) {
        gRespawn_powerups[i].actor = NULL;
    }
    for (i = 0; i < (int)CARPOCALYPSE2_ASIZE(gPickedup_powerups); i++) {
        if (gPickedup_powerups[i].icon_actor != NULL) {
            gPickedup_powerups[i].icon_actor->render_style = BR_RSTYLE_NONE;
        }
    }
    EmptyInventory();

    gNumber_of_icons = 0;
    gDAT_006a0958 = 0;
    gInventory_cycling = 0;
    gInventory_timeout = 0;
    gCount_cloaked_cars = 0;
    ResetRepulseEffects();
    gNext_goody_time = 0;
#else
    __asm {
            mov ecx, dword ptr [gNet_mode]
            push ebx
            xor ebx, ebx
            xor eax, eax
            cmp ecx, ebx
            mov ecx, dword ptr [gPowerup_respawn_specs]
            setne al
            push esi
            push edi
            mov edx, dword ptr [ecx + eax*4]
            mov eax, dword ptr [gNumber_of_powerups]
            xor ecx, ecx
            cmp eax, ebx
            mov dword ptr [gRace_powerup_respawn_bools], edx
            jle L004da67d
            mov eax, dword ptr [gPowerup_array]
            add eax, 0x28
    L004da664:
            mov dword ptr [eax - 0x24], ebx
            mov dword ptr [eax], 0xffffffff
            mov edx, dword ptr [gNumber_of_powerups]
            inc ecx
            add eax, 0xac
            cmp ecx, edx
            jl L004da664
    L004da67d:
            mov eax, offset gRespawn_powerups
    L004da682:
            mov dword ptr [eax], ebx
            add eax, 0xc
            cmp eax, offset gMutant_tail_first_collision_info
            jl L004da682
            mov ecx, offset gPickedup_powerups+0x10
            mov dl, 1
    L004da695:
            mov eax, dword ptr [ecx]
            cmp eax, ebx
            je L004da69e
            mov byte ptr [eax + 0x20], dl
    L004da69e:
            add ecx, 0x14
            cmp ecx, offset gUnknown_00705b80
            jl L004da695
            mov ecx, 0x14
            or eax, 0xffffffff
            mov edi, offset gInventory
            mov dword ptr [gInventory_selected], 0xffffffff
            rep stosd
            mov dword ptr [gINT_006a0a6c], ebx
            mov dword ptr [gINT_006a0a5c], ebx
            mov dword ptr [gNumber_of_icons], ebx
            mov dword ptr [gDAT_006a0958], ebx
            mov dword ptr [gInventory_cycling], ebx
            mov dword ptr [gInventory_timeout], ebx
            mov dword ptr [gCount_cloaked_cars], ebx
            mov esi, offset gRepulse_links+0xc
    L004da6f1:
            mov edx, dword ptr [gNon_track_actor]
            mov dword ptr [esi - 0xc], ebx
            mov eax, dword ptr [esi]
            mov ecx, dword ptr [eax + 0xc]
            cmp ecx, edx
            jne L004da70c
            push eax
            call BrActorRemove
            add esp, 4
    L004da70c:
            add esi, 0x28
            cmp esi, offset gINT_006a0a5c
            jl L004da6f1
            pop edi
            mov dword ptr [gNext_goody_time], ebx
            pop esi
            pop ebx
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
            nop
            nop
            nop
            nop
    }
#endif
}

static br_pixelmap* DRMapFindMeAPixie(const char* name) {
    char s[256];
    br_pixelmap* map;

    map = BrMapFind(name);
    if (map != NULL) {
        return map;
    }
    strcpy(s, name);
    strcat(s, ".PIX");
    map = BrMapFind(s);
    if (map != NULL) {
        return map;
    }
    strcpy(s, name);
    strcat(s, ".pix");
    map = BrMapFind(s);
    if (map != NULL) {
        return map;
    }
    return NULL;
}

// FUNCTION: CARMA2_HW 0x004d96c0
void C2_NAKED C2_HOOK_FASTCALL LoadPowerups(void) {
#ifndef CARPOCALYPSE2_MATCHING
    FILE* f;
    tPath_name the_path;
    tTWTVFS twtvfs;
    int action_index;
    int first;
    int i, j, k;
    int time;
    int count_index_groups;
    int index_range;
    char s[256];
    tPowerup* the_powerup;

    C2_HOOK_BUG_ON(sizeof(tPowerup) != 172);

    if (gPowerup_txt_path == NULL) {
        SetDefaultPowerupFilename();
    }
    PathCat(the_path, gApplication_path, gGraf_specs[gGraf_spec_index].data_dir_name);
    PathCat(the_path, the_path, "POWERUPS");
    twtvfs = OpenPackFileAndSetTiffLoading(the_path);
    LoadAllImagesInDirectory(&gMisc_storage_space, the_path);
    ClosePackFileAndSetTiffLoading(twtvfs);

    for (i = 0; i < CARPOCALYPSE2_ASIZE(gFizzle_in); i++) {
        gFizzle_in[i] = DRMapFindMeAPixie(gFizzle_names[i]);
    }
    gFizzle_height = gFizzle_in[0]->height / 4;

    PathCat(the_path, gApplication_path, gPowerup_txt_path);
    f = DRfopen(the_path, "rt");
    if (f == NULL) {
        FatalError(kFatalError_CannotLoadCarResolutionIndependentFile);
    }
    /* Number of powerups */
    gNumber_of_powerups = GetAnInt(f);
    gPowerup_array = BrMemAllocate(sizeof(tPowerup) * gNumber_of_powerups, kMem_powerup_array);
    for (i = 0; i < gNumber_of_powerups; i++) {
        the_powerup = &gPowerup_array[i];

        GetALineAndDontArgue(f, the_powerup->message);
        if (strcmp(the_powerup->message, "dummy") == 0) {
            the_powerup->type = ePowerup_dummy;
        } else {
            if (strcmp(the_powerup->message, "n/a") == 0) {
                the_powerup->message[0] = '\0';
            }
            GetAString(f, s);
            the_powerup->icon = DRMapFindMeAPixie(s);
            if (the_powerup->icon != NULL) {
                BrMapAdd(the_powerup->icon);
                BRPM_convert(the_powerup->icon, gBack_screen->type);
            }
            if (the_powerup->icon != NULL) {
                the_powerup->icon_actor = CreateBillBoard(the_powerup->icon);
                the_powerup->icon_actor->material->extra_prim = the_powerup->material_prims;
            }

            /* Fizzle type */
            the_powerup->fizzle_type = GetAnInt(f);
            /* Time limit (-1 instantaneous, 0 whole race, x seconds) */
            time = 1000 * GetAnInt(f);
            the_powerup->duration = time;
            if (time < 0) {
                the_powerup->type = ePowerup_instantaneous;
            } else if (time == 0) {
                the_powerup->type = ePowerup_whole_race;
            } else {
                the_powerup->type = ePowerup_timed;
            }
            /* Initial value for keyboard operable powerups
             * (zero if not key operable,
             *  positive if time in seconds,
             *  negative if number of uses)
             */
            the_powerup->initial_value = GetAnInt(f);
            if (the_powerup->initial_value > 0) {
                the_powerup->initial_value *= 1000;
            }
            /* Action index */
            action_index = GetAnInt(f);
            if (action_index >= 0) {
                the_powerup->got_proc = gGot_procs[action_index];
                the_powerup->lose_proc = gLose_procs[action_index];
                the_powerup->periodic_proc = gPeriodic_procs[action_index];
            } else {
                the_powerup->lose_proc = NULL;
                the_powerup->periodic_proc = NULL;
                the_powerup->got_proc = NULL;
            }
            /*  Number of floating point params */
            the_powerup->number_of_float_params = GetAnInt(f);
            the_powerup->float_params = BrMemAllocate(sizeof(float) * the_powerup->number_of_float_params, kMem_powerup_parms);
            for (j = 0; j < the_powerup->number_of_float_params; j++) {
                the_powerup->float_params[j] = GetAFloat(f);
            }
            /* Number of integer params */
            the_powerup->number_of_integer_params = GetAnInt(f);
            the_powerup->integer_params = BrMemAllocate(sizeof(int) * the_powerup->number_of_integer_params, kMem_powerup_parms);
            for (j = 0; j < the_powerup->number_of_integer_params; j++) {
                the_powerup->integer_params[j] = GetAnInt(f);
            }
            /* Group inclusion */
            the_powerup->group_inclusion = GetAnInt(f);
            /* Pratcam sequence */
            the_powerup->prat_cam_event = GetAnInt(f);
            /* Flags (1 = net_global, 2 = net_inappropriate, 4 = off_before_snapweld) */
            the_powerup->net_type = GetAnInt(f);
        }
    }

    /* RESPAWN SPECS */

    /* Number of respawn specs */
    gNumber_of_powerup_respawn_specs = GetAnInt(f);
    gPowerup_respawn_specs = BrMemAllocate(gNumber_of_powerup_respawn_specs * sizeof(tPowerup_respawn_spec), kMem_powerup_array);
    for (i = 0; i < gNumber_of_powerup_respawn_specs; i++) {
        gPowerup_respawn_specs[i].bools = BrMemAllocate(gNumber_of_powerups * sizeof(tU8), kMem_powerup_array);
        /* Number of index groups */
        count_index_groups = GetAnInt(f);
        for (j = 0; j < count_index_groups; j++) {
            index_range = GetAnInt(f);
            if (index_range >= 0) {
                /* index_range is index */
                gPowerup_respawn_specs[i].bools[index_range] = 1;
            } else {
                /* index_range is range */
                first = GetAnInt(f);
                for (k = first; k < first - index_range; k++) {
                    gPowerup_respawn_specs[i].bools[k] = 1;
                }
            }
        }
    }
    gRace_powerup_respawn_bools = gPowerup_respawn_specs[0].bools;

    /* NETWORK AUTO-GOODIES */

    /* Min,max times between Auto-goodies */
    GetPairOfInts(f, &gNet_powerup_time_between_goodies_min, &gNet_powerup_time_between_goodies_max);
    gNet_powerup_time_between_goodies_min *= 1000;
    gNet_powerup_time_between_goodies_max *= 1000;
    /* Number of goodies to choose from */
    gNet_count_powerup_goodies = GetAnInt(f);
    gNet_powerup_goodies = BrMemAllocate(gNet_count_powerup_goodies * sizeof(int), kMem_powerup_array);
    for (i = 0; i < gNet_count_powerup_goodies; i++) {
        gNet_powerup_goodies[i] = GetAnInt(f);
    }
    PFfclose(f);
#else
    __asm {
            mov eax, dword ptr [gPowerup_txt_path]
            sub esp, 0x304
            test eax, eax
            push ebx
            push ebp
            push esi
            push edi
            jne L004d96dd
            mov dword ptr [gPowerup_txt_path], 0x65ec10
    L004d96dd:
            mov eax, dword ptr [gGraf_spec_index]
            lea ecx, [eax + eax*2]
            lea edx, [eax + ecx*4]
            lea ecx, [esp + 0x114]
            mov eax, dword ptr [edx*4 + gGraf_specs+0x1c]
            mov edx, offset gApplication_path
            push eax
            call PathCat
            lea edx, [esp + 0x114]
            lea ecx, [esp + 0x114]
            push offset gMass_mine+0x48
            call PathCat
            lea ecx, [esp + 0x114]
            call OpenPackFileAndSetTiffLoading
            lea edx, [esp + 0x114]
            mov ecx, offset gMisc_storage_space
            mov esi, eax
            call LoadAllImagesInDirectory
            mov ecx, esi
            call ClosePackFileAndSetTiffLoading
            xor ebp, ebp
    L004d9741:
            mov ebx, dword ptr [ebp + gFizzle_names]
            push ebx
            call BrMapFind
            add esp, 4
            test eax, eax
            jne L004d9814
            mov edi, ebx
            or ecx, 0xffffffff
            repne scasb
            not ecx
            sub edi, ecx
            lea edx, [esp + 0x14]
            mov eax, ecx
            mov esi, edi
            mov edi, edx
            lea edx, [esp + 0x14]
            shr ecx, 2
            rep movsd
            mov ecx, eax
            xor eax, eax
            and ecx, 3
            rep movsb
            mov edi, offset gDRFont_to_polyfont_mapping+0x314
            or ecx, 0xffffffff
            repne scasb
            not ecx
            sub edi, ecx
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
            lea eax, [esp + 0x14]
            and ecx, 3
            push eax
            rep movsb
            call BrMapFind
            add esp, 4
            test eax, eax
            jne L004d9814
            mov edi, ebx
            or ecx, 0xffffffff
            repne scasb
            not ecx
            sub edi, ecx
            lea edx, [esp + 0x14]
            mov eax, ecx
            mov esi, edi
            mov edi, edx
            lea edx, [esp + 0x14]
            shr ecx, 2
            rep movsd
            mov ecx, eax
            xor eax, eax
            and ecx, 3
            rep movsb
            mov edi, offset gMass_mine+0x40
            or ecx, 0xffffffff
            repne scasb
            not ecx
            sub edi, ecx
            mov esi, edi
            mov ebx, ecx
            mov edi, edx
            or ecx, 0xffffffff
            repne scasb
            mov ecx, ebx
            dec edi
            shr ecx, 2
            rep movsd
            mov ecx, ebx
            lea eax, [esp + 0x14]
            and ecx, 3
            push eax
            rep movsb
            call BrMapFind
            add esp, 4
    L004d9814:
            mov dword ptr [ebp + gFizzle_in], eax
            add ebp, 4
            cmp ebp, 0xc
            jl L004d9741
            mov edx, dword ptr [gFizzle_in]
            mov eax, dword ptr [gPowerup_txt_path]
            xor ecx, ecx
            push eax
            mov cx, word ptr [edx + 0x36]
            mov edx, offset gApplication_path
            shr ecx, 2
            mov dword ptr [gFizzle_height], ecx
            lea ecx, [esp + 0x118]
            call PathCat
            mov edx, offset gNext_crush_actor_id+0x180
            lea ecx, [esp + 0x114]
            call DRfopen
            mov ebx, eax
            test ebx, ebx
            jne L004d9873
            push 0x19
            call FatalError
            add esp, 4
    L004d9873:
            mov ecx, ebx
            call GetAnInt
            mov ecx, eax
            mov dword ptr [gNumber_of_powerups], eax
            shl ecx, 3
            sub ecx, eax
            push 0xc5
            lea ecx, [ecx + ecx*2]
            lea edx, [eax + ecx*2]
            shl edx, 2
            push edx
            call BrMemAllocate
            mov ecx, dword ptr [gNumber_of_powerups]
            add esp, 8
            test ecx, ecx
            mov dword ptr [gPowerup_array], eax
            mov dword ptr [esp + 0x10], 0
            jle L004d9bad
            lea ebp, [eax + 0x14]
    L004d98bb:
            lea edi, [ebp + 0x3c]
            mov ecx, ebx
            mov edx, edi
            call GetALineAndDontArgue
            mov esi, offset gMass_mine+0x38
            mov eax, edi
    L004d98ce:
            mov dl, byte ptr [eax]
            mov cl, dl
            cmp dl, byte ptr [esi]
            jne L004d98f2
            test cl, cl
            je L004d98ee
            mov dl, byte ptr [eax + 1]
            mov cl, dl
            cmp dl, byte ptr [esi + 1]
            jne L004d98f2
            add eax, 2
            add esi, 2
            test cl, cl
            jne L004d98ce
    L004d98ee:
            xor eax, eax
            jmp L004d98f7
    L004d98f2:
            sbb eax, eax
            sbb eax, -1
    L004d98f7:
            test eax, eax
            jne L004d9903
            mov dword ptr [ebp - 0x14], eax
            jmp L004d9b90
    L004d9903:
            mov esi, offset gMass_mine+0x34
            mov eax, edi
    L004d990a:
            mov dl, byte ptr [eax]
            mov cl, dl
            cmp dl, byte ptr [esi]
            jne L004d992e
            test cl, cl
            je L004d992a
            mov dl, byte ptr [eax + 1]
            mov cl, dl
            cmp dl, byte ptr [esi + 1]
            jne L004d992e
            add eax, 2
            add esi, 2
            test cl, cl
            jne L004d990a
    L004d992a:
            xor eax, eax
            jmp L004d9933
    L004d992e:
            sbb eax, eax
            sbb eax, -1
    L004d9933:
            test eax, eax
            jne L004d9939
            mov byte ptr [edi], al
    L004d9939:
            lea edx, [esp + 0x214]
            mov ecx, ebx
            call GetAString
            lea eax, [esp + 0x214]
            push eax
            call BrMapFind
            add esp, 4
            test eax, eax
            jne L004d9a25
            lea edi, [esp + 0x214]
            or ecx, 0xffffffff
            repne scasb
            not ecx
            sub edi, ecx
            lea edx, [esp + 0x14]
            mov eax, ecx
            mov esi, edi
            mov edi, edx
            lea edx, [esp + 0x14]
            shr ecx, 2
            rep movsd
            mov ecx, eax
            xor eax, eax
            and ecx, 3
            rep movsb
            mov edi, offset gDRFont_to_polyfont_mapping+0x314
            or ecx, 0xffffffff
            repne scasb
            not ecx
            sub edi, ecx
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
            lea eax, [esp + 0x14]
            and ecx, 3
            push eax
            rep movsb
            call BrMapFind
            add esp, 4
            test eax, eax
            jne L004d9a25
            lea edi, [esp + 0x214]
            or ecx, 0xffffffff
            repne scasb
            not ecx
            sub edi, ecx
            lea edx, [esp + 0x14]
            mov eax, ecx
            mov esi, edi
            mov edi, edx
            lea edx, [esp + 0x14]
            shr ecx, 2
            rep movsd
            mov ecx, eax
            xor eax, eax
            and ecx, 3
            rep movsb
            mov edi, offset gMass_mine+0x40
            or ecx, 0xffffffff
            repne scasb
            not ecx
            sub edi, ecx
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
            lea eax, [esp + 0x14]
            and ecx, 3
            push eax
            rep movsb
            call BrMapFind
            add esp, 4
    L004d9a25:
            xor esi, esi
            mov dword ptr [ebp], eax
            cmp eax, esi
            je L004d9a4a
            push eax
            call BrMapAdd
            mov ecx, dword ptr [gBack_screen]
            xor edx, edx
            add esp, 4
            mov dl, byte ptr [ecx + 0x2c]
            mov ecx, dword ptr [ebp]
            call BRPM_convert
    L004d9a4a:
            mov eax, dword ptr [ebp]
            cmp eax, esi
            je L004d9a67
            mov ecx, eax
            call CreateBillBoard
            mov dword ptr [ebp + 0x7c], eax
            mov eax, dword ptr [eax + 0x1c]
            lea edx, [ebp + 0x80]
            mov dword ptr [eax + 0x58], edx
    L004d9a67:
            mov ecx, ebx
            call GetAnInt
            mov ecx, ebx
            mov dword ptr [ebp + 4], eax
            call GetAnInt
            lea eax, [eax + eax*4]
            lea eax, [eax + eax*4]
            lea eax, [eax + eax*4]
            shl eax, 3
            mov dword ptr [ebp - 0xc], eax
            jns L004d9a92
            mov dword ptr [ebp - 0x14], 1
            jmp L004d9aa6
    L004d9a92:
            cmp eax, esi
            jne L004d9a9f
            mov dword ptr [ebp - 0x14], 3
            jmp L004d9aa6
    L004d9a9f:
            mov dword ptr [ebp - 0x14], 2
    L004d9aa6:
            mov ecx, ebx
            call GetAnInt
            cmp eax, esi
            mov dword ptr [ebp + 0x20], eax
            jle L004d9ac3
            lea eax, [eax + eax*4]
            lea eax, [eax + eax*4]
            lea ecx, [eax + eax*4]
            shl ecx, 3
            mov dword ptr [ebp + 0x20], ecx
    L004d9ac3:
            mov ecx, ebx
            call GetAnInt
            cmp eax, esi
            jge L004d9ad9
            mov dword ptr [ebp + 0x28], esi
            mov dword ptr [ebp + 0x2c], esi
            mov dword ptr [ebp + 0x30], esi
            jmp L004d9af7
    L004d9ad9:
            mov edx, dword ptr [eax*4 + gGot_procs]
            mov dword ptr [ebp + 0x28], edx
            mov ecx, dword ptr [eax*4 + gLose_procs]
            mov dword ptr [ebp + 0x2c], ecx
            mov edx, dword ptr [eax*4 + gPeriodic_procs]
            mov dword ptr [ebp + 0x30], edx
    L004d9af7:
            mov ecx, ebx
            call GetAnInt
            mov dword ptr [ebp + 8], eax
            shl eax, 2
            push 0xc6
            push eax
            call BrMemAllocate
            mov dword ptr [ebp + 0x34], eax
            mov eax, dword ptr [ebp + 8]
            add esp, 8
            test eax, eax
            jle L004d9b30
    L004d9b1c:
            mov ecx, ebx
            call GetAScalar
            mov ecx, dword ptr [ebp + 0x34]
            inc esi
            fstp dword ptr [ecx + esi*4 - 4]
            cmp esi, dword ptr [ebp + 8]
            jl L004d9b1c
    L004d9b30:
            mov ecx, ebx
            call GetAnInt
            lea edx, [eax*4]
            mov dword ptr [ebp + 0xc], eax
            push 0xc6
            push edx
            call BrMemAllocate
            mov dword ptr [ebp + 0x10], eax
            mov eax, dword ptr [ebp + 0xc]
            add esp, 8
            xor esi, esi
            test eax, eax
            jle L004d9b71
    L004d9b5b:
            mov ecx, ebx
            call GetAnInt
            mov ecx, dword ptr [ebp + 0x10]
            inc esi
            mov dword ptr [ecx + esi*4 - 4], eax
            mov eax, dword ptr [ebp + 0xc]
            cmp esi, eax
            jl L004d9b5b
    L004d9b71:
            mov ecx, ebx
            call GetAnInt
            mov ecx, ebx
            mov word ptr [ebp - 4], ax
            call GetAnInt
            mov ecx, ebx
            mov dword ptr [ebp + 0x18], eax
            call GetAnInt
            mov byte ptr [ebp + 0x24], al
    L004d9b90:
            mov eax, dword ptr [esp + 0x10]
            mov ecx, dword ptr [gNumber_of_powerups]
            inc eax
            add ebp, 0xac
            cmp eax, ecx
            mov dword ptr [esp + 0x10], eax
            jl L004d98bb
    L004d9bad:
            mov ecx, ebx
            call GetAnInt
            lea edx, [eax*4]
            mov dword ptr [gNumber_of_powerup_respawn_specs], eax
            push 0xc5
            push edx
            call BrMemAllocate
            mov dword ptr [gPowerup_respawn_specs], eax
            mov eax, dword ptr [gNumber_of_powerup_respawn_specs]
            add esp, 8
            xor edi, edi
            test eax, eax
            jle L004d9c51
    L004d9bde:
            mov eax, dword ptr [gNumber_of_powerups]
            push 0xc5
            push eax
            call BrMemAllocate
            mov ecx, dword ptr [gPowerup_respawn_specs]
            add esp, 8
            mov dword ptr [ecx + edi*4], eax
            mov ecx, ebx
            call GetAnInt
            test eax, eax
            jle L004d9c47
            mov ebp, eax
    L004d9c07:
            mov ecx, ebx
            call GetAnInt
            mov esi, eax
            test esi, esi
            jge L004d9c38
            mov ecx, ebx
            call GetAnInt
            mov ecx, eax
            sub ecx, esi
            cmp eax, ecx
            jge L004d9c44
    L004d9c23:
            mov edx, dword ptr [gPowerup_respawn_specs]
            inc eax
            cmp eax, ecx
            mov edx, dword ptr [edx + edi*4]
            mov byte ptr [edx + eax - 1], 1
            jl L004d9c23
            jmp L004d9c44
    L004d9c38:
            mov eax, dword ptr [gPowerup_respawn_specs]
            mov ecx, dword ptr [eax + edi*4]
            mov byte ptr [ecx + esi], 1
    L004d9c44:
            dec ebp
            jne L004d9c07
    L004d9c47:
            mov eax, dword ptr [gNumber_of_powerup_respawn_specs]
            inc edi
            cmp edi, eax
            jl L004d9bde
    L004d9c51:
            mov edx, dword ptr [gPowerup_respawn_specs]
            push offset gNet_powerup_time_between_goodies_max
            mov ecx, ebx
            mov eax, dword ptr [edx]
            mov edx, offset gNet_powerup_time_between_goodies_min
            mov dword ptr [gRace_powerup_respawn_bools], eax
            call GetPairOfInts
            mov eax, dword ptr [gNet_powerup_time_between_goodies_min]
            lea eax, [eax + eax*4]
            lea eax, [eax + eax*4]
            lea ecx, [eax + eax*4]
            mov eax, dword ptr [gNet_powerup_time_between_goodies_max]
            shl ecx, 3
            lea eax, [eax + eax*4]
            mov dword ptr [gNet_powerup_time_between_goodies_min], ecx
            mov ecx, ebx
            lea eax, [eax + eax*4]
            lea edx, [eax + eax*4]
            shl edx, 3
            mov dword ptr [gNet_powerup_time_between_goodies_max], edx
            call GetAnInt
            mov dword ptr [gNet_count_powerup_goodies], eax
            shl eax, 2
            push 0xc5
            push eax
            call BrMemAllocate
            mov dword ptr [gNet_powerup_goodies], eax
            mov eax, dword ptr [gNet_count_powerup_goodies]
            add esp, 8
            xor esi, esi
            test eax, eax
            jle L004d9ce5
    L004d9cca:
            mov ecx, ebx
            call GetAnInt
            mov ecx, dword ptr [gNet_powerup_goodies]
            inc esi
            mov dword ptr [ecx + esi*4 - 4], eax
            mov eax, dword ptr [gNet_count_powerup_goodies]
            cmp esi, eax
            jl L004d9cca
    L004d9ce5:
            mov ecx, ebx
            call PFfclose
            pop edi
            pop esi
            pop ebp
            pop ebx
            add esp, 0x304
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

// FUNCTION: CARMA2_HW 0x004d9ea0
void C2_NAKED C2_HOOK_FASTCALL InitPowerups(void) {
#ifndef CARPOCALYPSE2_MATCHING
    br_pixelmap* repulse;
    int i;
    int j;

    repulse = LoadPixelmap("REPULSE.PIX");
    if (repulse == 0) {
        FatalError(kFatalError_CantFindFile_S, "REPULSE.PIX");
    }
    BrMapAdd(repulse);

    C2_HOOK_BUG_ON(CARPOCALYPSE2_ASIZE(gRepulse_links) != 6);
    C2_HOOK_BUG_ON(sizeof(tRepulse_link) != 0x28);

    for (i = 0; i < 6; i++) {
        tRepulse_link *link = &gRepulse_links[i];
        br_model *model;
        br_material *material;
        br_face *face;
        br_vertex *vertex;

        link->actor = BrActorAllocate(BR_ACTOR_MODEL, 0);
        model = BrModelAllocate(0, 102, 100);
        model->flags |= BR_MODF_UPDATEABLE;
        material = BrMaterialAllocate("");
        material->colour_map = repulse;
        material->flags &= ~BR_MATF_LIGHT;
        material->flags |= BR_MATF_TWO_SIDED;
        BrMaterialAdd(material);
        BlendifyMaterial(material, 100);
        BrMaterialUpdate(material, BR_MATU_ALL);
        face = model->faces;
        for (j = 0; j < model->nfaces / 2; j++) {
            face->material = material;
            face->vertices[0] = j + 0;
            face->vertices[1] = j + 1;
            face->vertices[2] = j + 1 + model->nvertices / 2;
            face++;
        }
        for (j = model->nfaces / 2; j < model->nfaces; j++) {
            face->material = material;
            face->vertices[0] = j + 1;
            face->vertices[1] = j + 2;
            face->vertices[2] = j + 1 - model->nvertices / 2;
            face++;
        }
        vertex = model->vertices;
        for (j = 0; j < model->nvertices / 2; j++) {
            vertex->map.v[0] = (float) (j & 1);
            vertex->map.v[1] = 0.f;
            vertex++;
        }
        for (j = model->nvertices / 2; j < model->nvertices; j++) {
            vertex->map.v[0] = (float) (j & 1);
            vertex->map.v[1] = 1.f;
            vertex++;
        }
        link->actor->model = model;
        link->actor->render_style = BR_RSTYLE_FACES;
        link->material = material;
        BrModelAdd(model);
    }

    {
        int i;
        tPhysics_object* parent_collision_info;
        tPath_name the_path;
        tTWTVFS twt;

        parent_collision_info = NULL;
        PathCat(the_path, gApplication_path, "TAIL");
        twt = OpenPackFileAndSetTiffLoading(the_path);
        LoadAllStuffInDirectory(&gMisc_storage_space, the_path, kRendererShadingType_Specular);
        ClosePackFileAndSetTiffLoading(twt);
        for (i = 0; i < gCount_mutant_tail_parts; i++) {
            int is_last;
            tPhysics_object *collision_info;
            br_model *model;

            is_last = i == gCount_mutant_tail_parts - 1;
            if (is_last) {
                model = BrModelFind("TAILBALL");
                collision_info = MungeSphereObject(model, gMass_mutant_tail_ball);
            } else {
                model = BrModelFind("TAILLINK");
                collision_info = MungeBoxObject(model CARPOCALYPSE2_THISCALL_EDX, gMass_mutant_tail_link);
            }

            C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tPhysics_object, physics_joint1, 0x188);

            collision_info->flags_0x238 = 0x20;
            collision_info->physics_joint1 = AllocatePhysicsJoint(1, kMem_physics_joint);
            collision_info->physics_joint1->type = eJoint_ball_n_socket;
            BrVector3SetFloat(&collision_info->physics_joint1->field_0x08,
                              0.f, 0.f, model->bounds.min.v[2] * (is_last ? 0.45 : 0.99));
            if (i != 0) {
                BrVector3SetFloat(&collision_info->physics_joint1->field_0x14,
                                  0.f, 0.f, model->bounds.max.v[2] * (is_last ? 0.45 : 0.99));
                PhysicsAddObject(parent_collision_info, collision_info);
            } else {
                gMutant_tail_first_collision_info = collision_info;
                gMutant_tail_first_joint = collision_info->physics_joint1;

                C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tPhysics_joint, type, 0x00);
                C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tPhysics_joint, friction, 0x04);
                C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tPhysics_joint, field_0x08, 0x08);
                C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tPhysics_joint, hinge_axis2, 0x20);
                C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tPhysics_joint, hinge_axis3, 0x2c);
                C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tPhysics_joint, hinge_axis, 0x38);
                C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tPhysics_joint, count_limits, 0x50);
                C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tPhysics_joint, limits, 0x54);

                collision_info->physics_joint1->count_limits = 1;
                BrVector3SetFloat(&collision_info->physics_joint1->limits[0].child, 0.f, 0.f, 1.f);
                BrVector3SetFloat(&collision_info->physics_joint1->limits[0].parent, 0.f, 0.f, 1.f);
            }
            BrVector3SetFloat(&collision_info->physics_joint1->hinge_axis2, 0.f, 0.f, 1.f);
            BrVector3SetFloat(&collision_info->physics_joint1->hinge_axis3, 0.f, 1.f, 0.f);
            BrVector3SetFloat(&collision_info->physics_joint1->hinge_axis, 1.f, 0.f, 0.f);
            BrMatrix34Translate(&collision_info->transform_matrix, 0.f, 0.f, 0.f);
            BrMatrix34Translate(&collision_info->actor->t.t.mat, 0.f, 0.f, 0.f);
            parent_collision_info = collision_info;
        }
        gMutant_tail_state = 0;
    }

    InitMineShit();
#else
    __asm {
            sub esp, 0x108
            push ebx
            push ebp
            push esi
            push edi
            mov ecx, offset gMass_mine+0x80
            call LoadPixelmap
            mov ebx, eax
            test ebx, ebx
            mov dword ptr [esp + 0x10], ebx
            jne L004d9ecd
            push offset gMass_mine+0x80
            push 0x75
            call FatalError
            add esp, 8
    L004d9ecd:
            push ebx
            call BrMapAdd
            add esp, 4
            mov ebp, offset gRepulse_links+0xc
            jmp L004d9ee1
    L004d9edd:
            mov ebx, dword ptr [esp + 0x10]
    L004d9ee1:
            push 0
            push 1
            call BrActorAllocate
            add esp, 8
            mov dword ptr [ebp], eax
            push 0x64
            push 0x66
            push 0
            call BrModelAllocate
            add esp, 0xc
            mov esi, eax
            or byte ptr [esi + 0x20], 0x80
            push offset gDisable_sound+0x8
            call BrMaterialAllocate
            mov edi, eax
            add esp, 4
            mov eax, dword ptr [edi + 0x20]
            push edi
            and al, 0xfe
            or ah, 0x10
            mov dword ptr [edi + 0x40], ebx
            mov dword ptr [edi + 0x20], eax
            call BrMaterialAdd
            add esp, 4
            mov edx, 0x64
            mov ecx, edi
            call BlendifyMaterial
            push 0x7fff
            push edi
            call BrMaterialUpdate
            mov dx, word ptr [esi + 0x12]
            mov ecx, dword ptr [esi + 0xc]
            and edx, 0xfffe
            add esp, 8
            xor eax, eax
            test dx, dx
            jbe L004d9f84
    L004d9f58:
            lea edx, [eax + 1]
            mov dword ptr [ecx + 8], edi
            mov word ptr [ecx], ax
            mov word ptr [ecx + 2], dx
            mov dx, word ptr [esi + 0x10]
            add ecx, 0x28
            shr dx, 1
            lea edx, [edx + eax + 1]
            inc eax
            mov word ptr [ecx - 0x24], dx
            xor edx, edx
            mov dx, word ptr [esi + 0x12]
            shr edx, 1
            cmp eax, edx
            jl L004d9f58
    L004d9f84:
            xor edx, edx
            mov ebx, dword ptr [esi + 0xc]
            mov dx, word ptr [esi + 0x12]
            mov eax, edx
            shr eax, 1
            cmp eax, edx
            lea ecx, [eax + eax*4]
            lea ecx, [ebx + ecx*8]
            jge L004d9fc9
    L004d9f9b:
            lea edx, [eax + 1]
            mov dword ptr [ecx + 8], edi
            mov word ptr [ecx], dx
            lea edx, [eax + 2]
            mov word ptr [ecx + 2], dx
            mov dx, word ptr [esi + 0x10]
            shr dx, 1
            mov ebx, eax
            add ecx, 0x28
            sub ebx, edx
            xor edx, edx
            inc ebx
            inc eax
            mov word ptr [ecx - 0x24], bx
            mov dx, word ptr [esi + 0x12]
            cmp eax, edx
            jl L004d9f9b
    L004d9fc9:
            mov dx, word ptr [esi + 0x10]
            mov ecx, dword ptr [esi + 8]
            and edx, 0xfffe
            xor eax, eax
            test dx, dx
            jbe L004da006
            add ecx, 0x10
    L004d9fe0:
            mov edx, eax
            mov dword ptr [ecx], 0
            and edx, 1
            inc eax
            mov dword ptr [esp + 0x14], edx
            xor edx, edx
            fild dword ptr [esp + 0x14]
            add ecx, 0x28
            fstp dword ptr [ecx - 0x2c]
            mov dx, word ptr [esi + 0x10]
            shr edx, 1
            cmp eax, edx
            jl L004d9fe0
    L004da006:
            mov ebx, dword ptr [esi + 8]
            xor edx, edx
            mov dx, word ptr [esi + 0x12]
            xor ecx, ecx
            mov cx, word ptr [esi + 0x10]
            shr edx, 1
            mov eax, ecx
            shr eax, 1
            lea edx, [edx + edx*4]
            cmp eax, ecx
            lea edx, [ebx + edx*8]
            jge L004da04c
            lea ecx, [edx + 0x10]
    L004da028:
            mov edx, eax
            mov dword ptr [ecx], 0x3f800000
            and edx, 1
            inc eax
            mov dword ptr [esp + 0x14], edx
            xor edx, edx
            fild dword ptr [esp + 0x14]
            add ecx, 0x28
            fstp dword ptr [ecx - 0x2c]
            mov dx, word ptr [esi + 0x10]
            cmp eax, edx
            jl L004da028
    L004da04c:
            mov eax, dword ptr [ebp]
            push esi
            mov dword ptr [eax + 0x18], esi
            mov ecx, dword ptr [ebp]
            mov byte ptr [ecx + 0x20], 4
            mov dword ptr [ebp + 8], edi
            call BrModelAdd
            add ebp, 0x28
            add esp, 4
            cmp ebp, offset gINT_006a0a5c
            jl L004d9edd
            xor ebp, ebp
            push offset gMass_mine+0x78
            mov edx, offset gApplication_path
            lea ecx, [esp + 0x1c]
            mov dword ptr [esp + 0x14], ebp
            call PathCat
            lea ecx, [esp + 0x18]
            call OpenPackFileAndSetTiffLoading
            lea edx, [esp + 0x18]
            mov ecx, offset gMisc_storage_space
            push 2
            mov esi, eax
            call LoadAllStuffInDirectory
            mov ecx, esi
            call ClosePackFileAndSetTiffLoading
            mov eax, dword ptr [gCount_mutant_tail_parts]
            cmp eax, ebp
            jle L004da276
    L004da0bc:
            dec eax
            xor ebx, ebx
            cmp ebp, eax
            sete bl
            test ebx, ebx
            je L004da0e7
            push offset gMass_mine+0x6c
            call BrModelFind
            mov edx, dword ptr [gMass_mutant_tail_ball]
            add esp, 4
            mov edi, eax
            push edx
            mov ecx, edi
            call MungeSphereObject
            jmp L004da103
    L004da0e7:
            push offset gMass_mine+0x60
            call BrModelFind
            add esp, 4
            mov edi, eax
            mov eax, dword ptr [gMass_mutant_tail_link]
            mov ecx, edi
            push eax
            call MungeBoxObject
    L004da103:
            mov esi, eax
            mov edx, 0xdb
            mov ecx, 1
            mov byte ptr [esi + 0x238], 0x20
            call AllocatePhysicsJoint
            mov dword ptr [esi + 0x188], eax
            mov dword ptr [eax], 3
            test ebx, ebx
            je L004da133
            fld qword ptr [gDOUBLE_0058abf0]
            jmp L004da139
    L004da133:
            fld qword ptr [gDOUBLE_0058abf8]
    L004da139:
            fmul dword ptr [edi + 0x3c]
            push ecx
            mov ecx, dword ptr [esi + 0x188]
            add ecx, 8
            _emit 0xd9
        _emit 0x1c
        _emit 0x24
            push 0
            push 0
            push ecx
            call BrVector3SetFloat
            add esp, 0x10
            test ebp, ebp
            je L004da196
            test ebx, ebx
            je L004da166
            fld qword ptr [gDOUBLE_0058abf0]
            jmp L004da16c
    L004da166:
            fld qword ptr [gDOUBLE_0058abf8]
    L004da16c:
            fmul dword ptr [edi + 0x48]
            mov edx, dword ptr [esi + 0x188]
            push ecx
            add edx, 0x14
            _emit 0xd9
        _emit 0x1c
        _emit 0x24
            push 0
            push 0
            push edx
            call BrVector3SetFloat
            mov ecx, dword ptr [esp + 0x20]
            add esp, 0x10
            mov edx, esi
            call PhysicsAddObject
            jmp L004da1ea
    L004da196:
            mov dword ptr [gMutant_tail_first_collision_info], esi
            mov eax, dword ptr [esi + 0x188]
            mov dword ptr [gMutant_tail_first_joint], eax
            mov ecx, dword ptr [esi + 0x188]
            push 0x3f800000
            push 0
            mov dword ptr [ecx + 0x50], 1
            mov edx, dword ptr [esi + 0x188]
            add edx, 0x5c
            push 0
            push edx
            call BrVector3SetFloat
            mov eax, dword ptr [esi + 0x188]
            add esp, 0x10
            add eax, 0x68
            push 0x3f800000
            push 0
            push 0
            push eax
            call BrVector3SetFloat
            add esp, 0x10
    L004da1ea:
            mov ecx, dword ptr [esi + 0x188]
            push 0x3f800000
            push 0
            add ecx, 0x20
            push 0
            push ecx
            mov dword ptr [esp + 0x20], esi
            call BrVector3SetFloat
            mov edx, dword ptr [esi + 0x188]
            add esp, 0x10
            add edx, 0x2c
            push 0
            push 0x3f800000
            push 0
            push edx
            call BrVector3SetFloat
            mov eax, dword ptr [esi + 0x188]
            add esp, 0x10
            add eax, 0x38
            push 0
            push 0
            push 0x3f800000
            push eax
            call BrVector3SetFloat
            add esp, 0x10
            lea ecx, [esi + 0x8c]
            push 0
            push 0
            push 0
            push ecx
            call BrMatrix34Translate
            mov edx, dword ptr [esi]
            add esp, 0x10
            add edx, 0x2c
            push 0
            push 0
            push 0
            push edx
            call BrMatrix34Translate
            mov eax, dword ptr [gCount_mutant_tail_parts]
            add esp, 0x10
            inc ebp
            cmp ebp, eax
            jl L004da0bc
    L004da276:
            mov dword ptr [gMutant_tail_state], 0
            call InitMineShit
            pop edi
            pop esi
            pop ebp
            pop ebx
            add esp, 0x108
            ret
    }
#endif
}

// FUNCTION: CARMA2_HW 0x004e06a0
void C2_HOOK_FASTCALL RemoveTail(void) {
    tPhysics_object* node;

    if (gMutant_tail_state == 0) {
        return;
    }
    node = gMutant_tail_first_collision_info;
    if (gMutant_tail_first_collision_info == NULL) {
        goto skip;
    }
    do {
        BrActorRemove(node->actor);
        node = node->child;
    } while (node != NULL);
skip:
    if (gMutant_tail_state == 2) {
        PHILRemoveFromParentChain(gMutant_tail_first_collision_info);
    } else {
        PHILRemoveObject(gMutant_tail_first_collision_info);
    }
    gMutant_tail_first_collision_info->physics_joint1 = gMutant_tail_first_joint;
    gMutant_tail_state = 0;
}

// FUNCTION: CARMA2_HW 0x004da730
void C2_NAKED C2_HOOK_FASTCALL CloseDownPowerUps(void) {
#ifndef CARPOCALYPSE2_MATCHING
    volatile int i;
    int j;
    tPowerup* powerup;

    C2_HOOK_BUG_ON(sizeof(tPowerup) != 172);

    i = 0;
    if (gNumber_of_powerups > 0) {
        powerup = gPowerup_array;
        do {
            if (powerup->got_time != 0) {
                if (powerup->lose_proc != NULL) {
                    powerup->lose_proc(powerup, powerup->car);
                }
                if (powerup->net_type & 0x80) {
                    ARPowerupLose(powerup, powerup->car);
                }
                if (gNet_mode != eNet_mode_none) {
                    tNet_message* message;

                    message = NetBuildGuaranteedMessage(0x16, 0);
                    *(int*)((tU8*)message + 0x28) = 2;
                    *(tPlayer_ID*)((tU8*)message + 0x1c) = gLocal_net_ID;
                    *(int*)((tU8*)message + 0x20) = powerup - gPowerup_array;
                    NetGuaranteedSendMessageToAllPlayers(gCurrent_net_game, message, NULL);
                }
                gProgram_state.current_car.powerups[powerup - gPowerup_array] = 0;
                if (powerup->initial_value == 0) {
                    for (j = 0; j < gNumber_of_icons; j++) {
                        if (powerup == gPickedup_powerups[j].powerup) {
                            gPickedup_powerups[j].fizzle_stage = 3;
                            gPickedup_powerups[j].fizzle_direction = -1;
                            gPickedup_powerups[j].fizzle_start = GetTotalTime();
                            break;
                        }
                    }
                }
                powerup->got_time = 0;
            }
            i++;
            powerup++;
        } while (i < gNumber_of_powerups);
    }
#else
    __asm {
            push ecx
            mov eax, dword ptr [gNumber_of_powerups]
            push ebp
            xor ebp, ebp
            cmp eax, ebp
            mov dword ptr [esp + 4], ebp
            jle L004da860
            mov eax, dword ptr [gPowerup_array]
            push edi
            push esi
            push ebx
            lea edi, [eax + 0x40]
            mov ebx, 2
    L004da755:
            cmp dword ptr [edi - 0x3c], ebp
            je L004da840
            mov eax, dword ptr [edi]
            cmp eax, ebp
            je L004da76c
            mov edx, dword ptr [edi + 0xc]
            lea ecx, [edi - 0x40]
            call eax
    L004da76c:
            test byte ptr [edi - 8], 0x80
            je L004da77d
            mov edx, dword ptr [edi + 0xc]
            lea ecx, [edi - 0x40]
            call ARPowerupLose
    L004da77d:
            cmp dword ptr [gNet_mode], ebp
            je L004da7ca
            xor edx, edx
            mov cl, 0x16
            call NetBuildGuaranteedMessage
            mov esi, eax
            push ebp
            mov dword ptr [esi + 0x28], ebx
            mov ecx, dword ptr [gLocal_net_ID]
            mov dword ptr [esi + 0x1c], ecx
            mov eax, dword ptr [gPowerup_array]
            mov ecx, edi
            sub ecx, eax
            mov eax, 0x2fa0be83
            sub ecx, 0x40
            imul ecx
            sar edx, 5
            mov eax, edx
            shr eax, 0x1f
            add edx, eax
            mov dword ptr [esi + 0x20], edx
            mov ecx, dword ptr [gCurrent_net_game]
            mov edx, esi
            call NetGuaranteedSendMessageToAllPlayers
    L004da7ca:
            mov esi, dword ptr [gPowerup_array]
            mov ecx, edi
            sub ecx, esi
            mov eax, 0x2fa0be83
            sub ecx, 0x40
            imul ecx
            sar edx, 5
            mov ecx, edx
            shr ecx, 0x1f
            add edx, ecx
            mov dword ptr [edx*4 + gProgram_state+0x17bc], ebp
            mov eax, dword ptr [edi - 0xc]
            cmp eax, ebp
            jne L004da83d
            mov edx, dword ptr [gNumber_of_icons]
            xor eax, eax
            cmp edx, ebp
            jle L004da83d
            mov ecx, offset gPickedup_powerups
    L004da807:
            lea esi, [edi - 0x40]
            cmp esi, dword ptr [ecx]
            je L004da818
            inc eax
            add ecx, 0x14
            cmp eax, edx
            jl L004da807
            jmp L004da83d
    L004da818:
            lea esi, [eax + eax*4]
            shl esi, 2
            mov dword ptr [esi + gPickedup_powerups+0x4], 3
            mov dword ptr [esi + gPickedup_powerups+0x8], 0xffffffff
            call GetTotalTime
            mov dword ptr [esi + gPickedup_powerups+0xc], eax
    L004da83d:
            mov dword ptr [edi - 0x3c], ebp
    L004da840:
            mov eax, dword ptr [esp + 0x10]
            mov ecx, dword ptr [gNumber_of_powerups]
            inc eax
            add edi, 0xac
            cmp eax, ecx
            mov dword ptr [esp + 0x10], eax
            jl L004da755
            pop ebx
            pop esi
            pop edi
    L004da860:
            pop ebp
            pop ecx
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
            nop
            nop
    }
#endif
}

// FUNCTION: CARMA2_HW 0x004def00
void C2_NAKED C2_HOOK_FASTCALL CloseDownPowerUpsForCar(tCar_spec* pCar) {
#ifndef CARPOCALYPSE2_MATCHING
    int powerup_address;
    int message_address;
    tU32 total_time;
    int* active_powerup;
    int* picked_up_powerup;
    int powerup_offset;
    int i;
    int j;

    if (((pCar != NULL) && (pCar->driver == eDriver_local_human)) && (i = 0, 0 < gNumber_of_powerups)) {
        powerup_offset = 0;
        active_powerup = pCar->powerups;
        do {
            if ((*active_powerup != 0) && (powerup_address = powerup_offset + (int)gPowerup_array, (((tPowerup*)powerup_address)->net_type & 1) == 0)) {
                if (((tPowerup*)powerup_address)->got_time != 0 && ((tPowerup*)powerup_address)->lose_proc != NULL) {
                    ((tPowerup*)powerup_address)->lose_proc((tPowerup*)powerup_address, ((tPowerup*)powerup_address)->car);
                }
                if (((tPowerup*)powerup_address)->net_type & 0x80) {
                    ARPowerupLose((tPowerup*)powerup_address, ((tPowerup*)powerup_address)->car);
                }
                if (gNet_mode != 0) {
                    message_address = (int)NetBuildGuaranteedMessage(0x16, 0);
                    *(int*)(message_address + 0x28) = 2;
                    *(tPlayer_ID*)(message_address + 0x1c) = gLocal_net_ID;
                    *(int*)(message_address + 0x20) = (powerup_address - (int)gPowerup_array) / 0xac;
                    NetGuaranteedSendMessageToAllPlayers(gCurrent_net_game, (tNet_message*)message_address, 0);
                }
                gProgram_state.current_car.powerups[(powerup_address - (int)gPowerup_array) / 0xac] = 0;
                if (((tPowerup*)powerup_address)->initial_value == 0 && (j = 0, 0 < gNumber_of_icons)) {
                    picked_up_powerup = (int*)&gPickedup_powerups[0].powerup;
                    while (j < gNumber_of_icons) {
                        if (powerup_address == *picked_up_powerup) {
                            gPickedup_powerups[j].fizzle_stage = 3;
                            gPickedup_powerups[j].fizzle_direction = -1;
                            total_time = GetTotalTime();
                            gPickedup_powerups[j].fizzle_start = total_time;
                            break;
                        }
                        j++;
                        picked_up_powerup += 5;
                    }
                }
                ((tPowerup*)powerup_address)->got_time = 0;
            }
            i++;
            active_powerup++;
            powerup_offset += 0xac;
        } while (i < gNumber_of_powerups);
    }
#else
    __asm {
            sub esp, 8
            push ebp
            xor ebp, ebp
            cmp ecx, ebp
            je L004df061
            cmp dword ptr [ecx + 0xc], 8
            jne L004df061
            mov eax, dword ptr [gNumber_of_powerups]
            mov dword ptr [esp + 4], ebp
            cmp eax, ebp
            jle L004df061
            push edi
            push esi
            push ebx
            xor ebx, ebx
            add ecx, 0x1710
            mov dword ptr [esp + 0x14], ecx
    L004def38:
            cmp dword ptr [ecx], ebp
            je L004df036
            mov eax, dword ptr [gPowerup_array]
            lea esi, [eax + ebx]
            mov al, byte ptr [eax + ebx + 0x38]
            test al, 1
            jne L004df036
            cmp dword ptr [esi + 4], ebp
            je L004def67
            mov eax, dword ptr [esi + 0x40]
            cmp eax, ebp
            je L004def67
            mov edx, dword ptr [esi + 0x4c]
            mov ecx, esi
            call eax
    L004def67:
            test byte ptr [esi + 0x38], 0x80
            je L004def77
            mov edx, dword ptr [esi + 0x4c]
            mov ecx, esi
            call ARPowerupLose
    L004def77:
            cmp dword ptr [gNet_mode], ebp
            je L004defc6
            xor edx, edx
            mov cl, 0x16
            call NetBuildGuaranteedMessage
            mov ecx, eax
            mov eax, 0x2fa0be83
            push ebp
            mov dword ptr [ecx + 0x28], 2
            mov edx, dword ptr [gLocal_net_ID]
            mov dword ptr [ecx + 0x1c], edx
            mov edi, dword ptr [gPowerup_array]
            mov edx, esi
            sub edx, edi
            imul edx
            sar edx, 5
            mov eax, edx
            shr eax, 0x1f
            add edx, eax
            mov dword ptr [ecx + 0x20], edx
            mov edx, ecx
            mov ecx, dword ptr [gCurrent_net_game]
            call NetGuaranteedSendMessageToAllPlayers
    L004defc6:
            mov edx, dword ptr [gPowerup_array]
            mov ecx, esi
            sub ecx, edx
            mov eax, 0x2fa0be83
            imul ecx
            sar edx, 5
            mov ecx, edx
            shr ecx, 0x1f
            add edx, ecx
            mov dword ptr [edx*4 + gProgram_state+0x17bc], ebp
            mov eax, dword ptr [esi + 0x34]
            cmp eax, ebp
            jne L004df033
            mov edx, dword ptr [gNumber_of_icons]
            xor eax, eax
            cmp edx, ebp
            jle L004df033
            mov ecx, offset gPickedup_powerups
    L004df000:
            cmp esi, dword ptr [ecx]
            je L004df00e
            inc eax
            add ecx, 0x14
            cmp eax, edx
            jl L004df000
            jmp L004df033
    L004df00e:
            lea edi, [eax + eax*4]
            shl edi, 2
            mov dword ptr [edi + gPickedup_powerups+0x4], 3
            mov dword ptr [edi + gPickedup_powerups+0x8], 0xffffffff
            call GetTotalTime
            mov dword ptr [edi + gPickedup_powerups+0xc], eax
    L004df033:
            mov dword ptr [esi + 4], ebp
    L004df036:
            mov eax, dword ptr [esp + 0x10]
            mov ecx, dword ptr [esp + 0x14]
            mov edx, dword ptr [gNumber_of_powerups]
            inc eax
            add ecx, 4
            add ebx, 0xac
            cmp eax, edx
            mov dword ptr [esp + 0x10], eax
            mov dword ptr [esp + 0x14], ecx
            jl L004def38
            pop ebx
            pop esi
            pop edi
    L004df061:
            pop ebp
            add esp, 8
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
    }
#endif
}

static __inline void C2_HOOK_FASTCALL ReadAwardInline(FILE* pF, tAward_info* pAward_info) {

    pAward_info->frequency = GetALineAndInterpretCommand(pF, gRepeatability_names, CARPOCALYPSE2_ASIZE(gRepeatability_names));
    if (pAward_info->frequency != kRepeatability_None) {
        pAward_info->field_0x4 = GetAScalar(pF);
        pAward_info->field_0x8 = GetAScalar(pF);
        pAward_info->field_0xc = GetAnInt(pF);
        pAward_info->field_0x10 = GetAnInt(pF);
    }
}

static __inline void C2_HOOK_FASTCALL ReadPowerupSmashable(FILE* pF, tSmashable_item_spec* pSmashable_spec) {
    int i;
    int d1, d2;

#ifndef CARPOCALYPSE2_MATCHING
    C2_HOOK_BUG_ON(offsetof(tShrapnel_spec, initial_pos) != 40);
    C2_HOOK_BUG_ON(offsetof(tShrapnel_spec, type_info) != 56);
    C2_HOOK_BUG_ON(sizeof(tShrapnel_spec) != 88);
    C2_HOOK_BUG_ON(offsetof(tSmashable_item_spec, mode_data) + offsetof(tSmashable_item_spec_shrapnel, connotations.shrapnel) != 44);
    C2_HOOK_BUG_ON(offsetof(tSmashable_item_spec, mode_data) + offsetof(tSmashable_item_spec_shrapnel, connotations.special_effects.explosion_animation) != 572);
    C2_HOOK_BUG_ON(sizeof(tSmashable_item_spec) != 736);
#endif

    memset(pSmashable_spec, 0, sizeof(tSmashable_item_spec));
    /* Number of sounds */
    pSmashable_spec->mode_data.shrapnel.connotations.count_sounds = GetAnInt(pF);
    for (i = 0; i < pSmashable_spec->mode_data.shrapnel.connotations.count_sounds; i++) {
        /* Sound ID */
        pSmashable_spec->mode_data.shrapnel.connotations.sounds[i] = LoadSingleSound(&gTrack_storage_space, GetAnInt(pF));
    }
    ReadShrapnelSpec(pF, pSmashable_spec->mode_data.shrapnel.connotations.shrapnel, &pSmashable_spec->mode_data.shrapnel.connotations.count_shrapnel);
    ReadExplosion(pF, &pSmashable_spec->mode_data.shrapnel.connotations.special_effects.explosion_animation);
    ReadSpillData(pF, &pSmashable_spec->mode_data.shrapnel.connotations.special_effects.slick);
    ReadNonCarCuboidActivation(pF, &pSmashable_spec->mode_data.shrapnel.connotations.side_effects.activations);
    ReadShrapnelSideEffects(pF, &pSmashable_spec->mode_data.shrapnel.connotations.side_effects.side_effects);

    /* Extension flags */
    pSmashable_spec->mode_data.shrapnel.connotations.side_effects.extension_flags = GetAnInt(pF);
    if (pSmashable_spec->mode_data.shrapnel.connotations.side_effects.extension_flags & 0x1) {
        pSmashable_spec->mode_data.shrapnel.connotations.side_effects.extension_arg = GetAnInt(pF);
    }
    /* Room turn on */
    pSmashable_spec->mode_data.shrapnel.connotations.side_effects.room_turn_on_code = GetAnInt(pF);
    /* Award code */
    ReadAwardInline(pF, &pSmashable_spec->mode_data.shrapnel.connotations.award);
    /* Count variable changes */
    pSmashable_spec->mode_data.shrapnel.connotations.variable_changes.count = GetAnInt(pF);
    for (i = 0; i < pSmashable_spec->mode_data.shrapnel.connotations.variable_changes.count; i++) {
        GetPairOfInts(pF, &d1, &d2);
        pSmashable_spec->mode_data.shrapnel.connotations.variable_changes.runtime_changes[i].field_0x2 = d1;
        pSmashable_spec->mode_data.shrapnel.connotations.variable_changes.runtime_changes[i].field_0x0 = d2;
    }
    pSmashable_spec->trigger_type = kSmashableTrigger_Model | kSmashableTrigger_Number;
    CARPOCALYPSE2_BUG_ON((kSmashableTrigger_Model | kSmashableTrigger_Number) != 0x3);
    pSmashable_spec->mode = kSmashableMode_Remove;
    CARPOCALYPSE2_BUG_ON(kSmashableMode_Remove != 0x3);
}

// FUNCTION: CARMA2_HW 0x004efa00
void C2_NAKED C2_HOOK_FASTCALL ReadPowerupSmashables(FILE* pF) {
#ifndef CARPOCALYPSE2_MATCHING

    ReadPowerupSmashable(pF, &gPowerup_pickup_smashable);
    ReadPowerupSmashable(pF, &gPowerup_respawn_smashable);

    memset(&gUnknown_smashable_006a3660, 0, sizeof(gUnknown_smashable_006a3660));
    gUnknown_smashable_006a3660.trigger_type = kSmashableTrigger_Model | kSmashableTrigger_Number;
    gUnknown_smashable_006a3660.mode = kSmashableMode_Remove;
#else
    __asm {
            sub esp, 8
            xor eax, eax
            push ebx
            push ebp
            push esi
            mov esi, ecx
            push edi
            mov ecx, 0xb8
            mov edi, offset gPowerup_pickup_smashable
            rep stosd
            mov ecx, esi
            call GetAnInt
            xor edi, edi
            mov dword ptr [gPowerup_pickup_smashable+0x18], eax
            test eax, eax
            jle L004efa50
            mov ebx, offset gPowerup_pickup_smashable+0x1c
    L004efa2e:
            mov ecx, esi
            call GetAnInt
            mov edx, eax
            mov ecx, offset gTrack_storage_space
            call LoadSingleSound
            mov dword ptr [ebx], eax
            mov eax, dword ptr [gPowerup_pickup_smashable+0x18]
            inc edi
            add ebx, 4
            cmp edi, eax
            jl L004efa2e
    L004efa50:
            push offset gPowerup_pickup_smashable+0x28
            mov edx, offset gPowerup_pickup_smashable+0x2c
            mov ecx, esi
            call ReadShrapnelSpec
            mov edx, offset gPowerup_pickup_smashable+0x23c
            mov ecx, esi
            call ReadExplosion
            mov edx, offset gPowerup_pickup_smashable+0x244
            mov ecx, esi
            call ReadSpillData
            mov edx, offset gPowerup_pickup_smashable+0x274
            mov ecx, esi
            call ReadNonCarCuboidActivation
            mov edx, offset gPowerup_pickup_smashable+0x27c
            mov ecx, esi
            call ReadShrapnelSideEffects
            mov ecx, esi
            call GetAnInt
            mov dword ptr [gPowerup_pickup_smashable+0x288], eax
            mov al, byte ptr [gPowerup_pickup_smashable+0x288]
            test al, 1
            je L004efab2
            mov ecx, esi
            call GetAnInt
            mov dword ptr [gPowerup_pickup_smashable+0x284], eax
    L004efab2:
            mov ecx, esi
            call GetAnInt
            mov edx, offset gRepeatability_names
            mov ecx, esi
            push 4
            mov dword ptr [gPowerup_pickup_smashable+0x28c], eax
            call GetALineAndInterpretCommand
            test eax, eax
            mov dword ptr [gPowerup_pickup_smashable+0x290], eax
            je L004efb07
            mov ecx, esi
            call GetAScalar
            fstp dword ptr [gPowerup_pickup_smashable+0x294]
            mov ecx, esi
            call GetAScalar
            fstp dword ptr [gPowerup_pickup_smashable+0x298]
            mov ecx, esi
            call GetAnInt
            mov ecx, esi
            mov dword ptr [gPowerup_pickup_smashable+0x29c], eax
            call GetAnInt
            mov dword ptr [gPowerup_pickup_smashable+0x2a0], eax
    L004efb07:
            mov ecx, esi
            call GetAnInt
            xor ebp, ebp
            mov byte ptr [gPowerup_pickup_smashable+0x2a8], al
            test al, al
            jbe L004efb52
            mov edi, offset gPowerup_pickup_smashable+0x2aa
    L004efb1e:
            lea eax, [esp + 0x14]
            lea edx, [esp + 0x10]
            push eax
            mov ecx, esi
            call GetPairOfInts
            inc ebp
            add edi, 4
            mov cx, word ptr [esp + 0x10]
            mov dx, word ptr [esp + 0x14]
            mov word ptr [edi - 2], cx
            mov word ptr [edi - 4], dx
            mov eax, dword ptr [gPowerup_pickup_smashable+0x2a8]
            and eax, 0xff
            cmp ebp, eax
            jl L004efb1e
    L004efb52:
            mov ecx, 0xb8
            xor eax, eax
            mov edi, offset gPowerup_respawn_smashable
            mov ebp, 3
            rep stosd
            mov ecx, esi
            mov dword ptr [gPowerup_pickup_smashable+0x8], ebp
            mov dword ptr [gPowerup_pickup_smashable+0xc], ebp
            call GetAnInt
            xor edi, edi
            mov dword ptr [gPowerup_respawn_sound_count], eax
            test eax, eax
            jle L004efbaa
            mov ebx, offset gPowerup_respawn_sound_ids
    L004efb88:
            mov ecx, esi
            call GetAnInt
            mov edx, eax
            mov ecx, offset gTrack_storage_space
            call LoadSingleSound
            mov dword ptr [ebx], eax
            mov eax, dword ptr [gPowerup_respawn_sound_count]
            inc edi
            add ebx, 4
            cmp edi, eax
            jl L004efb88
    L004efbaa:
            push offset gPowerup_respawn_sound_ids+0xc
            mov edx, offset gPowerup_respawn_sound_ids+0x10
            mov ecx, esi
            call ReadShrapnelSpec
            mov edx, offset gPowerup_respawn_explosion
            mov ecx, esi
            call ReadExplosion
            mov edx, offset gPowerup_respawn_spill_time_offset
            mov ecx, esi
            call ReadSpillData
            mov edx, offset gPowerup_respawn_field_0x50+0x4
            mov ecx, esi
            call ReadNonCarCuboidActivation
            mov edx, offset gPowerup_respawn_field_0x50+0xc
            mov ecx, esi
            call ReadShrapnelSideEffects
            mov ecx, esi
            call GetAnInt
            mov dword ptr [gPowerup_respawn_field_0x50+0x18], eax
            mov al, byte ptr [gPowerup_respawn_field_0x50+0x18]
            test al, 1
            je L004efc0c
            mov ecx, esi
            call GetAnInt
            mov dword ptr [gPowerup_respawn_field_0x50+0x14], eax
    L004efc0c:
            mov ecx, esi
            call GetAnInt
            mov edx, offset gRepeatability_names
            mov ecx, esi
            push 4
            mov dword ptr [gPowerup_respawn_field_0x50+0x1c], eax
            call GetALineAndInterpretCommand
            test eax, eax
            mov dword ptr [gPowerup_respawn_field_0x50+0x20], eax
            je L004efc61
            mov ecx, esi
            call GetAScalar
            fstp dword ptr [gPowerup_respawn_field_0x50+0x24]
            mov ecx, esi
            call GetAScalar
            fstp dword ptr [gPowerup_respawn_field_0x50+0x28]
            mov ecx, esi
            call GetAnInt
            mov ecx, esi
            mov dword ptr [gPowerup_respawn_field_0x50+0x2c], eax
            call GetAnInt
            mov dword ptr [gPowerup_respawn_field_0x50+0x30], eax
    L004efc61:
            mov ecx, esi
            call GetAnInt
            xor ebx, ebx
            mov byte ptr [gPowerup_respawn_field_0x50+0x38], al
            test al, al
            jbe L004efcae
            mov edi, offset gPowerup_respawn_field_0x50+0x3a
    L004efc78:
            lea ecx, [esp + 0x10]
            lea edx, [esp + 0x14]
            push ecx
            mov ecx, esi
            call GetPairOfInts
            inc ebx
            add edi, 4
            mov dx, word ptr [esp + 0x14]
            mov ax, word ptr [esp + 0x10]
            mov word ptr [edi - 2], dx
            mov word ptr [edi - 4], ax
            mov ecx, dword ptr [gPowerup_respawn_field_0x50+0x38]
            and ecx, 0xff
            cmp ebx, ecx
            jl L004efc78
    L004efcae:
            mov ecx, 0xb8
            xor eax, eax
            mov edi, offset gUnknown_smashable_006a3660
            mov dword ptr [gPowerup_respawn_smashable+0x8], ebp
            rep stosd
            pop edi
            mov dword ptr [gPowerup_respawn_smashable+0xc], ebp
            mov dword ptr [gUnknown_smashable_006a3660+0x8], ebp
            mov dword ptr [gUnknown_smashable_006a3660+0xc], ebp
            pop esi
            pop ebp
            pop ebx
            add esp, 8
            ret
    }
#endif
}

// FUNCTION: CARMA2_HW 0x004da530
void C2_HOOK_FASTCALL InitMineShit(void) {
    tPath_name the_path;
    tTWTVFS twt;
    int i;

    PathCat(the_path, gApplication_path, "MINE");
    twt = OpenPackFileAndSetTiffLoading(the_path);
    LoadAllStuffInDirectory(&gMisc_storage_space, the_path, kRendererShadingType_Specular);
    ClosePackFileAndSetTiffLoading(twt);

    C2_HOOK_BUG_ON(sizeof(tShit_mine) != 56);
    C2_HOOK_BUG_ON(CARPOCALYPSE2_ASIZE(gShit_mines) != 20);
    for (i = 0; i < (int)CARPOCALYPSE2_ASIZE(gShit_mines); i++) {
        br_model* model;
        gShit_mines[i].flags = 4;
        gShit_mines[i].field_0x35 = 0;
        model = BrModelFind("SHITMINE.ACT");
        gShit_mines[i].collision_info = MungeSphereObject(model, gMass_mine);
        gShit_mines[i].collision_info->uid = i;
        gShit_mines[i].collision_info->flags_0x238 = 0x40;
        gShit_mines[i].collision_info->owner = &gShit_mines[i];
        BrMatrix34Translate(&gShit_mines[i].collision_info->transform_matrix, 0.f, 0.f, 0.f);
        BrMatrix34Translate(&gShit_mines[i].collision_info->actor->t.t.mat, 0.f, 0.f, 0.f);
    }
}

void C2_HOOK_FASTCALL FillInMine(tNet_message_chunk* pChunk, tShit_mine* pMine, int pArg3) {

#ifndef CARPOCALYPSE2_MATCHING
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tNet_message_chunk, mine_explode.mine_index, 0x2);
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tNet_message_chunk, mine_explode.owner_index, 0x3);
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tNet_message_chunk, mine_explode.powerup_index, 0x4);
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tNet_message_chunk, mine_explode.player_index, 0x5);
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tNet_message_chunk, mine_explode.field_0x8, 0x8);
#endif

    pChunk->mine_explode.mine_index = pMine - gShit_mines;
    pChunk->mine_explode.owner_index = pMine->field_0x35;
    pChunk->mine_explode.powerup_index = pMine->field_0x34;
    pChunk->mine_explode.player_index = NetPlayerFromCar(pMine->field_0x30) - gNet_players;
    pChunk->mine_explode.field_0x8 = pArg3;
    GetHierarchyNetworkStuff(pMine->collision_info, pChunk->mine_explode.object_hierarchy, NET_OBJECT_HIERARCHY_DEFAULT_SIZE);
}

// FUNCTION: CARMA2_HW 0x004ddab0
int C2_HOOK_FASTCALL MineExplode(tShit_mine *pMine) {
    tNet_message* message;
    br_vector3 offset;
    int hierarchy_size;

    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tNet_message, contents, 0x18);

    if (pMine->flags & 0x2) {
        return 0;
    }
    pMine->flags |= 0x2;
    if (gNet_mode == eNet_mode_host) {
        hierarchy_size = GetHierarchyNetworkSize(pMine->collision_info);
        message = NetBuildGuaranteedMessage(49, hierarchy_size + 12);
        FillInMine(&message->contents, pMine, -1);
        NetGuaranteedSendMessageToAllPlayers(gCurrent_net_game, message, 0);
    }
    PHILRemoveObject(pMine->collision_info);
    pMine->next_think_time = GetTotalTime() + 1600;
    BrVector3Negate(&offset, &gExplosion_pix_animation_groups.groups[0].offset);
    Explode(&gExplosion_pix_animation_groups, pMine->collision_info->actor, &pMine->collision_info->actor->model->bounds, &offset);
    DRS3StartSound3D(gXXX_outlet, eSoundId_Explosion, &pMine->collision_info->actor->t.t.translate.t, &gZero_v__car, 1, 255, BR_FIXED_INT(1), BR_FIXED_INT(1));
    PipeSingleEndShitMine(pMine->collision_info->actor);
    if (pMine->collision_info->actor->parent != NULL) {
        BrActorRemove(pMine->collision_info->actor);
    }
    pMine->collision_info->actor->render_style = BR_RSTYLE_NONE;
    return 1;
}

static __inline int C2_HOOK_FASTCALL CarVulnerableByMine(tCar_spec* pCar) {

    return pCar->field_0x4c8 <= gDOUBLE_0058ac18
        && (gNet_mode == eNet_mode_none
            || NetPlayerFromCar(pCar) == NULL
            || NetPlayerFromCar(pCar)->field_0x80 == 0)
        && !pCar->invulnerable_no_crushage;
}

// FUNCTION: CARMA2_HW 0x004ddc20
void C2_NAKED C2_HOOK_FASTCALL CollideShitMine(tPhysics_object* pObject1, tPhysics_object* pObject2, float pCollision_speed) {
#ifndef CARPOCALYPSE2_MATCHING
    tShit_mine* mine;
    tPhysics_object* other_object;
    void* other_owner;
    tCar_spec* car;
    tNet_game_player_info* net_player;

    mine = NULL;
    other_object = NULL;
    if (gNet_mode == 3) {
        return;
    }
    if (pObject1 != NULL && pObject1->flags_0x238 == '@') {
        mine = (tShit_mine*)pObject1->owner;
        other_object = pObject2;
        if (mine->flags & 0x20) {
            return;
        }
    } else if (pObject2 != NULL && pObject2->flags_0x238 == '@') {
        mine = (tShit_mine*)pObject2->owner;
        other_object = pObject1;
        if (mine->flags & 0x20) {
            return;
        }
    }
    if (mine != NULL && other_object != NULL) {
        other_owner = other_object->owner;
        if (other_owner != NULL && other_object->flags_0x238 == 1) {
            car = (tCar_spec*)other_owner;
        } else {
            car = NULL;
        }
        if (car != NULL && car->driver != 0
                && (car->driver == 4 || car->driver == 2 || car->driver == 3)
                && (*(tU32*)((tU8*)car + 0x100) & 0x10000) != 0) {
            goto collision;
        }
        if (other_object->flags_0x238 == '@') {
            ((tShit_mine*)other_owner)->flags |= 0x20;
            return;
        }
        car = (tCar_spec*)other_object->owner;
        if (car != NULL && other_object->flags_0x238 == 1) {
            if (car->driver != 0
                    && (car->driver == 2 || car->driver == 3)) {
                goto bad;
            }
            if (car->driver <= 5) {
                goto bad;
            }
            if (car->field_0x4c8 <= gDOUBLE_0058ac18
                    && (gNet_mode == 0 || (net_player = NetPlayerFromCar(car)) == NULL
                        || (net_player = NetPlayerFromCar(car), net_player->field_0x80 == 0))) {
                tU32 flags = mine->flags | 0x80;
                mine->car = car;
                mine->flags = flags;
                mine->max_damage = (float)(car->softness_factor * gDOUBLE_0058ac48);
                mine->flags = flags | 0x20;
                return;
            }
            mine->flags |= 0x20;
            return;
        }
    bad:
        FUN_004d9d00(other_object, 0);
        mine->flags |= 0x20;
        return;
    }
collision:
    if (pCollision_speed <= mine->field_0x1c || gPinball_factor != gFLOAT_0058ac0c) {
        if (pCollision_speed > gFLOAT_0058ac88) {
            DRS3StartSound3D(gXXX_outlet, 0x151b, &mine->collision_info->actor->t.t.translate.t,
                    &gZero_v__car, 1, 0xff, 0x10000, 0x10000);
        }
        return;
    }
    mine->flags |= 0x20;
#else
    __asm {
            push ebp
            mov ebp, dword ptr [gNet_mode]
            mov eax, ecx
            push esi
            xor esi, esi
            xor ecx, ecx
            cmp ebp, 3
            push edi
            je L004dddd2
            test eax, eax
            je L004ddc58
            cmp byte ptr [eax + 0x238], 0x40
            jne L004ddc58
            mov esi, dword ptr [eax + 0x23c]
            mov ecx, edx
            test byte ptr [esi], 0x20
            jne L004dddd2
            jmp L004ddc76
    L004ddc58:
            test edx, edx
            je L004ddc76
            cmp byte ptr [edx + 0x238], 0x40
            jne L004ddc76
            mov esi, dword ptr [edx + 0x23c]
            mov ecx, eax
            test byte ptr [esi], 0x20
            jne L004dddd2
    L004ddc76:
            test esi, esi
            je L004ddd65
            test ecx, ecx
            je L004ddd65
            mov edi, dword ptr [ecx + 0x23c]
            test edi, edi
            je L004ddc9d
            cmp byte ptr [ecx + 0x238], 1
            jne L004ddc9d
            mov edx, edi
            jmp L004ddc9f
    L004ddc9d:
            xor edx, edx
    L004ddc9f:
            test edx, edx
            je L004ddcc9
            mov eax, dword ptr [edx + 0xc]
            test eax, eax
            je L004ddcc9
            cmp eax, 4
            je L004ddcb9
            cmp eax, 2
            je L004ddcb9
            cmp eax, 3
            jne L004ddcc9
    L004ddcb9:
            test dword ptr [edx + 0x100], 0x10000
            jne L004ddd65
    L004ddcc9:
            mov al, byte ptr [ecx + 0x238]
            cmp al, 0x40
            jne L004ddcdf
            mov eax, dword ptr [edi]
            or al, 0x20
            mov dword ptr [edi], eax
            pop edi
            pop esi
            pop ebp
            ret 4
    L004ddcdf:
            test edi, edi
            je L004ddd52
            cmp al, 1
            jne L004ddd52
            mov eax, dword ptr [edi + 0xc]
            test eax, eax
            je L004ddcf8
            cmp eax, 2
            je L004ddd52
            cmp eax, 3
            je L004ddd52
    L004ddcf8:
            cmp eax, 5
            jle L004ddd52
            fld dword ptr [edi + 0x4c8]
            fcomp qword ptr [gDOUBLE_0058ac18]
            fnstsw ax
            test ah, 0x41
            je L004ddd86
            test ebp, ebp
            je L004ddd30
            mov ecx, edi
            call NetPlayerFromCar
            test eax, eax
            je L004ddd30
            mov ecx, edi
            call NetPlayerFromCar
            mov ecx, dword ptr [eax + 0x80]
            test ecx, ecx
            jne L004ddd86
    L004ddd30:
            fld dword ptr [edi + 0x18d0]
            mov eax, dword ptr [esi]
            mov dword ptr [esi + 0x28], edi
            fmul qword ptr [gDOUBLE_0058ac48]
            or al, 0x80
            mov dword ptr [esi], eax
            or al, 0x20
            fstp dword ptr [esi + 0x2c]
            mov dword ptr [esi], eax
            pop edi
            pop esi
            pop ebp
            ret 4
    L004ddd52:
            xor edx, edx
            call FUN_004d9d00
            mov eax, dword ptr [esi]
            or al, 0x20
            mov dword ptr [esi], eax
            pop edi
            pop esi
            pop ebp
            ret 4
    L004ddd65:
            fld dword ptr [esp + 0x10]
            fcomp dword ptr [esi + 0x1c]
            fnstsw ax
            test ah, 0x41
            jne L004ddd92
            fld dword ptr [gPinball_factor]
            fcomp dword ptr [gFLOAT_0058ac0c]
            fnstsw ax
            test ah, 0x40
            je L004ddd92
    L004ddd86:
            mov eax, dword ptr [esi]
            or al, 0x20
            mov dword ptr [esi], eax
            pop edi
            pop esi
            pop ebp
            ret 4
    L004ddd92:
            fld dword ptr [esp + 0x10]
            fcomp dword ptr [gFLOAT_0058ac88]
            fnstsw ax
            test ah, 0x41
            jne L004dddd2
            mov eax, dword ptr [esi + 8]
            push 0x10000
            push 0x10000
            push 0xff
            mov ecx, dword ptr [eax]
            push 1
            add ecx, 0x50
            push offset gZero_v__car
            push ecx
            mov ecx, dword ptr [gXXX_outlet]
            mov edx, 0x151b
            call DRS3StartSound3D
    L004dddd2:
            pop edi
            pop esi
            pop ebp
            ret 4
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

// FUNCTION: CARMA2_HW 0x004dde20
void C2_NAKED C2_HOOK_FASTCALL ProcessShitMines(tU32 pTime) {
#ifndef CARPOCALYPSE2_MATCHING
    int i;

    C2_HOOK_BUG_ON(CARPOCALYPSE2_ASIZE(gShit_mines) != 20);

    for (i = 0; i < (int)CARPOCALYPSE2_ASIZE(gShit_mines); i++) {
        tShit_mine* mine = &gShit_mines[i];

        if (!(mine->flags & 0x1)) {
            continue;
        }
        if (mine->flags & 0x80) {
            tCar_spec *car = mine->car;

            if (car != NULL) {
                int car_vulnerable;

                mine->flags &= ~0x80;
                car_vulnerable = CarVulnerableByMine(car);
                if (car_vulnerable) {
                    TotallySpamTheModel(car, mine->max_damage * gFLOAT_0058ac40);
                }
                if (car_vulnerable && (gNet_mode == eNet_mode_none || gNet_mode == eNet_mode_host)) {
                    car->collision_info->v.v[1] += FRandomBetween(mine->field3_0xc, mine->initial_y_speed_factor) / (car->collision_info->M * gDouble_0058abe0);
                    car->collision_info->omega.v[2] += FRandomPosNeg(mine->initial_z_omega_factor) * gDouble_0058abe8 / car->collision_info->M;
                    car->collision_info->omega.v[0] += FRandomPosNeg(mine->initial_x_omega_factor) * gDouble_0058abe8 / car->collision_info->M;
                    car->collision_info->disable_move_rotate = 0;

                    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tPhysics_object, field_0x49c, 0x49c);
                    car->collision_info->field_0x49c = 40 + gPHIL_last_physics_tick;
                    if (!car->invulnerable_no_damage) {
                        C2_HOOK_BUG_ON(CARPOCALYPSE2_ASIZE(car->damage_units) != 12);
                        for (i = 0; i < (int)CARPOCALYPSE2_ASIZE(car->damage_units); i++) {
                            DoDamage(car, i, IRandomBetween(0, (int)(mine->max_damage * gFLOAT_0058ac8c)));
                        }
                        if (car != mine->field_0x30) {
                            car->last_hit_by = mine->field_0x30;
                            car->time_last_hit = PDGetTotalTime();
                            CrashEarnings(car, NULL);
                        }
                    }
                }
            }
        }
        if (mine->flags & 0x20) {
            mine->flags &= ~0x20;
            MineExplode(mine);
        } else if (mine->flags & 0x2) {
            if (mine->next_think_time < pTime) {
                mine->flags &= ~0x3;
            }
        } else if ((mine->flags & 0x4) && mine->next_think_time < pTime) {
            MineExplode(mine);
        }
    }
#else
    __asm {
            push ecx
            push ebx
            push ebp
            push esi
            push edi
            mov dword ptr [esp + 0x10], ecx
            xor ebx, ebx
    L004dde2b:
            mov eax, ebx
            shl eax, 3
            sub eax, ebx
            lea edi, [eax*8 + gShit_mines]
            mov eax, dword ptr [eax*8 + gShit_mines]
            test al, 1
            je L004ddfe2
            test al, 0x80
            je L004ddfab
            mov esi, dword ptr [edi + 0x28]
            test esi, esi
            je L004ddfab
            and al, 0x7f
            mov dword ptr [edi], eax
            fld dword ptr [esi + 0x4c8]
            fcomp qword ptr [gDOUBLE_0058ac18]
            fnstsw ax
            test ah, 0x41
            je L004ddff2
            mov eax, dword ptr [gNet_mode]
            test eax, eax
            je L004dde9f
            mov ecx, esi
            call NetPlayerFromCar
            test eax, eax
            je L004dde9f
            mov ecx, esi
            call NetPlayerFromCar
            mov ecx, dword ptr [eax + 0x80]
            test ecx, ecx
            jne L004ddff2
    L004dde9f:
            mov eax, dword ptr [esi + 0x1b4]
            test eax, eax
            jne L004ddff2
            fld dword ptr [edi + 0x2c]
            fmul dword ptr [gFLOAT_0058ac40]
            push ecx
            mov ecx, esi
            mov ebp, 1
            fstp dword ptr [esp]
            call TotallySpamTheModel
    L004ddec6:
            test ebp, ebp
            je L004ddfab
            mov eax, dword ptr [gNet_mode]
            test eax, eax
            je L004ddee0
            cmp eax, 2
            jne L004ddfab
    L004ddee0:
            mov ecx, dword ptr [edi + 0x10]
            mov edx, dword ptr [edi + 0xc]
            mov ebp, dword ptr [esi + 8]
            push ecx
            push edx
            call FRandomBetween
            fld dword ptr [ebp + 4]
            fmul qword ptr [gDouble_0058abe0]
            fdivp st(1), st(0)
            fadd dword ptr [ebp + 0x6c]
            fstp dword ptr [ebp + 0x6c]
            mov eax, dword ptr [edi + 0x14]
            mov ebp, dword ptr [esi + 8]
            push eax
            call FRandomPosNeg
            fmul qword ptr [gDouble_0058abe8]
            fld dword ptr [ebp + 4]
            fdivp st(1), st(0)
            fadd dword ptr [ebp + 0x7c]
            fstp dword ptr [ebp + 0x7c]
            mov ecx, dword ptr [edi + 0x18]
            mov ebp, dword ptr [esi + 8]
            push ecx
            call FRandomPosNeg
            fmul qword ptr [gDouble_0058abe8]
            fld dword ptr [ebp + 4]
            fdivp st(1), st(0)
            fadd dword ptr [ebp + 0x74]
            fstp dword ptr [ebp + 0x74]
            mov edx, dword ptr [esi + 8]
            mov byte ptr [edx + 0xec], 0
            mov ecx, dword ptr [gPHIL_last_physics_tick]
            mov eax, dword ptr [esi + 8]
            add ecx, 0x28
            mov dword ptr [eax + 0x49c], ecx
            mov eax, dword ptr [esi + 0x1b8]
            test eax, eax
            jne L004ddfab
            xor ebx, ebx
    L004ddf63:
            fld dword ptr [edi + 0x2c]
            fmul dword ptr [gFLOAT_0058ac8c]
            call _ftol
            mov edx, eax
            xor ecx, ecx
            call IRandomBetween
            mov edx, ebx
            mov ecx, esi
            push eax
            call DoDamage
            inc ebx
            cmp ebx, 0xc
            jl L004ddf63
            mov eax, dword ptr [edi + 0x30]
            cmp esi, eax
            je L004ddfab
            mov dword ptr [esi + 0x13ec], eax
            call PDGetTotalTime
            xor edx, edx
            mov ecx, esi
            mov dword ptr [esi + 0x13e4], eax
            call CrashEarnings
    L004ddfab:
            mov eax, dword ptr [edi]
            test al, 0x20
            je L004ddfb7
            and al, 0xdf
            mov dword ptr [edi], eax
            jmp L004ddfdb
    L004ddfb7:
            test al, 2
            je L004ddfcc
            mov edx, dword ptr [esp + 0x10]
            mov ecx, dword ptr [edi + 4]
            cmp ecx, edx
            jae L004ddfe2
            and al, 0xfc
            mov dword ptr [edi], eax
            jmp L004ddfe2
    L004ddfcc:
            test al, 4
            je L004ddfe2
            mov eax, dword ptr [esp + 0x10]
            mov ecx, dword ptr [edi + 4]
            cmp ecx, eax
            jae L004ddfe2
    L004ddfdb:
            mov ecx, edi
            call MineExplode
    L004ddfe2:
            inc ebx
            cmp ebx, 0x14
            jl L004dde2b
            pop edi
            pop esi
            pop ebp
            pop ebx
            pop ecx
            ret
    L004ddff2:
            xor ebp, ebp
            jmp L004ddec6
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

// FUNCTION: CARMA2_HW 0x00502e00
void C2_HOOK_FASTCALL MaxOutAPO(void) {
    int i;

    for (i = 0; i < 3; i++) {
        gProgram_state.current_car.power_up_levels[i] = gInitial_powerup_slots[i];
        gProgram_state.current_car.power_up_slots[i] = gInitial_powerup_slots[i];
    }
}

void C2_HOOK_FASTCALL CheckRespawnQueue(tU32 pTime) {
    int i;

    for (i = 0; i < (int)CARPOCALYPSE2_ASIZE(gRespawn_powerups); i++) {
        tRespawn_powerup *respawn_powerup = &gRespawn_powerups[i];
        if (respawn_powerup->actor == NULL || (int)pTime < (int)respawn_powerup->respawn_time) {
            continue;
        }
        if (respawn_powerup->actor->render_style != BR_RSTYLE_FACES) {
            respawn_powerup->actor->render_style = BR_RSTYLE_FACES;
        }
        PipeSinglePowerupRespawn(respawn_powerup->actor, respawn_powerup->index);
        DoPowerupRespawnSmash(respawn_powerup->actor);
        respawn_powerup->actor = NULL;
    }
}

// FUNCTION: CARMA2_HW 0x004db880
void C2_NAKED C2_HOOK_FASTCALL MungePowerupStuff(undefined4 pArg1) {
#ifndef CARPOCALYPSE2_MATCHING
    tU32 time;

    time = GetTotalTime();
    MungeRepulseRays(time);
    CheckRespawnQueue(time);
#else
    __asm {
            push ebx
            push esi
            push edi
            call GetTotalTime
            mov edi, eax
            mov ecx, edi
            call MungeRepulseRays
            mov esi, offset gRespawn_powerups
            mov bl, 4
    L004db898:
            mov eax, dword ptr [esi]
            test eax, eax
            je L004db8c2
            cmp edi, dword ptr [esi + 8]
            jl L004db8c2
            cmp byte ptr [eax + 0x20], bl
            je L004db8ab
            mov byte ptr [eax + 0x20], bl
    L004db8ab:
            mov edx, dword ptr [esi + 4]
            mov ecx, dword ptr [esi]
            call PipeSinglePowerupRespawn
            mov ecx, dword ptr [esi]
            call DoPowerupRespawnSmash
            mov dword ptr [esi], 0
    L004db8c2:
            add esi, 0xc
            cmp esi, offset gMutant_tail_first_collision_info
            jl L004db898
            pop edi
            pop esi
            pop ebx
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
            nop
            nop
            nop
            nop
    }
#endif
}

tU32 C2_HOOK_FASTCALL GetNextGoodyTime(tU32 pTime) {

    return pTime + IRandomBetween(gNet_powerup_time_between_goodies_min, gNet_powerup_time_between_goodies_max);
}

__inline void C2_HOOK_FASTCALL AutoGoody(tU32 pTime) {

    if (gNext_goody_time == 0) {
        gNext_goody_time = GetNextGoodyTime(pTime);
    } else if (gNet_mode != eNet_mode_none
            && !gRace_finished
            && !gProgram_state.current_car.knackered
            && gCurrent_net_game->options.random_car_choice
            && pTime > gNext_goody_time) {
        gNext_goody_time = GetNextGoodyTime(pTime);
        GotPowerupEarwig(&gProgram_state.current_car, gNet_powerup_goodies[IRandomBetween(0, gNet_count_powerup_goodies - 1)], 2, 0);
    }
}

// FUNCTION: CARMA2_HW 0x004db630
__inline void C2_HOOK_FASTCALL LosePowerupX(tPowerup* pPowerup, undefined4 pArg2) {
    int i;

    (void)pArg2;
    if (pPowerup->got_time != 0 && pPowerup->lose_proc != NULL) {
        pPowerup->lose_proc(pPowerup, pPowerup->car);
    }
    if (pPowerup->net_type & 0x80) {
        ARPowerupLose(pPowerup, pPowerup->car);
    }
    if (gNet_mode != eNet_mode_none) {
        tNet_message* message;

        message = NetBuildGuaranteedMessage(0x16, 0);
        *(int*)((tU8*)message + 0x28) = 2;
        *(tPlayer_ID*)((tU8*)message + 0x1c) = gLocal_net_ID;
        *(int*)((tU8*)message + 0x20) = pPowerup - gPowerup_array;
        NetGuaranteedSendMessageToAllPlayers(gCurrent_net_game, message, NULL);
    }
    gProgram_state.current_car.powerups[pPowerup - gPowerup_array] = 0;
    if (pPowerup->initial_value == 0) {
        for (i = 0; i < gNumber_of_icons; i++) {
            if (pPowerup == gPickedup_powerups[i].powerup) {
                gPickedup_powerups[i].fizzle_stage = 3;
                gPickedup_powerups[i].fizzle_direction = -1;
                gPickedup_powerups[i].fizzle_start = GetTotalTime();
                break;
            }
        }
    }
    pPowerup->got_time = 0;
}

__inline void C2_HOOK_FASTCALL LosePowerup(tPowerup* pPowerup) {

    LosePowerupX(pPowerup, 1);
}

int C2_HOOK_FASTCALL GotPowerupXX(tCar_spec* pCar, int pArg2, int pArg3) {
    int value;

    value = 0;
    if (gNet_mode != eNet_mode_none) {
        if (pCar != NULL && pCar->driver ==  eDriver_local_human) {
            value = 1;
        }
    }
    return GotPowerupX(pCar, pArg2, value, pArg3, 0);
}

__inline void C2_HOOK_FASTCALL MungeKeyboardPowerup(void) {
    tPowerup* powerup;

    if (gInventory_selected < 0) {
        return;
    }
    if (gCountdown) {
        return;
    }
    if (gRace_finished) {
        return;
    }
    if (gProgram_state.current_car.knackered) {
        return;
    }
    powerup = &gPowerup_array[gInventory[gInventory_selected]];
    if (gINT_006a0a5c) {
        gINT_006a0a5c = KeyIsDown(61);
    } else if (gINT_006a0a6c != KeyIsDown(61)) {
        gINT_006a0a6c = !gINT_006a0a6c && !gProgram_state.current_car.knackered;
        if (gINT_006a0a6c) {
            int result;

            result = GotPowerupXX(&gProgram_state.current_car, gInventory[gInventory_selected], 1);
            if (result >= 0 && powerup->initial_value < 0) {
                powerup->value += 1;
                if (powerup->value >= 0) {
                    KeyboardPowerupFinished(powerup, 1);
                }
        } else
            LosePowerup(powerup);
        }
    }
    if (powerup->initial_value > 0 && gINT_006a0a6c) {
        powerup->value -= gFrame_period;
        if (powerup->value <= 0) {
            KeyboardPowerupFinished(powerup, 1);
        }
    }
}

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

// FUNCTION: CARMA2_HW 0x004e0de0
void C2_HOOK_FASTCALL PeriodicCloaking(void) {
    int i;

    if (gNet_mode != eNet_mode_none) {

        for (i = 0; i < gCount_cloaked_cars; i++) {
            DoCamouflageThing(gCloaked_cars[i]);
        }
    }
}

// FUNCTION: CARMA2_HW 0x004db1c0
void C2_NAKED C2_HOOK_FASTCALL DoPowerupPeriodics(tU32 pFrame_period) {
#ifndef CARPOCALYPSE2_MATCHING
    tU32 the_time;
    int i;
    tPowerup* powerup;

    the_time = GetTotalTime();
    AutoGoody(the_time);
    PeriodicCloaking();
    for (i = 0, powerup = gPowerup_array; i < gNumber_of_powerups; i++, powerup++) {
        if (powerup->got_time != 0) {
            if (powerup->type == ePowerup_timed && the_time >= powerup->lose_time && powerup->initial_value == 0) {
                LosePowerup(powerup);
            } else if (powerup->current_value == 0 && powerup->initial_value == 0) {
                LosePowerup(powerup);
            } else if (powerup->periodic_proc != NULL) {
                powerup->periodic_proc(powerup, pFrame_period);
            }
        }
    }
    MungeKeyboardPowerup();
#else
    __asm {
            sub esp, 0xc
            push ebx
            push ebp
            push esi
            push edi
            mov dword ptr [esp + 0x18], ecx
            call GetTotalTime
            mov esi, eax
            mov eax, dword ptr [gNext_goody_time]
            xor ebp, ebp
            mov dword ptr [esp + 0x14], esi
            cmp eax, ebp
            mov ebx, 2
            jne L004db200
            mov edx, dword ptr [gNet_powerup_time_between_goodies_max]
            mov ecx, dword ptr [gNet_powerup_time_between_goodies_min]
            call IRandomBetween
            add eax, esi
            mov dword ptr [gNext_goody_time], eax
            jmp L004db262
    L004db200:
            cmp dword ptr [gNet_mode], ebp
            je L004db262
            cmp dword ptr [gRace_finished], ebp
            jne L004db262
            cmp dword ptr [gProgram_state+0x280], ebp
            jne L004db262
            mov ecx, dword ptr [gCurrent_net_game]
            cmp dword ptr [ecx + 0x68], ebp
            je L004db262
            cmp esi, eax
            jbe L004db262
            mov edx, dword ptr [gNet_powerup_time_between_goodies_max]
            mov ecx, dword ptr [gNet_powerup_time_between_goodies_min]
            call IRandomBetween
            mov edx, dword ptr [gNet_count_powerup_goodies]
            add eax, esi
            push ebp
            push ebx
            dec edx
            xor ecx, ecx
            mov dword ptr [gNext_goody_time], eax
            call IRandomBetween
            mov ecx, dword ptr [gNet_powerup_goodies]
            mov edx, dword ptr [ecx + eax*4]
            mov ecx, offset gProgram_state+0xac
            call GotPowerupEarwig
    L004db262:
            cmp dword ptr [gNet_mode], ebp
            je L004db28e
            mov eax, dword ptr [gCount_cloaked_cars]
            xor esi, esi
            cmp eax, ebp
            jle L004db28e
            mov edi, offset gCloaked_cars
    L004db27a:
            mov ecx, dword ptr [edi]
            call DoCamouflageThing
            mov eax, dword ptr [gCount_cloaked_cars]
            inc esi
            add edi, 4
            cmp esi, eax
            jl L004db27a
    L004db28e:
            mov eax, dword ptr [gNumber_of_powerups]
            mov dword ptr [esp + 0x10], ebp
            cmp eax, ebp
            jle L004db4b4
            mov edx, dword ptr [gPowerup_array]
            lea edi, [edx + 0x40]
    L004db2a8:
            cmp dword ptr [edi - 0x3c], ebp
            je L004db497
            mov eax, dword ptr [edi - 0x40]
            lea esi, [edi - 0x40]
            cmp eax, ebx
            jne L004db399
            mov eax, dword ptr [esp + 0x14]
            mov ecx, dword ptr [edi - 0x34]
            cmp eax, ecx
            jb L004db399
            cmp dword ptr [edi - 0xc], ebp
            jne L004db399
            mov eax, dword ptr [edi]
            cmp eax, ebp
            je L004db2e4
            mov edx, dword ptr [edi + 0xc]
            mov ecx, esi
            call eax
    L004db2e4:
            test byte ptr [edi - 8], 0x80
            je L004db2f4
            mov edx, dword ptr [edi + 0xc]
            mov ecx, esi
            call ARPowerupLose
    L004db2f4:
            cmp dword ptr [gNet_mode], ebp
            je L004db33e
            xor edx, edx
            mov cl, 0x16
            call NetBuildGuaranteedMessage
            mov ecx, eax
            push ebp
            mov dword ptr [ecx + 0x28], ebx
            mov edx, dword ptr [gLocal_net_ID]
            mov dword ptr [ecx + 0x1c], edx
            mov eax, dword ptr [gPowerup_array]
            mov edx, esi
            sub edx, eax
            mov eax, 0x2fa0be83
            imul edx
            sar edx, 5
            mov eax, edx
            shr eax, 0x1f
            add edx, eax
            mov dword ptr [ecx + 0x20], edx
            mov edx, ecx
            mov ecx, dword ptr [gCurrent_net_game]
            call NetGuaranteedSendMessageToAllPlayers
    L004db33e:
            mov edx, dword ptr [gPowerup_array]
            mov eax, 0x2fa0be83
            sub esi, edx
            imul esi
            sar edx, 5
            mov ecx, edx
            shr ecx, 0x1f
            add edx, ecx
            mov dword ptr [edx*4 + gProgram_state+0x17bc], ebp
            mov eax, dword ptr [edi - 0xc]
            cmp eax, ebp
            jne L004db483
            mov edx, dword ptr [gNumber_of_icons]
            xor eax, eax
            cmp edx, ebp
            jle L004db483
            mov ecx, offset gPickedup_powerups
    L004db37e:
            lea esi, [edi - 0x40]
            cmp esi, dword ptr [ecx]
            je L004db45e
            inc eax
            add ecx, 0x14
            cmp eax, edx
            jl L004db37e
            mov dword ptr [edi - 0x3c], ebp
            jmp L004db497
    L004db399:
            cmp dword ptr [edi - 0x18], ebp
            jne L004db488
            cmp dword ptr [edi - 0xc], ebp
            jne L004db488
            mov eax, dword ptr [edi]
            cmp eax, ebp
            je L004db3b8
            mov edx, dword ptr [edi + 0xc]
            mov ecx, esi
            call eax
    L004db3b8:
            test byte ptr [edi - 8], 0x80
            je L004db3c8
            mov edx, dword ptr [edi + 0xc]
            mov ecx, esi
            call ARPowerupLose
    L004db3c8:
            cmp dword ptr [gNet_mode], ebp
            je L004db412
            xor edx, edx
            mov cl, 0x16
            call NetBuildGuaranteedMessage
            mov ecx, eax
            push ebp
            mov dword ptr [ecx + 0x28], ebx
            mov edx, dword ptr [gLocal_net_ID]
            mov dword ptr [ecx + 0x1c], edx
            mov eax, dword ptr [gPowerup_array]
            mov edx, esi
            sub edx, eax
            mov eax, 0x2fa0be83
            imul edx
            sar edx, 5
            mov eax, edx
            shr eax, 0x1f
            add edx, eax
            mov dword ptr [ecx + 0x20], edx
            mov edx, ecx
            mov ecx, dword ptr [gCurrent_net_game]
            call NetGuaranteedSendMessageToAllPlayers
    L004db412:
            mov edx, dword ptr [gPowerup_array]
            mov eax, 0x2fa0be83
            sub esi, edx
            imul esi
            sar edx, 5
            mov ecx, edx
            shr ecx, 0x1f
            add edx, ecx
            mov dword ptr [edx*4 + gProgram_state+0x17bc], ebp
            mov eax, dword ptr [edi - 0xc]
            cmp eax, ebp
            jne L004db483
            mov edx, dword ptr [gNumber_of_icons]
            xor eax, eax
            cmp edx, ebp
            jle L004db483
            mov ecx, offset gPickedup_powerups
    L004db44a:
            lea esi, [edi - 0x40]
            cmp esi, dword ptr [ecx]
            je L004db45e
            inc eax
            add ecx, 0x14
            cmp eax, edx
            jl L004db44a
            mov dword ptr [edi - 0x3c], ebp
            jmp L004db497
    L004db45e:
            lea esi, [eax + eax*4]
            shl esi, 2
            mov dword ptr [esi + gPickedup_powerups+0x4], 3
            mov dword ptr [esi + gPickedup_powerups+0x8], 0xffffffff
            call GetTotalTime
            mov dword ptr [esi + gPickedup_powerups+0xc], eax
    L004db483:
            mov dword ptr [edi - 0x3c], ebp
            jmp L004db497
    L004db488:
            mov eax, dword ptr [edi + 4]
            cmp eax, ebp
            je L004db497
            mov edx, dword ptr [esp + 0x18]
            mov ecx, esi
            call eax
    L004db497:
            mov eax, dword ptr [esp + 0x10]
            mov ecx, dword ptr [gNumber_of_powerups]
            inc eax
            add edi, 0xac
            cmp eax, ecx
            mov dword ptr [esp + 0x10], eax
            jl L004db2a8
    L004db4b4:
            mov eax, dword ptr [gInventory_selected]
            cmp eax, ebp
            jl L004db615
            cmp dword ptr [gCountdown], ebp
            jne L004db615
            cmp dword ptr [gRace_finished], ebp
            jne L004db615
            cmp dword ptr [gProgram_state+0x280], ebp
            jne L004db615
            mov ecx, dword ptr [eax*4 + gInventory]
            mov edi, 1
            mov eax, ecx
            shl eax, 3
            sub eax, ecx
            lea edx, [eax + eax*2]
            lea eax, [ecx + edx*2]
            mov ecx, dword ptr [gPowerup_array]
            lea esi, [ecx + eax*4]
            mov eax, dword ptr [gINT_006a0a5c]
            cmp eax, ebp
            mov ecx, 0x3d
            je L004db524
            call KeyIsDown
            mov dword ptr [gINT_006a0a5c], eax
            jmp L004db5eb
    L004db524:
            call KeyIsDown
            mov ecx, dword ptr [gINT_006a0a6c]
            cmp eax, ecx
            je L004db5eb
            cmp ecx, ebp
            jne L004db61d
            cmp dword ptr [gProgram_state+0x280], ebp
            jne L004db61d
            mov edx, dword ptr [gInventory_selected]
            mov eax, dword ptr [gNet_mode]
            cmp eax, ebp
            mov dword ptr [gINT_006a0a6c], edi
            mov edx, dword ptr [edx*4 + gInventory]
            je L004db57d
            mov eax, offset gProgram_state+0xac
            test eax, eax
            je L004db57d
            cmp dword ptr [gProgram_state+0xb8], 8
            jne L004db57d
            mov eax, edi
            jmp L004db57f
    L004db57d:
            xor eax, eax
    L004db57f:
            push ebp
            push edi
            push eax
            mov ecx, offset gProgram_state+0xac
            call GotPowerupX
            test eax, eax
            jl L004db5eb
            cmp dword ptr [esi + 0x34], ebp
            jge L004db5ee
            mov eax, dword ptr [esi + 0x30]
            inc eax
            mov dword ptr [esi + 0x30], eax
            js L004db5eb
            mov edx, edi
            mov ecx, esi
            call LosePowerupX
            mov ecx, dword ptr [gInventory_selected]
            or eax, 0xffffffff
            mov dword ptr [gInventory_selected], eax
            mov dword ptr [gINT_006a0a5c], edi
            mov dword ptr [ecx*4 + gInventory], eax
            xor ecx, ecx
            mov eax, offset gInventory
    L004db5c9:
            cmp dword ptr [eax], ebp
            jge L004db5da
            add eax, 4
            inc ecx
            cmp eax, offset gFizzle_in
            jl L004db5c9
            jmp L004db5eb
    L004db5da:
            mov dword ptr [gInventory_selected], ecx
            jmp L004db5eb
    L004db5e2:
            mov edx, edi
            mov ecx, esi
            call LosePowerupX
    L004db5eb:
            cmp dword ptr [esi + 0x34], ebp
    L004db5ee:
            jle L004db615
            cmp dword ptr [gINT_006a0a6c], ebp
            je L004db615
            mov edx, dword ptr [gFrame_period]
            mov ecx, dword ptr [esi + 0x30]
            sub ecx, edx
            mov eax, ecx
            mov dword ptr [esi + 0x30], ecx
            cmp eax, ebp
            jg L004db615
            mov edx, edi
            mov ecx, esi
            call KeyboardPowerupFinished
    L004db615:
            pop edi
            pop esi
            pop ebp
            pop ebx
            add esp, 0xc
            ret
    L004db61d:
            mov dword ptr [gINT_006a0a6c], ebp
            jmp L004db5e2
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

#undef PERIODIC_POWERUP

// FUNCTION: CARMA2_HW 0x004df650
void C2_HOOK_CDECL RenderSpinningPowerup(br_actor* actor, br_model* model, br_material* material, void* order_table, br_uint_8 style, int on_screen) {
    if (actor->user == NULL) {
        actor->user = (void*)(uintptr_t)GetTotalTime();
    } else {
        tU32 prev;
        tU32 now;
        tU32 diff;

        now = GetTotalTime();
        prev = (tU32)(uintptr_t)actor->user;
        diff = (now - prev) / 16;
        actor->user = (void*)(uintptr_t)now;
        BrMatrix34PreRotateY(&actor->t.t.mat, BR_ANGLE_DEG(diff));
    }
    BrZsModelRender(actor, model, material, order_table, style, on_screen, 0);
}

// FUNCTION: CARMA2_HW 0x004df570
void C2_HOOK_FASTCALL SetSpinningPowerup(br_actor* pActor, int pOpacity) {
    // GLOBAL: CARMA2_HW 0x0065ebc0
    static br_token_value spinning_powerup_extra_prim[] = {
        { BRT_BLEND_B, { 1 } },
        { BRT_OPACITY_X, { BrScalarToFixed(75) } },
    };
    unsigned int i;

    if (pActor == NULL) {
        return;
    }
    BrMatrix34PreTranslate(&pActor->t.t.mat, 0.f, .1f, 0.f);
    pActor->model->flags |= BR_MODF_CUSTOM;
    pActor->model->custom = RenderSpinningPowerup;
    if (pOpacity == 0 || pOpacity == 255) {
        return;
    }
    if (pActor->material != NULL) {
        pActor->material->extra_prim = spinning_powerup_extra_prim;
        pActor->material->extra_prim[1].v.x = pOpacity << 16;
        BrMaterialUpdate(pActor->material, BR_MATU_EXTRA_PRIM);
        return;
    }
    for (i = 0; i < pActor->model->nfaces; i++) {
        if (pActor->model->faces[i].material != NULL) {
            pActor->model->faces[i].material->extra_prim = spinning_powerup_extra_prim;
            pActor->model->faces[i].material->extra_prim[1].v.x = pOpacity << 16;
            BrMaterialUpdate(pActor->model->faces[i].material, BR_MATU_EXTRA_PRIM);
        }
    }
}

// FUNCTION: CARMA2_HW 0x004dfe10
void C2_HOOK_CDECL RenderChangingPowerup(br_actor* actor, br_model* model, br_material* material, void* order_table, br_uint_8 style, int on_screen) {
    if (actor->user == NULL) {
        actor->user = (void*)(uintptr_t)GetTotalTime();
        actor->model = gModel_powerup_armor;
    } else {
        tU32 prev;
        tU32 now;
        tU32 diff;

        now = GetTotalTime();
        prev = (tU32)(uintptr_t)actor->user;
        diff = (now - prev) / 16;
        if (diff > 15) {
            if (actor->model == gModel_powerup_armor) {
                actor->model = gModel_powerup_power;
            } else if (actor->model == gModel_powerup_power) {
                actor->model = gModel_powerup_offence;
            } else {
                actor->model = gModel_powerup_armor;
            }
            actor->user = (void*)(uintptr_t)now;
        }
        BrMatrix34PreRotateY(&actor->t.t.mat, BR_ANGLE_DEG(diff + 12));
    }
    BrZsModelRender(actor, model, material, order_table, style, on_screen, 0);
}

static __inline br_model* C2_HOOK_FASTCALL DuplicateModel(br_model* pModel, const char* pName) {
    br_model* copy;

    copy = BrModelAllocate(pName, pModel->nvertices, pModel->nfaces);
    if (copy == NULL) {
        return copy;
    }
    copy->pivot = pModel->pivot;
    copy->flags = pModel->flags;
    copy->user = pModel->user;
    copy->crease_angle = pModel->crease_angle;
    copy->radius = pModel->radius;
    copy->bounds = pModel->bounds;
    memcpy(copy->vertices, pModel->vertices, pModel->nvertices * sizeof(br_vertex));
    memcpy(copy->faces, pModel->faces, pModel->nfaces * sizeof(br_face));
    return copy;
}

// FUNCTION: CARMA2_HW 0x004df6c0
void C2_HOOK_FASTCALL SetChangingPowerup(br_actor* pActor) {
    tPath_name path;
    char errmsg[256];
    char s[256];

    if (pActor == NULL) {
        return;
    }
    if (gModel_powerup_armor == NULL || gModel_powerup_power == NULL || gModel_powerup_offence == NULL) {

        gModel_powerup_armor = DuplicateModel(BrModelFind("&68powerup1.ACT"), "PowArm");
        gModel_powerup_armor->flags |= BR_MODF_CUSTOM;
        gModel_powerup_armor->custom = RenderChangingPowerup;
        BrModelAdd(gModel_powerup_armor);

        gModel_powerup_power = DuplicateModel(BrModelFind("&70powerup1.ACT"), "PowPow");
        gModel_powerup_power->flags |= BR_MODF_CUSTOM;
        gModel_powerup_power->custom = RenderChangingPowerup;
        BrModelAdd(gModel_powerup_power);

        gModel_powerup_offence = DuplicateModel(BrModelFind("&69powerup1.ACT"), "PowOff");
        gModel_powerup_offence->flags |= BR_MODF_CUSTOM;
        gModel_powerup_offence->custom = RenderChangingPowerup;
        BrModelAdd(gModel_powerup_offence);
    }

    if (gModel_powerup_armor == NULL) {
        FILE* f;
        int i;
        br_actor* actor;

        BrModelFind("&77powerup.ACT");
        PathCat(path, gApplication_path, "POWRSHIT.TXT");
        f = DRfopen(path, "rt");
        if (f == NULL) {
            sprintf(errmsg, "Cant open %s", "POWRSHIT.TXT");
            PDFatalError(errmsg);
        }
        for (i = 0; i < 67; i++) {
            GetAString(f, s);
        }
        actor = LoadActor(s);
        if (actor != NULL && actor->model != NULL) {
            gModel_powerup_armor = DuplicateModel(actor->model, "PowArm");
            gModel_powerup_armor->flags |= BR_MODF_CUSTOM;
            gModel_powerup_armor->custom = RenderChangingPowerup;
            BrModelAdd(gModel_powerup_armor);
            BrActorFree(actor);
        }
        PFfclose(f);
    }

    if (gModel_powerup_power == NULL) {
        FILE* f;
        int i;
        br_actor* actor;

        PathCat(path, gApplication_path, "POWRSHIT.TXT");
        f = DRfopen(path, "rt");
        if (f == NULL) {
            sprintf(errmsg, "Cant open %s", "POWRSHIT.TXT");
            PDFatalError(errmsg);
        }
        for (i = 0; i < 68; i++) {
            GetAString(f, s);
        }
        actor = LoadActor(s);
        if (actor != NULL && actor->model != NULL) {
            gModel_powerup_power = DuplicateModel(actor->model, "PowPow");
            gModel_powerup_power->flags |= BR_MODF_CUSTOM;
            gModel_powerup_power->custom = RenderChangingPowerup;
            BrModelAdd(gModel_powerup_power);
            BrActorFree(actor);
        }
        PFfclose(f);
    }

    if (gModel_powerup_offence == NULL) {
        FILE* f;
        int i;
        br_actor* actor;

        PathCat(path, gApplication_path, "POWRSHIT.TXT");
        f = DRfopen(path, "rt");
        if (f == NULL) {
            sprintf(errmsg, "Cant open %s", "POWRSHIT.TXT");
            PDFatalError(errmsg);
        }
        for (i = 0; i < 69; i++) {
            GetAString(f, s);
        }
        actor = LoadActor(s);
        if (actor != NULL && actor->model != NULL) {
            gModel_powerup_offence = DuplicateModel(actor->model, "PowOff");
            gModel_powerup_offence->flags |= BR_MODF_CUSTOM;
            gModel_powerup_offence->custom = RenderChangingPowerup;
            BrModelAdd(gModel_powerup_offence);
            BrActorFree(actor);
        }
        PFfclose(f);
    }
    BrMatrix34PreTranslate(&pActor->t.t.mat, .0f, .1f, .0f);
    pActor->model = gModel_powerup_armor;
    pActor->model->flags |= BR_MODF_CUSTOM;
    pActor->model->custom = RenderChangingPowerup;
}

// FUNCTION: CARMA2_HW 0x004d8d30
int C2_HOOK_FASTCALL GotPowerup(tCar_spec* pCar, int pIndex) {
    GotPowerupEarwig(pCar, pIndex, 1, 1);
}

// FUNCTION: CARMA2_HW 0x004dc170
int C2_HOOK_FASTCALL GotCredits(tPowerup* powerup, tCar_spec* car) {
    int credits;
    char str[256];

    if (car != NULL && car->driver == eDriver_local_human) {
        credits = (IRandomBetween(powerup->integer_params[0], powerup->integer_params[1]) / 100) * 100;
        strcpy(str, powerup->message);
        strcat(str, " ");
        EarnCredits2(credits, str);
    }
    return powerup - gPowerup_array;
}

// FUNCTION: CARMA2_HW 0x004dc720
int C2_HOOK_FASTCALL SetPedSpeed(tPowerup* powerup, tCar_spec* car) {
    gPedestrian_speed_factor = powerup->float_params[0];
    return powerup - gPowerup_array;
}

// FUNCTION: CARMA2_HW 0x004dc750
int C2_HOOK_FASTCALL SetPedSize(tPowerup* powerup, tCar_spec* car) {
    br_vector3 scale;

    BrVector3Set(&scale, powerup->float_params[0], powerup->float_params[1], powerup->float_params[2]);
    ScalePeds(&scale);
    return powerup - gPowerup_array;
}

// FUNCTION: CARMA2_HW 0x004dc7d0
int C2_HOOK_FASTCALL SetPedExplode(tPowerup* powerup, tCar_spec* car) {
    gExploding_pedestrians = 1;
    return powerup - gPowerup_array;
}

// FUNCTION: CARMA2_HW 0x004de7a0
int C2_HOOK_FASTCALL PickAtRandom(tPowerup* powerup, tCar_spec* car) {
    int pIndex;

    pIndex = powerup->integer_params[IRandomBetween(0, powerup->number_of_integer_params - 1)];
    GotPowerupEarwig(car, pIndex, 1, 1);
    return -1;
}

// FUNCTION: CARMA2_HW 0x004dc830
int C2_HOOK_FASTCALL SetInvulnerability(tPowerup* powerup, tCar_spec* car) {
    car->invulnerable_no_crushage = 1;
    car->invulnerable_no_damage = 1;
    return powerup - gPowerup_array;
}

// FUNCTION: CARMA2_HW 0x004dc8a0
int C2_HOOK_FASTCALL SetFreeRepairs(tPowerup* powerup, tCar_spec* car) {
    if (car && car->driver == eDriver_local_human) {
        gFree_repairs = 1;
    }
    return powerup - gPowerup_array;
}

// FUNCTION: CARMA2_HW 0x004dc990
int C2_HOOK_FASTCALL DoInstantRepair(tPowerup* powerup, tCar_spec* car) {
    if (car && car->driver == eDriver_local_human) {
        TotallyRepairCar();
    }
    return powerup - gPowerup_array;
}

// FUNCTION: CARMA2_HW 0x004dc930
int C2_HOOK_FASTCALL FreezeTimer(tPowerup* powerup, tCar_spec* car) {
    if (car && car->driver == eDriver_local_human) {
        gFreeze_timer = !gFreeze_timer;
    }
    return powerup - gPowerup_array;
}

// FUNCTION: CARMA2_HW 0x004dca30
int C2_HOOK_FASTCALL SetEngineFactor(tPowerup* powerup, tCar_spec* car) {
    float f2;
    float f3;
    float f4;
    float f5;
    float f6;

    if (powerup->initial_value > 0) {
        PlayExclusiveFFBEffect("Afterburner", 1);
    }
    car->field_0x4d4 = powerup->float_params[0];
    car->grip_multiplier = powerup->float_params[1];
    f2 = powerup->float_params[2];
    f3 = powerup->float_params[3];
    f6 = powerup->float_params[6];
    f4 = powerup->float_params[4];
    f5 = powerup->float_params[5];
    SetCarSuspGiveAndHeight(car, f2, f3, f6, f4, f5);
    return powerup - gPowerup_array;
}

// FUNCTION: CARMA2_HW 0x004dcab0
int C2_HOOK_FASTCALL SetUnderwater(tPowerup* powerup, tCar_spec* car) {
    car->underwater_ability = 1;
    return powerup - gPowerup_array;
}

// FUNCTION: CARMA2_HW 0x004dc470
int C2_HOOK_FASTCALL GotTimeOrPower(tPowerup* powerup, tCar_spec* car) {
    if (gNet_mode) {
        return GotPowerupX(car, gNet_powerup_time_replacement, 1, 1, 0);
    }
    if (car != NULL && car->driver == eDriver_local_human) {
        AwardTime(IRandomBetween(powerup->integer_params[0], powerup->integer_params[1]));
    }
    return powerup - gPowerup_array;
}

// FUNCTION: CARMA2_HW 0x004dcae0
int C2_HOOK_FASTCALL TrashBodywork(tPowerup* powerup, tCar_spec* car) {
    TotallySpamTheModel(car, (float)(*(float*)((unsigned char*)car + 0x18d0) * gFLOAT_0058ac40 * gDOUBLE_0058ac48));
    if (car != NULL && car->driver == eDriver_local_human) {
        DRS3StartSound2(gXXX_outlet, 0x1388, 1, 0xff, 0xff, -1, -1);
        DRS3StartSound2(gXXX_outlet, 0x1389, 1, 0xff, 0xff, -1, -1);
        DRS3StartSound2(gXXX_outlet, 0x138a, 1, 0xff, 0xff, -1, -1);
    }
    return powerup - gPowerup_array;
}

// FUNCTION: CARMA2_HW 0x004dcb90
int C2_HOOK_FASTCALL TakeDrugs(tPowerup* powerup, tCar_spec* car) {
    if (car != NULL && car->driver == eDriver_local_human) {
        if (gDrug_trip_data == NULL) {
            gDrug_trip_data = BrMemAllocate(0x400, 0xfd);
            memcpy(gDrug_trip_data, gRender_palette->pixels, 0x400);
        }
        gOn_drugs = 1;
    }
    return powerup - gPowerup_array;
}

// FUNCTION: CARMA2_HW 0x004dcca0
int C2_HOOK_FASTCALL SetOpponentsSpeed(tPowerup* powerup, tCar_spec* car) {
    int i;

    for (i = 0; i < gCurrent_race.number_of_racers && gCurrent_race.opponent_list[i].index != 0x1d; i++) {
    }
    if (i < gCurrent_race.number_of_racers) {
        SetEngineFactor(gPowerup_array + 5, gCurrent_race.opponent_list[i].car_spec);
    }
    gOpponent_speed_factor = powerup->float_params[0];
    return powerup - gPowerup_array;
}

// FUNCTION: CARMA2_HW 0x004dcd70
int C2_HOOK_FASTCALL SetCopsSpeed(tPowerup* powerup, tCar_spec* car) {
    gCop_speed_factor = powerup->float_params[0];
    return powerup - gPowerup_array;
}

// FUNCTION: CARMA2_HW 0x004dcda0
int C2_HOOK_FASTCALL SetGravity(tPowerup* powerup, tCar_spec* car) {
    gGravity_multiplier = powerup->float_params[0];
    return powerup - gPowerup_array;
}

// FUNCTION: CARMA2_HW 0x004dcdd0
int C2_HOOK_FASTCALL SetPinball(tPowerup* powerup, tCar_spec* car) {
    gPinball_factor = powerup->float_params[0];
    return powerup - gPowerup_array;
}

// FUNCTION: CARMA2_HW 0x004dce00
int C2_HOOK_FASTCALL SetWallclimb(tPowerup* powerup, tCar_spec* car) {
    car->wall_climber_mode = 1;
    return powerup - gPowerup_array;
}

// FUNCTION: CARMA2_HW 0x004dce30
int C2_HOOK_FASTCALL SetBouncey(tPowerup* powerup, tCar_spec* car) {
    car->bounce_rate = gBounce_rate_const / powerup->float_params[0];
    car->bounce_amount = powerup->float_params[1];
    car->last_bounce = 0;
    return powerup - gPowerup_array;
}

// FUNCTION: CARMA2_HW 0x004dce80
int C2_HOOK_FASTCALL SetSuspension(tPowerup* powerup, tCar_spec* car) {
    SetCarSuspGiveAndHeight(car,
                            ((volatile float*)powerup->float_params)[0],
                            ((volatile float*)powerup->float_params)[1],
                            ((volatile float*)powerup->float_params)[4],
                            ((volatile float*)powerup->float_params)[2],
                            ((volatile float*)powerup->float_params)[3]);
    return powerup - gPowerup_array;
}

// FUNCTION: CARMA2_HW 0x004dced0
int C2_HOOK_FASTCALL SetTyreGrip(tPowerup* powerup, tCar_spec* car) {
    car->grip_multiplier = powerup->float_params[0];
    return powerup - gPowerup_array;
}

// FUNCTION: CARMA2_HW 0x004dcf80
int C2_HOOK_FASTCALL SetDamageMultiplier(tPowerup* powerup, tCar_spec* car) {
    car->damage_multiplier = powerup->float_params[0];
    return powerup - gPowerup_array;
}

// FUNCTION: CARMA2_HW 0x004dcf00
int C2_HOOK_FASTCALL SetImmortalPeds(tPowerup* powerup, tCar_spec* car) {
    gImmortal_peds = 1;
    return powerup - gPowerup_array;
}

// FUNCTION: CARMA2_HW 0x004dc8f0
int C2_HOOK_FASTCALL SetStupidPedestrians(tPowerup* powerup, tCar_spec* car) {
    gBlind_pedestrians = 1;
    return powerup - gPowerup_array;
}

// FUNCTION: CARMA2_HW 0x004dcf40
int C2_HOOK_FASTCALL SetSuicidalPedestrians(tPowerup* powerup, tCar_spec* car) {
    gPeds_suicidal = 1;
    return powerup - gPowerup_array;
}

// FUNCTION: CARMA2_HW 0x004de7d0
int C2_HOOK_FASTCALL GotVouchers(tPowerup* powerup, tCar_spec* car) {
    if (car && car->driver == eDriver_local_human) {
        AddVouchers(*powerup->integer_params);
    }
    return powerup - gPowerup_array;
}

// FUNCTION: CARMA2_HW 0x004debb0
int C2_HOOK_FASTCALL SetMassMultiplier(tPowerup* powerup, tCar_spec* car) {
    car->field_0x4c8 = powerup->float_params[0];
    if (gNet_mode) {
        RecalculateCarMassMomentOfInertia(car);
    }
    return powerup - gPowerup_array;
}

// FUNCTION: CARMA2_HW 0x004de820
int C2_HOOK_FASTCALL SetInstantHandbrake(tPowerup* powerup, tCar_spec* car) {
    if (car && car->driver == eDriver_local_human) {
        gInstant_handbrake = 1;
    }
    return powerup - gPowerup_array;
}

// FUNCTION: CARMA2_HW 0x004dec10
int C2_HOOK_FASTCALL ShowPedestrians(tPowerup* powerup, tCar_spec* car) {
    if (car && car->driver == eDriver_local_human) {
        gShow_peds_on_map = 1;
    }
    return powerup - gPowerup_array;
}

// FUNCTION: CARMA2_HW 0x004dea40
int C2_NAKED C2_HOOK_FASTCALL HitMine(tPowerup* powerup, tCar_spec* car) {
#ifndef CARPOCALYPSE2_MATCHING
    int j;
    int max_damage;
    float f;

    if (car->field_0x4c8 > gDOUBLE_0058ac18
            || (gNet_mode != eNet_mode_none
                && NetPlayerFromCar(car) != NULL
                && NetPlayerFromCar(car)->field_0x80 != 0)) {
        return powerup - gPowerup_array;
    }
    car->collision_info->v.v[1] += FRandomBetween(powerup->float_params[0], powerup->float_params[1])
        / (car->collision_info->M * gDouble_0058abe0);
    car->collision_info->omega.v[2] += FRandomPosNeg(powerup->float_params[2]) * gDouble_0058abe8 / car->collision_info->M;
    car->collision_info->omega.v[0] += FRandomPosNeg(powerup->float_params[3]) * gDouble_0058abe8 / car->collision_info->M;
    if (car != NULL) {
      if (car->driver > 5) {
        f = *(float*)((unsigned char*)car + 0x18d0) * gDOUBLE_0058ac48;
        if (!car->invulnerable_no_damage) {
            j = 0;
            max_damage = (int)(f * gFLOAT_0058ac8c);
            for (; j < (int)CARPOCALYPSE2_ASIZE(car->damage_units); j++) {
                DamageUnit(car, j, IRandomBetween(0, max_damage));
            }
        }
        if (!car->invulnerable_no_crushage) {
            TotallySpamTheModel(car, f * gFLOAT_0058ac40);
        }
      }
    }
    car->collision_info->disable_move_rotate = 0;
    return powerup - gPowerup_array;
#else
    __asm {
            push ecx
            push ebx
            push ebp
            push esi
            mov esi, edx
            push edi
            mov ebx, ecx
            fld dword ptr [esi + 0x4c8]
            fcomp qword ptr [gDOUBLE_0058ac18]
            fnstsw ax
            test ah, 0x41
            je L004deb86
            mov eax, dword ptr [gNet_mode]
            test eax, eax
            je L004dea89
            mov ecx, esi
            call NetPlayerFromCar
            test eax, eax
            je L004dea89
            mov ecx, esi
            call NetPlayerFromCar
            mov ecx, dword ptr [eax + 0x80]
            test ecx, ecx
            jne L004deb86
    L004dea89:
            mov eax, dword ptr [ebx + 0x48]
            mov edi, dword ptr [esi + 8]
            mov ecx, dword ptr [eax + 4]
            mov edx, dword ptr [eax]
            push ecx
            push edx
            call FRandomBetween
            fld dword ptr [edi + 4]
            fmul qword ptr [gDouble_0058abe0]
            fdivp st(1), st(0)
            fadd dword ptr [edi + 0x6c]
            fstp dword ptr [edi + 0x6c]
            mov eax, dword ptr [ebx + 0x48]
            mov edi, dword ptr [esi + 8]
            mov ecx, dword ptr [eax + 8]
            push ecx
            call FRandomPosNeg
            fmul qword ptr [gDouble_0058abe8]
            fld dword ptr [edi + 4]
            fdivp st(1), st(0)
            fadd dword ptr [edi + 0x7c]
            fstp dword ptr [edi + 0x7c]
            mov edx, dword ptr [ebx + 0x48]
            mov edi, dword ptr [esi + 8]
            mov eax, dword ptr [edx + 0xc]
            push eax
            call FRandomPosNeg
            fmul qword ptr [gDouble_0058abe8]
            fld dword ptr [edi + 4]
            fdivp st(1), st(0)
            test esi, esi
            fadd dword ptr [edi + 0x74]
            fstp dword ptr [edi + 0x74]
            je L004deb5b
            cmp dword ptr [esi + 0xc], 5
            jle L004deb5b
            fld dword ptr [esi + 0x18d0]
            fmul qword ptr [gDOUBLE_0058ac48]
            mov eax, dword ptr [esi + 0x1b8]
            test eax, eax
            fstp dword ptr [esp + 0x10]
            jne L004deb3c
            fld dword ptr [esp + 0x10]
            fmul dword ptr [gFLOAT_0058ac8c]
            xor edi, edi
            call _ftol
            mov ebp, eax
    L004deb23:
            mov edx, ebp
            xor ecx, ecx
            call IRandomBetween
            mov edx, edi
            mov ecx, esi
            push eax
            call DamageUnit
            inc edi
            cmp edi, 0xc
            jl L004deb23
    L004deb3c:
            mov eax, dword ptr [esi + 0x1b4]
            test eax, eax
            jne L004deb5b
            fld dword ptr [esp + 0x10]
            fmul dword ptr [gFLOAT_0058ac40]
            push ecx
            mov ecx, esi
            fstp dword ptr [esp]
            call TotallySpamTheModel
    L004deb5b:
            mov ecx, dword ptr [esi + 8]
            mov eax, 0x2fa0be83
            mov byte ptr [ecx + 0xec], 0
            mov ebp, dword ptr [gPowerup_array]
            sub ebx, ebp
            imul ebx
            mov eax, edx
            sar eax, 5
            mov edx, eax
            shr edx, 0x1f
            add eax, edx
            pop edi
            pop esi
            pop ebp
            pop ebx
            pop ecx
            ret
    L004deb86:
            mov ebp, dword ptr [gPowerup_array]
            mov eax, 0x2fa0be83
            sub ebx, ebp
            pop edi
            imul ebx
            mov eax, edx
            pop esi
            sar eax, 5
            mov ecx, eax
            pop ebp
            shr ecx, 0x1f
            add eax, ecx
            pop ebx
            pop ecx
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

// FUNCTION: CARMA2_HW 0x004dec60
int C2_HOOK_FASTCALL SetElectroBastard(tPowerup* powerup, tCar_spec* car) {
    car->proxy_ray_distance = powerup->float_params[0] * powerup->float_params[0];
    return powerup - gPowerup_array;
}

// FUNCTION: CARMA2_HW 0x004dec90
void C2_HOOK_FASTCALL ResetElectroBastard(tPowerup* powerup, tCar_spec* car) {
    car->proxy_ray_distance = 0.0f;
}

// FUNCTION: CARMA2_HW 0x004dc7a0
int C2_HOOK_FASTCALL SetPedHeadSize(tPowerup* powerup, tCar_spec* car) {
    br_scalar head_scale;

    head_scale = powerup->float_params[0];
    ScalePedHeads(head_scale);
    return powerup - gPowerup_array;
}

// FUNCTION: CARMA2_HW 0x004deca0
int C2_HOOK_FASTCALL SetMutantCorpses(tPowerup* powerup, tCar_spec* car) {
    gMutant_speed = powerup->float_params[0];
    return powerup - gPowerup_array;
}

// FUNCTION: CARMA2_HW 0x004dd700
int C2_NAKED C2_HOOK_FASTCALL ShitMine(tPowerup* powerup, tCar_spec* car) {
#ifndef CARPOCALYPSE2_MATCHING
    int i;
    tShit_mine* mine = NULL;
    br_vector3 drop_pos;
    br_vector3* car_pos;
    br_vector3* rear_dir;
    int mortar;

    if (car == NULL || car->car_master_actor == NULL) {
        return -1;
    }
    for (i = 0; i < CARPOCALYPSE2_ASIZE(gShit_mines); i++) {
        if (!(gShit_mines[i].flags & 0x1)) {
            mine = &gShit_mines[i];
            break;
        }
    }
    if (mine == NULL) {
        return -1;
    }
    mortar = powerup->number_of_integer_params > 0 ? powerup->integer_params[0] : 0;
    /* +m[2] points rearward (car faces -m[2]). */
    car_pos = (br_vector3*)car->car_master_actor->t.t.mat.m[3];
    rear_dir = (br_vector3*)car->car_master_actor->t.t.mat.m[2];
    BrVector3Copy(&drop_pos, car_pos);
    if (mortar) {
        BrVector3Scale(&drop_pos, rear_dir, -3.0f);
        BrVector3Accumulate(&drop_pos, car_pos);
        drop_pos.v[1] += 1.0f;
    } else {
        BrVector3Scale(&drop_pos, rear_dir, 2.5f);
        BrVector3Accumulate(&drop_pos, car_pos);
    }
    BrVector3Copy((br_vector3*)&mine->collision_info->transform_matrix.m[3], &drop_pos);
    BrVector3Copy(&mine->collision_info->actor->t.t.translate.t, &drop_pos);
    BrVector3Copy(&mine->collision_info->v, &car->collision_info->v);
    if (mortar) {
        mine->collision_info->v.v[1] += powerup->float_params[6];
        BrVector3Scale(&drop_pos, rear_dir, -powerup->float_params[7]);
        BrVector3Accumulate(&mine->collision_info->v, &drop_pos);
    }
    mine->collision_info->disable_move_rotate = 0;
    mine->field3_0xc = powerup->float_params[1];
    mine->initial_y_speed_factor = powerup->float_params[2];
    mine->initial_z_omega_factor = powerup->float_params[3];
    mine->initial_x_omega_factor = powerup->float_params[4];
    mine->max_damage = 1.0f;
    mine->field_0x30 = car;
    mine->car = NULL;
    mine->flags = 0x1 | 0x4;
    mine->next_think_time = GetTotalTime() + (tU32)(powerup->float_params[0] * 1000.f);
    mine->collision_info->actor->render_style = BR_RSTYLE_FACES;
    if (mine->collision_info->actor->parent == NULL) {
        BrActorAdd(gUniverse_actor, mine->collision_info->actor);
    }
    PHILAddObject(mine->collision_info);
    return powerup - gPowerup_array;
#else
    __asm {
            sub esp, 0x44
            mov eax, dword ptr [gNet_mode]
            push ebx
            push ebp
            push esi
            push edi
            mov edi, edx
            xor esi, esi
            cmp eax, 3
            mov dword ptr [esp + 0x14], edi
            mov ebp, ecx
            je L004dda7d
            xor ecx, ecx
            mov eax, offset gShit_mines
    L004dd726:
            test byte ptr [eax], 1
            je L004dd738
            add eax, 0x38
            inc ecx
            cmp eax, offset gCount_mutant_tail_parts
            jl L004dd726
            jmp L004dd746
    L004dd738:
            mov eax, ecx
            shl eax, 3
            sub eax, ecx
            lea esi, [eax*8 + gShit_mines]
    L004dd746:
            test esi, esi
            jne L004dd7a9
            call GetTotalTime
            mov ecx, dword ptr [ebp + 0x48]
            mov ebx, eax
            _emit 0xd9
        _emit 0x01
            fmul qword ptr [gDOUBLE_0058ac78]
            call _ftol
            or edi, 0xffffffff
            mov edx, offset gShit_mines+0x4
    L004dd769:
            mov ecx, dword ptr [edx - 4]
            test cl, 4
            je L004dd782
            test cl, 2
            jne L004dd782
            mov ecx, dword ptr [edx]
            sub ecx, ebx
            cmp ecx, eax
            jae L004dd782
            mov eax, ecx
            mov edi, esi
    L004dd782:
            add edx, 0x38
            inc esi
            cmp edx, offset gCount_mutant_tail_parts+0x4
            jl L004dd769
            cmp edi, -1
            je L004dda7d
            mov edx, edi
            shl edx, 3
            sub edx, edi
            mov edi, dword ptr [esp + 0x14]
            lea esi, [edx*8 + gShit_mines]
    L004dd7a9:
            push edi
            mov edx, ebp
            mov ecx, esi
            call ConfigureShitMine
            mov cl, byte ptr [esi + 0x35]
            mov edx, edi
            inc cl
            mov dword ptr [esi + 0x30], edi
            mov byte ptr [esi + 0x35], cl
            mov ecx, esi
            call InitializeMineMotion
            mov eax, dword ptr [gNet_mode]
            test eax, eax
            jne L004dd7da
            mov edx, dword ptr [esi + 8]
            mov ecx, dword ptr [edx]
            call PipeSingleMatrix34Type66
    L004dd7da:
            mov al, byte ptr [esi]
            push 0x10000
            push 0x10000
            push 0xff
            test al, 0x40
            push 1
            push offset gZero_v__car
            je L004dd806
            mov eax, dword ptr [esi + 8]
            mov edx, 0x151d
            mov ecx, dword ptr [eax]
            add ecx, 0x50
            push ecx
            jmp L004dd814
    L004dd806:
            mov edx, dword ptr [esi + 8]
            mov eax, dword ptr [edx]
            mov edx, 0x151c
            add eax, 0x50
            push eax
    L004dd814:
            mov ecx, dword ptr [gXXX_outlet]
            call DRS3StartSound3D
            mov eax, dword ptr [esi + 0x30]
            test eax, eax
            je L004dd838
            cmp dword ptr [eax + 0xc], 8
            jne L004dd838
            xor edx, edx
            mov ecx, offset gMass_mine+0xb0
            call PlayExclusiveFFBEffect
    L004dd838:
            mov eax, dword ptr [edi + 8]
            lea ecx, [esp + 0x30]
            lea edx, [eax + 0x8c]
            add eax, 0x14
            push edx
            push eax
            push ecx
            call BrMatrix34ApplyP
            mov eax, dword ptr [esi + 8]
            add esp, 0xc
            lea edx, [esp + 0x3c]
            lea ecx, [eax + 0x8c]
            add eax, 0x14
            push ecx
            push eax
            push edx
            call BrMatrix34ApplyP
            fld dword ptr [esp + 0x4c]
            fld dword ptr [esp + 0x48]
            fsub dword ptr [esp + 0x3c]
            fxch st(1)
            fsub dword ptr [esp + 0x40]
            fld dword ptr [esp + 0x50]
            fsub dword ptr [esp + 0x44]
            fld st(2)
            fmul st(0), st(3)
            fld st(2)
            fmul st(0), st(3)
            fxch st(2)
            fst dword ptr [esp + 0x2c]
            fmul dword ptr [esp + 0x2c]
            fxch st(2)
            faddp st(1), st(0)
            fxch st(1)
            add esp, 0xc
            faddp st(1), st(0)
            fsqrt
            fcom dword ptr [gFLOAT_0058ac10]
            fnstsw ax
            test ah, 0x41
            jne L004dd8db
            fdivr qword ptr [gDOUBLE_0058ac18]
            fld st(0)
            fld st(1)
            fxch st(1)
            fmul st(0), st(4)
            fxch st(1)
            fmul st(0), st(3)
            fxch st(2)
            fmul dword ptr [esp + 0x20]
            fxch st(1)
            fstp dword ptr [esp + 0x24]
            fxch st(1)
            fstp dword ptr [esp + 0x28]
            fstp dword ptr [esp + 0x2c]
            jmp L004dd8f5
    L004dd8db:
            fstp st(0)
            mov dword ptr [esp + 0x24], 0x3f800000
            mov dword ptr [esp + 0x28], 0
            mov dword ptr [esp + 0x2c], 0
    L004dd8f5:
            fld dword ptr [esp + 0x24]
            fld dword ptr [esp + 0x28]
            fld dword ptr [esp + 0x2c]
            fxch st(2)
            fmul qword ptr [gDOUBLE_0058ac68]
            fxch st(1)
            fmul qword ptr [gDOUBLE_0058ac68]
            fld st(3)
            fxch st(3)
            fmul qword ptr [gDOUBLE_0058ac68]
            fxch st(5)
            faddp st(2), st(0)
            fxch st(2)
            faddp st(2), st(0)
            mov edx, dword ptr [gList_collision_infos]
            lea eax, [esp + 0x18]
            push offset gWorld_callbacks
            push eax
            fstp dword ptr [esp + 0x20]
            fxch st(2)
            fadd dword ptr [esp + 0x28]
            fxch st(2)
            fstp dword ptr [esp + 0x24]
            fxch st(1)
            fstp dword ptr [esp + 0x28]
            mov ecx, dword ptr [esi + 8]
            fstp st(0)
            call TestForObjectInSensiblePlace
            test eax, eax
            jne L004dd96a
            mov edx, dword ptr [gSensible_place_blocker]
            mov ecx, dword ptr [esi + 8]
            push 0x3f800000
            jmp L004dda78
    L004dd96a:
            mov ecx, dword ptr [edi + 8]
            xor eax, eax
            xor edx, edx
            mov ax, word ptr [ecx + 0x174]
            mov dx, word ptr [ecx + 0x176]
            sub edx, eax
            lea ecx, [esp + 0x48]
            lea eax, [eax + eax*4]
            push edx
            shl eax, 4
            add eax, offset gFace_list__car
            lea edx, [esp + 0x14]
            push eax
            push ecx
            lea eax, [esp + 0x30]
            push edx
            push eax
            lea edx, [esp + 0x2c]
            lea ecx, [esp + 0x44]
            call FUN_004226d0
            test eax, eax
            jne L004dd9f4
            mov ecx, dword ptr [esi + 8]
            xor edx, edx
            mov ax, word ptr [ecx + 0x174]
            mov dx, word ptr [ecx + 0x176]
            sub edx, eax
            lea ecx, [esp + 0x48]
            lea eax, [eax + eax*4]
            push edx
            shl eax, 4
            add eax, offset gFace_list__car
            lea edx, [esp + 0x14]
            push eax
            push ecx
            lea eax, [esp + 0x30]
            push edx
            push eax
            lea edx, [esp + 0x2c]
            lea ecx, [esp + 0x44]
            call FUN_004226d0
            test eax, eax
            je L004dda7d
    L004dd9f4:
            fld dword ptr [gFLOAT_0058ac20]
            fsub dword ptr [esp + 0x10]
            fld dword ptr [esp + 0x18]
            fld dword ptr [esp + 0x1c]
            fxch st(2)
            fchs
            fsub qword ptr [gDOUBLE_0058ac80]
            fld dword ptr [esp + 0x20]
            fxch st(2)
            fmul st(0), st(1)
            fxch st(3)
            fmul st(0), st(1)
            fxch st(2)
            fmul st(0), st(1)
            fxch st(3)
            fstp dword ptr [esp + 0x18]
            fxch st(1)
            fstp dword ptr [esp + 0x1c]
            fxch st(1)
            fstp dword ptr [esp + 0x20]
            mov eax, dword ptr [esi + 8]
            push 0x3f800000
            fstp st(0)
            fld dword ptr [esp + 0x1c]
            fadd dword ptr [eax + 0xb0]
            fstp dword ptr [eax + 0xb0]
            mov eax, dword ptr [esi + 8]
            fld dword ptr [esp + 0x20]
            fadd dword ptr [eax + 0xb4]
            fstp dword ptr [eax + 0xb4]
            mov eax, dword ptr [esi + 8]
            fld dword ptr [esp + 0x24]
            fadd dword ptr [eax + 0xb8]
            fstp dword ptr [eax + 0xb8]
            mov edx, dword ptr [esi + 8]
            mov ecx, dword ptr [edi + 8]
    L004dda78:
            call CollideShitMine
    L004dda7d:
            mov ebx, dword ptr [gPowerup_array]
            mov eax, 0x2fa0be83
            sub ebp, ebx
            pop edi
            mov ecx, ebp
            pop esi
            imul ecx
            mov eax, edx
            pop ebp
            sar eax, 5
            mov ecx, eax
            pop ebx
            shr ecx, 0x1f
            add eax, ecx
            add esp, 0x44
            ret
    }
#endif
}

// FUNCTION: CARMA2_HW 0x004de040
int C2_HOOK_FASTCALL PissOutOil(tPowerup* powerup, tCar_spec* car) {

    PlayExclusiveFFBEffect("Oil_Slick", 0);
    QueueOilSpill2(NULL, car->car_master_actor, car->car_actor->model,
        0, 0.35f, 0.6f, 9.0e-5f, 0.0003f, 0.2f, 1.5f, 2.5f, 0.01f, 0.15f,
        oily_material, 0);
    return powerup - gPowerup_array;
}

// FUNCTION: CARMA2_HW 0x004de0c0
int C2_HOOK_FASTCALL KangerooJump(tPowerup* powerup, tCar_spec* car) {

    PlayExclusiveFFBEffect("Kangeroo", 0);
    PratcamEvent(0x2a);
    *(float*)((tU8*)car->collision_info + 0x6c) = powerup->float_params[0] * gConst_kangeroo_force_factor + *(float*)((tU8*)car->collision_info + 0x6c);
    if (car != NULL && car->driver == eDriver_local_human) {
        DRS3StartSound(gXXX_outlet, 0x2332);
    }
    *((tU8*)car->collision_info + 0xec) = 0;
    return powerup - gPowerup_array;
}

// FUNCTION: CARMA2_HW 0x004de140
int C2_HOOK_FASTCALL AnnihilatePeds(tPowerup* powerup, tCar_spec* car) {
    int result;

    if (FindPedestrianNearCar(car, powerup->float_params[0]) != 0) {
        result = powerup - gPowerup_array;
    } else {
        result = -1;
    }
    if (result != -1) {
        PlayExclusiveFFBEffect("Ped_Ray", 0);
    }
    return result;
}

#pragma auto_inline(off)
// FUNCTION: CARMA2_HW 0x004de1f0
static int C2_NAKED C2_HOOK_FASTCALL RepulseOpponents2(tPowerup* powerup, tCar_spec* car, int pTell_net) {
#ifndef CARPOCALYPSE2_MATCHING
    int count;
    int i;
    int hits;
    float length;
    float dist;
    int j;
    tU32 time;
    tCar_spec* other;
    br_vector3* other_pos;
    br_vector3 delta;
    br_vector3 dir;
    br_vector3 spin;
    float inverse_distance;
    float scale;
    float impulse;
    tNet_message* message;

    hits = 0;
    time = GetTotalTime();
    for (i = eVehicle_self; i <= eVehicle_drone; i++) {
        count = (i == eVehicle_self) ? 1 : GetCarCount(i);
        for (j = 0; j < count; j++) {
            other = (i == eVehicle_self) ? &gProgram_state.current_car : GetCarSpec(i, j);
            if (other == car) {
                continue;
            }
            other_pos = &other->pos;
            delta.v[0] = other->pos.v[0] - car->pos.v[0];
            delta.v[1] = other->pos.v[1] - car->pos.v[1];
            delta.v[2] = other->pos.v[2] - car->pos.v[2];
            length = BrVector3Length(&delta);
            dist = length;
            if (dist >= powerup->float_params[3]) {
                continue;
            }
            hits++;
            if (gNet_mode != eNet_mode_client) {
                scale = (powerup->float_params[3] - dist) / powerup->float_params[3] / other->field_0x4c8;
                if (dist > gFLOAT_0058ac10) {
                    inverse_distance = gDOUBLE_0058ac18 / dist;
                    dir.v[0] = delta.v[0] * inverse_distance;
                    dir.v[1] = delta.v[1] * inverse_distance;
                    dir.v[2] = delta.v[2] * inverse_distance;
                } else {
                    dir.v[0] = gFLOAT_0058ac20;
                    dir.v[1] = 0.0f;
                    dir.v[2] = 0.0f;
                }
                if (fabs((double)dir.v[1]) < gDOUBLE_0058ac80) {
                    dir.v[1] = powerup->float_params[1];
                }
                impulse = scale * powerup->float_params[0];
                other->collision_info->v.v[0] += dir.v[0] * impulse;
                other->collision_info->v.v[1] += dir.v[1] * impulse;
                other->collision_info->v.v[2] += dir.v[2] * impulse;
                impulse = scale * powerup->float_params[2];
                spin.v[0] = FRandomPosNeg(impulse);
                spin.v[1] = FRandomPosNeg(impulse);
                spin.v[2] = FRandomPosNeg(impulse);
                other->collision_info->omega.v[0] += spin.v[0];
                other->collision_info->omega.v[1] += spin.v[1];
                other->collision_info->omega.v[2] += spin.v[2];
                other->collision_info->disable_move_rotate = 0;
                other->collision_info->field_0x49c = gPHIL_last_physics_tick + 0x28;
                other->time_last_hit = PDGetTotalTime();
                other->last_hit_by = car;
            }
            AddRepulseLink(time, car, other, &car->pos, other_pos, 0.06f, 0.12f);
        }
    }
    if (gNet_mode != eNet_mode_none && pTell_net) {
        message = NetBuildGuaranteedMessage(0x23, 0);
        message->contents.powerup.player_id = gLocal_net_ID;
        message->contents.powerup.powerup_index = powerup - gPowerup_array;
        NetGuaranteedSendMessageToAllPlayers(gCurrent_net_game, message, 0);
    }
    if (hits != 0 && car != NULL && car->driver == eDriver_local_human) {
        DRS3StartSound3D(gXXX_outlet, 0x1324, &car->pos, &gZero_v__car, 1, 255, BR_FIXED_INT(1), BR_FIXED_INT(1));
    }
    if (hits != 0) {
        return powerup - gPowerup_array;
    }
    return -1;
#else
    __asm {
            sub esp, 0x44
            push ebx
            push ebp
            push esi
            push edi
            mov edi, ecx
            xor ebx, ebx
            mov ebp, edx
            mov dword ptr [esp + 0x28], edi
            mov dword ptr [esp + 0x1c], ebx
            call GetTotalTime
            mov dword ptr [esp + 0x2c], eax
            mov dword ptr [esp + 0x18], ebx
    L004de212:
            test ebx, ebx
            jne L004de220
            mov dword ptr [esp + 0x14], 1
            jmp L004de22b
    L004de220:
            mov ecx, ebx
            call GetCarCount
            mov dword ptr [esp + 0x14], eax
    L004de22b:
            mov eax, dword ptr [esp + 0x14]
            xor edx, edx
            test eax, eax
            mov dword ptr [esp + 0x24], edx
            jle L004de465
    L004de23d:
            test ebx, ebx
            jne L004de248
            mov esi, offset gProgram_state+0xac
            jmp L004de251
    L004de248:
            mov ecx, ebx
            call GetCarSpec
            mov esi, eax
    L004de251:
            cmp ebp, esi
            je L004de450
            fld dword ptr [esi + 0x50]
            fld dword ptr [esi + 0x4c]
            fsub dword ptr [ebp + 0x4c]
            fxch st(1)
            fsub dword ptr [ebp + 0x50]
            lea ebx, [esi + 0x4c]
            mov ecx, dword ptr [edi + 0x48]
            fld dword ptr [esi + 0x54]
            fsub dword ptr [ebp + 0x54]
            fxch st(2)
            fst dword ptr [esp + 0x30]
            fxch st(1)
            fst dword ptr [esp + 0x34]
            fxch st(1)
            fmul dword ptr [esp + 0x30]
            fxch st(2)
            fstp dword ptr [esp + 0x38]
            fmul dword ptr [esp + 0x34]
            fld dword ptr [esp + 0x38]
            fmul dword ptr [esp + 0x38]
            fxch st(1)
            faddp st(2), st(0)
            faddp st(1), st(0)
            fsqrt
            fstp dword ptr [esp + 0x10]
            mov eax, dword ptr [esp + 0x10]
            mov dword ptr [esp + 0x20], eax
            fld dword ptr [esp + 0x20]
            fcomp dword ptr [ecx + 0xc]
            fnstsw ax
            test ah, 1
            je L004de44c
            mov edx, dword ptr [esp + 0x1c]
            mov eax, dword ptr [gNet_mode]
            inc edx
            cmp eax, 3
            mov dword ptr [esp + 0x1c], edx
            je L004de431
            fld dword ptr [ecx + 0xc]
            fsub dword ptr [esp + 0x20]
            fld dword ptr [esp + 0x10]
            fxch st(1)
            fdiv dword ptr [ecx + 0xc]
            fdiv dword ptr [esi + 0x4c8]
            fxch st(1)
            fst dword ptr [esp + 0x10]
            fxch st(1)
            fxch st(1)
            fcomp dword ptr [gFLOAT_0058ac10]
            fnstsw ax
            test ah, 0x41
            jne L004de333
            fld dword ptr [esp + 0x10]
            fdivr qword ptr [gDOUBLE_0058ac18]
            fld st(0)
            fld st(1)
            fxch st(1)
            fmul dword ptr [esp + 0x30]
            fxch st(1)
            fmul dword ptr [esp + 0x34]
            fxch st(2)
            fmul dword ptr [esp + 0x38]
            fxch st(1)
            fxch st(2)
            fxch st(1)
            fxch st(1)
            fstp dword ptr [esp + 0x40]
            fstp dword ptr [esp + 0x44]
            jmp L004de349
    L004de333:
            fld dword ptr [gFLOAT_0058ac20]
            mov dword ptr [esp + 0x40], 0
            mov dword ptr [esp + 0x44], 0
    L004de349:
            fld dword ptr [esp + 0x40]
            fabs
            fcomp qword ptr [gDOUBLE_0058ac80]
            fnstsw ax
            test ah, 1
            je L004de363
            mov edx, dword ptr [ecx + 4]
            mov dword ptr [esp + 0x40], edx
    L004de363:
            fld st(1)
            fmul dword ptr [ecx]
            mov eax, dword ptr [esi + 8]
            fstp dword ptr [esp + 0x10]
            fmul dword ptr [esp + 0x10]
            fld dword ptr [esp + 0x40]
            fmul dword ptr [esp + 0x10]
            fxch st(1)
            fadd dword ptr [eax + 0x68]
            fstp dword ptr [eax + 0x68]
            fld dword ptr [esp + 0x44]
            mov eax, dword ptr [esi + 8]
            fmul dword ptr [esp + 0x10]
            fxch st(1)
            fadd dword ptr [eax + 0x6c]
            fxch st(1)
            fst dword ptr [esp + 0x44]
            fxch st(1)
            fstp dword ptr [eax + 0x6c]
            mov eax, dword ptr [esi + 8]
            fadd dword ptr [eax + 0x70]
            fstp dword ptr [eax + 0x70]
            mov eax, dword ptr [edi + 0x48]
            fld dword ptr [eax + 8]
            fmul st(0), st(1)
            fstp dword ptr [esp + 0x10]
            mov edi, dword ptr [esp + 0x10]
            fstp st(0)
            push edi
            call FRandomPosNeg
            fstp dword ptr [esp + 0x48]
            push edi
            call FRandomPosNeg
            fstp dword ptr [esp + 0x4c]
            push edi
            call FRandomPosNeg
            mov eax, dword ptr [esi + 8]
            fld dword ptr [esp + 0x48]
            fadd dword ptr [eax + 0x74]
            fld dword ptr [esp + 0x4c]
            fxch st(2)
            fstp dword ptr [esp + 0x50]
            fstp dword ptr [eax + 0x74]
            mov eax, dword ptr [esi + 8]
            fld dword ptr [esp + 0x50]
            fxch st(1)
            fadd dword ptr [eax + 0x78]
            fstp dword ptr [eax + 0x78]
            mov eax, dword ptr [esi + 8]
            fadd dword ptr [eax + 0x7c]
            fstp dword ptr [eax + 0x7c]
            mov ecx, dword ptr [esi + 8]
            mov byte ptr [ecx + 0xec], 0
            mov eax, dword ptr [gPHIL_last_physics_tick]
            mov edx, dword ptr [esi + 8]
            add eax, 0x28
            mov dword ptr [edx + 0x49c], eax
            call PDGetTotalTime
            mov edi, dword ptr [esp + 0x28]
            mov dword ptr [esi + 0x13e4], eax
            mov dword ptr [esi + 0x13ec], ebp
    L004de431:
            mov ecx, dword ptr [esp + 0x2c]
            push 0x3df5c28f
            push 0x3d75c28f
            lea eax, [ebp + 0x4c]
            push ebx
            push eax
            push esi
            mov edx, ebp
            call AddRepulseLink
    L004de44c:
            mov ebx, dword ptr [esp + 0x18]
    L004de450:
            mov edx, dword ptr [esp + 0x24]
            mov eax, dword ptr [esp + 0x14]
            inc edx
            cmp edx, eax
            mov dword ptr [esp + 0x24], edx
            jl L004de23d
    L004de465:
            inc ebx
            cmp ebx, 4
            mov dword ptr [esp + 0x18], ebx
            jle L004de212
            mov eax, dword ptr [gNet_mode]
            test eax, eax
            je L004de4c5
            mov eax, dword ptr [esp + 0x58]
            test eax, eax
            je L004de4c5
            xor edx, edx
            mov cl, 0x23
            call NetBuildGuaranteedMessage
            mov edx, dword ptr [gLocal_net_ID]
            mov ecx, eax
            mov eax, 0x2fa0be83
            push 0
            mov dword ptr [ecx + 0x1c], edx
            mov esi, dword ptr [gPowerup_array]
            mov edx, edi
            sub edx, esi
            imul edx
            sar edx, 5
            mov eax, edx
            shr eax, 0x1f
            add edx, eax
            mov dword ptr [ecx + 0x20], edx
            mov edx, ecx
            mov ecx, dword ptr [gCurrent_net_game]
            call NetGuaranteedSendMessageToAllPlayers
    L004de4c5:
            mov esi, dword ptr [esp + 0x1c]
            test esi, esi
            je L004de52a
            test ebp, ebp
            je L004de501
            cmp dword ptr [ebp + 0xc], 8
            jne L004de501
            mov ecx, dword ptr [gXXX_outlet]
            push 0x10000
            push 0x10000
            push 0xff
            push 1
            add ebp, 0x4c
            push offset gZero_v__car
            push ebp
            mov edx, 0x1324
            call DRS3StartSound3D
    L004de501:
            test esi, esi
            je L004de52a
            mov ebx, dword ptr [gPowerup_array]
            mov eax, 0x2fa0be83
            sub edi, ebx
            imul edi
            sar edx, 5
            mov ecx, edx
            shr ecx, 0x1f
            add edx, ecx
            mov eax, edx
            pop edi
            pop esi
            pop ebp
            pop ebx
            add esp, 0x44
            ret 4
    L004de52a:
            pop edi
            pop esi
            pop ebp
            or eax, 0xffffffff
            pop ebx
            add esp, 0x44
            ret 4
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
#pragma auto_inline(on)

// FUNCTION: CARMA2_HW 0x004de1e0
int C2_HOOK_FASTCALL RepulseOpponents(tPowerup* powerup, tCar_spec* car) {
    return RepulseOpponents2(powerup, car, 1);
}

// FUNCTION: CARMA2_HW 0x004dc800
int C2_HOOK_FASTCALL SetPedBrittle(tPowerup* powerup, tCar_spec* car) {
    gPed_dismemberfest = 1;
    return powerup - gPowerup_array;
}

// FUNCTION: CARMA2_HW 0x004de930
int C2_HOOK_FASTCALL SetGhostPeds(tPowerup* powerup, tCar_spec* car) {
    MakePedsEthereal();
    return powerup - gPowerup_array;
}

// FUNCTION: CARMA2_HW 0x004de870
int C2_HOOK_FASTCALL SetDancingPeds(tPowerup* powerup, tCar_spec* car) {
    gDancing_peds = 1;
    return powerup - gPowerup_array;
}

// FUNCTION: CARMA2_HW 0x004de8a0
int C2_HOOK_FASTCALL SetPanickingPeds(tPowerup* powerup, tCar_spec* car) {
    gPanicking_peds = 1;
    return powerup - gPowerup_array;
}

// FUNCTION: CARMA2_HW 0x004de8d0
int C2_HOOK_FASTCALL SetLowGravityPeds(tPowerup* powerup, tCar_spec* car) {
    gPed_gravity_multiplier = powerup->float_params[0];
    return powerup - gPowerup_array;
}

// FUNCTION: CARMA2_HW 0x004de900
int C2_HOOK_FASTCALL SetPissedPeds(tPowerup* powerup, tCar_spec* car) {
    gDrunk_pedestrians = 1;
    return powerup - gPowerup_array;
}

// FUNCTION: CARMA2_HW 0x004de540
int C2_HOOK_FASTCALL RepulsePedestrians(tPowerup* powerup, tCar_spec* car) {
    int result;
    float* params;

    params = powerup->float_params;
    if (DoPedestrianRepulsifictor(car, params[0], params[2], params[1], params[3]) != 0) {
        result = powerup - gPowerup_array;
    } else {
        result = -1;
    }
    if (result != -1) {
        PlayExclusiveFFBEffect("Repulse", 0);
    }
    return result;
}

#pragma auto_inline(off)
// FUNCTION: CARMA2_HW 0x004dc3e0
static int C2_HOOK_FASTCALL GainAPO2(tCar_spec* car, int index, int amount) {
    int old;

    while (amount < 0) {
        int slots = car->power_up_slots[index];
        int levels = car->power_up_levels[index];
        if (levels >= slots) {
            return 0;
        }
        amount = slots - levels;
    }
    if (index == 1 && gNet_mode != eNet_mode_none
        && gCurrent_net_game->type == eNet_game_type_foxy
        && gThis_net_player_index == gIt_or_fox) {
        return 0;
    }
    old = car->power_up_levels[index];
    car->power_up_levels[index] = old + amount;
    if (car->power_up_levels[index] > car->power_up_slots[index]) {
        car->power_up_levels[index] = car->power_up_slots[index];
    }
    return car->power_up_levels[index] - old;
}
#pragma auto_inline(on)

// FUNCTION: CARMA2_HW 0x004dc240
int C2_HOOK_FASTCALL GainAPO(tPowerup* powerup, tCar_spec* car) {
    int powerup_index;
    int old0, old1, old2;
    int idx;
    int amount;
    int orig_idx;
    int dir;

    powerup_index = powerup - gPowerup_array;
    old0 = car->power_up_levels[0];
    old1 = car->power_up_levels[1];
    old2 = car->power_up_levels[2];
    if (powerup->integer_params[0] != 0
        && powerup->integer_params[1] != 0
        && powerup->integer_params[2] != 0) {
        GainAPO2(car, 0, powerup->integer_params[0]);
        GainAPO2(car, 1, powerup->integer_params[1]);
        GainAPO2(car, 2, powerup->integer_params[2]);
    } else {
        if (powerup->integer_params[0] != 0) {
            idx = 0;
            amount = powerup->integer_params[0];
        } else if (powerup->integer_params[1] != 0) {
            idx = 1;
            amount = powerup->integer_params[1];
        } else {
            idx = 2;
            amount = powerup->integer_params[2];
        }
        if (GainAPO2(car, idx, amount) == 0) {
            orig_idx = idx;
            dir = PercentageChance(50) ? 1 : -1;
            idx += dir;
            if (idx < 0) {
                idx = 2;
            } else if (idx > 2) {
                idx = 0;
            }
            if (GainAPO2(car, idx, amount) == 0) {
                idx += dir;
                if (idx < 0) {
                    idx = 2;
                } else if (idx > 2) {
                    idx = 0;
                }
                GainAPO2(car, idx, amount);
            }
            powerup_index += idx - orig_idx;
        }
    }
    if (old0 == car->power_up_levels[0]
        && old1 == car->power_up_levels[1]
        && old2 == car->power_up_levels[2]) {
        if (gNet_mode != eNet_mode_none) {
            EarnCredits(gSubstitution_value_APO.initial_network[gCurrent_net_game->type]);
        } else {
            EarnCredits(gSubstitution_value_APO.initial[gProgram_state.skill_level]);
        }
        return -1;
    }
    return powerup_index;
}

// FUNCTION: CARMA2_HW 0x004dc4d0
int C2_NAKED C2_HOOK_FASTCALL GainAPOPotential(tPowerup* powerup, tCar_spec* car) {
#ifndef CARPOCALYPSE2_MATCHING
    int powerup_index;
    int old0, old1, old2;
    int* params;
    int a0, a1, a2;
    int idx;
    int amount;
    int orig_idx;
    int dir;
    int old;
    int nw;

    powerup_index = powerup - gPowerup_array;
    old0 = car->power_up_slots[0];
    old1 = car->power_up_slots[1];
    old2 = car->power_up_slots[2];
    params = powerup->integer_params;
    a0 = params[0];
    a1 = params[1];
    a2 = params[2];
    if (a0 != 0 && a1 != 0 && a2 != 0) {
        nw = old0 + a0;
        car->power_up_slots[0] = nw;
        if (nw > gInitial_powerup_slots[0]) {
            car->power_up_slots[0] = gInitial_powerup_slots[0];
        }
        nw = car->power_up_slots[1] + a1;
        car->power_up_slots[1] = nw;
        if (nw > gInitial_powerup_slots[1]) {
            car->power_up_slots[1] = gInitial_powerup_slots[1];
        }
        nw = car->power_up_slots[2] + a2;
        car->power_up_slots[2] = nw;
        if (nw > gInitial_powerup_slots[2]) {
            car->power_up_slots[2] = gInitial_powerup_slots[2];
        }
    } else {
        if (a0 != 0) {
            idx = 0;
            amount = a0;
        } else if (a1 != 0) {
            idx = 1;
            amount = a1;
        } else {
            idx = 2;
            amount = a2;
        }
        old = car->power_up_slots[idx];
        nw = old + amount;
        car->power_up_slots[idx] = nw;
        if (nw > gInitial_powerup_slots[idx]) {
            car->power_up_slots[idx] = gInitial_powerup_slots[idx];
        }
        if (car->power_up_slots[idx] == old) {
            orig_idx = idx;
            dir = PercentageChance(50) ? 1 : -1;
            idx += dir;
            if (idx < 0) {
                idx = 2;
            } else if (idx > 2) {
                idx = 0;
            }
            old = car->power_up_slots[idx];
            nw = old + amount;
            car->power_up_slots[idx] = nw;
            if (nw > gInitial_powerup_slots[idx]) {
                car->power_up_slots[idx] = gInitial_powerup_slots[idx];
            }
            if (car->power_up_slots[idx] == old) {
                idx += dir;
                if (idx < 0) {
                    idx = 2;
                } else if (idx > 2) {
                    idx = 0;
                }
                old = car->power_up_slots[idx];
                nw = old + amount;
                car->power_up_slots[idx] = nw;
                if (nw > gInitial_powerup_slots[idx]) {
                    car->power_up_slots[idx] = gInitial_powerup_slots[idx];
                }
            }
            powerup_index += idx - orig_idx;
        }
    }
    if (old0 == car->power_up_slots[0]
        && old1 == car->power_up_slots[1]
        && old2 == car->power_up_slots[2]) {
        if (gNet_mode != eNet_mode_none) {
            EarnCredits(gPotential_substitution_value_APO.initial_network[gCurrent_net_game->type]);
        } else {
            EarnCredits(gPotential_substitution_value_APO.initial[gProgram_state.skill_level]);
        }
        return -1;
    }
    return powerup_index;
#else
    __asm {
            sub esp, 0x14
            push ebx
            push ebp
            push esi
            mov esi, edx
            push edi
            mov edi, dword ptr [gPowerup_array]
            mov edx, ecx
            mov eax, 0x2fa0be83
            sub edx, edi
            imul edx
            sar edx, 5
            mov eax, edx
            shr eax, 0x1f
            add edx, eax
            mov eax, dword ptr [esi + 0x18b8]
            mov dword ptr [esp + 0x10], edx
            mov edx, dword ptr [esi + 0x18b4]
            mov dword ptr [esp + 0x18], edx
            mov edx, dword ptr [esi + 0x18bc]
            mov dword ptr [esp + 0x1c], eax
            mov eax, dword ptr [ecx + 0x24]
            mov dword ptr [esp + 0x20], edx
            mov edx, dword ptr [eax]
            test edx, edx
            je L004dc5b0
            mov edi, dword ptr [eax + 4]
            test edi, edi
            je L004dc5a6
            mov edi, dword ptr [eax + 8]
            test edi, edi
            je L004dc5a6
            mov ebx, dword ptr [esi + 0x18b4]
            add ebx, edx
            mov dword ptr [esi + 0x18b4], ebx
            mov edx, dword ptr [gInitial_powerup_slots]
            mov eax, ebx
            cmp eax, edx
            jle L004dc551
            mov dword ptr [esi + 0x18b4], edx
    L004dc551:
            mov eax, dword ptr [ecx + 0x24]
            mov ebx, dword ptr [esi + 0x18b8]
            mov eax, dword ptr [eax + 4]
            add ebx, eax
            mov dword ptr [esi + 0x18b8], ebx
            mov edx, dword ptr [gInitial_powerup_slots+0x4]
            mov eax, ebx
            cmp eax, edx
            jle L004dc577
            mov dword ptr [esi + 0x18b8], edx
    L004dc577:
            mov ecx, dword ptr [ecx + 0x24]
            mov edi, dword ptr [esi + 0x18bc]
            mov edx, dword ptr [ecx + 8]
            add edi, edx
            mov dword ptr [esi + 0x18bc], edi
            mov ecx, dword ptr [gInitial_powerup_slots+0x8]
            mov eax, edi
            cmp eax, ecx
            jle L004dc696
            mov dword ptr [esi + 0x18bc], ecx
            jmp L004dc696
    L004dc5a6:
            test edx, edx
            je L004dc5b0
            xor edi, edi
            mov ebp, edx
            jmp L004dc5c6
    L004dc5b0:
            mov ebp, dword ptr [eax + 4]
            test ebp, ebp
            je L004dc5be
            mov edi, 1
            jmp L004dc5c6
    L004dc5be:
            mov ebp, dword ptr [eax + 8]
            mov edi, 2
    L004dc5c6:
            mov edx, dword ptr [esi + edi*4 + 0x18b4]
            lea eax, [ebp + edx]
            mov dword ptr [esi + edi*4 + 0x18b4], eax
            mov ecx, dword ptr [edi*4 + gInitial_powerup_slots]
            cmp eax, ecx
            jle L004dc5ea
            mov dword ptr [esi + edi*4 + 0x18b4], ecx
    L004dc5ea:
            mov eax, dword ptr [esi + edi*4 + 0x18b4]
            sub eax, edx
            jne L004dc696
            mov ecx, 0x32
            mov dword ptr [esp + 0x14], edi
            call PercentageChance
            neg eax
            sbb eax, eax
            and eax, 2
            dec eax
            add edi, eax
            jns L004dc61a
            mov edi, 2
            jmp L004dc621
    L004dc61a:
            cmp edi, 2
            jle L004dc621
            xor edi, edi
    L004dc621:
            mov ebx, dword ptr [esi + edi*4 + 0x18b4]
            lea ecx, [ebp + ebx]
            mov dword ptr [esi + edi*4 + 0x18b4], ecx
            mov edx, dword ptr [edi*4 + gInitial_powerup_slots]
            cmp ecx, edx
            jle L004dc645
            mov dword ptr [esi + edi*4 + 0x18b4], edx
    L004dc645:
            mov ecx, dword ptr [esi + edi*4 + 0x18b4]
            sub ecx, ebx
            jne L004dc686
            add edi, eax
            jns L004dc65b
            mov edi, 2
            jmp L004dc662
    L004dc65b:
            cmp edi, 2
            jle L004dc662
            xor edi, edi
    L004dc662:
            mov ebx, dword ptr [esi + edi*4 + 0x18b4]
            add ebx, ebp
            mov dword ptr [esi + edi*4 + 0x18b4], ebx
            mov eax, dword ptr [edi*4 + gInitial_powerup_slots]
            mov ecx, ebx
            cmp ecx, eax
            jle L004dc686
            mov dword ptr [esi + edi*4 + 0x18b4], eax
    L004dc686:
            mov ecx, dword ptr [esp + 0x14]
            mov eax, dword ptr [esp + 0x10]
            sub edi, ecx
            add eax, edi
            mov dword ptr [esp + 0x10], eax
    L004dc696:
            mov edx, dword ptr [esp + 0x18]
            mov eax, dword ptr [esi + 0x18b4]
            cmp edx, eax
            jne L004dc706
            mov eax, dword ptr [esp + 0x1c]
            mov ecx, dword ptr [esi + 0x18b8]
            cmp eax, ecx
            jne L004dc706
            mov ecx, dword ptr [esp + 0x20]
            mov eax, dword ptr [esi + 0x18bc]
            cmp ecx, eax
            jne L004dc706
            mov eax, dword ptr [gNet_mode]
            test eax, eax
            je L004dc6e9
            mov edx, dword ptr [gCurrent_net_game]
            mov eax, dword ptr [edx + 0x74]
            mov ecx, dword ptr [eax*4 + gPotential_substitution_value_APO+0xc]
            call EarnCredits
            or eax, 0xffffffff
            pop edi
            pop esi
            pop ebp
            pop ebx
            add esp, 0x14
            ret
    L004dc6e9:
            mov ecx, dword ptr [gProgram_state+0x20]
            mov ecx, dword ptr [ecx*4 + gPotential_substitution_value_APO]
            call EarnCredits
            or eax, 0xffffffff
            pop edi
            pop esi
            pop ebp
            pop ebx
            add esp, 0x14
            ret
    L004dc706:
            mov eax, dword ptr [esp + 0x10]
            pop edi
            pop esi
            pop ebp
            pop ebx
            add esp, 0x14
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
            nop
            nop
            nop
    }
#endif
}

// FUNCTION: CARMA2_HW 0x004df220
int C2_NAKED C2_HOOK_FASTCALL SetPissed(tPowerup* powerup, tCar_spec* car) {
#ifndef CARPOCALYPSE2_MATCHING
    if (car != NULL && car->driver == eDriver_local_human) {
        float* pOut;
        int k;

        DRS3StartSound2(gXXX_outlet, 0xe11, 1, 0xff, 0xff, -1, -1);
        gPissed_next_time = GetTotalTime() + IRandomBetween(0x5dc, 0xbb8);
        gCredit_multiplier = *powerup->integer_params;
        gPissed_hold_time = powerup->float_params[3];

        pOut = gPissed_flicker_frames;
        do {
            float* p = pOut;
            for (k = 1; k; k--) {
                float tmp;

                p[-12] = powerup->float_params[0];
                tmp = FRandomBetween(powerup->float_params[1], powerup->float_params[2]);
                p[0] = tmp;
                p[12] = FRandomBetween(0.0f, tmp);
                p += 1;
            }
            pOut += 3;
        } while ((int)pOut < (int)&gPissed_flicker_limit);
    }
    return powerup - gPowerup_array;
#else
    __asm {
            push ecx
            push ebx
            push edi
            test edx, edx
            mov edi, ecx
            je L004df2d9
            cmp dword ptr [edx + 0xc], 8
            jne L004df2d9
            mov ecx, dword ptr [gXXX_outlet]
            push esi
            push ebp
            push -1
            push -1
            push 0xff
            push 0xff
            push 1
            mov edx, 0xe11
            call DRS3StartSound2
            mov edx, 0xbb8
            mov ecx, 0x5dc
            call IRandomBetween
            mov esi, eax
            call GetTotalTime
            add esi, eax
            mov ebp, offset gPissed_flicker_frames
            mov dword ptr [gPissed_next_time], esi
            mov eax, dword ptr [edi + 0x24]
            mov ecx, dword ptr [eax]
            mov dword ptr [gCredit_multiplier], ecx
            mov edx, dword ptr [edi + 0x48]
            mov eax, dword ptr [edx + 0xc]
            mov dword ptr [gPissed_hold_time], eax
    L004df292:
            mov esi, ebp
            mov ebx, 1
    L004df299:
            mov ecx, dword ptr [edi + 0x48]
            mov edx, dword ptr [ecx]
            mov dword ptr [esi - 0x30], edx
            mov eax, dword ptr [edi + 0x48]
            mov ecx, dword ptr [eax + 8]
            mov edx, dword ptr [eax + 4]
            push ecx
            push edx
            call FRandomBetween
            fst dword ptr [esp + 0x10]
            mov eax, dword ptr [esp + 0x10]
            fstp dword ptr [esi]
            push eax
            push 0
            call FRandomBetween
            fstp dword ptr [esi + 0x30]
            add esi, 4
            dec ebx
            jne L004df299
            add ebp, 0xc
            cmp ebp, offset gPissed_flicker_limit
            jl L004df292
            pop ebp
            pop esi
    L004df2d9:
            mov ebx, dword ptr [gPowerup_array]
            mov eax, 0x2fa0be83
            sub edi, ebx
            imul edi
            mov eax, edx
            pop edi
            sar eax, 5
            mov ecx, eax
            pop ebx
            shr ecx, 0x1f
            add eax, ecx
            pop ecx
            ret
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

// FUNCTION: CARMA2_HW 0x004de190
int C2_HOOK_FASTCALL NapalmPeds(tPowerup* powerup, tCar_spec* car) {
    int result;

    if (NapalmPedestrianNearCar(car) != 0) {
        result = powerup - gPowerup_array;
    } else {
        result = -1;
    }
    if (result != -1) {
        PlayExclusiveFFBEffect("Flamer", 0);
    }
    return result;
}

// FUNCTION: CARMA2_HW 0x004dfec0
int C2_HOOK_FASTCALL PedValium(tPowerup* powerup, tCar_spec* car) {
    if (CalmDownAllPeds() != 0) {
        NewTextHeadupSlot(4,0, 3000, -4, GetMiscString(eMiscString_pedestrians_are_calm_now));
        DRS3StartSound2(gXXX_outlet, eSoundId_PedValium, 1, 0xff, 0xff, -1, -1);
        return powerup - gPowerup_array;
    }
    return -1;
}

// FUNCTION: CARMA2_HW 0x004dff30
int C2_NAKED C2_HOOK_FASTCALL PowerupCancel(tPowerup* powerup, tCar_spec* car) {
#ifndef CARPOCALYPSE2_MATCHING
    int i;
    int j;
    tPowerup* p;
    tNet_message* msg;
    int any = 0;
    tPowerup* saved = powerup;

    if (car == NULL) {
        goto fail;
    }
    if (car->driver != eDriver_local_human) {
        goto fail;
    }
    for (i = 0, p = gPowerup_array; i < gNumber_of_powerups; i++, p++) {
        if (p->got_time != 0) {
        if (p->lose_proc != NULL) {
            p->lose_proc(p, p->car);
        }
        if (p->net_type & 0x80) {
            ARPowerupLose(p, p->car);
        }
        if (gNet_mode != eNet_mode_none) {
            msg = NetBuildGuaranteedMessage(0x16, 0);
            *(int*)((tU8*)msg + 0x28) = 2;
            *(tPlayer_ID*)((tU8*)msg + 0x1c) = gLocal_net_ID;
            *(int*)((tU8*)msg + 0x20) = p - gPowerup_array;
            NetGuaranteedSendMessageToAllPlayers(gCurrent_net_game, msg, NULL);
        }
        gProgram_state.current_car.powerups[p - gPowerup_array] = 0;
        if (p->initial_value == 0) {
            for (j = 0; j < gNumber_of_icons; j++) {
                if (gPickedup_powerups[j].powerup == p) {
                    gPickedup_powerups[j].fizzle_stage = 3;
                    gPickedup_powerups[j].fizzle_direction = -1;
                    gPickedup_powerups[j].fizzle_start = GetTotalTime();
                    break;
                }
            }
        }
        p->got_time = 0;
        any = 1;
        }
    }
    if (gMutant_tail_state == 2) {
        for (i = 0; i < (int)CARPOCALYPSE2_ASIZE(gInventory); i++) {
            if (gInventory[i] == 0x5f) {
                gInventory[i] = -1;
            }
        }
        PHILRemoveFromParentChain(gMutant_tail_first_collision_info);
        gMutant_tail_first_collision_info->physics_joint1 = NULL;
        PHILAddActiveObject(gMutant_tail_first_collision_info, NULL, NULL, NULL);
        PHILSetObjectProperty(gMutant_tail_first_collision_info, 0, 1.0);
        PHILSetObjectProperty(gMutant_tail_first_collision_info, 1, 1);
        PHILSetObjectProperty(gMutant_tail_first_collision_info, 7, 1);
        PHILSetObjectProperty(gMutant_tail_first_collision_info, 6, 1);
        {
            tPowerup* tailp = &gPowerup_array[94];
            if (tailp->got_time != 0 && tailp->lose_proc != NULL) {
                tailp->lose_proc(tailp, tailp->car);
            }
            if (tailp->net_type & 0x80) {
                ARPowerupLose(tailp, tailp->car);
            }
            if (gNet_mode != eNet_mode_none) {
                msg = NetBuildGuaranteedMessage(0x16, 0);
                *(int*)((tU8*)msg + 0x28) = 2;
                *(tPlayer_ID*)((tU8*)msg + 0x1c) = gLocal_net_ID;
                *(int*)((tU8*)msg + 0x20) = tailp - gPowerup_array;
                NetGuaranteedSendMessageToAllPlayers(gCurrent_net_game, msg, NULL);
            }
            gProgram_state.current_car.powerups[tailp - gPowerup_array] = 0;
            if (tailp->initial_value == 0) {
                for (j = 0; j < gNumber_of_icons; j++) {
                    if (gPickedup_powerups[j].powerup == tailp) {
                        gPickedup_powerups[j].fizzle_stage = 3;
                        gPickedup_powerups[j].fizzle_direction = -1;
                        gPickedup_powerups[j].fizzle_start = GetTotalTime();
                        break;
                    }
                }
            }
            tailp->got_time = 0;
        }
        gMutant_tail_state = 1;
        gINT_00705b70 = GetTotalTime();
        any = 1;
    }
    if (!any) {
        goto fail;
    }
    NewTextHeadupSlot(4, 0, 0xbb8, -4, GetMiscString(0x113));
    DRS3StartSound2(gXXX_outlet, 0xeda, 1, 0xff, 0xff, -1, -1);
    return saved - gPowerup_array;
fail:
    return -1;
#else
    __asm {
            push ecx
            push ebx
            push ebp
            xor ebp, ebp
            xor eax, eax
            push esi
            cmp edx, ebp
            push edi
            mov dword ptr [esp + 0x10], ecx
            je L004e02a0
            cmp dword ptr [edx + 0xc], 8
            jne L004e02a0
            mov ecx, dword ptr [gNumber_of_powerups]
            xor ebx, ebx
            cmp ecx, ebp
            jle L004e0071
            mov ecx, dword ptr [gPowerup_array]
            lea edi, [ecx + 0x40]
    L004dff68:
            cmp dword ptr [edi - 0x3c], ebp
            je L004e005c
            mov eax, dword ptr [edi]
            cmp eax, ebp
            je L004dff7f
            mov edx, dword ptr [edi + 0xc]
            lea ecx, [edi - 0x40]
            call eax
    L004dff7f:
            test byte ptr [edi - 8], 0x80
            je L004dff90
            mov edx, dword ptr [edi + 0xc]
            lea ecx, [edi - 0x40]
            call ARPowerupLose
    L004dff90:
            cmp dword ptr [gNet_mode], ebp
            je L004dffe1
            xor edx, edx
            mov cl, 0x16
            call NetBuildGuaranteedMessage
            mov esi, eax
            mov ecx, edi
            push ebp
            mov dword ptr [esi + 0x28], 2
            mov edx, dword ptr [gLocal_net_ID]
            mov dword ptr [esi + 0x1c], edx
            mov eax, dword ptr [gPowerup_array]
            sub ecx, eax
            mov eax, 0x2fa0be83
            sub ecx, 0x40
            imul ecx
            sar edx, 5
            mov eax, edx
            shr eax, 0x1f
            add edx, eax
            mov dword ptr [esi + 0x20], edx
            mov ecx, dword ptr [gCurrent_net_game]
            mov edx, esi
            call NetGuaranteedSendMessageToAllPlayers
    L004dffe1:
            mov esi, dword ptr [gPowerup_array]
            mov ecx, edi
            sub ecx, esi
            mov eax, 0x2fa0be83
            sub ecx, 0x40
            imul ecx
            sar edx, 5
            mov ecx, edx
            shr ecx, 0x1f
            add edx, ecx
            mov dword ptr [edx*4 + gProgram_state+0x17bc], ebp
            mov eax, dword ptr [edi - 0xc]
            cmp eax, ebp
            jne L004e0054
            mov edx, dword ptr [gNumber_of_icons]
            xor eax, eax
            cmp edx, ebp
            jle L004e0054
            mov ecx, offset gPickedup_powerups
    L004e001e:
            lea esi, [edi - 0x40]
            cmp esi, dword ptr [ecx]
            je L004e002f
            inc eax
            add ecx, 0x14
            cmp eax, edx
            jl L004e001e
            jmp L004e0054
    L004e002f:
            lea esi, [eax + eax*4]
            shl esi, 2
            mov dword ptr [esi + gPickedup_powerups+0x4], 3
            mov dword ptr [esi + gPickedup_powerups+0x8], 0xffffffff
            call GetTotalTime
            mov dword ptr [esi + gPickedup_powerups+0xc], eax
    L004e0054:
            mov dword ptr [edi - 0x3c], ebp
            mov eax, 1
    L004e005c:
            mov ecx, dword ptr [gNumber_of_powerups]
            inc ebx
            add edi, 0xac
            cmp ebx, ecx
            jl L004dff68
    L004e0071:
            mov ecx, dword ptr [gMutant_tail_state]
            mov edi, 2
            cmp ecx, edi
            jne L004e0239
            mov eax, offset gInventory
    L004e0089:
            cmp dword ptr [eax], 0x5f
            jne L004e0094
            mov dword ptr [eax], 0xffffffff
    L004e0094:
            add eax, 4
            cmp eax, offset gFizzle_in
            jl L004e0089
            mov ecx, dword ptr [gMutant_tail_first_collision_info]
            call PHILRemoveFromParentChain
            mov edx, dword ptr [gMutant_tail_first_collision_info]
            push ebp
            push ebp
            mov dword ptr [edx + 0x188], ebp
            mov ecx, dword ptr [gMutant_tail_first_collision_info]
            xor edx, edx
            call PHILAddActiveObject
            mov eax, dword ptr [gMutant_tail_first_collision_info]
            push 0x3ff00000
            push ebp
            push ebp
            push eax
            call PHILSetObjectProperty
            mov ecx, dword ptr [gMutant_tail_first_collision_info]
            add esp, 0x10
            push 1
            push 1
            push ecx
            call PHILSetObjectProperty
            mov edx, dword ptr [gMutant_tail_first_collision_info]
            add esp, 0xc
            push 1
            push 7
            push edx
            call PHILSetObjectProperty
            mov eax, dword ptr [gMutant_tail_first_collision_info]
            add esp, 0xc
            push 1
            push 6
            push eax
            call PHILSetObjectProperty
            mov ecx, dword ptr [gPowerup_array]
            add esp, 0xc
            mov eax, dword ptr [ecx + 0x3f2c]
            lea esi, [ecx + 0x3f28]
            cmp eax, ebp
            je L004e0135
            mov eax, dword ptr [esi + 0x40]
            cmp eax, ebp
            je L004e0135
            mov edx, dword ptr [esi + 0x4c]
            mov ecx, esi
            call eax
    L004e0135:
            test byte ptr [esi + 0x38], 0x80
            je L004e0145
            mov edx, dword ptr [esi + 0x4c]
            mov ecx, esi
            call ARPowerupLose
    L004e0145:
            cmp dword ptr [gNet_mode], ebp
            je L004e0190
            xor edx, edx
            mov cl, 0x16
            call NetBuildGuaranteedMessage
            mov ecx, eax
            mov eax, 0x2fa0be83
            push ebp
            mov dword ptr [ecx + 0x28], edi
            mov edx, dword ptr [gLocal_net_ID]
            mov dword ptr [ecx + 0x1c], edx
            mov edi, dword ptr [gPowerup_array]
            mov edx, esi
            sub edx, edi
            imul edx
            sar edx, 5
            mov eax, edx
            shr eax, 0x1f
            add edx, eax
            mov dword ptr [ecx + 0x20], edx
            mov edx, ecx
            mov ecx, dword ptr [gCurrent_net_game]
            call NetGuaranteedSendMessageToAllPlayers
    L004e0190:
            mov edx, dword ptr [gPowerup_array]
            mov ecx, esi
            sub ecx, edx
            mov eax, 0x2fa0be83
            imul ecx
            sar edx, 5
            mov ecx, edx
            shr ecx, 0x1f
            add edx, ecx
            mov dword ptr [edx*4 + gProgram_state+0x17bc], ebp
            mov eax, dword ptr [esi + 0x34]
            cmp eax, ebp
            jne L004e01fd
            mov edx, dword ptr [gNumber_of_icons]
            xor eax, eax
            cmp edx, ebp
            jle L004e01fd
            mov ecx, offset gPickedup_powerups
    L004e01ca:
            cmp esi, dword ptr [ecx]
            je L004e01d8
            inc eax
            add ecx, 0x14
            cmp eax, edx
            jl L004e01ca
            jmp L004e01fd
    L004e01d8:
            lea edi, [eax + eax*4]
            shl edi, 2
            mov dword ptr [edi + gPickedup_powerups+0x4], 3
            mov dword ptr [edi + gPickedup_powerups+0x8], 0xffffffff
            call GetTotalTime
            mov dword ptr [edi + gPickedup_powerups+0xc], eax
    L004e01fd:
            push -1
            push -1
            push 0xff
            mov dword ptr [esi + 4], ebp
            mov ecx, dword ptr [gXXX_outlet]
            push 0xff
            push 1
            mov edx, 0xf3e
            mov dword ptr [gMutant_tail_state], 1
            call DRS3StartSound2
            call GetTotalTime
            mov dword ptr [gINT_00705b70], eax
            mov eax, 1
    L004e0239:
            cmp eax, ebp
            je L004e02a0
            mov ecx, 0x113
            call GetMiscString
            xor edx, edx
            mov ecx, 4
            push eax
            push -4
            push 0xbb8
            call NewTextHeadupSlot
            mov ecx, dword ptr [gXXX_outlet]
            mov edx, 0xeda
            push -1
            push -1
            push 0xff
            push 0xff
            push 1
            call DRS3StartSound2
            mov ebx, dword ptr [gPowerup_array]
            mov eax, 0x2fa0be83
            mov ecx, dword ptr [esp + 0x10]
            sub ecx, ebx
            imul ecx
            mov eax, edx
            sar eax, 5
            mov edx, eax
            shr edx, 0x1f
            add eax, edx
            pop edi
            pop esi
            pop ebp
            pop ebx
            pop ecx
            ret
    L004e02a0:
            pop edi
            pop esi
            pop ebp
            or eax, 0xffffffff
            pop ebx
            pop ecx
            ret
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

// FUNCTION: CARMA2_HW 0x004e02b0
int C2_HOOK_FASTCALL GrowATail(tPowerup* powerup, tCar_spec* car) {
    tPhysics_object* node;
    br_matrix34 tmp;
    br_vector3 tmp_v;

    if (gMutant_tail_state == 2) {
        return -1;
    }
    if (gMutant_tail_state == 1) {
        PHILRemoveObject(gMutant_tail_first_collision_info);
        gMutant_tail_first_collision_info->physics_joint1 = gMutant_tail_first_joint;
    } else {
        PipeSingleSpecial(1);
    }
    BrVector3SetFloat(&gMutant_tail_first_collision_info->physics_joint1->field_0x14,
        0.0f,
        (car->collision_info->bb1.max.v[1] - car->collision_info->bb1.min.v[1]) * gFLOAT_0058ac00,
        car->collision_info->bb1.max.v[2] * 0.95);
    PhysicsAddObject(car->collision_info, gMutant_tail_first_collision_info);
    node = gMutant_tail_first_collision_info;
    while (node != NULL) {
        if (node->actor->parent == NULL) {
            BrActorAdd(gNon_track_actor, node->actor);
        }
        node->owner = (void*)car;
        BrMatrix34Translate(&node->actor->t.t.mat,
            -node->physics_joint1->field_0x08.v[0],
            -node->physics_joint1->field_0x08.v[1],
            -node->physics_joint1->field_0x08.v[2]);
        BrMatrix34Copy(&tmp, &car->collision_info->actor->t.t.mat);
        BrMatrix34Post(&node->actor->t.t.mat, &tmp);
        BrMatrix34Rotate(&tmp, (br_angle)0xf8d4, (br_vector3*)&node->actor->t.t.mat);
        BrMatrix34Post(&node->actor->t.t.mat, &tmp);
        BrMatrix34ApplyP(&tmp_v, &node->physics_joint1->field_0x14, &node->parent->actor->t.t.mat);
        BrMatrix34PostTranslate(&node->actor->t.t.mat, tmp_v.v[0], tmp_v.v[1], tmp_v.v[2]);
        BrMatrix34Copy(&node->transform_matrix, &node->actor->t.t.mat);
        node->field_0xed = 1;
        node = node->child;
    }
    GotPowerupEarwig(car, 0x5f, 0, 1);
    gMutant_tail_state = 2;
    DRS3StartSound2(gXXX_outlet, 0xf3d, 1, 0xff, 0xff, -1, -1);
    return powerup - gPowerup_array;
}

// FUNCTION: CARMA2_HW 0x004e04c0
int C2_HOOK_FASTCALL CutOffTail(tPowerup* powerup, tCar_spec* car) {
    int i;
    tNet_message* msg;
    tPowerup* tailp;

    (void)car;
    if (gMutant_tail_state != 2) {
        goto done;
    }
    PHILRemoveFromParentChain(gMutant_tail_first_collision_info);
    gMutant_tail_first_collision_info->physics_joint1 = NULL;
    PHILAddActiveObject(gMutant_tail_first_collision_info, NULL, NULL, NULL);
    PHILSetObjectProperty(gMutant_tail_first_collision_info, 0, 1.0);
    PHILSetObjectProperty(gMutant_tail_first_collision_info, 1, 1);
    PHILSetObjectProperty(gMutant_tail_first_collision_info, 7, 1);
    PHILSetObjectProperty(gMutant_tail_first_collision_info, 6, 1);
    tailp = &gPowerup_array[94];
    if (tailp->got_time != 0 && tailp->lose_proc != NULL) {
        tailp->lose_proc(tailp, tailp->car);
    }
    if (tailp->net_type & 0x80) {
        ARPowerupLose(tailp, tailp->car);
    }
    if (gNet_mode != eNet_mode_none) {
        msg = NetBuildGuaranteedMessage(0x16, 0);
        *(int*)((tU8*)msg + 0x28) = 2;
        *(tPlayer_ID*)((tU8*)msg + 0x1c) = gLocal_net_ID;
        *(int*)((tU8*)msg + 0x20) = tailp - gPowerup_array;
        NetGuaranteedSendMessageToAllPlayers(gCurrent_net_game, msg, NULL);
    }
    gProgram_state.current_car.powerups[tailp - gPowerup_array] = 0;
    if (tailp->initial_value == 0) {
        for (i = 0; i < gNumber_of_icons; i++) {
            if (tailp == gPickedup_powerups[i].powerup) {
                gPickedup_powerups[i].fizzle_stage = 3;
                gPickedup_powerups[i].fizzle_direction = -1;
                gPickedup_powerups[i].fizzle_start = GetTotalTime();
                break;
            }
        }
    }
    tailp->got_time = 0;
    gMutant_tail_state = 1;
    DRS3StartSound2(gXXX_outlet, 0xf3e, 1, 0xff, 0xff, -1, -1);
    gINT_00705b70 = GetTotalTime();
done:
    return powerup - gPowerup_array;
}

// FUNCTION: CARMA2_HW 0x004dc9c0
void C2_HOOK_FASTCALL ResetPedSpeed(tPowerup* powerup, tCar_spec* car) {
    gPedestrian_speed_factor = 1.0f;
}

// FUNCTION: CARMA2_HW 0x004dc9d0
void C2_HOOK_FASTCALL ResetPedSize(tPowerup* powerup, tCar_spec* car) {
    br_vector3 scale = { 1.0f, 1.0f, 1.0f };

    ScalePeds(&scale);
}

// FUNCTION: CARMA2_HW 0x004dca10
void C2_HOOK_FASTCALL ResetPedExplode(tPowerup* powerup, tCar_spec* car) {
    gExploding_pedestrians = 0;
}

// FUNCTION: CARMA2_HW 0x004dc860
void C2_HOOK_FASTCALL ResetInvulnerability(tPowerup* powerup, tCar_spec* car) {
    if (gNet_mode) {
        if (NetPlayerFromCar(car)->field_0x80 != 0) {
            return;
        }
    }
    *(int *)((tU8 *)car + 0x1b4) = 0;
    *(int *)((tU8 *)car + 0x1b8) = 0;
}

// FUNCTION: CARMA2_HW 0x004dc8d0
void C2_HOOK_FASTCALL ResetFreeRepairs(tPowerup* powerup, tCar_spec* car) {
    if (car && car->driver == eDriver_local_human) {
        gFree_repairs = 0;
    }
}

// FUNCTION: CARMA2_HW 0x004dc970
void C2_HOOK_FASTCALL UnfreezeTimer(tPowerup* powerup, tCar_spec* car) {
    if (car && car->driver == eDriver_local_human) {
        gFreeze_timer = 0;
    }
}

void C2_HOOK_FASTCALL FUN_0045c760(void);

// FUNCTION: CARMA2_HW 0x004de5d0
void C2_HOOK_FASTCALL ResetEngineFactor(tPowerup* powerup, tCar_spec* car) {
    float f = 1.0f;

    FUN_0045c760();
    car->field_0x4d4 = f;
    car->grip_multiplier = f;
    ((void (C2_HOOK_FAKE_THISCALL *)(tCar_spec*, float, float, float, float, float))SetCarSuspGiveAndHeight)(car, f, f, f, 0.0f, 0.0f);
}

// FUNCTION: CARMA2_HW 0x004de600
void C2_HOOK_FASTCALL ResetUnderwater(tPowerup* powerup, tCar_spec* car) {
    car->underwater_ability = 0;
}

// FUNCTION: CARMA2_HW 0x004de610
void C2_HOOK_FASTCALL PukeDrugsBackUp(tPowerup* powerup, tCar_spec* car) {
    if (car != NULL && car->driver == eDriver_local_human) {
        gOn_drugs = 0;
        PaletteFuckedUpByDrugs(gRender_palette, 0);
        ResetPalette();
    }
}

// FUNCTION: CARMA2_HW 0x004de6b0
void C2_HOOK_FASTCALL ResetOpponentsSpeed(tPowerup* powerup, tCar_spec* car) {
    int count = gCurrent_race.number_of_racers;
    int i;

    for (i = 0; i < count; i++) {
        if (gCurrent_race.opponent_list[i].index == 0x1d) {
            break;
        }
    }
    if (i < count) {
        tCar_spec* opp_car = gCurrent_race.opponent_list[i].car_spec;
        float f = 1.0f;

        FUN_0045c760();
        opp_car->field_0x4d4 = f;
        opp_car->grip_multiplier = f;
        SetCarSuspGiveAndHeight(opp_car, f, f, f, 0.0f, 0.0f);
    }
    gOpponent_speed_factor = 1.0f;
}

// FUNCTION: CARMA2_HW 0x004de710
void C2_HOOK_FASTCALL ResetCopsSpeed(tPowerup* powerup, tCar_spec* car) {
    gCop_speed_factor = 1.0f;
}

// FUNCTION: CARMA2_HW 0x004de720
void C2_HOOK_FASTCALL ResetGravity(tPowerup* powerup, tCar_spec* car) {
    gGravity_multiplier = gDefault_gravity;
}

// FUNCTION: CARMA2_HW 0x004de730
void C2_HOOK_FASTCALL ResetPinball(tPowerup* powerup, tCar_spec* car) {
    gPinball_factor = 0.0f;
}

// FUNCTION: CARMA2_HW 0x004de740
void C2_HOOK_FASTCALL ResetWallclimb(tPowerup* powerup, tCar_spec* car) {
    car->wall_climber_mode = 0;
}

// FUNCTION: CARMA2_HW 0x004de750
void C2_HOOK_FASTCALL ResetBouncey(tPowerup* powerup, tCar_spec* car) {
    car->bounce_rate = 0.0f;
    car->bounce_amount = 0.0f;
}

// FUNCTION: CARMA2_HW 0x004de760
void C2_HOOK_FASTCALL ResetSuspension(tPowerup* powerup, tCar_spec* car) {
    SetCarSuspGiveAndHeight(car, 1.0f, 1.0f, 1.0f, 0.0f, 0.0f);
}

// FUNCTION: CARMA2_HW 0x004de790
void C2_HOOK_FASTCALL ResetTyreGrip(tPowerup* powerup, tCar_spec* car) {
    car->grip_multiplier = 1.0f;
}

// FUNCTION: CARMA2_HW 0x004de780
void C2_HOOK_FASTCALL ResetDamageMultiplier(tPowerup* powerup, tCar_spec* car) {
    car->damage_multiplier = 1.0f;
}

// FUNCTION: CARMA2_HW 0x004dcf30
void C2_HOOK_FASTCALL ResetImmortalPeds(tPowerup* powerup, tCar_spec* car) {
    gImmortal_peds = 0;
}

// FUNCTION: CARMA2_HW 0x004dc920
void C2_HOOK_FASTCALL ResetStupidPedestrians(tPowerup* powerup, tCar_spec* car) {
    gBlind_pedestrians = 0;
}

// FUNCTION: CARMA2_HW 0x004dcf70
void C2_HOOK_FASTCALL ResetSuicidalPeds(tPowerup* powerup, tCar_spec* car) {
    gPeds_suicidal = 0;
}

// FUNCTION: CARMA2_HW 0x004debf0
void C2_HOOK_FASTCALL ResetMassMultiplier(tPowerup* powerup, tCar_spec* car) {
    car->field_0x4c8 = 1.0f;
    if (gNet_mode) {
        RestoreCarPixelmaps(car);
    }
}

// FUNCTION: CARMA2_HW 0x004de850
void C2_HOOK_FASTCALL ResetInstantHandbrake(tPowerup* powerup, tCar_spec* car) {
    if (car && car->driver == eDriver_local_human) {
        gInstant_handbrake = 0;
    }
}

// FUNCTION: CARMA2_HW 0x004dec40
void C2_HOOK_FASTCALL HidePedestrians(tPowerup* powerup, tCar_spec* car) {
    if (car && car->driver == eDriver_local_human) {
        gShow_peds_on_map = 0;
    }
}

// FUNCTION: CARMA2_HW 0x004dca00
void C2_HOOK_FASTCALL ResetPedHeadSize(tPowerup* powerup, tCar_spec* car) {
    ScalePedHeads(1.0f);
}

// FUNCTION: CARMA2_HW 0x004decd0
void C2_HOOK_FASTCALL ResetMutantCorpses(tPowerup* powerup, tCar_spec* car) {
    gMutant_speed = 0.0f;
}

// FUNCTION: CARMA2_HW 0x004dca20
void C2_HOOK_FASTCALL ResetPedBrittle(tPowerup* powerup, tCar_spec* car) {
    gPed_dismemberfest = 0;
}

// FUNCTION: CARMA2_HW 0x004de9a0
void C2_HOOK_FASTCALL ResetGhostPeds(tPowerup* powerup, tCar_spec* car) {
    MakePedsNotEthereal();
}

// FUNCTION: CARMA2_HW 0x004de960
void C2_HOOK_FASTCALL ResetDancingPeds(tPowerup* powerup, tCar_spec* car) {
    gDancing_peds = 0;
}

// FUNCTION: CARMA2_HW 0x004de970
void C2_HOOK_FASTCALL ResetPanickingPeds(tPowerup* powerup, tCar_spec* car) {
    gPanicking_peds = 0;
}

// FUNCTION: CARMA2_HW 0x004de980
void C2_HOOK_FASTCALL ResetLowGravityPeds(tPowerup* powerup, tCar_spec* car) {
    gPed_gravity_multiplier = 1.0f;
}

// FUNCTION: CARMA2_HW 0x004de990
void C2_HOOK_FASTCALL ResetPissedPeds(tPowerup* powerup, tCar_spec* car) {
    gDrunk_pedestrians = 0;
}

// FUNCTION: CARMA2_HW 0x004df300
void C2_HOOK_FASTCALL ResetPissed(tPowerup* powerup, tCar_spec* car) {
    br_scalar sx;
    br_scalar sy;
    br_scalar sz;

    gCredit_multiplier = 0;
    sx = gCamera->t.t.mat.m[3][0];
    sy = gCamera->t.t.mat.m[3][1];
    sz = gCamera->t.t.mat.m[3][2];
    BrMatrix34Identity(&gCamera->t.t.mat);
    gCamera->t.t.mat.m[3][0] = sx;
    gCamera->t.t.mat.m[3][1] = sy;
    gCamera->t.t.mat.m[3][2] = sz;
    sx = gRearview_camera->t.t.mat.m[3][0];
    sy = gRearview_camera->t.t.mat.m[3][1];
    sz = gRearview_camera->t.t.mat.m[3][2];
    BrMatrix34Identity(&gRearview_camera->t.t.mat);
    gRearview_camera->t.t.mat.m[3][0] = sx;
    gRearview_camera->t.t.mat.m[3][1] = sy;
    gRearview_camera->t.t.mat.m[3][2] = sz;
}

// FUNCTION: CARMA2_HW 0x004dcc00
void C2_HOOK_FASTCALL TheEffectsOfDrugs(tPowerup* powerup, tU32 time) {
    PaletteFuckedUpByDrugs(gRender_palette, GetTotalTime() / 100);
    ResetPalette();
}

// FUNCTION: CARMA2_HW 0x004de9b0
void C2_HOOK_FASTCALL DoBouncey(tPowerup* powerup, tU32 time) {
    if (GetTotalTime() - gProgram_state.current_car.last_bounce < gProgram_state.current_car.bounce_rate
        || gProgram_state.current_car.number_of_wheels_on_ground <= 2) {
        return;
    }
    PratcamEvent(0x2a);
    gProgram_state.current_car.last_bounce = GetTotalTime();
    gProgram_state.current_car.collision_info->v.v[1] =
        gProgram_state.current_car.bounce_amount * gConst_kangeroo_force_factor
        + gProgram_state.current_car.collision_info->v.v[1];
    DRS3StartSound(gXXX_outlet, 0x2332);
    gProgram_state.current_car.collision_info->disable_move_rotate = 0;
}

// FUNCTION: CARMA2_HW 0x004de810
void C2_HOOK_FASTCALL MungeVouchers(tPowerup* powerup, tU32 time) {
    (void)time;
    powerup->current_value = GetRecoverVoucherCount();
}

// FUNCTION: CARMA2_HW 0x004df3b0
void C2_NAKED C2_HOOK_FASTCALL PissedEffect(tPowerup* powerup, tU32 time) {
#ifndef CARPOCALYPSE2_MATCHING
    tU32 now;
    float* pOut;
    float* p;
    int k;
    float amp;
    float period;
    float phase_off;
    double ph;
    double c;
    int idx;
    float v;

    (void)powerup;
    (void)time;
    now = GetTotalTime();
    if (gCredit_multiplier == 0) {
        return;
    }
    pOut = gPissed_flicker_frames;
    do {
        p = pOut;
        for (k = 1; k != 0; k--) {
            amp = p[-12];
            if (amp != 0.0f) {
                period = p[0];
                phase_off = p[12];
                ph = fmod((double)now + phase_off, period) / period * 6.283185307179586;
                if (ph > 4.71238898038469) {
                    idx = (int)((6.283185307179586 - ph) * 40.74366543152521);
                    c = (double)gCosine_array[idx];
                } else if (ph > 3.141592653589793) {
                    idx = (int)((ph - 3.141592653589793) * 40.74366543152521);
                    c = -(double)gCosine_array[idx];
                } else if (ph > 1.5707963267948966) {
                    idx = (int)((3.141592653589793 - ph) * 40.74366543152521);
                    c = -(double)gCosine_array[idx];
                } else {
                    idx = (int)(ph * 40.74366543152521);
                    c = (double)gCosine_array[idx];
                }
                v = (float)((c * amp + 1.0f) * gCamera_angle * 182.04444444444445);
                *(tU16*)((tU8*)gCamera->type_data + 6) = (tU16)(int)v;
            }
            p += 1;
        }
        pOut += 3;
    } while (pOut < &gPissed_flicker_limit);
    if (now > (tU32)gPissed_next_time) {
        int pitch;
        int r;
        int sound;

        pitch = IRandomBetween(0x5dc, 0xbb8);
        r = PercentageChance(50);
        sound = r ? 0xe12 : 0xe13;
        DRS3StartSound2(gXXX_outlet, sound, 1, 0xff, 0xff, pitch, -1);
        gPissed_next_time = GetTotalTime() + IRandomBetween(0x5dc, 0xbb8);
    }
#else
    __asm {
            sub esp, 0xc
            push ebx
            push ebp
            push esi
            push edi
            call GetTotalTime
            mov ebp, eax
            mov eax, dword ptr [gCredit_multiplier]
            test eax, eax
            je L004df565
            mov ebx, offset gPissed_flicker_frames
    L004df3d0:
            mov esi, ebx
            mov edi, 1
    L004df3d7:
            fld dword ptr [esi - 0x30]
            fst dword ptr [esp + 0x10]
            fcomp dword ptr [gFLOAT_0058ac0c]
            fnstsw ax
            test ah, 0x40
            jne L004df4e2
            _emit 0xd9
        _emit 0x06
            mov dword ptr [esp + 0x14], ebp
            mov dword ptr [esp + 0x18], 0
            fild qword ptr [esp + 0x14]
            fadd dword ptr [esi + 0x30]
            fxch st(1)
            call _CIfmod
            _emit 0xd8
        _emit 0x36
            fmul qword ptr [gDouble_0058abe8]
            fcom qword ptr [gConst_kangeroo_force_factor+0x8]
            fnstsw ax
            test ah, 0x41
            jne L004df443
            fld qword ptr [gDouble_0058abe8]
            fsub st(0), st(1)
            fmul qword ptr [gConst_kangeroo_force_factor+0x10]
            call _ftol
            shl eax, 2
            mov ecx, offset gCosine_array
            sub ecx, eax
            fstp st(0)
            _emit 0xd9
        _emit 0x01
            jmp L004df4ba
    L004df443:
            fcom qword ptr [gConst_kangeroo_force_factor+0x18]
            fnstsw ax
            test ah, 0x41
            jne L004df471
            fsub qword ptr [gConst_kangeroo_force_factor+0x18]
            fmul qword ptr [gConst_kangeroo_force_factor+0x10]
            call _ftol
            mov edx, offset gCosine_array
            shl eax, 2
            sub edx, eax
            _emit 0xd9
        _emit 0x02
            fchs
            jmp L004df4ba
    L004df471:
            fcom qword ptr [gConst_kangeroo_force_factor+0x20]
            fnstsw ax
            test ah, 0x41
            jne L004df4a3
            fld qword ptr [gConst_kangeroo_force_factor+0x18]
            fsub st(0), st(1)
            fmul qword ptr [gConst_kangeroo_force_factor+0x10]
            call _ftol
            mov ecx, offset gCosine_array
            shl eax, 2
            sub ecx, eax
            fstp st(0)
            _emit 0xd9
        _emit 0x01
            fchs
            jmp L004df4ba
    L004df4a3:
            fmul qword ptr [gConst_kangeroo_force_factor+0x10]
            call _ftol
            shl eax, 2
            mov edx, offset gCosine_array
            sub edx, eax
            _emit 0xd9
        _emit 0x02
    L004df4ba:
            fmul dword ptr [esp + 0x10]
            fsub dword ptr [gConst_kangeroo_force_factor+0x28]
            fmul dword ptr [gCamera_angle]
            fmul qword ptr [gConst_kangeroo_force_factor+0x30]
            call _ftol
            mov ecx, dword ptr [gCamera]
            mov edx, dword ptr [ecx + 0x5c]
            mov word ptr [edx + 6], ax
    L004df4e2:
            add esi, 4
            dec edi
            jne L004df3d7
            add ebx, 0xc
            cmp ebx, offset gPissed_flicker_limit
            jl L004df3d0
            call GetTotalTime
            cmp eax, dword ptr [gPissed_next_time]
            jbe L004df565
            push -1
            mov edx, 0x14000
            mov ecx, 0xc000
            call IRandomBetween
            mov ecx, 0x32
            push eax
            push 0xff
            push 0xff
            push 1
            call PercentageChance
            mov ecx, dword ptr [gXXX_outlet]
            mov edx, eax
            neg edx
            sbb edx, edx
            add edx, 0xe13
            call DRS3StartSound2
            mov edx, 0xbb8
            mov ecx, 0x5dc
            call IRandomBetween
            mov esi, eax
            call GetTotalTime
            add esi, eax
            mov dword ptr [gPissed_next_time], esi
    L004df565:
            pop edi
            pop esi
            pop ebp
            pop ebx
            add esp, 0xc
            ret
            nop
            nop
            nop
    }
#endif
}

// FUNCTION: CARMA2_HW 0x004dcfb0
void C2_NAKED C2_HOOK_FASTCALL SendMines(tU32 pNext_frame_time) {
#ifndef CARPOCALYPSE2_MATCHING
    int j;
    int k;
    int hierarchy_size;
    int chunk_size;
    tShit_mine* mine;
    tU8 tmp_buf[580];
    tNet_message_chunk_mine_explode* tmp = (tNet_message_chunk_mine_explode*)tmp_buf;
    tNet_message_chunk* chunk;
    br_vector3* mine_pos;
    br_vector3* car_pos;
    tCar_spec* car;
    float dx, dy, dz;
    tPlayer_ID target_id;
    tPlayer_ID proxy_id;
    int check_idx;

    if (gNet_mode != eNet_mode_host) {
        return;
    }
    if (gNumber_of_net_players == 1) {
        return;
    }
    for (mine = gShit_mines; mine < &gShit_mines[CARPOCALYPSE2_ASIZE(gShit_mines)]; mine++) {
        if (!(mine->flags & 1)) {
            continue;
        }
        tmp->mine_index = (tS8)(mine - gShit_mines);
        tmp->owner_index = mine->field_0x35;
        tmp->powerup_index = mine->field_0x34;
        tmp->player_index = (tU8)(NetPlayerFromCar(mine->field_0x30) - gNet_players);
        tmp->field_0x8 = (tS32)pNext_frame_time;
        GetHierarchyNetworkStuff(mine->collision_info, tmp->object_hierarchy, NET_OBJECT_HIERARCHY_DEFAULT_SIZE);
        hierarchy_size = GetHierarchyNetworkSize(mine->collision_info);
        chunk_size = (int)((float)hierarchy_size - (-12.0f));
        tmp->header.size = (tU8)chunk_size;
        tmp->header.type = 0x31;
        for (j = 1; j < gNumber_of_net_players; j++) {
            target_id = gNet_players[j].ID;
            proxy_id = gNet_players[j].field_0xd0;
            if (proxy_id == 0) {
                check_idx = j;
            } else {
                check_idx = 0;
                if (gNumber_of_net_players - 1 > 0) {
                    for (k = 0; k < gNumber_of_net_players - 1; k++) {
                        if (gNet_players[k].ID == proxy_id) {
                            break;
                        }
                    }
                    check_idx = k;
                }
            }
            mine_pos = (br_vector3*)&mine->collision_info->transform_matrix.m[3][0];
            car = gNet_players[check_idx].car;
            car_pos = (br_vector3*)&car->collision_info->transform_matrix.m[3][0];
            dx = mine_pos->v[0] - car_pos->v[0];
            dy = mine_pos->v[1] - car_pos->v[1];
            dz = mine_pos->v[2] - car_pos->v[2];
            if (dx * dx + dy * dy + dz * dz >= 400.0f) {
                continue;
            }
            chunk = NetGetToPlayerContents(0x31, chunk_size, target_id);
            if (chunk == NULL) {
                continue;
            }
            memcpy(chunk, tmp, chunk_size);
        }
    }
#else
    __asm {
            sub esp, 0x254
            mov eax, dword ptr [gNet_mode]
            mov dword ptr [esp + 8], ecx
            cmp eax, 2
            jne L004dd1a2
            cmp dword ptr [gNumber_of_net_players], 1
            je L004dd1a2
            push edi
            push esi
            push ebp
            push ebx
            mov ebp, offset gShit_mines+0x34
    L004dcfde:
            mov cl, byte ptr [ebp - 0x34]
            lea eax, [ebp - 0x34]
            test cl, 1
            je L004dd18f
            sub eax, offset gShit_mines
            mov ecx, eax
            mov eax, 0x92492493
            imul ecx
            add edx, ecx
            mov cl, byte ptr [ebp + 1]
            sar edx, 5
            mov eax, edx
            mov byte ptr [esp + 0x2b], cl
            mov ecx, dword ptr [ebp - 4]
            shr eax, 0x1f
            add edx, eax
            mov byte ptr [esp + 0x2a], dl
            mov dl, byte ptr [ebp]
            mov byte ptr [esp + 0x2c], dl
            call NetPlayerFromCar
            mov ecx, eax
            mov eax, 0x4d4873ed
            sub ecx, offset gNet_players
            push 0x48
            imul ecx
            mov ecx, dword ptr [esp + 0x1c]
            sar edx, 6
            mov eax, edx
            mov dword ptr [esp + 0x34], ecx
            mov ecx, dword ptr [ebp - 0x2c]
            shr eax, 0x1f
            add edx, eax
            mov byte ptr [esp + 0x31], dl
            lea edx, [esp + 0x38]
            call GetHierarchyNetworkStuff
            mov ecx, dword ptr [ebp - 0x2c]
            call GetHierarchyNetworkSize
            mov dword ptr [esp + 0x14], eax
            fild dword ptr [esp + 0x14]
            fsub dword ptr [gBounce_rate_const+0x4]
            call _ftol
            mov ebx, eax
            mov eax, dword ptr [gNumber_of_net_players]
            mov edi, 1
            mov byte ptr [esp + 0x28], bl
            cmp eax, edi
            mov byte ptr [esp + 0x29], 0x31
            mov dword ptr [esp + 0x14], edi
            jle L004dd18f
            mov dword ptr [esp + 0x10], 0x74be64
    L004dd098:
            mov edx, dword ptr [esp + 0x10]
            mov esi, dword ptr [edx]
            test esi, esi
            je L004dd0c7
            mov ecx, dword ptr [gNumber_of_net_players]
            xor eax, eax
            lea edx, [ecx - 1]
            test edx, edx
            jle L004dd0c9
            mov ecx, offset gNet_players+0x24
    L004dd0b6:
            cmp dword ptr [ecx], esi
            je L004dd0c9
            inc eax
            add ecx, 0xd4
            cmp eax, edx
            jl L004dd0b6
            jmp L004dd0c9
    L004dd0c7:
            mov eax, edi
    L004dd0c9:
            mov ecx, dword ptr [ebp - 0x2c]
            lea edx, [eax + eax*2]
            fld dword ptr [ecx + 0xb0]
            lea edx, [edx + edx*8]
            fld dword ptr [ecx + 0xb4]
            shl edx, 1
            fld dword ptr [ecx + 0xb8]
            fxch st(2)
            sub edx, eax
            mov eax, dword ptr [edx*4 + gNet_players+0xc8]
            mov eax, dword ptr [eax + 8]
            fsub dword ptr [eax + 0xb0]
            fxch st(1)
            fsub dword ptr [eax + 0xb4]
            fxch st(2)
            fsub dword ptr [eax + 0xb8]
            fxch st(1)
            fst dword ptr [esp + 0x1c]
            fxch st(2)
            fst dword ptr [esp + 0x20]
            fxch st(2)
            fmul dword ptr [esp + 0x1c]
            fxch st(2)
            fmul dword ptr [esp + 0x20]
            fld st(1)
            fmul st(0), st(2)
            fxch st(1)
            faddp st(3), st(0)
            faddp st(2), st(0)
            fxch st(1)
            fcomp dword ptr [gBounce_rate_const+0x8]
            fnstsw ax
            test ah, 1
            fstp st(0)
            je L004dd16f
            mov ecx, dword ptr [esp + 0x10]
            mov edx, dword ptr [ecx - 0xac]
            mov cl, 0x31
            push edx
            mov edx, ebx
            call NetGetToPlayerContents
            test eax, eax
            je L004dd16f
            mov ecx, ebx
            mov edi, eax
            mov eax, ecx
            lea esi, [esp + 0x28]
            shr ecx, 2
            rep movsd
            mov ecx, eax
            and ecx, 3
            rep movsb
            mov edi, dword ptr [esp + 0x14]
    L004dd16f:
            mov ecx, dword ptr [esp + 0x10]
            mov eax, dword ptr [gNumber_of_net_players]
            inc edi
            add ecx, 0xd4
            cmp edi, eax
            mov dword ptr [esp + 0x14], edi
            mov dword ptr [esp + 0x10], ecx
            jl L004dd098
    L004dd18f:
            add ebp, 0x38
            cmp ebp, offset gPickedup_powerups+0x14
            jl L004dcfde
            pop ebx
            pop ebp
            pop esi
            pop edi
    L004dd1a2:
            add esp, 0x254
            ret
    }
#endif
}

// FUNCTION: CARMA2_HW 0x004db730
void C2_NAKED C2_HOOK_FASTCALL KeyboardPowerupFinished(tPowerup* pPowerup, int pSelect_new) {
#ifndef CARPOCALYPSE2_MATCHING
    int i;

    if (pPowerup->initial_value < 0 || gINT_006a0a6c != 0) {
        if (pPowerup->got_time != 0 && pPowerup->lose_proc != NULL) {
            pPowerup->lose_proc(pPowerup, pPowerup->car);
        }
        if (pPowerup->net_type & 0x80) {
            ARPowerupLose(pPowerup, pPowerup->car);
        }
        if (gNet_mode != eNet_mode_none) {
            tNet_message* msg = NetBuildGuaranteedMessage(0x16, 0);
            *(int*)((tU8*)msg + 0x28) = 2;
            *(tPlayer_ID*)((tU8*)msg + 0x1c) = gLocal_net_ID;
            *(int*)((tU8*)msg + 0x20) = pPowerup - gPowerup_array;
            NetGuaranteedSendMessageToAllPlayers(gCurrent_net_game, msg, NULL);
        }
        gProgram_state.current_car.powerups[pPowerup - gPowerup_array] = 0;
        if (pPowerup->initial_value == 0) {
            for (i = 0; i < gNumber_of_icons; i++) {
                if (pPowerup == gPickedup_powerups[i].powerup) {
                    gPickedup_powerups[i].fizzle_stage = 3;
                    gPickedup_powerups[i].fizzle_direction = -1;
                    gPickedup_powerups[i].fizzle_start = GetTotalTime();
                    break;
                }
            }
        }
        pPowerup->got_time = 0;
    }
    gINT_006a0a5c = 1;
    if (!pSelect_new) {
        return;
    }
    i = gInventory_selected;
    gInventory[i] = gInventory_selected = -1;
    for (i = 0; i < (int)CARPOCALYPSE2_ASIZE(gInventory); i++) {
        if (gInventory[i] >= 0) {
            gInventory_selected = i;
            break;
        }
    }
#else
    __asm {
            push ebp
            push esi
            mov esi, ecx
            push edi
            mov ebp, edx
            mov eax, dword ptr [esi + 0x34]
            test eax, eax
            jl L004db74b
            mov eax, dword ptr [gINT_006a0a6c]
            test eax, eax
            je L004db838
    L004db74b:
            mov eax, dword ptr [esi + 4]
            test eax, eax
            je L004db760
            mov eax, dword ptr [esi + 0x40]
            test eax, eax
            je L004db760
            mov edx, dword ptr [esi + 0x4c]
            mov ecx, esi
            call eax
    L004db760:
            test byte ptr [esi + 0x38], 0x80
            je L004db770
            mov edx, dword ptr [esi + 0x4c]
            mov ecx, esi
            call ARPowerupLose
    L004db770:
            mov eax, dword ptr [gNet_mode]
            test eax, eax
            je L004db7c0
            xor edx, edx
            mov cl, 0x16
            call NetBuildGuaranteedMessage
            mov ecx, eax
            mov edx, esi
            push 0
            mov dword ptr [ecx + 0x28], 2
            mov eax, dword ptr [gLocal_net_ID]
            mov dword ptr [ecx + 0x1c], eax
            mov edi, dword ptr [gPowerup_array]
            sub edx, edi
            mov eax, 0x2fa0be83
            imul edx
            sar edx, 5
            mov eax, edx
            shr eax, 0x1f
            add edx, eax
            mov dword ptr [ecx + 0x20], edx
            mov edx, ecx
            mov ecx, dword ptr [gCurrent_net_game]
            call NetGuaranteedSendMessageToAllPlayers
    L004db7c0:
            mov edx, dword ptr [gPowerup_array]
            mov ecx, esi
            sub ecx, edx
            mov eax, 0x2fa0be83
            imul ecx
            sar edx, 5
            mov ecx, edx
            shr ecx, 0x1f
            add edx, ecx
            mov dword ptr [edx*4 + gProgram_state+0x17bc], 0
            mov eax, dword ptr [esi + 0x34]
            test eax, eax
            jne L004db831
            mov edx, dword ptr [gNumber_of_icons]
            xor eax, eax
            test edx, edx
            jle L004db831
            mov ecx, offset gPickedup_powerups
    L004db7fe:
            cmp esi, dword ptr [ecx]
            je L004db80c
            inc eax
            add ecx, 0x14
            cmp eax, edx
            jl L004db7fe
            jmp L004db831
    L004db80c:
            lea edi, [eax + eax*4]
            shl edi, 2
            mov dword ptr [edi + gPickedup_powerups+0x4], 3
            mov dword ptr [edi + gPickedup_powerups+0x8], 0xffffffff
            call GetTotalTime
            mov dword ptr [edi + gPickedup_powerups+0xc], eax
    L004db831:
            mov dword ptr [esi + 4], 0
    L004db838:
            test ebp, ebp
            mov dword ptr [gINT_006a0a5c], 1
            je L004db87c
            mov edx, dword ptr [gInventory_selected]
            or eax, 0xffffffff
            mov dword ptr [gInventory_selected], eax
            xor ecx, ecx
            mov dword ptr [edx*4 + gInventory], eax
            mov eax, offset gInventory
    L004db862:
            cmp dword ptr [eax], 0
            jge L004db876
            add eax, 4
            inc ecx
            cmp eax, offset gFizzle_in
            jl L004db862
            pop edi
            pop esi
            pop ebp
            ret
    L004db876:
            mov dword ptr [gInventory_selected], ecx
    L004db87c:
            pop edi
            pop esi
            pop ebp
            ret
    }
#endif
}

// FUNCTION: CARMA2_HW 0x004daff0
int C2_NAKED C2_HOOK_FASTCALL DrawSinglePowerupIcon(int pDraw, int pTime, tPowerup* pPowerup, tHeadup_icon* pIcon, int pX, int pY, int pUpdate_x) {
#ifndef CARPOCALYPSE2_MATCHING
    br_pixelmap *fizzle_pix;
    char s[8];
    int timer;

    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tGraf_data, power_up_space_between_icon_text_dx, 0x294);
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tGraf_data, power_up_space_between_icon_text_dy, 0x2a0);

    if (pIcon != NULL && pIcon->fizzle_stage < 4) {
        fizzle_pix = gFizzle_in[pPowerup->fizzle_type];
        if (pDraw) {
            PrintPowerupIconIn3D(pX, pY, pIcon, pPowerup, 1, pTime);
        }
        if (pUpdate_x) {
            return pX + fizzle_pix->width;
        }
        return 0;
    }
    if (pDraw) {
        PrintPowerupIconIn3D(pX, pY, pIcon, pPowerup, 0, pTime);
    }
    if (pPowerup->initial_value < 0) {
        sprintf(s, "%d", -pPowerup->value);
        if (pDraw) {
            DRPixelmapText(gBack_screen,
                pX + gCurrent_graf_data->power_up_space_between_icon_text_dx,
                pY + gCurrent_graf_data->power_up_space_between_icon_text_dy,
                &gFonts[2],
                s,
                pX + gCurrent_graf_data->power_up_space_between_icon_text_dx + 30);
        }
        if (pUpdate_x) {
            return pX + DRTextWidth(&gFonts[2], s);
        }
    } else if (pPowerup->initial_value > 0) {
        TimerString(pPowerup->value, s, 0, 0, 1);
        if (pDraw) {
            DRPixelmapText(gBack_screen,
                pX + gCurrent_graf_data->power_up_space_between_icon_text_dx,
                pY + gCurrent_graf_data->power_up_space_between_icon_text_dy,
                &gFonts[2],
                s,
                pX + gCurrent_graf_data->power_up_space_between_icon_text_dx + 30);
        }
        if (pUpdate_x) {
            return pX + DRTextWidth(&gFonts[2], s);
        }
    } else if (pPowerup->type == ePowerup_timed) {

        timer = pPowerup->lose_time - pTime;
        if (timer < 0) {
            timer = 0;
        }
        TimerString(timer, s, 0, 0, 1);
        if (pDraw) {
            DRPixelmapText(gBack_screen,
                pX + gCurrent_graf_data->power_up_space_between_icon_text_dx,
                pY + gCurrent_graf_data->power_up_space_between_icon_text_dy,
                &gFonts[1],
                s,
                pX + gCurrent_graf_data->power_up_space_between_icon_text_dx + 30);
        }
        if (pUpdate_x) {
            return pX + DRTextWidth(&gFonts[1], s);
        }
    } else if (pPowerup->current_value > 0) {

        sprintf(s, "%d", pPowerup->current_value);
        if (pDraw) {
            DRPixelmapText(gBack_screen,
                pX + gCurrent_graf_data->power_up_space_between_icon_text_dx,
                pY + gCurrent_graf_data->power_up_space_between_icon_text_dy,
                &gFonts[2],
                s,
                pX + gCurrent_graf_data->power_up_space_between_icon_text_dx + 30);
        }
        if (pUpdate_x) {
            return pX + DRTextWidth(&gFonts[2], s);
        }
    }
    return 0;
#else
    __asm {
            sub esp, 0xc
            mov eax, dword ptr [esp + 0x14]
            push ebx
            push ebp
            push esi
            push edi
            mov dword ptr [esp + 0x10], edx
            test eax, eax
            mov edi, ecx
            je L004db04f
            cmp dword ptr [eax + 4], 4
            jge L004db04f
            mov ecx, dword ptr [esp + 0x20]
            test edi, edi
            mov esi, dword ptr [ecx + 0x18]
            mov ebx, dword ptr [esi*4 + gFizzle_in]
            mov esi, dword ptr [esp + 0x28]
            je L004db031
            push edx
            mov edx, dword ptr [esp + 0x30]
            push 1
            push ecx
            push eax
            mov ecx, esi
            call PrintPowerupIconIn3D
    L004db031:
            mov eax, dword ptr [esp + 0x30]
            test eax, eax
            je L004db1aa
            xor eax, eax
            mov ax, word ptr [ebx + 0x34]
            add eax, esi
            pop edi
            pop esi
            pop ebp
            pop ebx
            add esp, 0xc
            ret 0x14
    L004db04f:
            mov ebp, dword ptr [esp + 0x2c]
            mov ebx, dword ptr [esp + 0x28]
            mov esi, dword ptr [esp + 0x20]
            test edi, edi
            je L004db071
            push edx
            push 0
            push esi
            push eax
            mov edx, ebp
            mov ecx, ebx
            call PrintPowerupIconIn3D
            mov edx, dword ptr [esp + 0x10]
    L004db071:
            mov eax, dword ptr [esi + 0x34]
            test eax, eax
            jge L004db092
            mov eax, dword ptr [esi + 0x30]
            lea ecx, [esp + 0x14]
            neg eax
            push eax
            push offset gPed_flags_0058f2e4+0x134
            push ecx
            call sprintf
            add esp, 0xc
            jmp L004db0a6
    L004db092:
            jle L004db0c0
            mov ecx, dword ptr [esi + 0x30]
            push 1
            push 0
            push 0
            lea edx, [esp + 0x20]
            call TimerString
    L004db0a6:
            test edi, edi
            je L004db17c
            mov eax, dword ptr [gCurrent_graf_data]
            mov edx, dword ptr [eax + 0x294]
            add edx, ebx
            jmp L004db15a
    L004db0c0:
            cmp dword ptr [esi], 2
            jne L004db12e
            mov esi, dword ptr [esi + 0xc]
            mov ecx, 0
            sub esi, edx
            push 1
            sets cl
            dec ecx
            push 0
            push 0
            lea edx, [esp + 0x20]
            and ecx, esi
            call TimerString
            test edi, edi
            je L004db117
            mov eax, dword ptr [gCurrent_graf_data]
            mov edx, dword ptr [eax + 0x294]
            mov eax, dword ptr [eax + 0x2a0]
            add edx, ebx
            add eax, ebp
            lea ecx, [edx + 0x1e]
            push ecx
            lea ecx, [esp + 0x18]
            push ecx
            mov ecx, dword ptr [gBack_screen]
            push offset gFonts+0x3a0
            push eax
            call DRPixelmapText
    L004db117:
            mov eax, dword ptr [esp + 0x30]
            test eax, eax
            je L004db1aa
            lea edx, [esp + 0x14]
            mov ecx, offset gFonts+0x3a0
            jmp L004db18d
    L004db12e:
            mov esi, dword ptr [esi + 0x28]
            test esi, esi
            jle L004db1aa
            push esi
            lea edx, [esp + 0x18]
            push offset gPed_flags_0058f2e4+0x134
            push edx
            call sprintf
            add esp, 0xc
            test edi, edi
            je L004db17c
            mov eax, dword ptr [gCurrent_graf_data]
            mov ecx, dword ptr [eax + 0x294]
            lea edx, [ecx + ebx]
    L004db15a:
            mov eax, dword ptr [eax + 0x2a0]
            lea ecx, [edx + 0x1e]
            push ecx
            lea ecx, [esp + 0x18]
            push ecx
            mov ecx, dword ptr [gBack_screen]
            add eax, ebp
            push offset gFonts+0x740
            push eax
            call DRPixelmapText
    L004db17c:
            mov eax, dword ptr [esp + 0x30]
            test eax, eax
            je L004db1aa
            lea edx, [esp + 0x14]
            mov ecx, offset gFonts+0x740
    L004db18d:
            call DRTextWidth
            mov ecx, dword ptr [gCurrent_graf_data]
            add eax, dword ptr [ecx + 0x294]
            add eax, ebx
            pop edi
            pop esi
            pop ebp
            pop ebx
            add esp, 0xc
            ret 0x14
    L004db1aa:
            pop edi
            pop esi
            pop ebp
            xor eax, eax
            pop ebx
            add esp, 0xc
            ret 0x14
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

__inline void C2_HOOK_FASTCALL DrawPowerupIcons2(tU32 pTime, tHeadup_icon* pHeadup_icons, int* pCount_icons, int pMax_count_icons, int pX, int pY) {
    int i;
    int y;
    tPowerup *the_powerup;
    tHeadup_icon *the_icon;

    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tGraf_data, power_up_icon_y_pitch, 0x29c);

    y =  pY;
    for (i = 0; i < *pCount_icons && i < pMax_count_icons; i++) {
        the_icon = &pHeadup_icons[i];
        the_powerup = the_icon->powerup;
        if (the_powerup->icon == NULL) {
            continue;
        }
        y += gCurrent_graf_data->power_up_icon_y_pitch;
        if (the_icon->fizzle_stage < 4) {
            if (the_icon->fizzle_direction < 0) {
                the_icon->fizzle_stage = 3 - (pTime - the_icon->fizzle_start) / 150;
            } else {
                the_icon->fizzle_stage = (pTime - the_icon->fizzle_start) / 150;
            }
            if (the_icon->fizzle_stage > 4) {
                the_icon->fizzle_stage = 4;
            } else if (the_icon->fizzle_stage < 0) {
                memmove(the_icon, &the_icon[1], sizeof(tHeadup_icon) * (*pCount_icons - i - 1));
                *pCount_icons -= 1;
                continue;
            }
        }
        DrawSinglePowerupIcon(1, pTime, the_icon->powerup, the_icon, pX, y, 0);
    }
}

__inline void C2_HOOK_FASTCALL DrawKeyPowerups(tU32 pTime) {
    int x;
    int i;

    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tGraf_data, keyboard_powerup_x, 0x2a4);
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tGraf_data, keyboard_powerup_y, 0x2a8);
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tGraf_data, keyboard_powerup_bg_y, 0x2ac);
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tGraf_data, keyboard_powerup_bg_bottom, 0x2b0);
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tGraf_data, keyboard_powerup_bg_x, 0x2b4);
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tGraf_data, keyboard_powerup_spacing_x, 0x2b8);

    if (gInventory_cycling) {
        if (gInventory_selected >= 0) {
            if (GetTotalTime() >= gInventory_timeout) {
                gInventory_timeout = 0;
                gInventory_cycling = 0;
            }
            x = gCurrent_graf_data->keyboard_powerup_x;
            i = gInventory_selected;
            do {
                if (gInventory[i] >= 0) {
                    x = DrawSinglePowerupIcon(0, pTime, &gPowerup_array[gInventory[i]], NULL,
                        x, gCurrent_graf_data->keyboard_powerup_y, 1) + gCurrent_graf_data->keyboard_powerup_spacing_x;
                }
                i += 1;
                if (i >= CARPOCALYPSE2_ASIZE(gInventory)) {
                    i = 0;
                }
            } while (i != gInventory_selected);
            if (x >= 0) {
                DimRectangle(gBack_screen, gCurrent_graf_data->keyboard_powerup_bg_x, gCurrent_graf_data->keyboard_powerup_bg_y,
                    x, gCurrent_graf_data->keyboard_powerup_bg_bottom, 1);
                i = gInventory_selected;
                x = gCurrent_graf_data->keyboard_powerup_x;
                DRPixelmapText(gBack_screen,
                    gCurrent_graf_data->keyboard_powerup_bg_x + 1, gCurrent_graf_data->keyboard_powerup_bg_bottom - 9,
                    &gFonts[2],
                    gPowerup_array[gInventory[gInventory_selected]].message,
                    gCurrent_graf_data->keyboard_powerup_bg_x + 100);
                do {
                    if (gInventory[i] >= 0 && gInventory_cycling) {
                        x = DrawSinglePowerupIcon(1, pTime, &gPowerup_array[gInventory[i]], NULL,
                            x, gCurrent_graf_data->keyboard_powerup_y, 1) + gCurrent_graf_data->keyboard_powerup_spacing_x;
                    }
                    i += 1;
                    if (i >= CARPOCALYPSE2_ASIZE(gInventory)) {
                        i = 0;
                    }
                } while (i != gInventory_selected);
            }
        }
    } else {
        if (gInventory_selected >= 0) {
            DrawSinglePowerupIcon(1, pTime, gPowerup_array + gInventory[gInventory_selected], NULL,
            gCurrent_graf_data->keyboard_powerup_x, gCurrent_graf_data->keyboard_powerup_y, 0);
        }
    }
}

// FUNCTION: CARMA2_HW 0x004dad00
void C2_NAKED C2_HOOK_FASTCALL DrawPowerups(tU32 pTime) {
#ifndef CARPOCALYPSE2_MATCHING

    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tGraf_data, power_up_icon_x, 0x290);
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tGraf_data, power_up_icon_y, 0x298);

    DrawPowerupIcons2(pTime, gPickedup_powerups, &gNumber_of_icons, 5,
        gCurrent_graf_data->power_up_icon_x,
        gCurrent_graf_data->power_up_icon_y);
    DrawKeyPowerups(pTime);
#else
    __asm {
            sub esp, 0xc
            mov edx, dword ptr [gCurrent_graf_data]
            push ebx
            push ebp
            mov ebp, ecx
            mov eax, dword ptr [edx + 0x290]
            xor ecx, ecx
            mov dword ptr [esp + 0x10], eax
            mov eax, dword ptr [gNumber_of_icons]
            push esi
            mov esi, dword ptr [edx + 0x298]
            test eax, eax
            push edi
            mov dword ptr [esp + 0x10], ecx
            jle L004dadd8
            mov ebx, offset gPickedup_powerups+0x4
    L004dad37:
            cmp ecx, 5
            jge L004dadd8
            mov eax, dword ptr [ebx - 4]
            lea edi, [ebx - 4]
            cmp dword ptr [eax + 0x14], 0
            je L004dadbd
            mov eax, dword ptr [edx + 0x29c]
            add esi, eax
            mov eax, dword ptr [ebx]
            cmp eax, 4
            mov dword ptr [esp + 0x14], esi
            jge L004dada1
            mov eax, dword ptr [ebx + 4]
            mov edx, ebp
            test eax, eax
            mov eax, dword ptr [ebx + 8]
            jge L004dad82
            sub edx, eax
            mov eax, 0x1b4e81b5
            mul edx
            shr edx, 4
            mov eax, 3
            sub eax, edx
            mov dword ptr [ebx], eax
            jmp L004dad90
    L004dad82:
            sub edx, eax
            mov eax, 0x1b4e81b5
            mul edx
            shr edx, 4
            mov dword ptr [ebx], edx
    L004dad90:
            mov eax, dword ptr [ebx]
            cmp eax, 4
            jle L004dafb2
            mov dword ptr [ebx], 4
    L004dada1:
            mov ecx, dword ptr [esp + 0x18]
            mov edx, dword ptr [edi]
            push 0
            push esi
            push ecx
            push edi
            push edx
            mov edx, ebp
            mov ecx, 1
            call DrawSinglePowerupIcon
    L004dadb9:
            mov ecx, dword ptr [esp + 0x10]
    L004dadbd:
            mov eax, dword ptr [gNumber_of_icons]
            mov edx, dword ptr [gCurrent_graf_data]
            inc ecx
            add ebx, 0x14
            cmp ecx, eax
            mov dword ptr [esp + 0x10], ecx
            jl L004dad37
    L004dadd8:
            mov eax, dword ptr [gInventory_cycling]
            xor edi, edi
            cmp eax, edi
            mov eax, dword ptr [gInventory_selected]
            je L004daf6c
            cmp eax, edi
            jl L004dafaa
            call GetTotalTime
            cmp eax, dword ptr [gInventory_timeout]
            jb L004dae0d
            mov dword ptr [gInventory_timeout], edi
            mov dword ptr [gInventory_cycling], edi
    L004dae0d:
            mov eax, dword ptr [gCurrent_graf_data]
            mov esi, dword ptr [gInventory_selected]
            mov edx, dword ptr [eax + 0x2a4]
    L004dae1e:
            mov ecx, dword ptr [esi*4 + gInventory]
            cmp ecx, edi
            jl L004dae65
            mov eax, dword ptr [eax + 0x2a8]
            push 1
            push eax
            lea eax, [ecx*8]
            sub eax, ecx
            push edx
            push edi
            lea edx, [eax + eax*2]
            lea eax, [ecx + edx*2]
            mov ecx, dword ptr [gPowerup_array]
            lea edx, [ecx + eax*4]
            xor ecx, ecx
            push edx
            mov edx, ebp
            call DrawSinglePowerupIcon
            mov ecx, eax
            mov eax, dword ptr [gCurrent_graf_data]
            mov edx, dword ptr [eax + 0x2b8]
            add edx, ecx
    L004dae65:
            inc esi
            cmp esi, 0x14
            jl L004dae6d
            xor esi, esi
    L004dae6d:
            cmp esi, dword ptr [gInventory_selected]
            jne L004dae1e
            cmp edx, edi
            jl L004dafaa
            mov ecx, dword ptr [eax + 0x2b0]
            push 1
            push ecx
            mov ecx, dword ptr [gBack_screen]
            push edx
            mov edx, dword ptr [eax + 0x2ac]
            mov eax, dword ptr [eax + 0x2b4]
            push edx
            push eax
            push ecx
            call DimRectangle
            mov edx, dword ptr [gCurrent_graf_data]
            mov ebx, dword ptr [gInventory_selected]
            add esp, 0x18
            mov edi, dword ptr [edx + 0x2b4]
            mov ecx, dword ptr [ebx*4 + gInventory]
            mov esi, dword ptr [edx + 0x2a4]
            mov edx, dword ptr [edx + 0x2b0]
            lea eax, [edi + 0x64]
            sub edx, 9
            push eax
            mov eax, ecx
            shl eax, 3
            sub eax, ecx
            lea eax, [eax + eax*2]
            lea ecx, [ecx + eax*2]
            mov eax, dword ptr [gPowerup_array]
            lea ecx, [eax + ecx*4 + 0x50]
            push ecx
            mov ecx, dword ptr [gBack_screen]
            push offset gFonts+0x740
            push edx
            lea edx, [edi + 1]
            call DRPixelmapText
            mov edx, dword ptr [gCurrent_graf_data]
    L004daf01:
            mov ecx, dword ptr [ebx*4 + gInventory]
            test ecx, ecx
            jl L004daf54
            lea eax, [ecx*8]
            sub eax, ecx
            lea eax, [eax + eax*2]
            lea ecx, [ecx + eax*2]
            mov eax, dword ptr [gPowerup_array]
            lea eax, [eax + ecx*4]
            mov ecx, dword ptr [gInventory_cycling]
            test ecx, ecx
            je L004daf54
            mov ecx, dword ptr [edx + 0x2a8]
            push 1
            push ecx
            push esi
            push 0
            push eax
            mov edx, ebp
            mov ecx, 1
            call DrawSinglePowerupIcon
            mov edx, dword ptr [gCurrent_graf_data]
            add eax, dword ptr [edx + 0x2b8]
            mov esi, eax
    L004daf54:
            inc ebx
            cmp ebx, 0x14
            jl L004daf5c
            xor ebx, ebx
    L004daf5c:
            cmp ebx, dword ptr [gInventory_selected]
            jne L004daf01
            pop edi
            pop esi
            pop ebp
            pop ebx
            add esp, 0xc
            ret
    L004daf6c:
            cmp eax, edi
            jl L004dafaa
            mov ecx, dword ptr [edx + 0x2a8]
            push edi
            push ecx
            mov ecx, dword ptr [eax*4 + gInventory]
            mov edx, dword ptr [edx + 0x2a4]
            mov eax, ecx
            shl eax, 3
            sub eax, ecx
            push edx
            mov edx, dword ptr [gPowerup_array]
            push edi
            lea eax, [eax + eax*2]
            lea ecx, [ecx + eax*2]
            lea eax, [edx + ecx*4]
            mov edx, ebp
            push eax
            mov ecx, 1
            call DrawSinglePowerupIcon
    L004dafaa:
            pop edi
            pop esi
            pop ebp
            pop ebx
            add esp, 0xc
            ret
    L004dafb2:
            test eax, eax
            jge L004dada1
            mov eax, dword ptr [gNumber_of_icons]
            lea esi, [ebx + 0x10]
            sub eax, ecx
            dec eax
            lea ecx, [eax + eax*4]
            shl ecx, 2
            mov eax, ecx
            shr ecx, 2
            rep movsd
            mov ecx, eax
            mov eax, dword ptr [gNumber_of_icons]
            and ecx, 3
            dec eax
            rep movsb
            mov esi, dword ptr [esp + 0x14]
            mov dword ptr [gNumber_of_icons], eax
            jmp L004dadb9
            nop
            nop
            nop
    }
#endif
}

// FUNCTION: CARMA2_HW 0x004e0750
void C2_NAKED C2_HOOK_FASTCALL MayQueuePowerupRespawn(int pPowerup_index, br_actor* actor) {
#ifndef CARPOCALYPSE2_MATCHING
    int i;

    if (actor != NULL && gRace_powerup_respawn_bools[pPowerup_index] && (gNet_mode == eNet_mode_none || gCurrent_net_game->options.powerup_respawn)) {
for (i = 0; i < (int)CARPOCALYPSE2_ASIZE(gRespawn_powerups); i++) {
            if (gRespawn_powerups[i].actor == NULL) {
                gRespawn_powerups[i].actor = actor;
                gRespawn_powerups[i].index = pPowerup_index;;
                gRespawn_powerups[i].respawn_time = GetTotalTime() + gPickup_respawn_max_extra_time_ms / 2 + gPickup_respawn_min_time_ms;
                break;
            }
        }
    }
#else
    __asm {
            push esi
            test edx, edx
            je L004e07c1
            mov eax, dword ptr [gRace_powerup_respawn_bools]
            cmp byte ptr [ecx + eax], 0
            je L004e07c1
            mov eax, dword ptr [gNet_mode]
            test eax, eax
            je L004e0775
            mov eax, dword ptr [gCurrent_net_game]
            mov esi, dword ptr [eax + 0x50]
            test esi, esi
            je L004e07c1
    L004e0775:
            xor esi, esi
            mov eax, offset gRespawn_powerups
    L004e077c:
            cmp dword ptr [eax], 0
            je L004e078e
            add eax, 0xc
            inc esi
            cmp eax, offset gMutant_tail_first_collision_info
            jl L004e077c
            pop esi
            ret
    L004e078e:
            lea esi, [esi + esi*2]
            shl esi, 2
            mov dword ptr [esi + gRespawn_powerups], edx
            mov dword ptr [esi + gRespawn_powerups+0x4], ecx
            call GetTotalTime
            mov ecx, eax
            mov eax, dword ptr [gPickup_respawn_max_extra_time_ms]
            cdq
            sub eax, edx
            mov edx, dword ptr [gPickup_respawn_min_time_ms]
            sar eax, 1
            add edx, ecx
            add eax, edx
            mov dword ptr [esi + gRespawn_powerups+0x8], eax
    L004e07c1:
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
            nop
            nop
    }
#endif
}
// FUNCTION: CARMA2_HW 0x004d8d10
char* C2_HOOK_FASTCALL GetPowerupMessage(int pIndex) {
    return gPowerup_array[pIndex].message;
}

// FUNCTION: CARMA2_HW 0x004dd3f0
void C2_HOOK_FASTCALL ConfigureShitMine(tShit_mine* pMine, tPowerup* pPowerup, tCar_spec* pCar) {
    pMine->flags = 5;
    pMine->flags = 5 + (pPowerup->integer_params[0] != 0 ? 0x40 : 0);
    pMine->next_think_time = GetTotalTime() - (tU32)(pPowerup->float_params[0] * gDOUBLE_0058ac60);
    pMine->field3_0xc = pPowerup->float_params[1];
    pMine->initial_y_speed_factor = pPowerup->float_params[2];
    pMine->initial_z_omega_factor = pPowerup->float_params[3];
    pMine->initial_x_omega_factor = pPowerup->float_params[4];
    pMine->field_0x1c = pPowerup->float_params[5];
    pMine->field_0x20 = pPowerup->float_params[6];
    pMine->field_0x24 = pPowerup->float_params[7];
    pMine->collision_info->field_0xed = 1;
    pMine->collision_info->disable_move_rotate = 0;
    pMine->collision_info->flags &= ~0x40;
    if (pMine->collision_info->actor->parent == NULL) {
        BrActorAdd(gNon_track_actor, pMine->collision_info->actor);
    }
    pMine->collision_info->actor->render_style = BR_RSTYLE_FACES;
    pMine->field_0x34 = pPowerup - gPowerup_array;
    PHILAddActiveObject(pMine->collision_info, NULL, NULL, NULL);
    PHILSetObjectProperty(pMine->collision_info, 0, 1.0);
    PHILSetObjectProperty(pMine->collision_info, 7, 1);
    PHILSetObjectProperty(pMine->collision_info, 6, 1);
    pMine->field_0x30 = pCar;
    pMine->collision_info->last_special_volume = pCar->collision_info->last_special_volume;
    pMine->collision_info->auto_special_volume = pCar->collision_info->auto_special_volume;
    pMine->collision_info->water_d = pCar->collision_info->water_d;
    pMine->collision_info->water_normal.v[0] = pCar->collision_info->water_normal.v[0];
    pMine->collision_info->water_normal.v[1] = pCar->collision_info->water_normal.v[1];
    pMine->collision_info->water_normal.v[2] = pCar->collision_info->water_normal.v[2];
}

// FUNCTION: CARMA2_HW 0x004ddc10
void C2_HOOK_FASTCALL FUN_004ddc10(unsigned int* pFlags) {
    *pFlags |= 0x20;
}

// FUNCTION: CARMA2_HW 0x004de5a0
int C2_HOOK_FASTCALL ReceivedRepulseOpponents(tNet_message_chunk_powerup* pMessage) {
    return RepulseOpponents2(&gPowerup_array[pMessage->powerup_index], NetCarFromPlayerID(pMessage->player_id), 0);
}

// FUNCTION: CARMA2_HW 0x004da910
void C2_NAKED C2_HOOK_FASTCALL RemoveRepulseLinksForCar(tCar_spec* pCar) {
#ifndef CARPOCALYPSE2_MATCHING
    int i;
    tRepulse_link* link;

    for (i = 0; i < (int)CARPOCALYPSE2_ASIZE(gRepulse_links); i++) {
        link = &gRepulse_links[i];
        if (link->time != 0
                && (link->car == pCar || link->field_0x24 == (undefined4)(uintptr_t)pCar)) {
            link->time = 0;

            if (link->actor->parent == gNon_track_actor) {
                BrActorRemove(link->actor);
            }
        }
    }
#else
    __asm {
            push esi
            push edi
            mov edi, ecx
            mov esi, offset gRepulse_links
    L004da919:
            cmp dword ptr [esi], 0
            je L004da947
            cmp dword ptr [esi + 0x20], edi
            je L004da928
            cmp dword ptr [esi + 0x24], edi
            jne L004da947
    L004da928:
            mov edx, dword ptr [gNon_track_actor]
            mov dword ptr [esi], 0
            mov eax, dword ptr [esi + 0xc]
            mov ecx, dword ptr [eax + 0xc]
            cmp ecx, edx
            jne L004da947
            push eax
            call BrActorRemove
            add esp, 4
    L004da947:
            add esi, 0x28
            cmp esi, offset gRace_powerup_respawn_bools
            jl L004da919
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
            nop
            nop
    }
#endif
}

// FUNCTION: CARMA2_HW 0x004ddde0
void C2_NAKED C2_HOOK_FASTCALL FUN_004ddde0(tCar_spec* pCar) {
#ifndef CARPOCALYPSE2_MATCHING
    tShit_mine* mine;

    for (mine = gShit_mines; (int)&mine->field_0x30 < (int)&gShit_mines[CARPOCALYPSE2_ASIZE(gShit_mines)].field_0x30; mine++) {
        if ((mine->flags & 1) != 0) {
            if (mine->car == pCar) {
                mine->car = NULL;
            }
            if (mine->field_0x30 == pCar) {
                mine->field_0x30 = NULL;
                MineExplode(mine);
            }
        }
    }
#else
    __asm {
            push ebx
            push esi
            push edi
            mov edi, ecx
            mov esi, offset gShit_mines+0x30
            xor ebx, ebx
    L004dddec:
            mov al, byte ptr [esi - 0x30]
            lea ecx, [esi - 0x30]
            test al, 1
            je L004dde09
            cmp dword ptr [esi - 8], edi
            jne L004dddfe
            mov dword ptr [esi - 8], ebx
    L004dddfe:
            cmp dword ptr [esi], edi
            jne L004dde09
            mov dword ptr [esi], ebx
            call MineExplode
    L004dde09:
            add esi, 0x38
            cmp esi, offset gPickedup_powerups+0x10
            jl L004dddec
            pop edi
            pop esi
            pop ebx
            ret
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

// FUNCTION: CARMA2_HW 0x004dd590
void C2_HOOK_FASTCALL InitializeMineMotion(tShit_mine* pMine, tCar_spec* pCar) {
    br_matrix34 car_transform;
    br_vector3 impulse;

    if (pMine->flags & 0x40) {
        BrMatrix34Translate(&pMine->collision_info->actor->t.t.mat,
            0.f,
            (pCar->collision_info->bb1.max.v[1] - pCar->collision_info->bb1.min.v[1]) * gFLOAT_0058ac00,
            pCar->collision_info->bb1.min.v[2] - gDOUBLE_0058ac68);
    } else {
        BrMatrix34Translate(&pMine->collision_info->actor->t.t.mat,
            0.f,
            (pCar->collision_info->bb1.max.v[1] - pCar->collision_info->bb1.min.v[1]) * gFLOAT_0058ac00,
            pCar->collision_info->bb1.max.v[2] - gDOUBLE_0058ac70);
    }

    BrMatrix34Copy(&car_transform, &pCar->collision_info->transform_matrix);
    BrMatrix34Post(&pMine->collision_info->actor->t.t.mat, &car_transform);
    BrMatrix34Copy(&pMine->collision_info->transform_matrix, &pMine->collision_info->actor->t.t.mat);

    {
        float vy;
        float vz;

        vy = pMine->field_0x20;
        vz = pMine->field_0x24;
        impulse.v[0] = 0.f;
        impulse.v[1] = vy;
        impulse.v[2] = vz;
    }
    BrMatrix34ApplyV(&pMine->collision_info->v, &impulse, &car_transform);

    pMine->collision_info->v.v[0] = pCar->collision_info->v.v[0] + pMine->collision_info->v.v[0];
    pMine->collision_info->v.v[1] = pCar->collision_info->v.v[1] + pMine->collision_info->v.v[1];
    pMine->collision_info->v.v[2] = pCar->collision_info->v.v[2] + pMine->collision_info->v.v[2];
    if (pMine->flags & 0x40) {
        pMine->collision_info->v.v[0] = pCar->collision_info->v.v[0] + pMine->collision_info->v.v[0];
        pMine->collision_info->v.v[1] = pCar->collision_info->v.v[1] + pMine->collision_info->v.v[1];
        pMine->collision_info->v.v[2] = pCar->collision_info->v.v[2] + pMine->collision_info->v.v[2];
    }
    pMine->collision_info->omega.v[0] = 0.f;
    pMine->collision_info->omega.v[1] = 0.f;
    pMine->collision_info->omega.v[2] = 0.f;
}

// FUNCTION: CARMA2_HW 0x004dd210
void C2_NAKED C2_HOOK_FASTCALL ReceiveMineUpdate(tNet_message_chunk_mine_explode* pChunk) {
#ifndef CARPOCALYPSE2_MATCHING
    int mine_offset;
    tShit_mine* mine;
    tU32 mine_index;
    int player_index;
    tU32 hierarchy_size;
    tCar_spec* car;
    tPowerup* powerup;

    mine_index = pChunk->mine_index;
    player_index = pChunk->player_index;
    mine_offset = mine_index * sizeof(tShit_mine);
    mine = (tShit_mine*)((tU8*)gShit_mines + mine_offset);
    powerup = &gPowerup_array[pChunk->powerup_index];
    car = gNet_players[0].car;
    if (player_index >= 0 && player_index < gNumber_of_net_players) {
        car = gNet_players[player_index].car;
    }
    if (pChunk->field_0x8 == -1) {
        if (mine->field_0x35 != pChunk->owner_index
                && ((mine->flags & 0x2) || !(mine->flags & 0x1))) {
            return;
        }
        if (!(mine->flags & 0x1)) {
            ConfigureShitMine(mine, powerup, car);
        }
        SelectCrushNetwork((undefined4*)pChunk->object_hierarchy, mine->collision_info, 2);
        MineExplode(mine);
        return;
    }
    if (mine->flags & 0x1) {
        if (mine->field_0x35 == pChunk->owner_index) {
            goto set_owner;
        }
    }
    if (!(mine->flags & 0x1)) {
        ConfigureShitMine(mine, powerup, car);
    }
    if (player_index >= 0 && player_index < gNumber_of_net_players) {
        InitializeMineMotion(mine, mine->field_0x30);
    } else {
        BrMatrix34Translate(&mine->collision_info->transform_matrix, 1000.f, 1000.f, 1000.f);
        BrMatrix34Copy(&mine->collision_info->actor->t.t.mat,
            &mine->collision_info->transform_matrix);
    }
    if (mine->flags & 0x40) {
        DRS3StartSound3D(gXXX_outlet, 0x151d,
            &mine->collision_info->actor->t.t.translate.t, &gZero_v__car,
            1, 0xff, 0x10000, 0x10000);
    } else {
        DRS3StartSound3D(gXXX_outlet, 0x151c,
            &mine->collision_info->actor->t.t.translate.t, &gZero_v__car,
            1, 0xff, 0x10000, 0x10000);
    }
    if (mine->field_0x30 != NULL && mine->field_0x30->driver == eDriver_local_human) {
        PlayExclusiveFFBEffect("Mine_Shit", 0);
    }
set_owner:
    mine->field_0x35 = pChunk->owner_index;
    mine->collision_info->message_time = pChunk->field_0x8;
    mine->collision_info->field_0x261 = pChunk->header.type;
    mine->collision_info->disable_move_rotate = 0;
    hierarchy_size = pChunk->header.size - 0xc;
    if (hierarchy_size > 0x48) {
        hierarchy_size = 0x48;
    }
    memcpy(&mine->collision_info->field_0x294, pChunk->object_hierarchy, hierarchy_size);
#else
    __asm {
            push ebx
            push ebp
            push esi
            mov esi, ecx
            xor eax, eax
            push edi
            mov al, byte ptr [esi + 2]
            xor ebx, ebx
            mov ecx, eax
            mov bl, byte ptr [esi + 5]
            shl ecx, 3
            sub ecx, eax
            lea edi, [ecx*8 + gShit_mines]
            xor ecx, ecx
            mov cl, byte ptr [esi + 4]
            mov eax, ecx
            shl eax, 3
            sub eax, ecx
            test ebx, ebx
            lea edx, [eax + eax*2]
            lea eax, [ecx + edx*2]
            mov ecx, dword ptr [gPowerup_array]
            lea ebp, [ecx + eax*4]
            jl L004dd268
            cmp ebx, dword ptr [gNumber_of_net_players]
            jge L004dd268
            lea eax, [ebx + ebx*2]
            lea edx, [eax + eax*8]
            shl edx, 1
            sub edx, ebx
            mov ecx, dword ptr [edx*4 + gNet_players+0xc8]
            jmp L004dd26e
    L004dd268:
            mov ecx, dword ptr [gNet_players+0xc8]
    L004dd26e:
            cmp dword ptr [esi + 8], -1
            jne L004dd2b8
            mov al, byte ptr [edi + 0x35]
            mov dl, byte ptr [esi + 3]
            cmp al, dl
            je L004dd290
            mov eax, dword ptr [edi]
            test al, 2
            jne L004dd3dc
            test al, 1
            je L004dd3dc
    L004dd290:
            test byte ptr [edi], 1
            jne L004dd29f
            push ecx
            mov edx, ebp
            mov ecx, edi
            call ConfigureShitMine
    L004dd29f:
            mov edx, dword ptr [edi + 8]
            push 2
            lea ecx, [esi + 0xc]
            call SelectCrushNetwork
            mov ecx, edi
            call MineExplode
            pop edi
            pop esi
            pop ebp
            pop ebx
            ret
    L004dd2b8:
            mov eax, dword ptr [edi]
            and eax, 1
            je L004dd2cb
            mov dl, byte ptr [edi + 0x35]
            cmp dl, byte ptr [esi + 3]
            je L004dd38d
    L004dd2cb:
            test eax, eax
            jne L004dd2d9
            push ecx
            mov edx, ebp
            mov ecx, edi
            call ConfigureShitMine
    L004dd2d9:
            test ebx, ebx
            jl L004dd2f1
            cmp ebx, dword ptr [gNumber_of_net_players]
            jge L004dd2f1
            mov edx, dword ptr [edi + 0x30]
            mov ecx, edi
            call InitializeMineMotion
            jmp L004dd329
    L004dd2f1:
            mov eax, dword ptr [edi + 8]
            push 0x447a0000
            push 0x447a0000
            add eax, 0x8c
            push 0x447a0000
            push eax
            call BrMatrix34Translate
            mov eax, dword ptr [edi + 8]
            add esp, 0x10
            mov edx, dword ptr [eax]
            lea ecx, [eax + 0x8c]
            add edx, 0x2c
            push ecx
            push edx
            call BrMatrix34Copy
            add esp, 8
    L004dd329:
            mov al, byte ptr [edi]
            push 0x10000
            push 0x10000
            push 0xff
            test al, 0x40
            push 1
            push offset gZero_v__car
            je L004dd355
            mov eax, dword ptr [edi + 8]
            mov edx, 0x151d
            mov ecx, dword ptr [eax]
            add ecx, 0x50
            push ecx
            jmp L004dd363
    L004dd355:
            mov edx, dword ptr [edi + 8]
            mov eax, dword ptr [edx]
            mov edx, 0x151c
            add eax, 0x50
            push eax
    L004dd363:
            mov ecx, dword ptr [gXXX_outlet]
            call DRS3StartSound3D
            mov eax, dword ptr [edi + 0x30]
            test eax, eax
            je L004dd387
            cmp dword ptr [eax + 0xc], 8
            jne L004dd387
            xor edx, edx
            mov ecx, offset gMass_mine+0xb0
            call PlayExclusiveFFBEffect
    L004dd387:
            mov cl, byte ptr [esi + 3]
            mov byte ptr [edi + 0x35], cl
    L004dd38d:
            mov edx, dword ptr [edi + 8]
            mov eax, dword ptr [esi + 8]
            mov dword ptr [edx + 0x268], eax
            mov ecx, dword ptr [edi + 8]
            mov dl, byte ptr [esi + 1]
            mov byte ptr [ecx + 0x261], dl
            mov eax, dword ptr [edi + 8]
            mov byte ptr [eax + 0xec], 0
            xor eax, eax
            mov al, byte ptr [esi]
            add eax, -0xc
            cmp eax, 0x48
            jbe L004dd3c0
            mov eax, 0x48
    L004dd3c0:
            mov edi, dword ptr [edi + 8]
            mov ecx, eax
            mov edx, ecx
            add esi, 0xc
            add edi, 0x294
            shr ecx, 2
            rep movsd
            mov ecx, edx
            and ecx, 3
            rep movsb
    L004dd3dc:
            pop edi
            pop esi
            pop ebp
            pop ebx
            ret
    }
#endif
}

// FUNCTION: CARMA2_HW 0x004df070
void C2_NAKED C2_HOOK_FASTCALL LoseCarPowerups(tCar_spec* pCar) {
#ifndef CARPOCALYPSE2_MATCHING
    if (pCar == NULL || pCar->driver != eDriver_local_human) {
        return;
    }
    C2_HOOK_BUG_ON(sizeof(tPowerup) != 0xac);
    {
        int i;

        i = 0;
        if (gNumber_of_powerups > 0) {
            int j;
            int powerup_offset;
            int* car_powerup;
            tPowerup* powerup;
            tNet_message* message;

            powerup_offset = 0;
            car_powerup = pCar->powerups;
            do {
                if (*car_powerup != 0) {
                    powerup = (tPowerup*)((tU8*)gPowerup_array + powerup_offset);
                    if (powerup->net_type & eNet_powerup_off_before_snapweld) {
                        if (powerup->got_time != 0 && powerup->lose_proc != NULL) {
                            powerup->lose_proc(powerup, powerup->car);
                        }
                        if (powerup->net_type & 0x80) {
                            ARPowerupLose(powerup, powerup->car);
                        }
                        if (gNet_mode != eNet_mode_none) {
                            message = NetBuildGuaranteedMessage(0x16, 0);
                            *(int*)((tU8*)message + 0x28) = 2;
                            *(tPlayer_ID*)((tU8*)message + 0x1c) = gLocal_net_ID;
                            *(int*)((tU8*)message + 0x20) = powerup - gPowerup_array;
                            NetGuaranteedSendMessageToAllPlayers(gCurrent_net_game, message, NULL);
                        }
                        gProgram_state.current_car.powerups[powerup - gPowerup_array] = 0;
                        if (powerup->initial_value == 0) {
                            for (j = 0; j < gNumber_of_icons; j++) {
                                if (powerup == gPickedup_powerups[j].powerup) {
                                    gPickedup_powerups[j].fizzle_stage = 3;
                                    gPickedup_powerups[j].fizzle_direction = -1;
                                    gPickedup_powerups[j].fizzle_start = GetTotalTime();
                                    break;
                                }
                            }
                        }
                        powerup->got_time = 0;
                    }
                }
                i++;
                car_powerup++;
                powerup_offset += sizeof(tPowerup);
            } while (i < gNumber_of_powerups);
        }
    }
#else
    __asm {
            sub esp, 8
            push ebp
            xor ebp, ebp
            cmp ecx, ebp
            je L004df1d1
            cmp dword ptr [ecx + 0xc], 8
            jne L004df1d1
            mov eax, dword ptr [gNumber_of_powerups]
            mov dword ptr [esp + 4], ebp
            cmp eax, ebp
            jle L004df1d1
            push edi
            push esi
            push ebx
            xor ebx, ebx
            add ecx, 0x1710
            mov dword ptr [esp + 0x14], ecx
    L004df0a8:
            cmp dword ptr [ecx], ebp
            je L004df1a6
            mov eax, dword ptr [gPowerup_array]
            lea esi, [eax + ebx]
            mov al, byte ptr [eax + ebx + 0x38]
            test al, 4
            je L004df1a6
            cmp dword ptr [esi + 4], ebp
            je L004df0d7
            mov eax, dword ptr [esi + 0x40]
            cmp eax, ebp
            je L004df0d7
            mov edx, dword ptr [esi + 0x4c]
            mov ecx, esi
            call eax
    L004df0d7:
            test byte ptr [esi + 0x38], 0x80
            je L004df0e7
            mov edx, dword ptr [esi + 0x4c]
            mov ecx, esi
            call ARPowerupLose
    L004df0e7:
            cmp dword ptr [gNet_mode], ebp
            je L004df136
            xor edx, edx
            mov cl, 0x16
            call NetBuildGuaranteedMessage
            mov ecx, eax
            mov eax, 0x2fa0be83
            push ebp
            mov dword ptr [ecx + 0x28], 2
            mov edx, dword ptr [gLocal_net_ID]
            mov dword ptr [ecx + 0x1c], edx
            mov edi, dword ptr [gPowerup_array]
            mov edx, esi
            sub edx, edi
            imul edx
            sar edx, 5
            mov eax, edx
            shr eax, 0x1f
            add edx, eax
            mov dword ptr [ecx + 0x20], edx
            mov edx, ecx
            mov ecx, dword ptr [gCurrent_net_game]
            call NetGuaranteedSendMessageToAllPlayers
    L004df136:
            mov edx, dword ptr [gPowerup_array]
            mov ecx, esi
            sub ecx, edx
            mov eax, 0x2fa0be83
            imul ecx
            sar edx, 5
            mov ecx, edx
            shr ecx, 0x1f
            add edx, ecx
            mov dword ptr [edx*4 + gProgram_state+0x17bc], ebp
            mov eax, dword ptr [esi + 0x34]
            cmp eax, ebp
            jne L004df1a3
            mov edx, dword ptr [gNumber_of_icons]
            xor eax, eax
            cmp edx, ebp
            jle L004df1a3
            mov ecx, offset gPickedup_powerups
    L004df170:
            cmp esi, dword ptr [ecx]
            je L004df17e
            inc eax
            add ecx, 0x14
            cmp eax, edx
            jl L004df170
            jmp L004df1a3
    L004df17e:
            lea edi, [eax + eax*4]
            shl edi, 2
            mov dword ptr [edi + gPickedup_powerups+0x4], 3
            mov dword ptr [edi + gPickedup_powerups+0x8], 0xffffffff
            call GetTotalTime
            mov dword ptr [edi + gPickedup_powerups+0xc], eax
    L004df1a3:
            mov dword ptr [esi + 4], ebp
    L004df1a6:
            mov eax, dword ptr [esp + 0x10]
            mov ecx, dword ptr [esp + 0x14]
            mov edx, dword ptr [gNumber_of_powerups]
            inc eax
            add ecx, 4
            add ebx, 0xac
            cmp eax, edx
            mov dword ptr [esp + 0x10], eax
            mov dword ptr [esp + 0x14], ecx
            jl L004df0a8
            pop ebx
            pop esi
            pop edi
    L004df1d1:
            pop ebp
            add esp, 8
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
    }
#endif
}

// FUNCTION: CARMA2_HW 0x004da870
void C2_NAKED C2_HOOK_FASTCALL AddRepulseLink(tU32 pTime, tCar_spec* pCar, undefined4 pField_0x24, br_vector3* pOrigin, br_vector3* pVictim, float pField_0x18, float pField_0x1c) {
#ifndef CARPOCALYPSE2_MATCHING
    tRepulse_link* link;
    tU32 link_time;
    tU32 min_time;
    int index;
    int best;
    int i;

    min_time = 0x7fffffff;
    index = -1;
    i = 0;
    link = &gRepulse_links[0];
    while ((int)link < (int)&gRepulse_links[6]) {
        link_time = link->time;
        if (link_time == 0) {
            index = i;
            break;
        }
        if (link_time < min_time) {
            min_time = link_time;
            best = i;
        }
        link++;
        i++;
    }
    if (index < 0) {
        index = best;
    }
    gRepulse_links[index].pos_origin = pOrigin;
    gRepulse_links[index].pos_victim = pVictim;
    gRepulse_links[index].time = pTime;
    gRepulse_links[index].field_0x10 = 0xffffffff;
    gRepulse_links[index].field_0x18 = pField_0x18;
    gRepulse_links[index].field_0x1c = pField_0x1c;
    gRepulse_links[index].car = pCar;
    gRepulse_links[index].field_0x24 = pField_0x24;
#else
    __asm {
            push ecx
            push ebx
            push ebp
            mov ebp, dword ptr [esp + 0x14]
            mov dword ptr [esp + 8], ecx
            push esi
            push edi
            or ecx, 0xffffffff
            mov ebx, 0x7fffffff
            xor edi, edi
            mov esi, offset gRepulse_links
    L004da88c:
            mov eax, dword ptr [esi]
            test eax, eax
            je L004da8a8
            cmp eax, ebx
            jae L004da89a
            mov ebx, eax
            mov ebp, edi
    L004da89a:
            add esi, 0x28
            inc edi
            cmp esi, offset gRace_powerup_respawn_bools
            jl L004da88c
            jmp L004da8aa
    L004da8a8:
            mov ecx, edi
    L004da8aa:
            test ecx, ecx
            jge L004da8b0
            mov ecx, ebp
    L004da8b0:
            lea eax, [ecx + ecx*4]
            mov ecx, dword ptr [esp + 0x1c]
            shl eax, 3
            pop edi
            pop esi
            mov dword ptr [eax + gRepulse_links+0x4], ecx
            mov ecx, dword ptr [esp + 0x18]
            mov dword ptr [eax + gRepulse_links+0x8], ecx
            mov ecx, dword ptr [esp + 8]
            mov dword ptr [eax + gRepulse_links], ecx
            mov ecx, dword ptr [esp + 0x1c]
            mov dword ptr [eax + gRepulse_links+0x10], 0xffffffff
            mov dword ptr [eax + gRepulse_links+0x18], ecx
            mov ecx, dword ptr [esp + 0x20]
            pop ebp
            mov dword ptr [eax + gRepulse_links+0x1c], ecx
            mov dword ptr [eax + gRepulse_links+0x20], edx
            mov edx, dword ptr [esp + 0xc]
            pop ebx
            mov dword ptr [eax + gRepulse_links+0x24], edx
            pop ecx
            ret 0x14
            nop
            nop
            nop
            nop
            nop
            nop
    }
#endif
}

// FUNCTION: CARMA2_HW 0x004dece0
void C2_HOOK_FASTCALL ReceivedPowerup(tNet_message_chunk_powerup* pMessage) {
    tPowerup* powerup;
    tCar_spec* car;

    if (!gProgram_state.racing || pMessage->powerup_index >= gNumber_of_powerups || pMessage->powerup_index < 0) {
        return;
    }
    powerup = &gPowerup_array[pMessage->powerup_index];
    if (pMessage->field_0x10 != 0 && pMessage->field_0x10 != 1) {
        if ((powerup->net_type & 1) == 0 && powerup->lose_proc != NULL) {
            car = NetCarFromPlayerID(pMessage->player_id);
            if (car != NULL) {
                powerup->lose_proc(powerup, car);
                car->powerups[pMessage->powerup_index] = 0;
            }
        }
        return;
    }
    if ((powerup->net_type & 1) != 0) {
        GotPowerupX(&gProgram_state.current_car, pMessage->powerup_index, 0, pMessage->field_0x10 == 0, pMessage->time_remaining);
    } else if (pMessage->player_id != gLocal_net_ID && powerup->got_proc != NULL) {
        car = NetCarFromPlayerID(pMessage->player_id);
        if (car != NULL) {
            powerup->got_proc(powerup, car);
            switch (powerup->type) {
            case ePowerup_instantaneous:
                break;
            case ePowerup_timed:
                car->powerups[pMessage->powerup_index] = GetTotalTime() + pMessage->time_remaining;
                break;
            case ePowerup_whole_race:
                car->powerups[pMessage->powerup_index] = -1;
                return;
            }
        }
    }
}

// FUNCTION: CARMA2_HW 0x004d9d00
void C2_NAKED C2_HOOK_FASTCALL FUN_004d9d00(tPhysics_object* pObject, int pMode) {
#ifndef CARPOCALYPSE2_MATCHING
    int previous_dismemberfest;

    if ((pObject->field_0x1a0 & 0xf0000) != 0) {
        if (!IsItReallyOKThatWeDontMakeAnyEffortToProtectAnySadFuckersOutThereThatDontWishToSeeInnocentPeopleBlownToBitsByHighExplosiveMinesAndShells()) {
            return;
        }
        if ((((tPed_character_instance*)pObject->owner)->ped->character->field_0x14 & 4) == 0 && gCrush_deferred == 0) {
            SetThisPedPhysicing(((tPed_character_instance*)pObject->owner)->ped);
        }
        if (pMode != 0xe) {
            pObject->v.v[0] = 0.0f;
            pObject->v.v[1] = 0.0f;
            pObject->v.v[2] = 0.0f;
        }
        pObject->v.v[1] = (float)((double)FRandomBetween(0.7f, 1.0f) / ((double)pObject->M * gDouble_0058abe0) + pObject->v.v[1]);
        pObject->omega.v[2] = (float)((double)FRandomPosNeg(0.5f) * gDouble_0058abe8 / pObject->M + pObject->omega.v[2]);
        pObject->disable_move_rotate = 0;
        pObject->omega.v[0] = (float)((double)FRandomPosNeg(0.5f) * gDouble_0058abe8 / pObject->M + pObject->omega.v[0]);
        previous_dismemberfest = gPed_dismemberfest;
        SetPedFall((tPedestrian*)(gPed_dismemberfest = 1, ((tPed_character_instance*)pObject->owner)->ped),
                500, 0x61, 0x61, gProgram_state.current_car.collision_info);
        gPed_dismemberfest = previous_dismemberfest;
        return;
    }

    if (pObject == NULL || pObject->flags_0x238 != 0x10) {
        pObject->v.v[1] = (float)((double)FRandomBetween(1.0f, 2.0f) / ((double)pObject->M * gDouble_0058abe0) + pObject->v.v[1]);
        pObject->omega.v[2] = (float)((double)FRandomPosNeg(0.5f) * gDouble_0058abe8 / pObject->M + pObject->omega.v[2]);
        pObject->disable_move_rotate = 0;
        pObject->omega.v[0] = (float)((double)FRandomPosNeg(0.5f) * gDouble_0058abe8 / pObject->M + pObject->omega.v[0]);
    } else {
        pObject->v.v[1] = (float)((double)FRandomBetween(3.0f, 4.0f) / ((double)pObject->M * gDouble_0058abe0) + pObject->v.v[1]);
        pObject->omega.v[2] = (float)((double)FRandomPosNeg(1.75f) * gDouble_0058abe8 / pObject->M + pObject->omega.v[2]);
        pObject->disable_move_rotate = 0;
        pObject->omega.v[0] = (float)((double)FRandomPosNeg(1.75f) * gDouble_0058abe8 / pObject->M + pObject->omega.v[0]);
    }
#else
    __asm {
            push ebx
            push esi
            mov esi, ecx
            push edi
            mov edi, edx
            test dword ptr [esi + 0x1a0], 0xf0000
            je L004d9dec
            call IsItReallyOKThatWeDontMakeAnyEffortToProtectAnySadFuckersOutThereThatDontWishToSeeInnocentPeopleBlownToBitsByHighExplosiveMinesAndShells
            test eax, eax
            je L004d9e93
            mov eax, dword ptr [esi + 0x23c]
            xor ebx, ebx
            mov ecx, dword ptr [eax + 0xe4]
            mov edx, dword ptr [ecx]
            test byte ptr [edx + 0x14], 4
            jne L004d9d47
            cmp dword ptr [gCrush_deferred], ebx
            jne L004d9d47
            call SetThisPedPhysicing
    L004d9d47:
            cmp edi, 0xe
            je L004d9d55
            mov dword ptr [esi + 0x68], ebx
            mov dword ptr [esi + 0x6c], ebx
            mov dword ptr [esi + 0x70], ebx
    L004d9d55:
            push 0x3f800000
            push 0x3f333333
            call FRandomBetween
            fld dword ptr [esi + 4]
            fmul qword ptr [gDouble_0058abe0]
            push 0x3f000000
            fdivp st(1), st(0)
            fadd dword ptr [esi + 0x6c]
            fstp dword ptr [esi + 0x6c]
            call FRandomPosNeg
            fmul qword ptr [gDouble_0058abe8]
            fld dword ptr [esi + 4]
            fdivp st(1), st(0)
            push 0x3f000000
            fadd dword ptr [esi + 0x7c]
            fstp dword ptr [esi + 0x7c]
            call FRandomPosNeg
            fmul qword ptr [gDouble_0058abe8]
            fld dword ptr [esi + 4]
            fdivp st(1), st(0)
            mov byte ptr [esi + 0xec], bl
            mov edx, 0x1f4
            fadd dword ptr [esi + 0x74]
            fstp dword ptr [esi + 0x74]
            mov edi, dword ptr [gPed_dismemberfest]
            mov ecx, dword ptr [gProgram_state+0xb4]
            mov dword ptr [gPed_dismemberfest], 1
            mov eax, dword ptr [esi + 0x23c]
            push ecx
            push 0x61
            mov ecx, dword ptr [eax + 0xe4]
            push 0x61
            call SetPedFall
            mov dword ptr [gPed_dismemberfest], edi
            pop edi
            pop esi
            pop ebx
            ret
    L004d9dec:
            xor ebx, ebx
            cmp esi, ebx
            je L004d9e37
            cmp byte ptr [esi + 0x238], 0x10
            jne L004d9e37
            push 0x40800000
            push 0x40400000
            call FRandomBetween
            fld dword ptr [esi + 4]
            fmul qword ptr [gDouble_0058abe0]
            push 0x3fe00000
            fdivp st(1), st(0)
            fadd dword ptr [esi + 0x6c]
            fstp dword ptr [esi + 0x6c]
            call FRandomPosNeg
            fmul qword ptr [gDouble_0058abe8]
            fld dword ptr [esi + 4]
            fdivp st(1), st(0)
            push 0x3fe00000
            jmp L004d9e71
    L004d9e37:
            push 0x40000000
            push 0x3f800000
            call FRandomBetween
            fld dword ptr [esi + 4]
            fmul qword ptr [gDouble_0058abe0]
            push 0x3f000000
            fdivp st(1), st(0)
            fadd dword ptr [esi + 0x6c]
            fstp dword ptr [esi + 0x6c]
            call FRandomPosNeg
            fmul qword ptr [gDouble_0058abe8]
            fld dword ptr [esi + 4]
            fdivp st(1), st(0)
            push 0x3f000000
    L004d9e71:
            fadd dword ptr [esi + 0x7c]
            fstp dword ptr [esi + 0x7c]
            call FRandomPosNeg
            fmul qword ptr [gDouble_0058abe8]
            fld dword ptr [esi + 4]
            fdivp st(1), st(0)
            mov byte ptr [esi + 0xec], bl
            fadd dword ptr [esi + 0x74]
            fstp dword ptr [esi + 0x74]
    L004d9e93:
            pop edi
            pop esi
            pop ebx
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
