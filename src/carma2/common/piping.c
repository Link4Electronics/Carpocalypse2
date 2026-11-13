#include "piping.h"

extern const br_scalar gFLOAT_00589330;

#include "compress.h"

#include "crush.h"
#include "errors.h"

#include "flap.h"

#include "globvars.h"

#include "input.h"

#include "network.h"
#include "opponent.h"
#include "pedestrn.h"
#include "physics.h"
#include "platform.h"

#include "replay.h"

#include "smashing.h"
#include "sound.h"
#include "utility.h"

#include "video.h"

#include <brender/brender.h>

#include <math.h>

#include <s3/internal/audio.h>

#include "carpocalypse2_macros.h"

#include "c2_string.h"

#include <stdarg.h>
#include "piping.h"

#define CRUSH_SPACE_SIZE 0x4000
#define SIZE_OFFSET_PIPING(T, M) ((int)sizeof(((T*)NULL)->M)), ((int)offsetof(T, M))

typedef union tFloat_u32_union {
    float f;
    tU32 u;
} tFloat_u32_union;

void C2_HOOK_FASTCALL ARAdvanceChunkPtr(tPipe_chunk** pChunk_ptr, tPipe_chunk_type pType);

// GLOBAL: CARMA2_HW 0x00676918
int gDisable_sound;

// GLOBAL: CARMA2_HW 0x00589338
const double gDouble_00589338 = 1500.0;

// GLOBAL: CARMA2_HW 0x00589340
const double gDouble_00589340 = 0.0000152587890625;

// GLOBAL: CARMA2_HW 0x00694104
undefined4* gCrush_space;

// GLOBAL: CARMA2_HW 0x006768b8
tU8* gPipe_play_ptr = NULL;

// GLOBAL: CARMA2_HW 0x006768c0
int gPlay_direction = 0;

// GLOBAL: CARMA2_HW 0x006768bc
tU8* gPipe_record_ptr = NULL;

// GLOBAL: CARMA2_HW 0x006768a8
tU32 gPipe_UNK_006768a8;

// GLOBAL: CARMA2_HW 0x006768ac
tU8* gPipe_buffer_oldest = NULL;

// GLOBAL: CARMA2_HW 0x006768b0
tU32 gPipe_UNK_006768b0;

// GLOBAL: CARMA2_HW 0x00676904
tU32 gPipe_UNK_00676904;

// GLOBAL: CARMA2_HW 0x00694108
tU32 gYoungest_time;

// GLOBAL: CARMA2_HW 0x006940d0
tU32 gOldest_time;

// GLOBAL: CARMA2_HW 0x006940f0
br_vector3 gCar_pos;

// GLOBAL: CARMA2_HW 0x006940d8
br_scalar gMax_distance;

// GLOBAL: CARMA2_HW 0x006940e0
br_vector3 gReference_pos;

// GLOBAL: CARMA2_HW 0x00694100
tCar_spec* gCar_ptr;

// GLOBAL: CARMA2_HW 0x00694110
tU32 gTrigger_time;

// GLOBAL: CARMA2_HW 0x0069410c
volatile br_scalar gUNK_0069410c;

// GLOBAL: CARMA2_HW 0x0058a908
const double gDouble_0058a908 = 468099.99999999994;

// GLOBAL: CARMA2_HW 0x0058ab98
const double gDouble_0058ab98 = 0.000004;


// GLOBAL: CARMA2_HW 0x00676908
tU8* gPipe_buffer_start = NULL;

// GLOBAL: CARMA2_HW 0x006768fc
int gReplay_pause_state = 0;

// GLOBAL: CARMA2_HW 0x0067690c
int gPipe_undo_enabled = 0;

#ifdef CARPOCALYPSE2_MATCHING
#endif
#ifdef CARPOCALYPSE2_MATCHING
#endif
#ifdef CARPOCALYPSE2_MATCHING
#endif
#ifdef CARPOCALYPSE2_MATCHING
extern void C2_HOOK_FASTCALL AREndPipingSession(void);
extern void C2_HOOK_FASTCALL ApplyWaterOmegaBrake(tPhysics_object* pObject, const br_vector3* pV);
extern void C2_HOOK_CDECL BrMaterialUpdate();
extern void C2_HOOK_FASTCALL CompressVector3(tCompressed_vector3* pDest, const br_vector3* pSrc, float pMin, float pMax);
extern intptr_t C2_HOOK_FASTCALL DRActorEnumRecurse(br_actor* pActor, br_actor_enum_cbfn* callback, void* arg);
extern tU16 C2_HOOK_FASTCALL DRScalarToU16(float pValue, float pMin, float pMax);
extern void C2_HOOK_FASTCALL EndPipingSession2(int pMunge_reentrancy);
extern void C2_HOOK_FASTCALL ExpandVector3(br_vector3* pDest, const tCompressed_vector3 *pSrc, float pMin, float pMax);
extern void C2_HOOK_FASTCALL FUN_0042f5d0(br_model* pSource_model, br_model** pModel_out1, br_model** pModel_out2, float* pPlane, int pArg5);
extern int C2_HOOK_FASTCALL FUN_00564e5e(int pSample_id);
extern void C2_NORETURN C2_HOOK_CDECL FatalError();
extern int C2_HOOK_CDECL FindActorByCrushEntryID();
extern int C2_HOOK_FASTCALL GetCarCount(tVehicle_type pCategory);
extern tCar_spec* C2_HOOK_FASTCALL GetCarSpec(tVehicle_type pCategory, int pIndex);
extern tU32 C2_HOOK_FASTCALL GetTotalTime(void);
extern intptr_t C2_HOOK_CDECL MakeCarModelsMaterialsDoubleSided();
extern tCar_spec* C2_HOOK_FASTCALL NetCarFromPlayerID(tPlayer_ID pPlayer);
extern tPhysics_object* C2_HOOK_FASTCALL PHILGetFirstObject(void);
extern tPhysics_object* C2_HOOK_FASTCALL PHILGetNextObject(tPhysics_object* pCollision_info);
extern int C2_HOOK_FASTCALL PhysicsObjectRecurseChildren(tPhysics_object* pObject, tEnumCollision_cbfn* pCallback, void* pContext);
extern tPhysics_object* C2_HOOK_FASTCALL SemiDetachBit(tCar_spec* pCar, br_actor* pActor, float pArg3, int* pArg4, br_vector3* pArg5, br_bounds3* pArg6, br_vector3* pArg7, br_vector3* pArg8);
extern void __cdecl _chkstk();
extern void __cdecl _ftol();
extern br_uint_8 bl;
extern int gCount_net_crush_detach_list;
extern int gCount_net_crush_full_detach_bit_list;
extern tCrush_net_detach_list_item gNet_crush_detach_list[8];
extern tCrush_net_full_detach_bit_list_item gNet_crush_full_detach_bit_list[8];
extern int gPed_676914;
extern tProgram_state gProgram_state;
#endif
#ifdef CARPOCALYPSE2_MATCHING
#endif
#ifdef CARPOCALYPSE2_MATCHING
extern void C2_HOOK_FASTCALL ARDoActionReplay(int pStep_count, tARDoActionReplay_step_callback* pStep_callback, tARDoActionReplay_frame_callback* pFrame_start_callback, tARDoActionReplay_frame_callback* pFrame_end_callback);
extern tU8* C2_HOOK_FASTCALL ARGetPipePlayPtr(void);
extern int C2_HOOK_FASTCALL ARReplayForwards(void);
extern void C2_HOOK_CDECL BrMatrix34ApplyV();
extern void C2_HOOK_FASTCALL DisposeReplayPixelmaps(void);
extern void C2_HOOK_FASTCALL InitializeActionReplayPix(void);
extern int C2_HOOK_FASTCALL MovieStopRecordingIfNecessary(void);
extern int C2_HOOK_FASTCALL S3SetSampleFilterCallbacks(tS3_sample_filter pEnable, tS3_sample_filter pDisable);
extern void C2_HOOK_CDECL dr_dprintf();
extern tU32 gFrame_period;
extern br_vector3 gInitial_position;
extern tU32 gLast_replay_frame_time;
#endif
// FUNCTION: CARMA2_HW 0x004030d0
void C2_HOOK_FASTCALL SetPipeUndoFlag(void) {
    gPipe_undo_enabled = 1;
}

// FUNCTION: CARMA2_HW 0x004030e0
void C2_HOOK_FASTCALL ClearPipeUndoFlag(void) {
    gPipe_undo_enabled = 0;
}

// GLOBAL: CARMA2_HW 0x006768e4
int gReentrancy_count = 0;

// GLOBAL: CARMA2_HW 0x006768d0
tPipe_chunk_type gReentrancy_array[5];

// GLOBAL: CARMA2_HW 0x006768cc
tPipe_chunk_type gReentrancy_array_base_minus_one;

// GLOBAL: CARMA2_HW 0x006768b4
tU8* gLocal_buffer = NULL;

// GLOBAL: CARMA2_HW 0x006768f4
tU32 gLocal_buffer_size = 0;

// GLOBAL: CARMA2_HW 0x006768a0
tU8* gMr_chunky = NULL;

// GLOBAL: CARMA2_HW 0x006768e8
const tReplay_callback* gPipe_callbacks = NULL;

// GLOBAL: CARMA2_HW 0x006768f0
tU8* gPipe_buffer_phys_end = NULL;

// GLOBAL: CARMA2_HW 0x006768c4
tU8* gPipe_buffer_working_end = NULL;

// GLOBAL: CARMA2_HW 0x006768c8
tU8* gPipe_buffer_end_marker = NULL;

// GLOBAL: CARMA2_HW 0x0058f1e8
const int gPipe_session_search_limit = 1000;

// GLOBAL: CARMA2_HW 0x006768a4
tU32 gReplay_saved_frame_period;

// GLOBAL: CARMA2_HW 0x006768f8
tU32 gReplay_saved_average_frame_period;

// GLOBAL: CARMA2_HW 0x006940d4
tPipe_smudge_data* gSmudge_space;

// GLOBAL: CARMA2_HW 0x00676910
int gPipe_count_callbacks = 0;

// GLOBAL: CARMA2_HW 0x006768ec
tU32 gPipe_buffer_size = 0;

// GLOBAL: CARMA2_HW 0x00676900
float gReplay_rate = 0.f;
extern const br_scalar gFLOAT_00589330;

// FUNCTION: CARMA2_HW 0x00403d60
int C2_NAKED C2_HOOK_FASTCALL ARToggleReplay(tTurn_on_AR_callback* pTurn_on_AR_cb, tTurn_off_AR_callback* pTurn_off_AR_cb, tAfter_AR_callback* pAfter_AR_cb, tZappy_AR_callback* pZappy_AR_cb, tPreprocess_AR_callback* pPreprocess_cb, tPostProcess_AR_callback* pPostProcess_cb, int* pArg7, int* pArg8) {
#ifndef CARPOCALYPSE2_MATCHING

    NOT_IMPLEMENTED();
#else
    __asm {
            push ecx
            mov eax, dword ptr [gPipe_buffer_start]
            push ebx
            push esi
            mov esi, edx
            test eax, eax
            mov ebx, ecx
            je L00403ea2
            mov eax, dword ptr [gPed_676914]
            test eax, eax
            jne L00403df7
            push offset gPipe_session_search_limit+0x28
            call dr_dprintf
            add esp, 4
            call InitializeActionReplayPix
            mov eax, dword ptr [esp + 0x20]
            mov ecx, dword ptr [gFrame_period]
            test eax, eax
            je L00403da6
            mov eax, dword ptr [eax]
            mov dword ptr [gReplay_saved_frame_period], eax
            jmp L00403dac
    L00403da6:
            mov dword ptr [gReplay_saved_frame_period], ecx
    L00403dac:
            mov eax, dword ptr [esp + 0x24]
            test eax, eax
            je L00403db6
            mov ecx, dword ptr [eax]
    L00403db6:
            mov dword ptr [gReplay_saved_average_frame_period], ecx
            mov dword ptr [gReplay_rate], 0x3f800000
            mov dword ptr [gReplay_pause_state], 1
            mov dword ptr [gPlay_direction], 1
            call GetTotalTime
            mov edx, dword ptr [gPipe_record_ptr]
            mov dword ptr [gLast_replay_frame_time], eax
            mov dword ptr [gPipe_play_ptr], edx
            call ebx
            jmp L00403e81
    L00403df7:
            mov ecx, dword ptr [esp + 0x1c]
            mov edx, dword ptr [esp + 0x18]
            mov eax, dword ptr [gReplay_rate]
            push ecx
            push edx
            mov edx, dword ptr [esp + 0x1c]
            mov ecx, 0x7fffffff
            mov dword ptr [gDisable_sound], 1
            mov dword ptr [esp + 0x10], eax
            mov dword ptr [gReplay_rate], 0x42c80000
            call ARDoActionReplay
            xor edx, edx
            xor ecx, ecx
            mov eax, dword ptr [esp + 8]
            mov dword ptr [gDisable_sound], 0
            mov dword ptr [gReplay_rate], eax
            call S3SetSampleFilterCallbacks
            call esi
            mov eax, dword ptr [esp + 0x20]
            test eax, eax
            je L00403e5a
            mov ecx, dword ptr [gReplay_saved_frame_period]
            mov dword ptr [eax], ecx
    L00403e5a:
            mov eax, dword ptr [esp + 0x24]
            test eax, eax
            je L00403e6a
            mov edx, dword ptr [gReplay_saved_average_frame_period]
            mov dword ptr [eax], edx
    L00403e6a:
            call MovieStopRecordingIfNecessary
            push offset gPipe_session_search_limit+0x4
            call dr_dprintf
            add esp, 4
            call DisposeReplayPixelmaps
    L00403e81:
            mov ecx, dword ptr [gPed_676914]
            xor eax, eax
            test ecx, ecx
            sete al
            mov dword ptr [gPed_676914], eax
            call dword ptr [esp + 0x10]
            mov eax, 1
            pop esi
            pop ebx
            pop ecx
            ret 0x18
    L00403ea2:
            pop esi
            xor eax, eax
            pop ebx
            pop ecx
            ret 0x18
    }
#endif
}

// FUNCTION: CARMA2_HW 0x004023e0
void C2_HOOK_FASTCALL ARResetPiping(void) {
    gPipe_record_ptr = gPipe_buffer_start;
    gPipe_buffer_working_end = gPipe_buffer_phys_end;
    gPipe_buffer_oldest = NULL;
    gReentrancy_count = 0;
}

// FUNCTION: CARMA2_HW 0x0051b9a0
void C2_HOOK_FASTCALL PDAllocateActionReplayBuffer(tU8** buffer, tU32* size) {
    PDReallyAllocateActionReplayBuffer();
    if (buffer != NULL) {
        *buffer = gPDActionReplayBuffer;
    }
    if (size != NULL) {
        *size = gPDActionReplayBufferSize;
    }
}

// FUNCTION: CARMA2_HW 0x00402410
void C2_HOOK_FASTCALL ARInitialise(int pEnable, int pCount_callbacks, const tReplay_callback* pCallbacks) {
    if (pEnable == 0) {
        gPipe_buffer_start = NULL;
        gLocal_buffer = NULL;
    } else {
        gPipe_count_callbacks = pCount_callbacks;
        gPipe_callbacks = pCallbacks;
#pragma inline_depth(0)
        PDAllocateActionReplayBuffer(&gPipe_buffer_start, &gPipe_buffer_size);
#pragma inline_depth()
        gPipe_buffer_phys_end = gPipe_buffer_start + gPipe_buffer_size;
        gLocal_buffer = BrMemAllocate(15000, kMem_pipe_model_geometry);
    }
    ARResetPiping();
}

// FUNCTION: CARMA2_HW 0x004025a0
int C2_HOOK_FASTCALL ARIsActionReplayAvailable(void) {
    return gPipe_buffer_start != NULL;
}

void C2_HOOK_FASTCALL FUN_DisposeReplayBuffer(void* pBuffer);

// FUNCTION: CARMA2_HW 0x004024a0
void C2_HOOK_FASTCALL DisposePiping(void) {
    if (gPipe_buffer_start != NULL) {
        FUN_DisposeReplayBuffer(gPipe_buffer_start);
    }
    gPipe_buffer_start = NULL;
    if (gLocal_buffer != NULL) {
        BrMemFree(gLocal_buffer);
        gLocal_buffer = NULL;
    }
}

void C2_HOOK_FASTCALL StartPipingSession2(tPipe_chunk_type pType, int pMunge_reentrancy) {

    if (gPipe_buffer_start == NULL || gPed_676914 || !gProgram_state.racing) {
        return;
    }
    /* Header of gLocal_buffer is { int chunk_type, int chunk_data_size } */
    if (pMunge_reentrancy) {
        if (gReentrancy_count != 0) {
            gReentrancy_array[gReentrancy_count - 1] = ((tPipe_chunk*)gLocal_buffer)->type;
            EndPipingSession2(0);
        }
        gReentrancy_count += 1;
    }
    ((tPipe_chunk*)gLocal_buffer)->type = pType;
    ((tPipe_chunk*)gLocal_buffer)->count = 0;
    gLocal_buffer_size = 2 * sizeof(int);
    gMr_chunky = gLocal_buffer + 2 * sizeof(int);
}

// FUNCTION: CARMA2_HW 0x004025e0
void C2_NAKED C2_HOOK_FASTCALL ARStartPipingSession(tPipe_chunk_type pType) {
#ifndef CARPOCALYPSE2_MATCHING

    StartPipingSession2(pType, 1);
#else
    __asm {
            mov eax, dword ptr [gPipe_buffer_start]
            push esi
            test eax, eax
            mov esi, ecx
            je L00402655
            mov eax, dword ptr [gPed_676914]
            test eax, eax
            jne L00402655
            mov eax, dword ptr [gProgram_state+0x28]
            test eax, eax
            je L00402655
            mov eax, dword ptr [gReentrancy_count]
            test eax, eax
            je L0040261d
            mov ecx, dword ptr [gLocal_buffer]
            mov edx, dword ptr [ecx]
            xor ecx, ecx
            mov dword ptr [eax*4 + gReentrancy_array_base_minus_one], edx
            call EndPipingSession2
    L0040261d:
            mov eax, dword ptr [gReentrancy_count]
            inc eax
            mov dword ptr [gReentrancy_count], eax
            mov eax, dword ptr [gLocal_buffer]
            mov dword ptr [eax], esi
            mov ecx, dword ptr [gLocal_buffer]
            mov dword ptr [ecx + 4], 0
            mov edx, dword ptr [gLocal_buffer]
            add edx, 8
            mov dword ptr [gLocal_buffer_size], 8
            mov dword ptr [gMr_chunky], edx
    L00402655:
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

// FUNCTION: CARMA2_HW 0x00402890
void C2_HOOK_FASTCALL AREndPipingSession(void) {
    EndPipingSession2(1);
}

// FUNCTION: CARMA2_HW 0x004030f0
C2_NAKED tPipe_chunk_data* C2_HOOK_FASTCALL FindNextChunk(tPipe_chunk_type pType, uintptr_t pOwner) {
#ifndef CARPOCALYPSE2_MATCHING
    tPipe_chunk* cur;
    tU32 i;

    cur = (tPipe_chunk*)gPipe_play_ptr;
    if (cur == (tPipe_chunk*)gPipe_record_ptr) {
        return NULL;
    }
    for (;;) {
        if (cur->type == pType) {
            tPipe_chunk_data* the_chunk;

            the_chunk = (tPipe_chunk_data*)((tU8*)cur + sizeof(tPipe_chunk));
            for (i = 0; i < cur->count; i++) {
                    if (the_chunk->count == (tU32)pOwner) {
                        return the_chunk;
                    } else {
                        tPipe_chunk_type chunk_type;
                        tU32 length;
                        tU32 size;

                        chunk_type = pType;
                        if (gPipe_callbacks[chunk_type].calc_length != NULL) {
                            length = gPipe_callbacks[chunk_type].calc_length((tPipe_chunk*)the_chunk);
                            size = length + gPipe_callbacks[chunk_type].length + sizeof(void*);
                        } else {
                            size = gPipe_callbacks[chunk_type].length + sizeof(void*);
                        }
                        the_chunk = (tPipe_chunk_data*)((tU8*)the_chunk + size);
                    }
                }
        }
        {
            tPipe_chunk* cur_session;
            tU32 total;

            total = 0;
            cur_session = cur;
            for (i = 0; i < cur_session->count; i++) {
                tPipe_chunk_type chunk_type;
                tPipe_chunk* the_chunk;
                tU32 length;
                tU32 size;

                chunk_type = cur_session->type;
                the_chunk = (tPipe_chunk*)((tU8*)cur_session + sizeof(tPipe_chunk) + total);
                if (gPipe_callbacks[chunk_type].calc_length != NULL) {
                    length = gPipe_callbacks[chunk_type].calc_length(the_chunk);
                    size = length + gPipe_callbacks[chunk_type].length + sizeof(void*);
                } else {
                    size = gPipe_callbacks[chunk_type].length + sizeof(void*);
                }
                total += size;
            }
            total += 10;
            if ((total & 1) != 0) {
                FatalError(0x62);
            }
            cur = (tPipe_chunk*)((tU8*)cur + total);
        }
        if ((tU8*)cur >= gPipe_buffer_working_end) {
            if (cur == (tPipe_chunk*)gPipe_record_ptr) {
                return NULL;
            }
            cur = (tPipe_chunk*)gPipe_buffer_start;
        }
        if (cur == (tPipe_chunk*)gPipe_record_ptr) {
            return NULL;
        }
    }
#else
    __asm {
            sub esp, 8
            mov eax, dword ptr [gPipe_record_ptr]
            push ebx
            push ebp
            push esi
            push edi
            mov edi, dword ptr [gPipe_play_ptr]
            mov esi, ecx
            cmp edi, eax
            mov dword ptr [esp + 0x14], edx
            mov dword ptr [esp + 0x10], esi
            je L004031fd
            mov edx, dword ptr [gPipe_callbacks]
    L0040311a:
            cmp dword ptr [edi], esi
            jne L0040316f
            mov eax, dword ptr [edi + 4]
            xor ebp, ebp
            test eax, eax
            lea ebx, [edi + 8]
            jbe L0040316f
    L0040312a:
            mov eax, dword ptr [esp + 0x14]
            mov ecx, dword ptr [ebx]
            cmp ecx, eax
            je L004031f3
            lea esi, [esi + esi*2]
            shl esi, 3
            mov eax, dword ptr [edx + esi + 8]
            test eax, eax
            je L0040315a
            mov ecx, ebx
            call eax
            mov edx, dword ptr [gPipe_callbacks]
            mov ecx, dword ptr [edx + esi + 4]
            lea esi, [eax + ecx + 4]
            jmp L00403161
    L0040315a:
            mov esi, dword ptr [edx + esi + 4]
            add esi, 4
    L00403161:
            mov eax, dword ptr [edi + 4]
            add ebx, esi
            mov esi, dword ptr [esp + 0x10]
            inc ebp
            cmp ebp, eax
            jb L0040312a
    L0040316f:
            mov eax, dword ptr [edi + 4]
            xor ebx, ebx
            xor ebp, ebp
            test eax, eax
            jbe L004031b5
    L0040317a:
            mov eax, dword ptr [edi]
            lea ecx, [ebx + edi + 8]
            lea esi, [eax + eax*2]
            shl esi, 3
            mov eax, dword ptr [edx + esi + 8]
            test eax, eax
            je L004031a0
            call eax
            mov edx, dword ptr [gPipe_callbacks]
            mov ecx, dword ptr [edx + esi + 4]
            lea esi, [eax + ecx + 4]
            jmp L004031a7
    L004031a0:
            mov esi, dword ptr [edx + esi + 4]
            add esi, 4
    L004031a7:
            mov eax, dword ptr [edi + 4]
            add ebx, esi
            inc ebp
            cmp ebp, eax
            jb L0040317a
            mov esi, dword ptr [esp + 0x10]
    L004031b5:
            add ebx, 0xa
            test bl, 1
            je L004031cd
            push 0x62
            call FatalError
            mov edx, dword ptr [gPipe_callbacks]
            add esp, 4
    L004031cd:
            mov eax, dword ptr [gPipe_buffer_working_end]
            add edi, ebx
            cmp edi, eax
            jb L004031e6
            cmp edi, dword ptr [gPipe_record_ptr]
            je L004031fd
            mov edi, dword ptr [gPipe_buffer_start]
    L004031e6:
            cmp edi, dword ptr [gPipe_record_ptr]
            je L004031fd
            jmp L0040311a
    L004031f3:
            mov eax, ebx
            pop edi
            pop esi
            pop ebp
            pop ebx
            add esp, 8
            ret
    L004031fd:
            pop edi
            pop esi
            pop ebp
            xor eax, eax
            pop ebx
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
    }
#endif
}

// FUNCTION: CARMA2_HW 0x00403750
tU32 C2_HOOK_FASTCALL LengthOfSession(tPipe_chunk* pSession) {
    tU32 i;
    tU32 running_total;
    tPipe_chunk* the_chunk;

    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tPipe_chunk, type, 0x0);
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tPipe_chunk, count, 0x4);
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tPipe_chunk, data, 0x8);

    running_total = 0;
    for (i = 0; i < pSession->count; i++) {
        the_chunk = (tPipe_chunk*)((tU8*)pSession + running_total + offsetof(tPipe_chunk, data));
        running_total += LengthOfChunk(the_chunk, pSession->type);
    }
    running_total += offsetof(tPipe_chunk, data) + sizeof(tU16);
    if (running_total & 0x1) {
        FatalError(0x62);
    }
    return running_total;
}

