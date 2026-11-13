#include "frontend_wrecks.h"

#include "frontend.h"
#include "frontend_loadgame.h"
#include "frontend_main.h"
#include "frontend_quit.h"
#include "globvars.h"
#include "graphics.h"
#include "input.h"
#include "intrface.h"
#include "opponent.h"
#include "platform.h"
#include "raycast.h"
#include "sound.h"
#include "utility.h"

#include "carpocalypse2_macros.h"

#include "c2_string.h"
// GLOBAL: CARMA2_HW 0x00763720
tWreck_gallery_sell_info gWreck_gallery_sell_infos[32];

// GLOBAL: CARMA2_HW 0x00763b5c
tWreck_gallery_car_info gWreck_gallery_car_infos[30];

// GLOBAL: CARMA2_HW 0x00763844
br_actor* gFrontend_wrecks_light;

// GLOBAL: CARMA2_HW 0x00705190
int gFrontend_wrecks_car_count;

// GLOBAL: CARMA2_HW 0x00688af8
float gFrontend_wreck_bought_car_dz;

// GLOBAL: CARMA2_HW 0x00763890
int gFrontend_wrecks_current;

// GLOBAL: CARMA2_HW 0x00688afc
int gFrontend_wrecks_hscroll;

// GLOBAL: CARMA2_HW 0x00688b00
int gFrontend_wrecks_pending_hscroll;

// GLOBAL: CARMA2_HW 0x007635ec
tU32 gFrontend_wrecks_previous_update;

// GLOBAL: CARMA2_HW 0x0059b0dc
int gFrontend_wrecks_rotate_prev_x = -1;

// GLOBAL: CARMA2_HW 0x0059b0e0
int gFrontend_wrecks_rotate_prev_y = -1;

// GLOBAL: CARMA2_HW 0x00763928
int gPicked_wreck;


#ifdef CARPOCALYPSE2_MATCHING
#endif
#ifdef CARPOCALYPSE2_MATCHING
extern void C2_HOOK_CDECL BrMatrix34Post();
extern void C2_HOOK_CDECL BrMatrix34PostRotateX();
extern void C2_HOOK_CDECL BrMatrix34Pre();
extern void C2_HOOK_CDECL BrMatrix34PreTranslate();
extern void C2_HOOK_CDECL BrMatrix34RollingBall();
extern void C2_HOOK_CDECL BrMatrix34RotateY();
extern tS3_sound_tag C2_HOOK_FASTCALL DRS3StartSound(tS3_outlet* pOutlet, tS3_sound_id pSound);
extern int C2_HOOK_FASTCALL DRScenePick2DXY(br_actor* world, br_actor* camera, br_pixelmap* viewport, int pick_x, int pick_y, dr_pick2d_cbfn* callback, void* arg);
extern int C2_HOOK_FASTCALL EitherMouseButtonDown(void);
extern int C2_HOOK_FASTCALL FRONTEND_CreateMenu(tFrontend_spec* pFrontend_spec);
extern int C2_HOOK_FASTCALL FRONTEND_DestroyMenu(tFrontend_spec* pFrontend);
extern void C2_HOOK_FASTCALL FuckWithWidths(tFrontend_spec* pFrontend);
extern tCar_spec* C2_HOOK_FASTCALL GetCarSpec(tVehicle_type pCategory, int pIndex);
extern int C2_HOOK_FASTCALL GetItemAtMousePos(tFrontend_spec *pFrontend, int pX, int pY);
extern const char* C2_HOOK_FASTCALL GetMiscString(int pIndex);
extern void C2_HOOK_FASTCALL GetMousePosition(int *pX, int *pY);
extern tOpponent_spec* C2_HOOK_FASTCALL GetOpponentSpecFromCarSpec(tCar_spec* pCar_spec);
extern void C2_HOOK_FASTCALL MenuSetDriverImage(int pOpponent_index, int pFrontend_index);
extern void C2_HOOK_FASTCALL Morph_Initialise(tFrontend_spec* pCurrent, tFrontend_spec* pNext);
extern char* C2_HOOK_FASTCALL MungeCommas(int pValue);
extern void C2_HOOK_FASTCALL MungeMetaCharactersChar(char* pText, char pMeta, char pChar);
extern tU32 C2_HOOK_FASTCALL PDGetTotalTime(void);
extern int C2_HOOK_FASTCALL PDKeyDown(int pKey_index);
extern void C2_HOOK_FASTCALL ToggleSelection(tFrontend_spec* pFrontend);
extern void C2_HOOK_FASTCALL UpdateScrollPositions(tFrontend_spec* pFrontend);
extern void __cdecl _ftol();
extern int gCancel_recover;
extern char gDouble_0058a068[];
extern tS3_outlet* gEffects_outlet;
extern tFrontend_spec gFrontend_QUIT;
extern int gFrontend_leave_current_menu;
extern int gFrontend_mouse_down;
extern int gFrontend_opponent_profile_pic_needs_update;
extern int gFrontend_selected_item_index;
extern tU32 gFrontend_time_last_input;
extern br_pixelmap* gFrontend_wrecks_pixelmap;
extern int gMouse_in_use;
extern tOpponent* gOpponents;
extern int gPrev_frontend_mouse_down;
extern tProgram_state gProgram_state;
#endif
// FUNCTION: CARMA2_HW 0x0046e830
int C2_HOOK_FASTCALL WrecksInFunc(tFrontend_spec* pFrontend) {
    int car_count;
    int i;

    C2_HOOK_BUG_ON(sizeof(gWreck_gallery_sell_infos) != 0x100);
    C2_HOOK_BUG_ON(sizeof(tWreck_gallery_car_info) != 0xa4);
    C2_HOOK_BUG_ON(sizeof(tWreck_gallery_sell_info) != 0x8);

    gCurrent_frontend_spec = pFrontend;
    DRS3StartSound(gEffects_outlet, eSoundId_Swingin);
    if (!pFrontend->loaded) {
        LoadMenuSettings(pFrontend);
    }
    gFrontend_wrecks_actor = BrActorAllocate(BR_ACTOR_NONE, NULL);
    gFrontend_wrecks_camera = BrActorAllocate(BR_ACTOR_CAMERA, NULL);
    gFrontend_wrecks_light = BrActorAllocate(BR_ACTOR_LIGHT, NULL);

    ((br_camera*)gFrontend_wrecks_camera->type_data)->aspect = 1.6346154f;
    ((br_camera*)gFrontend_wrecks_camera->type_data)->field_of_view = BR_ANGLE_DEG(55);
    ((br_camera*)gFrontend_wrecks_camera->type_data)->hither_z = 1.f;
    ((br_camera*)gFrontend_wrecks_camera->type_data)->yon_z = 100.f;
    ((br_camera*)gFrontend_wrecks_camera->type_data)->type = BR_CAMERA_PERSPECTIVE_FOV;
    ((br_camera*)gFrontend_wrecks_camera->type_data)->width = 640.f;
    ((br_camera*)gFrontend_wrecks_camera->type_data)->height = 480.f;
    BrActorAdd(gFrontend_wrecks_actor, gFrontend_wrecks_camera);

    gFrontend_wrecks_pixelmap = BrPixelmapAllocateSub(gBack_screen, 65, 55, 510, 312);
    gFrontend_wrecks_pixelmap->origin_x = 255;
    gFrontend_wrecks_pixelmap->origin_y = 120;

    BrMatrix34RotateY(&gFrontend_wrecks_camera->t.t.mat, BR_ANGLE_DEG(180));
    BrMatrix34PostTranslate(&gFrontend_wrecks_camera->t.t.mat, 0.f, 0.f, -3.f);

    car_count = GetCarCount(eVehicle_opponent);
    gFrontend_wrecks_car_count = 0;

    for (i = 0; i < car_count; i++) {
        tCar_spec* car;
        br_actor* actor;

        car = GetCarSpec(eVehicle_opponent, i);
        actor = car->car_master_actor;
        actor->render_style = BR_RSTYLE_FACES;
        if (actor->parent != NULL) {
            BrActorRemove(actor);
        }
        BrActorAdd(gFrontend_wrecks_actor, actor);
        gWreck_gallery_car_infos[gFrontend_wrecks_car_count].original_matrix = actor->t.t.mat;
        BrMatrix34Identity(&actor->t.t.mat);
        BrMatrix34Identity(&gWreck_gallery_car_infos[gFrontend_wrecks_car_count].field_0x14);
        BrMatrix34Identity(&gWreck_gallery_car_infos[gFrontend_wrecks_car_count].field_0x74);
        gWreck_gallery_car_infos[gFrontend_wrecks_car_count].actor = actor;
        gFrontend_wrecks_car_count += 1;
    }
    memset(gWreck_gallery_sell_infos, 0, sizeof(gWreck_gallery_sell_infos));
    gFrontend_wreck_bought_car_dz = 0.f;

    gFrontend_selected_item_index = 0;
    gFrontend_wrecks_current = 0;
    gFrontend_wrecks_hscroll = 0;
    gFrontend_wrecks_pending_hscroll = 0;
    gFrontend_wrecks_previous_update = PDGetTotalTime();
    gFrontend_wrecks_rotate_prev_x = -1;
    gFrontend_wrecks_rotate_prev_y = -1;
    FuckWithWidths(pFrontend);
    gFrontend_opponent_profile_pic_needs_update = 1;
    return 1;
}

