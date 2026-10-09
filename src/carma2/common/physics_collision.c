#include "physics.h"

#include "globvars.h"
#include "platform.h"
#include "utility.h"

#include <brender/brender.h>

#include "carpocalypse2_macros.h"

// SetCollisionInfoDoNothing

// SetCollisionInfoChildsDoNothing

// FUNCTION: CARMA2_HW 0x004b9e40
void C2_HOOK_FASTCALL SetCollisionInfoDoNothingChain(tPhysics_object *pCollision_info, tU32 pDisable) {
    tPhysics_object *child;
    tPhysics_object *cur;
    tPhysics_object *sibling;

    pCollision_info->disable_move_rotate = pDisable;
    cur = pCollision_info;
    while (1) {
        child = cur->child;
        cur->disable_move_rotate = pDisable;
        cur->field_0x1df = 0;
        if (child != NULL) {
            for (sibling = child; sibling != NULL; sibling = sibling->next) {
                sibling->disable_move_rotate = pDisable;
                sibling->field_0x1df = 0;
                if (sibling->child != NULL) {
                    SetCollisionInfoDoNothing(sibling->child, pDisable);
                }
            }
        }
        cur = cur->field183_0x1d8;
        if (cur == NULL || cur == pCollision_info) {
            break;
        }
    }
}

// FUNCTION: CARMA2_HW 0x004b9eb0
void C2_HOOK_FASTCALL SetCollisionInfoDoNothing(tPhysics_object *pCollision_info, tU32 pDisable) {

    while (pCollision_info != NULL) {
        pCollision_info->disable_move_rotate = pDisable;
        pCollision_info->field_0x1df = 0;
        if (pCollision_info->child != NULL) {
            SetCollisionInfoDoNothing(pCollision_info->child, pDisable);
        }
        pCollision_info = pCollision_info->next;
    }
}