// FUNCTION: CARMA2_HW 0x00402660
void C2_NAKED C2_HOOK_FASTCALL EndPipingSession2(int pMunge_reentrancy) {
#ifndef CARPOCALYPSE2_MATCHING

    if (gPipe_buffer_start == NULL || gPed_676914 || !gProgram_state.racing) {
        return;
    }
    /* End session with buffer size */
    *(tS16*)(gLocal_buffer + gLocal_buffer_size) = gLocal_buffer_size;
    gLocal_buffer_size += sizeof(tS16);

    if (((tPipe_chunk*)gLocal_buffer)->count != 0 && gLocal_buffer_size <= 15000) {
        if (gPipe_buffer_phys_end < gPipe_record_ptr + gLocal_buffer_size) {
            gPipe_buffer_working_end = gPipe_record_ptr;
            gPipe_buffer_oldest = gPipe_buffer_start;
            gPipe_record_ptr = gPipe_buffer_start;
        }
        while (gPipe_record_ptr <= gPipe_buffer_oldest && gPipe_buffer_oldest < gPipe_record_ptr + gLocal_buffer_size) {
            // Remove older sessions
            gPipe_buffer_oldest += LengthOfSession((tPipe_chunk*)gPipe_buffer_oldest);

            if (gPipe_buffer_working_end <= gPipe_buffer_oldest) {
                gPipe_buffer_working_end = gPipe_buffer_phys_end;
                gPipe_buffer_oldest = gPipe_buffer_start;
            }
        }
        if (gPipe_buffer_oldest == NULL) {
            gPipe_buffer_oldest = gPipe_record_ptr;
        }
        memcpy(gPipe_record_ptr, gLocal_buffer, gLocal_buffer_size);
        gPipe_record_ptr += gLocal_buffer_size;
        if (gPipe_buffer_working_end < gPipe_record_ptr) {
            gPipe_buffer_working_end = gPipe_record_ptr;
        }
    }
    if (pMunge_reentrancy) {
        if (gReentrancy_count != 0) {
            gReentrancy_count -= 1;
        }
        if (gReentrancy_count != 0) {
            StartPipingSession2(gReentrancy_array[gReentrancy_count - 1], 0);
        }
    }
#else
    __asm {
            sub esp, 0x10
            mov eax, dword ptr [gPipe_buffer_start]
            push ebx
            push ebp
            push esi
            test eax, eax
            push edi
            mov dword ptr [esp + 0x1c], ecx
            je L00402883
            mov eax, dword ptr [gPed_676914]
            test eax, eax
            jne L00402883
            mov eax, dword ptr [gProgram_state+0x28]
            test eax, eax
            je L00402883
            mov eax, dword ptr [gLocal_buffer_size]
            mov ecx, dword ptr [gLocal_buffer]
            mov word ptr [eax + ecx], ax
            mov ecx, dword ptr [gLocal_buffer_size]
            mov edx, dword ptr [gLocal_buffer]
            add ecx, 2
            mov dword ptr [gLocal_buffer_size], ecx
            mov eax, dword ptr [edx + 4]
            test eax, eax
            je L00402819
            cmp ecx, 0x3a98
            ja L00402819
            mov eax, dword ptr [gPipe_record_ptr]
            mov esi, dword ptr [gPipe_buffer_phys_end]
            lea edx, [ecx + eax]
            cmp edx, esi
            jbe L004026fa
            mov edi, dword ptr [gPipe_buffer_start]
            mov dword ptr [gPipe_buffer_working_end], eax
            mov dword ptr [gPipe_buffer_oldest], edi
            mov dword ptr [gPipe_record_ptr], edi
            mov eax, edi
            jmp L00402700
    L004026fa:
            mov edi, dword ptr [gPipe_buffer_oldest]
    L00402700:
            cmp eax, edi
            ja L004027d9
            mov edx, dword ptr [gPipe_callbacks]
    L0040270e:
            lea esi, [ecx + eax]
            cmp esi, edi
            jbe L004027d9
            mov ecx, dword ptr [edi + 4]
            lea eax, [edi + 4]
            xor ebx, ebx
            mov ebp, edi
            test ecx, ecx
            mov dword ptr [esp + 0x10], ebx
            mov dword ptr [esp + 0x18], eax
            jbe L00402781
            lea eax, [edi + 8]
            mov dword ptr [esp + 0x14], eax
    L00402736:
            mov eax, dword ptr [ebp]
            mov ecx, dword ptr [esp + 0x14]
            add ecx, ebx
            lea esi, [eax + eax*2]
            shl esi, 3
            mov eax, dword ptr [edx + esi + 8]
            test eax, eax
            je L00402765
            call eax
            mov edx, dword ptr [gPipe_callbacks]
            mov edi, dword ptr [gPipe_buffer_oldest]
            mov ecx, dword ptr [edx + esi + 4]
            lea esi, [eax + ecx + 4]
            jmp L0040276c
    L00402765:
            mov esi, dword ptr [edx + esi + 4]
            add esi, 4
    L0040276c:
            mov ecx, dword ptr [esp + 0x18]
            mov eax, dword ptr [esp + 0x10]
            add ebx, esi
            inc eax
            mov esi, dword ptr [ecx]
            mov dword ptr [esp + 0x10], eax
            cmp eax, esi
            jb L00402736
    L00402781:
            add ebx, 0xa
            test bl, 1
            je L0040279f
            push 0x62
            call FatalError
            mov edi, dword ptr [gPipe_buffer_oldest]
            mov edx, dword ptr [gPipe_callbacks]
            add esp, 4
    L0040279f:
            mov eax, dword ptr [gPipe_buffer_working_end]
            add edi, ebx
            cmp edi, eax
            mov dword ptr [gPipe_buffer_oldest], edi
            jb L004027c6
            mov eax, dword ptr [gPipe_buffer_phys_end]
            mov edi, dword ptr [gPipe_buffer_start]
            mov dword ptr [gPipe_buffer_working_end], eax
            mov dword ptr [gPipe_buffer_oldest], edi
    L004027c6:
            mov eax, dword ptr [gPipe_record_ptr]
            mov ecx, dword ptr [gLocal_buffer_size]
            cmp eax, edi
            jbe L0040270e
    L004027d9:
            test edi, edi
            jne L004027e2
            mov dword ptr [gPipe_buffer_oldest], eax
    L004027e2:
            mov esi, dword ptr [gLocal_buffer]
            mov edx, ecx
            mov edi, eax
            shr ecx, 2
            rep movsd
            mov ecx, edx
            and ecx, 3
            rep movsb
            mov eax, dword ptr [gPipe_record_ptr]
            mov edx, dword ptr [gLocal_buffer_size]
            mov ecx, dword ptr [gPipe_buffer_working_end]
            add eax, edx
            cmp eax, ecx
            mov dword ptr [gPipe_record_ptr], eax
            jbe L00402819
            mov dword ptr [gPipe_buffer_working_end], eax
    L00402819:
            mov eax, dword ptr [esp + 0x1c]
            test eax, eax
            je L00402883
            mov eax, dword ptr [gReentrancy_count]
            test eax, eax
            je L00402883
            dec eax
            mov dword ptr [gReentrancy_count], eax
            je L00402883
            mov ecx, dword ptr [gPipe_buffer_start]
            mov eax, dword ptr [eax*4 + gReentrancy_array_base_minus_one]
            test ecx, ecx
            je L00402883
            mov ecx, dword ptr [gPed_676914]
            test ecx, ecx
            jne L00402883
            mov ecx, dword ptr [gProgram_state+0x28]
            test ecx, ecx
            je L00402883
            mov ecx, dword ptr [gLocal_buffer]
            mov dword ptr [ecx], eax
            mov edx, dword ptr [gLocal_buffer]
            mov dword ptr [edx + 4], 0
            mov eax, dword ptr [gLocal_buffer]
            add eax, 8
            mov dword ptr [gLocal_buffer_size], 8
            mov dword ptr [gMr_chunky], eax
    L00402883:
            pop edi
            pop esi
            pop ebp
            pop ebx
            add esp, 0x10
            ret
            nop
            nop
            nop
            nop
            nop
    }
#endif
}

// FUNCTION: CARMA2_HW 0x004028a0
void C2_HOOK_FASTCALL ARAddDataToSession(int pType, uintptr_t pOwner, void *pData, int pSize) {
    int new_size;

    if (gPipe_buffer_start == NULL || gPed_676914 || !gProgram_state.racing) {
        return;
    }
    new_size = gLocal_buffer_size + pSize + sizeof(void*);
    if ((unsigned int)new_size < 15000U) {
        ((tPipe_chunk*)gLocal_buffer)->count += 1;
        *((uintptr_t*)gMr_chunky) = pOwner;
        gMr_chunky += sizeof(uintptr_t);
        if (pSize != 0) {
            memcpy(gMr_chunky, pData, pSize);
        }
        gMr_chunky += pSize;
        gLocal_buffer_size = new_size;
    }
}

// FUNCTION: CARMA2_HW 0x00403720
int C2_HOOK_FASTCALL LengthOfChunk(void* pChunk, int pType) {
    if (gPipe_callbacks[pType].calc_length != NULL) {
        return gPipe_callbacks[pType].calc_length(pChunk) + gPipe_callbacks[pType].length + sizeof(void*);
    } else {
        return gPipe_callbacks[pType].length + sizeof(void*);
    }
}

