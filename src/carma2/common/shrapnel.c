#include "shrapnel.h"

#include "crush.h"
#include "errors.h"
#include "loading.h"
#include "smashing.h"
#include "temp.h"
#include "utility.h"
#include "world.h"

#include "platform.h"  /* For PDFatalError */

#include "c2_string.h"

#include "carpocalypse2_macros.h"
// GLOBAL: CARMA2_HW 0x0065fec0
const char* gShrapnel_type_names[4] = {
    "abstract",
    "shards",
    "ghostparts",
    "noncars",
};


// GLOBAL: CARMA2_HW 0x006a3338
int gShrapnel_ghost_actor_count;

// GLOBAL: CARMA2_HW 0x006a5140
br_actor* gSmashable_noncar_shrapnel_actors[100];

// GLOBAL: CARMA2_HW 0x0065fef0
const char* gSmash_side_effect_direction_names[3] = {
    "parallel",
    "towards",
    "away",
};


#ifdef CARPOCALYPSE2_MATCHING
extern void* C2_HOOK_CDECL BrMemAllocate();
extern int C2_HOOK_FASTCALL DRStricmp(const char* p1, const char* p2);
extern int C2_HOOK_FASTCALL GetALineAndInterpretCommand(FILE* pF, const char** pString_list, int pCount);
extern br_scalar C2_HOOK_FASTCALL GetAScalar(FILE* pF);
extern void C2_HOOK_FASTCALL GetAString(FILE* pF, char* pString);
extern int C2_HOOK_FASTCALL GetAnInt(FILE* pF);
extern void C2_HOOK_FASTCALL GetPairOfFloats(FILE* pF, float* pF1, float* pF2);
extern void C2_HOOK_FASTCALL GetPairOfInts(FILE* pF, int* pF1, int* pF2);
extern void C2_HOOK_FASTCALL LoadMinMax(FILE* pF, br_bounds3* pBounds);
extern const double gDOUBLE_0058af40;
extern char gDefault_drone_direction[];
extern tFrontend_spec gFrontend_QUIT;
extern char* gMaterial_names[2];
extern float gOpponent_nastyness_frigger;
extern const char* gPosition_type_names[2];
extern char gSplit_triangle_vertex_orders[];
#endif
// FUNCTION: CARMA2_HW 0x004ef440
intptr_t C2_HOOK_CDECL AddGhostActorToBuffer(br_actor* pActor, void* data) {
    br_actor** pActorBuffer = data;

    pActorBuffer[gShrapnel_ghost_actor_count] = pActor;
    gShrapnel_ghost_actor_count += 1;
    return 0;
}

static __inline br_actor* C2_HOOK_FASTCALL ReadShrapnelSpec_LoadActor(const char* pName) {
    tPath_name path;

    PathCat(path, gSmashable_track_environment_path, pName);
    return BrActorLoad(path);
}

static __inline void ReadShrapnelSpec_MovementSpec(FILE* pF, tMovement_spec* pInitial_speed) {

    GetPairOfFloats(pF, &pInitial_speed->min_speed, &pInitial_speed->max_speed);
    pInitial_speed->impactee_velocity_factor = GetAScalar(pF);
    pInitial_speed->random_velocity_max = GetAScalar(pF);
    pInitial_speed->random_up_velocity_max = GetAScalar(pF);
    pInitial_speed->random_normal_velocity_max = GetAScalar(pF);
    pInitial_speed->random_spin_rate_max = GetAScalar(pF) * 0.1f;
}

static __inline void ReadShrapnelSpec_InitialPosition(FILE* pF, tSmashable_initial_position_spec* pInitial_pos) {

    pInitial_pos->type = GetALineAndInterpretCommand(pF, gInitial_smashable_position_type_names, CARPOCALYPSE2_ASIZE(gInitial_smashable_position_type_names));
    switch (pInitial_pos->type) {
    case kInitialSmashablePosition_SphereClumped:
        pInitial_pos->position.sphere.radius = GetAScalar(pF);
        pInitial_pos->position.sphere.where = GetALineAndInterpretCommand(pF, gInitial_position_sphere_where, CARPOCALYPSE2_ASIZE(gInitial_position_sphere_where));
        break;
    case kInitialSmashablePosition_BoxClumped:
        GetThreeFloats(pF, &pInitial_pos->position.box.v[0], &pInitial_pos->position.box.v[1], &pInitial_pos->position.box.v[2]);
        break;
    default:
        break;
    }
}

