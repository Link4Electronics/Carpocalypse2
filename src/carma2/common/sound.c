#include "sound.h"

#include "brucetrk.h"
#include "car.h"
#include "errors.h"
#include "globvars.h"
#include "graphics.h"
#include "loading.h"
#include "opponent.h"
#include "piping.h"
#include "replay.h"
#include "utility.h"
#include "world.h"

#include "platform.h"

#include <s3/s3.h>

#include "c2_string.h"

#include "carpocalypse2_macros.h"
#include "sound.h"

#include "car.h"
#include "loading1.h"
#include "piping.h"
#include "world3.h"
#include "opponent.h"
#include "utility.h"
#include "errors.h"
#include "packfile.h"
#include "globvars.h"
#include "platform.h"
#include "carpocalypse2_macros.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
extern tS3_outlet* gIndexed_outlets[7];
// GLOBAL: CARMA2_HW 0x00595c28
extern const char* gSound_periodicity_choices[3];
// GLOBAL: CARMA2_HW 0x00595c44
extern int gINT_00595c44;
// GLOBAL: CARMA2_HW 0x00684548
extern tU32 gNext_track_finished_check;
// GLOBAL: CARMA2_HW 0x00595c20
extern int gINT_00595c20;
// GLOBAL: CARMA2_HW 0x00684558
extern int gServicing_sound;
// GLOBAL: CARMA2_HW 0x00684544
extern tU32 gLast_sound_service;
// GLOBAL: CARMA2_HW 0x0068454c
extern int gCDA_started_playing;
// GLOBAL: CARMA2_HW 0x0068455c
extern undefined4 gUNK_0068455c;
// GLOBAL: CARMA2_HW 0x00595c50
extern int gLast_tune;
// GLOBAL: CARMA2_HW 0x00595c78
extern int gRandom_CDA_tunes[8];
// GLOBAL: CARMA2_HW 0x00595c98
extern int gRandom_CDA_tunes_1[4];
// GLOBAL: CARMA2_HW 0x00595ca8
extern int gRandom_CDA_tunes_2[4];
// GLOBAL: CARMA2_HW 0x0079e1a0
tEnvironment_sound_generator_info gEnvironmental_sound_sources_buffer[2][20];

// GLOBAL: CARMA2_HW 0x00684550
const char* gPed_sound_path;

// GLOBAL: CARMA2_HW 0x0068b898
int gCD_fully_installed;

// GLOBAL: CARMA2_HW 0x00684558
int gServicing_sound;

// GLOBAL: CARMA2_HW 0x00684560
int gSound_sources_inited;

// GLOBAL: CARMA2_HW 0x00684544
tU32 gLast_sound_service;

// GLOBAL: CARMA2_HW 0x0068454c
int gCDA_started_playing;

// GLOBAL: CARMA2_HW 0x00684548
tU32 gNext_track_finished_check;

// GLOBAL: CARMA2_HW 0x00684564
int gMusic_available;

// GLOBAL: CARMA2_HW 0x00684568
int gINT_00684568;

// GLOBAL: CARMA2_HW 0x0079e140
int gINT_0079e140;

// GLOBAL: CARMA2_HW 0x00684554
int gINT_00684554;

// GLOBAL: CARMA2_HW 0x00595c44
int gINT_00595c44 = 1;

// GLOBAL: CARMA2_HW 0x00595c20
int gINT_00595c20 = 9998;

// GLOBAL: CARMA2_HW 0x0068455c
undefined4 gUNK_0068455c;

// GLOBAL: CARMA2_HW 0x0079e13c
int gCD_is_disabled;

// GLOBAL: CARMA2_HW 0x00595c78
int gRandom_CDA_tunes[8] = { 9600, 9601, 9602, 9603, 9604, 9605, 9606, 9607 };

// GLOBAL: CARMA2_HW 0x00595c98
int gRandom_CDA_tunes_1[4] = { 9600, 9601, 9602, 9603 };

// GLOBAL: CARMA2_HW 0x00595ca8
int gRandom_CDA_tunes_2[4] = { 9604, 9605, 9606, 9607 };

// GLOBAL: CARMA2_HW 0x00595c50
int gLast_tune = -1;

// GLOBAL: CARMA2_HW 0x00595c28
const char* gSound_periodicity_choices[3] = {
    "RANDOM",
    "PERIODIC",
    "CONTINUOUS",
};

// GLOBAL: CARMA2_HW 0x00595c48
int gSound_detail_level = 1;

// GLOBAL: CARMA2_HW 0x00684570
tEnvironment_sound_source gEnvironment_sound_sources[5];

// GLOBAL: CARMA2_HW 0x006845fc
tS3_outlet* gEffects_outlet;

// GLOBAL: CARMA2_HW 0x00684600
tS3_outlet* gXXX_outlet;

// GLOBAL: CARMA2_HW 0x00684604
tS3_outlet* gEngine_outlet;

// GLOBAL: CARMA2_HW 0x00684608
tS3_outlet* gDriver_outlet;

// GLOBAL: CARMA2_HW 0x0068460c
tS3_outlet* gPedestrians_outlet;

// GLOBAL: CARMA2_HW 0x00684610
tS3_outlet* gMusic_outlet;

// GLOBAL: CARMA2_HW 0x00684614
tS3_outlet* gCar_outlet;

// GLOBAL: CARMA2_HW 0x0079e160
tS3_outlet* gIndexed_outlets[7] = { 0 };

// GLOBAL: CARMA2_HW 0x0079e17c
int gEnvironmental_sound_sources_buffer_index = 0;

// GLOBAL: CARMA2_HW 0x00595c38
const char* gSound_generator_type_names[3] = {
    "NONCAR",
    "ACTOR",
    "POINT",
};

// GLOBAL: CARMA2_HW 0x00655dd8
int gVirgin_pass = 1;

// GLOBAL: CARMA2_HW 0x00595c4c
int gOld_sound_detail_level = -1;

// GLOBAL: CARMA2_HW 0x0079e120
br_vector3 gCamera_left;

// GLOBAL: CARMA2_HW 0x0079e130
br_vector3 gCamera_position;

// GLOBAL: CARMA2_HW 0x0079e180
br_vector3 gOld_camera_position;

// GLOBAL: CARMA2_HW 0x0079ea60
br_vector3 gCamera_velocity;

// GLOBAL: CARMA2_HW 0x00684540
tU32 gNext_sound_generator_munging;

// GLOBAL: CARMA2_HW 0x0079e18c
int gCount_environmental_sound_sources;

// GLOBAL: CARMA2_HW 0x00589ea8
const double gDOUBLE_00589ea8 = 1.0;

// GLOBAL: CARMA2_HW 0x00589f14
const br_scalar gFLOAT_00589f14 = 2.38418579e-7f;

// GLOBAL: CARMA2_HW 0x00589f1c
const br_scalar gFLOAT_00589f1c = 0.1f;

#ifdef CARPOCALYPSE2_MATCHING
#endif
#ifdef CARPOCALYPSE2_MATCHING
#endif
#ifdef CARPOCALYPSE2_MATCHING
#endif
#ifdef CARPOCALYPSE2_MATCHING
extern tSpecial_volume* C2_HOOK_FASTCALL FindSpecialVolume(br_vector3* pP, tSpecial_volume* pLast_vol, int pArg3);
extern int C2_HOOK_FASTCALL GetCarCount(tVehicle_type pCategory);
extern tCar_spec* C2_HOOK_FASTCALL GetCarSpec(tVehicle_type pCategory, int pIndex);
extern tU32 C2_HOOK_FASTCALL GetTotalTime(void);
extern int C2_HOOK_FASTCALL IRandomBetween(int pA, int pB);
extern int C2_HOOK_FASTCALL IRandomPosNeg(int pN);
extern void C2_HOOK_FASTCALL MungeSoundGenerators(void);
extern void C2_HOOK_FASTCALL PipeSingleSound(tS3_outlet* pOutlet, int pSound, tS3_volume pL_volume, tS3_volume pR_volume, int pPitch, const br_vector3* pPosition);
extern int C2_HOOK_FASTCALL S3BindAmbientSoundToOutlet(tS3_outlet* pOutlet, int pSound, tS3_sound_source* source, float pMax_distance, int pPeriod, int pRepeats, int pVolume, int pPitch, int pSpeed);
extern void C2_HOOK_FASTCALL S3Disable(void);
extern void C2_HOOK_FASTCALL S3Enable(void);
extern int C2_HOOK_FASTCALL S3ReleaseSoundSource(tS3_sound_source* src);
extern void C2_HOOK_FASTCALL S3Service(int inside_cockpit, int unk1);
extern int C2_HOOK_FASTCALL S3StartSound3D(tS3_outlet *pOutlet, int pSound, const br_vector3* pInitial_position, const br_vector3* pInitial_velocity, int pRepeats, int pVolume, int pPitch, int pSpeed);
extern int C2_HOOK_FASTCALL S3StopAllOutletSounds(void);
extern int C2_HOOK_FASTCALL S3StopOutletSound(tS3_outlet* pOutlet);
extern void C2_HOOK_FASTCALL S3UpdateSoundSource(tS3_outlet* outlet, tS3_sound_tag tag, tS3_sound_source* src, float pMax_distance_squared, int pPeriod, tS3_repeats pAmbient_repeats, tS3_volume pVolume, int pPitch, tS3_speed pSpeed);
extern int gAmbient_sound;
extern br_matrix34 gCamera_to_world;
extern int gFaded_palette;
extern tProgram_state gProgram_state;
extern int gSound_available;
extern int gSound_enabled;
extern br_vector3 gZero_v__car;
#endif
#ifdef CARPOCALYPSE2_MATCHING
extern float C2_HOOK_FASTCALL ARGetReplayRate(void);
extern void* C2_HOOK_CDECL BrMemAllocate();
extern br_model* C2_HOOK_CDECL BrModelFind();
extern int C2_HOOK_FASTCALL DRS3StartCDA(int pSound);
extern void C2_HOOK_FASTCALL DoAnEnvironmentalSound(void* pSrc_object, tEnvironment_sound_generator_info* pEnv_info, int pVolume, br_vector3* pPos);
extern void C2_NORETURN C2_HOOK_CDECL FatalError();
extern int C2_HOOK_FASTCALL GetALineAndInterpretCommand(FILE* pF, const char** pString_list, int pCount);
extern char * C2_HOOK_FASTCALL GetALineWithNoPossibleService(FILE* pF, char* pS);
extern void C2_HOOK_FASTCALL GetAString(FILE* pF, char* pString);
extern int C2_HOOK_FASTCALL GetAnInt(FILE* pF);
extern void C2_HOOK_FASTCALL GetThreeFloats(FILE * pF, float* pF1, float* pF2, float* pF3);
extern int C2_HOOK_FASTCALL PDCheckDriveExists(const char* pThe_path);
extern tU32 C2_HOOK_FASTCALL PDGetTotalTime(void);
extern int C2_HOOK_FASTCALL PFfclose(FILE* pFile);
extern FILE* C2_HOOK_FASTCALL PFfopen(const char* pPath, const char* mode);
extern void C2_HOOK_FASTCALL PathCat(char* pDestn_str, const char* pStr_1, const char* pStr_2);
extern void C2_HOOK_FASTCALL PossibleService(void);
extern int C2_HOOK_FASTCALL ProcessNearbyActors(tTrack_spec* pTrack, br_vector3* pPos, float pMax_dist, int pMatch_type, char pIdentifier_value, tS8 pIdentifier_index, tU16 pMatch_flags, tNearbyActors_cbfn* pCallback, void* pContext);
extern void C2_HOOK_FASTCALL S3BindListenerLeftBRender(br_vector3* left);
extern void C2_HOOK_FASTCALL S3BindListenerPositionBRender(br_vector3* pos);
extern void C2_HOOK_FASTCALL S3BindListenerVelocityBRender(br_vector3* vel);
extern tS3_outlet* C2_HOOK_FASTCALL S3CreateOutlet(int pCount_channels_1, int pCount_channels_2);
extern tS3_sound_source* C2_HOOK_FASTCALL S3CreateSoundSourceBR(br_vector3* pPosition, br_vector3* pVelocity, tS3_outlet* pBound_outlet);
extern int C2_HOOK_FASTCALL S3Init(const char* pPath, int pLow_memory_mode, const char* pSound_path);
extern int C2_HOOK_FASTCALL S3IsCDAPlaying(void);
extern tS3_error_codes C2_HOOK_FASTCALL S3ReleaseOutlet(tS3_outlet* pOutlet);
extern int C2_HOOK_STDCALL S3Set3DSoundEnvironment(float a1, float a2, float a3);
extern int C2_HOOK_FASTCALL S3SetOutletVolume(tS3_outlet* pOutlet, int pVolume);
extern tS3_error_codes C2_HOOK_FASTCALL S3StopSound(int pTag);
extern br_scalar C2_HOOK_STDCALL SRandomPosNeg(br_scalar pN);
extern void __cdecl _ftol();
extern void C2_HOOK_CDECL dr_dprintf();
extern tNon_car_spec* gActive_non_car_list[99];
extern tPath_name gApplication_path;
extern int gAusterity_mode;
extern const char* gDir_separator;
extern char gDrone_select_max_distance_squared[];
extern tU32 gFrame_period;
extern char gNext_crush_actor_id[];
extern int gNum_active_non_cars;
extern int gPed_676914;
extern int gSound_override;
#endif
// FUNCTION: CARMA2_HW 0x00456af0
void C2_NAKED C2_HOOK_FASTCALL DoEnvSound(tSpecial_volume* pVolume, br_vector3* pP, tSpecial_volume_soundfx_type pType, tSpecial_volume_soundfx_data* pSound_data, float pSound_dist, br_vector3 *pSound_dir) {
#ifndef CARPOCALYPSE2_MATCHING

    NOT_IMPLEMENTED();
#else
    __asm {
            mov eax, dword ptr [gAmbient_sound]
            sub esp, 0xc
            test eax, eax
            push esi
            push edi
            mov esi, edx
            mov edi, ecx
            je L00456c51
            mov eax, dword ptr [esp + 0x18]
            cmp eax, -1
            je L00456c51
            test eax, eax
            jne L00456bf3
            fld dword ptr [esp + 0x20]
            fcomp dword ptr [gDrone_select_max_distance_squared+0xcc]
            fnstsw ax
            test ah, 0x40
            jne L00456bdd
            mov ecx, dword ptr [esp + 0x24]
            fld dword ptr [ecx + 4]
            fld dword ptr [ecx + 8]
            _emit 0xd9
        _emit 0x01
            fxch st(2)
            fst dword ptr [esp + 0x18]
            fxch st(1)
            fst dword ptr [esp + 0x24]
            fxch st(1)
            fmul dword ptr [esp + 0x18]
            fxch st(1)
            fmul dword ptr [esp + 0x24]
            fld st(2)
            fmul st(0), st(3)
            fxch st(1)
            faddp st(2), st(0)
            faddp st(1), st(0)
            fsqrt
            fstp st(1)
            fcom dword ptr [gFLOAT_00589f14]
            fnstsw ax
            test ah, 0x41
            jne L00456b8d
            fdivr qword ptr [gDOUBLE_00589ea8]
            fld dword ptr [ecx + 8]
            fld dword ptr [ecx + 4]
            fld st(2)
            _emit 0xd8
        _emit 0x09
            fxch st(1)
            fmul st(0), st(3)
            fxch st(2)
            fmul st(0), st(3)
            fxch st(1)
            fstp st(3)
            jmp L00456ba1
    L00456b8d:
            fstp st(0)
            fld dword ptr [gDOUBLE_00589ea8+0x60]
            fld dword ptr [gDrone_select_max_distance_squared+0xcc]
            fld dword ptr [gDrone_select_max_distance_squared+0xcc]
    L00456ba1:
            fld dword ptr [esp + 0x20]
            fld st(1)
            fxch st(1)
            fchs
            fxch st(4)
            fmul st(0), st(4)
            fxch st(3)
            fmul st(0), st(4)
            fxch st(1)
            fmul st(0), st(4)
            fxch st(3)
            _emit 0xd8
        _emit 0x06
            fxch st(1)
            fadd dword ptr [esi + 4]
            fxch st(3)
            fadd dword ptr [esi + 8]
            fxch st(1)
            fstp dword ptr [esp + 8]
            fxch st(2)
            fstp dword ptr [esp + 0xc]
            fxch st(1)
            fstp dword ptr [esp + 0x10]
            fstp st(0)
            fstp st(0)
            jmp L00456c25
    L00456bdd:
            mov eax, dword ptr [esi]
            mov ecx, dword ptr [esi + 4]
            mov edx, dword ptr [esi + 8]
            mov dword ptr [esp + 8], eax
            mov dword ptr [esp + 0xc], ecx
            mov dword ptr [esp + 0x10], edx
            jmp L00456c25
    L00456bf3:
            push 0x41200000
            call SRandomPosNeg
            _emit 0xd8
        _emit 0x06
            push 0x41200000
            fstp dword ptr [esp + 0xc]
            call SRandomPosNeg
            fadd dword ptr [esi + 4]
            push 0x41200000
            fstp dword ptr [esp + 0x10]
            call SRandomPosNeg
            fadd dword ptr [esi + 8]
            fstp dword ptr [esp + 0x10]
    L00456c25:
            fld dword ptr [gFLOAT_00589f14+0x4]
            fsub dword ptr [esp + 0x20]
            lea eax, [esp + 8]
            push eax
            fmul dword ptr [gDOUBLE_00589ea8+0x8]
            fmul dword ptr [gFLOAT_00589f1c]
            call _ftol
            mov edx, dword ptr [esp + 0x20]
            push eax
            mov ecx, edi
            call DoAnEnvironmentalSound
    L00456c51:
            pop edi
            pop esi
            add esp, 0xc
            ret 0x10
    }
#endif
}