// FUNCTION: CARMA2_HW 0x00402ab0
void C2_NAKED C2_HOOK_FASTCALL ARDoSingleSession(int pType, uintptr_t pOwner, void* pData, tU32 pSize) {
#ifndef CARPOCALYPSE2_MATCHING

    if (gPipe_buffer_start == NULL || gPed_676914 || !gProgram_state.racing) {
        return;
    }
    ARStartPipingSession((tPipe_chunk_type)pType);
    ARAddDataToSession(pType, pOwner, pData, pSize);
    EndPipingSession2(1);
#else
    __asm {
            mov eax, dword ptr [gPipe_buffer_start]
            push esi
            push edi
            mov edi, edx
            test eax, eax
            mov esi, ecx
            je L00402bbc
            mov eax, dword ptr [gPed_676914]
            test eax, eax
            jne L00402bbc
            mov eax, dword ptr [gProgram_state+0x28]
            test eax, eax
            je L00402bbc
            mov eax, dword ptr [gReentrancy_count]
            push ebx
            test eax, eax
            je L00402afd
            mov ecx, dword ptr [gLocal_buffer]
            mov edx, dword ptr [ecx]
            xor ecx, ecx
            mov dword ptr [eax*4 + gReentrancy_array_base_minus_one], edx
            call EndPipingSession2
    L00402afd:
            mov ecx, dword ptr [gReentrancy_count]
            mov eax, dword ptr [gLocal_buffer]
            inc ecx
            mov dword ptr [gReentrancy_count], ecx
            mov dword ptr [eax], esi
            mov ecx, dword ptr [gLocal_buffer]
            mov dword ptr [ecx + 4], 0
            mov ecx, dword ptr [gLocal_buffer]
            mov eax, dword ptr [gPipe_buffer_start]
            mov dword ptr [gLocal_buffer_size], 8
            lea edx, [ecx + 8]
            test eax, eax
            mov dword ptr [gMr_chunky], edx
            je L00402bb1
            mov eax, dword ptr [gPed_676914]
            test eax, eax
            jne L00402bb1
            mov eax, dword ptr [gProgram_state+0x28]
            test eax, eax
            je L00402bb1
            mov eax, dword ptr [esp + 0x14]
            lea edx, [eax + 0xc]
            cmp edx, 0x3a98
            jae L00402bb1
            mov ebx, dword ptr [ecx + 4]
            inc ebx
            mov dword ptr [ecx + 4], ebx
            mov ecx, dword ptr [gMr_chunky]
            mov dword ptr [ecx], edi
            mov esi, dword ptr [gMr_chunky]
            add esi, 4
            test eax, eax
            mov dword ptr [gMr_chunky], esi
            je L00402b9d
            mov esi, dword ptr [esp + 0x10]
            mov edi, dword ptr [gMr_chunky]
            mov ecx, eax
            mov ebx, ecx
            shr ecx, 2
            rep movsd
            mov ecx, ebx
            and ecx, 3
            rep movsb
    L00402b9d:
            mov ecx, dword ptr [gMr_chunky]
            mov dword ptr [gLocal_buffer_size], edx
            add ecx, eax
            mov dword ptr [gMr_chunky], ecx
    L00402bb1:
            mov ecx, 1
            call EndPipingSession2
            pop ebx
    L00402bbc:
            pop edi
            pop esi
            ret 8
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

// FUNCTION: CARMA2_HW 0x00402bd0
void C2_NAKED C2_HOOK_CDECL ARDoSingleVariedSession(int pType, uintptr_t pOwner, int pCount, ...) {
#ifndef CARPOCALYPSE2_MATCHING
    char buffer[5000];

    ARStartPipingSession(pType);
    if (gPipe_buffer_start != NULL && !gPed_676914 && gProgram_state.racing) {
        int i;
        va_list ap;

        va_start(ap, pCount);
        for (i = 0; i < pCount; i++) {
            int size;
            int offset;

            size = va_arg(ap, int);
            offset = va_arg(ap, int);
            if (size < 0) {
                uintptr_t pv = va_arg(ap, uintptr_t);
                memcpy(buffer + sizeof(uintptr_t) + offset, &pv, sizeof(pv));
            } else {
                uintptr_t pv = va_arg(ap, uintptr_t);
                if (size == 1) {
                    tU8 b = (tU8)pv;
                    memcpy(buffer + sizeof(uintptr_t) + offset, &b, sizeof(b));
                } else if (size == 2) {
                    tU16 s = (tU16)pv;
                    memcpy(buffer + sizeof(uintptr_t) + offset, &s, sizeof(s));
                } else if (size == 4) {
                    tU32 i = (tU32)pv;
                    memcpy(buffer + sizeof(uintptr_t) + offset, &i, sizeof(i));
                } else if (pv != 0) {
                    memcpy(buffer + sizeof(uintptr_t) + offset, (void*)pv, size);
                }
            }
        }
        va_end(ap);
        memcpy(buffer, &pOwner, sizeof(uintptr_t));

        ARAddDataToSession(pType, pOwner, buffer, LengthOfChunk(buffer, pType) - sizeof(void*));
    }
    AREndPipingSession();
#else
    __asm {
            mov eax, 0x1388
            call _chkstk
            mov eax, dword ptr [gPipe_buffer_start]
            push ebx
            push ebp
            mov ebp, dword ptr [esp + 0x1394]
            push esi
            push edi
            test eax, eax
            je L00402c57
            mov eax, dword ptr [gPed_676914]
            test eax, eax
            jne L00402c57
            mov eax, dword ptr [gProgram_state+0x28]
            test eax, eax
            je L00402c57
            mov eax, dword ptr [gReentrancy_count]
            test eax, eax
            je L00402c1f
            mov ecx, dword ptr [gLocal_buffer]
            mov edx, dword ptr [ecx]
            xor ecx, ecx
            mov dword ptr [eax*4 + gReentrancy_array_base_minus_one], edx
            call EndPipingSession2
    L00402c1f:
            mov eax, dword ptr [gReentrancy_count]
            inc eax
            mov dword ptr [gReentrancy_count], eax
            mov eax, dword ptr [gLocal_buffer]
            mov dword ptr [eax], ebp
            mov ecx, dword ptr [gLocal_buffer]
            mov dword ptr [ecx + 4], 0
            mov edx, dword ptr [gLocal_buffer]
            add edx, 8
            mov dword ptr [gLocal_buffer_size], 8
            mov dword ptr [gMr_chunky], edx
    L00402c57:
            mov eax, dword ptr [gPipe_buffer_start]
            test eax, eax
            je L00402db2
            mov eax, dword ptr [gPed_676914]
            test eax, eax
            jne L00402db2
            mov eax, dword ptr [gProgram_state+0x28]
            test eax, eax
            je L00402db2
            mov ecx, dword ptr [esp + 0x13a4]
            lea eax, [esp + 0x13a8]
            test ecx, ecx
            jle L00402cec
            mov ebx, ecx
    L00402c92:
            mov ecx, dword ptr [eax]
            add eax, 4
            add eax, 4
            test ecx, ecx
            mov edi, dword ptr [eax - 4]
            jge L00402cac
            fld dword ptr [eax]
            add eax, 4
            fstp dword ptr [esp + edi + 0x10]
            jmp L00402ce9
    L00402cac:
            mov edx, dword ptr [eax]
            add eax, 4
            mov esi, ecx
            dec esi
            je L00402ce5
            dec esi
            je L00402cde
            sub esi, 2
            je L00402cd8
            test edx, edx
            je L00402ce9
            mov esi, edx
            mov edx, ecx
            lea edi, [esp + edi + 0x14]
            shr ecx, 2
            rep movsd
            mov ecx, edx
            and ecx, 3
            rep movsb
            jmp L00402ce9
    L00402cd8:
            mov dword ptr [esp + edi + 0x14], edx
            jmp L00402ce9
    L00402cde:
            mov word ptr [esp + edi + 0x14], dx
            jmp L00402ce9
    L00402ce5:
            mov byte ptr [esp + edi + 0x14], dl
    L00402ce9:
            dec ebx
            jne L00402c92
    L00402cec:
            mov edi, dword ptr [esp + 0x13a0]
            mov ecx, dword ptr [gPipe_callbacks]
            lea esi, [ebp + ebp*2]
            mov dword ptr [esp + 0x10], edi
            shl esi, 3
            mov eax, dword ptr [ecx + esi + 8]
            test eax, eax
            je L00402d22
            lea ecx, [esp + 0x10]
            call eax
            mov ecx, dword ptr [gPipe_callbacks]
            mov edx, dword ptr [ecx + esi + 4]
            lea esi, [eax + edx + 4]
            jmp L00402d29
    L00402d22:
            mov esi, dword ptr [ecx + esi + 4]
            add esi, 4
    L00402d29:
            mov ecx, dword ptr [gPipe_buffer_start]
            lea eax, [esi - 4]
            test ecx, ecx
            je L00402db2
            mov ecx, dword ptr [gPed_676914]
            test ecx, ecx
            jne L00402db2
            mov ecx, dword ptr [gProgram_state+0x28]
            test ecx, ecx
            je L00402db2
            mov ecx, dword ptr [gLocal_buffer_size]
            lea edx, [eax + ecx + 4]
            cmp edx, 0x3a98
            jae L00402db2
            mov ecx, dword ptr [gLocal_buffer]
            mov ebx, dword ptr [ecx + 4]
            inc ebx
            mov dword ptr [ecx + 4], ebx
            mov ecx, dword ptr [gMr_chunky]
            mov dword ptr [ecx], edi
            mov esi, dword ptr [gMr_chunky]
            add esi, 4
            test eax, eax
            mov dword ptr [gMr_chunky], esi
            je L00402d9e
            mov edi, dword ptr [gMr_chunky]
            mov ecx, eax
            mov ebx, ecx
            lea esi, [esp + 0x14]
            shr ecx, 2
            rep movsd
            mov ecx, ebx
            and ecx, 3
            rep movsb
    L00402d9e:
            mov ecx, dword ptr [gMr_chunky]
            mov dword ptr [gLocal_buffer_size], edx
            add ecx, eax
            mov dword ptr [gMr_chunky], ecx
    L00402db2:
            mov ecx, 1
            call EndPipingSession2
            pop edi
            pop esi
            pop ebp
            pop ebx
            add esp, 0x1388
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

// FUNCTION: CARMA2_HW 0x00402930
void C2_HOOK_CDECL ARAddVariedDataToSession(int pType, uintptr_t pOwner, int pCount, ...) {
    va_list ap;
    int i;
    int size;
    int offset;
    void* data;
    float value;
    tU8 buffer[5000];

    if (gPipe_buffer_start != NULL && !gPed_676914 && gProgram_state.racing) {
        va_start(ap, pCount);
        for (i = 0; i < pCount; i++) {
            size = va_arg(ap, int);
            offset = va_arg(ap, int);
            if (size < 0) {
                value = va_arg(ap, tFloat_u32_union).f;
                *(float*)&buffer[offset] = value;
            } else {
                data = va_arg(ap, void*);
                switch (size) {
                case 1:
                    *(tU8*)&buffer[sizeof(uintptr_t) + offset] = (tU8)(uintptr_t)data;
                    break;
                case 2:
                    *(tU16*)&buffer[sizeof(uintptr_t) + offset] = (tU16)(uintptr_t)data;
                    break;
                case 4:
                    *(tU32*)&buffer[sizeof(uintptr_t) + offset] = (tU32)(uintptr_t)data;
                    break;
                default:
                    if (data != NULL) {
                        memcpy(&buffer[sizeof(uintptr_t) + offset], data, size);
                    }
                    break;
                }
            }
        }
        va_end(ap);
        *(uintptr_t*)buffer = pOwner;

        ARAddDataToSession(pType, pOwner, &buffer[sizeof(uintptr_t)], LengthOfChunk(buffer, pType) - sizeof(uintptr_t));
    }
}

// FUNCTION: CARMA2_HW 0x004c6580
void C2_HOOK_FASTCALL InitLastDamageArrayEtc(void) {
    int i;
    int j;
    int cat;
    int car_count;
    tCar_spec* car;

    C2_HOOK_BUG_ON(CARPOCALYPSE2_ASIZE(car->frame_start_damage) != 12);

    for (cat = eVehicle_self; cat <= eVehicle_not_really; cat++) {
        if (cat == eVehicle_self) {
            car_count = 1;
        } else {
            car_count = GetCarCount(cat);
        }
        for (i = 0; i < car_count; i++) {
            if (cat == eVehicle_self) {
                car = &gProgram_state.current_car;
            } else {
                car = GetCarSpec(cat, i);
            }
            if (cat != eVehicle_not_really) {
                for (j = 0; j < CARPOCALYPSE2_ASIZE(car->frame_start_damage); j++) {
                    car->frame_start_damage[j] = 0;
                }
            }
            car->car_ID = (cat << 8) + i;
        }
    }
}

// FUNCTION: CARMA2_HW 0x004c8600
void C2_HOOK_FASTCALL PipeSingleSkidAdjustment(int pSkid_num, br_matrix34* pMatrix, br_material* pMaterial) {
    ARDoSingleVariedSession(0x16, pSkid_num, 2,
        0x30, 0, pMatrix,
        4, 0x30, pMaterial);
}

void C2_HOOK_FASTCALL AddNonCarToPipingSession(tPhysics_object* pObject, br_actor* pActor) {
    tNon_car_spec* non_car;

    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tNon_car_spec, car_ID, 0x80);

    non_car = (tNon_car_spec*)pActor->type_data;
    ARAddVariedDataToSession(ePipe_chunk_non_car, (uintptr_t)non_car->car_ID, 2,
        SIZE_OFFSET_PIPING(tPipe_chunk_non_car, actor), pActor,
        SIZE_OFFSET_PIPING(tPipe_chunk_non_car, matrix), &pActor->t.t.mat);
}

// FUNCTION: CARMA2_HW 0x004c6ff0
void C2_HOOK_FASTCALL AddNonCarActorToPipingSession(int pCar_ID, br_actor* pActor) {
    ARAddVariedDataToSession(ePipe_chunk_non_car, (uintptr_t)pCar_ID, 2,
        SIZE_OFFSET_PIPING(tPipe_chunk_non_car, actor), pActor,
        SIZE_OFFSET_PIPING(tPipe_chunk_non_car, matrix), &pActor->t.t.mat);
}

// FUNCTION: CARMA2_HW 0x004c68a0
int C2_HOOK_FASTCALL PipeObjectPosition(tPhysics_object* pObject, void* pContext) {
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tNon_car_spec, car_ID, 0x80);

    if (pObject != NULL && pObject->owner != NULL && pObject->flags_0x238 == 1) {
        AddNonCarActorToPipingSession((tU16)((tNon_car_spec*)pObject->actor->type_data)->car_ID, pObject->actor);
    } else if (pObject != NULL && pObject->owner != NULL && pObject->flags_0x238 == 0x20) {
        AddNonCarActorToPipingSession((int)(uintptr_t)pObject, pObject->actor);
    }
    return 0;
}

void C2_HOOK_FASTCALL AddCarToPipingSession(int pCar_id, br_matrix34* pMatrix, br_vector3* pV, float pSpeedo_speed, float pRevs, int pGear, int pFrame_collision_flag, undefined4 pArg7) {
    tPipe_car_chunk chunk;
    chunk.revs = (int)pRevs / 10;
    chunk.frame_collision_flag = pFrame_collision_flag;
    chunk.gear = pGear + 1;

    ARAddVariedDataToSession(ePipe_chunk_car, (uintptr_t)pCar_id, 5,
        SIZE_OFFSET_PIPING(tPipe_car_chunk, mat), pMatrix,
        SIZE_OFFSET_PIPING(tPipe_car_chunk, v), pV,
        SIZE_OFFSET_PIPING(tPipe_car_chunk, speedo_speed), (int)(pSpeedo_speed * 468100.f),
        SIZE_OFFSET_PIPING(tPipe_car_chunk, flags), chunk.flags,
        SIZE_OFFSET_PIPING(tPipe_car_chunk, field_0x40), chunk.flags);
}

void C2_HOOK_FASTCALL AddDamageToPipingSession(int pCar_id, tU8 *pDamage_deltas) {
    tU8 damage_deltas[12];

    C2_HOOK_BUG_ON(sizeof(damage_deltas) != 12);

    memcpy(damage_deltas, pDamage_deltas, sizeof(damage_deltas));
    ARAddDataToSession(ePipe_chunk_damage, (uintptr_t)pCar_id, damage_deltas, sizeof(damage_deltas));
}

// FUNCTION: CARMA2_HW 0x004c6640
void C2_NAKED C2_HOOK_FASTCALL PipeCarPositions(void) {
#ifndef CARPOCALYPSE2_MATCHING
    int category;
    int car_count;
    int car_index;
    int session_started;

    ARStartPipingSession(ePipe_chunk_car);
    for (category = eVehicle_self; category < eVehicle_not_really; category++) {

        car_count = category == eVehicle_self ? 1 : GetCarCount(category);
        for (car_index = 0; car_index < car_count; car_index++) {
            tCar_spec* car;

            car = category == eVehicle_self ? &gProgram_state.current_car : GetCarSpec(category, car_index);
            AddCarToPipingSession((category << 8) | car_index,
                &car->car_master_actor->t.t.mat,
                &car->collision_info->v,
                car->speedo_speed,
                car->revs,
                car->gear,
                car->frame_collision_flag,
                car->field_0x18cc);
        }
    }
    AREndPipingSession();

    session_started = 0;
    category = eVehicle_self;
    car_count = category == eVehicle_self ? 1 : GetCarCount(category);
    for (car_index = 0; car_index < car_count; car_index++) {
        tCar_spec *car;

        car = category == eVehicle_self ? &gProgram_state.current_car : GetCarSpec(category, car_index);
        if (car->active) {
            size_t i;
            tU8 damage_deltas[12];
            int any_damage;

            C2_HOOK_BUG_ON(CARPOCALYPSE2_ASIZE(car->damage_units) != 12);

            any_damage = 0;
            for (i = 0; i < CARPOCALYPSE2_ASIZE(car->damage_units); i++) {
                int damage;
                int damage_delta;

                damage = car->damage_units[i].damage_level;
                damage_delta = damage - car->frame_start_damage[i];
                car->frame_start_damage[i] = damage;
                damage_deltas[i] = damage_delta;
                any_damage |= damage_delta;
            }
            if (any_damage) {
                if (!session_started) {
                    ARStartPipingSession(ePipe_chunk_damage);
                    session_started = 1;
                }
                AddDamageToPipingSession((category << 8) | car_index, damage_deltas);
            }
        }
    }
    if (session_started) {
        AREndPipingSession();
    }

    ARStartPipingSession(ePipe_chunk_non_car);
    for (category = eVehicle_self; category < eVehicle_not_really; category++) {

        car_count = category == eVehicle_self ? 1 : GetCarCount(category);
        for (car_index = 0; car_index < car_count; car_index++) {
            tCar_spec *car;

            car = category == eVehicle_self ? &gProgram_state.current_car : GetCarSpec(category, car_index);
            PhysicsObjectRecurseChildren(car->collision_info, PipeObjectPosition, NULL);
        }
    }
    AREndPipingSession();
#else
    __asm {
            sub esp, 0x24
            push ebx
            push ebp
            push esi
            push edi
            mov ecx, 5
            call ARStartPipingSession
            xor esi, esi
            mov dword ptr [esp + 0x10], esi
    L004c6657:
            test esi, esi
            jne L004c6662
            mov ebp, 1
            jmp L004c666b
    L004c6662:
            mov ecx, esi
            call GetCarCount
            mov ebp, eax
    L004c666b:
            xor edi, edi
            test ebp, ebp
            jle L004c6726
            mov ebx, esi
            shl ebx, 8
    L004c667a:
            test esi, esi
            jne L004c6685
            mov esi, offset gProgram_state+0xac
            jmp L004c6690
    L004c6685:
            mov edx, edi
            mov ecx, esi
            call GetCarSpec
            mov esi, eax
    L004c6690:
            mov eax, dword ptr [esi + 0x18cc]
            fld dword ptr [esi + 0x1340]
            push eax
            push 0x40
            push 4
            call _ftol
            mov ecx, eax
            mov eax, 0x66666667
            imul ecx
            fld dword ptr [esi + 0x4a4]
            mov eax, dword ptr [esi + 0x135c]
            sar edx, 2
            mov ecx, edx
            shr ecx, 0x1f
            add edx, ecx
            mov ecx, dword ptr [esi + 0x64]
            and edx, 0x7ff
            inc eax
            shl eax, 0xc
            add edx, eax
            neg ecx
            sbb ecx, ecx
            and ecx, 0x800
            add edx, ecx
            fmul qword ptr [gDouble_0058a908]
            push edx
            push 0x3e
            push 2
            call _ftol
            mov edx, dword ptr [esi + 8]
            push eax
            mov eax, dword ptr [esi + 0x10]
            push 0x3c
            add edx, 0x68
            push 2
            push edx
            push 0x30
            add eax, 0x2c
            push 0xc
            push eax
            push 0
            push 0x30
            push 5
            push ebx
            push 5
            call ARAddVariedDataToSession
            mov esi, dword ptr [esp + 0x58]
            add esp, 0x48
            inc edi
            inc ebx
            cmp edi, ebp
            jl L004c667a
    L004c6726:
            inc esi
            cmp esi, 4
            mov dword ptr [esp + 0x10], esi
            jle L004c6657
            call AREndPipingSession
            xor esi, esi
            mov dword ptr [esp + 0x18], 0
            mov dword ptr [esp + 0x10], esi
            test esi, esi
    L004c6749:
            jne L004c6755
            mov dword ptr [esp + 0x14], 1
            jmp L004c6760
    L004c6755:
            mov ecx, esi
            call GetCarCount
            mov dword ptr [esp + 0x14], eax
    L004c6760:
            mov eax, dword ptr [esp + 0x14]
            xor ebp, ebp
            test eax, eax
            jle L004c681f
    L004c676e:
            test esi, esi
            jne L004c6779
            mov eax, offset gProgram_state+0xac
            jmp L004c6782
    L004c6779:
            mov edx, ebp
            mov ecx, esi
            call GetCarSpec
    L004c6782:
            mov ecx, dword ptr [eax + 0x1d0]
            test ecx, ecx
            je L004c6812
            xor edi, edi
            xor ecx, ecx
            lea esi, [eax + 0x4e0]
    L004c679a:
            mov bl, byte ptr [esi]
            add esi, 0x2c
            mov dl, bl
            sub dl, byte ptr [ecx + eax + 0x6e8]
            mov byte ptr [ecx + eax + 0x6e8], bl
            mov byte ptr [esp + ecx + 0x1c], dl
            movsx edx, dl
            or edi, edx
            inc ecx
            cmp ecx, 0xc
            jl L004c679a
            test edi, edi
            je L004c680e
            mov eax, dword ptr [esp + 0x18]
            test eax, eax
            jne L004c67dc
            mov ecx, 7
            call ARStartPipingSession
            mov dword ptr [esp + 0x18], 1
    L004c67dc:
            mov edx, dword ptr [esp + 0x24]
            mov eax, dword ptr [esp + 0x1c]
            mov ecx, dword ptr [esp + 0x20]
            mov dword ptr [esp + 0x30], edx
            mov edx, dword ptr [esp + 0x10]
            mov dword ptr [esp + 0x28], eax
            lea eax, [esp + 0x28]
            mov dword ptr [esp + 0x2c], ecx
            shl edx, 8
            push 0xc
            push eax
            add edx, ebp
            mov ecx, 7
            call ARAddDataToSession
    L004c680e:
            mov esi, dword ptr [esp + 0x10]
    L004c6812:
            mov eax, dword ptr [esp + 0x14]
            inc ebp
            cmp ebp, eax
            jl L004c676e
    L004c681f:
            inc esi
            test esi, esi
            mov dword ptr [esp + 0x10], esi
            jle L004c6749
            mov eax, dword ptr [esp + 0x18]
            test eax, eax
            je L004c6839
            call AREndPipingSession
    L004c6839:
            mov ecx, 0xe
            call ARStartPipingSession
            xor ebx, ebx
    L004c6845:
            test ebx, ebx
            jne L004c6850
            mov edi, 1
            jmp L004c6859
    L004c6850:
            mov ecx, ebx
            call GetCarCount
            mov edi, eax
    L004c6859:
            xor esi, esi
            test edi, edi
            jle L004c6887
    L004c685f:
            test ebx, ebx
            jne L004c686a
            mov eax, offset gProgram_state+0xac
            jmp L004c6873
    L004c686a:
            mov edx, esi
            mov ecx, ebx
            call GetCarSpec
    L004c6873:
            mov ecx, dword ptr [eax + 8]
            push 0
            mov edx, offset PipeObjectPosition
            call PhysicsObjectRecurseChildren
            inc esi
            cmp esi, edi
            jl L004c685f
    L004c6887:
            inc ebx
            cmp ebx, 4
            jle L004c6845
            call AREndPipingSession
            pop edi
            pop esi
            pop ebp
            pop ebx
            add esp, 0x24
            ret
            nop
            nop
            nop
            nop
            nop
            nop
    }
#endif
}

#pragma auto_inline(off)
// FUNCTION: CARMA2_HW 0x00402df0
tU8* C2_HOOK_FASTCALL ARGetPipePlayPtr(void) {
    return gPipe_play_ptr;
}
#pragma auto_inline(on)

// FUNCTION: CARMA2_HW 0x004023b0
int C2_HOOK_FASTCALL ARGetReplayDirection(void) {
    return gPlay_direction;
}

#pragma auto_inline(off)
// FUNCTION: CARMA2_HW 0x00402300
int C2_HOOK_FASTCALL ARReplayForwards(void) {
    if (gPipe_play_ptr == gPipe_record_ptr) {
        return 0;
    }
    if (gPipe_play_ptr == gPipe_buffer_oldest) {
        return 1;
    }
    if (gReplay_rate != gFLOAT_00589330) {
        if (gReplay_rate > gFLOAT_00589330) {
            return 1;
        } else {
            return 0;
        }
    } else {
        return gPlay_direction > 0;
    }
}
#pragma auto_inline(on)

// FUNCTION: CARMA2_HW 0x00402540
int C2_HOOK_FASTCALL ARReplayPlaying(void) {
    if (gPipe_play_ptr == gPipe_record_ptr) {
        return 0;
    }
    if (gPipe_play_ptr == gPipe_buffer_oldest) {
        return 1;
    }
    if (gReplay_rate != gFLOAT_00589330) {
        if (gReplay_rate > gFLOAT_00589330) {
            return 1;
        } else {
            return 0;
        }
    } else {
        return gPlay_direction > 0;
    }
}

// GLOBAL: CARMA2_HW 0x006940fc
static tU8* gPipe_car_state_chunk;

static int C2_HOOK_FASTCALL CheckPipeCarState(tPipe_chunk* pChunk, int pChunk_count, tU32 pTime);

// FUNCTION: CARMA2_HW 0x004c6c90
int C2_HOOK_FASTCALL GetPipeCarStateAtTime(tS32 pOffset, tS32* pMode, br_scalar* pTowards, undefined4* pData, tS32* pTime) {
    tU8* temp_ptr;

    temp_ptr = ARGetPipePlayPtr();
    gTrigger_time = 0;
    if (ARReplayForwards()) {
        gOldest_time = GetTotalTime() + pOffset;
    } else {
        gOldest_time = GetTotalTime() - pOffset;
    }
    ARScanBuffer(&temp_ptr, ePipe_chunk_ped_incident, GetTotalTime(), CheckPipeCarState, NULL);
    if (gTrigger_time != 0) {
        *pTime = gTrigger_time - GetTotalTime();
        *pMode = *(tS32*)gPipe_car_state_chunk;
        *pTowards = *(br_scalar*)(gPipe_car_state_chunk + 4);
        switch (*pMode) {
        case 0:
            {
                tPedestrian* ped = *(tPedestrian**)(gPipe_car_state_chunk + 8);
                br_actor* actor = GetCharacterActorPtr(ped->character);

                pData[0] = (undefined4)(uintptr_t)actor;
                if (actor == NULL) {
                    gTrigger_time = 0;
                }
                pData[1] = *(undefined4*)(gPipe_car_state_chunk + 0xc);
            }
            break;
        case 1:
            {
                tU32 car_id = *(tU16*)(gPipe_car_state_chunk + 8);
                tCar_spec* car;

                if ((car_id & 0xffffff00) == 0) {
                    car = &gProgram_state.current_car;
                } else {
                    car = GetCarSpec(car_id >> 8, car_id & 0xff);
                }
                pData[0] = (undefined4)(uintptr_t)car;
                pData[1] = *(undefined4*)(gPipe_car_state_chunk + 0xc);
                pData[2] = *(undefined4*)(gPipe_car_state_chunk + 0x10);
                pData[3] = *(undefined4*)(gPipe_car_state_chunk + 0x14);
            }
            break;
        case 2:
            pData[0] = *(undefined4*)(gPipe_car_state_chunk + 8);
            pData[1] = *(undefined4*)(gPipe_car_state_chunk + 0xc);
            pData[2] = *(undefined4*)(gPipe_car_state_chunk + 0x10);
            break;
        default:
            break;
        }
    }
    return gTrigger_time;
}

// FUNCTION: CARMA2_HW 0x004c6e00
static int C2_HOOK_FASTCALL CheckPipeCarState(tPipe_chunk* pChunk, int pChunk_count, tU32 pTime) {
    if (ARReplayForwards()) {
        if (pTime <= gOldest_time) {
            return 0;
        }
    } else {
        if (pTime >= gOldest_time) {
            return 0;
        }
    }
    gTrigger_time = pTime;
    gPipe_car_state_chunk = (tU8*)pChunk;
    return 1;
}

// FUNCTION: CARMA2_HW 0x004c6bc0
int C2_HOOK_FASTCALL CarTimeout(tU32 pTime) {
    if (ARReplayForwards()) {
        if (pTime > gYoungest_time) {
            return 0;
        }
    } else {
        if (pTime < gYoungest_time) {
            return 0;
        }
    }
    return 1;
}

// FUNCTION: CARMA2_HW 0x004c6a30
int C2_NAKED C2_HOOK_FASTCALL CheckCar(tPipe_chunk* pChunk_ptr, int pChunk_count, tU32 pTime) {
#ifndef CARPOCALYPSE2_MATCHING
#pragma inline_depth(0)

    int i;
    tCar_spec* car;
    br_vector3 com_offset_c;
    br_vector3 com_offset_w;
    br_vector3 difference;
    tPipe_chunk* temp_ptr;
    tU32 subject_index;
    br_matrix34* transformation;

    temp_ptr = pChunk_ptr;
    if (ARReplayForwards()) {
        if (pTime <= gOldest_time) {
            return 0;
        }
    } else {
        if (pTime >= gOldest_time) {
            return 0;
        }
    }
    for (i = 0; i < pChunk_count; i++) {
        subject_index = *(tU32*)((tU8*)temp_ptr);
        transformation = (br_matrix34*)((tU8*)temp_ptr + sizeof(tU32));
        if ((subject_index & 0xffffff00) == 0) {
            car = &gProgram_state.current_car;
        } else {
            car = GetCarSpec(subject_index >> 8, subject_index & 0xff);
        }
        if (car == gCar_ptr) {
            BrVector3Copy(&gCar_pos, (br_vector3*)&transformation->m[3][0]);
            BrVector3InvScale(&com_offset_c, &car->pos, WORLD_SCALE);
            BrMatrix34ApplyV(&com_offset_w, &com_offset_c, transformation);
            BrVector3Accumulate(&gCar_pos, &com_offset_w);
            BrVector3Sub(&difference, &gCar_pos, &gReference_pos);
            gTrigger_time = pTime;
            if (BrVector3LengthSquared(&difference) <= gMax_distance) {
                return 0;
            } else {
                return 1;
            }
        }
        ARAdvanceChunkPtr(&temp_ptr, ePipe_chunk_car);
    }
    return 0;
#pragma inline_depth()
#else
    __asm {
            sub esp, 0x1c
            push ebx
            push esi
            push edi
            mov edi, edx
            mov dword ptr [esp + 0xc], ecx
            call ARReplayForwards
            mov ebx, dword ptr [esp + 0x2c]
            test eax, eax
            mov eax, dword ptr [gOldest_time]
            je L004c6a5d
            cmp ebx, eax
            ja L004c6a65
            xor eax, eax
            pop edi
            pop esi
            pop ebx
            add esp, 0x1c
            ret 4
    L004c6a5d:
            cmp ebx, eax
            jae L004c6ba7
    L004c6a65:
            xor esi, esi
            test edi, edi
            jle L004c6ba7
    L004c6a6f:
            mov eax, dword ptr [esp + 0xc]
            mov ecx, dword ptr [eax]
            test ecx, 0xffffff00
            jne L004c6a84
            mov eax, offset gProgram_state+0xac
            jmp L004c6a94
    L004c6a84:
            mov edx, ecx
            and edx, 0xff
            shr ecx, 8
            call GetCarSpec
    L004c6a94:
            cmp eax, dword ptr [gCar_ptr]
            je L004c6aba
            mov edx, 5
            lea ecx, [esp + 0xc]
            call ARAdvanceChunkPtr
            inc esi
            cmp esi, edi
            jl L004c6a6f
            xor eax, eax
            pop edi
            pop esi
            pop ebx
            add esp, 0x1c
            ret 4
    L004c6aba:
            mov ecx, dword ptr [esp + 0xc]
            lea esi, [esp + 0x10]
            mov edx, dword ptr [ecx + 0x28]
            mov dword ptr [gCar_pos], edx
            mov edi, dword ptr [ecx + 0x2c]
            lea edx, [ecx + 4]
            mov dword ptr [gCar_pos+0x4], edi
            mov ecx, dword ptr [ecx + 0x30]
            push edx
            mov dword ptr [gCar_pos+0x8], ecx
            lea edx, [esp + 0x20]
            fld dword ptr [eax + 0x58]
            fmul dword ptr [gDouble_0058a908+0xc]
            push esi
            push edx
            fstp dword ptr [esp + 0x1c]
            fld dword ptr [eax + 0x5c]
            fmul dword ptr [gDouble_0058a908+0xc]
            fstp dword ptr [esp + 0x20]
            fld dword ptr [eax + 0x60]
            fmul dword ptr [gDouble_0058a908+0xc]
            fstp dword ptr [esp + 0x24]
            call BrMatrix34ApplyV
            fld dword ptr [esp + 0x2c]
            fld dword ptr [esp + 0x28]
            fadd dword ptr [gCar_pos]
            fxch st(1)
            fadd dword ptr [gCar_pos+0x4]
            fld dword ptr [esp + 0x30]
            fadd dword ptr [gCar_pos+0x8]
            fxch st(2)
            fst dword ptr [gCar_pos]
            fsub dword ptr [gReference_pos]
            fxch st(1)
            fst dword ptr [gCar_pos+0x4]
            fsub dword ptr [gReference_pos+0x4]
            fxch st(2)
            fst dword ptr [gCar_pos+0x8]
            fsub dword ptr [gReference_pos+0x8]
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
            add esp, 0xc
            mov dword ptr [gTrigger_time], ebx
            faddp st(2), st(0)
            fxch st(1)
            fcomp dword ptr [gMax_distance]
            fnstsw ax
            test ah, 0x41
            fstp st(0)
            jne L004c6ba7
            mov eax, 1
            pop edi
            pop esi
            pop ebx
            add esp, 0x1c
            ret 4
    L004c6ba7:
            pop edi
            pop esi
            xor eax, eax
            pop ebx
            add esp, 0x1c
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
            nop
            nop
            nop
            nop
            nop
    }
#endif
}

// FUNCTION: CARMA2_HW 0x00402e10
int C2_NAKED C2_HOOK_FASTCALL ARScanBuffer(tU8** pPtr, tPipe_chunk_type pType, tU32 pDefault_time, tARScanBuffer_callback* pCallback, tARScanBuffer_time_check* pTime_check) {
#ifndef CARPOCALYPSE2_MATCHING
    tPipe_chunk* chunk;
    tU8* cur;
    int direction;
    int total;
    int len;
    int i;

    for (;;) {
        if (gPipe_play_ptr == gPipe_record_ptr) {
            direction = 0;
        } else if (gPipe_play_ptr == gPipe_buffer_oldest) {
            direction = 1;
        } else if (gReplay_rate != 0.0f) {
            if (gReplay_rate > 0.0f) {
                direction = 1;
            } else {
                direction = 0;
            }
        } else {
            direction = (gPlay_direction > 0);
        }

        if (direction) {
            cur = *pPtr;
            chunk = (tPipe_chunk*)cur;
            total = 0;
            for (i = 0; i < chunk->count; i++) {
                if (gPipe_callbacks[chunk->type].calc_length) {
                    len = gPipe_callbacks[chunk->type].calc_length((tPipe_chunk*)&chunk->data[total]) + gPipe_callbacks[chunk->type].length + 4;
                } else {
                    len = gPipe_callbacks[chunk->type].length + 4;
                }
                total += len;
            }
            total += 10;
            if (total & 1) {
                FatalError(98);
            }
            *pPtr = cur + total;
            if (!(*pPtr < gPipe_buffer_working_end) && *pPtr != gPipe_record_ptr) {
                *pPtr = gPipe_buffer_start;
            }
            if (*pPtr == gPipe_record_ptr) {
                return 0;
            }
        } else {
            if (*pPtr == gPipe_buffer_oldest && *pPtr != gPipe_record_ptr) {
                return 0;
            }
            if (*pPtr == gPipe_buffer_start) {
                *pPtr = gPipe_buffer_working_end;
            }
            cur = *pPtr - 2;
            *pPtr = cur - *(tU16*)cur;
        }

        cur = *pPtr;
        chunk = (tPipe_chunk*)cur;
        total = 0;
        for (i = 0; i < chunk->count; i++) {
            if (gPipe_callbacks[chunk->type].calc_length) {
                len = gPipe_callbacks[chunk->type].calc_length((tPipe_chunk*)&chunk->data[total]) + gPipe_callbacks[chunk->type].length + 4;
            } else {
                len = gPipe_callbacks[chunk->type].length + 4;
            }
            total += len;
        }
        total += 10;
        if (total & 1) {
            FatalError(98);
        }
        gPipe_buffer_end_marker = cur + total - 2;

        if (chunk->type == 0) {
            pDefault_time = *(tU32*)(cur + 0xc);
        } else if (chunk->type == pType) {
            int result;
            if ((result = pCallback((tPipe_chunk*)&chunk->data[0], chunk->count, pDefault_time))) {
                return result;
            }
        }

        if (pTime_check != NULL && pTime_check(pDefault_time)) {
            continue;
        }
        return 0;
    }
#else
    __asm {
            sub esp, 8
            mov eax, dword ptr [esp + 0xc]
            push ebx
            push ebp
            push esi
            mov esi, ecx
            push edi
            mov dword ptr [esp + 0x14], edx
            mov dword ptr [esp + 0x10], esi
            mov dword ptr [esp + 0x1c], eax
    L00402e29:
            mov edx, dword ptr [gPipe_callbacks]
    L00402e2f:
            mov eax, dword ptr [gPipe_play_ptr]
            mov edi, dword ptr [gPipe_record_ptr]
            mov ecx, dword ptr [gPipe_buffer_oldest]
            cmp eax, edi
            jne L00402e48
            xor eax, eax
            jmp L00402e91
    L00402e48:
            cmp eax, ecx
            jne L00402e53
            mov eax, 1
            jmp L00402e91
    L00402e53:
            fld dword ptr [gReplay_rate]
            fcomp dword ptr [gFLOAT_00589330]
            fnstsw ax
            test ah, 0x40
            jne L00402e84
            fld dword ptr [gReplay_rate]
            fcomp dword ptr [gFLOAT_00589330]
            fnstsw ax
            test ah, 0x41
            jne L00402e80
            mov eax, 1
            jmp L00402e91
    L00402e80:
            xor eax, eax
            jmp L00402e91
    L00402e84:
            mov ebx, dword ptr [gPlay_direction]
            xor eax, eax
            test ebx, ebx
            setg al
    L00402e91:
            test eax, eax
            je L00402f26
            mov edi, dword ptr [esi]
            xor ebx, ebx
            xor ebp, ebp
            mov eax, dword ptr [edi + 4]
            test eax, eax
            jbe L00402ee1
    L00402ea6:
            mov eax, dword ptr [edi]
            lea ecx, [ebx + edi + 8]
            lea esi, [eax + eax*2]
            shl esi, 3
            mov eax, dword ptr [edx + esi + 8]
            test eax, eax
            je L00402ecc
            call eax
            mov edx, dword ptr [gPipe_callbacks]
            mov ecx, dword ptr [edx + esi + 4]
            lea esi, [eax + ecx + 4]
            jmp L00402ed3
    L00402ecc:
            mov esi, dword ptr [edx + esi + 4]
            add esi, 4
    L00402ed3:
            mov eax, dword ptr [edi + 4]
            add ebx, esi
            inc ebp
            cmp ebp, eax
            jb L00402ea6
            mov esi, dword ptr [esp + 0x10]
    L00402ee1:
            add ebx, 0xa
            test bl, 1
            je L00402ef3
            push 0x62
            call FatalError
            add esp, 4
    L00402ef3:
            mov edx, dword ptr [esi]
            add edx, ebx
            mov dword ptr [esi], edx
            mov ecx, dword ptr [gPipe_buffer_working_end]
            mov eax, edx
            cmp eax, ecx
            jb L00402f15
            cmp eax, dword ptr [gPipe_record_ptr]
            je L00402f15
            mov edx, dword ptr [gPipe_buffer_start]
            mov dword ptr [esi], edx
    L00402f15:
            mov eax, dword ptr [gPipe_record_ptr]
            mov ecx, dword ptr [esi]
            cmp ecx, eax
            je L00403012
            jmp L00402f56
    L00402f26:
            mov eax, dword ptr [esi]
            cmp eax, ecx
            jne L00402f34
            cmp eax, edi
            jne L00403012
    L00402f34:
            cmp eax, dword ptr [gPipe_buffer_start]
            jne L00402f44
            mov ecx, dword ptr [gPipe_buffer_working_end]
            mov dword ptr [esi], ecx
    L00402f44:
            mov edx, dword ptr [esi]
            add edx, -2
            mov dword ptr [esi], edx
            mov eax, edx
            xor edx, edx
            mov dx, word ptr [eax]
            sub eax, edx
            mov dword ptr [esi], eax
    L00402f56:
            mov edi, dword ptr [esi]
            mov edx, dword ptr [gPipe_callbacks]
            xor ebx, ebx
            xor ebp, ebp
            mov eax, dword ptr [edi + 4]
            test eax, eax
            jbe L00402fa4
    L00402f69:
            mov eax, dword ptr [edi]
            lea ecx, [ebx + edi + 8]
            lea esi, [eax + eax*2]
            shl esi, 3
            mov eax, dword ptr [edx + esi + 8]
            test eax, eax
            je L00402f8f
            call eax
            mov edx, dword ptr [gPipe_callbacks]
            mov ecx, dword ptr [edx + esi + 4]
            lea esi, [eax + ecx + 4]
            jmp L00402f96
    L00402f8f:
            mov esi, dword ptr [edx + esi + 4]
            add esi, 4
    L00402f96:
            mov eax, dword ptr [edi + 4]
            add ebx, esi
            inc ebp
            cmp ebp, eax
            jb L00402f69
            mov esi, dword ptr [esp + 0x10]
    L00402fa4:
            add ebx, 0xa
            test bl, 1
            je L00402fbc
            push 0x62
            call FatalError
            mov edx, dword ptr [gPipe_callbacks]
            add esp, 4
    L00402fbc:
            mov eax, dword ptr [esi]
            lea ecx, [ebx + eax - 2]
            mov dword ptr [gPipe_buffer_end_marker], ecx
            mov eax, dword ptr [esi]
            mov ecx, dword ptr [eax]
            test ecx, ecx
            jne L00402fd9
            mov eax, dword ptr [eax + 0xc]
            mov dword ptr [esp + 0x1c], eax
            jmp L00402ff8
    L00402fd9:
            cmp ecx, dword ptr [esp + 0x14]
            jne L00402ff8
            mov ecx, dword ptr [esp + 0x1c]
            mov edx, dword ptr [eax + 4]
            push ecx
            lea ecx, [eax + 8]
            call dword ptr [esp + 0x24]
            test eax, eax
            jne L00403012
            mov edx, dword ptr [gPipe_callbacks]
    L00402ff8:
            mov eax, dword ptr [esp + 0x24]
            test eax, eax
            je L00402e2f
            mov ecx, dword ptr [esp + 0x1c]
            call eax
            test eax, eax
            jne L00402e29
    L00403012:
            pop edi
            pop esi
            pop ebp
            pop ebx
            add esp, 8
            ret 0xc
            nop
            nop
            nop
            nop
    }
#endif
}

// FUNCTION: CARMA2_HW 0x004c6920
void C2_NAKED C2_HOOK_FASTCALL ScanCarsPositions(tCar_spec* pCar, br_vector3* pSource_pos, br_scalar pMax_distance_sqr, tU32 pOffset_time, tU32 pTime_period, br_vector3* pCar_pos, tU32* pTime_returned) {
#ifndef CARPOCALYPSE2_MATCHING
    tU8* temp_ptr;

    temp_ptr = ARGetPipePlayPtr();
    gTrigger_time = 0;
    gMax_distance = pMax_distance_sqr;
    BrVector3Copy(&gReference_pos, pSource_pos);
    gCar_ptr = pCar;

    if (ARReplayForwards()) {
        gOldest_time = GetTotalTime() + pOffset_time;
        gYoungest_time = gOldest_time + pTime_period;
    } else {
        gOldest_time = GetTotalTime() - pOffset_time;
        gYoungest_time = gOldest_time - pTime_period;
    }

    ARScanBuffer(&temp_ptr, ePipe_chunk_car, GetTotalTime(), CheckCar, CarTimeout);
    BrVector3Copy(pCar_pos, &gCar_pos);
    if (pCar_pos->v[0] > 500.f) {
        BrVector3Sub(pCar_pos, pCar_pos, &gInitial_position);
    }
    *pTime_returned = gTrigger_time;
#else
    __asm {
            push ecx
            push esi
            push edi
            mov esi, edx
            mov edi, ecx
            call ARGetPipePlayPtr
            mov dword ptr [esp + 8], eax
            mov eax, dword ptr [esp + 0x10]
            mov dword ptr [gTrigger_time], 0
            mov dword ptr [gMax_distance], eax
            mov ecx, dword ptr [esi]
            mov dword ptr [gReference_pos], ecx
            mov edx, dword ptr [esi + 4]
            mov dword ptr [gReference_pos+0x4], edx
            mov eax, dword ptr [esi + 8]
            mov dword ptr [gReference_pos+0x8], eax
            mov dword ptr [gCar_ptr], edi
            call ARReplayForwards
            test eax, eax
            je L004c6989
            call GetTotalTime
            mov ecx, dword ptr [esp + 0x14]
            add eax, ecx
            mov ecx, dword ptr [esp + 0x18]
            add ecx, eax
            mov dword ptr [gOldest_time], eax
            mov dword ptr [gYoungest_time], ecx
            jmp L004c69a4
    L004c6989:
            call GetTotalTime
            mov edx, dword ptr [esp + 0x14]
            mov ecx, dword ptr [esp + 0x18]
            sub eax, edx
            mov dword ptr [gOldest_time], eax
            sub eax, ecx
            mov dword ptr [gYoungest_time], eax
    L004c69a4:
            push offset CarTimeout
            push offset CheckCar
            call GetTotalTime
            mov edx, 5
            lea ecx, [esp + 0x10]
            push eax
            call ARScanBuffer
            mov eax, dword ptr [gCar_pos]
            mov ecx, dword ptr [esp + 0x1c]
            mov edx, ecx
            mov dword ptr [edx], eax
            mov eax, dword ptr [gCar_pos+0x4]
            mov dword ptr [edx + 4], eax
            mov eax, dword ptr [gCar_pos+0x8]
            mov dword ptr [edx + 8], eax
            _emit 0xd9
        _emit 0x01
            fcomp dword ptr [gDouble_0058a908+0x8]
            fnstsw ax
            test ah, 0x41
            jne L004c6a12
            _emit 0xd9
        _emit 0x01
            fsub dword ptr [gInitial_position]
            fld dword ptr [ecx + 4]
            fld dword ptr [ecx + 8]
            fxch st(2)
            _emit 0xd9
        _emit 0x19
            fsub dword ptr [gInitial_position+0x4]
            fstp dword ptr [ecx + 4]
            fsub dword ptr [gInitial_position+0x8]
            fstp dword ptr [ecx + 8]
    L004c6a12:
            mov ecx, dword ptr [esp + 0x20]
            mov edx, dword ptr [gTrigger_time]
            pop edi
            pop esi
            mov dword ptr [ecx], edx
            pop ecx
            ret 0x14
    }
#endif
}

// FUNCTION: CARMA2_HW 0x004c82e0
void C2_NAKED C2_HOOK_FASTCALL PipeSingleOilSpill(int pIndex, br_matrix34* pMat, br_scalar pFull_size, br_scalar pGrow_rate, tU32 pSpill_time, tU32 pStop_time, tCar_spec* pCar, br_vector3* pOriginal_pos, br_pixelmap* pPixelmap) {
#ifndef CARPOCALYPSE2_MATCHING

    gUNK_0069410c_u32 = *(tU32*)&pFull_size;
    ARDoSingleVariedSession(0x10, pIndex, 8,
        0x30, 0, pMat,
        4, 0x30, gUNK_0069410c_u32,
        4, 0x34, gUNK_0069410c_u32,
        4, 0x38, pSpill_time,
        4, 0x3c, pStop_time,
        4, 0x40, pCar,
        0xc, 0x44, pOriginal_pos,
        4, 0x50, pPixelmap);
#else
    __asm {
            mov eax, dword ptr [esp + 4]
            mov dword ptr [gUNK_0069410c], eax
            mov eax, dword ptr [esp + 0x1c]
            push eax
            mov eax, dword ptr [esp + 0x1c]
            push 0x50
            push 4
            push eax
            mov eax, dword ptr [esp + 0x24]
            push 0x44
            push 0xc
            push eax
            mov eax, dword ptr [esp + 0x2c]
            push 0x40
            push 4
            push eax
            mov eax, dword ptr [esp + 0x34]
            push 0x3c
            push 4
            push eax
            mov eax, dword ptr [gUNK_0069410c]
            push 0x38
            push 4
            push eax
            push 0x34
            push 4
            push eax
            push 0x30
            push 4
            push edx
            push 0
            push 0x30
            push 8
            push ecx
            push 0x10
            call ARDoSingleVariedSession
            add esp, 0x6c
            ret 0x1c
            nop
            nop
            nop
            nop
            nop
            nop
    }
#endif
}

// FUNCTION: CARMA2_HW 0x004c8b30
void C2_NAKED C2_HOOK_FASTCALL PipeSinglePedMove(tPedestrian* pPed, tS16 pArg2, tS16 pOriginal_move_id, tS16 pMove_id, undefined4 pArg5,undefined4 pArg6, int pArg7, undefined4 pArg8, br_vector3* pOriginal_pos, br_vector3* pPos, int pOriginal_action, int pAction, br_matrix34* pArg13) {
#ifndef CARPOCALYPSE2_MATCHING
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tPipe_ped_move_data, move_id, 0x0);
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tPipe_ped_move_data, original_move_id, 0x2);
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tPipe_ped_move_data, field_0x4, 0x4);
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tPipe_ped_move_data, field_0x6, 0x6);
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tPipe_ped_move_data, field_0x8, 0x8);
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tPipe_ped_move_data, field_0xc, 0xc);
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tPipe_ped_move_data, field_0x10, 0x10);
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tPipe_ped_move_data, original_pos, 0x14);
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tPipe_ped_move_data, pos, 0x20);
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tPipe_ped_move_data, original_action, 0x2c);
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tPipe_ped_move_data, action, 0x30);
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tPipe_ped_move_data, field_0x34, 0x34);
    C2_HOOK_STATIC_ASSERT_STRUCT_MEMBER_SIZE(tPipe_ped_move_data, move_id, 2);
    C2_HOOK_STATIC_ASSERT_STRUCT_MEMBER_SIZE(tPipe_ped_move_data, original_move_id, 2);
    C2_HOOK_STATIC_ASSERT_STRUCT_MEMBER_SIZE(tPipe_ped_move_data, field_0x4, 2);
    C2_HOOK_STATIC_ASSERT_STRUCT_MEMBER_SIZE(tPipe_ped_move_data, field_0x6, 2);
    C2_HOOK_STATIC_ASSERT_STRUCT_MEMBER_SIZE(tPipe_ped_move_data, field_0x8, 4);
    C2_HOOK_STATIC_ASSERT_STRUCT_MEMBER_SIZE(tPipe_ped_move_data, field_0xc, 4);
    C2_HOOK_STATIC_ASSERT_STRUCT_MEMBER_SIZE(tPipe_ped_move_data, field_0x10, 4);
    C2_HOOK_STATIC_ASSERT_STRUCT_MEMBER_SIZE(tPipe_ped_move_data, original_pos, 12);
    C2_HOOK_STATIC_ASSERT_STRUCT_MEMBER_SIZE(tPipe_ped_move_data, pos, 12);
    C2_HOOK_STATIC_ASSERT_STRUCT_MEMBER_SIZE(tPipe_ped_move_data, original_action, 4);
    C2_HOOK_STATIC_ASSERT_STRUCT_MEMBER_SIZE(tPipe_ped_move_data, action, 4);
    C2_HOOK_STATIC_ASSERT_STRUCT_MEMBER_SIZE(tPipe_ped_move_data, field_0x34, 48);

    gUNK_0069410c_u32 = pArg5;
    ARDoSingleVariedSession(ePipe_chunk_ped_move, (uintptr_t)pPed, 12,
        SIZE_OFFSET_PIPING(tPipe_ped_move_data, original_move_id), pOriginal_move_id,
        SIZE_OFFSET_PIPING(tPipe_ped_move_data, move_id),          pMove_id,
        SIZE_OFFSET_PIPING(tPipe_ped_move_data, field_0x6),        pArg2,
        SIZE_OFFSET_PIPING(tPipe_ped_move_data, field_0xc),        pArg5,
        SIZE_OFFSET_PIPING(tPipe_ped_move_data, field_0x8),        pArg5,
        SIZE_OFFSET_PIPING(tPipe_ped_move_data, field_0x4),        pArg7 != 0,
        SIZE_OFFSET_PIPING(tPipe_ped_move_data, field_0x10),       pArg8,
        SIZE_OFFSET_PIPING(tPipe_ped_move_data, original_pos),     pOriginal_pos,
        SIZE_OFFSET_PIPING(tPipe_ped_move_data, pos),              pPos,
        SIZE_OFFSET_PIPING(tPipe_ped_move_data, original_action),  pOriginal_action,
        SIZE_OFFSET_PIPING(tPipe_ped_move_data, action),           pAction,
        SIZE_OFFSET_PIPING(tPipe_ped_move_data, field_0x34),       pArg13);