// FUNCTION: CARMA2_HW 0x0046e620
void C2_HOOK_FASTCALL DisposeWrecksGallery(void) {
    int i;

    for (i = 0; i < gFrontend_wrecks_car_count; i++) {
        BrActorRemove(gWreck_gallery_car_infos[i].actor);
    }
    if (gFrontend_wrecks_camera != NULL) {
        if (gFrontend_wrecks_camera->parent != NULL) {
            BrActorRemove(gFrontend_wrecks_camera);
        }
        BrActorFree(gFrontend_wrecks_camera);
    }
    if (gFrontend_wrecks_actor != NULL) {
        if (gFrontend_wrecks_actor->parent != NULL) {
            BrActorRemove(gFrontend_wrecks_actor);
        }
        BrActorFree(gFrontend_wrecks_actor);
    }
    gFrontend_wrecks_actor = NULL;
    gFrontend_wrecks_camera = NULL;
}

// FUNCTION: CARMA2_HW 0x0046ead0
int C2_HOOK_FASTCALL WrecksOutFunc(tFrontend_spec* pFrontend) {
    DisposeWrecksGallery();
    DRS3StartSound(gEffects_outlet, eSoundId_Swingout);
    return 1;
}

// FUNCTION: CARMA2_HW 0x0046f470
int C2_HOOK_FASTCALL FUN_0046f470(int* pCar_index) {
    int i;

    for (i = 0; i < gProgram_state.number_of_cars; i++) {
        if (gProgram_state.cars_available[i] == *pCar_index) {
            return 1;
        }
    }
    return 0;
}

// FUNCTION: CARMA2_HW 0x0046f4f0
int C2_HOOK_CDECL WreckPick(br_actor* world, br_model* model, br_material* material, br_vector3* pos, br_vector3* dir, br_scalar pNear, br_scalar pFar, void* arg) {
    int i;

    for (i = 0; i < gFrontend_wrecks_car_count; i++) {
        gHierarchy_has_actor = 0;
        BrActorEnum(gWreck_gallery_car_infos[i].actor, HeirarchyPick, world);
        if (gHierarchy_has_actor) {
            gPicked_wreck = i;
            return 1;
        }
    }
    return 0;
}

// FUNCTION: CARMA2_HW 0x0046eab0
int C2_HOOK_FASTCALL KeepInRange(int pIndex, int pRange) {
    while (pIndex < 0) {
        pIndex += pRange;
    }
    while (pIndex >= pRange) {
        pIndex -= pRange;
    }
    return pIndex;
}

int C2_HOOK_FASTCALL GotItAlready(tCar_spec* pCar) {
    int i;

    for (i = 0; i < gProgram_state.number_of_cars; i++) {
        if (gProgram_state.cars_available[i] == pCar->index) {
            return 1;
        }
    }
    return 0;
}

int C2_HOOK_FASTCALL GetCarSelectedByMouse(int pX, int pY) {

    gPicked_wreck = -1;
    DRScenePick2DXY(gFrontend_wrecks_actor,
        gFrontend_wrecks_camera,
        gFrontend_wrecks_pixelmap,
        pX - gFrontend_wrecks_pixelmap->origin_x - 65,
        pY - gFrontend_wrecks_pixelmap->origin_y - 55,
        WreckPick,
        NULL);
    return gPicked_wreck;
}

