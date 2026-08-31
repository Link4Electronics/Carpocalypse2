#include "world1.h"

#include "world.h"
#include "globvars.h"
#include "pedestrn.h"
#include "c2_string.h"
#include "carpocalypse2_types.h"

extern char gAdditional_actor_path[256];
extern char gAdditional_model_path[256];
extern br_actor* gLast_actor;

// DeleteBastards

// DeleteAnyZeroBastards

// ApplyTransToModels

// FUNCTION: CARMA2_HW 0x00507360
void C2_HOOK_FASTCALL AutoSaveAdditionalStuff(void) {
    int len1;
    int len2;

    len1 = strlen(gAdditional_actor_path);
    len2 = strlen(gAdditional_actor_path);
    strcat(gAdditional_actor_path, " autosave");
    strcat(gAdditional_model_path, " autosave");
    if (gSpec_vol_mode != 0) {
        int index;

        index = FindSpecVolIndex(gLast_actor);
        if (index >= 0) {
            tSpecial_volume* v;

            v = &gProgram_state.special_volumes[index];
            BrMatrix34Copy(&v->boundary.box.mat, &gLast_actor->t.t.mat);
            FindInverseAndWorldBox(v);
            SetSpecVolMatSize(gLast_actor);
        }
        SaveSpecialVolumes();
    } else {
        SaveAdditionalActors();
    }
    gAdditional_actor_path[len1] = 0;
    gAdditional_model_path[len2] = 0;
    FlushAllPedCaches();
}

// FindInverseAndWorldBox

// MungeMaterialSV

int C2_HOOK_FASTCALL FindSpecVolIndex(br_actor* pActor) {
    int i;

    if (pActor == NULL) {
        return -1;
    }
    for (i = 0; i < gProgram_state.special_volume_count; i++) {

        if (gSpec_vol_actors[i] == pActor) {
            return i;
        }
    }
    return -1;
}

void C2_HOOK_FASTCALL SetSpecVolMatSize(br_actor* pActor) {
    br_model* model;

    model = pActor->model;
    MungeMaterialSV(&pActor->t.t.mat, model->faces[ 5].material, model->faces[17].material, 0, 1);
    MungeMaterialSV(&pActor->t.t.mat, model->faces[11].material, model->faces[23].material, 1, 2);
    MungeMaterialSV(&pActor->t.t.mat, model->faces[ 7].material, model->faces[19].material, 0, 2);
}

void C2_HOOK_FASTCALL UpdateSpecVol(void) {
    int index;

    index = FindSpecVolIndex(gLast_actor);
    if (index >= 0) {
        tSpecial_volume* v;

        v = &gProgram_state.special_volumes[index];
        BrMatrix34Copy(&v->boundary.box.mat, &gLast_actor->t.t.mat);
        FindInverseAndWorldBox(v);
        SetSpecVolMatSize(gLast_actor);
    }
}

// SaveAdditionalActors

// DoSaveAdditionalStuff