#else
    __asm {
            push esi
            mov esi, dword ptr [esp + 0x18]
            xor eax, eax
            test esi, esi
            je L004c8b40
            mov eax, 1
    L004c8b40:
            mov esi, dword ptr [esp + 0x10]
            mov dword ptr [gUNK_0069410c], esi
            mov esi, dword ptr [esp + 0x30]
            push esi
            mov esi, dword ptr [esp + 0x30]
            push 0x34
            push 0x30
            push esi
            mov esi, dword ptr [esp + 0x38]
            push 0x30
            push 4
            push esi
            mov esi, dword ptr [esp + 0x40]
            push 0x2c
            push 4
            push esi
            mov esi, dword ptr [esp + 0x48]
            push 0x20
            push 0xc
            push esi
            mov esi, dword ptr [esp + 0x50]
            push 0x14
            push 0xc
            push esi
            push 0x10
            push 4
            push eax
            mov eax, dword ptr [gUNK_0069410c]
            push 4
            push 2
            push eax
            push 8
            push 4
            push eax
            push 0xc
            movsx eax, dx
            movsx edx, word ptr [esp + 0x74]
            push 4
            push eax
            movsx eax, word ptr [esp + 0x78]
            push 6
            push 2
            push edx
            push 0
            push 2
            push eax
            push 2
            push 2
            push 0xc
            push ecx
            push 0x2a
            call ARDoSingleVariedSession
            add esp, 0x9c
            pop esi
            ret 0x2c
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

// FUNCTION: CARMA2_HW 0x004c8bd0
void C2_HOOK_FASTCALL PipeSinglePedPhysics(int pId, br_matrix34* pMat) {
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tPipe_ped_physics_data, matrix, 0x0);
    C2_HOOK_STATIC_ASSERT_STRUCT_MEMBER_SIZE(tPipe_ped_physics_data, matrix, 0x30);

    ARDoSingleVariedSession(ePipe_chunk_ped_physics, (uintptr_t)pId, 1,
        SIZE_OFFSET_PIPING(tPipe_ped_physics_data, matrix),       pMat);
}

// FUNCTION: CARMA2_HW 0x004c8b10
void C2_HOOK_FASTCALL PipeSinglePedDir(int pId, const br_vector3* pDir) {
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tPipe_ped_dir_data, direction, 0x0);
    C2_HOOK_STATIC_ASSERT_STRUCT_MEMBER_SIZE(tPipe_ped_dir_data, direction, 0xc);

    ARDoSingleVariedSession(ePipe_chunk_ped_dir, (uintptr_t)pId, 1,
        SIZE_OFFSET_PIPING(tPipe_ped_dir_data, direction),       pDir);
}

// FUNCTION: CARMA2_HW 0x004c8c30
void C2_HOOK_FASTCALL PipeSinglePedMoveStatus(tPedestrian* pPed, float pValue, float pArg3) {
    gUNK_0069410c_u32 = *(tU32*)&pValue;
    ARDoSingleVariedSession(ePipe_chunk_ped_move_status, (uintptr_t)pPed, 2,
        4, 0,  *(tU32*)&pValue,
        4, 4,  *(tU32*)&pValue);
}

// FUNCTION: CARMA2_HW 0x004c8ea0
void C2_HOOK_FASTCALL PipeSingleDroneRender(tDrone_spec* pDrone_spec, int pRender) {
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tPipe_drone_render, render, 0x0);
    C2_HOOK_STATIC_ASSERT_STRUCT_MEMBER_SIZE(tPipe_drone_render, render, 0x2);

    ARDoSingleVariedSession(ePipe_chunk_drone_render, pDrone_spec->id, 1,
        SIZE_OFFSET_PIPING(tPipe_drone_render, render),         pRender);
}

// FUNCTION: CARMA2_HW 0x004c8ec0
void C2_HOOK_FASTCALL PipeSingleDroneCornerPos(tDrone_spec* pDrone, tU16 pField_0x48, tU16 pField_0x74, tU16 pField_0x70, tCompressed_matrix34* pMatrix) {
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tPipe_drone_corner_pos, field_0x48, 0x0);
    C2_HOOK_STATIC_ASSERT_STRUCT_MEMBER_SIZE(tPipe_drone_corner_pos, field_0x48, 0x2);
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tPipe_drone_corner_pos, field_0x74, 0x2);
    C2_HOOK_STATIC_ASSERT_STRUCT_MEMBER_SIZE(tPipe_drone_corner_pos, field_0x74, 0x2);
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tPipe_drone_corner_pos, field_0x70, 0x4);
    C2_HOOK_STATIC_ASSERT_STRUCT_MEMBER_SIZE(tPipe_drone_corner_pos, field_0x70, 0x2);
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tPipe_drone_corner_pos, matrix, 0x6);
    C2_HOOK_STATIC_ASSERT_STRUCT_MEMBER_SIZE(tPipe_drone_corner_pos, matrix, 0x18);

    ARDoSingleVariedSession(ePipe_chunk_drone_corner_pos, pDrone->id, 7,
        SIZE_OFFSET_PIPING(tPipe_drone_corner_pos, field_0x48),         pField_0x48,
        SIZE_OFFSET_PIPING(tPipe_drone_corner_pos, field_0x74),         pField_0x74,
        SIZE_OFFSET_PIPING(tPipe_drone_corner_pos, field_0x70),         pField_0x70,
        SIZE_OFFSET_PIPING(tPipe_drone_corner_pos, matrix.m[0]),        &pMatrix->m[0],
        SIZE_OFFSET_PIPING(tPipe_drone_corner_pos, matrix.m[1]),        &pMatrix->m[1],
        SIZE_OFFSET_PIPING(tPipe_drone_corner_pos, matrix.m[2]),        &pMatrix->m[2],
        SIZE_OFFSET_PIPING(tPipe_drone_corner_pos, matrix.m[3]),        &pMatrix->m[3]);
}

// FUNCTION: CARMA2_HW 0x004c6ea0
void C2_NAKED C2_HOOK_FASTCALL AddSmudgeToPipingSession(tU16 pCar_ID, int pModel_index, int pVertex_count, tSmudged_vertex* pCoordinates) {
#ifndef CARPOCALYPSE2_MATCHING
    tU32 data_size;

    if (gSmudge_space != NULL) {
        if (pVertex_count > 600) {
            pVertex_count = 600;
        }
        gSmudge_space->vertex_count = pVertex_count;
        gSmudge_space->model_index = pModel_index;
        memcpy(gSmudge_space->vertex_changes, pCoordinates, pVertex_count * sizeof(tSmudged_vertex));
        data_size = offsetof(tPipe_smudge_data, vertex_changes) + pVertex_count * sizeof(tSmudged_vertex);
        ARAddDataToSession(ePipe_chunk_smudge, pCar_ID, gSmudge_space, data_size);
    }
#else
    __asm {
            push ecx
            push esi
            mov esi, dword ptr [gSmudge_space]
            test esi, esi
            mov word ptr [esp + 4], cx
            je L004c6f12
            mov ecx, dword ptr [esp + 0xc]
            push edi
            cmp ecx, 0x258
            jle L004c6ec3
            mov ecx, 0x258
    L004c6ec3:
            lea eax, [ecx*4]
            mov word ptr [esi], cx
            mov ecx, dword ptr [gSmudge_space]
            mov esi, dword ptr [esp + 0x14]
            mov dword ptr [ecx + 4], edx
            mov edx, dword ptr [gSmudge_space]
            mov ecx, eax
            add eax, 8
            lea edi, [edx + 8]
            mov edx, ecx
            shr ecx, 2
            rep movsd
            mov ecx, edx
            mov edx, dword ptr [esp + 8]
            and ecx, 3
            push eax
            rep movsb
            mov eax, dword ptr [gSmudge_space]
            and edx, 0xffff
            push eax
            mov ecx, 0x13
            call ARAddDataToSession
            pop edi
    L004c6f12:
            pop esi
            pop ecx
            ret 8
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

// FUNCTION: CARMA2_HW 0x004c8e30
void C2_HOOK_FASTCALL PipeSingleDSModel(int pArg1, void* pThing) {
    /*     pArg1 | pThing
     *     ----- | ----------
     *         0 | tCar_spec*
     *         1 | br_model*
     */
    ARDoSingleVariedSession(ePipe_single_ds_model, (uintptr_t)pThing, 1,
        SIZE_OFFSET_PIPING(tPipe_single_ds_model, field_0x0), pArg1);
}

// FUNCTION: CARMA2_HW 0x004c8e80
void C2_HOOK_FASTCALL PipeSingleVector3(br_vector3* pDest, br_vector3* pValue) {
    ARDoSingleVariedSession(ePipe_single_vector3, (uintptr_t)pDest, 1,
        SIZE_OFFSET_PIPING(tPipe_single_vector, vector), pValue);
}

// FUNCTION: CARMA2_HW 0x004c8d80
void C2_HOOK_FASTCALL PipeSingleGrooveOnOff(int pOn, br_actor *pActor, int pBlock_flags) {
    ARDoSingleVariedSession(ePipe_single_groove_on_off, (uintptr_t)pActor, 2,
        SIZE_OFFSET_PIPING(tPipe_single_groove_on_of, enable), pOn,
        SIZE_OFFSET_PIPING(tPipe_single_groove_on_of, block_flags), pBlock_flags);
}

// FUNCTION: CARMA2_HW 0x004c8ff0
void C2_HOOK_FASTCALL PipeSingleTransformType(br_actor* pActor, tS16 pTransform_type) {
    tU16 type_from = pActor->t.type;

    C2_HOOK_STATIC_ASSERT_STRUCT_MEMBER_SIZE(tPipe_single_transform_type, type_from, 0x1);
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tPipe_single_transform_type, type_to, 0x1);
    C2_HOOK_STATIC_ASSERT_STRUCT_MEMBER_SIZE(tPipe_single_transform_type, type_to, 0x1);

    ARDoSingleVariedSession(ePipe_single_transform_type, (uintptr_t)pActor, 2,
        SIZE_OFFSET_PIPING(tPipe_single_transform_type, type_from), type_from,
        SIZE_OFFSET_PIPING(tPipe_single_transform_type, type_to), (tU16)pTransform_type);
}

// FUNCTION: CARMA2_HW 0x0043bf30
void C2_HOOK_FASTCALL ARCompressModelMash(tCompressed_vector3 *pDest, br_model* pModel, br_vector3* pSrc) {
    br_uint_16 i;

    for (i = 0; i < pModel->nvertices; i++) {
        CompressVector3(&pDest[i], &pSrc[i], -10.f, 10.f);
    }
}

// FUNCTION: CARMA2_HW 0x0043c240
void C2_NAKED C2_HOOK_FASTCALL FUN_0043c240(tCar_spec* pCar_spec, float* pPlane) {
#ifndef CARPOCALYPSE2_MATCHING
    br_model** car_models;
    tCar_crush_spec* car_crush_spec;
    br_model* temporary_model;
    float plane[4];
    br_model** car_model_ptr;
    br_model** crush_models;
    br_model** crush_model_ptr;
    br_model* model;
    br_material* material;
    int i;
    int model_offset;
    int group;

    plane[0] = pPlane[0];
    plane[1] = pPlane[1];
    plane[2] = pPlane[2];
    plane[3] = pPlane[3];
    car_crush_spec = pCar_spec->car_crush_spec;
    car_models = (br_model**)pCar_spec->car_model_actor->user;
    crush_models = car_crush_spec->field_0x4c4;
    plane[3] = pPlane[3] - (pCar_spec->car_model_actor->t.t.mat.m[3][2] * plane[2]
        + pCar_spec->car_model_actor->t.t.mat.m[3][0] * plane[0]
        + pCar_spec->car_model_actor->t.t.mat.m[3][1] * plane[1]);

    FUN_0042f5d0(car_models[0], crush_models, &car_crush_spec->model_actor->model, plane, 0);
    i = 1;
    car_model_ptr = car_models;
    if (i < pCar_spec->count_detail_levels) {
        model_offset = (int)crush_models - (int)car_models;
        do {
            if (car_model_ptr[1] != NULL) {
                FUN_0042f5d0(car_model_ptr[1], (br_model**)((int)car_model_ptr + 4 + model_offset), &temporary_model, plane, 1);
            }
            i++;
            car_model_ptr++;
        } while (i < pCar_spec->count_detail_levels);
    }
    i = 0;
    if (i < pCar_spec->count_detail_levels) {
        model_offset = (int)car_models - (int)crush_models;
        crush_model_ptr = crush_models;
        do {
            temporary_model = *crush_model_ptr;
            *crush_model_ptr = *(br_model**)((int)crush_model_ptr + model_offset);
            *(br_model**)((int)crush_model_ptr + model_offset) = temporary_model;
            i++;
            crush_model_ptr++;
        } while (i < pCar_spec->count_detail_levels);
    }
    pCar_spec->car_model_actor->model = car_models[0];
    DRActorEnumRecurse(pCar_spec->car_model_actor, MakeCarModelsMaterialsDoubleSided, pCar_spec);

    model = car_crush_spec->model_actor->model;
    if (model != NULL) {
        for (group = 0; group < model->prepared->ngroups; group++) {
            material = model->faces[*model->prepared->groups[group].face_user].material;
            if (material != NULL && (material->flags & BR_MATF_TWO_SIDED) == 0) {
                material->flags |= BR_MATF_TWO_SIDED;
                BrMaterialUpdate(material, BR_MATU_RENDERING);
            }
        }
    }
#else
    __asm {
            sub esp, 0x20
            fld dword ptr [edx]
            fld dword ptr [edx + 4]
            fld dword ptr [edx + 8]
            fld dword ptr [edx + 0xc]
            fxch st(3)
            push ebx
            push ebp
            push esi
            mov esi, ecx
            push edi
            lea ebx, [esp + 0x20]
            mov eax, dword ptr [esi + 0xe0c]
            mov edi, dword ptr [esi + 0x18d4]
            push 0
            push ebx
            mov ecx, dword ptr [eax + 0x60]
            lea ebp, [edi + 0x4c4]
            fstp dword ptr [esp + 0x28]
            fxch st(1)
            fstp dword ptr [esp + 0x2c]
            mov dword ptr [esp + 0x18], ecx
            mov edx, ebp
            fstp dword ptr [esp + 0x30]
            mov dword ptr [esp + 0x1c], edi
            mov dword ptr [esp + 0x20], ebp
            fstp dword ptr [esp + 0x34]
            fld dword ptr [eax + 0x54]
            fld dword ptr [eax + 0x50]
            fld dword ptr [eax + 0x58]
            fxch st(2)
            fmul dword ptr [esp + 0x2c]
            fxch st(1)
            fmul dword ptr [esp + 0x28]
            fxch st(2)
            fmul dword ptr [esp + 0x30]
            fxch st(2)
            faddp st(1), st(0)
            fxch st(1)
            faddp st(1), st(0)
            fsubr dword ptr [esp + 0x34]
            fstp dword ptr [esp + 0x34]
            mov eax, dword ptr [edi + 0x4d0]
            mov ecx, dword ptr [ecx]
            add eax, 0x18
            push eax
            call FUN_0042f5d0
            mov eax, dword ptr [esi + 0xe14]
            mov ebx, 1
            cmp eax, ebx
            jle L0043c319
            mov ecx, dword ptr [esp + 0x10]
            mov eax, ecx
            lea edi, [ecx + 4]
            sub ebp, eax
    L0043c2e8:
            mov ecx, dword ptr [edi]
            test ecx, ecx
            je L0043c303
            lea edx, [esp + 0x20]
            push 1
            lea eax, [esp + 0x20]
            push edx
            push eax
            lea edx, [ebp + edi]
            call FUN_0042f5d0
    L0043c303:
            mov eax, dword ptr [esi + 0xe14]
            inc ebx
            add edi, 4
            cmp ebx, eax
            jl L0043c2e8
            mov ebp, dword ptr [esp + 0x18]
            mov edi, dword ptr [esp + 0x14]
    L0043c319:
            mov eax, dword ptr [esi + 0xe14]
            xor edx, edx
            test eax, eax
            jle L0043c349
            mov ecx, dword ptr [esp + 0x10]
            mov eax, ebp
            sub ecx, ebp
    L0043c32d:
            mov ebx, dword ptr [eax]
            mov edi, dword ptr [ecx + eax]
            mov dword ptr [ecx + eax], ebx
            mov dword ptr [eax], edi
            mov edi, dword ptr [esi + 0xe14]
            inc edx
            add eax, 4
            cmp edx, edi
            jl L0043c32d
            mov edi, dword ptr [esp + 0x14]
    L0043c349:
            mov edx, dword ptr [esp + 0x10]
            mov ecx, dword ptr [esi + 0xe0c]
            push esi
            mov eax, dword ptr [edx]
            mov edx, offset MakeCarModelsMaterialsDoubleSided
            mov dword ptr [ecx + 0x18], eax
            mov ecx, dword ptr [esi + 0xe0c]
            call DRActorEnumRecurse
            mov ecx, dword ptr [edi + 0x4d0]
            mov esi, dword ptr [ecx + 0x18]
            test esi, esi
            je L0043c3c7
            mov eax, dword ptr [esi + 0x4c]
            xor ebx, ebx
            cmp word ptr [eax + 8], bx
            jbe L0043c3c7
            xor edi, edi
    L0043c383:
            mov edx, dword ptr [eax + 0x18]
            xor eax, eax
            mov ecx, dword ptr [edx + edi + 0xc]
            mov ax, word ptr [ecx]
            lea edx, [eax + eax*4]
            mov eax, dword ptr [esi + 0xc]
            mov eax, dword ptr [eax + edx*8 + 8]
            test eax, eax
            je L0043c3b6
            mov ecx, dword ptr [eax + 0x20]
            test ch, 0x10
            jne L0043c3b6
            or ch, 0x10
            push 2
            push eax
            mov dword ptr [eax + 0x20], ecx
            call BrMaterialUpdate
            add esp, 8
    L0043c3b6:
            mov eax, dword ptr [esi + 0x4c]
            xor ecx, ecx
            inc ebx
            add edi, 0x24
            mov cx, word ptr [eax + 8]
            cmp ebx, ecx
            jl L0043c383
    L0043c3c7:
            pop edi
            pop esi
            pop ebp
            pop ebx
            add esp, 0x20
            ret
            nop
    }
#endif
}

// FUNCTION: CARMA2_HW 0x004c7550
void C2_NAKED C2_HOOK_FASTCALL AddModelMashToPipingSession(br_model* pModel, br_vector3* pMash) {
#ifndef CARPOCALYPSE2_MATCHING
    size_t size;
    tPipe_model_mash* pipe_model_mash;

    pipe_model_mash = (tPipe_model_mash*)gCrush_space;
    if (pipe_model_mash == NULL) {
        return;
    }
    size = pModel->nvertices * sizeof(tCompressed_vector3) + sizeof(br_model*);
    if (size > CRUSH_SPACE_SIZE) {
        FatalError(kFatalError_ChunkIsTooLargeToPipe);
    }
    pipe_model_mash->model = pModel;
    ARCompressModelMash(pipe_model_mash->vectors, pModel, pMash);
    ARAddDataToSession(ePipe_model_mash, -1, pipe_model_mash, size);
#else
    __asm {
            push ebp
            push esi
            push edi
            mov edi, dword ptr [gCrush_space]
            test edi, edi
            mov ebp, edx
            mov esi, ecx
            je L004c759e
            xor eax, eax
            push ebx
            mov ax, word ptr [esi + 0x10]
            lea eax, [eax + eax*2]
            lea ebx, [eax + eax + 4]
            cmp ebx, 0x4000
            jbe L004c7581
            push 0x7d
            call FatalError
            add esp, 4
    L004c7581:
            push ebp
            mov edx, esi
            lea ecx, [edi + 4]
            mov dword ptr [edi], esi
            call ARCompressModelMash
            or edx, 0xffffffff
            mov ecx, 0x18
            push ebx
            push edi
            call ARAddDataToSession
            pop ebx
    L004c759e:
            pop edi
            pop esi
            pop ebp
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

// FUNCTION: CARMA2_HW 0x004c8a10
void C2_HOOK_FASTCALL PipeSingleSmashTextureChange(tCar_spec* pCar_spec, br_material* pMaterial, br_pixelmap* pTexture) {
    ARDoSingleVariedSession(ePipe_smash_texture_change, (uintptr_t)pMaterial, 3,
        SIZE_OFFSET_PIPING(tPipe_smash_texture_change, car_spec), pCar_spec,
        SIZE_OFFSET_PIPING(tPipe_smash_texture_change, texture1), pMaterial->colour_map,
        SIZE_OFFSET_PIPING(tPipe_smash_texture_change, texture2), pTexture);
}

// FUNCTION: CARMA2_HW 0x004c8980
void C2_NAKED C2_HOOK_FASTCALL PipeSingleSmashRemoveFaces(br_model* pModel, int pCount, const void* pFaces, br_material* pMaterial) {
#ifndef CARPOCALYPSE2_MATCHING
#pragma inline_depth(0)
    tU8* crush_space;
    br_model* model;
    tU32 size;

    crush_space = (tU8*)gCrush_space;
    if (crush_space == NULL) {
        return;
    }
    model = pModel;
    size = (tU32)pCount * 8u;
    *(int*)(crush_space + 4) = pCount;
    *(br_material**)crush_space = pMaterial;
    memcpy(crush_space + 8, pFaces, size);
    size += 8;
    if (size > CRUSH_SPACE_SIZE) {
        FatalError(kFatalError_ChunkIsTooLargeToPipe);
    }
    ARDoSingleSession(34, (uintptr_t)model, crush_space, size);
#pragma inline_depth()
#else
    __asm {
            push ecx
            push ebp
            mov ebp, dword ptr [gCrush_space]
            test ebp, ebp
            mov dword ptr [esp + 4], ecx
            je L004c89e3
            push edi
            mov eax, dword ptr [esp + 0x14]
            push esi
            mov esi, dword ptr [esp + 0x14]
            push ebx
            lea ebx, [edx*8]
            mov ecx, ebx
            mov dword ptr [ebp + 4], edx
            mov edx, ecx
            lea edi, [ebp + 8]
            shr ecx, 2
            mov dword ptr [ebp], eax
            add ebx, 8
            rep movsd
            mov ecx, edx
            and ecx, 3
            cmp ebx, 0x4000
            rep movsb
            jbe L004c89d0
            push 0x7d
            call FatalError
            add esp, 4
    L004c89d0:
            mov edx, dword ptr [esp + 0x10]
            push ebx
            push ebp
            mov ecx, 0x22
            call ARDoSingleSession
            pop ebx
            pop esi
            pop edi
    L004c89e3:
            pop ebp
            pop ecx
            ret 8
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

// FUNCTION: CARMA2_HW 0x004c8de0
void C2_HOOK_FASTCALL PipeSingleFunkEnable(int pIndex, tU16 pFlags) {
    ARDoSingleVariedSession(ePipe_funk_enable, (uintptr_t)pIndex, 1,
        SIZE_OFFSET_PIPING(tPipe_funk_enable, flags), pFlags);
}

// FUNCTION: CARMA2_HW 0x004c8530
void C2_HOOK_FASTCALL PipeSingleSpecial(tSpecial_type pType) {
    ARDoSingleVariedSession(ePipe_chunk_special, pType, 0);
}

// FUNCTION: CARMA2_HW 0x004c8f30
void C2_HOOK_FASTCALL PipeSingleDroneStraightPos(tDrone_spec* pDrone, undefined4 pParam1, tU16 pParam2, tU16 pParam3) {
    // FIXME: is this a bug, or is pParam2 intentionally ignored?
    ARDoSingleVariedSession(ePipe_chunk_single_drone_straight_pos, pDrone->id, 3,
        SIZE_OFFSET_PIPING(tPipe_single_drone_straight_pos, field_0x0), pParam1,
        SIZE_OFFSET_PIPING(tPipe_single_drone_straight_pos, field_0x4), pParam3,
        SIZE_OFFSET_PIPING(tPipe_single_drone_straight_pos, field_0x6), pParam3);
}

// FUNCTION: CARMA2_HW 0x004c9020
void C2_HOOK_FASTCALL PipeSingleOppoRenderage(tOpponent_spec *pOpponent_spec, int pEnable) {
    ARDoSingleVariedSession(ePipe_chunk_single_oppo_renderage, (uintptr_t)pOpponent_spec, 1,
        SIZE_OFFSET_PIPING(tPipe_single_oppo_renderage, enable), pEnable);
}

// FUNCTION: CARMA2_HW 0x004c8a90
void C2_HOOK_FASTCALL PipeSinglePedStatus(const tPedestrian* pPed, tS8 pArg2, tS8 pArg3, tS8 pArg4, tS8 pArg5, tS8 pArg6, tU8 pArg7, tS8 pArg8, tU8 pArg9, const br_vector3* pArg10, const br_vector3* pArg11, const br_matrix34* pArg12) {
    ARDoSingleVariedSession(ePipe_chunk_single_ped_status, (uintptr_t)pPed, 11,
        SIZE_OFFSET_PIPING(tPipe_single_ped_status, field_0x00), pArg2,
        SIZE_OFFSET_PIPING(tPipe_single_ped_status, field_0x01), pArg3,
        SIZE_OFFSET_PIPING(tPipe_single_ped_status, field_0x02), pArg4,
        SIZE_OFFSET_PIPING(tPipe_single_ped_status, field_0x03), pArg5,
        SIZE_OFFSET_PIPING(tPipe_single_ped_status, field_0x04), pArg6,
        SIZE_OFFSET_PIPING(tPipe_single_ped_status, field_0x05), (tS8)pArg7,
        SIZE_OFFSET_PIPING(tPipe_single_ped_status, field_0x06), pArg8,
        SIZE_OFFSET_PIPING(tPipe_single_ped_status, field_0x07), (tS8)pArg9,
        SIZE_OFFSET_PIPING(tPipe_single_ped_status, field_0x08), pArg10,
        SIZE_OFFSET_PIPING(tPipe_single_ped_status, field_0x14), pArg11,
        SIZE_OFFSET_PIPING(tPipe_single_ped_status, field_0x20), pArg12);
}

// FUNCTION: CARMA2_HW 0x004c8c60
void C2_HOOK_FASTCALL PipeSinglePedPos(const tPedestrian* pPed, const br_vector3* pPos, const br_vector3 *pVelocity) {
    ARDoSingleVariedSession(ePipe_chunk_single_ped_pos, (uintptr_t)pPed, 2,
        SIZE_OFFSET_PIPING(tPipe_single_ped_pos, p), pPos,
        SIZE_OFFSET_PIPING(tPipe_single_ped_pos, v), pVelocity);
}

// FUNCTION: CARMA2_HW 0x004c8e10
void C2_HOOK_FASTCALL PipeSingleVanishedDismembered(const tPedestrian* pPed, tU32 pOld_value, tU32 pNew_value) {
    C2_HOOK_STATIC_ASSERT_STRUCT_MEMBER_SIZE(tPipe_single_vanished_dismembered, field_0x0, 0x4);
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tPipe_single_vanished_dismembered, field_0x4, 0x4);
    C2_HOOK_STATIC_ASSERT_STRUCT_MEMBER_SIZE(tPipe_single_vanished_dismembered, field_0x4, 0x4);

    ARDoSingleVariedSession(ePipe_chunk_single_vanished_dismembered, (uintptr_t)pPed, 2,
        SIZE_OFFSET_PIPING(tPipe_single_vanished_dismembered, field_0x0), pOld_value,
        SIZE_OFFSET_PIPING(tPipe_single_vanished_dismembered, field_0x4), pNew_value);
}

// FUNCTION: CARMA2_HW 0x004c8c80
void C2_HOOK_FASTCALL PipeSinglePedModelChange(tPedestrian* pPed, undefined4 pArg2, br_model* pModel, undefined4 pArg4, undefined4 pArg5, undefined4 pArg6) {
    ARDoSingleVariedSession(ePipe_chunk_single_ped_model_change, (uintptr_t)pPed, 5,
        SIZE_OFFSET_PIPING(tPipe_single_ped_model_change, field_0x0), pArg2,
        SIZE_OFFSET_PIPING(tPipe_single_ped_model_change, model), pModel,
        SIZE_OFFSET_PIPING(tPipe_single_ped_model_change, field_0x2), pArg4,
        SIZE_OFFSET_PIPING(tPipe_single_ped_model_change, field_0x6), pArg5,
        SIZE_OFFSET_PIPING(tPipe_single_ped_model_change, field_0x4), pArg6);
}

// FUNCTION: CARMA2_HW 0x004c83f0
void C2_HOOK_FASTCALL PipeSingleGraphicalWheelStuff(tCar_spec* pCar) {

    ARDoSingleVariedSession(ePipe_chunk_single_graphpical_wheel_stuff, (tU16)pCar->car_ID, 5,
        SIZE_OFFSET_PIPING(tPipe_single_graphical_wheel_stuff, steering_angle), (int)(pCar->steering_angle * (32767.0 / 60.0)),
        SIZE_OFFSET_PIPING(tPipe_single_graphical_wheel_stuff, lf_sus_position), (int)(pCar->lf_sus_position * 127.f * (20.0 / 3.0)),
        SIZE_OFFSET_PIPING(tPipe_single_graphical_wheel_stuff, rf_sus_position), (int)(pCar->rf_sus_position * 127.f * (20.0 / 3.0)),
        SIZE_OFFSET_PIPING(tPipe_single_graphical_wheel_stuff, lr_sus_position), (int)(pCar->lr_sus_position * 127.f * (20.0 / 3.0)),
        SIZE_OFFSET_PIPING(tPipe_single_graphical_wheel_stuff, rr_sus_position), (int)(pCar->rr_sus_position * 127.f * (20.0 / 3.0)));
}

// FUNCTION: CARMA2_HW 0x004c8540
void C2_HOOK_FASTCALL PipeSingleCarIncident(tCar_spec* pCar, br_vector3* pPosition, float pForce) {
    gUNK_0069410c_u32 = *(tU32*)&pForce;
    ARDoSingleVariedSession(ePipe_chunk_ped_incident, 1, 3,
        4, 0, *(tU32*)&pForce,
        2, 4, (tU16)pCar->car_ID,
        0xc, 8, pPosition);
}

// FUNCTION: CARMA2_HW 0x004c85b0
void C2_HOOK_STDCALL PipeSingleLocalCarIncident(br_scalar pArg) {
}

// FUNCTION: CARMA2_HW 0x004c8580
void C2_HOOK_FASTCALL PipeSinglePedIncident(tPedestrian* pPed, br_actor* pActor) {
    gUNK_0069410c_u32 = 0;
    ARDoSingleVariedSession(ePipe_chunk_ped_incident, 0, 3,
        SIZE_OFFSET_PIPING(tPipe_ped_incident, field_0x0), gUNK_0069410c_u32,
        SIZE_OFFSET_PIPING(tPipe_ped_incident, ped), pPed,
        SIZE_OFFSET_PIPING(tPipe_ped_incident, actor), pActor);
}

// FUNCTION: CARMA2_HW 0x004c7e70
void C2_HOOK_FASTCALL AddBurningPedToSession(int pIndex, tPedestrian* pPed, br_vector3* pPositions) {
    ARAddVariedDataToSession(ePipe_chunk_burning_ped, pIndex, 2,
        SIZE_OFFSET_PIPING(tPipe_burning_ped, ped), pPed,
        SIZE_OFFSET_PIPING(tPipe_burning_ped, positions), pPositions);
}

// FUNCTION: CARMA2_HW 0x004c8cc0
void C2_HOOK_FASTCALL PipeSinglePHILObject(tPhysics_object* pObject) {
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tPipe_phil_object, matrix, 0x0);
    C2_HOOK_STATIC_ASSERT_STRUCT_MEMBER_SIZE(tPipe_phil_object, matrix, 0x30);

    ARDoSingleVariedSession(ePipe_chunk_phil_object, (uintptr_t)pObject, 1,
        SIZE_OFFSET_PIPING(tPipe_phil_object, matrix), &pObject->actor->t.t.mat);
}

// FUNCTION: CARMA2_HW 0x004c6f40
void C2_HOOK_FASTCALL AddShrapnelToPipingSession(int pShrapnel_index, const br_vector3* pPos, tU16 pAge, br_material* pMaterial) {
    if (pShrapnel_index & 0x8000) {
        ARAddVariedDataToSession(ePipe_chunk_shrapnel, pShrapnel_index, 3,
            SIZE_OFFSET_PIPING(tPipe_chunk_shrapnel, pos), pPos,
            SIZE_OFFSET_PIPING(tPipe_chunk_shrapnel, age), (int)pAge,
            SIZE_OFFSET_PIPING(tPipe_chunk_shrapnel, material), pMaterial);
    } else {
        ARAddVariedDataToSession(ePipe_chunk_shrapnel, pShrapnel_index, 1,
            SIZE_OFFSET_PIPING(tPipe_chunk_shrapnel, pos), pPos);
    }
}

// FUNCTION: CARMA2_HW 0x004c70c0
void C2_HOOK_FASTCALL AddSmokeColumnToPipingSession(int pIndex, tSmoke_column* pSmoke_column, int pVertex, int pColour) {
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tPipe_chunk_smoke_column, core, 0x0);
    C2_HOOK_STATIC_ASSERT_STRUCT_MEMBER_SIZE(tPipe_chunk_smoke_column, core, 0x10);
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tPipe_chunk_smoke_column, vertex, 0x10);
    C2_HOOK_STATIC_ASSERT_STRUCT_MEMBER_SIZE(tPipe_chunk_smoke_column, vertex, 0x2);

    ARAddVariedDataToSession(ePipe_chunk_smoke_column, (pColour << 14) + pIndex, 2,
        SIZE_OFFSET_PIPING(tPipe_chunk_smoke_column, core), &pSmoke_column->core,
        SIZE_OFFSET_PIPING(tPipe_chunk_smoke_column, vertex), pVertex);
}

// FUNCTION: CARMA2_HW 0x004c8f90
void C2_HOOK_FASTCALL PipeSinglePowerupRespawn(br_actor* pActor, int pPowerup_id) {
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tPipe_chunk_powerup_respawn, index, 0x0);
    C2_HOOK_STATIC_ASSERT_STRUCT_MEMBER_SIZE(tPipe_chunk_powerup_respawn, index, 0x4);

    ARDoSingleVariedSession(ePipe_chunk_powerup_respawn, (uintptr_t)pActor, 1,
        SIZE_OFFSET_PIPING(tPipe_chunk_powerup_respawn, index), pPowerup_id);
}