// FUNCTION: CARMA2_HW 0x0046eb80
int C2_NAKED C2_HOOK_FASTCALL WrecksUpdateFunc(tFrontend_spec* pFrontend) {
#ifndef CARPOCALYPSE2_MATCHING
    int item_at_mouse;
    int i;
    int mouse_x;
    int mouse_y;
    tU32 now;
    int count_visible_cars;
    int rotate_angle;
    int index_first_car;

    item_at_mouse = -1;
    gFrontend_mouse_down = 0;
    if (gMouse_in_use) {
        gFrontend_selected_item_index = 0;
        ResetInterfaceTimeout();
        GetMousePosition(&mouse_x, &mouse_y);
        item_at_mouse = GetItemAtMousePos(pFrontend, mouse_x, mouse_y);
        if (item_at_mouse != -1) {
            gFrontend_selected_item_index = item_at_mouse;
        }
        gPrev_frontend_mouse_down = gFrontend_mouse_down;
        gFrontend_mouse_down = EitherMouseButtonDown();
    }
    if (gFrontend_selected_item_index == 0) {
        gFrontend_selected_item_index = 10;
    }

    now = PDGetTotalTime();
    if (gFrontend_wreck_bought_car_dz != 0.f) {
        int dt;

        dt = now - gFrontend_wrecks_previous_update;
        gFrontend_wreck_bought_car_dz += 0.0025f * (float)dt;
        if (gFrontend_wreck_bought_car_dz > 2.5f) {
            gFrontend_wreck_bought_car_dz = 0.f;
            gWreck_gallery_sell_infos[gFrontend_wrecks_current].sold = 1;
        }
    }

    if (gFrontend_wrecks_pending_hscroll > 0) {
        gFrontend_wrecks_pending_hscroll -= 5;
        gFrontend_wrecks_hscroll -= 5;
        if (gFrontend_wrecks_pending_hscroll < 0) {
            gFrontend_wrecks_pending_hscroll = 0;
        }
    }
    if (gFrontend_wrecks_pending_hscroll < 0) {
        gFrontend_wrecks_pending_hscroll += 5;
        gFrontend_wrecks_hscroll += 5;
        if (gFrontend_wrecks_pending_hscroll > 0) {
            gFrontend_wrecks_pending_hscroll = 0;
        }
    }

    gFrontend_wrecks_previous_update = now;

    strcpy(pFrontend->items[6].text, MungeCommas(gProgram_state.credits));

    if (gFrontend_wrecks_hscroll % 30 == 0 && gFrontend_wreck_bought_car_dz == 0.f) {
        tCar_spec* car;
        tOpponent_spec* opponent;

        pFrontend->items[2].enabled = kFrontendItemEnabled_enabled;
        pFrontend->items[3].enabled = kFrontendItemEnabled_enabled;
        pFrontend->items[4].enabled = kFrontendItemEnabled_enabled;

        gFrontend_wrecks_current = KeepInRange((gFrontend_wrecks_hscroll / 30) % gFrontend_wrecks_car_count, gFrontend_wrecks_car_count);

        car = GetCarSpec(eVehicle_opponent, gFrontend_wrecks_current);
        opponent = GetOpponentSpecFromCarSpec(car);
        if (!gWreck_gallery_sell_infos[gFrontend_wrecks_current].sold) {
            int got_it_already;

            sprintf(pFrontend->items[2].text, "%s @R%s",
                gOpponents[opponent->index].car_name,
                gOpponents[opponent->index].line4_description);
            MungeMetaCharactersChar(pFrontend->items[2].text, 'R', '\r');
            got_it_already = GotItAlready(car);
            pFrontend->items[9].enabled = kFrontendItemEnabled_disabled;
            pFrontend->items[2].visible = 1;
            if (!car->knackered) {
                sprintf(pFrontend->items[8].text, "%s", GetMiscString(eMiscString_not_wasted));
            } else {
                int price = gOpponents[opponent->index].price;

                if (price > gProgram_state.credits) {
                    sprintf(pFrontend->items[8].text, "%s - %s",
                        MungeCommas(price),
                        GetMiscString(eMiscString_dream_on));
                } else if (got_it_already) {
                    sprintf(pFrontend->items[8].text, "%s",
                        GetMiscString(eMiscString_already_have_it));
                } else {
                    strcpy(pFrontend->items[8].text, MungeCommas(price));
                    pFrontend->items[9].enabled = kFrontendItemEnabled_enabled;
                }
            }
            if (gFrontend_opponent_profile_pic_needs_update) {
                MenuSetDriverImage(opponent->index, 11);
                gFrontend_opponent_profile_pic_needs_update = 0;
            }
            FuckWithWidths(pFrontend);
        } else {
            pFrontend->items[2].visible = 0;
        }

        if (gFrontend_mouse_down && gFrontend_wrecks_pending_hscroll == 0) {
            int selected_car;

            if (gFrontend_wrecks_rotate_prev_x != -1 && gFrontend_wrecks_rotate_prev_y != -1) {
                BrMatrix34RollingBall(&gWreck_gallery_car_infos[gFrontend_wrecks_current].field_0x14,
                    mouse_x - gFrontend_wrecks_rotate_prev_x,
                    mouse_y - gFrontend_wrecks_rotate_prev_y,
                    50);
            }
            selected_car = GetCarSelectedByMouse(mouse_x, mouse_y);
            if (selected_car != -1) {
                if (selected_car == gFrontend_wrecks_current) {
                    if (gFrontend_wrecks_rotate_prev_x == -1) {
                        gFrontend_wrecks_rotate_prev_x = mouse_x;
                        gFrontend_wrecks_rotate_prev_y = mouse_y;
                    }
                } else if (!gPrev_frontend_mouse_down) {
                    DRS3StartSound(gEffects_outlet, eSoundId_LeftButton);

                    if (gFrontend_wrecks_current == KeepInRange(selected_car + 2, gFrontend_wrecks_car_count) && gFrontend_wrecks_car_count >= 2) {
                        gFrontend_wrecks_pending_hscroll += 60;
                    } else if (gFrontend_wrecks_current == KeepInRange(selected_car + 1, gFrontend_wrecks_car_count)) {
                        gFrontend_wrecks_pending_hscroll += 30;
                    } else if (gFrontend_wrecks_current == KeepInRange(selected_car - 2, gFrontend_wrecks_car_count) && gFrontend_wrecks_car_count >= 2) {
                        gFrontend_wrecks_pending_hscroll -= 60;
                    } else if (gFrontend_wrecks_current == KeepInRange(selected_car - 1, gFrontend_wrecks_car_count)) {
                        gFrontend_wrecks_pending_hscroll -= 30;
                    }
                }
            }
        }
        if (!gFrontend_mouse_down) {
            gFrontend_wrecks_rotate_prev_x = -1;
            gFrontend_wrecks_rotate_prev_y = -1;
        }
    } else {
        pFrontend->items[2].visible = 0;
        pFrontend->items[2].enabled = kFrontendItemEnabled_disabled;
        pFrontend->items[3].enabled = kFrontendItemEnabled_disabled;
        pFrontend->items[4].enabled = kFrontendItemEnabled_disabled;
        pFrontend->items[9].enabled = kFrontendItemEnabled_disabled;
    }

    count_visible_cars = MIN(5, gFrontend_wrecks_car_count);
    rotate_angle = count_visible_cars / 2 * -30 - gFrontend_wrecks_hscroll % 30;

    if (gFrontend_wrecks_car_count == 1) {
        pFrontend->items[3].enabled = kFrontendItemEnabled_disabled;
        pFrontend->items[4].enabled = kFrontendItemEnabled_disabled;
    } else {
        pFrontend->items[3].enabled = kFrontendItemEnabled_enabled;
        pFrontend->items[4].enabled = kFrontendItemEnabled_enabled;
    }

    index_first_car = KeepInRange((gFrontend_wrecks_hscroll / 30) % gFrontend_wrecks_car_count - count_visible_cars / 2, gFrontend_wrecks_car_count);

    for (i = 0; i < gFrontend_wrecks_car_count; i++) {
        gWreck_gallery_car_infos[i].actor->render_style = BR_RSTYLE_NONE;
    }

    for (i = 0; i < count_visible_cars; i++) {
        int car_index;
        br_matrix34 *mat;
        br_vector3 pos;

        car_index = KeepInRange(index_first_car + i, gFrontend_wrecks_car_count);
        mat = &gWreck_gallery_car_infos[car_index].actor->t.t.mat;
        BrMatrix34RotateY(mat, BrDegreeToAngle(rotate_angle + 30 * i));
        BrMatrix34PostRotateX(mat, BrDegreeToAngle(340));
        if (car_index == gFrontend_wrecks_current) {
            BrMatrix34PreTranslate(mat, 0.f, 0.f, -1.5f - gFrontend_wreck_bought_car_dz);
        } else {
            BrMatrix34PreTranslate(mat, 0.f, 0.f, -1.5f);
        }
        BrMatrix34Post(&gWreck_gallery_car_infos[car_index].field_0x74,
            &gWreck_gallery_car_infos[car_index].field_0x14);

        BrVector3Copy(&pos, &gWreck_gallery_car_infos[car_index].actor->t.t.translate.t);
        BrVector3Set(&gWreck_gallery_car_infos[car_index].actor->t.t.translate.t, 0.f, 0.f, 0.f);
        BrMatrix34Pre(mat, &gWreck_gallery_car_infos[car_index].field_0x74);
        BrVector3Copy(&gWreck_gallery_car_infos[car_index].actor->t.t.translate.t, &pos);
        gWreck_gallery_car_infos[car_index].actor-> render_style = BR_RSTYLE_FACES;
        if (gWreck_gallery_sell_infos[car_index].sold) {
            gWreck_gallery_car_infos[car_index].actor-> render_style = BR_RSTYLE_NONE;
        }
    }

    FuckWithWidths(pFrontend);
    if (PDKeyDown(51) || PDKeyDown(52) || (gFrontend_mouse_down && item_at_mouse != -1)) {

        ToggleSelection(pFrontend);
        if (pFrontend->items[gFrontend_selected_item_index].action != NULL) {
            pFrontend->items[gFrontend_selected_item_index].action(pFrontend);
        }
        if (pFrontend->items[gFrontend_selected_item_index].field_0xc == 1) {
            gFrontend_leave_current_menu = 1;
        }
        if (pFrontend->items[gFrontend_selected_item_index].menuInfo != NULL) {

            gCurrent_frontend_spec->default_item = gFrontend_selected_item_index;
            for (i = 0; i < gCurrent_frontend_spec->count_scrollers; i++) {
                gCurrent_frontend_spec->scrollers[i].indexOfItemAtTop = gCurrent_frontend_spec->scrollers[i].indexTopItem;
            }
            FRONTEND_DestroyMenu(pFrontend);
            gCurrent_frontend_spec = pFrontend->items[gFrontend_selected_item_index].menuInfo;
            FRONTEND_CreateMenu(gCurrent_frontend_spec);
            if (gCurrent_frontend_spec != pFrontend->previous) {
                gCurrent_frontend_spec->previous = pFrontend;
            }
            Morph_Initialise(pFrontend, gCurrent_frontend_spec);
            if (gCurrent_frontend_spec == &gFrontend_QUIT) {
                gFrontend_selected_item_index = 0;
            } else {
                gFrontend_selected_item_index = gCurrent_frontend_spec->default_item;
            }
            for (i = 0; i < gCurrent_frontend_spec->count_scrollers; i++) {
                gCurrent_frontend_spec->scrollers[i].indexOfItemAtTop = gCurrent_frontend_spec->scrollers[i].indexTopItem;
            }
            UpdateScrollPositions(gCurrent_frontend_spec);
        }
    }
    if (gFrontend_leave_current_menu != 0) {
        return 1;
    } else {
        return 0;
    }
#else
    __asm {
            sub esp, 0x24
            mov eax, dword ptr [gMouse_in_use]
            push ebx
            push ebp
            xor ebx, ebx
            mov ebp, ecx
            push esi
            cmp eax, ebx
            push edi
            mov dword ptr [esp + 0x24], ebp
            mov dword ptr [esp + 0x20], 0xffffffff
            mov dword ptr [gFrontend_mouse_down], ebx
            je L0046ebf7
            mov dword ptr [gFrontend_selected_item_index], ebx
            call PDGetTotalTime
            lea edx, [esp + 0x14]
            lea ecx, [esp + 0x10]
            mov dword ptr [gFrontend_time_last_input], eax
            call GetMousePosition
            mov eax, dword ptr [esp + 0x14]
            mov edx, dword ptr [esp + 0x10]
            push eax
            mov ecx, ebp
            call GetItemAtMousePos
            cmp eax, -1
            mov dword ptr [esp + 0x20], eax
            je L0046ebe1
            mov dword ptr [gFrontend_selected_item_index], eax
    L0046ebe1:
            mov ecx, dword ptr [gFrontend_mouse_down]
            mov dword ptr [gPrev_frontend_mouse_down], ecx
            call EitherMouseButtonDown
            mov dword ptr [gFrontend_mouse_down], eax
    L0046ebf7:
            cmp dword ptr [gFrontend_selected_item_index], ebx
            jne L0046ec09
            mov dword ptr [gFrontend_selected_item_index], 0xa
    L0046ec09:
            call PDGetTotalTime
            fld dword ptr [gFrontend_wreck_bought_car_dz]
            fcomp dword ptr [gDouble_0058a068+0x30]
            mov ecx, eax
            fnstsw ax
            test ah, 0x40
            jne L0046ec6d
            mov eax, dword ptr [gFrontend_wrecks_previous_update]
            mov edx, ecx
            sub edx, eax
            mov dword ptr [esp + 0x1c], edx
            fild dword ptr [esp + 0x1c]
            fmul qword ptr [gDouble_0058a068+0x38]
            fadd dword ptr [gFrontend_wreck_bought_car_dz]
            fst dword ptr [gFrontend_wreck_bought_car_dz]
            fcomp qword ptr [gDouble_0058a068+0x40]
            fnstsw ax
            test ah, 0x41
            jne L0046ec6d
            mov eax, dword ptr [gFrontend_wrecks_current]
            mov dword ptr [gFrontend_wreck_bought_car_dz], 0
            mov dword ptr [eax*8 + gWreck_gallery_sell_infos], 1
    L0046ec6d:
            mov eax, dword ptr [gFrontend_wrecks_pending_hscroll]
            mov edx, 5
            cmp eax, ebx
            jle L0046ec9d
            mov edi, dword ptr [gFrontend_wrecks_hscroll]
            sub eax, edx
            sub edi, edx
            cmp eax, ebx
            mov dword ptr [gFrontend_wrecks_pending_hscroll], eax
            mov dword ptr [gFrontend_wrecks_hscroll], edi
            jge L0046ecbe
            xor eax, eax
            mov dword ptr [gFrontend_wrecks_pending_hscroll], eax
            cmp eax, ebx
    L0046ec9d:
            jge L0046ecbe
            mov esi, dword ptr [gFrontend_wrecks_hscroll]
            add eax, edx
            add esi, edx
            cmp eax, ebx
            mov dword ptr [gFrontend_wrecks_pending_hscroll], eax
            mov dword ptr [gFrontend_wrecks_hscroll], esi
            jle L0046ecbe
            mov dword ptr [gFrontend_wrecks_pending_hscroll], ebx
    L0046ecbe:
            mov dword ptr [gFrontend_wrecks_previous_update], ecx
            mov ecx, dword ptr [gProgram_state]
            call MungeCommas
            mov edi, eax
            or ecx, 0xffffffff
            xor eax, eax
            lea edx, [ebp + 0x999]
            repne scasb
            not ecx
            sub edi, ecx
            mov eax, ecx
            mov esi, edi
            mov edi, edx
            shr ecx, 2
            rep movsd
            mov ecx, eax
            and ecx, 3
            rep movsb
            mov eax, dword ptr [gFrontend_wrecks_hscroll]
            mov ecx, 0x1e
            cdq
            idiv ecx
            test edx, edx
            jne L0046f0c2
            fld dword ptr [gFrontend_wreck_bought_car_dz]
            fcomp dword ptr [gDouble_0058a068+0x30]
            fnstsw ax
            test ah, 0x40
            je L0046f0c2
            mov eax, 1
            mov dword ptr [ebp + 0x408], eax
            mov dword ptr [ebp + 0x560], eax
            mov dword ptr [ebp + 0x6b8], eax
            mov ecx, dword ptr [gFrontend_wrecks_hscroll]
            mov eax, 0x88888889
            imul ecx
            add edx, ecx
            sar edx, 4
            mov ecx, edx
            mov eax, edx
            shr ecx, 0x1f
            add eax, ecx
            mov ecx, dword ptr [gFrontend_wrecks_car_count]
            cdq
            idiv ecx
            cmp edx, ebx
            mov dword ptr [gFrontend_wrecks_current], edx
            jge L0046ed6f
    L0046ed65:
            add edx, ecx
            js L0046ed65
            mov dword ptr [gFrontend_wrecks_current], edx
    L0046ed6f:
            cmp edx, ecx
            jl L0046ed7f
    L0046ed73:
            sub edx, ecx
            cmp edx, ecx
            jge L0046ed73
            mov dword ptr [gFrontend_wrecks_current], edx
    L0046ed7f:
            mov ecx, 2
            call GetCarSpec
            mov edi, eax
            mov ecx, edi
            call GetOpponentSpecFromCarSpec
            mov edx, dword ptr [gFrontend_wrecks_current]
            mov ebx, eax
            mov eax, dword ptr [edx*8 + gWreck_gallery_sell_infos]
            test eax, eax
            je L0046edb4
            mov dword ptr [ebp + 0x40c], 0
            jmp L0046ef08
    L0046edb4:
            mov eax, dword ptr [ebx]
            mov edx, dword ptr [gOpponents]
            mov ecx, eax
            lea esi, [ebp + 0x439]
            shl ecx, 3
            sub ecx, eax
            shl ecx, 6
            lea eax, [ecx + edx]
            lea ecx, [eax + 0x110]
            add eax, 0x90
            push ecx
            push eax
            push offset gFrontend_QUIT+0xbc68
            push esi
            call sprintf
            add esp, 0x10
            mov dl, 0x52
            mov ecx, esi
            push 0xd
            call MungeMetaCharactersChar
            mov edx, dword ptr [gProgram_state+0x88]
            xor eax, eax
            test edx, edx
            jle L0046ee18
            mov esi, dword ptr [edi]
            mov ecx, offset gProgram_state+0x1aa0
    L0046ee08:
            cmp dword ptr [ecx], esi
            je L0046f462
            inc eax
            add ecx, 4
            cmp eax, edx
            jl L0046ee08
    L0046ee18:
            xor edx, edx
    L0046ee1a:
            mov dword ptr [ebp + 0xd70], 0xffffffff
            mov dword ptr [ebp + 0x40c], 1
            mov eax, dword ptr [edi + 0x1d4]
            test eax, eax
            jne L0046ee3f
            mov ecx, 0x124
            jmp L0046ee92
    L0046ee3f:
            mov eax, dword ptr [ebx]
            mov ecx, eax
            shl ecx, 3
            sub ecx, eax
            mov eax, dword ptr [gOpponents]
            shl ecx, 6
            mov ecx, dword ptr [ecx + eax + 0x1a0]
            mov eax, dword ptr [gProgram_state]
            cmp eax, ecx
            jge L0046ee89
            call MungeCommas
            mov ecx, 0x123
            mov esi, eax
            call GetMiscString
            lea ecx, [ebp + 0xc49]
            push eax
            push esi
            push offset gFrontend_QUIT+0xbc60
            push ecx
            call sprintf
            add esp, 0x10
            jmp L0046eee2
    L0046ee89:
            test edx, edx
            je L0046eeae
            mov ecx, 0x125
    L0046ee92:
            call GetMiscString
            lea edx, [ebp + 0xc49]
            push eax
            push offset gCancel_recover+0xbc
            push edx
            call sprintf
            add esp, 0xc
            jmp L0046eee2
    L0046eeae:
            call MungeCommas
            mov edi, eax
            or ecx, 0xffffffff
            xor eax, eax
            lea edx, [ebp + 0xc49]
            repne scasb
            not ecx
            sub edi, ecx
            mov eax, ecx
            mov esi, edi
            mov edi, edx
            shr ecx, 2
            rep movsd
            mov ecx, eax
            and ecx, 3
            rep movsb
            mov dword ptr [ebp + 0xd70], 1
    L0046eee2:
            mov eax, dword ptr [gFrontend_opponent_profile_pic_needs_update]
            test eax, eax
            je L0046ef01
            mov ecx, dword ptr [ebx]
            mov edx, 0xb
            call MenuSetDriverImage
            mov dword ptr [gFrontend_opponent_profile_pic_needs_update], 0
    L0046ef01:
            mov ecx, ebp
            call FuckWithWidths
    L0046ef08:
            mov eax, dword ptr [gFrontend_mouse_down]
            test eax, eax
            je L0046f09a
            mov eax, dword ptr [gFrontend_wrecks_pending_hscroll]
            test eax, eax
            jne L0046f09a
            mov ecx, dword ptr [gFrontend_wrecks_rotate_prev_x]
            cmp ecx, -1
            je L0046ef62
            mov eax, dword ptr [gFrontend_wrecks_rotate_prev_y]
            cmp eax, -1
            je L0046ef62
            mov edx, dword ptr [esp + 0x14]
            push 0x32
            sub edx, eax
            mov eax, dword ptr [esp + 0x14]
            sub eax, ecx
            push edx
            push eax
            mov eax, dword ptr [gFrontend_wrecks_current]
            lea ecx, [eax + eax*4]
            lea edx, [eax + ecx*8]
            lea eax, [edx*4 + gWreck_gallery_car_infos+0x14]
            push eax
            call BrMatrix34RollingBall
            add esp, 0x10
    L0046ef62:
            mov eax, dword ptr [gFrontend_wrecks_pixelmap]
            mov edx, dword ptr [esp + 0x14]
            or edi, 0xffffffff
            push 0
            mov dword ptr [gPicked_wreck], edi
            push offset WreckPick
            movsx ecx, word ptr [eax + 0x3a]
            sub edx, ecx
            movsx ecx, word ptr [eax + 0x38]
            sub edx, 0x37
            push edx
            mov edx, dword ptr [esp + 0x1c]
            sub edx, ecx
            mov ecx, dword ptr [gFrontend_wrecks_actor]
            sub edx, 0x41
            push edx
            mov edx, dword ptr [gFrontend_wrecks_camera]
            push eax
            call DRScenePick2DXY
            mov esi, dword ptr [gPicked_wreck]
            cmp esi, edi
            je L0046f0a5
            cmp esi, dword ptr [gFrontend_wrecks_current]
            jne L0046efdf
            cmp dword ptr [gFrontend_wrecks_rotate_prev_x], edi
            jne L0046f0a5
            mov eax, dword ptr [esp + 0x10]
            mov ecx, dword ptr [esp + 0x14]
            mov dword ptr [gFrontend_wrecks_rotate_prev_x], eax
            mov dword ptr [gFrontend_wrecks_rotate_prev_y], ecx
            jmp L0046f0a5
    L0046efdf:
            mov eax, dword ptr [gPrev_frontend_mouse_down]
            test eax, eax
            jne L0046f0a5
            mov ecx, dword ptr [gEffects_outlet]
            mov edx, 0xbb8
            call DRS3StartSound
            mov ecx, dword ptr [gFrontend_wrecks_car_count]
            lea eax, [esi + 2]
            test eax, eax
            jge L0046f00d
    L0046f009:
            add eax, ecx
            js L0046f009
    L0046f00d:
            cmp eax, ecx
            jl L0046f017
    L0046f011:
            sub eax, ecx
            cmp eax, ecx
            jge L0046f011
    L0046f017:
            mov edx, dword ptr [gFrontend_wrecks_current]
            cmp edx, eax
            jne L0046f02f
            cmp ecx, 3
            jle L0046f02f
            add dword ptr [gFrontend_wrecks_pending_hscroll], 0x3c
            jmp L0046f0ab
    L0046f02f:
            lea eax, [esi + 1]
            test eax, eax
            jge L0046f03a
    L0046f036:
            add eax, ecx
            js L0046f036
    L0046f03a:
            cmp eax, ecx
            jl L0046f044
    L0046f03e:
            sub eax, ecx
            cmp eax, ecx
            jge L0046f03e
    L0046f044:
            cmp edx, eax
            jne L0046f051
            add dword ptr [gFrontend_wrecks_pending_hscroll], 0x1e
            jmp L0046f0ab
    L0046f051:
            lea eax, [esi - 2]
            test eax, eax
            jge L0046f05c
    L0046f058:
            add eax, ecx
            js L0046f058
    L0046f05c:
            cmp eax, ecx
            jl L0046f066
    L0046f060:
            sub eax, ecx
            cmp eax, ecx
            jge L0046f060
    L0046f066:
            cmp edx, eax
            jne L0046f078
            cmp ecx, 3
            jle L0046f078
            sub dword ptr [gFrontend_wrecks_pending_hscroll], 0x3c
            jmp L0046f0ab
    L0046f078:
            lea eax, [esi - 1]
            test eax, eax
            jge L0046f083
    L0046f07f:
            add eax, ecx
            js L0046f07f
    L0046f083:
            cmp eax, ecx
            jl L0046f08d
    L0046f087:
            sub eax, ecx
            cmp eax, ecx
            jge L0046f087
    L0046f08d:
            cmp edx, eax
            jne L0046f0ab
            sub dword ptr [gFrontend_wrecks_pending_hscroll], 0x1e
            jmp L0046f0ab
    L0046f09a:
            mov ecx, dword ptr [gFrontend_wrecks_car_count]
            or edi, 0xffffffff
            jmp L0046f0ab
    L0046f0a5:
            mov ecx, dword ptr [gFrontend_wrecks_car_count]
    L0046f0ab:
            mov eax, dword ptr [gFrontend_mouse_down]
            test eax, eax
            jne L0046f0e9
            mov dword ptr [gFrontend_wrecks_rotate_prev_x], edi
            mov dword ptr [gFrontend_wrecks_rotate_prev_y], edi
            jmp L0046f0e9
    L0046f0c2:
            or eax, 0xffffffff
            mov dword ptr [ebp + 0x40c], ebx
            mov dword ptr [ebp + 0x408], eax
            mov dword ptr [ebp + 0x560], eax
            mov dword ptr [ebp + 0x6b8], eax
            mov dword ptr [ebp + 0xd70], eax
            mov ecx, dword ptr [gFrontend_wrecks_car_count]
    L0046f0e9:
            cmp ecx, 5
            mov ebx, 5
            jg L0046f0f5
            mov ebx, ecx
    L0046f0f5:
            mov eax, ebx
            mov edi, 0x1e
            cdq
            sub eax, edx
            mov esi, eax
            mov eax, dword ptr [gFrontend_wrecks_hscroll]
            cdq
            idiv edi
            sar esi, 1
            mov eax, esi
            shl eax, 4
            sub eax, esi
            neg eax
            shl eax, 1
            sub eax, edx
            mov dword ptr [esp + 0x1c], eax
            mov eax, 1
            fild dword ptr [esp + 0x1c]
            cmp ecx, eax
            fstp dword ptr [esp + 0x18]
            jne L0046f130
            or eax, 0xffffffff
    L0046f130:
            mov dword ptr [ebp + 0x560], eax
            mov dword ptr [ebp + 0x6b8], eax
            mov ecx, dword ptr [gFrontend_wrecks_hscroll]
            mov eax, 0x88888889
            imul ecx
            add edx, ecx
            sar edx, 4
            mov ecx, edx
            mov eax, edx
            shr ecx, 0x1f
            add eax, ecx
            mov ecx, dword ptr [gFrontend_wrecks_car_count]
            cdq
            idiv ecx
            mov edi, edx
            sub edi, esi
            jns L0046f16a
    L0046f166:
            add edi, ecx
            js L0046f166
    L0046f16a:
            cmp edi, ecx
            jl L0046f174
    L0046f16e:
            sub edi, ecx
            cmp edi, ecx
            jge L0046f16e
    L0046f174:
            xor eax, eax
            test ecx, ecx
            jle L0046f196
            mov edx, offset gWreck_gallery_car_infos+0x10
    L0046f17f:
            mov ecx, dword ptr [edx]
            inc eax
            add edx, 0xa4
            mov byte ptr [ecx + 0x20], 1
            mov ecx, dword ptr [gFrontend_wrecks_car_count]
            cmp eax, ecx
            jl L0046f17f
    L0046f196:
            test ebx, ebx
            jle L0046f2d7
            mov dword ptr [esp + 0x1c], ebx
            jmp L0046f1aa
    L0046f1a4:
            mov ecx, dword ptr [gFrontend_wrecks_car_count]
    L0046f1aa:
            test edi, edi
            jge L0046f1b2
    L0046f1ae:
            add edi, ecx
            js L0046f1ae
    L0046f1b2:
            cmp edi, ecx
            jl L0046f1bc
    L0046f1b6:
            sub edi, ecx
            cmp edi, ecx
            jge L0046f1b6
    L0046f1bc:
            fld dword ptr [esp + 0x18]
            fmul qword ptr [gDouble_0058a068+0x48]
            lea edx, [edi + edi*4]
            lea esi, [edi + edx*8]
            shl esi, 2
            mov ebp, dword ptr [esi + gWreck_gallery_car_infos+0x10]
            add ebp, 0x2c
            call _ftol
            push eax
            push ebp
            call BrMatrix34RotateY
            add esp, 8
            push 0xf1c8
            push ebp
            call BrMatrix34PostRotateX
            mov eax, dword ptr [gFrontend_wrecks_current]
            add esp, 8
            cmp edi, eax
            jne L0046f212
            fld dword ptr [gFrontend_wreck_bought_car_dz]
            fsub qword ptr [gDouble_0058a068+0x50]
            push ecx
            fchs
            _emit 0xd9
        _emit 0x1c
        _emit 0x24
            jmp L0046f217
    L0046f212:
            push 0xbfc00000
    L0046f217:
            push 0
            push 0
            push ebp
            call BrMatrix34PreTranslate
            add esp, 0x10
            lea eax, [esi + gWreck_gallery_car_infos+0x14]
            lea ebx, [esi + gWreck_gallery_car_infos+0x74]
            push eax
            push ebx
            call BrMatrix34Post
            mov eax, dword ptr [esi + gWreck_gallery_car_infos+0x10]
            add esp, 8
            add eax, 0x50
            mov ecx, eax
            push ebx
            push ebp
            mov edx, dword ptr [ecx]
            mov dword ptr [esp + 0x30], edx
            mov edx, dword ptr [ecx + 4]
            mov dword ptr [esp + 0x34], edx
            mov ecx, dword ptr [ecx + 8]
            mov dword ptr [esp + 0x38], ecx
            xor ecx, ecx
            mov dword ptr [eax], ecx
            mov edx, dword ptr [esi + gWreck_gallery_car_infos+0x10]
            mov dword ptr [edx + 0x54], ecx
            mov eax, dword ptr [esi + gWreck_gallery_car_infos+0x10]
            mov dword ptr [eax + 0x58], ecx
            call BrMatrix34Pre
            mov ecx, dword ptr [esi + gWreck_gallery_car_infos+0x10]
            mov edx, dword ptr [esp + 0x30]
            mov eax, dword ptr [esp + 0x34]
            add ecx, 0x50
            add esp, 8
            mov dword ptr [ecx], edx
            mov edx, dword ptr [esp + 0x30]
            mov dword ptr [ecx + 4], eax
            mov dword ptr [ecx + 8], edx
            mov eax, dword ptr [esi + gWreck_gallery_car_infos+0x10]
            mov byte ptr [eax + 0x20], 4
            mov eax, dword ptr [edi*8 + gWreck_gallery_sell_infos]
            test eax, eax
            je L0046f2b5
            mov ecx, dword ptr [esi + gWreck_gallery_car_infos+0x10]
            mov byte ptr [ecx + 0x20], 1
    L0046f2b5:
            fld dword ptr [esp + 0x18]
            fsub dword ptr [gDouble_0058a068+0x58]
            mov eax, dword ptr [esp + 0x1c]
            inc edi
            dec eax
            mov dword ptr [esp + 0x1c], eax
            fstp dword ptr [esp + 0x18]
            jne L0046f1a4
            mov ebp, dword ptr [esp + 0x24]
    L0046f2d7:
            mov ecx, ebp
            call FuckWithWidths
            mov ecx, 0x33
            call PDKeyDown
            test eax, eax
            jne L0046f312
            mov ecx, 0x34
            call PDKeyDown
            test eax, eax
            jne L0046f312
            mov eax, dword ptr [gFrontend_mouse_down]
            test eax, eax
            je L0046f44d
            cmp dword ptr [esp + 0x20], -1
            je L0046f44d
    L0046f312:
            mov ecx, ebp
            call ToggleSelection
            mov eax, dword ptr [gFrontend_selected_item_index]
            lea edx, [eax + eax*4]
            lea ecx, [eax + edx*4]
            lea edx, [eax + ecx*2]
            mov edx, dword ptr [ebp + edx*8 + 0x134]
            test edx, edx
            je L0046f33b
            mov ecx, ebp
            call edx
            mov eax, dword ptr [gFrontend_selected_item_index]
    L0046f33b:
            lea ecx, [eax + eax*4]
            lea edx, [eax + ecx*4]
            lea ecx, [eax + edx*2]
            mov edx, 1
            mov esi, dword ptr [ebp + ecx*8 + 0x13c]
            lea ecx, [ebp + ecx*8]
            cmp esi, edx
            jne L0046f35e
            mov dword ptr [gFrontend_leave_current_menu], edx
    L0046f35e:
            mov edx, dword ptr [ecx + 0x138]
            test edx, edx
            je L0046f44d
            mov edx, dword ptr [gCurrent_frontend_spec]
            mov dword ptr [edx + 0x118], eax
            mov eax, dword ptr [gCurrent_frontend_spec]
            xor edx, edx
            mov ecx, dword ptr [eax + 0x8790]
            test ecx, ecx
            jle L0046f3aa
            xor ecx, ecx
    L0046f38b:
            mov esi, dword ptr [eax + ecx + 0x87a0]
            inc edx
            mov dword ptr [eax + ecx + 0x87c0], esi
            mov eax, dword ptr [gCurrent_frontend_spec]
            add ecx, 0x34
            cmp edx, dword ptr [eax + 0x8790]
            jl L0046f38b
    L0046f3aa:
            mov ecx, ebp
            call FRONTEND_DestroyMenu
            mov eax, dword ptr [gFrontend_selected_item_index]
            lea ecx, [eax + eax*4]
            lea edx, [eax + ecx*4]
            lea eax, [eax + edx*2]
            mov ecx, dword ptr [ebp + eax*8 + 0x138]
            mov dword ptr [gCurrent_frontend_spec], ecx
            call FRONTEND_CreateMenu
            mov edx, dword ptr [gCurrent_frontend_spec]
            mov eax, dword ptr [ebp + 0x114]
            cmp edx, eax
            je L0046f3ed
            mov dword ptr [edx + 0x114], ebp
            mov edx, dword ptr [gCurrent_frontend_spec]
    L0046f3ed:
            mov ecx, ebp
            call Morph_Initialise
            mov ecx, dword ptr [gCurrent_frontend_spec]
            cmp ecx, offset gFrontend_QUIT
            jne L0046f40e
            mov dword ptr [gFrontend_selected_item_index], 0
            jmp L0046f41a
    L0046f40e:
            mov edx, dword ptr [ecx + 0x118]
            mov dword ptr [gFrontend_selected_item_index], edx
    L0046f41a:
            mov eax, dword ptr [ecx + 0x8790]
            xor edx, edx
            test eax, eax
            jle L0046f448
            xor eax, eax
    L0046f428:
            mov esi, dword ptr [ecx + eax + 0x87c0]
            inc edx
            mov dword ptr [ecx + eax + 0x87a0], esi
            mov ecx, dword ptr [gCurrent_frontend_spec]
            add eax, 0x34
            cmp edx, dword ptr [ecx + 0x8790]
            jl L0046f428
    L0046f448:
            call UpdateScrollPositions
    L0046f44d:
            mov ecx, dword ptr [gFrontend_leave_current_menu]
            xor eax, eax
            pop edi
            pop esi
            test ecx, ecx
            pop ebp
            pop ebx
            setne al
            add esp, 0x24
            ret
    L0046f462:
            mov edx, 1
            jmp L0046ee1a
    }
#endif
}