// FUNCTION: CARMA2_HW 0x00456c60
void C2_NAKED C2_HOOK_FASTCALL DoAnEnvironmentalSound(void* pSrc_object, tEnvironment_sound_generator_info* pEnv_info, int pVolume, br_vector3* pPos) {
#ifndef CARPOCALYPSE2_MATCHING

    NOT_IMPLEMENTED();
#else
    __asm {
            push ebx
            push ebp
            push esi
            push edi
            mov edi, edx
            mov ebx, ecx
            call GetTotalTime
            mov esi, eax
            mov eax, dword ptr [edi]
            xor ebp, ebp
            sub eax, ebp
            je L00456d62
            dec eax
            je L00456d40
            dec eax
            jne L00456d7c
            or edx, 0xffffffff
            xor eax, eax
            mov esi, offset gEnvironment_sound_sources
    L00456c93:
            mov ecx, dword ptr [esi + 0x10]
            cmp ecx, 1
            jne L00456ca0
            cmp dword ptr [esi + 0x14], ebx
            je L00456caf
    L00456ca0:
            test ecx, ecx
            jne L00456ca6
            mov edx, eax
    L00456ca6:
            inc eax
            add esi, 0x1c
            cmp eax, 5
            jl L00456c93
    L00456caf:
            cmp eax, 5
            jne L00456cd4
            test edx, edx
            jl L00456cd2
            mov eax, edx
            shl eax, 3
            sub eax, edx
            lea esi, [eax*4 + gEnvironment_sound_sources]
            mov dword ptr [esi + 0x10], 1
            mov dword ptr [esi + 0x14], ebx
            jmp L00456cd4
    L00456cd2:
            xor esi, esi
    L00456cd4:
            test esi, esi
            je L00456d34
            mov edx, dword ptr [esp + 0x18]
            lea ecx, [esi + 4]
            push -1
            mov eax, dword ptr [edx]
            mov dword ptr [ecx], eax
            mov eax, dword ptr [edx + 4]
            mov dword ptr [ecx + 4], eax
            mov edx, dword ptr [edx + 8]
            mov dword ptr [esi + 0x18], 1
            mov dword ptr [ecx + 8], edx
            mov ecx, dword ptr [edi + 0x14]
            call IRandomPosNeg
            mov ecx, dword ptr [esi]
            add eax, 0x10000
            mov edx, dword ptr [edi + 0x10]
            push eax
            mov eax, dword ptr [esp + 0x1c]
            dec edx
            push eax
            push -1
            push -1
            push 0xbf800000
            push ecx
            xor ecx, ecx
            call IRandomBetween
            mov ecx, dword ptr [gCar_outlet]
            xor edx, edx
            mov dx, word ptr [edi + eax*2 + 0x18]
            call S3UpdateSoundSource
    L00456d34:
            mov dword ptr [gINT_0079e140], 1
            jmp L00456d7c
    L00456d40:
            mov ecx, dword ptr [edi + 0xc]
            cmp esi, ecx
            jb L00456d7c
            mov eax, dword ptr [edi + 4]
            mov edx, esi
            sub edx, ecx
            mov ebp, 1
            cmp edx, eax
            jbe L00456d5e
            add esi, eax
            mov dword ptr [edi + 0xc], esi
            jmp L00456d7c
    L00456d5e:
            add eax, ecx
            jmp L00456d79
    L00456d62:
            cmp esi, dword ptr [edi + 0xc]
            jb L00456d7c
            mov edx, dword ptr [edi + 8]
            mov ecx, dword ptr [edi + 4]
            mov ebp, 1
            call IRandomBetween
            add eax, esi
    L00456d79:
            mov dword ptr [edi + 0xc], eax
    L00456d7c:
            test ebp, ebp
            je L00456e13
            mov ecx, dword ptr [edi + 0x14]
            call IRandomPosNeg
            mov edx, dword ptr [edi + 0x10]
            mov ebx, eax
            add ebx, 0x10000
            dec edx
            xor ecx, ecx
            call IRandomBetween
            xor esi, esi
            mov si, word ptr [edi + eax*2 + 0x18]
            mov eax, dword ptr [gSound_enabled]
            mov edi, dword ptr [gCar_outlet]
            test eax, eax
            je L00456e13
            mov ebp, dword ptr [esp + 0x14]
            test ebp, ebp
            je L00456df7
            cmp esi, 0x3e8
            je L00456df7
            cmp esi, 0xbb8
            jl L00456dd5
            cmp esi, 0xbbf
            jle L00456df7
    L00456dd5:
            cmp esi, 0x14b4
            jl L00456de5
            cmp esi, 0x14c8
            jle L00456df7
    L00456de5:
            mov eax, dword ptr [esp + 0x18]
            mov edx, esi
            push eax
            push ebx
            push 0
            push ebp
            mov ecx, edi
            call PipeSingleSound
    L00456df7:
            mov ecx, dword ptr [esp + 0x18]
            push 0x10000
            push ebx
            push ebp
            push 1
            push offset gZero_v__car
            push ecx
            mov edx, esi
            mov ecx, edi
            call S3StartSound3D
    L00456e13:
            pop edi
            pop esi
            pop ebp
            pop ebx
            ret 8
            nop
            nop
            nop
            nop
            nop
            nop
    }
#endif
}

// FUNCTION: CARMA2_HW 0x00455950
int C2_HOOK_FASTCALL DRS3SetVolume(int pVolume) {
    if (gSound_enabled) {
        return S3SetVolume(pVolume);
    } else {
        return 0;
    }
}