static __inline void ReadShrapnelSpec_TimeInfo(FILE* pF, int* pTimes) {
    float min_time;
    float max_time;

    GetPairOfFloats(pF, &min_time, &max_time);
    pTimes[0] = (int)(min_time * 1000.0);
    pTimes[1] = (int)(max_time * 1000.0);
}

static __inline void AddMaterialToFogification(br_material* pMaterial) {
    int count = gMaterials_to_adapt_count;

    if (count < (int)CARPOCALYPSE2_ASIZE(gMaterials_to_adapt)) {
        gMaterials_to_adapt[count] = pMaterial;
        gMaterials_to_adapt_count = count + 1;
    }
}

// FUNCTION: CARMA2_HW 0x004eed70
void C2_HOOK_FASTCALL ReadShrapnelSpec(FILE* pF, tShrapnel_spec* pShrapnel_specs, int* pShrapnel_count) {
    char s[256];
    char actor_name[256];
    br_actor* actor_buffer[50];
    br_bounds bounds;
    int i;
    int j;
    int k;
    tShrapnel_spec* spec;
    tSmashable_environment_name* noncar_name;

    *pShrapnel_count = GetAnInt(pF);
    for (i = 0, spec = pShrapnel_specs; i < *pShrapnel_count; i++, spec++) {
        spec->type = GetALineAndInterpretCommand(pF, gShrapnel_type_names, CARPOCALYPSE2_ASIZE(gShrapnel_type_names));
        if (spec->type == kShrapnelType_Abstract) {
            spec->type_info.abstract.count_materials = GetAnInt(pF);
            for (j = 0; j < spec->type_info.abstract.count_materials; j++) {
                GetAString(pF, s);
                spec->type_info.abstract.materials[j] = BrMaterialFind(s);
                if (spec->type_info.abstract.materials[j] == NULL) {
                    FatalError(kFatalError_CannotFindSmashMaterial_S, s);
                }
                AddMaterialToFogification(spec->type_info.abstract.materials[j]);
            }
            continue;
        }
        ReadShrapnelSpec_MovementSpec(pF, &spec->initial_speed);
        if (spec->type != kShrapnelType_Shards) {
            ReadShrapnelSpec_InitialPosition(pF, &spec->initial_pos);
        }
        if (spec->type != kShrapnelType_NonCars) {
            ReadShrapnelSpec_TimeInfo(pF, &spec->time_info.min_time);
        }
        switch (spec->type) {
        case kShrapnelType_Shards:
            spec->type_info.shard.field_0x4 = GetAScalar(pF);
            spec->type_info.shard.field_0x8 = GetAnInt(pF);
            GetAString(pF, s);
            spec->type_info.shard.material = BrMaterialFind(s);
            if (spec->type_info.shard.material == NULL) {
                FatalError(kFatalError_CannotFindSmashMaterial_S, s);
            }
            AddMaterialToFogification(spec->type_info.shard.material);
            spec->type_info.shard.material->flags = BR_MATF_TWO_SIDED;
            BrMatrix23Identity(&spec->type_info.shard.material->map_transform);
            BrMaterialUpdate(spec->type_info.shard.material, BR_MATU_ALL);
            break;
        case kShrapnelType_GhostParts:
            GetPairOfInts(pF, &spec->type_info.ghost.field_0x0, &spec->type_info.ghost.field_0x4);
            spec->type_info.ghost.count_actors = GetAnInt(pF);
            if (spec->type_info.ghost.count_actors > 0) {
                spec->type_info.ghost.actors = BrMemAllocate(spec->type_info.ghost.count_actors * sizeof(*spec->type_info.ghost.actors), kMem_smashable_env_info);
                spec->type_info.ghost.bounds_dx = 0.f;
                spec->type_info.ghost.bounds_dy = 0.f;
                for (j = 0; j < spec->type_info.ghost.count_actors; j++) {
                    GetAString(pF, s);
                    spec->type_info.ghost.actors[j] = ReadShrapnelSpec_LoadActor(s);
                    if (spec->type_info.ghost.actors[j] == NULL) {
                        FatalError(kFatalError_CannotFindSmashActorModel_S, s);
                    }
                    ProcessMaterials(spec->type_info.ghost.actors[j], TemporaryMaterialStore);
                }
            } else {
                br_actor* actor;

                GetAString(pF, s);
                actor = ReadShrapnelSpec_LoadActor(s);
                if (actor == NULL) {
                    FatalError(kFatalError_CannotFindSmashActorModel_S, s);
                }
                ProcessMaterials(actor, TemporaryMaterialStore);
                gShrapnel_ghost_actor_count = 0;
                BrActorToBounds(&bounds, actor);
                spec->type_info.ghost.bounds_dx = bounds.max.v[0] - bounds.min.v[0];
                spec->type_info.ghost.bounds_dy = bounds.max.v[1] - bounds.min.v[1];
                BrActorEnum(actor, AddGhostActorToBuffer, actor_buffer);
                spec->type_info.ghost.count_actors = gShrapnel_ghost_actor_count;
                spec->type_info.ghost.actors = BrMemAllocate(gShrapnel_ghost_actor_count * sizeof(*spec->type_info.ghost.actors), kMem_smashable_env_info);
                for (j = 0; j < spec->type_info.ghost.count_actors; j++) {
                    BrActorRemove(actor_buffer[j]);
                    spec->type_info.ghost.actors[j] = actor_buffer[j];
                }
                BrActorFree(actor);
            }
            break;
        case kShrapnelType_NonCars: {
            br_actor* actor;

            GetPairOfInts(pF, &spec->type_info.noncar.amount_min, &spec->type_info.noncar.amount_max);
            /* % Chance of fire/smoke */
            spec->type_info.noncar.probability_fire = GetAnInt(pF);
            if (spec->type_info.noncar.probability_fire != 0) {
                /* Number of fires/smoke columns */
                spec->type_info.noncar.count_fire_columns = GetAnInt(pF);
                /* Min,Max smokiness (0 = fire, 1 = black smoke, 2 = grey smoke, 3 = white smoke) */
                GetPairOfInts(pF, &spec->type_info.noncar.min_smokiness, &spec->type_info.noncar.max_smokiness);
            }
            GetAString(pF, s);
            actor = ReadShrapnelSpec_LoadActor(s);
            if (actor == NULL) {
                FatalError(kFatalError_CannotFindSmashActorModel_S, s);
            }
            ProcessMaterials(actor, TemporaryMaterialStore);
            spec->type_info.noncar.count_actors = GetAnInt(pF);
            spec->type_info.noncar.actors = BrMemAllocate(spec->type_info.noncar.count_actors * sizeof(*spec->type_info.noncar.actors), kMem_smashable_env_info);
            for (j = 0; j < spec->type_info.noncar.count_actors; j++) {
                br_actor* sep_actor;
                br_actor* match_sep_actor;

                GetAString(pF, s);
                sep_actor = DRActorFindRecurse(actor, s);
                match_sep_actor = sep_actor;
                if (sep_actor == NULL) {
                    FatalError(kFatalError_CannotFindSmashActorModel_S, s);
                }
                for (k = 0; k < gCount_smashable_noncar_shrapnel_actors; k++) {
                    if (strcmp(gSmashable_noncar_shrapnel_actors[j]->identifier, sep_actor->identifier) == 0) {
                        match_sep_actor = gSmashable_noncar_shrapnel_actors[j];
                        break;
                    }
                }
                if (k == gCount_smashable_noncar_shrapnel_actors) {
                    if (gCount_smashable_noncar_shrapnel_actors < (int)CARPOCALYPSE2_ASIZE(gSmashable_noncar_shrapnel_actors)) {
                        gSmashable_noncar_shrapnel_actors[gCount_smashable_noncar_shrapnel_actors] = match_sep_actor;
                        gCount_smashable_noncar_shrapnel_actors += 1;
                    }
                    if (match_sep_actor != actor) {
                        BrActorRemove(match_sep_actor);
                    }
                }
                spec->type_info.noncar.actors[j] = match_sep_actor;
                GetAString(pF, s);
                for (k = 0, noncar_name = gSmashable_noncars; k < gCount_smashable_noncars; k++, noncar_name++) {
                    if (strcmp(s, *noncar_name) == 0) {
                        break;
                    }
                }
                if (k == gCount_smashable_noncars) {
                    strcpy(*noncar_name, s);
                    gCount_smashable_noncars += 1;
                }
                BrResFree(match_sep_actor->identifier);
                sprintf(actor_name, "&%c%c!bbbb.ACT", s[0], s[1]);
                match_sep_actor->identifier = BrResStrDup(match_sep_actor, actor_name);
            }
            if (spec->type_info.noncar.count_actors != 1 || spec->type_info.noncar.actors[0] != actor) {
                BrActorFree(actor);
            }
            break;
        }
        default:
            break;
        }
    }
}

