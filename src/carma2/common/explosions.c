#include "explosions.h"

#include "animation.h"
#include "globvars.h"
#include "graphics.h"
#include "loading.h"
#include "utility.h"
#include "world.h"

#include "brender/brender.h"

#include "c2_hooks.h"
#include "c2_string.h"

#include "carpocalypse2_macros.h"
// GLOBAL: CARMA2_HW 0x006a55c8
extern tExplosion gExplosions[50];
// GLOBAL: CARMA2_HW 0x006a6d40
tSmash_explosion gSmash_explosions[20];

// FUNCTION: CARMA2_HW 0x004eaac0
void C2_HOOK_FASTCALL ResetExplosions(void) {
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
void C2_HOOK_FASTCALL MungeExplosions(void) {
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
}

// GLOBAL: CARMA2_HW 0x006a82a0
int gNext_explosion_slot;

// FUNCTION: CARMA2_HW 0x004c8920
br_actor* C2_HOOK_FASTCALL CreateExplosionBillboard(int pArg) {
    (void)pArg;
    NOT_IMPLEMENTED();
    return NULL;
}

// FUNCTION: CARMA2_HW 0x004ead00
void C2_HOOK_FASTCALL Explode(tExplosion_animation* pExplosion_animation, br_actor* pActor, br_bounds3* pBounds, br_vector3* pPos) {
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
            CreateExplosionBillboard(frametime);
        }
    }
}