// FUNCTION: CARMA2_HW 0x00457260
intptr_t C2_NAKED C2_HOOK_FASTCALL FoundSoundSource(br_actor* pActor, void* pContext) {
#ifndef CARPOCALYPSE2_MATCHING
    tEnvironment_sound_generator_vector* ctx = (tEnvironment_sound_generator_vector*)pContext;
    tEnvironment_sound_generator_info* entry;
    char* s;
    char* source;
    int index;
    tTrackSoundGenerator* generator;

    if (ctx->count >= 0x14) {
        return 0;
    }
    entry = &ctx->sources[ctx->count];
    ctx->count += 1;
    entry->actor = pActor;

    s = pActor->model->identifier;
    index = (int)*(tS8*)(s + 1) * 0xa + (int)*(tS8*)(s + 2) - 0x210;
    generator = &gProgram_state.track_sound_generators[index];
    entry->generator = generator;

    source = (char*)((tU8*)generator + 0x4);
    if (generator->type == kSoundGeneratorType_noncar && pActor->identifier[3] == '!') {
        source = (char*)((tU8*)generator + 0x4c);
    }
    memcpy(&entry->soundfx, source, sizeof(entry->soundfx));
    entry->pos = pActor->t.t.translate.t;
    return 0;
#else
    __asm {
            push ebx
            mov ebx, ecx
            mov ecx, dword ptr [edx]
            cmp ecx, 0x14
            jge L004572e3
            push edi
            lea eax, [ecx*8]
            push esi
            mov esi, dword ptr [edx + 4]
            sub eax, ecx
            inc ecx
            mov dword ptr [edx], ecx
            lea eax, [esi + eax*8]
            mov dword ptr [eax + 0x28], ebx
            mov ecx, dword ptr [ebx + 0x18]
            mov ecx, dword ptr [ecx + 4]
            movsx edx, byte ptr [ecx + 1]
            movsx ecx, byte ptr [ecx + 2]
            lea edx, [edx + edx*4]
            lea ecx, [ecx + edx*2 - 0x210]
            mov edx, ecx
            shl edx, 5
            sub edx, ecx
            mov ecx, dword ptr [gProgram_state+0x1c08]
            lea ecx, [ecx + edx*4]
            mov dword ptr [eax + 0x24], ecx
            cmp dword ptr [ecx], 0
            jne L004572bf
            mov edx, dword ptr [ebx + 0x14]
            cmp byte ptr [edx + 3], 0x21
            jne L004572bf
            lea esi, [ecx + 0x4c]
            jmp L004572c2
    L004572bf:
            lea esi, [ecx + 4]
    L004572c2:
            mov ecx, 9
            mov edi, eax
            add ebx, 0x50
            add eax, 0x2c
            rep movsd
            mov ecx, dword ptr [ebx]
            pop esi
            mov dword ptr [eax], ecx
            pop edi
            mov edx, dword ptr [ebx + 4]
            mov dword ptr [eax + 4], edx
            mov ecx, dword ptr [ebx + 8]
            mov dword ptr [eax + 8], ecx
    L004572e3:
            xor eax, eax
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

// FUNCTION: CARMA2_HW 0x00456ea0
void C2_NAKED C2_HOOK_FASTCALL MungeSoundGenerators(void) {
#ifndef CARPOCALYPSE2_MATCHING

    if (gAmbient_sound) {
        int i;

        if (GetTotalTime() - gNext_sound_generator_munging >= 200) {
            tEnvironment_sound_generator_vector found_sources;

            gNext_sound_generator_munging += 200;
            found_sources.sources = gEnvironmental_sound_sources_buffer[gEnvironmental_sound_sources_buffer_index ^ 1];
            found_sources.count = 0;
            ProcessNearbyActors(&gProgram_state.track_spec, (br_vector3*)gCamera_to_world.m[3], 10.f, 1, ')', 0, 0x1003, FoundSoundSource, &found_sources);
            for (i = 0; i < gProgram_state.count_track_sound_generators; i++) {
                tTrackSoundGenerator* generator = &gProgram_state.track_sound_generators[i];
                if (generator->type == kSoundGeneratorType_point && Vector3DistanceSquared(&generator->point, (br_vector3*)gCamera_to_world.m[3]) < 10.f * 10.f) {
                    tEnvironment_sound_generator_info* src = &found_sources.sources[found_sources.count];
                    src->generator = generator;
                    src->soundfx = generator->fx;
                    src->pos = generator->point;
                    src->actor = NULL;
                    found_sources.count += 1;
                }
            }
            for (i = 0; i < gNum_active_non_cars; i++) {
                tNon_car_spec* c = gActive_non_car_list[i];
                br_actor* a = c->collision_info->actor;
                int src_count = found_sources.count;
                if (a != NULL && a->model !=NULL
                        && a->model->identifier != NULL && a->model->identifier[0] == ')'
                        && Vector3DistanceSquared(&a->t.t.translate.t, (br_vector3*)gCamera_to_world.m[3]) < 10.f * 10.f) {
                    FoundSoundSource(a, &found_sources);
                    if (src_count < found_sources.count && !Vector3IsZero(&c->collision_info->v)) {
                        found_sources.sources[src_count].soundfx = found_sources.sources[src_count].generator->fx1_noncar;
                    }
                }
            }
            for (i = 0; i < found_sources.count; i++) {
                int j;
                tEnvironment_sound_generator_info * src_now = &found_sources.sources[i];

                for (j = 0; j < gCount_environmental_sound_sources; j++) {
                    tEnvironment_sound_generator_info* src_prev = &gEnvironmental_sound_sources_buffer[gEnvironmental_sound_sources_buffer_index][j];

                    if (src_now->generator == src_prev->generator && src_now->actor == src_prev->actor) {
                        src_now->soundfx.field_0xc = src_prev->soundfx.field_0xc;
                        break;
                    }
                }
            }
            gEnvironmental_sound_sources_buffer_index ^= 1;
            gCount_environmental_sound_sources = found_sources.count;
        }

        for (i = 0; i < gCount_environmental_sound_sources; i++) {
            br_vector3 delta;
            tEnvironment_sound_generator_info* src;
            void *src_obj;

            src = &gEnvironmental_sound_sources_buffer[gEnvironmental_sound_sources_buffer_index][i];
            BrVector3Sub(&delta, &src->pos, (br_vector3*)gCamera_to_world.m[3]);
            src_obj = src->actor;
            if (src_obj == NULL) {
                src_obj = src->generator;
            }
            DoAnEnvironmentalSound(src_obj, src, (int)((10.f - BrVector3LengthSquared(&delta)) * 255.f / 10.f), &src->pos);
        }
    }
#else
    __asm {
            sub esp, 0xc
            push ebx
            push ebp
            push esi
            push edi
            call GetTotalTime
            mov ecx, dword ptr [gAmbient_sound]
            xor ebx, ebx
            cmp ecx, ebx
            je L00457249
            mov edx, dword ptr [gNext_sound_generator_munging]
            sub eax, edx
            cmp eax, 0xc8
            jb L00457194
            mov ecx, dword ptr [gEnvironmental_sound_sources_buffer_index]
            mov edi, edx
            xor ecx, 1
            add edi, 0xc8
            mov eax, ecx
            mov edx, offset gCamera_to_world+0x24
            shl eax, 3
            sub eax, ecx
            lea ecx, [esp + 0x14]
            push ecx
            push offset FoundSoundSource
            lea eax, [eax + eax*4]
            push 0x1003
            shl eax, 5
            push ebx
            push 0x29
            add eax, offset gEnvironmental_sound_sources_buffer
            push 1
            push 0x41200000
            mov ecx, offset gProgram_state+0x1ba0
            mov dword ptr [gNext_sound_generator_munging], edi
            mov dword ptr [esp + 0x30], ebx
            mov dword ptr [esp + 0x34], eax
            call ProcessNearbyActors
            mov eax, dword ptr [gProgram_state+0x1c04]
            mov edx, dword ptr [gProgram_state+0x1c08]
            cmp eax, ebx
            jle L00456feb
    L00456f3b:
            cmp dword ptr [edx], 2
            jne L00456fda
            fld dword ptr [edx + 0x74]
            fld dword ptr [edx + 0x70]
            fsub dword ptr [gCamera_to_world+0x24]
            fxch st(1)
            fsub dword ptr [gCamera_to_world+0x28]
            fld dword ptr [edx + 0x78]
            fsub dword ptr [gCamera_to_world+0x2c]
            fxch st(2)
            fst dword ptr [esp + 0x10]
            fld st(1)
            fxch st(1)
            fmul dword ptr [esp + 0x10]
            fxch st(1)
            fmul st(0), st(2)
            fxch st(3)
            fst dword ptr [esp + 0x10]
            fmul dword ptr [esp + 0x10]
            fxch st(3)
            faddp st(1), st(0)
            fxch st(2)
            faddp st(2), st(0)
            fxch st(1)
            fsqrt
            fstp st(1)
            fcomp dword ptr [gFLOAT_00589f1c+0x4]
            fnstsw ax
            test ah, 1
            je L00456fda
            mov ecx, dword ptr [esp + 0x14]
            mov esi, dword ptr [esp + 0x18]
            mov eax, ecx
            shl eax, 3
            sub eax, ecx
            inc ecx
            mov dword ptr [esp + 0x14], ecx
            mov ecx, 9
            lea eax, [esi + eax*8]
            lea esi, [edx + 4]
            mov edi, eax
            mov dword ptr [eax + 0x24], edx
            rep movsd
            mov esi, dword ptr [edx + 0x70]
            lea ecx, [eax + 0x2c]
            mov dword ptr [eax + 0x2c], esi
            mov esi, dword ptr [edx + 0x74]
            mov dword ptr [ecx + 4], esi
            mov esi, dword ptr [edx + 0x78]
            mov dword ptr [eax + 0x28], 0
            mov dword ptr [ecx + 8], esi
    L00456fda:
            mov eax, dword ptr [gProgram_state+0x1c04]
            inc ebx
            add edx, 0x7c
            cmp ebx, eax
            jl L00456f3b
    L00456feb:
            mov eax, dword ptr [gNum_active_non_cars]
            xor ebp, ebp
            test eax, eax
            jle L004570f2
            mov ebx, offset gActive_non_car_list
    L00456fff:
            mov edi, dword ptr [ebx]
            mov edx, dword ptr [edi + 8]
            mov ecx, dword ptr [edx]
            test ecx, ecx
            je L004570e1
            mov eax, dword ptr [ecx + 0x18]
            test eax, eax
            je L004570e1
            mov eax, dword ptr [eax + 4]
            test eax, eax
            je L004570e1
            cmp byte ptr [eax], 0x29
            jne L004570e1
            fld dword ptr [ecx + 0x54]
            fld dword ptr [ecx + 0x50]
            fsub dword ptr [gCamera_to_world+0x24]
            fxch st(1)
            fsub dword ptr [gCamera_to_world+0x28]
            fld dword ptr [ecx + 0x58]
            fsub dword ptr [gCamera_to_world+0x2c]
            fxch st(2)
            fst dword ptr [esp + 0x10]
            fld st(1)
            fxch st(1)
            fmul dword ptr [esp + 0x10]
            fxch st(1)
            fmul st(0), st(2)
            fxch st(3)
            fst dword ptr [esp + 0x10]
            fmul dword ptr [esp + 0x10]
            fxch st(3)
            faddp st(1), st(0)
            fxch st(2)
            faddp st(2), st(0)
            fxch st(1)
            fcomp dword ptr [gFLOAT_00589f1c+0x4]
            fnstsw ax
            test ah, 1
            fstp st(0)
            je L004570e1
            mov esi, dword ptr [esp + 0x14]
            lea edx, [esp + 0x14]
            call FoundSoundSource
            cmp dword ptr [esp + 0x14], esi
            jle L004570e1
            mov ecx, dword ptr [edi + 8]
            fld dword ptr [ecx + 0x68]
            fcomp dword ptr [gDrone_select_max_distance_squared+0xcc]
            fnstsw ax
            test ah, 0x40
            je L004570c5
            fld dword ptr [ecx + 0x6c]
            fcomp dword ptr [gDrone_select_max_distance_squared+0xcc]
            fnstsw ax
            test ah, 0x40
            je L004570c5
            fld dword ptr [ecx + 0x70]
            fcomp dword ptr [gDrone_select_max_distance_squared+0xcc]
            fnstsw ax
            test ah, 0x40
            jne L004570e1
    L004570c5:
            mov ecx, dword ptr [esp + 0x18]
            mov eax, esi
            shl eax, 3
            sub eax, esi
            mov esi, dword ptr [ecx + eax*8 + 0x24]
            add esi, 0x28
            lea edi, [ecx + eax*8]
            mov ecx, 9
            rep movsd
    L004570e1:
            mov eax, dword ptr [gNum_active_non_cars]
            inc ebp
            add ebx, 4
            cmp ebp, eax
            jl L00456fff
    L004570f2:
            mov ecx, dword ptr [esp + 0x14]
            mov dword ptr [esp + 0x10], 0
            test ecx, ecx
            jle L0045717d
            mov edx, dword ptr [esp + 0x18]
            mov esi, dword ptr [gEnvironmental_sound_sources_buffer_index]
            lea edi, [edx + 0x28]
            mov edx, dword ptr [gCount_environmental_sound_sources]
    L00457115:
            mov eax, esi
            xor ecx, ecx
            shl eax, 3
            sub eax, esi
            lea eax, [eax + eax*4]
            shl eax, 5
            add eax, offset gEnvironmental_sound_sources_buffer
            test edx, edx
            jle L00457158
            mov ebx, dword ptr [edi - 4]
    L00457130:
            cmp dword ptr [eax + 0x24], ebx
            jne L0045713c
            mov ebp, dword ptr [eax + 0x28]
            cmp ebp, dword ptr [edi]
            je L00457146
    L0045713c:
            inc ecx
            add eax, 0x38
            cmp ecx, edx
            jl L00457130
            jmp L00457158
    L00457146:
            mov eax, dword ptr [eax + 0xc]
            mov dword ptr [edi - 0x1c], eax
            mov esi, dword ptr [gEnvironmental_sound_sources_buffer_index]
            mov edx, dword ptr [gCount_environmental_sound_sources]
    L00457158:
            mov eax, dword ptr [esp + 0x10]
            mov ecx, dword ptr [esp + 0x14]
            inc eax
            add edi, 0x38
            cmp eax, ecx
            mov dword ptr [esp + 0x10], eax
            jl L00457115
            xor esi, 1
            mov dword ptr [gCount_environmental_sound_sources], ecx
            mov dword ptr [gEnvironmental_sound_sources_buffer_index], esi
            jmp L0045719a
    L0045717d:
            mov esi, dword ptr [gEnvironmental_sound_sources_buffer_index]
            mov dword ptr [gCount_environmental_sound_sources], ecx
            xor esi, 1
            mov dword ptr [gEnvironmental_sound_sources_buffer_index], esi
            jmp L0045719a
    L00457194:
            mov esi, dword ptr [gEnvironmental_sound_sources_buffer_index]
    L0045719a:
            mov eax, esi
            xor ebx, ebx
            shl eax, 3
            sub eax, esi
            lea esi, [eax + eax*4]
            mov eax, dword ptr [gCount_environmental_sound_sources]
            shl esi, 5
            add esi, offset gEnvironmental_sound_sources_buffer
            test eax, eax
            jle L00457249
    L004571bc:
            fld dword ptr [esi + 0x30]
            fld dword ptr [esi + 0x2c]
            fsub dword ptr [gCamera_to_world+0x24]
            fxch st(1)
            fsub dword ptr [gCamera_to_world+0x28]
            lea eax, [esi + 0x2c]
            mov ecx, dword ptr [esi + 0x28]
            fld dword ptr [esi + 0x34]
            fsub dword ptr [gCamera_to_world+0x2c]
            fxch st(2)
            fst dword ptr [esp + 0x10]
            fld st(1)
            fxch st(1)
            fmul dword ptr [esp + 0x10]
            fxch st(1)
            fmul st(0), st(2)
            fxch st(3)
            fst dword ptr [esp + 0x10]
            fmul dword ptr [esp + 0x10]
            fxch st(3)
            faddp st(1), st(0)
            fxch st(2)
            faddp st(2), st(0)
            fxch st(1)
            test ecx, ecx
            mov edi, ecx
            fsqrt
            fstp st(1)
            jne L00457212
            mov edi, dword ptr [esi + 0x24]
    L00457212:
            fld dword ptr [gFLOAT_00589f14+0x4]
            fsub st(0), st(1)
            push eax
            fmul dword ptr [gDOUBLE_00589ea8+0x8]
            fmul dword ptr [gFLOAT_00589f1c]
            call _ftol
            push eax
            mov edx, esi
            mov ecx, edi
            fstp st(0)
            call DoAnEnvironmentalSound
            mov eax, dword ptr [gCount_environmental_sound_sources]
            inc ebx
            add esi, 0x38
            cmp ebx, eax
            jl L004571bc
    L00457249:
            pop edi
            pop esi
            pop ebp
            pop ebx
            add esp, 0xc
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

// FUNCTION: CARMA2_HW 0x00456e20
void C2_NAKED C2_HOOK_FASTCALL MungeEnvironmentalSound(void) {
#ifndef CARPOCALYPSE2_MATCHING

    if (gAmbient_sound) {
        int i;

        for (i = 0; i < (int)CARPOCALYPSE2_ASIZE(gEnvironment_sound_sources); i++) {
            gEnvironment_sound_sources[i].field_0x18 = 0;
        }
        if (!gFaded_palette) {
            FindSpecialVolume((br_vector3*)gCamera_to_world.m[3], 0, 1);
            MungeSoundGenerators();
        }
        for (i = 0; i < (int)CARPOCALYPSE2_ASIZE(gEnvironment_sound_sources); i++) {

            if (gEnvironment_sound_sources[i].field_0x18 == 0 && gEnvironment_sound_sources[i].field_0x10 != NULL) {
                S3UpdateSoundSource(gCar_outlet, -1, gEnvironment_sound_sources[i].source, -1.f, -1, -1, 0, -1, -1);
                gEnvironment_sound_sources[i].field_0x10 = NULL;
            }
        }
    }
#else
    __asm {
            mov eax, dword ptr [gAmbient_sound]
            test eax, eax
            je L00456e9f
            push esi
            mov eax, offset gEnvironment_sound_sources+0x18
    L00456e2f:
            mov dword ptr [eax], 0
            add eax, 0x1c
            cmp eax, offset gCar_outlet
            jl L00456e2f
            mov eax, dword ptr [gFaded_palette]
            test eax, eax
            jne L00456e5b
            push 1
            xor edx, edx
            mov ecx, offset gCamera_to_world+0x24
            call FindSpecialVolume
            call MungeSoundGenerators
    L00456e5b:
            mov esi, offset gEnvironment_sound_sources+0x10
    L00456e60:
            mov eax, dword ptr [esi + 8]
            test eax, eax
            jne L00456e93
            cmp dword ptr [esi], 0
            je L00456e93
            mov eax, dword ptr [esi - 0x10]
            mov ecx, dword ptr [gCar_outlet]
            push -1
            push -1
            push 0
            push -1
            push -1
            push 0xbf800000
            push eax
            or edx, 0xffffffff
            call S3UpdateSoundSource
            mov dword ptr [esi], 0
    L00456e93:
            add esi, 0x1c
            cmp esi, offset gPedestrians_outlet
            jl L00456e60
            pop esi
    L00456e9f:
            ret
    }
#endif
}
// FUNCTION: CARMA2_HW 0x00500060
void C2_HOOK_FASTCALL SplungeSomeData(void* pData, br_size_t size) {
    // empty
}

// FUNCTION: CARMA2_HW 0x00454f40
void C2_HOOK_FASTCALL UsePathFileToDetermineIfFullInstallation(void) {

    char line1[80];
    char line2[80];
    char line3[80];
#if defined(CARPOCALYPSE2_FIX_BUGS)
    /* retail's 80 bytes cannot hold a long install path plus PATHS.TXT */
    char path_file[sizeof(tPath_name) + 16];
#else
    char path_file[80];
#endif
    FILE* fp;

    strcpy(path_file, gApplication_path);
    strcat(path_file, (char*)gDir_separator);
    strcat(path_file, "PATHS.TXT");

    if (PDCheckDriveExists(path_file)) {
        fp = PFfopen(path_file, "rt");
        if (fp == NULL) {
            return;
        }
        line1[0] = '\0';
        line2[0] = '\0';
        line3[0] = '\0';
        GetALineWithNoPossibleService(fp, line1);
        GetALineWithNoPossibleService(fp, line2);
        GetALineWithNoPossibleService(fp, line3);
        PFfclose(fp);
        if (strcmp(line3, "Full") != 0) {
            return;
        }
    }
    gCD_fully_installed = 1;
}

// FUNCTION: CARMA2_HW 0x00455080
void C2_NAKED C2_HOOK_FASTCALL InitSound(void) {
#ifndef CARPOCALYPSE2_MATCHING
    tPath_name the_path;
    int engine_channel_count;
    int car_channel_count;
    int ped_channel_count;
    int xxx_channel_count;

    if (gVirgin_pass) {
        PathCat(the_path, gApplication_path, "SOUND");
        PathCat(the_path, the_path, "SOUND.TXT");
        if (gSound_override) {
            gSound_available = gSound_enabled = 0;
        } else {
            gSound_available = gSound_enabled = S3Init(the_path, gAusterity_mode, gPed_sound_path) == 0;
        }
        S3Set3DSoundEnvironment(0.14492753f, -1.0f, -1.0f);
        gVirgin_pass = 0;
        gCD_is_disabled = 0;
        UsePathFileToDetermineIfFullInstallation();
    }
    if (!gSound_available) {
        return;
    }
    if (gSound_detail_level == 0) {
        engine_channel_count = 2;
        ped_channel_count = 3;
        car_channel_count = 2;
        xxx_channel_count = 1;
    } else if (gSound_detail_level == 1) {
        engine_channel_count = 4;
        car_channel_count = 3;
        ped_channel_count = 4;
        xxx_channel_count = 3;
    } else {
        ped_channel_count = 5;
        engine_channel_count = 6;
        car_channel_count = 4;
        xxx_channel_count = 5;
    }
    if (gDriver_outlet == NULL) {
        gIndexed_outlets[0] = gDriver_outlet = S3CreateOutlet(1, 1);
        if (gDriver_outlet == NULL) {
            gSound_available = 0;
            return;
        }
    }
    if (gMusic_outlet == NULL) {
        gIndexed_outlets[2] = gMusic_outlet = S3CreateOutlet(1, 1);
        gMusic_available = gMusic_outlet != NULL;
        DRS3SetOutletVolume(gMusic_outlet, gProgram_state.music_volume);
    }
    if (gSound_detail_level != gOld_sound_detail_level) {
        if (gXXX_outlet != NULL) {
            S3ReleaseOutlet(gXXX_outlet);
            gXXX_outlet = NULL;
        }
        if (gPedestrians_outlet != NULL) {
            S3ReleaseOutlet(gPedestrians_outlet);
            gPedestrians_outlet = NULL;
        }
        if (gEngine_outlet != NULL) {
            S3ReleaseOutlet(gEngine_outlet);
            gEngine_outlet = NULL;
        }
        if (gCar_outlet != NULL) {
            S3ReleaseOutlet(gCar_outlet);
            gCar_outlet = NULL;
        }
        if (gEngine_outlet == NULL) {
            gIndexed_outlets[1] = gEngine_outlet = S3CreateOutlet(engine_channel_count, engine_channel_count);
            if (gEngine_outlet == NULL) {
                gSound_available = 0;
                return;
            }
            DRS3SetOutletVolume(gEngine_outlet, gProgram_state.effects_volume);
        }
        if (gXXX_outlet == NULL) {
            gIndexed_outlets[3] = gXXX_outlet = S3CreateOutlet(car_channel_count, car_channel_count);
            if (gXXX_outlet == NULL) {
                gSound_available = 0;
                return;
            }
            DRS3SetOutletVolume(gXXX_outlet, gProgram_state.effects_volume);
        }
        if (gPedestrians_outlet == NULL) {
            gIndexed_outlets[4] = gPedestrians_outlet = S3CreateOutlet(ped_channel_count, ped_channel_count);
            if (gPedestrians_outlet == NULL) {
                gSound_available = 0;
                return;
            }
            DRS3SetOutletVolume(gPedestrians_outlet, gProgram_state.effects_volume);
        }
        if (gCar_outlet == NULL) {
            gIndexed_outlets[5] = gCar_outlet = S3CreateOutlet(xxx_channel_count, xxx_channel_count);
            if (gCar_outlet == NULL) {
                gSound_available = 0;
                return;
            }
            DRS3SetOutletVolume(gCar_outlet, gProgram_state.effects_volume);
        }
    }
    if (gEffects_outlet == NULL) {
        gIndexed_outlets[5] = gEffects_outlet = S3CreateOutlet(2, 2);
        if (gEffects_outlet == NULL) {
            gSound_available = 0;
            return;
        }
        DRS3SetOutletVolume(gEffects_outlet, gProgram_state.effects_volume);
    }
    gOld_sound_detail_level = gSound_detail_level;
    SetSoundVolumes(0);
#else
    __asm {
            mov eax, dword ptr [gVirgin_pass]
            sub esp, 0x244
            push ebp
            xor ebp, ebp
            push esi
            cmp eax, ebp
            push edi
            je L00455252
            push offset gRandom_CDA_tunes_2+0xf4
            mov edx, offset gApplication_path
            lea ecx, [esp + 0x154]
            call PathCat
            lea edx, [esp + 0x150]
            lea ecx, [esp + 0x150]
            push offset gRandom_CDA_tunes_2+0xe8
            call PathCat
            cmp dword ptr [gSound_override], ebp
            je L004550dc
            mov dword ptr [gSound_enabled], ebp
            mov dword ptr [gSound_available], ebp
            jmp L00455103
    L004550dc:
            mov eax, dword ptr [gPed_sound_path]
            mov edx, dword ptr [gAusterity_mode]
            push eax
            lea ecx, [esp + 0x154]
            call S3Init
            neg eax
            sbb eax, eax
            inc eax
            mov dword ptr [gSound_enabled], eax
            mov dword ptr [gSound_available], eax
    L00455103:
            push 0xbf800000
            push 0xbf800000
            push 0x3e1467e2
            call S3Set3DSoundEnvironment
            or ecx, 0xffffffff
            mov edi, offset gApplication_path
            xor eax, eax
            lea edx, [esp + 0x10]
            repne scasb
            not ecx
            sub edi, ecx
            mov dword ptr [gVirgin_pass], ebp
            mov eax, ecx
            mov esi, edi
            shr ecx, 2
            mov edi, edx
            mov dword ptr [gCD_is_disabled], ebp
            rep movsd
            mov ecx, eax
            xor eax, eax
            and ecx, 3
            lea edx, [esp + 0x10]
            rep movsb
            mov edi, dword ptr [gDir_separator]
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
            lea edx, [esp + 0x10]
            and ecx, 3
            rep movsb
            mov edi, offset gRandom_CDA_tunes_2+0x54
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
            and ecx, 3
            rep movsb
            lea ecx, [esp + 0x10]
            call PDCheckDriveExists
            test eax, eax
            je L00455678
            mov edx, offset gNext_crush_actor_id+0x180
            lea ecx, [esp + 0x10]
            call PFfopen
            mov esi, eax
            cmp esi, ebp
            je L00455252
            lea edx, [esp + 0x100]
            mov ecx, esi
            mov byte ptr [esp + 0x100], 0
            mov byte ptr [esp + 0xb0], 0
            mov byte ptr [esp + 0x60], 0
            call GetALineWithNoPossibleService
            lea edx, [esp + 0xb0]
            mov ecx, esi
            call GetALineWithNoPossibleService
            lea edx, [esp + 0x60]
            mov ecx, esi
            call GetALineWithNoPossibleService
            mov ecx, esi
            call PFfclose
            mov esi, offset gRandom_CDA_tunes_2+0x4c
            lea eax, [esp + 0x60]
    L00455219:
            mov dl, byte ptr [eax]
            mov cl, dl
            cmp dl, byte ptr [esi]
            jne L0045523d
            test cl, cl
            je L00455239
            mov dl, byte ptr [eax + 1]
            mov cl, dl
            cmp dl, byte ptr [esi + 1]
            jne L0045523d
            add eax, 2
            add esi, 2
            test cl, cl
            jne L00455219
    L00455239:
            xor eax, eax
            jmp L00455242
    L0045523d:
            sbb eax, eax
            sbb eax, -1
    L00455242:
            test eax, eax
            jne L00455250
            mov dword ptr [gCD_fully_installed], 1
    L00455250:
            xor ebp, ebp
    L00455252:
            cmp dword ptr [gSound_available], ebp
            je L0045566e
            mov eax, dword ptr [gSound_detail_level]
            cmp eax, ebp
            jne L0045527d
            mov esi, 2
            mov ebp, 3
            mov edi, esi
            mov dword ptr [esp + 0xc], 1
            jmp L004552a7
    L0045527d:
            cmp eax, 1
            jne L00455294
            mov esi, 4
            mov edi, 3
            mov ebp, esi
            mov dword ptr [esp + 0xc], edi
            jmp L004552a7
    L00455294:
            mov ebp, 5
            mov esi, 6
            mov edi, 4
            mov dword ptr [esp + 0xc], ebp
    L004552a7:
            mov eax, dword ptr [gDriver_outlet]
            test eax, eax
            jne L004552d9
            mov edx, 1
            mov ecx, edx
            call S3CreateOutlet
            test eax, eax
            mov dword ptr [gDriver_outlet], eax
            mov dword ptr [gIndexed_outlets], eax
            jne L004552d9
            mov dword ptr [gSound_available], eax
            pop edi
            pop esi
            pop ebp
            add esp, 0x244
            ret
    L004552d9:
            mov eax, dword ptr [gMusic_outlet]
            test eax, eax
            jne L0045531c
            mov edx, 1
            mov ecx, edx
            call S3CreateOutlet
            xor ecx, ecx
            mov dword ptr [gMusic_outlet], eax
            test eax, eax
            setne cl
            mov dword ptr [gMusic_available], ecx
            mov ecx, dword ptr [gSound_enabled]
            test ecx, ecx
            mov dword ptr [gIndexed_outlets+0x8], eax
            je L0045531c
            mov edx, dword ptr [gProgram_state+0x70]
            mov ecx, eax
            call S3SetOutletVolume
    L0045531c:
            mov edx, dword ptr [gSound_detail_level]
            mov eax, dword ptr [gOld_sound_detail_level]
            cmp eax, edx
            je L004554ad
            mov ecx, dword ptr [gXXX_outlet]
            test ecx, ecx
            je L00455348
            call S3ReleaseOutlet
            mov dword ptr [gXXX_outlet], 0
    L00455348:
            mov ecx, dword ptr [gPedestrians_outlet]
            test ecx, ecx
            je L00455361
            call S3ReleaseOutlet
            mov dword ptr [gPedestrians_outlet], 0
    L00455361:
            mov ecx, dword ptr [gEngine_outlet]
            test ecx, ecx
            je L0045537a
            call S3ReleaseOutlet
            mov dword ptr [gEngine_outlet], 0
    L0045537a:
            mov ecx, dword ptr [gCar_outlet]
            test ecx, ecx
            je L00455393
            call S3ReleaseOutlet
            mov dword ptr [gCar_outlet], 0
    L00455393:
            mov eax, dword ptr [gEngine_outlet]
            test eax, eax
            jne L004553d9
            mov edx, esi
            mov ecx, esi
            call S3CreateOutlet
            test eax, eax
            mov dword ptr [gEngine_outlet], eax
            mov dword ptr [gIndexed_outlets+0x4], eax
            jne L004553c2
            mov dword ptr [gSound_available], eax
            pop edi
            pop esi
            pop ebp
            add esp, 0x244
            ret
    L004553c2:
            mov ecx, dword ptr [gSound_enabled]
            test ecx, ecx
            je L004553d9
            mov edx, dword ptr [gProgram_state+0x74]
            mov ecx, eax
            call S3SetOutletVolume
    L004553d9:
            mov eax, dword ptr [gXXX_outlet]
            test eax, eax
            jne L0045541f
            mov edx, edi
            mov ecx, edi
            call S3CreateOutlet
            test eax, eax
            mov dword ptr [gXXX_outlet], eax
            mov dword ptr [gIndexed_outlets+0xc], eax
            jne L00455408
            mov dword ptr [gSound_available], eax
            pop edi
            pop esi
            pop ebp
            add esp, 0x244
            ret
    L00455408:
            mov ecx, dword ptr [gSound_enabled]
            test ecx, ecx
            je L0045541f
            mov edx, dword ptr [gProgram_state+0x74]
            mov ecx, eax
            call S3SetOutletVolume
    L0045541f:
            mov eax, dword ptr [gPedestrians_outlet]
            test eax, eax
            jne L00455465
            mov edx, ebp
            mov ecx, ebp
            call S3CreateOutlet
            test eax, eax
            mov dword ptr [gPedestrians_outlet], eax
            mov dword ptr [gIndexed_outlets+0x10], eax
            jne L0045544e
            mov dword ptr [gSound_available], eax
            pop edi
            pop esi
            pop ebp
            add esp, 0x244
            ret
    L0045544e:
            mov ecx, dword ptr [gSound_enabled]
            test ecx, ecx
            je L00455465
            mov edx, dword ptr [gProgram_state+0x74]
            mov ecx, eax
            call S3SetOutletVolume
    L00455465:
            mov eax, dword ptr [gCar_outlet]
            test eax, eax
            jne L004554ad
            mov ecx, dword ptr [esp + 0xc]
            mov edx, ecx
            call S3CreateOutlet
            test eax, eax
            mov dword ptr [gCar_outlet], eax
            mov dword ptr [gIndexed_outlets+0x14], eax
            jne L00455496
            mov dword ptr [gSound_available], eax
            pop edi
            pop esi
            pop ebp
            add esp, 0x244
            ret
    L00455496:
            mov ecx, dword ptr [gSound_enabled]
            test ecx, ecx
            je L004554ad
            mov edx, dword ptr [gProgram_state+0x74]
            mov ecx, eax
            call S3SetOutletVolume
    L004554ad:
            mov ecx, dword ptr [gEffects_outlet]
            test ecx, ecx
            jne L004554fe
            mov edx, 2
            mov ecx, edx
            call S3CreateOutlet
            mov ecx, eax
            test ecx, ecx
            mov dword ptr [gEffects_outlet], ecx
            mov dword ptr [gIndexed_outlets+0x14], ecx
            jne L004554e4
            mov dword ptr [gSound_available], eax
            pop edi
            pop esi
            pop ebp
            add esp, 0x244
            ret
    L004554e4:
            mov eax, dword ptr [gSound_enabled]
            test eax, eax
            je L004554fe
            mov edx, dword ptr [gProgram_state+0x74]
            call S3SetOutletVolume
            mov ecx, dword ptr [gEffects_outlet]
    L004554fe:
            mov eax, dword ptr [gSound_detail_level]
            mov dword ptr [gOld_sound_detail_level], eax
            mov eax, dword ptr [gSound_enabled]
            test eax, eax
            je L0045566e
            test ecx, ecx
            je L00455524
            mov edx, dword ptr [gProgram_state+0x74]
            call S3SetOutletVolume
    L00455524:
            mov eax, dword ptr [gSound_enabled]
            test eax, eax
            je L0045558c
            mov edx, dword ptr [gProgram_state+0x74]
            mov ecx, dword ptr [gXXX_outlet]
            call S3SetOutletVolume
            mov eax, dword ptr [gSound_enabled]
            test eax, eax
            je L0045558c
            mov edx, dword ptr [gProgram_state+0x74]
            mov ecx, dword ptr [gEngine_outlet]
            call S3SetOutletVolume
            mov eax, dword ptr [gSound_enabled]
            test eax, eax
            je L0045558c
            mov edx, dword ptr [gProgram_state+0x74]
            mov ecx, dword ptr [gDriver_outlet]
            call S3SetOutletVolume
            mov eax, dword ptr [gSound_enabled]
            test eax, eax
            je L0045558c
            mov edx, dword ptr [gProgram_state+0x74]
            mov ecx, dword ptr [gPedestrians_outlet]
            call S3SetOutletVolume
    L0045558c:
            mov eax, dword ptr [gINT_00595c44]
            test eax, eax
            je L00455654
            mov ecx, dword ptr [gProgram_state+0x70]
            push ecx
            push offset gRandom_CDA_tunes_2+0xcc
            call dr_dprintf
            mov eax, dword ptr [gCDA_started_playing]
            xor esi, esi
            add esp, 8
            cmp eax, esi
            jne L004555cf
            push offset gRandom_CDA_tunes_2+0x98
            call dr_dprintf
            add esp, 4
            pop edi
            pop esi
            pop ebp
            add esp, 0x244
            ret
    L004555cf:
            cmp dword ptr [gProgram_state+0x70], 0x80
            jge L00455614
            cmp dword ptr [gCD_fully_installed], esi
            je L0045566e
            cmp dword ptr [gMusic_available], esi
            je L0045566e
            mov ecx, dword ptr [gINT_00684568]
            cmp ecx, esi
            je L0045566e
            call S3StopSound
            mov dword ptr [gINT_00684554], esi
            mov dword ptr [gINT_00684568], esi
            pop edi
            pop esi
            pop ebp
            add esp, 0x244
            ret
    L00455614:
            call S3IsCDAPlaying
            test eax, eax
            jne L0045566e
            call PDGetTotalTime
            add eax, 0x2710
            push eax
            push offset gRandom_CDA_tunes_2+0x60
            mov dword ptr [gNext_track_finished_check], eax
            call dr_dprintf
            mov ecx, dword ptr [gINT_00595c20]
            add esp, 8
            call DRS3StartCDA
            mov dword ptr [gINT_00684568], eax
            pop edi
            pop esi
            pop ebp
            add esp, 0x244
            ret
    L00455654:
            mov eax, dword ptr [gSound_enabled]
            test eax, eax
            je L0045566e
            mov edx, dword ptr [gProgram_state+0x70]
            mov ecx, dword ptr [gMusic_outlet]
            call S3SetOutletVolume
    L0045566e:
            pop edi
            pop esi
            pop ebp
            add esp, 0x244
            ret
    L00455678:
            mov dword ptr [gCD_fully_installed], 1
            jmp L00455252
    }
#endif
}

// FUNCTION: CARMA2_HW 0x00455690
tS3_sound_tag C2_HOOK_FASTCALL DRS3StartSound(tS3_outlet* pOutlet, tS3_sound_id pSound) {
    if (gSound_enabled) {
        if (pSound != eSoundId_pratcam_whirr_noise && !(pSound >= 3000 && pSound <= 3007) && !(pSound >= 5300 && pSound <= 5320)) {
            PipeSingleSound(pOutlet, pSound, 0, 0, -1, NULL);
        }
        return S3StartSound(pOutlet, pSound);
    } else {
        return 0;
    }
}

// FUNCTION: CARMA2_HW 0x004556f0
int C2_HOOK_FASTCALL DRS3StartSoundNoPiping(tS3_outlet* pOutlet, tS3_sound_id pSound) {
    if (gSound_enabled) {
        return S3StartSound(pOutlet, pSound);
    } else {
        return 0;
    }
}

// FUNCTION: CARMA2_HW 0x00455710
int C2_HOOK_FASTCALL DRS3StartSound2(tS3_outlet *pOutlet, tS3_sound_id pSound, int pRepeats, int pLeft_volume, int pRight_volume, int pLeft_pitch, int pRight_pitch) {
    if (gSound_enabled) {
        if (pOutlet != gMusic_outlet
                && pSound != 1000
                && !(pSound >= 3000 && pSound <= 3007)
                && !(pSound >= 5300 && pSound <= 5320)
                && (pLeft_volume != 0 || pRight_volume != 0)) {
            PipeSingleSound(pOutlet, pSound, pLeft_volume, pRight_volume, pLeft_pitch, NULL);
        }
        return S3StartSound2(pOutlet, pSound, pRepeats, pLeft_volume, pRight_volume, pLeft_pitch, pRight_pitch);
    } else {
        return 0;
    }
}

// FUNCTION: CARMA2_HW 0x004557a0
int C2_HOOK_FASTCALL DRS3ChangeVolume(int pChannel_id, int pVolume) {
    if (gSound_enabled) {
        return S3SetChannelVolume(pChannel_id, pVolume);
    } else {
        return 0;
    }
}

// FUNCTION: CARMA2_HW 0x00455840
int C2_HOOK_FASTCALL DRS3StopSound(tS3_sound_tag pSound_tag) {
    if (gSound_enabled) {
        return S3StopSound(pSound_tag);
    } else {
        return 0;
    }
}

// FUNCTION: CARMA2_HW 0x004558a0
void C2_HOOK_FASTCALL DRS3Service(void) {
    if (gSound_enabled) {
        S3Service(gProgram_state.cockpit_on && gProgram_state.cockpit_image_index >= 0, 1);
    }
}

// FUNCTION: CARMA2_HW 0x00455860
int C2_HOOK_FASTCALL DRS3LoadSample(int pSample_id) {
    if (gSound_enabled) {
        return S3LoadSample(pSample_id);
    } else {
        return 0;
    }
}

// FUNCTION: CARMA2_HW 0x004558f0
int C2_HOOK_FASTCALL DRS3SoundStillPlaying(int pTag) {
    if (gSound_enabled) {
        return S3SoundStillPlaying(pTag);
    } else {
        return 0;
    }
}

// FUNCTION: CARMA2_HW 0x00455910
void C2_HOOK_FASTCALL DRS3ShutDown(void) {
    if (gSound_enabled) {
        gSound_enabled = 0;
        S3DisableSound();
    }
}

// FUNCTION: CARMA2_HW 0x00455930
int C2_HOOK_FASTCALL DRS3SetOutletVolume(tS3_outlet* pOutlet, int pVolume) {
    if (gSound_enabled) {
        return S3SetOutletVolume(pOutlet, pVolume);
    }
    return 0;
}

// FUNCTION: CARMA2_HW 0x00455970
int C2_HOOK_FASTCALL DRS3StopOutletSound(tS3_outlet* pOutlet) {
    if (gSound_enabled) {
        return S3StopOutletSound(pOutlet);
    }
    return 0;
}

// FUNCTION: CARMA2_HW 0x00455990
int C2_NAKED C2_HOOK_FASTCALL DRS3StopAllOutletSounds(void) {
#ifndef CARPOCALYPSE2_MATCHING
    int result;

    result = 0;
    if (gSound_enabled) {
        S3StopAllOutletSounds();
        if (gSound_enabled) {
            if (gProgram_state.cockpit_on && gProgram_state.cockpit_image_index >= 0) {
                int inside_cockpit;

                inside_cockpit = 1;
                S3Service(inside_cockpit, inside_cockpit);
            } else {
                S3Service(0, 1);
            }
        }
    }
    return result;
#else
    __asm {
            mov eax, dword ptr [gSound_enabled]
            test eax, eax
            je L004559dc
            push esi
            call S3StopAllOutletSounds
            mov esi, eax
            mov eax, dword ptr [gSound_enabled]
            test eax, eax
            je L004559d8
            mov eax, dword ptr [gProgram_state+0x4c]
            test eax, eax
            je L004559cc
            mov eax, dword ptr [gProgram_state+0x50]
            test eax, eax
            jl L004559cc
            mov ecx, 1
            mov edx, ecx
            call S3Service
            mov eax, esi
            pop esi
            ret
    L004559cc:
            xor ecx, ecx
            mov edx, 1
            call S3Service
    L004559d8:
            mov eax, esi
            pop esi
            ret
    L004559dc:
            xor eax, eax
            ret
            nop
    }
#endif
}

// FUNCTION: CARMA2_HW 0x004559e0
int C2_HOOK_FASTCALL DRS3StopAllOutletSoundsExceptCDA(void) {
    if (gSound_enabled) {
        DRS3StopOutletSound(gXXX_outlet);
        DRS3StopOutletSound(gEngine_outlet);
        DRS3StopOutletSound(gCar_outlet);
        DRS3StopOutletSound(gEffects_outlet);
        DRS3StopOutletSound(gPedestrians_outlet);
        return 1;
    } else {
        return 0;
    }
}

// FUNCTION: CARMA2_HW 0x00455a50
void C2_HOOK_FASTCALL ToggleSoundEnable(void) {
    if (gSound_enabled) {
        S3StopAllOutletSounds();
        S3Disable();
        gSound_enabled = 0;
    } else {
        S3Enable();
        gSound_enabled = 1;
    }
}

// FUNCTION: CARMA2_HW 0x00455a80
void C2_HOOK_FASTCALL SoundService(void) {

    if (!gSound_enabled) {
        return;
    }
    if (gServicing_sound) {
        return;
    }
    gServicing_sound = 1;
    gLast_sound_service = PDGetTotalTime();
    if (gCDA_started_playing) {
        tU32 now = PDGetTotalTime();
        if (now > gNext_track_finished_check && !S3IsCDAPlaying()) {
            dr_dprintf("CDINFO: SoundService(): Stopping & starting CD, time %d, gNext_track_finished_check %d",
                now, gNext_track_finished_check);
            StopMusic();
            StartMusic();
        }
    }
    DRS3Service();
    gServicing_sound = 0;
}

// FUNCTION: CARMA2_HW 0x00455bb0
void C2_NAKED C2_HOOK_FASTCALL InitSoundSources(void) {
#ifndef CARPOCALYPSE2_MATCHING
    int toggle;
    int cat;
    int car_count;
    tCar_spec* car;
    tEnvironment_sound_source* env_src;
    int i;

    if (!gSound_available) {
        return;
    }

    if (!gSound_enabled) {
        toggle = 1;
        ToggleSoundEnable();
    } else {
        toggle = 0;
    }
    BrVector3Scale(&gCamera_left, (br_vector3*)gCamera_to_world.m[0], -1.0f);
    BrVector3Copy(&gCamera_position, (br_vector3*)gCamera_to_world.m[3]);
    S3BindListenerPositionBRender(&gCamera_position);
    S3BindListenerVelocityBRender(&gCamera_velocity);
    S3BindListenerLeftBRender(&gCamera_left);
    if (!gSound_sources_inited) {

        for (cat = eVehicle_rozzer; cat >= eVehicle_self; cat -= 1) {

            if (cat == eVehicle_self) {
                car_count = 1;
            } else {
                car_count = GetCarCount(cat);
            }
            for (i = 0; i < car_count; i++) {

                PossibleService();
                if (cat == eVehicle_self) {
                    car = &gProgram_state.current_car;
                } else {
                    car = GetCarSpec(cat, i);
                }

                if ((car != NULL && car->driver == eDriver_local_human) || gSound_detail_level >= 3 || cat == eVehicle_rozzer) {

                    car->sound_source = S3CreateSoundSourceBR(&car->pos, &car->vel, gEngine_outlet);
                    if (car->sound_source != NULL) {
                        S3BindAmbientSoundToOutlet(gEngine_outlet, cat == eVehicle_rozzer ? eSoundId_Cop_Siren : car->engine_noises[0], car->sound_source, 250.0f, 0, 0, 0, BR_FIXED_INT(1), BR_FIXED_INT(1));
                    }
                }
            }
        }

        for (i = 0; i < (int)CARPOCALYPSE2_ASIZE(gEnvironment_sound_sources); i++) {

            env_src = &gEnvironment_sound_sources[i];
            env_src->field_0x10 = NULL;
            if (env_src->source == NULL) {
                env_src->source = S3CreateSoundSourceBR(&env_src->position, &gZero_v__car, gCar_outlet);
                if (env_src->source != NULL) {
                    S3BindAmbientSoundToOutlet(gCar_outlet, eSoundId_Cop_Siren, env_src->source, 100.f, 0, 0, 0, BR_FIXED_INT(1), BR_FIXED_INT(1));
                }
            }
        }
        gSound_sources_inited = 1;
    }
    if (toggle) {
        ToggleSoundEnable();
    }

    gCount_environmental_sound_sources = 0;
    gEnvironmental_sound_sources_buffer_index = 0;
    gNext_sound_generator_munging = 0;
#else
    __asm {
            push ecx
            mov eax, dword ptr [gSound_available]
            mov dword ptr [esp], 0
            test eax, eax
            je L00455dda
            mov eax, dword ptr [gSound_enabled]
            push edi
            push esi
            push ebp
            test eax, eax
            push ebx
            jne L00455be6
            call S3Enable
            mov eax, 1
            mov dword ptr [gSound_enabled], eax
            mov dword ptr [esp + 0x10], eax
    L00455be6:
            fld dword ptr [gCamera_to_world]
            fld dword ptr [gCamera_to_world+0x4]
            fld dword ptr [gCamera_to_world+0x8]
            fxch st(2)
            fmul dword ptr [gDrone_select_max_distance_squared+0xbc]
            fxch st(1)
            fmul dword ptr [gDrone_select_max_distance_squared+0xbc]
            fld dword ptr [gCamera_to_world+0x28]
            fxch st(3)
            fmul dword ptr [gDrone_select_max_distance_squared+0xbc]
            fxch st(3)
            fstp dword ptr [gCamera_position+0x4]
            mov ecx, dword ptr [gCamera_to_world+0x2c]
            mov eax, dword ptr [gCamera_to_world+0x24]
            fxch st(1)
            fstp dword ptr [gCamera_left]
            mov dword ptr [gCamera_position+0x8], ecx
            mov ecx, offset gCamera_position
            fstp dword ptr [gCamera_left+0x4]
            mov dword ptr [gCamera_position], eax
            fstp dword ptr [gCamera_left+0x8]
            call S3BindListenerPositionBRender
            mov ecx, offset gCamera_velocity
            call S3BindListenerVelocityBRender
            mov ecx, offset gCamera_left
            call S3BindListenerLeftBRender
            mov eax, dword ptr [gSound_sources_inited]
            test eax, eax
            jne L00455d91
            mov edi, 3
    L00455c78:
            test edi, edi
            jne L00455c83
            mov ebp, 1
            jmp L00455c8c
    L00455c83:
            mov ecx, edi
            call GetCarCount
            mov ebp, eax
    L00455c8c:
            xor ebx, ebx
            test ebp, ebp
            jle L00455d24
    L00455c96:
            call PossibleService
            test edi, edi
            jne L00455ca6
            mov esi, offset gProgram_state+0xac
            jmp L00455cb1
    L00455ca6:
            mov edx, ebx
            mov ecx, edi
            call GetCarSpec
            mov esi, eax
    L00455cb1:
            test esi, esi
            je L00455cbb
            cmp dword ptr [esi + 0xc], 8
            je L00455cc9
    L00455cbb:
            cmp dword ptr [gSound_detail_level], 3
            jge L00455cc9
            cmp edi, 3
            jne L00455d1b
    L00455cc9:
            mov edx, dword ptr [gEngine_outlet]
            lea ecx, [esi + 0x4c]
            push edx
            lea edx, [esi + 0x13cc]
            call S3CreateSoundSourceBR
            test eax, eax
            mov dword ptr [esi + 0x1370], eax
            je L00455d1b
            cmp edi, 3
            je L00455cf5
            mov edx, dword ptr [esi + 0x470]
            jmp L00455cfa
    L00455cf5:
            mov edx, 0x14e6
    L00455cfa:
            mov ecx, dword ptr [gEngine_outlet]
            push 0x10000
            push 0x10000
            push 0
            push 0
            push 0
            push 0x437a0000
            push eax
            call S3BindAmbientSoundToOutlet
    L00455d1b:
            inc ebx
            cmp ebx, ebp
            jl L00455c96
    L00455d24:
            dec edi
            jns L00455c78
            mov esi, offset gEnvironment_sound_sources
    L00455d30:
            lea ecx, [esi + 4]
            mov dword ptr [ecx + 0xc], 0
            mov eax, dword ptr [esi]
            test eax, eax
            jne L00455d7c
            mov eax, dword ptr [gCar_outlet]
            mov edx, offset gZero_v__car
            push eax
            call S3CreateSoundSourceBR
            test eax, eax
            mov dword ptr [esi], eax
            je L00455d7c
            mov ecx, dword ptr [gCar_outlet]
            push 0x10000
            push 0x10000
            push 0
            push 0
            push 0
            push 0x42c80000
            push eax
            mov edx, 0x14e6
            call S3BindAmbientSoundToOutlet
    L00455d7c:
            add esi, 0x1c
            cmp esi, offset gEffects_outlet
            jl L00455d30
            mov dword ptr [gSound_sources_inited], 1
    L00455d91:
            mov eax, dword ptr [esp + 0x10]
            xor esi, esi
            cmp eax, esi
            je L00455dc4
            cmp dword ptr [gSound_enabled], esi
            je L00455db5
            call S3StopAllOutletSounds
            call S3Disable
            mov dword ptr [gSound_enabled], esi
            jmp L00455dc4
    L00455db5:
            call S3Enable
            mov dword ptr [gSound_enabled], 1
    L00455dc4:
            pop ebx
            mov dword ptr [gCount_environmental_sound_sources], esi
            mov dword ptr [gEnvironmental_sound_sources_buffer_index], esi
            mov dword ptr [gNext_sound_generator_munging], esi
            pop ebp
            pop esi
            pop edi
    L00455dda:
            pop ecx
            ret
            nop
            nop
            nop
            nop
    }
#endif
}

// FUNCTION: CARMA2_HW 0x00455de0
void C2_NAKED C2_HOOK_FASTCALL DisposeSoundSources(void) {
#ifndef CARPOCALYPSE2_MATCHING
    int cat;
    int car_count;
    int i;
    int toggle;
    tCar_spec* the_car;

    if (gSound_available) {
        if (gSound_enabled) {
            toggle = 0;
        } else {
            toggle = 1;
            ToggleSoundEnable();
        }
        if (gSound_sources_inited) {
            DRS3StopOutletSound(gIndexed_outlets[0]);
            S3Service(gProgram_state.cockpit_on && gProgram_state.cockpit_image_index >= 0, 0);

            for (cat = eVehicle_rozzer; cat >= eVehicle_self; cat--) {
                if (cat == eVehicle_self) {
                    car_count = 1;
                } else {
                    car_count = GetCarCount(cat);
                }
                for (i = 0; i < car_count; i++) {
                    if (cat == eVehicle_self) {
                        the_car = &gProgram_state.current_car;
                    } else {
                        the_car = GetCarSpec(cat, i);
                    }
                    if ((the_car != NULL && the_car->driver == eDriver_local_human) || gSound_detail_level == 3 || cat == eVehicle_rozzer) {
                        if (the_car->sound_source != NULL) {
                            S3UpdateSoundSource(gIndexed_outlets[0], -1, the_car->sound_source, 0.0f, 0, 0, 0, 0x10000, 0x10000);
                            S3ReleaseSoundSource(the_car->sound_source);
                        }
                        the_car->sound_source = NULL;
                    }
                }
                // FIXME: move outside of car category loop?
                for (i = 0; i < (int)CARPOCALYPSE2_ASIZE(gEnvironment_sound_sources); i++) {
                    if (gEnvironment_sound_sources[i].source != NULL) {
                        S3BindAmbientSoundToOutlet(gIndexed_outlets[0], -1, gEnvironment_sound_sources[i].source, 100.0f, 0, 0, 0, BR_FIXED_INT(1), BR_FIXED_INT(1));
                    }
                }
            }
            gSound_sources_inited = 0;
        }
        if (toggle) {
            ToggleSoundEnable();
        }
    }
#else
    __asm {
            push ecx
            mov eax, dword ptr [gSound_available]
            push ebx
            push ebp
            push esi
            test eax, eax
            push edi
            mov dword ptr [esp + 0x10], 0
            je L00455f6c
            mov eax, dword ptr [gSound_enabled]
            test eax, eax
            jne L00455e16
            call S3Enable
            mov eax, 1
            mov dword ptr [gSound_enabled], eax
            mov dword ptr [esp + 0x10], eax
    L00455e16:
            mov ecx, dword ptr [gSound_sources_inited]
            test ecx, ecx
            je L00455f37
            mov ecx, dword ptr [gEngine_outlet]
            test eax, eax
            je L00455e33
            call S3StopOutletSound
    L00455e33:
            mov eax, dword ptr [gProgram_state+0x4c]
            test eax, eax
            je L00455e4c
            mov eax, dword ptr [gProgram_state+0x50]
            test eax, eax
            jl L00455e4c
            mov ecx, 1
            jmp L00455e4e
    L00455e4c:
            xor ecx, ecx
    L00455e4e:
            xor edx, edx
            call S3Service
            mov ebx, 3
    L00455e5a:
            test ebx, ebx
            jne L00455e65
            mov ebp, 1
            jmp L00455e6e
    L00455e65:
            mov ecx, ebx
            call GetCarCount
            mov ebp, eax
    L00455e6e:
            xor edi, edi
            test ebp, ebp
            jle L00455ee7
    L00455e74:
            test ebx, ebx
            jne L00455e7f
            mov esi, offset gProgram_state+0xac
            jmp L00455e8a
    L00455e7f:
            mov edx, edi
            mov ecx, ebx
            call GetCarSpec
            mov esi, eax
    L00455e8a:
            test esi, esi
            je L00455e94
            cmp dword ptr [esi + 0xc], 8
            je L00455ea2
    L00455e94:
            cmp dword ptr [gSound_detail_level], 3
            je L00455ea2
            cmp ebx, 3
            jne L00455ee2
    L00455ea2:
            mov eax, dword ptr [esi + 0x1370]
            test eax, eax
            je L00455ed8
            mov ecx, dword ptr [gEngine_outlet]
            push 0x10000
            push 0x10000
            push 0
            push 0
            push 0
            push 0
            push eax
            or edx, 0xffffffff
            call S3UpdateSoundSource
            mov ecx, dword ptr [esi + 0x1370]
            call S3ReleaseSoundSource
    L00455ed8:
            mov dword ptr [esi + 0x1370], 0
    L00455ee2:
            inc edi
            cmp edi, ebp
            jl L00455e74
    L00455ee7:
            mov esi, offset gEnvironment_sound_sources
    L00455eec:
            mov eax, dword ptr [esi]
            test eax, eax
            je L00455f16
            mov ecx, dword ptr [gCar_outlet]
            push 0x10000
            push 0x10000
            push 0
            push 0
            push 0
            push 0x42c80000
            push eax
            or edx, 0xffffffff
            call S3BindAmbientSoundToOutlet
    L00455f16:
            add esi, 0x1c
            cmp esi, offset gEffects_outlet
            jl L00455eec
            dec ebx
            jns L00455e5a
            mov eax, dword ptr [gSound_enabled]
            mov dword ptr [gSound_sources_inited], 0
    L00455f37:
            mov ecx, dword ptr [esp + 0x10]
            test ecx, ecx
            je L00455f6c
            test eax, eax
            je L00455f5d
            call S3StopAllOutletSounds
            call S3Disable
            mov dword ptr [gSound_enabled], 0
            pop edi
            pop esi
            pop ebp
            pop ebx
            pop ecx
            ret
    L00455f5d:
            call S3Enable
            mov dword ptr [gSound_enabled], 1
    L00455f6c:
            pop edi
            pop esi
            pop ebp
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
            nop
            nop
            nop
            nop
            nop
    }
#endif
}

// FUNCTION: CARMA2_HW 0x00455f80
int C2_HOOK_FASTCALL DRS3StartSound3D(tS3_outlet* pOutlet, int pSound_id, const br_vector3* pInitial_position, const br_vector3* pInitial_velocity, int pRepeats, int pVolume, int pPitch, int pSpeed) {
    if (gSound_enabled) {
        if (pVolume != 0
                && pSound_id != eSoundId_pratcam_whirr_noise
                && !(pSound_id >= 3000 && pSound_id <= 3007)
                && !(pSound_id >= 5300 && pSound_id <= 5320)) {
            PipeSingleSound(pOutlet, pSound_id, pVolume, 0, pPitch, pInitial_position);
                }
        return S3StartSound3D(pOutlet, pSound_id, pInitial_position, pInitial_velocity, pRepeats, pVolume, pPitch, pSpeed);
    } else {
        return 0;
    }
}

// FUNCTION: CARMA2_HW 0x00456010
tS3_sound_tag C2_HOOK_FASTCALL DRS3StartSoundFromSource3(tS3_sound_source* pSource, tS3_sound_id pSound, tS3_repeats pRepeats, tS3_volume pVolume, tS3_pitch pPitch, tS3_speed pSpeed) {
    if (gSound_enabled) {
        return S3StartSoundFromSource3(pSound, pSource, pRepeats, pVolume, pPitch, pSpeed);
    }
    return 0;
}

// FUNCTION: CARMA2_HW 0x00456070
void C2_NAKED C2_HOOK_FASTCALL MungeEngineNoise(void) {
#ifndef CARPOCALYPSE2_MATCHING
    int cat;
    int car_count;
    tCar_spec* the_car;
    float pitch;
    float vol;
    int type_of_engine_noise;
    int i;

    if (!gSound_available || !gProgram_state.racing) {
        return;
    }
    gCamera_left.v[0] = (br_scalar)(gCamera_to_world.m[0][0] * -1.0f);
    gCamera_left.v[1] = (br_scalar)(gCamera_to_world.m[0][1] * -1.0f);
    gCamera_left.v[2] = (br_scalar)(gCamera_to_world.m[0][2] * -1.0f);
    BrVector3Copy(&gCamera_position, (br_vector3*)gCamera_to_world.m[3]);
    if (gFrame_period != 0) {
        gCamera_velocity.v[0] = (br_scalar)((double)(gCamera_position.v[0] - gOld_camera_position.v[0]) / ((double)gFrame_period / 1000.0));
        gCamera_velocity.v[1] = (br_scalar)((double)(gCamera_position.v[1] - gOld_camera_position.v[1]) / ((double)gFrame_period / 1000.0));
        gCamera_velocity.v[2] = (br_scalar)((double)(gCamera_position.v[2] - gOld_camera_position.v[2]) / ((double)gFrame_period / 1000.0));
    } else {
        BrVector3Set(&gCamera_velocity, 0.0f, 0.0f, 0.0f);
    }
    BrVector3Copy(&gOld_camera_position, &gCamera_position);
    if (gAction_replay_mode && fabs(ARGetReplayRate()) <= 1.0) {
        ARGetReplayRate();
    }
    for (cat = eVehicle_rozzer; cat >= 0; cat--) {

        if (cat == eVehicle_self) {
            car_count = 1;
        } else {
            car_count = GetCarCount(cat);
        }
        for (i = 0; i < car_count; i++) {

            if (cat == eVehicle_self) {
                the_car = &gProgram_state.current_car;
            } else {
                the_car = GetCarSpec(cat, i);
            }
            if ((the_car != NULL && the_car->driver == eDriver_local_human)
                    || gSound_detail_level >= 3 || cat == eVehicle_rozzer) {

                BrVector3Copy(&the_car->vel, &the_car->collision_info->v);
                if (cat == eVehicle_rozzer) {
                    vol = 255.0f;
                    pitch = (float)BR_FIXED_INT(1);
                } else {
                    if (the_car->collision_info->last_special_volume != NULL) {
                        type_of_engine_noise = the_car->collision_info->last_special_volume->engine_noise_index;
                    } else {
                        type_of_engine_noise = 0;
                    }
                    pitch = (float)(40960.0 + 10.0 * the_car->revs);
                    if (gAction_replay_mode) {
                        pitch = (float)(int)(fabs((double)ARGetReplayRate()) * pitch);
                    }
                    if (type_of_engine_noise == 1) {
                        pitch = (float)(pitch * 0.75);
                    } else if (type_of_engine_noise == 2) {
                        pitch = (float)(pitch * 0.55);
                    }

                    if (pitch < 4096.0f) {
                        pitch = 4096.0f;
                    }
                    if (pitch > 131072.0f) {
                        pitch = 131072.0f;
                    }

                    vol = (float)(96.0 + (double)the_car->revs * 0.0015);
                    if (type_of_engine_noise == 1) {
                        vol = (float)(vol * 5.0);
                    } else if (type_of_engine_noise == 2) {
                        vol = (float)(vol * 2.0);
                    } else {
                        vol *= 2.5;
                    }
                    vol = MIN(vol, 255);
                }
                if (the_car->field_0x4d4 > 1.0) {
                    vol = (float)(vol * (1.0 + (the_car->field_0x4d4 - 1.0f) / 12.0f));
                }
                S3UpdateSoundSource(gEngine_outlet, -1, the_car->sound_source, (float)(gAction_replay_mode ? 300 : 250), 0, 0, (int)vol, (int)pitch, BR_FIXED_INT(1));
            }
        }
    }
    SoundService();
#else
    __asm {
            mov eax, dword ptr [gSound_available]
            sub esp, 0x14
            test eax, eax
            push ebx
            push ebp
            push esi
            push edi
            je L0045651d
            mov eax, dword ptr [gProgram_state+0x28]
            test eax, eax
            je L0045651d
            fld dword ptr [gCamera_to_world]
            fld dword ptr [gCamera_to_world+0x4]
            fld dword ptr [gCamera_to_world+0x8]
            fld dword ptr [gCamera_to_world+0x24]
            fld dword ptr [gCamera_to_world+0x28]
            fld dword ptr [gCamera_to_world+0x2c]
            fxch st(5)
            fmul dword ptr [gDrone_select_max_distance_squared+0xbc]
            mov eax, dword ptr [gCamera_to_world+0x24]
            mov ecx, dword ptr [gCamera_to_world+0x28]
            mov edx, dword ptr [gCamera_to_world+0x2c]
            mov dword ptr [gCamera_position], eax
            mov eax, dword ptr [gFrame_period]
            mov dword ptr [gCamera_position+0x4], ecx
            fstp dword ptr [gCamera_left]
            fxch st(3)
            fmul dword ptr [gDrone_select_max_distance_squared+0xbc]
            fxch st(1)
            fsub dword ptr [gOld_camera_position]
            fxch st(2)
            fmul dword ptr [gDrone_select_max_distance_squared+0xbc]
            fxch st(3)
            fsub dword ptr [gOld_camera_position+0x4]
            fxch st(4)
            fsub dword ptr [gOld_camera_position+0x8]
            fxch st(1)
            fstp dword ptr [gCamera_left+0x4]
            fxch st(2)
            fstp dword ptr [gCamera_left+0x8]
            test eax, eax
            fxch st(2)
            fxch st(1)
            mov dword ptr [gCamera_position+0x8], edx
            je L0045616c
            mov dword ptr [esp + 0x1c], eax
            mov dword ptr [esp + 0x20], 0
            fild qword ptr [esp + 0x1c]
            fmul qword ptr [gDrone_select_max_distance_squared+0xc4]
            fld st(1)
            fxch st(4)
            fdiv st(0), st(1)
            fxch st(3)
            fdiv st(0), st(1)
            fxch st(4)
            fdiv st(0), st(1)
            fxch st(3)
            fstp dword ptr [gCamera_velocity]
            fxch st(3)
            fstp dword ptr [gCamera_velocity+0x4]
            fxch st(1)
            fstp dword ptr [gCamera_velocity+0x8]
            fstp st(0)
            fstp st(0)
            jmp L00456190
    L0045616c:
            fstp st(0)
            fstp st(0)
            fstp st(0)
            mov dword ptr [gCamera_velocity], 0
            mov dword ptr [gCamera_velocity+0x4], 0
            mov dword ptr [gCamera_velocity+0x8], 0
    L00456190:
            mov eax, dword ptr [gCamera_to_world+0x24]
            mov ecx, dword ptr [gCamera_to_world+0x28]
            mov edx, dword ptr [gCamera_to_world+0x2c]
            mov dword ptr [gOld_camera_position], eax
            mov eax, dword ptr [gPed_676914]
            mov dword ptr [gOld_camera_position+0x4], ecx
            test eax, eax
            mov dword ptr [gOld_camera_position+0x8], edx
            je L004561d6
            call ARGetReplayRate
            fabs
            fcomp qword ptr [gDOUBLE_00589ea8]
            fnstsw ax
            test ah, 0x41
            je L004561d6
            call ARGetReplayRate
            fstp st(0)
    L004561d6:
            mov ebp, 3
    L004561db:
            test ebp, ebp
            jne L004561e9
            mov dword ptr [esp + 0x14], 1
            jmp L004561f4
    L004561e9:
            mov ecx, ebp
            call GetCarCount
            mov dword ptr [esp + 0x14], eax
    L004561f4:
            mov eax, dword ptr [esp + 0x14]
            xor esi, esi
            test eax, eax
            mov dword ptr [esp + 0x18], esi
            jle L004563ee
    L00456206:
            test ebp, ebp
            jne L00456211
            mov edi, offset gProgram_state+0xac
            jmp L0045621c
    L00456211:
            mov edx, esi
            mov ecx, ebp
            call GetCarSpec
            mov edi, eax
    L0045621c:
            test edi, edi
            je L00456226
            cmp dword ptr [edi + 0xc], 8
            je L00456238
    L00456226:
            cmp dword ptr [gSound_detail_level], 3
            jge L00456238
            cmp ebp, 3
            jne L004563dd
    L00456238:
            mov eax, dword ptr [edi + 8]
            cmp ebp, 3
            mov ecx, dword ptr [eax + 0x68]
            mov dword ptr [edi + 0x13cc], ecx
            mov edx, dword ptr [eax + 0x6c]
            mov dword ptr [edi + 0x13d0], edx
            mov ecx, dword ptr [eax + 0x70]
            mov dword ptr [edi + 0x13d4], ecx
            jne L00456274
            fld dword ptr [gDOUBLE_00589ea8+0x8]
            mov esi, dword ptr [gPed_676914]
            mov dword ptr [esp + 0x10], 0x47800000
            jmp L00456363
    L00456274:
            mov eax, dword ptr [eax + 0x258]
            test eax, eax
            je L00456286
            mov ebx, dword ptr [eax + 0xa8]
            jmp L00456288
    L00456286:
            xor ebx, ebx
    L00456288:
            fld dword ptr [edi + 0x1340]
            fmul qword ptr [gDOUBLE_00589ea8+0x10]
            mov esi, dword ptr [gPed_676914]
            test esi, esi
            fsubr qword ptr [gDOUBLE_00589ea8+0x18]
            fstp dword ptr [esp + 0x10]
            je L004562ca
            call ARGetReplayRate
            fabs
            fmul dword ptr [esp + 0x10]
            call _ftol
            mov esi, dword ptr [gPed_676914]
            mov dword ptr [esp + 0x10], eax
            fild dword ptr [esp + 0x10]
            fstp dword ptr [esp + 0x10]
    L004562ca:
            cmp ebx, 1
            jne L004562db
            fld dword ptr [esp + 0x10]
            fmul qword ptr [gDOUBLE_00589ea8+0x20]
            jmp L004562ea
    L004562db:
            cmp ebx, 2
            jne L004562ee
            fld dword ptr [esp + 0x10]
            fmul qword ptr [gDOUBLE_00589ea8+0x28]
    L004562ea:
            fstp dword ptr [esp + 0x10]
    L004562ee:
            fld dword ptr [esp + 0x10]
            fcomp dword ptr [gDOUBLE_00589ea8+0x30]
            fnstsw ax
            test ah, 1
            je L00456307
            mov dword ptr [esp + 0x10], 0x45800000
    L00456307:
            fld dword ptr [esp + 0x10]
            fcomp dword ptr [gDOUBLE_00589ea8+0x34]
            fnstsw ax
            test ah, 0x41
            jne L00456320
            mov dword ptr [esp + 0x10], 0x48000000
    L00456320:
            fld dword ptr [edi + 0x1340]
            fmul qword ptr [gDOUBLE_00589ea8+0x38]
            cmp ebx, 1
            fsubr qword ptr [gDOUBLE_00589ea8+0x40]
            jne L0045633f
            fmul qword ptr [gDOUBLE_00589ea8+0x48]
            jmp L0045634e
    L0045633f:
            cmp ebx, 2
            jne L00456348
            fadd st(0), st(0)
            jmp L0045634e
    L00456348:
            fmul qword ptr [gDOUBLE_00589ea8+0x58]
    L0045634e:
            fcom dword ptr [gDOUBLE_00589ea8+0x8]
            fnstsw ax
            test ah, 1
            jne L00456363
            fstp st(0)
            fld dword ptr [gDOUBLE_00589ea8+0x8]
    L00456363:
            fld dword ptr [edi + 0x4d4]
            fcomp qword ptr [gDOUBLE_00589ea8]
            fnstsw ax
            test ah, 0x41
            jne L00456392
            fld dword ptr [edi + 0x4d4]
            fsub dword ptr [gDOUBLE_00589ea8+0x60]
            fmul dword ptr [gDOUBLE_00589ea8+0x64]
            fsubr qword ptr [gDOUBLE_00589ea8]
            fxch st(1)
            fmulp st(1), st(0)
    L00456392:
            fld dword ptr [esp + 0x10]
            push 0x10000
            call _ftol
            push eax
            call _ftol
            neg esi
            sbb esi, esi
            mov edx, dword ptr [edi + 0x1370]
            and esi, 0x32
            push eax
            add esi, 0xfa
            push 0
            mov dword ptr [esp + 0x2c], esi
            push 0
            fild dword ptr [esp + 0x30]
            push ecx
            mov ecx, dword ptr [gEngine_outlet]
            _emit 0xd9
        _emit 0x1c
        _emit 0x24
            push edx
            or edx, 0xffffffff
            call S3UpdateSoundSource
            mov esi, dword ptr [esp + 0x18]
    L004563dd:
            mov eax, dword ptr [esp + 0x14]
            inc esi
            cmp esi, eax
            mov dword ptr [esp + 0x18], esi
            jl L00456206
    L004563ee:
            dec ebp
            jns L004561db
            mov eax, dword ptr [gSound_enabled]
            test eax, eax
            je L0045651d
            mov eax, dword ptr [gServicing_sound]
            test eax, eax
            jne L0045651d
            mov dword ptr [gServicing_sound], 1
            call PDGetTotalTime
            mov dword ptr [gLast_sound_service], eax
            mov eax, dword ptr [gCDA_started_playing]
            test eax, eax
            je L004564e5
            call PDGetTotalTime
            mov esi, eax
            mov eax, dword ptr [gNext_track_finished_check]
            cmp esi, eax
            jbe L004564e5
            call S3IsCDAPlaying
            test eax, eax
            jne L004564e5
            mov eax, dword ptr [gNext_track_finished_check]
            push eax
            push esi
            push offset gRandom_CDA_tunes_2+0xfc
            call dr_dprintf
            mov eax, dword ptr [gCD_fully_installed]
            add esp, 0xc
            test eax, eax
            je L0045649a
            mov eax, dword ptr [gMusic_available]
            test eax, eax
            je L0045649a
            mov ecx, dword ptr [gINT_00684568]
            test ecx, ecx
            je L0045649a
            call S3StopSound
            mov dword ptr [gINT_00684554], 0
            mov dword ptr [gINT_00684568], 0
    L0045649a:
            mov eax, dword ptr [gINT_00595c44]
            test eax, eax
            je L004564af
            cmp dword ptr [gProgram_state+0x70], 0x80
            jl L004564e5
    L004564af:
            call S3IsCDAPlaying
            test eax, eax
            jne L004564e5
            call PDGetTotalTime
            add eax, 0x2710
            push eax
            push offset gRandom_CDA_tunes_2+0x60
            mov dword ptr [gNext_track_finished_check], eax
            call dr_dprintf
            mov ecx, dword ptr [gINT_00595c20]
            add esp, 8
            call DRS3StartCDA
            mov dword ptr [gINT_00684568], eax
    L004564e5:
            mov eax, dword ptr [gSound_enabled]
            test eax, eax
            je L00456513
            mov eax, dword ptr [gProgram_state+0x4c]
            test eax, eax
            je L00456507
            mov eax, dword ptr [gProgram_state+0x50]
            test eax, eax
            jl L00456507
            mov ecx, 1
            jmp L00456509
    L00456507:
            xor ecx, ecx
    L00456509:
            mov edx, 1
            call S3Service
    L00456513:
            mov dword ptr [gServicing_sound], 0
    L0045651d:
            pop edi
            pop esi
            pop ebp
            pop ebx
            add esp, 0x14
            ret
    }
#endif
}

// FUNCTION: CARMA2_HW 0x00456530
void C2_HOOK_FASTCALL SetSoundVolumes(int pCD_audio) {

    if (!gSound_enabled) {
        return;
    }
    if (gEffects_outlet != NULL) {
        DRS3SetOutletVolume(gEffects_outlet, gProgram_state.effects_volume);
    }
    DRS3SetOutletVolume(gXXX_outlet, gProgram_state.effects_volume);
    DRS3SetOutletVolume(gEngine_outlet, gProgram_state.effects_volume);
    DRS3SetOutletVolume(gDriver_outlet, gProgram_state.effects_volume);
    DRS3SetOutletVolume(gPedestrians_outlet, gProgram_state.effects_volume);
    if (gINT_00595c44) {
        dr_dprintf("CD: Setting volume to %d", gProgram_state.music_volume);
        if (!gCDA_started_playing && !pCD_audio) {
            dr_dprintf("CD: Hasn't played yet, so not stopping/starting it");
        } else {
            if (gProgram_state.music_volume < 128) {
                StopMusic();
            } else {
                StartMusic();
            }
        }
    } else {
        DRS3SetOutletVolume(gMusic_outlet, gProgram_state.music_volume);
    }
}

// FUNCTION: CARMA2_HW 0x004566a0
tS3_outlet* C2_HOOK_FASTCALL GetOutletFromIndex(int pIndex) {
    return gIndexed_outlets[pIndex];
}

// FUNCTION: CARMA2_HW 0x004566b0
int C2_HOOK_FASTCALL GetIndexFromOutlet(tS3_outlet* pOutlet) {
    int i;

    for (i = 0; i < (int)CARPOCALYPSE2_ASIZE(gIndexed_outlets); i++) {
        if (gIndexed_outlets[i] == pOutlet) {
            return i;
        }
    }
    return 0;
}

// FUNCTION: CARMA2_HW 0x004566d0
int C2_HOOK_FASTCALL DRS3StartCDA(int pSound) {

    if (pSound == 9999 || pSound == 9998 || pSound == 9997) {
        gINT_00595c20 = pSound;
    }
    if (!gCD_is_disabled && gMusic_available && !gINT_00684554 && !gINT_00684568 && S3IsCDAEnabled()) {
        dr_dprintf("CDINFO: DRS3StartCDA(): Requested track id %d", pSound);
        gCDA_started_playing = 1;
        S3StopOutletSound(gMusic_outlet);
        if (gSound_enabled) {
            S3Service(gProgram_state.cockpit_on != 0 && gProgram_state.cockpit_image_index >= 0, 0);
            /* Random CDA track */
            if (pSound == 9999 || pSound == 9998 || pSound == 9997) {
                do {
                    switch (pSound) {
                        case 9997:
                            pSound = gRandom_CDA_tunes_2[IRandomBetween(0, CARPOCALYPSE2_ASIZE(gRandom_CDA_tunes_2) - 1)];
                            break;
                        case 9998:
                            pSound = gRandom_CDA_tunes_1[IRandomBetween(0, CARPOCALYPSE2_ASIZE(gRandom_CDA_tunes_1) - 1)];
                            break;
                        default:
                            pSound = gRandom_CDA_tunes[IRandomBetween(0, CARPOCALYPSE2_ASIZE(gRandom_CDA_tunes) - 1)];
                    }
                } while (pSound == gLast_tune);
            }
            gLast_tune = pSound;
            gINT_00684554 = DRS3StartSoundNoPiping(gMusic_outlet, pSound);
            gINT_00684568 = gINT_00684554;
            if (!gINT_00684554) {
                dr_dprintf("CDINFO: DRS3StartCDA(): Chosen actual CD track %d", pSound);
            }
            gUNK_0068455c = 0;
        }
    }
    return gINT_00684568;
}

void C2_HOOK_FASTCALL DRS3StopCDA(void) {

    if (gCD_fully_installed && gMusic_available && gINT_00684568 != 0) {
        S3StopSound(gINT_00684568);
        gINT_00684554 = 0;
        gINT_00684568 = 0;
    }
}

// StartMusicTrack

// FUNCTION: CARMA2_HW 0x004568c0
void C2_HOOK_FASTCALL StartMusic(void) {

    if (!gINT_00595c44 || gProgram_state.music_volume >= 128) {
        if (!S3IsCDAPlaying()) {
            gNext_track_finished_check = PDGetTotalTime() + 10000;
            dr_dprintf("CDINFO: StartMusic(): New gNext_track_finished_check %d", gNext_track_finished_check);
            gINT_00684568 = DRS3StartCDA(gINT_00595c20);
        }
    }
}

// FUNCTION: CARMA2_HW 0x00456910
void C2_HOOK_FASTCALL StopMusic(void) {
    if (gCD_fully_installed) {
        DRS3StopCDA();
    }
}

// FUNCTION: CARMA2_HW 0x004569e0
int C2_HOOK_FASTCALL GetSoundDetailLevel(void) {
    return gSound_detail_level;
}

// FUNCTION: CARMA2_HW 0x00456950
void C2_HOOK_FASTCALL SetSoundDetailLevel(int pLevel) {
    gSound_detail_level = pLevel;
}

// ReallySetSoundDetailLevel

// GetSoundDetailLevel

// FUNCTION: CARMA2_HW 0x004569f0
void C2_HOOK_FASTCALL ReadSoundSpec(FILE* pF, tSpecial_volume_soundfx_data* pSpec) {
    int i;
    float f1, f2;

    pSpec->periodicity = GetALineAndInterpretCommand(pF, gSound_periodicity_choices, CARPOCALYPSE2_ASIZE(gSound_periodicity_choices));
    if (pSpec->periodicity != kSoundFxPeriodicity_None) {
        switch (pSpec->periodicity) {
        case kSoundFxPeriodicity_Periodic:
            f1 = GetAScalar(pF);
            pSpec->period = (int)(1000.0f * f1);
            break;
        case kSoundFxPeriodicity_Random:
            GetPairOfFloats(pF, &f1, &f2);
            pSpec->random.min_gap = (int)(1000.0f * f1);
            pSpec->random.max_gap = (int)(1000.0f * f2);
            break;
#ifdef CARPOCALYPSE2_FIX_BUGS
        default:
            break;
#endif
        }
        pSpec->max_deviation = BR_FIXED_INT(GetAnInt(pF)) / 100;
        pSpec->count_sound_alternatives = GetAnInt(pF);

        C2_HOOK_BUG_ON(CARPOCALYPSE2_ASIZE(pSpec->sound_alternatives) != 5);
        if (pSpec->count_sound_alternatives > (int)CARPOCALYPSE2_ASIZE(pSpec->sound_alternatives)) {
            FatalError(kFatalError_TooManyEnvironmentalSoundAlternatives);
        }
        for (i = 0; i < pSpec->count_sound_alternatives; i++) {

            pSpec->sound_alternatives[i] = LoadSingleSound(&gTrack_storage_space, GetAnInt(pF));
        }
        pSpec->field_0xc = 0;
    }
}

// TryToSetEnvironmentalSound

// DoAnEnvironmentalSound

// DoEnvSound

// FoundSoundSource

// MungeSoundGenerators

// MungeEnvironmentalSound

void C2_HOOK_FASTCALL BuggerModelName(tTrack_spec* pTrack_spec, const char* pName, int pIndex) {
    br_model* model;
    char identifier[4];

    sprintf(identifier, "%c%02d", ')', pIndex);
    model = BrModelFind(identifier);
    if (model == NULL || model->identifier == NULL) {
#ifdef CARPOCALYPSE2_FIX_BUGS
        FatalError(kFatalError_CannotFindModelReferencedInSoundGeneratorList_S, identifier);
#else
        FatalError(kFatalError_CannotFindModelReferencedInSoundGeneratorList_S);
#endif
    } else {
        memcpy(model->identifier, identifier, sizeof(identifier));
    }
}

// FUNCTION: CARMA2_HW 0x004572f0
void C2_NAKED C2_HOOK_FASTCALL ReadSoundGenerators(tTrack_spec* pTrack_spec, FILE* pF) {
#ifndef CARPOCALYPSE2_MATCHING
    float x;
    float y;
    float z;
    char name[32];
    tTrackSoundGenerator* generator;
    int i;

    gProgram_state.count_track_sound_generators = GetAnInt(pF);
    if (gProgram_state.count_track_sound_generators != 0) {

        gProgram_state.track_sound_generators = BrMemAllocate(gProgram_state.count_track_sound_generators * sizeof(tTrackSoundGenerator), kMem_sound_generator);
        for (i = 0, generator = gProgram_state.track_sound_generators; i < gProgram_state.count_track_sound_generators; i++, generator++) {

            generator->type = GetALineAndInterpretCommand(pF, gSound_generator_type_names, CARPOCALYPSE2_ASIZE(gSound_generator_type_names));
            switch (generator->type) {
            case kSoundGeneratorType_point:
                GetThreeFloats(pF, &x, &y, &z);
                BrVector3Set(&generator->point, x, y, z);
                break;
            case kSoundGeneratorType_actor:
                GetAString(pF, name);
                BuggerModelName(pTrack_spec, name, i);
                break;
            case kSoundGeneratorType_noncar:
                GetAString(pF, name);
                BuggerModelName(pTrack_spec, name, i);
                break;
            }
            ReadSoundSpec(pF, &generator->fx);
            if (generator->type == kSoundGeneratorType_noncar) {
                ReadSoundSpec(pF, &generator->fx1_noncar);
                ReadSoundSpec(pF, &generator->fx2_noncar);
            }
        }
    }
#else
    __asm {
            sub esp, 0x34
            push ebx
            push esi
            mov esi, edx
            push edi
            mov ecx, esi
            call GetAnInt
            test eax, eax
            mov dword ptr [gProgram_state+0x1c04], eax
            je L00457449
            mov ecx, eax
            push 0xc8
            shl ecx, 5
            sub ecx, eax
            shl ecx, 2
            push ecx
            call BrMemAllocate
            mov dword ptr [gProgram_state+0x1c08], eax
            mov edi, eax
            mov eax, dword ptr [gProgram_state+0x1c04]
            add esp, 8
            xor ebx, ebx
            test eax, eax
            jle L00457449
    L0045733a:
            push 3
            mov edx, offset gSound_generator_type_names
            mov ecx, esi
            call GetALineAndInterpretCommand
            mov dword ptr [edi], eax
            sub eax, 0
            je L004573c8
            dec eax
            je L00457388
            dec eax
            jne L00457415
            lea edx, [esp + 0x14]
            lea eax, [esp + 0x10]
            push edx
            push eax
            lea edx, [esp + 0x14]
            mov ecx, esi
            call GetThreeFloats
            mov ecx, dword ptr [esp + 0xc]
            mov edx, dword ptr [esp + 0x10]
            mov eax, dword ptr [esp + 0x14]
            mov dword ptr [edi + 0x70], ecx
            mov dword ptr [edi + 0x74], edx
            mov dword ptr [edi + 0x78], eax
            jmp L00457415
    L00457388:
            lea edx, [esp + 0x20]
            mov ecx, esi
            call GetAString
            lea ecx, [esp + 0x18]
            push ebx
            push 0x29
            push offset gRandom_CDA_tunes_2+0x1f8
            push ecx
            call sprintf
            add esp, 0x10
            lea edx, [esp + 0x20]
            push edx
            call BrModelFind
            add esp, 4
            test eax, eax
            je L00457408
            mov eax, dword ptr [eax + 4]
            test eax, eax
            je L00457408
            mov ecx, dword ptr [esp + 0x18]
            mov dword ptr [eax], ecx
            jmp L00457415
    L004573c8:
            lea edx, [esp + 0x20]
            mov ecx, esi
            call GetAString
            lea edx, [esp + 0x1c]
            push ebx
            push 0x29
            push offset gRandom_CDA_tunes_2+0x1f8
            push edx
            call sprintf
            add esp, 0x10
            lea eax, [esp + 0x20]
            push eax
            call BrModelFind
            add esp, 4
            test eax, eax
            je L00457408
            mov eax, dword ptr [eax + 4]
            test eax, eax
            je L00457408
            mov ecx, dword ptr [esp + 0x1c]
            mov dword ptr [eax], ecx
            jmp L00457415
    L00457408:
            push 0x84
            call FatalError
            add esp, 4
    L00457415:
            lea edx, [edi + 4]
            mov ecx, esi
            call ReadSoundSpec
            cmp dword ptr [edi], 0
            jne L00457438
            lea edx, [edi + 0x28]
            mov ecx, esi
            call ReadSoundSpec
            lea edx, [edi + 0x4c]
            mov ecx, esi
            call ReadSoundSpec
    L00457438:
            mov eax, dword ptr [gProgram_state+0x1c04]
            inc ebx
            add edi, 0x7c
            cmp ebx, eax
            jl L0045733a
    L00457449:
            pop edi
            pop esi
            pop ebx
            add esp, 0x34
            ret
    }
#endif
}

// FUNCTION: CARMA2_HW 0x00457450
void C2_NAKED C2_HOOK_FASTCALL WriteOutSoundSpec(FILE* pF, tSpecial_volume_soundfx_data* pSpec) {
#ifndef CARPOCALYPSE2_MATCHING
    int i;

    if (pSpec->periodicity == kSoundFxPeriodicity_None) {
        fprintf(pF, "NONE\t\t\t\t\t// sound time type\n");
        return;
    }

    fprintf(pF, "%s\t\t\t\t\t// sound time type\n", gSound_periodicity_choices[pSpec->periodicity]);
    switch (pSpec->periodicity) {
    case kSoundFxPeriodicity_Periodic:
        fprintf(pF,"%.2f\t\t\t\t\t// period\n",
            (double)pSpec->period / 1000.0);
        break;
    case kSoundFxPeriodicity_Random:
        fprintf(pF, "%.2f,%.2f\t\t\t\t\t// min,max gap\n",
           (double)pSpec->random.min_gap / 1000.0,
           (double)pSpec->random.max_gap / 1000.0);
        break;
#ifdef CARPOCALYPSE2_FIX_BUGS
    default:
        break;
#endif
    }
    fprintf(pF, "%d\t\t\t\t\t// max deviation\n", (int)((100.0 * (double)pSpec->max_deviation) / 65536.0));
    fprintf(pF, "%d\t\t\t\t\t// num sounds\n", pSpec->count_sound_alternatives);
    for (i = 0; i < pSpec->count_sound_alternatives; i++) {

        fprintf(pF, "%d\t\t\t\t\t// sound ID\n", pSpec->sound_alternatives[i]);
    }
#else
    __asm {
            sub esp, 8
            push ebx
            push ebp
            push esi
            mov esi, edx
            push edi
            mov ebp, ecx
            mov eax, dword ptr [esi]
            cmp eax, -1
            jne L00457478
            push offset gRandom_CDA_tunes_2+0x298
            push ebp
            call fprintf
            add esp, 8
            pop edi
            pop esi
            pop ebp
            pop ebx
            add esp, 8
            ret
    L00457478:
            mov eax, dword ptr [eax*4 + gSound_periodicity_choices]
            push eax
            push offset gRandom_CDA_tunes_2+0x27c
            push ebp
            call fprintf
            mov eax, dword ptr [esi]
            xor edi, edi
            add esp, 0xc
            sub eax, edi
            je L004574c4
            dec eax
            jne L00457509
            mov ecx, dword ptr [esi + 4]
            mov dword ptr [esp + 0x14], edi
            mov dword ptr [esp + 0x10], ecx
            sub esp, 8
            fild qword ptr [esp + 0x18]
            fmul qword ptr [gDrone_select_max_distance_squared+0xc4]
            _emit 0xdd
        _emit 0x1c
        _emit 0x24
            push offset gRandom_CDA_tunes_2+0x268
            push ebp
            call fprintf
            add esp, 0x10
            jmp L00457509
    L004574c4:
            mov edx, dword ptr [esi + 8]
            mov dword ptr [esp + 0x14], edi
            mov dword ptr [esp + 0x10], edx
            mov eax, dword ptr [esi + 4]
            fild qword ptr [esp + 0x10]
            sub esp, 8
            mov dword ptr [esp + 0x18], eax
            mov dword ptr [esp + 0x1c], edi
            sub esp, 8
            fmul qword ptr [gDrone_select_max_distance_squared+0xc4]
            fstp qword ptr [esp + 8]
            fild qword ptr [esp + 0x20]
            fmul qword ptr [gDrone_select_max_distance_squared+0xc4]
            _emit 0xdd
        _emit 0x1c
        _emit 0x24
            push offset gRandom_CDA_tunes_2+0x248
            push ebp
            call fprintf
            add esp, 0x18
    L00457509:
            fild dword ptr [esi + 0x14]
            fmul qword ptr [gFLOAT_00589f1c+0xc]
            fmul qword ptr [gFLOAT_00589f1c+0x14]
            call _ftol
            push eax
            push offset gRandom_CDA_tunes_2+0x22c
            push ebp
            call fprintf
            mov ecx, dword ptr [esi + 0x10]
            add esp, 0xc
            push ecx
            push offset gRandom_CDA_tunes_2+0x214
            push ebp
            call fprintf
            mov eax, dword ptr [esi + 0x10]
            add esp, 0xc
            cmp eax, edi
            jle L00457567
            lea ebx, [esi + 0x18]
    L00457548:
            xor edx, edx
            mov dx, word ptr [ebx]
            push edx
            push offset gRandom_CDA_tunes_2+0x200
            push ebp
            call fprintf
            mov eax, dword ptr [esi + 0x10]
            add esp, 0xc
            inc edi
            add ebx, 2
            cmp edi, eax
            jl L00457548
    L00457567:
            pop edi
            pop esi
            pop ebp
            pop ebx
            add esp, 8
            ret
    }
#endif
}

// FUNCTION: CARMA2_HW 0x00457570
void C2_HOOK_FASTCALL SetDefaultSoundFolderName(void) {
    gPed_sound_path = NULL;
}


// FUNCTION: CARMA2_HW 0x00456840 (StartMusicTrack)
void C2_HOOK_FASTCALL StartMusicTrack(int pMusic_track) {

    if (gCD_fully_installed && gMusic_available && gINT_00684568 != 0) {
        S3StopSound(gINT_00684568);
        gINT_00684554 = 0;
        gINT_00684568 = 0;
    }
    if (!gINT_00595c44 || gProgram_state.music_volume >= 128) {
        gNext_track_finished_check = PDGetTotalTime() + 10000;
        dr_dprintf("CDINFO: StartMusicTrack(): New gNext_track_finished_check %d", gNext_track_finished_check);
        gINT_00684568 = DRS3StartCDA(pMusic_track);
    }
}
#pragma auto_inline(off)
// FUNCTION: CARMA2_HW 0x005648ac
int C2_NAKED C2_HOOK_STDCALL FUN_005648ac(int pArg) {
#ifndef CARPOCALYPSE2_MATCHING
    (void)pArg;
    return 12;
#else
    __asm {
            push ebp
            mov ebp, esp
            sub esp, 0xc
            mov dword ptr [ebp - 0xc], edx
            mov dword ptr [ebp - 8], ecx
            mov eax, 0xc
            mov esp, ebp
            pop ebp
            ret 4
    }
#endif
}
#pragma auto_inline(on)

// FUNCTION: CARMA2_HW 0x004557c0
int C2_HOOK_STDCALL FUN_004557c0(int pArg) {
    if (gSound_enabled != 0) {
        return FUN_005648ac(pArg);
    }
    return 0;
}

// FUNCTION: CARMA2_HW 0x00455880
int C2_HOOK_FASTCALL DRS3ReleaseSound(int pSound_id) {
    if (gSound_enabled) {
        return S3ReleaseSound(pSound_id);
    } else {
        return 0;
    }
}
