#include "explosions.h"

#include "animation.h"

#include "errors.h"

#include "globvars.h"
#include "graphics.h"
#include "loading.h"
#include "piping.h"

#include "smashing.h"

#include "utility.h"
#include "world.h"

#include "brender/brender.h"

#include "c2_hooks.h"
#include "c2_string.h"

#include "carpocalypse2_macros.h"

// GLOBAL: CARMA2_HW 0x006a55c8
extern tExplosion gExplosions[50];
extern const double gDouble_0058aeb8;

// GLOBAL: CARMA2_HW 0x006a6d40

// GLOBAL: CARMA2_HW 0x006a55c8
extern tExplosion gExplosions[50];
// GLOBAL: CARMA2_HW 0x006a6d40
tSmash_explosion gSmash_explosions[20];

#ifdef CARPOCALYPSE2_MATCHING
#endif
#ifdef CARPOCALYPSE2_MATCHING
#endif
#ifdef CARPOCALYPSE2_MATCHING
extern float C2_HOOK_STDCALL FRandomBetween(float pA, float pB);
extern tU32 C2_HOOK_FASTCALL GetTotalTime(void);
extern int C2_HOOK_FASTCALL IRandomBetween(int pA, int pB);
extern int C2_HOOK_FASTCALL PercentageChance(int pC);
extern br_uint_8 bl;
extern tSmashable_item_spec gPowerup_respawn_smashable;
#endif
#ifdef CARPOCALYPSE2_MATCHING
extern br_actor* C2_HOOK_CDECL BrActorAdd();
extern br_actor* C2_HOOK_CDECL BrActorRemove();
extern void C2_HOOK_CDECL BrMaterialUpdate();
extern void C2_HOOK_CDECL BrMatrix34ApplyV();
extern void C2_HOOK_CDECL BrMatrix34PreRotateZ();
extern void C2_HOOK_CDECL BrMatrix34PreScale();
extern int C2_HOOK_FASTCALL PointOutOfSight(const br_vector3* pPoint, undefined4 pArg2, br_scalar pMax_distance);
extern br_actor* gCamera;
extern char gConst_kangeroo_force_factor[];
extern br_actor* gNon_track_actor;
extern int gPed_676914;
#endif
// FUNCTION: CARMA2_HW 0x004eaac0
void C2_NAKED C2_HOOK_FASTCALL ResetExplosions(void) {
#ifndef CARPOCALYPSE2_MATCHING
    int i;

    C2_HOOK_BUG_ON(CARPOCALYPSE2_ASIZE(gExplosions) != 50);
    C2_HOOK_BUG_ON(sizeof(gExplosions[0]) != 0x78);
    C2_HOOK_BUG_ON(CARPOCALYPSE2_ASIZE(gSmash_explosions) != 20);
    C2_HOOK_BUG_ON(sizeof(gSmash_explosions[0]) != 0xc8);

    for (i = 0; i < (int)CARPOCALYPSE2_ASIZE(gExplosions); i++) {
        gExplosions[i].start = 0;
        gExplosions[i].finished = 0;
    }
    for (i = 0; i < (int)CARPOCALYPSE2_ASIZE(gSmash_explosions); i++) {
        gSmash_explosions[i].active = 0;
    }
#else
    __asm {
            mov eax, offset gExplosions+0xa
            xor ecx, ecx
    L004eaac7:
            mov dword ptr [eax - 0xa], ecx
            mov byte ptr [eax], cl
            add eax, 0x78
            cmp eax, offset gSmash_explosions+0x2
            jl L004eaac7
            mov eax, offset gSmash_explosions+0x4
    L004eaadb:
            mov dword ptr [eax], ecx
            add eax, 0xc8
            cmp eax, offset gPowerup_respawn_smashable+0x4
            jl L004eaadb
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

// FUNCTION: CARMA2_HW 0x004f5ec0
void C2_HOOK_FASTCALL ReadExplosionInfo(FILE* pF, int* pChance_explosion, int* pExplosion_sound_id, tExplosion_animation* pExplosion_groups) {
    tPath_name the_path;
    char* name;
    char s[256];

    /* Wasted explosion settings */

    /* Percentage chance of car exploding */
    *pChance_explosion = GetAnInt(pF);
    /* Sound ID */
    *pExplosion_sound_id = GetAnInt(pF);

    PathCat(the_path, gApplication_path, "COMMON");
    /* Name of pixelmap file */
    GetALineAndDontArgue(pF, s);
    name = strtok(s, "\t ,/");
    name[strlen(name) - 4] = '\0';
    PathCat(the_path, the_path, name);
    LoadAllImagesInDirectory(&gMisc_storage_space, the_path);
    ReadExplosion(pF, pExplosion_groups);
}

int C2_HOOK_FAKE_THISCALL PointOutOfSightNotAR(const br_vector3* pPoint, undefined4 pArg2, float pMax_distance) {

    if (gAction_replay_mode) {
        return 1;
    }
    return PointOutOfSight(pPoint, pArg2, pMax_distance);
}

// FUNCTION: CARMA2_HW 0x004eaaf0
void C2_NAKED C2_HOOK_FASTCALL MungeExplosions(void) {
#ifndef CARPOCALYPSE2_MATCHING
    int i;
    tU32 now;

    now = GetTotalTime();
    for (i = 0; i < CARPOCALYPSE2_ASIZE(gExplosions); i++) {
        tExplosion* explosion = &gExplosions[i];
        int frame;
        undefined prev_frame;
        br_vector3 tv;

        if (explosion->start == 0) {
            continue;
        }
        frame = (now - explosion->start) / explosion->period;
        prev_frame = explosion->field_0x8;
        if (now < explosion->start) {
            if (explosion->actor->parent != NULL) {
                BrActorRemove(explosion->actor);
            }
            continue;
        }
        if (frame >= explosion->count_frames) {
            if (explosion->actor->parent != NULL) {
                BrActorRemove(explosion->actor);
            }
            explosion->finished = 1;
            continue;
        }
        if (PointOutOfSightNotAR(&explosion->collision_actor->t.t.translate.t CARPOCALYPSE2_THISCALL_EDX, 0.f)) {
            if (explosion->actor->parent != NULL) {
                BrActorRemove(explosion->actor);
            }
            continue;
        }
        explosion->field_0x8 = frame;
        if (explosion->actor->parent == NULL) {
            BrActorAdd(gNon_track_actor, explosion->actor);
        }
        if (frame != prev_frame) {
            explosion->actor->material->colour_map = explosion->frames[frame].map;
            BrMaterialUpdate(explosion->actor->material, BR_MATU_ALL);
        }
        if (explosion->actor->material->colour_map == NULL) {
            if (explosion->actor->parent != NULL) {
                BrActorRemove(explosion->actor);
            }
            continue;
        }
        explosion->actor->t.t.mat = gCamera->t.t.mat;
        BrMatrix34ApplyV(&tv, &explosion->field_0x1c, &explosion->collision_actor->t.t.mat);
        BrVector3Accumulate(&explosion->actor->t.t.translate.t, &tv);
        BrMatrix34PreScale(&explosion->actor->t.t.mat,
            (float)explosion->actor->material->colour_map->width * explosion->field_0x14 / 128.f,
            (float)explosion->actor->material->colour_map->height * explosion->field_0x14 / 128.f,
            1.f);
        BrMatrix34PreRotateZ(&explosion->actor->t.t.mat, explosion->angle);
    }
#else
    __asm {
            sub esp, 0x18
            push ebx
            push ebp
            push esi
            push edi
            call GetTotalTime
            mov edi, eax
            mov ebp, offset gExplosions+0x10
            mov dword ptr [esp + 0x18], edi
            mov dword ptr [esp + 0x10], 0x32
    L004eab0f:
            mov ecx, dword ptr [ebp - 0x10]
            test ecx, ecx
            je L004eacb9
            mov eax, edi
            sub eax, ecx
            cdq
            idiv dword ptr [ebp - 0xc]
            movsx esi, byte ptr [ebp - 8]
            cmp edi, ecx
            mov ebx, eax
            jge L004eab3f
            mov eax, dword ptr [ebp]
            mov ecx, dword ptr [eax + 0xc]
            test ecx, ecx
            je L004eacb9
            jmp L004eacb0
    L004eab3f:
            movsx eax, byte ptr [ebp - 7]
            cmp ebx, eax
            jl L004eab63
            mov eax, dword ptr [ebp]
            mov ecx, dword ptr [eax + 0xc]
            test ecx, ecx
            je L004eab5a
            push eax
            call BrActorRemove
            add esp, 4
    L004eab5a:
            mov byte ptr [ebp - 6], 1
            jmp L004eacb9
    L004eab63:
            mov ecx, dword ptr [ebp - 4]
            mov eax, dword ptr [gPed_676914]
            add ecx, 0x50
            test eax, eax
            jne L004eab81
            push 0
            call PointOutOfSight
            test eax, eax
            jne L004eaca6
    L004eab81:
            mov ecx, dword ptr [ebp]
            mov byte ptr [ebp - 8], bl
            mov eax, dword ptr [ecx + 0xc]
            test eax, eax
            jne L004eab9f
            mov eax, dword ptr [gNon_track_actor]
            mov edx, ecx
            push edx
            push eax
            call BrActorAdd
            add esp, 8
    L004eab9f:
            cmp ebx, esi
            je L004eabc4
            mov ecx, dword ptr [ebp]
            mov eax, dword ptr [ebp + ebx*8 + 0x1c]
            push 0x7fff
            mov edx, dword ptr [ecx + 0x1c]
            mov dword ptr [edx + 0x40], eax
            mov ecx, dword ptr [ebp]
            mov edx, dword ptr [ecx + 0x1c]
            push edx
            call BrMaterialUpdate
            add esp, 8
    L004eabc4:
            mov eax, dword ptr [ebp]
            mov ecx, dword ptr [eax + 0x1c]
            mov edx, dword ptr [ecx + 0x40]
            test edx, edx
            jne L004eabe1
            mov ecx, dword ptr [eax + 0xc]
            test ecx, ecx
            je L004eacb9
            jmp L004eacb0
    L004eabe1:
            mov edx, dword ptr [gCamera]
            lea edi, [eax + 0x2c]
            mov ecx, 0xc
            lea esi, [edx + 0x2c]
            lea edx, [esp + 0x1c]
            rep movsd
            mov eax, dword ptr [ebp - 4]
            lea ecx, [ebp + 0xc]
            add eax, 0x2c
            push eax
            push ecx
            push edx
            call BrMatrix34ApplyV
            mov eax, dword ptr [ebp - 4]
            mov ecx, dword ptr [ebp]
            add esp, 0xc
            fld dword ptr [eax + 0x50]
            fadd dword ptr [esp + 0x1c]
            push 0x3f800000
            fstp dword ptr [ecx + 0x50]
            mov edx, dword ptr [ebp - 4]
            mov eax, dword ptr [ebp]
            fld dword ptr [edx + 0x54]
            fadd dword ptr [esp + 0x24]
            fstp dword ptr [eax + 0x54]
            mov ecx, dword ptr [ebp - 4]
            mov edx, dword ptr [ebp]
            fld dword ptr [ecx + 0x58]
            fadd dword ptr [esp + 0x28]
            fstp dword ptr [edx + 0x58]
            mov eax, dword ptr [ebp]
            xor edx, edx
            mov ecx, dword ptr [eax + 0x1c]
            add eax, 0x2c
            mov ecx, dword ptr [ecx + 0x40]
            push ecx
            mov dx, word ptr [ecx + 0x36]
            mov dword ptr [esp + 0x1c], edx
            xor edx, edx
            fild dword ptr [esp + 0x1c]
            mov dx, word ptr [ecx + 0x34]
            mov dword ptr [esp + 0x1c], edx
            fmul dword ptr [ebp + 4]
            fmul dword ptr [gConst_kangeroo_force_factor+0x220]
            _emit 0xd9
        _emit 0x1c
        _emit 0x24
            fild dword ptr [esp + 0x1c]
            push ecx
            fmul dword ptr [ebp + 4]
            fmul dword ptr [gConst_kangeroo_force_factor+0x220]
            _emit 0xd9
        _emit 0x1c
        _emit 0x24
            push eax
            call BrMatrix34PreScale
            mov ecx, dword ptr [ebp]
            mov ax, word ptr [ebp + 8]
            add esp, 0x10
            add ecx, 0x2c
            push eax
            push ecx
            call BrMatrix34PreRotateZ
            mov edi, dword ptr [esp + 0x20]
            add esp, 8
            jmp L004eacb9
    L004eaca6:
            mov eax, dword ptr [ebp]
            mov ecx, dword ptr [eax + 0xc]
            test ecx, ecx
            je L004eacb9
    L004eacb0:
            push eax
            call BrActorRemove
            add esp, 4
    L004eacb9:
            mov eax, dword ptr [esp + 0x10]
            add ebp, 0x78
            dec eax
            mov dword ptr [esp + 0x10], eax
            jne L004eab0f
            pop edi
            pop esi
            pop ebp
            pop ebx
            add esp, 0x18
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

// GLOBAL: CARMA2_HW 0x006a82a0
int gNext_explosion_slot;

// FUNCTION: CARMA2_HW 0x004c8920
void C2_HOOK_FASTCALL PipeSingleExplosion(int pIndex, const tExplosion* pData) {
    ARDoSingleVariedSession(ePipe_chunk_explosion, pIndex, 1, sizeof(tExplosion), 0, pData);
}

// FUNCTION: CARMA2_HW 0x004c89f0
void C2_HOOK_FASTCALL FUN_004c89f0(uintptr_t pOwner) {
    return ARDoSingleSession(0x23, pOwner, &gZero_vector__smash, sizeof(gZero_vector__smash.v[0]));
}

// FUNCTION: CARMA2_HW 0x004ead00
void C2_NAKED C2_HOOK_FASTCALL Explode(tExplosion_animation* pExplosion_animation, br_actor* pActor, br_bounds3* pBounds, br_vector3* pPos) {
#ifndef CARPOCALYPSE2_MATCHING
    int i;
    int j;
    int k;
    int count;
    tU32 now;
    tExplosion_animation_group* group;
    float sx;
    float sy;
    float sz;
    float rx;
    float ry;
    float rz;
    float px;
    float py;
    float pz;
    float scaling;
    int frametime;
    int start_delay;
    int angle;
    tExplosion* slot;
    int slot_idx;
    int slot_index;

    now = GetTotalTime();
    if (pActor == NULL) {
        return;
    }
    for (i = 0; i < pExplosion_animation->count_groups; i++) {
        group = &pExplosion_animation->groups[i];
        count = IRandomBetween(group->min_count, group->max_count);
        sx = FRandomBetween(group->min_xyz_factor.v[0] * pBounds->min.v[0], group->max_xyz_factor.v[0] * pBounds->max.v[0]);
        sy = FRandomBetween(group->min_xyz_factor.v[1] * pBounds->min.v[1], group->max_xyz_factor.v[1] * pBounds->max.v[1]);
        sz = FRandomBetween(group->min_xyz_factor.v[2] * pBounds->min.v[2], group->max_xyz_factor.v[2] * pBounds->max.v[2]);
        if (count <= 0) {
            continue;
        }
        for (j = 0; j < count; j++) {
            rx = FRandomBetween(sx, sx);
            if (!PercentageChance(50)) {
                rx = -rx;
            }
            ry = FRandomBetween(sy, sy);
            if (!PercentageChance(50)) {
                ry = -ry;
            }
            rz = FRandomBetween(sz, sz);
            if (!PercentageChance(50)) {
                rz = -rz;
            }
            if (pPos != NULL) {
                px = rx + pPos->v[0];
                py = ry + pPos->v[1];
                pz = rz + pPos->v[2];
            } else {
                px = rx;
                py = ry;
                pz = rz;
            }
            px += group->offset.v[0];
            py += group->offset.v[1];
            pz += group->offset.v[2];
            if (group->rotate_modus != kExplosionRotateModus_NoRotate) {
                angle = IRandomBetween(0, 0xffff);
            } else {
                angle = 0;
            }
            scaling = FRandomBetween(group->min_scaling_factor, group->max_scaling_factor);
            frametime = IRandomBetween(group->min_frametime_ms, group->max_frametime_ms);
            start_delay = IRandomBetween(group->min_start_delay_ms, group->max_start_delay_ms);
            slot = NULL;
            for (k = 0; k < (int)CARPOCALYPSE2_ASIZE(gExplosions); k++) {
                if (gExplosions[k].start == 0) {
                    slot = &gExplosions[k];
                    slot_index = k;
                    slot_idx = 1;
                    break;
                }
            }
            if (slot == NULL) {
                for (k = 0; k < (int)CARPOCALYPSE2_ASIZE(gExplosions); k++) {
                    gNext_explosion_slot++;
                    if (gNext_explosion_slot >= (int)CARPOCALYPSE2_ASIZE(gExplosions)) {
                        gNext_explosion_slot = 0;
                    }
                    slot = &gExplosions[gNext_explosion_slot];
                    if (slot->finished == 0) {
                        slot_index = gNext_explosion_slot;
                        break;
                    }
                }
                if (k >= (int)CARPOCALYPSE2_ASIZE(gExplosions)) {
                    continue;
                }
            }
            slot->start = now + start_delay;
            slot->period = frametime;
            slot->count_frames = group->count_frames;
            slot->finished = 0;
            slot->collision_actor = pActor;
            slot->field_0x14 = scaling * (1.0 / WORLD_SCALE);
            slot->field_0x8 = 0xff;
            slot->angle = angle;
            slot->field_0x1c.v[0] = px;
            slot->field_0x1c.v[1] = py;
            slot->field_0x1c.v[2] = pz;
            for (k = 0; k < group->count_frames; k++) {
                slot->frames[k].map = group->frames[k].map;
                slot->frames[k].opacity = group->frames[k].opacity;
            }
            PipeSingleExplosion(slot_index, slot);
        }
    }
#else
    __asm {
            sub esp, 0x4c
            push ebx
            push ebp
            push esi
            mov esi, edx
            mov ebx, ecx
            push edi
            mov dword ptr [esp + 0x3c], esi
            mov dword ptr [esp + 0x4c], ebx
            call GetTotalTime
            test esi, esi
            mov dword ptr [esp + 0x38], eax
            je L004eb012
            mov eax, dword ptr [ebx]
            mov ebp, dword ptr [ebx + 4]
            test eax, eax
            mov dword ptr [esp + 0x1c], 0
            jle L004eb012
            mov esi, dword ptr [esp + 0x60]
            mov edi, dword ptr [esp + 0x64]
    L004ead41:
            movsx edx, word ptr [ebp + 2]
            movsx ecx, word ptr [ebp]
            call IRandomBetween
            fld dword ptr [ebp + 0x18]
            fld dword ptr [ebp + 0x24]
            fld dword ptr [ebp + 0x1c]
            fld dword ptr [ebp + 0x28]
            fld dword ptr [ebp + 0x20]
            fld dword ptr [ebp + 0x2c]
            fxch st(5)
            fmul dword ptr [esi + 0xc]
            test eax, eax
            mov dword ptr [esp + 0x48], eax
            mov dword ptr [esp + 0x18], 0
            fstp dword ptr [esp + 0x24]
            fxch st(3)
            fmul dword ptr [esi + 0xc]
            fxch st(2)
            fmul dword ptr [esi + 0x10]
            fxch st(1)
            fmul dword ptr [esi + 0x10]
            fxch st(2)
            fstp dword ptr [esp + 0x20]
            fxch st(2)
            fmul dword ptr [esi + 0x14]
            fxch st(2)
            fstp dword ptr [esp + 0x2c]
            fxch st(2)
            fmul dword ptr [esi + 0x14]
            fxch st(2)
            fstp dword ptr [esp + 0x28]
            fstp dword ptr [esp + 0x34]
            fstp dword ptr [esp + 0x30]
            jle L004eaffc
            jmp L004eadb6
    L004eadb2:
            mov edi, dword ptr [esp + 0x64]
    L004eadb6:
            mov eax, dword ptr [esp + 0x20]
            mov ecx, dword ptr [esp + 0x24]
            push eax
            push ecx
            call FRandomBetween
            mov ecx, 0x32
            fstp dword ptr [esp + 0x10]
            call PercentageChance
            neg eax
            sbb eax, eax
            mov edx, dword ptr [esp + 0x28]
            and eax, 2
            push edx
            dec eax
            mov dword ptr [esp + 0x18], eax
            mov eax, dword ptr [esp + 0x30]
            fild dword ptr [esp + 0x18]
            push eax
            fmul dword ptr [esp + 0x18]
            fstp dword ptr [esp + 0x58]
            call FRandomBetween
            mov ecx, 0x32
            fstp dword ptr [esp + 0x10]
            call PercentageChance
            neg eax
            sbb eax, eax
            mov ecx, dword ptr [esp + 0x30]
            and eax, 2
            mov edx, dword ptr [esp + 0x34]
            dec eax
            push ecx
            mov dword ptr [esp + 0x18], eax
            push edx
            fild dword ptr [esp + 0x1c]
            fmul dword ptr [esp + 0x18]
            fstp dword ptr [esp + 0x5c]
            call FRandomBetween
            mov ecx, 0x32
            fstp dword ptr [esp + 0x10]
            call PercentageChance
            neg eax
            sbb eax, eax
            and eax, 2
            dec eax
            mov dword ptr [esp + 0x14], eax
            fild dword ptr [esp + 0x14]
            test edi, edi
            fmul dword ptr [esp + 0x10]
            je L004eae75
            fld dword ptr [edi]
            fld dword ptr [edi + 4]
            fld dword ptr [edi + 8]
            fxch st(2)
            fadd dword ptr [esp + 0x50]
            fxch st(1)
            fadd dword ptr [esp + 0x54]
            fxch st(2)
            faddp st(3), st(0)
            fxch st(1)
            fstp dword ptr [esp + 0x54]
            jmp L004eae79
    L004eae75:
            fld dword ptr [esp + 0x50]
    L004eae79:
            fld dword ptr [esp + 0x54]
            fxch st(1)
            fadd dword ptr [ebp + 0x30]
            fxch st(1)
            fadd dword ptr [ebp + 0x34]
            fxch st(2)
            fadd dword ptr [ebp + 0x38]
            fxch st(1)
            fstp dword ptr [esp + 0x50]
            mov eax, dword ptr [ebp + 0x3c]
            fxch st(1)
            fstp dword ptr [esp + 0x54]
            test eax, eax
            fstp dword ptr [esp + 0x58]
            je L004eaeb5
            mov edx, 0xffff
            xor ecx, ecx
            call IRandomBetween
            mov dword ptr [esp + 0x14], eax
            jmp L004eaebd
    L004eaeb5:
            mov dword ptr [esp + 0x14], 0
    L004eaebd:
            mov eax, dword ptr [ebp + 0x14]
            mov ecx, dword ptr [ebp + 0x10]
            mov esi, dword ptr [ebp + 0x40]
            push eax
            movsx ebx, word ptr [ebp + 4]
            push ecx
            call FRandomBetween
            movsx edx, word ptr [ebp + 0xc]
            fstp dword ptr [esp + 0x44]
            movsx ecx, word ptr [ebp + 0xa]
            call IRandomBetween
            movsx edx, word ptr [ebp + 8]
            movsx ecx, word ptr [ebp + 6]
            mov dword ptr [esp + 0x40], eax
            call IRandomBetween
            mov ecx, eax
            mov eax, dword ptr [esp + 0x38]
            add ecx, eax
            mov dword ptr [esp + 0x10], 0
            xor eax, eax
            mov edx, offset gExplosions
    L004eaf0a:
            cmp dword ptr [edx], 0
            je L004eaf1a
            inc eax
            add edx, 0x78
            cmp eax, 0x32
            jl L004eaf0a
            jmp L004eaf22
    L004eaf1a:
            mov dword ptr [esp + 0x10], 1
    L004eaf22:
            cmp eax, 0x32
            jne L004eaf67
            mov edi, dword ptr [gNext_explosion_slot]
            mov eax, edi
            lea edx, [edi + edi*2]
            lea edx, [edx + edx*4]
            lea edx, [edx*8 + gExplosions]
    L004eaf3c:
            inc eax
            cmp eax, 0x32
            jl L004eaf4b
            xor eax, eax
            mov edx, offset gExplosions
            jmp L004eaf4e
    L004eaf4b:
            add edx, 0x78
    L004eaf4e:
            cmp byte ptr [edx + 0xa], 0
            jne L004eaf5a
            cmp eax, edi
            je L004eaf67
            jmp L004eaf3c
    L004eaf5a:
            mov dword ptr [gNext_explosion_slot], eax
            mov dword ptr [esp + 0x10], 1
    L004eaf67:
            mov edi, dword ptr [esp + 0x10]
            test edi, edi
            je L004eb012
            fld dword ptr [esp + 0x44]
            fmul qword ptr [gDouble_0058aeb8]
            mov edi, dword ptr [esp + 0x3c]
            mov dword ptr [edx], ecx
            mov ecx, dword ptr [esp + 0x40]
            mov byte ptr [edx + 9], bl
            mov dword ptr [edx + 4], ecx
            lea ecx, [ebx*8]
            fstp dword ptr [edx + 0x14]
            mov ebx, ecx
            mov dword ptr [edx + 0xc], edi
            lea edi, [edx + 0x28]
            mov byte ptr [edx + 0xa], 0
            shr ecx, 2
            mov byte ptr [edx + 8], 0xff
            rep movsd
            mov ecx, ebx
            and ecx, 3
            rep movsb
            mov esi, dword ptr [esp + 0x50]
            lea ecx, [edx + 0x1c]
            mov dword ptr [edx + 0x1c], esi
            mov esi, dword ptr [esp + 0x54]
            mov dword ptr [ecx + 4], esi
            mov esi, dword ptr [esp + 0x58]
            mov dword ptr [ecx + 8], esi
            mov cx, word ptr [esp + 0x14]
            mov word ptr [edx + 0x18], cx
            mov ecx, eax
            call PipeSingleExplosion
            mov eax, dword ptr [esp + 0x18]
            mov ecx, dword ptr [esp + 0x48]
            inc eax
            cmp eax, ecx
            mov dword ptr [esp + 0x18], eax
            jl L004eadb2
            mov ebx, dword ptr [esp + 0x4c]
            mov esi, dword ptr [esp + 0x60]
            mov edi, dword ptr [esp + 0x64]
    L004eaffc:
            mov eax, dword ptr [esp + 0x1c]
            mov ecx, dword ptr [ebx]
            inc eax
            add ebp, 0x44
            cmp eax, ecx
            mov dword ptr [esp + 0x1c], eax
            jl L004ead41
    L004eb012:
            pop edi
            pop esi
            pop ebp
            pop ebx
            add esp, 0x4c
            ret 8
            nop
            nop
            nop
            nop
    }
#endif
}
// FUNCTION: CARMA2_HW 0x004f5fa0
void C2_HOOK_FASTCALL AddFacesToModel(br_model* pModel, int pFace_count, const tU16* pFace_data, br_material* pMaterial) {
    br_face* face;
    const tU16* face_data;
    int i;

    face = &pModel->faces[pModel->nfaces];
    if (pFace_count > 0) {
        face_data = pFace_data;
        i = pFace_count;
        do {
            face->material = pMaterial;
            face->vertices[0] = face_data[0];
            face->vertices[1] = face_data[1];
            face->vertices[2] = face_data[2];
            face->smoothing = face_data[3];
            face->flags = 0;
            face++;
            face_data += 4;
            i--;
        } while (i != 0);
    }
    pModel->nfaces += (br_int_16)pFace_count;
    DRModelUpdateAndKevificateMaterials(pModel, BR_MODU_ALL);
}