// FUNCTION: CARMA2_HW 0x004ef550
void C2_NAKED C2_HOOK_FASTCALL ReadShrapnelSideEffects(FILE* pF, tShrapnel_side_effects* pShrapnel_side_effects) {
#ifndef CARPOCALYPSE2_MATCHING

    /* Count smash activation cuboids */
    pShrapnel_side_effects->count_side_effects = GetAnInt(pF);
    if (pShrapnel_side_effects->count_side_effects != 0) {
        int i;

        C2_HOOK_BUG_ON(sizeof(tSmash_side_effect) != 0x4c);
#if 0
        /* FIXME: remove once we found something */
        PDFatalError("Non-zero non shash side effect detected! Notify carpocalypse2 developers!");
#endif
        pShrapnel_side_effects->side_effects = BrMemAllocate(pShrapnel_side_effects->count_side_effects * sizeof(tSmash_side_effect), kMem_smash_side_effects);
        for (i = 0; i < pShrapnel_side_effects->count_side_effects; i++) {
            int i1, i2;
            char s[256];

            GetPairOfInts(pF, &i1, &i2);
            pShrapnel_side_effects->side_effects[i].field_0x0 = 1000 * i1;
            pShrapnel_side_effects->side_effects[i].field_0x2 = 1000 * i2;

            GetAString(pF, s);
            strcpy(pShrapnel_side_effects->side_effects[i].field_0x8, s);
            if (strcmp(s, "*") == 0) {
                pShrapnel_side_effects->side_effects[i].field_0x28 = 0;
            } else if (s[0] == '*' && DRStricmp(s + strlen(s) - 4, ".MAT") == 0) {
                pShrapnel_side_effects->side_effects[i].field_0x28 = 1;
            } else if (DRStricmp(s + strlen(s) - 4, ".DAT") == 0 || DRStricmp(s + strlen(s) - 4, ".ACT") == 0) {
                pShrapnel_side_effects->side_effects[i].field_0x28 = 2;
            } else if (DRStricmp(s, "&*") == 0) {
                pShrapnel_side_effects->side_effects[i].field_0x28 = 3;
            } else if (s[0] == '&' && '0' <= s[1] && s[1] <= '9') {
                pShrapnel_side_effects->side_effects[i].field_0x28 = 6;
            } else if (DRStricmp(s + strlen(s) - 4, ".MAT") == 0) {
                pShrapnel_side_effects->side_effects[i].field_0x28 = 4;
            } else if (DRStricmp(s + strlen(s) - 4, ".DAT") == 0 || DRStricmp(s + strlen(s) - 4, ".ACT") == 0) {
                pShrapnel_side_effects->side_effects[i].field_0x28 = 5;
            } else {
                pShrapnel_side_effects->side_effects[i].field_0x28 = 0;
            }
            pShrapnel_side_effects->side_effects[i].field_0x4 = GetALineAndInterpretCommand(pF, gPosition_type_names, CARPOCALYPSE2_ASIZE(gPosition_type_names));
            LoadMinMax(pF, &pShrapnel_side_effects->side_effects[i].bounds);
            pShrapnel_side_effects->side_effects[i].field_0x44 = GetALineAndInterpretCommand(pF, gSmash_side_effect_direction_names, CARPOCALYPSE2_ASIZE(gSmash_side_effect_direction_names));
            pShrapnel_side_effects->side_effects[i].field_0x48 = GetAScalar(pF);
        }
    }
#else
    __asm {
            sub esp, 0x110
            push ebx
            push ebp
            push esi
            mov esi, ecx
            mov ebp, edx
            mov dword ptr [esp + 0xc], esi
            call GetAnInt
            xor ebx, ebx
            mov dword ptr [ebp], eax
            cmp eax, ebx
            je L004ef826
            lea ecx, [eax + eax*8]
            push 0x9e
            lea edx, [eax + ecx*2]
            shl edx, 2
            push edx
            call BrMemAllocate
            mov dword ptr [ebp + 4], eax
            mov eax, dword ptr [ebp]
            add esp, 8
            cmp eax, ebx
            mov dword ptr [esp + 0x10], ebx
            jle L004ef826
            push edi
            jmp L004ef5a3
    L004ef59f:
            mov esi, dword ptr [esp + 0x10]
    L004ef5a3:
            lea eax, [esp + 0x1c]
            lea edx, [esp + 0x18]
            push eax
            mov ecx, esi
            call GetPairOfInts
            mov edx, dword ptr [ebp + 4]
            mov eax, dword ptr [esp + 0x18]
            lea eax, [eax + eax*4]
            lea eax, [eax + eax*4]
            lea ecx, [eax + eax*4]
            shl ecx, 3
            mov word ptr [ebx + edx], cx
            mov eax, dword ptr [esp + 0x1c]
            mov ecx, dword ptr [ebp + 4]
            lea edx, [esp + 0x20]
            lea eax, [eax + eax*4]
            lea eax, [eax + eax*4]
            lea eax, [eax + eax*4]
            shl eax, 3
            mov word ptr [ebx + ecx + 2], ax
            mov ecx, esi
            call GetAString
            lea edi, [esp + 0x20]
            or ecx, 0xffffffff
            xor eax, eax
            mov edx, dword ptr [ebp + 4]
            repne scasb
            not ecx
            sub edi, ecx
            lea edx, [ebx + edx + 8]
            mov eax, ecx
            mov esi, edi
            mov edi, edx
            shr ecx, 2
            rep movsd
            mov ecx, eax
            and ecx, 3
            rep movsb
            mov edi, offset gOpponent_nastyness_frigger+0x3bc
            lea esi, [esp + 0x20]
    L004ef61e:
            mov cl, byte ptr [esi]
            mov dl, byte ptr [edi]
            mov al, cl
            cmp cl, dl
            jne L004ef646
            test al, al
            je L004ef642
            mov dl, byte ptr [esi + 1]
            mov cl, byte ptr [edi + 1]
            mov al, dl
            cmp dl, cl
            jne L004ef646
            add esi, 2
            add edi, 2
            test al, al
            jne L004ef61e
    L004ef642:
            xor eax, eax
            jmp L004ef64b
    L004ef646:
            sbb eax, eax
            sbb eax, -1
    L004ef64b:
            test eax, eax
            jne L004ef65f
            mov eax, dword ptr [ebp + 4]
            mov dword ptr [ebx + eax + 0x28], 0
            jmp L004ef7c4
    L004ef65f:
            cmp byte ptr [esp + 0x20], 0x2a
            jne L004ef6f9
            lea edi, [esp + 0x20]
            or ecx, 0xffffffff
            xor eax, eax
            mov edx, offset gMaterial_names+0x70
            repne scasb
            not ecx
            lea eax, [esp + 0x20]
            dec ecx
            sub eax, 4
            add ecx, eax
            call DRStricmp
            test eax, eax
            jne L004ef69f
            mov ecx, dword ptr [ebp + 4]
            mov dword ptr [ebx + ecx + 0x28], 1
            jmp L004ef7c4
    L004ef69f:
            cmp byte ptr [esp + 0x20], 0x2a
            jne L004ef6f9
            lea esi, [esp + 0x20]
            lea edi, [esp + 0x20]
            or ecx, 0xffffffff
            xor eax, eax
            sub esi, 4
            mov edx, offset gFrontend_QUIT+0xbb7c
            repne scasb
            not ecx
            dec ecx
            add ecx, esi
            call DRStricmp
            test eax, eax
            je L004ef6e9
            lea edi, [esp + 0x20]
            or ecx, 0xffffffff
            xor eax, eax
            mov edx, offset gDefault_drone_direction+0x6dc
            repne scasb
            not ecx
            dec ecx
            add ecx, esi
            call DRStricmp
            test eax, eax
            jne L004ef6f9
    L004ef6e9:
            mov edx, dword ptr [ebp + 4]
            mov dword ptr [ebx + edx + 0x28], 2
            jmp L004ef7c4
    L004ef6f9:
            mov edx, offset gSplit_triangle_vertex_orders+0x198
            lea ecx, [esp + 0x20]
            call DRStricmp
            test eax, eax
            jne L004ef71b
            mov eax, dword ptr [ebp + 4]
            mov dword ptr [ebx + eax + 0x28], 3
            jmp L004ef7c4
    L004ef71b:
            cmp byte ptr [esp + 0x20], 0x26
            jne L004ef73e
            mov al, byte ptr [esp + 0x21]
            cmp al, 0x30
            jl L004ef73e
            cmp al, 0x39
            jg L004ef73e
            mov ecx, dword ptr [ebp + 4]
            mov dword ptr [ebx + ecx + 0x28], 6
            jmp L004ef7c4
    L004ef73e:
            lea esi, [esp + 0x20]
            lea edi, [esp + 0x20]
            or ecx, 0xffffffff
            xor eax, eax
            sub esi, 4
            mov edx, offset gMaterial_names+0x70
            repne scasb
            not ecx
            dec ecx
            add ecx, esi
            call DRStricmp
            test eax, eax
            jne L004ef770
            mov edx, dword ptr [ebp + 4]
            mov dword ptr [ebx + edx + 0x28], 4
            jmp L004ef7c4
    L004ef770:
            lea edi, [esp + 0x20]
            or ecx, 0xffffffff
            xor eax, eax
            mov edx, offset gFrontend_QUIT+0xbb7c
            repne scasb
            not ecx
            dec ecx
            add ecx, esi
            call DRStricmp
            test eax, eax
            je L004ef7b9
            lea edi, [esp + 0x20]
            or ecx, 0xffffffff
            xor eax, eax
            mov edx, offset gDefault_drone_direction+0x6dc
            repne scasb
            not ecx
            dec ecx
            add ecx, esi
            call DRStricmp
            test eax, eax
            je L004ef7b9
            mov eax, dword ptr [ebp + 4]
            mov dword ptr [ebx + eax + 0x28], 0
            jmp L004ef7c4
    L004ef7b9:
            mov ecx, dword ptr [ebp + 4]
            mov dword ptr [ebx + ecx + 0x28], 5
    L004ef7c4:
            mov esi, dword ptr [esp + 0x10]
            push 2
            mov edx, offset gPosition_type_names
            mov ecx, esi
            call GetALineAndInterpretCommand
            mov edx, dword ptr [ebp + 4]
            mov ecx, esi
            mov dword ptr [ebx + edx + 4], eax
            mov eax, dword ptr [ebp + 4]
            lea edx, [ebx + eax + 0x2c]
            call LoadMinMax
            mov edx, offset gSmash_side_effect_direction_names
            mov ecx, esi
            push 3
            call GetALineAndInterpretCommand
            mov ecx, dword ptr [ebp + 4]
            mov dword ptr [ebx + ecx + 0x44], eax
            mov ecx, esi
            call GetAScalar
            mov edx, dword ptr [ebp + 4]
            mov eax, dword ptr [esp + 0x14]
            inc eax
            fstp dword ptr [ebx + edx + 0x48]
            mov ecx, dword ptr [ebp]
            add ebx, 0x4c
            cmp eax, ecx
            mov dword ptr [esp + 0x14], eax
            jl L004ef59f
            pop edi
    L004ef826:
            pop esi
            pop ebp
            pop ebx
            add esp, 0x110
            ret
    }
#endif
}