// FUNCTION: CARMA2_HW 0x004c7940
void C2_HOOK_FASTCALL AddRepulseRayToPipingSession(int pIndex, br_vector3* pOrigin, br_vector3* pVictim, tU32 pTime) {
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tPipe_chunk_repulse_ray, origin, 0x0);
    C2_HOOK_STATIC_ASSERT_STRUCT_MEMBER_SIZE(tPipe_chunk_repulse_ray, origin, 0x4);
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tPipe_chunk_repulse_ray, victim, 0x4);
    C2_HOOK_STATIC_ASSERT_STRUCT_MEMBER_SIZE(tPipe_chunk_repulse_ray, victim, 0x4);
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tPipe_chunk_repulse_ray, time, 0x8);
    C2_HOOK_STATIC_ASSERT_STRUCT_MEMBER_SIZE(tPipe_chunk_repulse_ray, time, 0x4);

    ARAddVariedDataToSession(ePipe_chunk_repulse_ray, (uintptr_t)pIndex, 3,
        SIZE_OFFSET_PIPING(tPipe_chunk_repulse_ray, origin), pOrigin,
        SIZE_OFFSET_PIPING(tPipe_chunk_repulse_ray, victim), pVictim,
        SIZE_OFFSET_PIPING(tPipe_chunk_repulse_ray, time), pTime);
}

// FUNCTION: CARMA2_HW 0x004c8090
void C2_HOOK_FASTCALL PipeSingleScreenWobble(int pWobble_x, int pWobble_y) {
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tPipe_chunk_screen_wobble, wobble_x, 0x0);
    C2_HOOK_STATIC_ASSERT_STRUCT_MEMBER_SIZE(tPipe_chunk_screen_wobble, wobble_x, 0x1);
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tPipe_chunk_screen_wobble, wobble_y, 0x1);
    C2_HOOK_STATIC_ASSERT_STRUCT_MEMBER_SIZE(tPipe_chunk_screen_wobble, wobble_y, 0x1);

    ARDoSingleVariedSession(ePipe_chunk_screen_wobble, 0, 2,
        SIZE_OFFSET_PIPING(tPipe_chunk_screen_wobble, wobble_x), pWobble_x,
        SIZE_OFFSET_PIPING(tPipe_chunk_screen_wobble, wobble_y), pWobble_y);
}

// FUNCTION: CARMA2_HW 0x004c8340
void C2_HOOK_FASTCALL PipeSingleFrameFinish(void) {
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tPipe_chunk_frame_finish, time, 0x0);
    C2_HOOK_STATIC_ASSERT_STRUCT_MEMBER_SIZE(tPipe_chunk_frame_finish, time, 0x4);

    ARDoSingleVariedSession(ePipe_chunk_frame_finish, 0, 1,
        SIZE_OFFSET_PIPING(tPipe_chunk_frame_finish, time), GetTotalTime());
}

// FUNCTION: CARMA2_HW 0x004c6f20
void C2_HOOK_FASTCALL AddSparkToPipingSession(int pSpark_index, const br_vector3* pPos, const br_vector3* pV) {
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tPipe_chunk_spark, p, 0x0);
    C2_HOOK_STATIC_ASSERT_STRUCT_MEMBER_SIZE(tPipe_chunk_spark, p, 0xc);
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tPipe_chunk_spark, v, 0xc);
    C2_HOOK_STATIC_ASSERT_STRUCT_MEMBER_SIZE(tPipe_chunk_spark, v, 0xc);

    ARAddVariedDataToSession(ePipe_chunk_spark, pSpark_index, 2,
        SIZE_OFFSET_PIPING(tPipe_chunk_spark, p), pPos,
        SIZE_OFFSET_PIPING(tPipe_chunk_spark, v), pV);
}

void C2_HOOK_FASTCALL SaveReducedPos(tReduced_pos* p, br_vector3* v) {
    br_vector3 tv;

    BrVector3Sub(&tv, v, (br_vector3*)((tU8*)gProgram_state.current_car.car_master_actor + 0x50));
    p->v[0] = (tS16)(tv.v[0] * 800.f);
    p->v[1] = (tS16)(tv.v[1] * 800.f);
    p->v[2] = (tS16)(tv.v[2] * 800.f);
}

// FUNCTION: CARMA2_HW 0x004c7010
void C2_HOOK_FASTCALL AddSmokeToPipingSession(int pIndex, tU8 pType, br_vector3* pPos, br_scalar pRadius, br_scalar pStrength) {
    tReduced_pos reduced_pos;

    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tPipe_chunk_smoke, pos, 0x0);
    C2_HOOK_STATIC_ASSERT_STRUCT_MEMBER_SIZE(tPipe_chunk_smoke, pos, 0x6);
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tPipe_chunk_smoke, radius, 0x6);
    C2_HOOK_STATIC_ASSERT_STRUCT_MEMBER_SIZE(tPipe_chunk_smoke, radius, 0x2);
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tPipe_chunk_smoke, strength, 0x8);
    C2_HOOK_STATIC_ASSERT_STRUCT_MEMBER_SIZE(tPipe_chunk_smoke, strength, 0x1);
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tPipe_chunk_smoke, type, 0x9);
    C2_HOOK_STATIC_ASSERT_STRUCT_MEMBER_SIZE(tPipe_chunk_smoke, type, 0x1);

    SaveReducedPos(&reduced_pos, pPos);
    ARAddVariedDataToSession(ePipe_chunk_smoke, pIndex, 4,
        SIZE_OFFSET_PIPING(tPipe_chunk_smoke, pos), &reduced_pos,
        SIZE_OFFSET_PIPING(tPipe_chunk_smoke, radius), (int)(1024.f * pRadius),
        SIZE_OFFSET_PIPING(tPipe_chunk_smoke, strength), (int)(pStrength * 255.f),
        SIZE_OFFSET_PIPING(tPipe_chunk_smoke, type), (tU8)pType);
}

// FUNCTION: CARMA2_HW 0x004c74c0
void C2_HOOK_FASTCALL AddProxRayToPipingSession(int pRay_index, tCar_spec* pCar, tU16 pPed_index, tU32 pTime) {
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tPipe_chunk_prox_ray, ped_index, 0x0);
    C2_HOOK_STATIC_ASSERT_STRUCT_MEMBER_SIZE(tPipe_chunk_prox_ray, ped_index, 0x2);
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tPipe_chunk_prox_ray, car_ID, 0x2);
    C2_HOOK_STATIC_ASSERT_STRUCT_MEMBER_SIZE(tPipe_chunk_prox_ray, car_ID, 0x2);
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tPipe_chunk_prox_ray, time, 0x4);
    C2_HOOK_STATIC_ASSERT_STRUCT_MEMBER_SIZE(tPipe_chunk_prox_ray, time, 0x4);

    ARAddVariedDataToSession(ePipe_chunk_prox_ray, pRay_index, 3,
        SIZE_OFFSET_PIPING(tPipe_chunk_prox_ray, ped_index), (tU16)pPed_index,
        SIZE_OFFSET_PIPING(tPipe_chunk_prox_ray, car_ID), (tU16)pCar->car_ID,
        SIZE_OFFSET_PIPING(tPipe_chunk_prox_ray, time), pTime);
}

// FUNCTION: CARMA2_HW 0x004c8870
void C2_HOOK_FASTCALL PipeSingleShrapnelShower(uintptr_t pId, undefined4 pArg2, undefined4 pArg3, undefined4 pArg4, br_vector3* pArg5, br_vector3* pArg6, float pArg7, br_vector3* pArg8, br_vector3* pArg9, undefined4 pArg10, br_bounds3* pArg11, undefined4 pArg12, br_matrix34* pArg13, br_vector3* pArg14, void* pArg15) {
    ARDoSingleVariedSession(0x1e, pId, 14,
        4, 0x40, pArg3,
        4, 0, pArg4,
        0xc, 4, pArg5,
        0xc, 0x10, pArg6,
        4, 0x9c, (int)pArg7,
        0xc, 0x1c, pArg8,
        0xc, 0x28, pArg9,
        4, 0x44, pArg2,
        0x30, 0x50, pArg13,
        4, 0x4c, pArg12,
        4, 0x80, pArg10,
        0x18, 0x84, pArg11,
        0xc, 0x34, pArg14,
        0x30, 0xa0, pArg15);
}

// FUNCTION: CARMA2_HW 0x004c8ce0
void C2_HOOK_FASTCALL PipeSingleGibShower(uintptr_t pId, undefined4 pArg2, undefined4 pArg3, undefined4 pArg4, undefined4 pArg5, br_vector3* pArg6, br_vector3* pArg7, br_vector3* pArg8) {
    gUNK_0069410c_u32 = pArg4;
    ARDoSingleVariedSession(0x31, pId, 7,
        4, 8, pArg2,
        4, 4, pArg3,
        4, 0xc, gUNK_0069410c_u32,
        4, 0, pArg5,
        0xc, 0x10, pArg6,
        0xc, 0x1c, pArg7,
        0xc, 0x28, pArg8);
}

// FUNCTION: CARMA2_HW 0x004c8d40
void C2_HOOK_FASTCALL PipeSingleBloodSpurt(uintptr_t pId, undefined4 pArg2, undefined4 pArg3, br_vector3* pArg4, br_vector3* pArg5, br_vector3* pArg6) {
    ARDoSingleVariedSession(ePipe_chunk_blood_spurt, pId, 5,
        SIZE_OFFSET_PIPING(tPipe_blood_spurt_data, field_0x4), pArg2,
        SIZE_OFFSET_PIPING(tPipe_blood_spurt_data, field_0x0), pArg3,
        SIZE_OFFSET_PIPING(tPipe_blood_spurt_data, field_0x8), pArg4,
        SIZE_OFFSET_PIPING(tPipe_blood_spurt_data, field_0x14), pArg5,
        SIZE_OFFSET_PIPING(tPipe_blood_spurt_data, field_0x20), pArg6);
}

// FUNCTION: CARMA2_HW 0x004c8fd0
void C2_HOOK_FASTCALL PipeSingleEndShitMine(br_actor* pActor) {
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tPipe_chunk_end_shit_mine, m, 0x0);
    C2_HOOK_STATIC_ASSERT_STRUCT_MEMBER_SIZE(tPipe_chunk_end_shit_mine, m, 0x30);

    ARDoSingleVariedSession(ePipe_chunk_end_shit_mine, (uintptr_t)pActor, 1,
        SIZE_OFFSET_PIPING(tPipe_chunk_end_shit_mine, m), &pActor->t.t.mat);
}
// ARReplayForwards

// ARReplayIsReallyPaused

// FUNCTION: CARMA2_HW 0x00402380
int C2_HOOK_FASTCALL ARReplayPauseState(void) {
    return gReplay_pause_state;
}

// FUNCTION: CARMA2_HW 0x00402390
float C2_HOOK_FASTCALL ARGetReplayRate(void) {
    return gReplay_rate;
}

// ARSetReplayRate

// ARGetReplayDirection

// FUNCTION: CARMA2_HW 0x004023c0
void C2_HOOK_FASTCALL ARSetReplayDirection(float pDirection) {
    gPlay_direction = (int)pDirection;
}

// ARResetPiping

// ARInitialise

// ARDispose

// LengthOfChunk

// LengthOfSession

// FUNCTION: CARMA2_HW 0x004024e0
void C2_HOOK_FASTCALL ARAdvanceChunkPtr(tPipe_chunk** pChunk_ptr, tPipe_chunk_type pType) {
    tU8* cur;
    tU8* next;
    tU32 size;

    cur = (tU8*)*pChunk_ptr;
    if (gPipe_undo_enabled != 0) {
        return;
    }
    size = sizeof(void*);
    if (gPipe_callbacks[pType].calc_length != NULL) {
        size += gPipe_callbacks[pType].calc_length((tPipe_chunk*)cur) + gPipe_callbacks[pType].length;
    } else {
        size += gPipe_callbacks[pType].length;
    }
    next = (tU8*)*pChunk_ptr + size;
    *pChunk_ptr = (tPipe_chunk*)next;
    if (gPipe_buffer_end_marker == next || (intptr_t)next > (intptr_t)gPipe_buffer_end_marker) {
        *pChunk_ptr = (tPipe_chunk*)cur;
    }
}

// PipingFrameReset

// ARPipeSearchForwards

// ARIsActionReplayAvailable

// FUNCTION: CARMA2_HW 0x004025b0
int C2_HOOK_FASTCALL ARSomeReplayLeft(void) {
    return (gPlay_direction > 0 && gPipe_play_ptr != gPipe_record_ptr)
        || (gPlay_direction < 0 && gPipe_play_ptr != gPipe_buffer_oldest);
}

// DisablePipedSounds

// EnablePipedSounds

// StartPipingSession2

// ARStartPipingSession

// EndPipingSession2

// AREndPipingSession

// ARAddDataToSession

// ARAddVariedDataToSession

// ARDoSingleSession

// ARDoSingleVariedSession

// ARResetPipePlayToEnd

// ARGetPipePlayPtr

// FUNCTION: CARMA2_HW 0x00402e00
void C2_HOOK_FASTCALL ARSetPipePlayPtr(tU8* pPtr) {
    gPipe_play_ptr = pPtr;
}

// ApplyPipedSession

// FUNCTION: CARMA2_HW 0x004037c0
int C2_HOOK_FASTCALL MoveSessionPointerBackOne(tU8** pPtr) {
    if (*pPtr == gPipe_buffer_oldest && *pPtr != gPipe_record_ptr) {
        return 1;
    }
    if (*pPtr == gPipe_buffer_start) {
        *pPtr = gPipe_buffer_working_end;
    }
    *pPtr -= sizeof(tU16);
    *pPtr -= *(tU16*)*pPtr;
    return 0;
}

// MoveSessionPointerForwardOne

// FindPreviousChunk

// UndoPipedSession

// ARScanBuffer

// CheckSound

// FUNCTION: CARMA2_HW 0x00403c50
int C2_HOOK_FASTCALL SoundTimeout(tU32 pTime) {
    return gPipe_UNK_00676904 < pTime
        || gPipe_UNK_00676904 - pTime < gPipe_UNK_006768a8;
}

