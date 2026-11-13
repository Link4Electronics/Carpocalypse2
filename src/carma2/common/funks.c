#include "funks.h"

#include "carpocalypse2_macros.h"
#include "carpocalypse2_types.h"
#include "c2_string.h"
#include "flicplay.h"
#include "utility.h"
#include "world.h"
// GLOBAL: CARMA2_HW 0x0068b840
br_scalar gSight_distance_squared;

// CalculateTextureBitsValue

// MapSawToTriangle
#ifdef CARPOCALYPSE2_MATCHING
#endif
#ifdef CARPOCALYPSE2_MATCHING
extern void C2_HOOK_CDECL BrActorFree();
extern br_actor* C2_HOOK_CDECL BrActorRemove();
extern void C2_HOOK_CDECL BrMemFree();
extern void C2_HOOK_CDECL BrPixelmapFree();
extern int C2_HOOK_FASTCALL EndFlic(tFlic_descriptor_ptr pFlic_info);
extern void C2_HOOK_FASTCALL PossibleService(void);
extern tFunkotronic_spec* gFunkotronics_array;
extern int gFunkotronics_array_size;
#endif
// FUNCTION: CARMA2_HW 0x00474880

void C2_HOOK_STDCALL SetSightDistance(br_scalar pYon) {

    gSight_distance_squared = (br_scalar)CARPOCALYPSE2_SQR(pYon * 1.02);
}

// AddFunkGrooveBinding

// ControlBoundFunkGroove

// ControlBoundFunkGroovePlus

// ShiftBoundGrooveFunks

// AddNewFunkotronic

