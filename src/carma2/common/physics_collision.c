#include "physics.h"

#include "globvars.h"
#include "platform.h"
#include "utility.h"

#include <brender/brender.h>

#include "carpocalypse2_macros.h"

// SetCollisionInfoDoNothing

// SetCollisionInfoChildsDoNothing

// SetCollisionInfoDoNothing

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