// FUNCTION: CARMA2_HW 0x00403990
void C2_NAKED C2_HOOK_FASTCALL ScanAndPlaySoundsToBe(tPipe_chunk* pSession, tU32 pOldest_time, tU32 pYoungest_time) {
#ifndef CARPOCALYPSE2_MATCHING
    int iVar3;
    int iVar4;
    tU32 uVar5;
    int iVar6;
    tPipe_chunk* piVar7;
    tPipe_chunk* chunk;
    tU32 local_time;

    if (gDisable_sound == 0) {
        gPipe_UNK_00676904 = pYoungest_time;
        gPipe_UNK_006768b0 = pOldest_time;
        gPipe_UNK_006768a8 = (tU32)(fabs(gReplay_rate) * gDouble_00589338);
        local_time = pOldest_time;
        do {
            int forward;

            if (gPipe_play_ptr == gPipe_record_ptr) {
                forward = 0;
            } else if (gPipe_play_ptr == gPipe_buffer_oldest) {
                forward = 1;
            } else if (gReplay_rate != 0.0f) {
                forward = gReplay_rate > 0.0f;
            } else {
                forward = gPlay_direction > 0;
            }

            if (forward) {
                iVar4 = 0;
                uVar5 = 0;
                if (pSession->count > 0) {
                    do {
                        iVar3 = LengthOfChunk((tPipe_chunk*)((tU8*)pSession + sizeof(tPipe_chunk) + iVar4), pSession->type);
                        iVar4 += iVar3;
                        uVar5++;
                    } while (uVar5 < pSession->count);
                }
                iVar4 += 10;
                if ((iVar4 & 1) != 0) {
                    FatalError(0x62);
                }
                piVar7 = (tPipe_chunk*)((tU8*)pSession + iVar4);
                pSession = piVar7;
                if ((tU8*)pSession >= gPipe_buffer_working_end) {
                    if ((tU8*)piVar7 == gPipe_record_ptr) {
                        return;
                    }
                    pSession = (tPipe_chunk*)gPipe_buffer_start;
                }
                if ((tU8*)pSession == gPipe_record_ptr) {
                    return;
                }
            } else {
                if ((tU8*)pSession == gPipe_buffer_oldest && (tU8*)pSession != gPipe_record_ptr) {
                    return;
                }
                if ((tU8*)pSession == gPipe_buffer_start) {
                    pSession = (tPipe_chunk*)gPipe_buffer_working_end;
                }
                pSession = (tPipe_chunk*)((tU8*)pSession - 2);
                pSession = (tPipe_chunk*)((tU8*)pSession - *(tU16*)pSession);
            }

            iVar3 = 0;
            uVar5 = 0;
            if (pSession->count > 0) {
                do {
                    tReplay_callback* callback;

                    callback = (tReplay_callback*)gPipe_callbacks + pSession->type;
                    if (callback->calc_length != NULL) {
                        iVar6 = callback->calc_length((tPipe_chunk*)((tU8*)pSession + sizeof(tPipe_chunk) + iVar3)) + 4 + callback->length;
                    } else {
                        iVar6 = callback->length + 4;
                    }
                    iVar3 += iVar6;
                    uVar5++;
                } while (uVar5 < pSession->count);
            }
            iVar3 += 10;
            if ((iVar3 & 1) != 0) {
                FatalError(0x62);
            }
            gPipe_buffer_end_marker = (tU8*)pSession + iVar3 - 2;
            if (pSession->type == 0) {
                local_time = ((tU32*)pSession)[3];
            } else if (pSession->type == 2) {
                iVar4 = pSession->count;
                chunk = (tPipe_chunk*)((tU8*)pSession + sizeof(tPipe_chunk));
                if (iVar4 > 0) {
                    do {
                        iVar3 = FUN_00564e5e(*(int*)chunk);
                        if (*(int*)((tU8*)chunk + 4) > 1) {
                            iVar3 = (int)((double)iVar3 / ((double)*(int*)((tU8*)chunk + 4) * gDouble_00589340));
                        }
                        piVar7 = chunk;
                        iVar3 += local_time;
                        if ((iVar3 < gPipe_UNK_00676904) || (gPipe_UNK_006768b0 <= iVar3)) {
                            if (gPipe_undo_enabled == 0) {
                                tU32 size;

                                if (gPipe_callbacks[2].calc_length == NULL) {
                                    size = gPipe_callbacks[2].length + 4;
                                } else {
                                    size = gPipe_callbacks[2].calc_length(chunk) + 4 + gPipe_callbacks[2].length;
                                }
                                piVar7 = (tPipe_chunk*)((tU8*)chunk + size);
                                if ((tU8*)piVar7 == gPipe_buffer_end_marker || (intptr_t)piVar7 > (intptr_t)gPipe_buffer_end_marker) {
                                    piVar7 = chunk;
                                }
                            }
                        } else {
                            if (gPipe_callbacks[2].apply != NULL) {
                                gPipe_callbacks[2].apply(&chunk);
                                piVar7 = chunk;
                            }
                        }
                        chunk = piVar7;
                        iVar4--;
                    } while (iVar4 != 0);
                }
            }
            iVar3 = gPipe_UNK_00676904;
            if (iVar3 < local_time) {
                continue;
            }
            iVar3 -= local_time;
            if (iVar3 < gPipe_UNK_006768a8) {
                continue;
            }
            break;
        } while (1);
    }
#else
    __asm {
            mov eax, dword ptr [gDisable_sound]
            sub esp, 0xc
            test eax, eax
            push ebx
            push ebp
            push esi
            push edi
            mov esi, edx
            jne L00403c3a
            fld dword ptr [gReplay_rate]
            mov eax, dword ptr [esp + 0x20]
            mov edi, ecx
            fabs
            fmul qword ptr [gDouble_00589338]
            mov dword ptr [gPipe_UNK_00676904], eax
            mov dword ptr [gPipe_UNK_006768b0], esi
            call _ftol
            mov dword ptr [gPipe_UNK_006768a8], eax
            mov dword ptr [esp + 0x10], esi
    L004039d3:
            mov eax, dword ptr [gPipe_play_ptr]
            mov ecx, dword ptr [gPipe_record_ptr]
            cmp eax, ecx
            jne L004039e6
            xor eax, eax
            jmp L00403a33
    L004039e6:
            cmp eax, dword ptr [gPipe_buffer_oldest]
            jne L004039f5
            mov eax, 1
            jmp L00403a33
    L004039f5:
            fld dword ptr [gReplay_rate]
            fcomp dword ptr [gFLOAT_00589330]
            fnstsw ax
            test ah, 0x40
            jne L00403a26
            fld dword ptr [gReplay_rate]
            fcomp dword ptr [gFLOAT_00589330]
            fnstsw ax
            test ah, 0x41
            jne L00403a22
            mov eax, 1
            jmp L00403a33
    L00403a22:
            xor eax, eax
            jmp L00403a33
    L00403a26:
            mov edx, dword ptr [gPlay_direction]
            xor eax, eax
            test edx, edx
            setg al
    L00403a33:
            test eax, eax
            je L00403a94
            mov eax, dword ptr [edi + 4]
            xor ebx, ebx
            xor esi, esi
            test eax, eax
            jbe L00403a57
    L00403a42:
            mov edx, dword ptr [edi]
            lea ecx, [ebx + edi + 8]
            call LengthOfChunk
            add ebx, eax
            mov eax, dword ptr [edi + 4]
            inc esi
            cmp esi, eax
            jb L00403a42
    L00403a57:
            add ebx, 0xa
            test bl, 1
            je L00403a69
            push 0x62
            call FatalError
            add esp, 4
    L00403a69:
            mov eax, dword ptr [gPipe_buffer_working_end]
            add edi, ebx
            cmp edi, eax
            jb L00403a86
            cmp edi, dword ptr [gPipe_record_ptr]
            je L00403c3a
            mov edi, dword ptr [gPipe_buffer_start]
    L00403a86:
            cmp edi, dword ptr [gPipe_record_ptr]
            je L00403c3a
            jmp L00403abc
    L00403a94:
            cmp edi, dword ptr [gPipe_buffer_oldest]
            jne L00403aa4
            cmp edi, ecx
            jne L00403c3a
    L00403aa4:
            cmp edi, dword ptr [gPipe_buffer_start]
            jne L00403ab2
            mov edi, dword ptr [gPipe_buffer_working_end]
    L00403ab2:
            sub edi, 2
            xor ecx, ecx
            mov cx, word ptr [edi]
            sub edi, ecx
    L00403abc:
            mov eax, dword ptr [edi + 4]
            xor ebx, ebx
            xor ebp, ebp
            test eax, eax
            jbe L00403b04
            mov edx, dword ptr [gPipe_callbacks]
    L00403acd:
            mov eax, dword ptr [edi]
            lea ecx, [ebx + edi + 8]
            lea esi, [eax + eax*2]
            shl esi, 3
            mov eax, dword ptr [edx + esi + 8]
            test eax, eax
            je L00403af3
            call eax
            mov edx, dword ptr [gPipe_callbacks]
            mov ecx, dword ptr [edx + esi + 4]
            lea esi, [eax + ecx + 4]
            jmp L00403afa
    L00403af3:
            mov esi, dword ptr [edx + esi + 4]
            add esi, 4
    L00403afa:
            mov eax, dword ptr [edi + 4]
            add ebx, esi
            inc ebp
            cmp ebp, eax
            jb L00403acd
    L00403b04:
            add ebx, 0xa
            test bl, 1
            je L00403b16
            push 0x62
            call FatalError
            add esp, 4
    L00403b16:
            lea edx, [ebx + edi - 2]
            mov dword ptr [gPipe_buffer_end_marker], edx
            mov eax, dword ptr [edi]
            test eax, eax
            jne L00403b32
            mov eax, dword ptr [edi + 0xc]
            mov dword ptr [esp + 0x10], eax
            jmp L00403c03
    L00403b32:
            cmp eax, 2
            jne L00403c03
            mov eax, dword ptr [edi + 4]
            lea esi, [edi + 8]
            test eax, eax
            mov dword ptr [esp + 0x20], esi
            jle L00403c03
            mov ebp, eax
    L00403b4f:
            mov ecx, dword ptr [esi]
            call FUN_00564e5e
            mov esi, dword ptr [esp + 0x20]
            mov dword ptr [esp + 0x18], eax
            mov ecx, dword ptr [esi + 4]
            cmp ecx, 1
            mov dword ptr [esp + 0x14], ecx
            jle L00403b7f
            fild dword ptr [esp + 0x18]
            fild dword ptr [esp + 0x14]
            fmul qword ptr [gDouble_00589340]
            fdivp st(1), st(0)
            call _ftol
    L00403b7f:
            mov ecx, dword ptr [esp + 0x10]
            add eax, ecx
            mov ecx, dword ptr [gPipe_UNK_00676904]
            cmp eax, ecx
            jb L00403bb0
            cmp eax, dword ptr [gPipe_UNK_006768b0]
            jae L00403bb0
            mov edx, dword ptr [gPipe_callbacks]
            mov eax, dword ptr [edx + 0x3c]
            test eax, eax
            je L00403bfc
            lea ecx, [esp + 0x20]
            call eax
            mov esi, dword ptr [esp + 0x20]
            jmp L00403bfc
    L00403bb0:
            mov eax, dword ptr [gPipe_undo_enabled]
            mov ebx, esi
            test eax, eax
            jne L00403bfc
            mov ecx, dword ptr [gPipe_callbacks]
            mov eax, dword ptr [ecx + 0x38]
            test eax, eax
            je L00403bdf
            mov ecx, esi
            call eax
            mov ecx, dword ptr [gPipe_callbacks]
            mov esi, dword ptr [esp + 0x20]
            mov edx, dword ptr [ecx + 0x34]
            lea eax, [eax + edx + 4]
            jmp L00403be5
    L00403bdf:
            mov eax, dword ptr [ecx + 0x34]
            add eax, 4
    L00403be5:
            add esi, eax
            mov eax, dword ptr [gPipe_buffer_end_marker]
            cmp esi, eax
            mov dword ptr [esp + 0x20], esi
            je L00403bf6
            jle L00403bfc
    L00403bf6:
            mov esi, ebx
            mov dword ptr [esp + 0x20], esi
    L00403bfc:
            dec ebp
            jne L00403b4f
    L00403c03:
            mov eax, offset SoundTimeout
            test eax, eax
            je L004039d3
            mov eax, dword ptr [gPipe_UNK_00676904]
            mov ecx, dword ptr [esp + 0x10]
            cmp eax, ecx
            jb L00403c2d
            sub eax, ecx
            mov ecx, dword ptr [gPipe_UNK_006768a8]
            cmp eax, ecx
            jb L00403c2d
            xor eax, eax
            jmp L00403c32
    L00403c2d:
            mov eax, 1
    L00403c32:
            test eax, eax
            jne L004039d3
    L00403c3a:
            pop edi
            pop esi
            pop ebp
            pop ebx
            add esp, 0xc
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
            nop
            nop
            nop
    }
#endif
}

// ScanAndPlaySoundsToBe

// FUNCTION: CARMA2_HW 0x00403020
tU32 C2_HOOK_FASTCALL ARGetStartTime(void) {
    tU8* session;

    session = gPipe_buffer_oldest;
    for (;;) {
        session = (tU8*)((uintptr_t)session + LengthOfSession((tPipe_chunk*)session));
        if ((uintptr_t)session >= (uintptr_t)gPipe_buffer_working_end && session != gPipe_record_ptr) {
            session = gPipe_buffer_start;
        }
        if (session == gPipe_record_ptr) {
            return 0;
        }
        if (((tPipe_chunk*)session)->type == 0) {
            return *(tU32*)(session + 12);
        }
    }
}

// ARDisableAdvance

// AREnableAdvance

// FindNextChunk

// MoveReplayBuffer

// ARMoveToEndOfReplay

// ARMoveToStartOfReplay

// ARDoActionReplay

// FUNCTION: CARMA2_HW 0x00403d10
void C2_HOOK_FASTCALL ReverseSound(tS3_effect_tag pEffect, tS3_sound_tag pTag) {
    S3ReverseSample(pEffect, pTag);
}

// FUNCTION: CARMA2_HW 0x00403d00
int C2_HOOK_FASTCALL ARGoBackwards(void) {
    return S3SetSampleFilterCallbacks(ReverseSound, ReverseSound);
}

// FUNCTION: CARMA2_HW 0x00403d20
int C2_HOOK_FASTCALL ARGoForwards(void) {
    return S3SetSampleFilterCallbacks(NULL, NULL);
}

// ARService

// ARMainLoopStart

// ARToggleReplay

// GetReducedPos

// SaveReducedPos

// InitLastDamageArrayEtc

// PipeObjectPosition

// PipeCarPositions

// DoSmudge

// DoOilSpillAR

// CheckCar

// CarTimeout

// ScanCarsPositions

// InitialiseActionReplay

// DisposeActionReplay

// CheckIncident

// GetNextIncident

// AddCrushToPipingSession

// AddSmudgeToPipingSession

// AddSparkToPipingSession

// AddShrapnelToPipingSession

// AddNonCarToPipingSession

// AddSmokeToPipingSession

// AddSmokeColumnToPipingSession

// FUNCTION: CARMA2_HW 0x004c70f0
void C2_HOOK_FASTCALL AddFlameToPipingSession(int pIndex, int pFrame, br_scalar pScale_x, br_scalar pScale_y, br_scalar pOffset_x, br_scalar pOffset_z) {
    tU32 bits;

    (void)pScale_y;
    (void)pOffset_x;
    (void)pOffset_z;
    memcpy(&bits, &pScale_x, sizeof(bits));
    gUNK_0069410c_u32 = bits;
    ARAddVariedDataToSession((tPipe_chunk_type)18, pIndex, 5,
        2, 0, pFrame, 4, 4, bits, 4, 8, bits, 4, 12, bits, 4, 16, bits);
}

// FUNCTION: CARMA2_HW 0x004c7130
void C2_HOOK_FASTCALL AddSplashToPipingSession(tPhysics_object* pCollision) {
    br_scalar water_d;

    water_d = pCollision->water_d;
    *(volatile br_scalar*)&gUNK_0069410c = water_d;
    ARAddVariedDataToSession(0x14, (uintptr_t)pCollision, 2,
        4, 0, *(tU32*)&gUNK_0069410c,
        0xc, 4, &pCollision->water_normal);
}

// FUNCTION: CARMA2_HW 0x004c7160
void C2_HOOK_FASTCALL AddExtendedSplashToPipingSession(tPhysics_object* pCollision, void* pArg2) {
    tU8 flag;
    br_scalar water_d;

    if (PHILGetObjectProperty(pCollision, 6) != 0.0f) {
        flag = 0xff;
    } else {
        flag = pCollision->flags_0x238;
    }
    water_d = pCollision->water_d;
    *(volatile br_scalar*)&gUNK_0069410c = water_d;
    ARAddVariedDataToSession(0x3b, (uintptr_t)pArg2, 4,
        1, 0, (*(int*)&flag) & 0xff,
        4, 4, *(tU32*)&gUNK_0069410c,
        0xc, 8, &pCollision->water_normal,
        0x18, 0x14, &pCollision->bb2);
}