// FUNCTION: CARMA2_HW 0x0046e720
int C2_HOOK_FASTCALL ScrollToPrevCar(tFrontend_spec* pFrontend) {

    if (gFrontend_wreck_bought_car_dz == 0.f
            && gFrontend_wrecks_pending_hscroll == 0
            && (gFrontend_wrecks_rotate_prev_x == -1 || gFrontend_wrecks_rotate_prev_y == -1)) {

        gFrontend_wrecks_pending_hscroll = 30;
        DRS3StartSound(gEffects_outlet, eSoundId_LeftButton);
        gFrontend_opponent_profile_pic_needs_update = 1;
    }
    return 0;
}

// FUNCTION: CARMA2_HW 0x0046e6c0
int C2_HOOK_FASTCALL ScrollToNextCar(tFrontend_spec* pFrontend) {

    if (gFrontend_wreck_bought_car_dz == 0.f
            && gFrontend_wrecks_pending_hscroll == 0
            && (gFrontend_wrecks_rotate_prev_x == -1 || gFrontend_wrecks_rotate_prev_y == -1)) {

        gFrontend_wrecks_pending_hscroll = -30;
        DRS3StartSound(gEffects_outlet, eSoundId_LeftButton);
        gFrontend_opponent_profile_pic_needs_update = 1;
    }
    return 0;
}

// FUNCTION: CARMA2_HW 0x0046e780
int C2_HOOK_FASTCALL BuyCurrentCar(tFrontend_spec* pFrontend) {
    tCar_spec* car;
    tOpponent_spec* opponent;

    C2_HOOK_BUG_ON(sizeof(tWreck_gallery_sell_info) != 0x8);

    if (gFrontend_wreck_bought_car_dz != 0.f) {
        return 0;
    }
    if (gWreck_gallery_sell_infos[gFrontend_wrecks_current].sold) {
        return 0;
    }
    car = GetCarSpec(eVehicle_opponent, gFrontend_wrecks_current);
    opponent = GetOpponentSpecFromCarSpec(car);
    gProgram_state.cars_available[gProgram_state.number_of_cars] = car->index;
    gProgram_state.number_of_cars += 1;
    gProgram_state.current_car_index = car->index;
    gProgram_state.credits -= gOpponents[opponent->index].price;
    gFrontend_wreck_bought_car_dz = 0.001f;
    DRS3StartSound(gEffects_outlet, eSoundId_Done);
    return 0;
}