// FUNCTION: CARMA2_HW 0x00474950
void C2_NAKED C2_HOOK_FASTCALL DisposeFunkotronics(int pOwner) {
#ifndef CARPOCALYPSE2_MATCHING
    int i;
    tFunkotronic_spec* the_funk;

    if (gFunkotronics_array == NULL || gFunkotronics_array_size <= 0) {
        return;
    }
    the_funk = gFunkotronics_array;
    for (i = 0; i < gFunkotronics_array_size; i++, the_funk++) {
        PossibleService();
        if (the_funk->owner != pOwner) {
            continue;
        }
        the_funk->owner = -999;
        if (the_funk->proximity_array != NULL) {
            BrMemFree(the_funk->proximity_array);
        }
        if (the_funk->texture_animation_type == eTexture_animation_frames) {
            if (the_funk->texture_animation_data.frames_info.mode == eMove_texturebits) {
                BrMemFree(the_funk->texture_animation_data.frames_info.texture_info.data);
            }
        }
        if (the_funk->texture_animation_type == eTexture_animation_flic) {
            BrMemFree(the_funk->texture_animation_data.flic_info.flic_data);
            EndFlic(&the_funk->texture_animation_data.flic_info.flic_descriptor);
            BrMemFree(the_funk->material->colour_map->pixels);
            the_funk->material->colour_map->pixels = NULL;
            BrPixelmapFree(the_funk->material->colour_map);
            the_funk->material->colour_map = NULL;
        } else if (the_funk->texture_animation_type == eTexture_animation_camera) {
            int j;

            for (j = 0; j < the_funk->texture_animation_data.camera_info.count; j++) {
                BrActorRemove(the_funk->texture_animation_data.camera_info.actors[j]);
                BrActorFree(the_funk->texture_animation_data.camera_info.actors[j]);
            }
        } else if (the_funk->texture_animation_type == eTexture_animation_mirror) {
            if (the_funk->texture_animation_data.mirror_info.actor != NULL) {
                BrActorRemove(the_funk->texture_animation_data.mirror_info.actor);
                BrActorFree(the_funk->texture_animation_data.mirror_info.actor);
            }
        }
    }
#else
    __asm {
            push ecx
            mov eax, dword ptr [gFunkotronics_array]
            push ebx
            push ebp
            xor ebp, ebp
            cmp eax, ebp
            mov dword ptr [esp + 8], ecx
            je L00474aad
            mov ecx, dword ptr [gFunkotronics_array_size]
            xor ebx, ebx
            cmp ecx, ebp
            jle L00474aad
            push edi
            push esi
            lea esi, [eax + 0x154]
    L0047497e:
            call PossibleService
            mov eax, dword ptr [esp + 0x10]
            mov ecx, dword ptr [esi - 0x154]
            cmp ecx, eax
            jne L00474a97
            mov eax, dword ptr [esi]
            mov dword ptr [esi - 0x154], 0xfffffc19
            cmp eax, ebp
            je L004749b0
            mov ecx, eax
            push ecx
            call BrMemFree
            add esp, 4
    L004749b0:
            cmp dword ptr [esi - 0x104], ebp
            jne L004749d0
            cmp dword ptr [esi - 0xf8], 6
            jne L004749d0
            mov edx, dword ptr [esi - 0xf4]
            push edx
            call BrMemFree
            add esp, 4
    L004749d0:
            mov eax, dword ptr [esi - 0x104]
            cmp eax, 1
            jne L00474a33
            mov eax, dword ptr [esi - 0xf8]
            push eax
            call BrMemFree
            add esp, 4
            lea ecx, [esi - 0xf0]
            call EndFlic
            mov ecx, dword ptr [esi - 0x14c]
            mov edx, dword ptr [ecx + 0x40]
            mov eax, dword ptr [edx + 8]
            push eax
            call BrMemFree
            mov ecx, dword ptr [esi - 0x14c]
            add esp, 4
            mov edx, dword ptr [ecx + 0x40]
            mov dword ptr [edx + 8], ebp
            mov eax, dword ptr [esi - 0x14c]
            mov ecx, dword ptr [eax + 0x40]
            push ecx
            call BrPixelmapFree
            mov edx, dword ptr [esi - 0x14c]
            add esp, 4
            mov dword ptr [edx + 0x40], ebp
            jmp L00474a97
    L00474a33:
            cmp eax, 2
            jne L00474a70
            mov eax, dword ptr [esi - 0xf0]
            xor ebx, ebx
            cmp eax, ebp
            jle L00474a97
            lea edi, [esi - 0xec]
    L00474a4a:
            mov eax, dword ptr [edi]
            push eax
            call BrActorRemove
            mov ecx, dword ptr [edi]
            add esp, 4
            push ecx
            call BrActorFree
            mov eax, dword ptr [esi - 0xf0]
            add esp, 4
            inc ebx
            add edi, 4
            cmp ebx, eax
            jl L00474a4a
            jmp L00474a97
    L00474a70:
            cmp eax, 3
            jne L00474a97
            mov eax, dword ptr [esi - 0xf0]
            cmp eax, ebp
            je L00474a97
            push eax
            call BrActorRemove
            mov edx, dword ptr [esi - 0xf0]
            add esp, 4
            push edx
            call BrActorFree
            add esp, 4
    L00474a97:
            mov eax, dword ptr [gFunkotronics_array_size]
            inc ebx
            add esi, 0x158
            cmp ebx, eax
            jl L0047497e
            pop esi
            pop edi
    L00474aad:
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
            nop
    }
#endif
}

// CalcProximities

// AddProximities

// TryThisFunkLink

// FindSmashableMaterial


// FUNCTION: CARMA2_HW 0x00476430
void C2_HOOK_FASTCALL DisposeGroovidelics(int pOwner) {
    tGroovidelic_spec* the_groove;
    int i;

    if (gGroovidelics_array != NULL) {
        for (i = 0, the_groove = gGroovidelics_array; i < gGroovidelics_array_size; i++, the_groove++) {
            PossibleService();
            if (the_groove->owner == pOwner) {
                the_groove->owner = -999;
            }
        }
    }
}

// AddNewGroovidelic

// AddGroovidelics

