#include "animation.h"

#include "errors.h"
#include "loading.h"

#include "brender/brender.h"
#include "carpocalypse2_macros.h"

#include "c2_hooks.h"
// GLOBAL: CARMA2_HW 0x0065fee8
const char* gExplosion_rotate_names[2] = {
    "norotate",
    "randomrotate",
};

// FUNCTION: CARMA2_HW 0x004ee780
void C2_HOOK_FASTCALL ReadExplosion(FILE* pF, tExplosion_animation* pAnimation) {
    int i;
    int j;
    char s[256];
    tExplosion_animation_group* group;
    float f1, f2;
    int d1, d2;

    C2_HOOK_BUG_ON(sizeof(tExplosion_animation_group) != 0x44);
    C2_HOOK_BUG_ON(sizeof(tExplosion_animation_frame) != 8);
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tExplosion_animation_group, count_frames, 0x4);
    C2_HOOK_BUG_ON(CARPOCALYPSE2_ASIZE(gExplosion_rotate_names) != 2);

    pAnimation->count_groups = GetAnInt(pF);
    if (pAnimation->count_groups == 0) {
        return;
    }
    pAnimation->groups = BrMemAllocate(pAnimation->count_groups * sizeof(tExplosion_animation_group), kMem_explosion_data);
    for (i = 0, group = pAnimation->groups; i < pAnimation->count_groups; i++, group++) {
        GetPairOfInts(pF, &d1, &d2);
        group->min_count = (tU16)d1;
        group->max_count = (tU16)d2;
        GetPairOfFloats(pF, &f1, &f2);
        group->min_start_delay_ms = (tU16)(1000.f * f1);
        group->max_start_delay_ms = (tU16)(1000.f * f2);
        GetThreeFloats(pF, &group->offset.v[0], &group->offset.v[1], &group->offset.v[2]);
        GetPairOfFloats(pF, &group->min_xyz_factor.v[0], &group->max_xyz_factor.v[0]);
        GetPairOfFloats(pF, &group->min_xyz_factor.v[1], &group->max_xyz_factor.v[1]);
        GetPairOfFloats(pF, &group->min_xyz_factor.v[2], &group->max_xyz_factor.v[2]);
        GetPairOfFloats(pF, &f1, &f2);
        group->min_frametime_ms = (tU16)(1000.f / f1);
        group->max_frametime_ms = (tU16)(1000.f / f2);
        GetPairOfFloats(pF, &group->min_scaling_factor, &group->max_scaling_factor);
        group->rotate_modus = GetALineAndInterpretCommand(pF, gExplosion_rotate_names, CARPOCALYPSE2_ASIZE(gExplosion_rotate_names));
        group->count_frames = (tS16)GetAnInt(pF);
        group->frames = BrMemAllocate(group->count_frames * sizeof(tExplosion_animation_frame), kMem_explosion_data);
        for (j = 0; j < group->count_frames; j++) {
            group->frames[j].opacity = GetAScalar(pF);
            group->frames[j].opacity = 100.f;
            GetAString(pF, s);
            group->frames[j].map = BrMapFind(s);
            if (group->frames[j].map == NULL) {
                FatalError(kFatalError_CantLoadSmashPix_S, s);
            }
        }
    }
}

// FUNCTION: CARMA2_HW 0x004ee950
void C2_HOOK_FASTCALL FreeExplosionAnimation(tExplosion_animation* pAnimation) {
    int i;
    tExplosion_animation_group* group;

    if (pAnimation->count_groups != 0) {
        i = 0;
        group = pAnimation->groups;
        if (pAnimation->count_groups > 0) {
            do {
                BrMemFree(group->frames);
                i++;
                group++;
            } while (i < pAnimation->count_groups);
        }
        BrMemFree(pAnimation->groups);
    }
}