// FUNCTION: CARMA2_HW 0x004c71e0
void C2_NAKED C2_HOOK_FASTCALL FUN_004c71e0(uintptr_t pOwner, void* pData, undefined4 pArg3, undefined4 pArg4, undefined4 pArg5, undefined4 pArg6, undefined4 pArg7, void* pArg8, undefined4 pArg9) {
#ifndef CARPOCALYPSE2_MATCHING
    gUNK_0069410c_u32 = pArg3;
    ARAddVariedDataToSession(0x10, pOwner, 8,
        0x30, 0, pData,
        4, 0x30, gUNK_0069410c_u32,
        4, 0x34, gUNK_0069410c_u32,
        4, 0x38, pArg5,
        4, 0x3c, pArg6,
        4, 0x40, pArg7,
        0xc, 0x44, pArg8,
        4, 0x50, pArg9);
#else
    __asm {
            mov eax, dword ptr [esp + 4]
            mov dword ptr [gUNK_0069410c], eax
            mov eax, dword ptr [esp + 0x1c]
            push eax
            mov eax, dword ptr [esp + 0x1c]
            push 0x50
            push 4
            push eax
            mov eax, dword ptr [esp + 0x24]
            push 0x44
            push 0xc
            push eax
            mov eax, dword ptr [esp + 0x2c]
            push 0x40
            push 4
            push eax
            mov eax, dword ptr [esp + 0x34]
            push 0x3c
            push 4
            push eax
            mov eax, dword ptr [gUNK_0069410c]
            push 0x38
            push 4
            push eax
            push 0x34
            push 4
            push eax
            push 0x30
            push 4
            push edx
            push 0
            push 0x30
            push 8
            push ecx
            push 0x10
            call ARAddVariedDataToSession
            add esp, 0x6c
            ret 0x1c
            nop
            nop
            nop
            nop
            nop
            nop
            call GetTotalTime
            push eax
            push 0
            push 4
            push 1
            push 0
            push 0
            call ARAddVariedDataToSession
            add esp, 0x18
            ret
            nop
            nop
            nop
            nop
            nop
            nop
            nop
            mov eax, dword ptr [esp + 0x18]
            push ebx
            push esi
            push edi
            fld dword ptr [esp + 0x18]
            mov edi, ecx
            push eax
            mov ecx, dword ptr [esp + 0x20]
            mov esi, edx
            push 0x40
            push 4
            lea ebx, [ecx + 1]
            shl ebx, 0xc
            call _ftol
            mov ecx, eax
            mov eax, 0x66666667
            imul ecx
            fld dword ptr [esp + 0x20]
            fmul qword ptr [gDouble_0058a908]
            sar edx, 2
            mov ecx, dword ptr [esp + 0x2c]
            mov eax, edx
            shr eax, 0x1f
            add edx, eax
            and edx, 0x7ff
            add ebx, edx
            neg ecx
            sbb ecx, ecx
            and ecx, 0x800
            add ebx, ecx
            push ebx
            push 0x3e
            push 2
            call _ftol
            mov edx, dword ptr [esp + 0x28]
            push eax
            push 0x3c
            push 2
            push edx
            push 0x30
            push 0xc
            push esi
            push 0
            push 0x30
            push 5
            push edi
            push 5
            call ARAddVariedDataToSession
            add esp, 0x48
            pop edi
            pop esi
            pop ebx
            ret 0x18
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

// AddCarToPipingSession

// AddDamageToPipingSession

// AddProxRayToPipingSession

// FUNCTION: CARMA2_HW 0x004c7520
void C2_HOOK_FASTCALL AddFlapToPipingSession(br_actor* pActor, br_angle pAngle) {
    ARAddVariedDataToSession(ePipe_chunk_flap_bits, (uintptr_t)pActor, 2,
        4, 0, (uintptr_t)pActor,
        2, 4, pAngle);
}

// AddModelMashToPipingSession

// AddRepulseRayToPipingSession

// AddNapalmBoltToPipingSession

// PipeSingleCrush

// PipeSingleScreenWobble

// PipeSingleGrooveStop

// PipeSingleNonCar

// PipeSingleOilSpill

// PipeSingleFrameFinish

// PipeSingleGraphicalWheelStuff

// FUNCTION: CARMA2_HW 0x004c84a0
void C2_HOOK_FASTCALL PipeSingleSound(tS3_outlet* pOutlet, int pSound, tS3_volume pL_volume, tS3_volume pR_volume, int pPitch, const br_vector3* pPosition) {
    if (gPed_676914) {
        return;
    }
    if (gProgram_state.racing == 0) {
        return;
    }
    {
        const br_vector3* pos = pPosition != NULL ? pPosition : &gZero_vector__smash;

        ARDoSingleVariedSession(ePipe_chunk_single_sound, pSound, 4,
            4, 0x0, pPitch,
            0xc, 0x4, (uintptr_t)pos,
            2, 0x10, (uint32_t)GetIndexFromOutlet(pOutlet),
            2, 0x12, (uint32_t)((pR_volume << 8) + pL_volume));
    }
}

// PipeSingleSpecial

// PipeSingleCarIncident

// PipeSinglePedIncident

// PipeSingleWallIncident

// PipeSingleSkidAdjustment

// FUNCTION: CARMA2_HW 0x004c8620
void C2_HOOK_FASTCALL PipeSingleFlap(br_actor* pActor, br_angle pAngle) {
    ARDoSingleVariedSession(ePipe_chunk_flap_bits, (uintptr_t)pActor, 2,
        4, 0, (uintptr_t)pActor,
        2, 4, (uintptr_t)pAngle);
}

// PipeSingleRelink

// FUNCTION: CARMA2_HW 0x004c86d0
void C2_HOOK_FASTCALL PipeSingleIdentity(br_actor* pActor) {
    ARDoSingleVariedSession(26, (uintptr_t)pActor, 1, 4, 0, (uintptr_t)pActor);
}

// PipeSingleSplitWeld

// PipeSingleShrapnelShower

// PipeSingleExplosion

// PipeSingleNonCarCreation

// FUNCTION: CARMA2_HW 0x004c8960
void C2_HOOK_FASTCALL PipeSingleSmashModelChange(br_actor* pActor, br_model* pOld_model, br_model* pNew_model) {
    C2_HOOK_STATIC_ASSERT_STRUCT_MEMBER_SIZE(tPipe_chunk_smash_model_change, field_0x0, 0x4);
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tPipe_chunk_smash_model_change, field_0x4, 0x4);
    C2_HOOK_STATIC_ASSERT_STRUCT_MEMBER_SIZE(tPipe_chunk_smash_model_change, field_0x4, 0x4);

    ARDoSingleVariedSession(ePipe_chunk_smash_model_change, (uintptr_t)pActor, 2,
        SIZE_OFFSET_PIPING(tPipe_chunk_smash_model_change, field_0x0), pOld_model,
        SIZE_OFFSET_PIPING(tPipe_chunk_smash_model_change, field_0x4), pNew_model);
}

// PipeSingleSmashRemoveFaces

// PipeSingleSmashDecal

// PipeSingleSmashTextureChange

// PipeSinglePedStatus

// PipeSinglePedDir

// PipeSinglePedMove

// PipeSinglePedPhysics

// PipeSinglePedDismember

// PipeSinglePedFPChange

// PipeSinglePedPos

// PipeSinglePedModelChange

// PipeSinglePHILObject

// PipeSingleGibShower

// PipeSingleBloodSpurt

// PipeSingleGrooveOnOff

// PipeSinglePowerupGot

// PipeSinglePowerupLose

// FUNCTION: CARMA2_HW 0x004c8da0
void C2_HOOK_FASTCALL ARPowerupGot(tPowerup* pPowerup, tCar_spec* pCar) {
    ARDoSingleVariedSession(0x34, (uintptr_t)pPowerup, 1, 4, 0, (uintptr_t)pCar);
}

// FUNCTION: CARMA2_HW 0x004c8dc0
void C2_HOOK_FASTCALL ARPowerupLose(tPowerup* pPowerup, tCar_spec* pCar) {
    ARDoSingleVariedSession(0x35, (uintptr_t)pPowerup, 1, 4, 0, (uintptr_t)pCar);
}

// PipeSingleFunkEnable

// PipeSingleVanishedDismembered

// PipeSingleDSModel

// PipeSingleVector3

// PipeSingleDroneRender

// PipeSingleDroneCornerPos

// PipeSingleDroneStraightPos

// PipeSinglePowerupRespawn

// PipeSingleShitMine

// PipeSingleEndShitMine

// PipeSingleTransformType

// PipeSingleOppoRenderage

// ApplyCrush

// ApplySmudge

// ApplySpark

// ApplyShrapnel

// ApplyScreenWobble

// ApplyGrooveStop

// ApplyNonCar

// ApplySmoke

// ApplySmokeColumn

// ApplyFlame

// ApplySplash

// ApplyExtendedSplash

// ApplyOilSpill

// ApplyFrameBoundary

// ApplySound

// ApplyCar

// ApplyGWS

// ApplyDamage

// ApplySpecial

// ApplyProxRay

// ApplySkidAdjustment

// ApplyFlap

// ApplyModelMash

// ApplyRelink

// ApplyIdentity

// ApplySplitWeld

// ApplyBend

// ApplyUnBend

// ApplyEndMyBend

// ApplyShrapnelShower

// ApplyExplosion

// ApplyNonCarCreation

// ApplySmashModelChange

// ApplySmashRemoveFaces

// ApplySmashDecal

// ApplySmashTextureChange

// ApplyRepulseRay

// ApplyActorTrans

// ApplyPedStatus

// ApplyPedDir

// ApplyPedMove

// ApplyPedPhysics

// ApplyPedDismember

// ApplyPedFPChange

// ApplyPedPos

// ApplyPedModelChange

// ApplyPHILObject

// ApplyGibShower

// ApplyBloodSpurt

// ApplyGrooveOnOff

// ApplyPowerupGot

// ApplyPowerupLose

// ApplyFunkEnable

// ApplyVanishDismembered

// ApplyDSModel

// ApplyPedDiagnostics

// ApplyVector3

// ApplyDroneRender

// ApplyDroneCornerPos

// ApplyDroneStraightPos

// ApplyDroneUnused

// ApplyNapalmBolt

// ApplyPowerupRespawn

// ApplyShitMine

// ApplyEndShitMine

// ApplyTransformType

// ApplyOppoRenderage

// UndoCrush

// UndoSmudge

// UndoDamage

// UndoSpecial

// UndoScreenWobble

// UndoSplash

// UndoExtendedSplash

// UndoSkidAdjustment

// UndoFlap

// UndoModelMash

// UndoRelink

// UndoSplitWeld

// UndoBend

// UndoUnBend

// UndoShrapnelShower

// UndoNonCarCreation

// UndoSmashModelChange

// UndoSmashRemoveFaces

// UndoSmashDecal

// UndoSmashTextureChange

// UndoActorTrans

// UndoPedStatus

// UndoPedMove

// UndoPedDismember

// UndoPedFPChange

// UndoPedPos

// UndoPedModelChange

// UndoGibShower

// UndoBloodSpurt

// UndoGrooveOnOff

// UndoPowerupGot

// UndoPowerupLose

// UndoFunkEnable

// UndoVanishDismembered

// UndoVector3

// UndoNapalmBolt

// UndoPowerupRespawn

// UndoShitMine

// UndoEndShitMine

// UndoTransformType

// CalcCrushLength

// CalcShrapnelLength

// CalcSmudgeLength

// CalcModelMashLength

// CalcEndMyBendLength

// CalcRemoveFacesLength
// FUNCTION: CARMA2_HW 0x004d6880
void C2_NAKED C2_HOOK_FASTCALL PedPreCollisionStuff(void) {
#ifndef CARPOCALYPSE2_MATCHING
    tPhysics_object* pObject;
    br_vector3 v;

    for (pObject = PHILGetFirstObject(); pObject != NULL; pObject = PHILGetNextObject(pObject)) {
        if (pObject->owner != NULL && pObject->flags_0x238 == 5) {
            v.v[0] = pObject->omega.v[0] * (BrVector3Length(&pObject->omega) * gDouble_0058ab98);
            v.v[1] = pObject->omega.v[1] * (BrVector3Length(&pObject->omega) * gDouble_0058ab98);
            v.v[2] = pObject->omega.v[2] * (float)(BrVector3Length(&pObject->omega) * gDouble_0058ab98);
            ApplyWaterOmegaBrake(pObject, &v);
        }
    }
#else
    __asm {
            sub esp, 0x10
            push esi
            call PHILGetFirstObject
            mov esi, eax
            test esi, esi
            je L004d699d
            push ebx
            mov bl, 5
    L004d6896:
            mov eax, dword ptr [esi + 0x23c]
            test eax, eax
            je L004d698b
            cmp byte ptr [esi + 0x238], bl
            jne L004d698b
            fld dword ptr [esi + 0x74]
            fld dword ptr [esi + 0x78]
            fld dword ptr [esi + 0x7c]
            fxch st(2)
            fst dword ptr [esp + 8]
            fld st(1)
            fxch st(1)
            fmul dword ptr [esp + 8]
            fxch st(1)
            fmul st(0), st(2)
            fxch st(3)
            fst dword ptr [esp + 8]
            fmul dword ptr [esp + 8]
            fxch st(3)
            faddp st(1), st(0)
            fld dword ptr [esi + 0x74]
            fxch st(3)
            faddp st(1), st(0)
            fsqrt
            fst dword ptr [esp + 8]
            fmul qword ptr [gDouble_0058ab98]
            fxch st(2)
            fmulp st(2), st(0)
            fxch st(1)
            fstp dword ptr [esp + 0xc]
            fstp st(0)
            fld dword ptr [esi + 0x74]
            fld dword ptr [esi + 0x78]
            fld dword ptr [esi + 0x7c]
            fxch st(2)
            fst dword ptr [esp + 8]
            fld st(1)
            fxch st(1)
            fmul dword ptr [esp + 8]
            fxch st(1)
            fmul st(0), st(2)
            fxch st(3)
            fst dword ptr [esp + 8]
            fmul dword ptr [esp + 8]
            fxch st(3)
            faddp st(1), st(0)
            fld dword ptr [esi + 0x78]
            fxch st(3)
            faddp st(1), st(0)
            fsqrt
            fst dword ptr [esp + 8]
            fmul qword ptr [gDouble_0058ab98]
            fxch st(2)
            fmulp st(2), st(0)
            fxch st(1)
            fstp dword ptr [esp + 0x10]
            fstp st(0)
            fld dword ptr [esi + 0x74]
            fld dword ptr [esi + 0x78]
            fld dword ptr [esi + 0x7c]
            fxch st(2)
            fst dword ptr [esp + 8]
            fld st(1)
            fxch st(1)
            fmul dword ptr [esp + 8]
            fxch st(1)
            fmul st(0), st(2)
            fxch st(3)
            fst dword ptr [esp + 8]
            fmul dword ptr [esp + 8]
            fxch st(3)
            faddp st(1), st(0)
            fxch st(2)
            lea edx, [esp + 0xc]
            mov ecx, esi
            faddp st(2), st(0)
            fxch st(1)
            fsqrt
            fstp st(1)
            fmul qword ptr [gDouble_0058ab98]
            fmul dword ptr [esi + 0x7c]
            fstp dword ptr [esp + 0x14]
            call ApplyWaterOmegaBrake
    L004d698b:
            mov ecx, esi
            call PHILGetNextObject
            mov esi, eax
            test esi, esi
            jne L004d6896
            pop ebx
    L004d699d:
            pop esi
            add esp, 0x10
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


// ===== Ported from bojan1986819 fork (exact-match replay pipe helpers) =====

#pragma auto_inline(off)
// C2PORT-NONMATCHING: best 35.23% | difftest PASS 2000 trials (spec difftest/specs/00403260_ARDoActionReplay.json) | reason stack-layout
// FUNCTION: CARMA2_HW 0x00403260
void C2_HOOK_FASTCALL ARDoActionReplay(int pStep_count, tARDoActionReplay_step_callback* pStep_callback, tARDoActionReplay_frame_callback* pFrame_start_callback, tARDoActionReplay_frame_callback* pFrame_end_callback) {
    tPipe_chunk* local_34;
    tPipe_chunk* piVar4;
    tPipe_chunk* piVar8;
    tPipe_chunk* piVar9;
    tPipe_chunk* current_chunk;
    tPipe_chunk* chunk_data_start;
    tPipe_chunk* next_session;
    tPipe_chunk_data* undo_target;
    int* session_count;
    const tReplay_callback* callback;
    int iVar2;
    int iVar3;
    int iVar5;
    int iVar6;
    int iVar10;
    int local_14;
    tU32 uVar11;
    tU32 uStack_24;
    int bVar12;
    tU32 start_time;
    tPipe_chunk* initial_session;

    local_34 = (tPipe_chunk*)gPipe_play_ptr;
    initial_session = local_34;
    start_time = GetTotalTime();
    local_14 = 0;
    iVar5 = (pStep_count ^ (pStep_count >> 31)) - (pStep_count >> 31);
    piVar9 = NULL;
    if (iVar5 < 1) {
        goto replay_complete;
    }

replay_outer:
    piVar4 = local_34;
    if ((piVar9 != local_34) && (KeyIsDown(0) == 0)) {
        if (pStep_count > 0) {
            if (gPipe_play_ptr != gPipe_record_ptr) {
                iVar6 = local_34->type;
                piVar9 = local_34;
                while (iVar6 != 0) {
                    iVar6 = LengthOfSession(piVar9);
                    piVar8 = (tPipe_chunk*)((tU8*)piVar9 + iVar6);
                    piVar9 = piVar8;

                    if ((tU8*)piVar8 >= gPipe_buffer_working_end) {
                        if ((tU8*)piVar8 == gPipe_record_ptr) {
                            goto replay_reset_callbacks;
                        }
                        piVar9 = (tPipe_chunk*)gPipe_buffer_start;
                    }
                    if ((tU8*)piVar9 == gPipe_record_ptr) {
                        goto replay_reset_callbacks;
                    }
                    iVar6 = piVar9->type;
                }
                gLast_replay_frame_time = *(tU32*)((tU8*)piVar9 + 0xc);
            }
        } else if (local_34 != (tPipe_chunk*)gPipe_buffer_oldest || local_34 == (tPipe_chunk*)gPipe_record_ptr) {
            piVar9 = local_34;
            if ((tU8*)local_34 == gPipe_buffer_start) {
                piVar9 = (tPipe_chunk*)gPipe_buffer_working_end;
            }
            piVar9 = (tPipe_chunk*)((tU8*)piVar9 - 2);
            piVar9 = (tPipe_chunk*)((tU8*)piVar9 - *(tU16*)piVar9);
            for (;;) {
                if ((tU8*)piVar9 == gPipe_buffer_oldest && (tU8*)piVar9 != gPipe_record_ptr) {
                    goto replay_reset_callbacks;
                }
                if ((tU8*)piVar9 == gPipe_buffer_start) {
                    piVar9 = (tPipe_chunk*)gPipe_buffer_working_end;
                }
                piVar9 = (tPipe_chunk*)((tU8*)piVar9 - 2);
                piVar9 = (tPipe_chunk*)((tU8*)piVar9 - *(tU16*)piVar9);
                if (piVar9->type == 0) {
                    break;
                }
            }
            gLast_replay_frame_time = *(tU32*)((tU8*)piVar9 + 0xc);
        }

replay_reset_callbacks:
        if (((gPlay_direction > 0) && (gPipe_play_ptr != gPipe_record_ptr)) ||
            ((gPlay_direction < 0) && (gPipe_play_ptr != gPipe_buffer_oldest))) {
            iVar6 = 0;
            iVar10 = 0;
            if (gPipe_count_callbacks > 0) {
                do {
                    callback = (const tReplay_callback*)((const tU8*)gPipe_callbacks + iVar10);
                    if (callback->reset != NULL) {
                        callback->reset();
                    }
                    iVar6++;
                    iVar10 += sizeof(tReplay_callback);
                } while (iVar6 < gPipe_count_callbacks);
            }
        }
        pFrame_start_callback(gFrame_period);
        if (pStep_count > 0) {
            for (;;) {
                if (local_34 == (tPipe_chunk*)gPipe_record_ptr) {
                    bVar12 = 1;
                } else {
                    iVar6 = 0;
                    uVar11 = 0;
                    session_count = (int*)((tU8*)local_34 + 4);
                    chunk_data_start = (tPipe_chunk*)((tU8*)local_34 + 8);
                    if (*session_count != 0) {
                        do {
                            iVar10 = LengthOfChunk((tPipe_chunk*)((tU8*)chunk_data_start + iVar6), local_34->type);
                            iVar6 += iVar10;
                            uVar11++;
                        } while (uVar11 < (tU32)*session_count);
                    }
                    if (((iVar6 + 10U) & 1) != 0) {
                        FatalError(0x62);
                    }
                    gPipe_buffer_end_marker = (tU8*)local_34 + iVar6 + 8;
                    iVar6 = local_34->type;
                    uVar11 = 0;
                    current_chunk = chunk_data_start;
                    if (*session_count != 0) {
                        do {
                            callback = &gPipe_callbacks[iVar6];
                            if (callback->apply != NULL) {
                                callback->apply(&current_chunk);
                            }
                            ARAdvanceChunkPtr(&current_chunk, iVar6);
                            uVar11++;
                        } while (uVar11 < (tU32)*session_count);
                    }
                    iVar10 = LengthOfSession(local_34);
                    local_34 = (tPipe_chunk*)((tU8*)local_34 + iVar10);
                    if (gPipe_buffer_working_end <= (tU8*)local_34 && local_34 != (tPipe_chunk*)gPipe_record_ptr) {
                        local_34 = (tPipe_chunk*)gPipe_buffer_start;
                    }
                    bVar12 = iVar6 == 0;
                }
                piVar9 = local_34;
                if (bVar12) {
                    break;
                }
                pStep_callback(pStep_count);
            }
        } else {
            piVar9 = (tPipe_chunk*)gPipe_play_ptr;
            if (pStep_count < 0) {
                do {
                    iVar6 = MoveSessionPointerBackOne((tU8**)&local_34);
                    if (iVar6 == 0) {
                        iVar6 = LengthOfSession(local_34);
                        uStack_24 = 0;
                        gPipe_buffer_end_marker = (tU8*)local_34 + iVar6 - 2;
                        if (local_34->count != 0) {
                            current_chunk = (tPipe_chunk*)((tU8*)local_34 + sizeof(tPipe_chunk));
                            callback = &gPipe_callbacks[local_34->type];
                            do {
                                iVar3 = (int)gPipe_buffer_end_marker;
                                iVar2 = callback->flags;
                                if (iVar2 < 2 || iVar2 == 5) {
                                    undo_target = NULL;
                                } else {
                                    undo_target = FUN_00403800(local_34, local_34->type, ((tPipe_chunk_data*)current_chunk)->count);
                                    callback = &gPipe_callbacks[local_34->type];
                                }
                                gPipe_buffer_end_marker = (tU8*)iVar3;
                                switch (callback->flags) {
                                case 1:
                                    if (callback->reset != NULL) {
                                        callback->reset();
                                        goto reverse_advance_chunk;
                                    }
                                    break;
                                case 2:
                                    if (callback->undo != NULL) {
                                        if (undo_target != NULL) {
                                            gPipe_undo_enabled = 1;
                                            callback->undo(&current_chunk, (tPipe_chunk*)undo_target);
                                            gPipe_undo_enabled = 0;
                                        }
                                        goto reverse_advance_chunk;
                                    }
                                    break;
                                case 3:
                                    if (callback->apply != NULL) {
                                        if (undo_target != NULL) {
                                            gPipe_undo_enabled = 1;
                                            callback->undo(&current_chunk, (tPipe_chunk*)undo_target);
                                            gPipe_undo_enabled = 0;
                                            goto reverse_advance_chunk;
                                        }
                                        callback->apply(&current_chunk);
                                        callback = &gPipe_callbacks[local_34->type];
                                    }
                                    break;
                                case 4:
                                case 5:
                                    if (callback->undo != NULL) {
                                        callback->undo(&current_chunk, (tPipe_chunk*)undo_target);
                                    }
reverse_advance_chunk:
                                    ARAdvanceChunkPtr(&current_chunk, local_34->type);
                                    callback = &gPipe_callbacks[local_34->type];
                                    break;
                                }
                                uStack_24++;
                            } while (uStack_24 < local_34->count);
                        }
                        next_session = local_34;
                        iVar6 = MoveSessionPointerBackOne((tU8**)&next_session);
                        if (iVar6 == 0) {
                            bVar12 = next_session->type == 0;
                        } else {
                            bVar12 = 1;
                        }
                    } else {
                        bVar12 = 1;
                    }
                    if (bVar12) {
                        break;
                    }
                    pStep_callback(pStep_count);
                } while (1);
                gPipe_play_ptr = (tU8*)local_34;
                piVar9 = (tPipe_chunk*)gPipe_play_ptr;
            }
        }
        gPipe_play_ptr = (tU8*)piVar9;
        pFrame_end_callback(gFrame_period);
        local_14++;
        piVar9 = piVar4;
        if (iVar5 <= local_14) {
            goto replay_complete;
        }
        goto replay_outer;
    }

replay_complete:
    if (gPipe_play_ptr == gPipe_record_ptr) {
        bVar12 = 0;
    } else if (gPipe_play_ptr == gPipe_buffer_oldest) {
        bVar12 = 1;
    } else if (gReplay_rate == 0.0f) {
        bVar12 = 0 < gPlay_direction;
    } else if (gReplay_rate <= 0.0f) {
        bVar12 = 0;
    } else {
        bVar12 = 1;
    }
    if (!bVar12) {
        ScanAndPlaySoundsToBe(initial_session, start_time, GetTotalTime());
    }
    if (piVar9 == local_34) {
        gReplay_rate = 0.0f;
        gReplay_pause_state = 1;
    }
    if (KeyIsDown(0) != 0) {
        WaitForNoKeys();
    }
}
#pragma auto_inline(on)

#pragma auto_inline(off)
// FUNCTION: CARMA2_HW 0x00402dd0
void C2_HOOK_FASTCALL FUN_00402dd0(void) {
    gPipe_play_ptr = gPipe_record_ptr;
}
#pragma auto_inline(on)

#pragma auto_inline(off)
// FUNCTION: CARMA2_HW 0x00403210
void C2_HOOK_FASTCALL FUN_00403210(tARDoActionReplay_step_callback* pStep_callback, tARDoActionReplay_frame_callback* pFrame_start_callback, tARDoActionReplay_frame_callback* pFrame_end_callback) {
    float saved_replay_rate;

    saved_replay_rate = gReplay_rate;
    gDisable_sound = 1;
    gReplay_rate = 100.0f;
    ARDoActionReplay(0x7fffffff, pStep_callback, pFrame_start_callback, pFrame_end_callback);
    gDisable_sound = 0;
    gReplay_rate = saved_replay_rate;
}
#pragma auto_inline(on)

#pragma auto_inline(off)
// FUNCTION: CARMA2_HW 0x004c6e40
void C2_HOOK_FASTCALL FUN_004c6e40(br_model* pModel, br_vector3* pDirection, const float* pScalars) {
#pragma inline_depth(0)
    int data_size;
    undefined4* pipe_data;

    pipe_data = gCrush_space;
    if (pipe_data != NULL) {
        data_size = pModel->nvertices * 2 + 10;
        if ((unsigned int)data_size > CRUSH_SPACE_SIZE) {
            FatalError(kFatalError_ChunkIsTooLargeToPipe);
        }
        pipe_data[0] = (undefined4)pModel;
        FUN_0043bca0((tCompressed_vector3*)(pipe_data + 1), (tU16*)((tU8*)pipe_data + 0xa), pModel, pDirection, pScalars);
        ARAddDataToSession(3, -1, pipe_data, data_size);
    }
}

// FUNCTION: CARMA2_HW 0x004c8650
void C2_NAKED C2_HOOK_FASTCALL FUN_004c8650(br_model* pModel, br_vector3* pMash) {
#ifndef CARPOCALYPSE2_MATCHING
    int size;
    tPipe_model_mash* pipe_model_mash;

    pipe_model_mash = (tPipe_model_mash*)gCrush_space;
    if (pipe_model_mash != NULL) {
        size = (int)pModel->nvertices * 6 + 4;
        if ((unsigned int)size > CRUSH_SPACE_SIZE) {
            FatalError(kFatalError_ChunkIsTooLargeToPipe);
        }
        pipe_model_mash->model = pModel;
        ARCompressModelMash(pipe_model_mash->vectors, pModel, pMash);
        ARDoSingleSession(ePipe_model_mash, -1, pipe_model_mash, size);
    }
#else
    __asm {
            push ebp
            push esi
            push edi
            mov edi, dword ptr [gCrush_space]
            test edi, edi
            mov ebp, edx
            mov esi, ecx
            je L004c869e
            xor eax, eax
            push ebx
            mov ax, word ptr [esi + 0x10]
            lea eax, [eax + eax*2]
            lea ebx, [eax + eax + 4]
            cmp ebx, 0x4000
            jbe L004c8681
            push 0x7d
            call FatalError
            add esp, 4
    L004c8681:
            push ebp
            mov edx, esi
            lea ecx, [edi + 4]
            mov dword ptr [edi], esi
            call ARCompressModelMash
            or edx, 0xffffffff
            mov ecx, 0x18
            push ebx
            push edi
            call ARDoSingleSession
            pop ebx
    L004c869e:
            pop edi
            pop esi
            pop ebp
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

#pragma inline_depth()
#pragma auto_inline(on)

#pragma auto_inline(off)
// FUNCTION: CARMA2_HW 0x004c72f0
void C2_HOOK_FASTCALL AddGraphicalWheelStuffToPipingSession(tCar_spec* pCar) {

    ARAddVariedDataToSession(ePipe_chunk_single_graphpical_wheel_stuff, (uintptr_t)(tU16)pCar->car_ID, 5,
        2, 0, (int)(*(float*)((tU8*)pCar + 0x4a0) * 546.1166666666667),
        1, 2, (int)(*(float*)((tU8*)pCar + 0x4a8) * 127.0f * 6.666666666666667),
        1, 3, (int)(*(float*)((tU8*)pCar + 0x4ac) * 127.0f * 6.666666666666667),
        1, 4, (int)(*(float*)((tU8*)pCar + 0x4b0) * 127.0f * 6.666666666666667),
        1, 5, (int)(*(float*)((tU8*)pCar + 0x4b4) * 127.0f * 6.666666666666667));
}
#pragma auto_inline(on)

#pragma auto_inline(off)
// FUNCTION: CARMA2_HW 0x004c73a0
void C2_HOOK_FASTCALL AddSoundToPipingSession(tS3_outlet* pOutlet, int pSound, tS3_volume pL_volume, tS3_volume pR_volume, int pPitch, const br_vector3* pPosition) {
    if (!gAction_replay_mode && gProgram_state.racing) {
        const br_vector3* pos;

        pos = pPosition != NULL ? pPosition : &gZero_vector__smash;
        ARAddVariedDataToSession(ePipe_chunk_single_sound, pSound, 4,
            4, 0x0, pPitch,
            0xc, 0x4, (uintptr_t)pos,
            2, 0x10, (uint32_t)GetIndexFromOutlet(pOutlet),
            2, 0x12, (uint32_t)((pR_volume << 8) + pL_volume));
    }
}
#pragma auto_inline(on)

#pragma auto_inline(off)
// FUNCTION: CARMA2_HW 0x004c7500
void C2_HOOK_FASTCALL FUN_004c7500(uintptr_t pOwner, void* pData, tU32 pArg3) {
    ARAddVariedDataToSession(ePipe_chunk_unk_22, pOwner, 2,
        0x30, 0, (uintptr_t)pData,
        4, 0x30, pArg3);
}
#pragma auto_inline(on)

#pragma auto_inline(off)
// FUNCTION: CARMA2_HW 0x004c75b0
void C2_HOOK_FASTCALL FUN_004c75b0(tU32 pArg1, br_actor* pActor) {
    ARAddVariedDataToSession(0x19, (uintptr_t)pActor, 3,
        4, 0, pActor,
        4, 4, pActor->parent,
        4, 8, pArg1);
}
#pragma auto_inline(on)

#pragma auto_inline(off)
// FUNCTION: CARMA2_HW 0x004c7f40
void C2_HOOK_FASTCALL FUN_004c7f40(br_model* pModel, br_vector3* pSrc, const float* pScalars) {
#pragma inline_depth(0)
    undefined4* piVar2;
    tU32 uVar1;

    piVar2 = gCrush_space;
    if (gCrush_space != NULL) {
        uVar1 = (tU32)pModel->nvertices * 2 + 10;
        if (uVar1 > 0x4000) {
            FatalError(kFatalError_ChunkIsTooLargeToPipe);
        }
        *piVar2 = (undefined4)pModel;
        FUN_0043bca0((tCompressed_vector3*)(piVar2 + 1), (tU16*)((tU8*)piVar2 + 0xa), pModel, pSrc, pScalars);
        ARDoSingleSession(3, -1, piVar2, uVar1);
    }
}
#pragma inline_depth()
#pragma auto_inline(on)

#pragma auto_inline(off)
// FUNCTION: CARMA2_HW 0x004c7fa0
void C2_HOOK_FASTCALL FUN_004c7fa0(tU16 pCar_ID, int pModel_index, int pVertex_count, tSmudged_vertex* pCoordinates) {
#pragma inline_depth(0)
    tU32 copy_size;

    if (gSmudge_space != NULL) {
        if (pVertex_count > 600) {
            pVertex_count = 600;
        }
        copy_size = pVertex_count * sizeof(tSmudged_vertex);
        gSmudge_space->vertex_count = (tU16)pVertex_count;
        gSmudge_space->model_index = pModel_index;
        memcpy(gSmudge_space->vertex_changes, pCoordinates, copy_size);
        ARDoSingleSession(ePipe_chunk_smudge, pCar_ID, gSmudge_space, copy_size + offsetof(tPipe_smudge_data, vertex_changes));
    }
}
#pragma inline_depth()
#pragma auto_inline(on)

#pragma auto_inline(off)
// FUNCTION: CARMA2_HW 0x004c8500
void C2_HOOK_FASTCALL FUN_004c8500(uintptr_t pOwner, const br_vector3* pVector) {
#pragma inline_depth(0)
    br_vector3 vector;

    vector = *pVector;
    ARDoSingleSession(7, pOwner, &vector, sizeof(vector));
}
#pragma inline_depth()

// PipeSingleSpecial

// PipeSingleCarIncident

// PipeSinglePedIncident

// PipeSingleWallIncident

// PipeSingleSkidAdjustment

// PipeSingleFlap

// PipeSingleRelink

// PipeSingleIdentity

// PipeSingleSplitWeld

// PipeSingleShrapnelShower

// PipeSingleExplosion

// PipeSingleNonCarCreation

// PipeSingleSmashModelChange

// PipeSingleSmashRemoveFaces

// PipeSingleSmashDecal

// PipeSingleSmashTextureChange

// PipeSinglePedStatus

// PipeSinglePedDir

// PipeSinglePedMove

// PipeSinglePedPhysics

// PipeSinglePedDismember

// PipeSinglePedFPChange

// PipeSinglePedPos

// PipeSinglePedModelChange

// PipeSinglePHILObject

// PipeSingleGibShower

// PipeSingleBloodSpurt

// PipeSingleGrooveOnOff

// PipeSinglePowerupGot

// PipeSinglePowerupLose
#pragma auto_inline(on)

#pragma auto_inline(off)
// FUNCTION: CARMA2_HW 0x004c85c0
void C2_HOOK_FASTCALL PipeSingleProxRay(int pRay_index, tCar_spec* pCar, tU16 pPed_index, tU32 pTime) {
    ARDoSingleVariedSession(ePipe_chunk_prox_ray, pRay_index, 3,
        SIZE_OFFSET_PIPING(tPipe_chunk_prox_ray, ped_index), (tU16)pPed_index,
        SIZE_OFFSET_PIPING(tPipe_chunk_prox_ray, car_ID), (tU16)pCar->car_ID,
        SIZE_OFFSET_PIPING(tPipe_chunk_prox_ray, time), pTime);
}
#pragma auto_inline(on)

#pragma auto_inline(off)
// FUNCTION: CARMA2_HW 0x004c86b0
void C2_HOOK_FASTCALL FUN_004c86b0(tU32 pArg1, br_actor* pActor) {
    ARDoSingleVariedSession(0x19, (uintptr_t)pActor, 3,
        4, 0, pActor,
        4, 4, pActor->parent,
        4, 8, pArg1);
}
#pragma auto_inline(on)

#pragma auto_inline(off)
// FUNCTION: CARMA2_HW 0x004c86f0
void C2_HOOK_FASTCALL FUN_004c86f0(int pOwner, void* pData, tU32 pArg3) {
    ARDoSingleVariedSession(0x1b, pOwner, 2,
        0x10, 0, pData,
        4, 0x10, pArg3);
}
#pragma auto_inline(on)

#pragma auto_inline(off)
// FUNCTION: CARMA2_HW 0x004c8940
void C2_HOOK_FASTCALL FUN_004c8940(uintptr_t pOwner) {
#pragma inline_depth(0)

    ARDoSingleSession(0x20, pOwner, &gZero_vector__smash, 4);
}
#pragma inline_depth()
#pragma auto_inline(on)

#pragma auto_inline(off)
// FUNCTION: CARMA2_HW 0x004c8a40
void C2_HOOK_FASTCALL PipeSingleRepulseRay(int pIndex, br_vector3* pOrigin, br_vector3* pVictim, tU32 pTime) {
    ARDoSingleVariedSession(ePipe_chunk_repulse_ray, (uintptr_t)pIndex, 3,
        SIZE_OFFSET_PIPING(tPipe_chunk_repulse_ray, origin), pOrigin,
        SIZE_OFFSET_PIPING(tPipe_chunk_repulse_ray, victim), pVictim,
        SIZE_OFFSET_PIPING(tPipe_chunk_repulse_ray, time), pTime);
}
#pragma auto_inline(on)

#pragma auto_inline(off)
// FUNCTION: CARMA2_HW 0x004c8bf0
void C2_HOOK_FASTCALL FUN_004c8bf0(int pId, tU8 pValue_1, char pValue_2, char pValue_3) {
    ARDoSingleVariedSession(0x2c, pId, 3,
        2, 0, pValue_1,
        1, 2, pValue_2,
        1, 3, pValue_3);
}
#pragma auto_inline(on)

#pragma auto_inline(off)
// FUNCTION: CARMA2_HW 0x004c8e50
void C2_HOOK_FASTCALL PipeSinglePedestrian(tPedestrian* pPed) {
    ARDoSingleVariedSession(0x39, (uintptr_t)pPed, 3,
        sizeof(tPedestrian), 0, pPed,
        sizeof(tPed_character_instance), sizeof(tPedestrian), pPed->character,
        sizeof(tPed_cache_006944c0), sizeof(tPedestrian) + sizeof(tPed_character_instance), pPed->field_0x0c);
}
#pragma auto_inline(on)

#pragma auto_inline(off)
// FUNCTION: CARMA2_HW 0x004c8f60
void C2_HOOK_STDCALL FUN_004c8f60(int pArg1) {
    // empty
}
#pragma auto_inline(on)

#pragma auto_inline(off)
// FUNCTION: CARMA2_HW 0x004c8fb0
void C2_HOOK_FASTCALL PipeSingleMatrix34Type66(br_actor* pActor) {
    ARDoSingleVariedSession(0x42, (uintptr_t)pActor, 1,
        0x30, 0, &pActor->t.t.mat);
}
#pragma auto_inline(on)

#pragma auto_inline(off)
// FUNCTION: CARMA2_HW 0x0043bca0
void C2_NAKED C2_HOOK_FASTCALL FUN_0043bca0(tCompressed_vector3* pDest, tU16* pDest2, br_model* pModel, br_vector3* pSrc, const float* pScalars) {
#ifndef CARPOCALYPSE2_MATCHING
    br_vector3 normalized;
    br_scalar length;
    int i;

    length = BrVector3Length(pSrc);
    normalized.v[0] = pSrc->v[0] / length;
    normalized.v[1] = pSrc->v[1] / length;
    normalized.v[2] = pSrc->v[2] / length;
    CompressVector3(pDest, &normalized, -1.f, 1.f);
    for (i = 0; i < pModel->nvertices; i++) {
        pDest2[i] = DRScalarToU16(pScalars[i] * length, 0.f, 10.f);
    }
#else
    __asm {
            sub esp, 0x10
            mov eax, dword ptr [esp + 0x18]
            push ebx
            push ebp
            push edi
            fld dword ptr [eax + 4]
            fld dword ptr [eax + 8]
            fld dword ptr [eax]
            fxch st(2)
            fst dword ptr [esp + 0x24]
            fxch st(1)
            fst dword ptr [esp + 0xc]
            fxch st(1)
            fmul dword ptr [esp + 0x24]
            fxch st(1)
            fmul dword ptr [esp + 0xc]
            fld st(2)
            fmul st(0), st(3)
            fxch st(1)
            faddp st(2), st(0)
            fld dword ptr [eax + 4]
            fld dword ptr [eax + 8]
            fxch st(2)
            faddp st(3), st(0)
            fld dword ptr [eax]
            fxch st(3)
            fsqrt
            mov ebx, edx
            push 0x3f800000
            push 0xbf800000
            lea edx, [esp + 0x18]
            fst dword ptr [esp + 0x2c]
            fstp dword ptr [esp + 0x2c]
            fxch st(2)
            fdiv dword ptr [esp + 0x2c]
            fxch st(2)
            fdiv dword ptr [esp + 0x2c]
            fxch st(1)
            fdiv dword ptr [esp + 0x2c]
            fxch st(2)
            fstp dword ptr [esp + 0x18]
            fstp dword ptr [esp + 0x1c]
            fstp dword ptr [esp + 0x20]
            fstp st(0)
            call CompressVector3
            xor edi, edi
            mov ebp, dword ptr [esp + 0x20]
            cmp word ptr [ebp + 0x10], di
            jbe L0043bd5d
            push esi
    L0043bd2e:
            mov eax, dword ptr [esp + 0x2c]
            mov esi, edi
            and esi, 0xffff
            push 0x41200000
            push 0
            push ecx
            fld dword ptr [eax + esi*4]
            fmul dword ptr [esp + 0x34]
            fstp dword ptr [esp]
            call DRScalarToU16
            inc edi
            mov word ptr [ebx + esi*2], ax
            cmp di, word ptr [ebp + 0x10]
            jb L0043bd2e
            pop esi
    L0043bd5d:
            pop edi
            pop ebp
            pop ebx
            add esp, 0x10
            ret 0xc
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

// FUNCTION: CARMA2_HW 0x004c8110
void C2_HOOK_FASTCALL PipeSingleSmoke(int pIndex, tU8 pType, br_vector3* pPos, br_scalar pRadius, br_scalar pStrength) {
    tReduced_pos reduced_pos;

    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tPipe_chunk_smoke, pos, 0x0);
    C2_HOOK_STATIC_ASSERT_STRUCT_MEMBER_SIZE(tPipe_chunk_smoke, pos, 0x6);
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tPipe_chunk_smoke, radius, 0x6);
    C2_HOOK_STATIC_ASSERT_STRUCT_MEMBER_SIZE(tPipe_chunk_smoke, radius, 0x2);
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tPipe_chunk_smoke, strength, 0x8);
    C2_HOOK_STATIC_ASSERT_STRUCT_MEMBER_SIZE(tPipe_chunk_smoke, strength, 0x1);
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tPipe_chunk_smoke, type, 0x9);
    C2_HOOK_STATIC_ASSERT_STRUCT_MEMBER_SIZE(tPipe_chunk_smoke, type, 0x1);

    SaveReducedPos(&reduced_pos, pPos);
    ARDoSingleVariedSession(ePipe_chunk_smoke, (uintptr_t)pIndex, 4,
        SIZE_OFFSET_PIPING(tPipe_chunk_smoke, pos), &reduced_pos,
        SIZE_OFFSET_PIPING(tPipe_chunk_smoke, radius), (int)(1024.f * pRadius),
        SIZE_OFFSET_PIPING(tPipe_chunk_smoke, strength), (int)(pStrength * 255.f),
        SIZE_OFFSET_PIPING(tPipe_chunk_smoke, type), (tU8)pType);
}

// FUNCTION: CARMA2_HW 0x004c65f0
void C2_HOOK_FASTCALL ClearCarActiveFlags(void) {
    int category;
    int car_count;
    int car_index;
    tCar_spec* car;

    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tCar_spec, active, 0x1d0);

    for (category = eVehicle_self; category <= eVehicle_drone; category++) {
        car_count = category == eVehicle_self ? 1 : GetCarCount(category);
        for (car_index = 0; car_index < car_count; car_index++) {
            car = category == eVehicle_self ? &gProgram_state.current_car : GetCarSpec(category, car_index);
            car->active = 0;
        }
    }
}

// FUNCTION: CARMA2_HW 0x004c8360
void C2_NAKED C2_HOOK_FASTCALL PipeSingleCar(int pCar_id, br_matrix34* pMatrix, br_vector3* pV, float pSpeedo_speed, float pRevs, int pGear, int pFrame_collision_flag, undefined4 pArg7) {
#ifndef CARPOCALYPSE2_MATCHING
    int flags;

    flags = pFrame_collision_flag != 0 ? 0x800 : 0;
    flags += ((int)pRevs / 10) & 0x7ff;
    flags += (pGear + 1) << 12;
    ARDoSingleVariedSession(ePipe_chunk_car, (uintptr_t)pCar_id, 5,
        SIZE_OFFSET_PIPING(tPipe_car_chunk, mat), pMatrix,
        SIZE_OFFSET_PIPING(tPipe_car_chunk, v), pV,
        SIZE_OFFSET_PIPING(tPipe_car_chunk, speedo_speed), (int)(pSpeedo_speed * gDouble_0058a908),
        SIZE_OFFSET_PIPING(tPipe_car_chunk, flags), flags,
        SIZE_OFFSET_PIPING(tPipe_car_chunk, field_0x40), pArg7);
#else
    __asm {
            mov eax, dword ptr [esp + 0x18]
            push ebx
            mov ebx, dword ptr [esp + 0x18]
            push esi
            fld dword ptr [esp + 0x14]
            neg ebx
            push edi
            push eax
            sbb ebx, ebx
            mov esi, edx
            mov edi, ecx
            push 0x40
            push 4
            and ebx, 0x800
            call _ftol
            mov ecx, eax
            mov eax, 0x66666667
            imul ecx
            fld dword ptr [esp + 0x20]
            fmul qword ptr [gDouble_0058a908]
            sar edx, 2
            mov ecx, edx
            shr ecx, 0x1f
            add edx, ecx
            and edx, 0x7ff
            add ebx, edx
            mov edx, dword ptr [esp + 0x28]
            inc edx
            shl edx, 0xc
            add ebx, edx
            push ebx
            push 0x3e
            push 2
            call _ftol
            push eax
            mov eax, dword ptr [esp + 0x2c]
            push 0x3c
            push 2
            push eax
            push 0x30
            push 0xc
            push esi
            push 0
            push 0x30
            push 5
            push edi
            push 5
            call ARDoSingleVariedSession
            add esp, 0x48
            pop edi
            pop esi
            pop ebx
            ret 0x18
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

// FUNCTION: CARMA2_HW 0x004c8710
void C2_NAKED C2_HOOK_FASTCALL FUN_004c8710(uintptr_t pOwner, tU16 pValue_0, tU16 pValue_2, tU16 pValue_4, br_vector3* pVector, br_scalar pValue_20) {
#ifndef CARPOCALYPSE2_MATCHING
    gUNK_0069410c_u32 = *(tU32*)&pValue_20;
    ARDoSingleVariedSession(0x1c, pOwner, 5,
        2, 0, pValue_0,
        2, 2, pValue_2,
        2, 4, pValue_4,
        0xc, 8, pVector,
        4, 0x14, gUNK_0069410c_u32);
#else
    __asm {
            push ecx
            mov eax, dword ptr [esp + 0x14]
            mov word ptr [esp], dx
            mov edx, eax
            mov dword ptr [gUNK_0069410c], eax
            mov eax, dword ptr [esp + 0x10]
            push edx
            mov edx, dword ptr [esp + 0x10]
            push 0x14
            push 4
            push eax
            mov eax, dword ptr [esp + 0x18]
            push 8
            and edx, 0xffff
            push 0xc
            push edx
            mov edx, dword ptr [esp + 0x1c]
            push 4
            and eax, 0xffff
            push 2
            push eax
            push 2
            and edx, 0xffff
            push 2
            push edx
            push 0
            push 2
            push 5
            push ecx
            push 0x1c
            call ARDoSingleVariedSession
            add esp, 0x48
            pop ecx
            ret 0x10
            nop
            nop
            nop
            nop
            push ecx
            mov eax, dword ptr [esp + 0xc]
            mov word ptr [esp], dx
            mov edx, dword ptr [esp + 8]
            and eax, 0xffff
            push eax
            mov eax, dword ptr [esp + 4]
            push 4
            and edx, 0xffff
            push 2
            push edx
            push 2
            and eax, 0xffff
            push 2
            push eax
            push 0
            push 2
            push 3
            push ecx
            push 0x1d
            call ARDoSingleVariedSession
            add esp, 0x30
            pop ecx
            ret 8
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
            push ecx
            mov eax, dword ptr [gCrush_space]
            push ebp
            test eax, eax
            mov dword ptr [esp + 4], ecx
            mov ebp, eax
            je L004c8862
            push edi
            push esi
            lea edi, [eax + 4]
            mov eax, dword ptr [esp + 0x1c]
            mov ecx, 0x1f
            mov esi, edx
            rep movsd
            push ebx
            lea ecx, [ebp + 0x80]
            mov edx, 0x40
    L004c87f3:
            mov si, word ptr [eax]
            add eax, 2
            mov word ptr [ecx], si
            add ecx, 2
            dec edx
            jne L004c87f3
            mov eax, dword ptr [esp + 0x1c]
            mov esi, dword ptr [esp + 0x18]
            cdq
            and edx, 3
            lea edi, [ebp + 0x100]
            add eax, edx
            mov ebx, eax
            sar ebx, 2
            add ebx, 2
            mov ecx, ebx
            add ebx, 0x100
            mov eax, ecx
            shr ecx, 2
            rep movsd
            mov ecx, eax
            and ecx, 3
            cmp ebx, 0x4000
            rep movsb
            jbe L004c8846
            push 0x7d
            call FatalError
            add esp, 4
    L004c8846:
            test bl, 1
            je L004c884c
            inc ebx
    L004c884c:
            mov edx, dword ptr [esp + 0x10]
            push ebx
            push ebp
            mov ecx, 0x26
            mov dword ptr [ebp], ebx
            call ARDoSingleSession
            pop ebx
            pop esi
            pop edi
    L004c8862:
            pop ebp
            pop ecx
            ret 0xc
    }
#endif
}
#pragma auto_inline(on)

// FUNCTION: CARMA2_HW 0x00403800
C2_NAKED tPipe_chunk_data* C2_HOOK_FASTCALL FUN_00403800(tPipe_chunk* pSession, tPipe_chunk_type pType, tU32 pOwner) {
#ifndef CARPOCALYPSE2_MATCHING
    int session_count;
    int chunk_index;
    tU8* session_ptr;
    tU8* chunk_ptr;
    tU8* end_marker;

    pOwner &= 0xfff;
    session_count = 0;
    session_ptr = (tU8*)pSession;
    for (;;) {
        if (session_ptr == gPipe_buffer_oldest && session_ptr != gPipe_record_ptr) {
            return NULL;
        }
        if (session_ptr == gPipe_buffer_start) {
            session_ptr = gPipe_buffer_working_end;
        }
        session_ptr -= sizeof(tU16);
        session_ptr -= *(tU16*)session_ptr;
        session_count++;
        if (session_count > gPipe_session_search_limit) {
            return NULL;
        }
        pSession = (tPipe_chunk*)session_ptr;
        end_marker = session_ptr + LengthOfSession(pSession) - sizeof(tU16);
        gPipe_buffer_end_marker = end_marker;
        if (pSession->type == pType) {
            chunk_ptr = session_ptr + sizeof(tPipe_chunk);
            for (chunk_index = 0; (tU32)chunk_index < pSession->count; chunk_index++) {
                if ((*(tU32*)chunk_ptr & 0xfff) == pOwner) {
                    return (tPipe_chunk_data*)chunk_ptr;
                }
                if (gPipe_undo_enabled == 0) {
                    tU8* next_chunk_ptr;

                    next_chunk_ptr = chunk_ptr + LengthOfChunk(chunk_ptr, pType);
                    if (next_chunk_ptr == end_marker || (int)end_marker < (int)next_chunk_ptr) {
                        next_chunk_ptr = chunk_ptr;
                    }
                    chunk_ptr = next_chunk_ptr;
                }
            }
        }
    }
#else
    __asm {
            sub esp, 0x10
            mov eax, dword ptr [esp + 0x14]
            push ebx
            push ebp
            mov ebp, ecx
            xor ecx, ecx
            mov dword ptr [esp + 0xc], edx
            mov edx, dword ptr [gPipe_callbacks]
            and eax, 0xfff
            push esi
            push edi
            mov dword ptr [esp + 0x18], eax
    L00403822:
            cmp ebp, dword ptr [gPipe_buffer_oldest]
            jne L00403836
            cmp ebp, dword ptr [gPipe_record_ptr]
            jne L0040397a
    L00403836:
            cmp ebp, dword ptr [gPipe_buffer_start]
            jne L00403844
            mov ebp, dword ptr [gPipe_buffer_working_end]
    L00403844:
            mov esi, dword ptr [gPipe_session_search_limit]
            sub ebp, 2
            xor eax, eax
            mov ax, word ptr [ebp]
            sub ebp, eax
            mov eax, ecx
            inc ecx
            cmp eax, esi
            mov dword ptr [esp + 0x1c], ecx
            jge L0040397a
            mov eax, dword ptr [ebp + 4]
            xor ebx, ebx
            xor edi, edi
            mov dword ptr [esp + 0x10], 0
            test eax, eax
            jbe L004038af
    L00403877:
            mov eax, dword ptr [ebp]
            lea ecx, [ebx + ebp + 8]
            lea esi, [eax + eax*2]
            shl esi, 3
            mov eax, dword ptr [edx + esi + 8]
            test eax, eax
            je L0040389e
            call eax
            mov edx, dword ptr [gPipe_callbacks]
            mov ecx, dword ptr [edx + esi + 4]
            lea esi, [eax + ecx + 4]
            jmp L004038a5
    L0040389e:
            mov esi, dword ptr [edx + esi + 4]
            add esi, 4
    L004038a5:
            mov eax, dword ptr [ebp + 4]
            add ebx, esi
            inc edi
            cmp edi, eax
            jb L00403877
    L004038af:
            add ebx, 0xa
            test bl, 1
            je L004038c7
            push 0x62
            call FatalError
            mov edx, dword ptr [gPipe_callbacks]
            add esp, 4
    L004038c7:
            lea ecx, [ebx + ebp - 2]
            lea edi, [ebp + 8]
            mov dword ptr [gPipe_buffer_end_marker], ecx
            mov eax, dword ptr [ebp + 4]
            test eax, eax
            mov dword ptr [esp + 0x24], 0
            jbe L00403951
    L004038e3:
            mov esi, dword ptr [esp + 0x14]
            mov eax, dword ptr [ebp]
            cmp eax, esi
            jne L00403951
            mov eax, dword ptr [edi]
            mov ebx, dword ptr [esp + 0x18]
            and eax, 0xfff
            cmp eax, ebx
            je L00403962
            mov eax, dword ptr [gPipe_undo_enabled]
            mov ebx, edi
            test eax, eax
            jne L00403941
            lea esi, [esi + esi*2]
            shl esi, 3
            mov eax, dword ptr [edx + esi + 8]
            test eax, eax
            je L00403930
            mov ecx, edi
            call eax
            mov edx, dword ptr [gPipe_callbacks]
            mov ecx, dword ptr [edx + esi + 4]
            lea esi, [eax + ecx + 4]
            mov ecx, dword ptr [gPipe_buffer_end_marker]
            jmp L00403937
    L00403930:
            mov esi, dword ptr [edx + esi + 4]
            add esi, 4
    L00403937:
            add edi, esi
            cmp edi, ecx
            je L0040393f
            jle L00403941
    L0040393f:
            mov edi, ebx
    L00403941:
            mov eax, dword ptr [esp + 0x24]
            mov esi, dword ptr [ebp + 4]
            inc eax
            cmp eax, esi
            mov dword ptr [esp + 0x24], eax
            jb L004038e3
    L00403951:
            mov ecx, dword ptr [esp + 0x1c]
    L00403955:
            mov eax, dword ptr [esp + 0x10]
            test eax, eax
            jne L0040396e
            jmp L00403822
    L00403962:
            mov eax, edi
            pop edi
            pop esi
            pop ebp
            pop ebx
            add esp, 0x10
            ret 4
    L0040396e:
            pop edi
            pop esi
            pop ebp
            xor eax, eax
            pop ebx
            add esp, 0x10
            ret 4
    L0040397a:
            mov dword ptr [esp + 0x10], 1
            jmp L00403955
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

// FUNCTION: CARMA2_HW 0x0043caf0
void C2_NAKED C2_HOOK_FASTCALL ReceivedCrushDetach(tU8* pContents) {
#ifndef CARPOCALYPSE2_MATCHING
    tCar_spec* car;
    tFind_crush_actor_arg find_arg;
    tU32 actor;
    int count;
    int i;

    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tFind_crush_actor_arg, actor, 0x4);
    if (gCount_net_crush_detach_list != CARPOCALYPSE2_ASIZE(gNet_crush_detach_list)) {
        car = NetCarFromPlayerID(*(tPlayer_ID*)(pContents + 4));
        if (car != NULL) {
            find_arg.id = pContents[8];
            find_arg.actor = NULL;
            actor = DRActorEnumRecurse(
                car->car_model_actor,
                FindActorByCrushEntryID,
                &find_arg) != 0
                ? (tU32)find_arg.actor
                : 0;
            if (actor != 0) {
                count = gCount_net_crush_detach_list;
                for (i = 0; i < count; i++) {
                    if (gNet_crush_detach_list[i].actor == (br_actor*)actor) {
                        return;
                    }
                }
                gNet_crush_detach_list[count].car = car;
                gNet_crush_detach_list[count].actor = (br_actor*)actor;
                gNet_crush_detach_list[count].field_0x8 = *(tU32*)(pContents + 0xc);
                ExpandVector3(&gNet_crush_detach_list[count].field_0xc, (tCompressed_vector3*)(pContents + 0x10), -10.f, 10.f);
                ExpandVector3(&gNet_crush_detach_list[gCount_net_crush_detach_list].field_0x18, (tCompressed_vector3*)(pContents + 0x16), -10.f, 10.f);
                gCount_net_crush_detach_list += 1;
            }
        }
    }
#else
    __asm {
            mov eax, dword ptr [gCount_net_crush_detach_list]
            sub esp, 8
            cmp eax, 8
            push ebx
            push esi
            push edi
            mov edi, ecx
            je L0043cbc1
            mov ecx, dword ptr [edi + 4]
            call NetCarFromPlayerID
            mov ebx, eax
            test ebx, ebx
            je L0043cbc1
            mov al, byte ptr [edi + 8]
            lea ecx, [esp + 0xc]
            push ecx
            mov ecx, dword ptr [ebx + 0xe0c]
            mov edx, offset FindActorByCrushEntryID
            mov byte ptr [esp + 0x10], al
            mov dword ptr [esp + 0x14], 0
            call DRActorEnumRecurse
            neg eax
            mov edx, dword ptr [esp + 0x10]
            sbb eax, eax
            and eax, edx
            test eax, eax
            je L0043cbc1
            mov ecx, dword ptr [gCount_net_crush_detach_list]
            xor edx, edx
            test ecx, ecx
            jle L0043cb67
            mov esi, offset gNet_crush_detach_list
    L0043cb5b:
            cmp dword ptr [esi], eax
            je L0043cbc1
            inc edx
            add esi, 0x24
            cmp edx, ecx
            jl L0043cb5b
    L0043cb67:
            lea ecx, [ecx + ecx*8]
            push 0x41200000
            shl ecx, 2
            push 0xc1200000
            mov dword ptr [ecx + gNet_crush_detach_list+0x4], ebx
            mov dword ptr [ecx + gNet_crush_detach_list], eax
            mov edx, dword ptr [edi + 0xc]
            mov dword ptr [ecx + gNet_crush_detach_list+0x8], edx
            lea edx, [edi + 0x10]
            lea ecx, [ecx + gNet_crush_detach_list+0xc]
            call ExpandVector3
            mov eax, dword ptr [gCount_net_crush_detach_list]
            lea edx, [edi + 0x16]
            push 0x41200000
            push 0xc1200000
            lea eax, [eax + eax*8]
            lea ecx, [eax*4 + gNet_crush_detach_list+0x18]
            call ExpandVector3
            inc dword ptr [gCount_net_crush_detach_list]
    L0043cbc1:
            pop edi
            pop esi
            pop ebx
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
    }
#endif
}

// FUNCTION: CARMA2_HW 0x0043cbd0
void C2_NAKED C2_HOOK_FASTCALL ReceivedFullDetachBit(tU8* pParam) {
#ifndef CARPOCALYPSE2_MATCHING
    tCar_spec* car;
    tCar_crush_spec* crush;
    register tPhysics_object* object;
    br_actor* actor;
    tFind_crush_actor_arg find_arg;
    br_vector3 local_3c;
    br_vector3 local_30;
    br_vector3 local_24;
    br_bounds3 local_18;
    int local_48;
    register int count;
    tU32* pPhysics_object;

    if (gCount_net_crush_full_detach_bit_list == CARPOCALYPSE2_ASIZE(gNet_crush_full_detach_bit_list)) {
        return;
    }
    car = NetCarFromPlayerID(*(tPlayer_ID*)(pParam + 4));
    if (car == NULL) {
        return;
    }
    crush = car->car_crush_spec;
    if (crush != NULL) {
        int i;

        count = crush->field_0x270;
        i = 0;
        if (count > 0) {
            pPhysics_object = (tU32*)&crush->field_0x274[0].field_0x8;
            while (i < count) {
                object = (tPhysics_object*)*pPhysics_object;
                if (object->uid == pParam[8]) {
                    goto found_crush_bit;
                }
                i++;
                pPhysics_object += 5;
            }
        }
    }
    object = NULL;
found_crush_bit:
    if (object == NULL) {
        find_arg.id = pParam[8];
        find_arg.actor = NULL;
        actor = DRActorEnumRecurse(car->car_model_actor, FindActorByCrushEntryID, &find_arg)
            ? find_arg.actor
            : NULL;
        if (actor == NULL) {
            return;
        }
        ExpandVector3(&local_24, (tCompressed_vector3*)(pParam + 0x10), -10.f, 10.f);
        ExpandVector3(&local_30, (tCompressed_vector3*)(pParam + 0x16), -10.f, 10.f);
        ExpandVector3(&local_3c, (tCompressed_vector3*)(pParam + 0x1c), -10.f, 10.f);
        ExpandVector3(&local_18.min, (tCompressed_vector3*)(pParam + 0x22), -10.f, 10.f);
        ExpandVector3(&local_18.max, (tCompressed_vector3*)(pParam + 0x28), -10.f, 10.f);
        object = SemiDetachBit(car, actor, 0.f, &local_48, &local_24, &local_18, &local_30, &local_3c);
        if (object == NULL) {
            return;
        }
    }
    {
        int list_count;
        int i;

        list_count = gCount_net_crush_full_detach_bit_list;
        for (i = 0; i < list_count; i++) {
            if (gNet_crush_full_detach_bit_list[i].object == object) {
                return;
            }
        }
        gNet_crush_full_detach_bit_list[list_count].car = car;
        gNet_crush_full_detach_bit_list[list_count].object = object;
        *(tU32*)gNet_crush_full_detach_bit_list[list_count].field_0x8 = *(tU32*)(pParam + 0xc);
        gCount_net_crush_full_detach_bit_list++;
    }
#else
    __asm {
            mov eax, dword ptr [gCount_net_crush_full_detach_bit_list]
            sub esp, 0x48
            cmp eax, 8
            push ebx
            push ebp
            push esi
            push edi
            mov ebp, ecx
            je L0043cd44
            mov ecx, dword ptr [ebp + 4]
            call NetCarFromPlayerID
            mov ebx, eax
            test ebx, ebx
            je L0043cd44
            mov eax, dword ptr [ebx + 0x18d4]
            test eax, eax
            je L0043cc2d
            mov edi, dword ptr [eax + 0x270]
            xor ecx, ecx
            test edi, edi
            jle L0043cc2d
            mov dl, byte ptr [ebp + 8]
            add eax, 0x27c
    L0043cc17:
            mov esi, dword ptr [eax]
            cmp byte ptr [esi + 0x190], dl
            je L0043cd4c
            inc ecx
            add eax, 0x14
            cmp ecx, edi
            jl L0043cc17
    L0043cc2d:
            xor eax, eax
    L0043cc2f:
            test eax, eax
            jne L0043cd05
            mov al, byte ptr [ebp + 8]
            lea ecx, [esp + 0x14]
            push ecx
            mov ecx, dword ptr [ebx + 0xe0c]
            mov edx, offset FindActorByCrushEntryID
            mov byte ptr [esp + 0x18], al
            mov dword ptr [esp + 0x1c], 0
            call DRActorEnumRecurse
            mov esi, eax
            mov ecx, dword ptr [esp + 0x18]
            neg esi
            sbb esi, esi
            and esi, ecx
            test esi, esi
            je L0043cd44
            push 0x41200000
            push 0xc1200000
            lea edx, [ebp + 0x10]
            lea ecx, [esp + 0x3c]
            call ExpandVector3
            lea edx, [ebp + 0x16]
            lea ecx, [esp + 0x28]
            push 0x41200000
            push 0xc1200000
            call ExpandVector3
            lea edx, [ebp + 0x1c]
            lea ecx, [esp + 0x1c]
            push 0x41200000
            push 0xc1200000
            call ExpandVector3
            lea edx, [ebp + 0x22]
            lea ecx, [esp + 0x40]
            push 0x41200000
            push 0xc1200000
            call ExpandVector3
            lea edx, [ebp + 0x28]
            lea ecx, [esp + 0x4c]
            push 0x41200000
            push 0xc1200000
            call ExpandVector3
            lea edx, [esp + 0x1c]
            lea eax, [esp + 0x28]
            push edx
            lea ecx, [esp + 0x44]
            push eax
            lea edx, [esp + 0x3c]
            push ecx
            lea eax, [esp + 0x1c]
            push edx
            push eax
            push 0
            mov edx, esi
            mov ecx, ebx
            call SemiDetachBit
            test eax, eax
            je L0043cd44
    L0043cd05:
            mov edx, dword ptr [gCount_net_crush_full_detach_bit_list]
            xor ecx, ecx
            test edx, edx
            jle L0043cd22
            mov edi, offset gNet_crush_full_detach_bit_list+0x4
    L0043cd16:
            cmp dword ptr [edi], eax
            je L0043cd44
            inc ecx
            add edi, 0xc
            cmp ecx, edx
            jl L0043cd16
    L0043cd22:
            lea ecx, [edx + edx*2]
            shl ecx, 2
            inc edx
            mov dword ptr [ecx + gNet_crush_full_detach_bit_list], ebx
            mov dword ptr [ecx + gNet_crush_full_detach_bit_list+0x4], eax
            mov eax, dword ptr [ebp + 0xc]
            mov dword ptr [gCount_net_crush_full_detach_bit_list], edx
            mov dword ptr [ecx + gNet_crush_full_detach_bit_list+0x8], eax
    L0043cd44:
            pop edi
            pop esi
            pop ebp
            pop ebx
            add esp, 0x48
            ret
    L0043cd4c:
            mov eax, esi
            jmp L0043cc2f
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