// FUNCTION: CARMA2_HW 0x00477160
void C2_HOOK_FASTCALL KillGroovadelic(int pOwner) {
    int i;
    tGroovidelic_spec* the_groove;

    if (gGroovidelics_array != NULL) {
        for (i = 0, the_groove = gGroovidelics_array; i < gGroovidelics_array_size; i++) {
            if (the_groove[i].owner == pOwner
                && the_groove[i].path_mode != eMove_controlled
                && the_groove[i].path_mode != eMove_absolute
                && the_groove[i].object_mode != eMove_controlled
                && the_groove[i].object_mode != eMove_absolute) {
                the_groove[i].owner = -999;
            }
        }
    }
}

// FUNCTION: CARMA2_HW 0x004771c0
void C2_HOOK_FASTCALL KillFunkotronic(int pOwner) {
    int i;
    tFunkotronic_spec* the_funk;

    if (gFunkotronics_array != NULL) {
        for (i = 0, the_funk = gFunkotronics_array; i < gFunkotronics_array_size; i++) {
            if (the_funk[i].owner == pOwner
                && the_funk[i].matrix_mode != eMove_controlled
                && the_funk[i].matrix_mode != eMove_absolute
                && the_funk[i].lighting_animation_type != eMove_controlled
                && the_funk[i].lighting_animation_type != eMove_absolute
                && ((the_funk[i].texture_animation_data.frames_info.mode != eMove_controlled
                        && the_funk[i].texture_animation_data.frames_info.mode != eMove_texturebits)
                    || the_funk[i].texture_animation_type != eTexture_animation_frames)) {
                the_funk[i].owner = -999;
            }
        }
    }
}

// NormaliseDegreeAngle

// FunkThoseTronics

// PathGrooveBastard

// ObjectGrooveBastard

// GrooveThisDelic

// GrooveThoseDelics

// StopGroovidelic

// DisableGroovidelic

// FUNCTION: CARMA2_HW 0x0047b130
void C2_HOOK_FASTCALL DisableGroovidelic(br_actor* pActor, tU32 pBlock_flags) {
    tGroovidelic_spec* the_groove;
    int i;

    the_groove = gGroovidelics_array;
    for (i = 0; i < gGroovidelics_array_size; i++, the_groove++) {
        if (the_groove->actor == pActor) {
            the_groove->block_flags |= pBlock_flags;
            return;
        }
    }
}

// EnableGroovidelic

// FUNCTION: CARMA2_HW 0x0047b1a0
void C2_HOOK_FASTCALL SetGrooveInterrupt(int pIndex, const br_matrix34* pMatrix, tInterrupt_status pPath_interrupt_status, tInterrupt_status pObject_interrupt_status, float pPath_resumption_value, float pObject_resumption_value) {
    tGroovidelic_spec* the_groove;
    br_actor* actor;

    the_groove = &gGroovidelics_array[pIndex];
    actor = the_groove->actor;
    the_groove->path_interrupt_status = pPath_interrupt_status;
    the_groove->object_interrupt_status = pObject_interrupt_status;
    the_groove->path_resumption_value = pPath_resumption_value;
    the_groove->object_resumption_value = pObject_resumption_value;
    BrMatrix34Copy(&actor->t.t.mat, pMatrix);
}

// ResetGrooveFlags

// DisableFunkotronic

// EnableFunkotronic

// MasterDisableFunkotronic

// MasterEnableFunkotronic

// FUNCTION: CARMA2_HW 0x0047b310
void C2_HOOK_FASTCALL AdjustFunkEnable(int pIndex, tS16 pFlags) {
    if (pFlags != 0) {
        gFunkotronics_array[pIndex].flags &= ~0x1;
        PipeSingleFunkEnable(pIndex, 1);
    } else {
        gFunkotronics_array[pIndex].flags |= 0x1;
        PipeSingleFunkEnable(pIndex, 0);
    }
}

// ActorsGroove