// FUNCTION: CARMA2_HW 0x004efce0
void C2_NAKED C2_HOOK_FASTCALL ReadNonCarCuboidActivation(FILE* pF, tNon_car_cuboid_activations* pNon_car_cuboid_activations) {
#ifndef CARPOCALYPSE2_MATCHING

    /* no. of non car cuboids activated */
    pNon_car_cuboid_activations->count_activations = GetAnInt(pF);
    if (pNon_car_cuboid_activations->count_activations != 0) {
        int i;

        C2_HOOK_BUG_ON(sizeof(tNon_car_cuboid_activation) != 0x40);

#if 1
        /* FIXME: remove once we found something */
        PDFatalError("Non-zero non car cuboid activation detected! Notify carpocalypse2 developers!");
#endif
        pNon_car_cuboid_activations->activations = BrMemAllocate(pNon_car_cuboid_activations->count_activations * sizeof(tNon_car_cuboid_activation), kMem_smash_side_effects);
        for (i = 0; i < pNon_car_cuboid_activations->count_activations; i++) {
            int i1, i2;

            GetPairOfInts(pF, &i1, &i2);
            pNon_car_cuboid_activations->activations[i].field_0x0 = (tS16)(i1 * 1000);
            pNon_car_cuboid_activations->activations[i].field_0x2 = (tS16)(i2 * 1000);
            pNon_car_cuboid_activations->activations[i].field_0x4 = GetALineAndInterpretCommand(pF, gPosition_type_names, CARPOCALYPSE2_ASIZE(gPosition_type_names));
            pNon_car_cuboid_activations->activations[i].field_0x8 = (tS8)GetAnInt(pF);
            LoadMinMax(pF, &pNon_car_cuboid_activations->activations[i].bounds);
            GetPairOfFloats(pF,
                &pNon_car_cuboid_activations->activations[i].field_0x24,
                &pNon_car_cuboid_activations->activations[i].field_0x28);
            pNon_car_cuboid_activations->activations[i].field_0x2c = GetAScalar(pF);
            pNon_car_cuboid_activations->activations[i].field_0x30 = GetAScalar(pF);
            pNon_car_cuboid_activations->activations[i].field_0x34 = GetAScalar(pF);
            pNon_car_cuboid_activations->activations[i].field_0x38 = GetAScalar(pF);
            pNon_car_cuboid_activations->activations[i].field_0x3c = GetAScalar(pF) / 10.f;
        }
    }
#else
    __asm {
            sub esp, 0xc
            push ebx
            push ebp
            push esi
            mov ebx, edx
            mov esi, ecx
            call GetAnInt
            xor ebp, ebp
            mov dword ptr [ebx], eax
            cmp eax, ebp
            je L004efdef
            shl eax, 6
            push 0x9e
            push eax
            call BrMemAllocate
            mov dword ptr [ebx + 4], eax
            mov eax, dword ptr [ebx]
            add esp, 8
            cmp eax, ebp
            mov dword ptr [esp + 0xc], ebp
            jle L004efdef
            push edi
    L004efd1e:
            lea eax, [esp + 0x18]
            lea edx, [esp + 0x14]
            push eax
            mov ecx, esi
            call GetPairOfInts
            mov edx, dword ptr [ebx + 4]
            mov eax, dword ptr [esp + 0x14]
            push 2
            lea eax, [eax + eax*4]
            lea eax, [eax + eax*4]
            lea ecx, [eax + eax*4]
            shl ecx, 3
            mov word ptr [edx + ebp], cx
            mov eax, dword ptr [esp + 0x1c]
            mov ecx, dword ptr [ebx + 4]
            mov edx, offset gPosition_type_names
            lea eax, [eax + eax*4]
            lea eax, [eax + eax*4]
            lea eax, [eax + eax*4]
            shl eax, 3
            mov word ptr [ecx + ebp + 2], ax
            mov ecx, esi
            call GetALineAndInterpretCommand
            mov edx, dword ptr [ebx + 4]
            mov ecx, esi
            mov dword ptr [edx + ebp + 4], eax
            call GetAnInt
            mov ecx, dword ptr [ebx + 4]
            mov byte ptr [ecx + ebp + 8], al
            mov edx, dword ptr [ebx + 4]
            mov ecx, esi
            lea edx, [edx + ebp + 0xc]
            call LoadMinMax
            mov eax, dword ptr [ebx + 4]
            lea edi, [eax + ebp + 0x24]
            mov edx, edi
            lea ecx, [edi + 4]
            push ecx
            mov ecx, esi
            call GetPairOfFloats
            mov ecx, esi
            call GetAScalar
            fstp dword ptr [edi + 8]
            mov ecx, esi
            call GetAScalar
            fstp dword ptr [edi + 0xc]
            mov ecx, esi
            call GetAScalar
            fstp dword ptr [edi + 0x10]
            mov ecx, esi
            call GetAScalar
            fstp dword ptr [edi + 0x14]
            mov ecx, esi
            call GetAScalar
            fmul dword ptr [gDOUBLE_0058af40+0x8]
            mov eax, dword ptr [esp + 0x10]
            add ebp, 0x40
            inc eax
            mov dword ptr [esp + 0x10], eax
            fstp dword ptr [edi + 0x18]
            cmp eax, dword ptr [ebx]
            jl L004efd1e
            pop edi
    L004efdef:
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
    }
#endif
}