#include "brucetrk.h"

#include "errors.h"
#include "globvars.h"
#include "globvrpb.h"
#include "init.h"
#include "loading.h"
#include "pedestrn.h"
#include "powerups.h"
#include "platform.h"
#include "smashing.h"
#include "utility.h"
#include "world.h"

#include "c2_string.h"

#include "carpocalypse2_macros.h"
#include "brucetrk.h"
#include "carpocalypse2_types.h"
// GLOBAL: CARMA2_HW 0x00655e60
extern br_scalar gYon_factor;
// GLOBAL: CARMA2_HW 0x0079efac
int gMax_count_non_cars;

// GLOBAL: CARMA2_HW 0x00679260
extern int gCount_track_non_cars;

// GLOBAL: CARMA2_HW 0x0058f490
int gRender_alternative_track_actors = 1;

// GLOBAL: CARMA2_HW 0x00679264
extern br_actor* gMr_blendy;

// GLOBAL: CARMA2_HW 0x00655e60
br_scalar gYon_factor = 0.25f;

// GLOBAL: CARMA2_HW 0x0074b74c
br_material* gDefault_track_material;

#ifdef CARPOCALYPSE2_MATCHING
#endif
#ifdef CARPOCALYPSE2_MATCHING
#endif
#ifdef CARPOCALYPSE2_MATCHING
extern br_actor* C2_HOOK_CDECL BrActorAllocate();
extern br_model* C2_HOOK_CDECL BrModelAllocate();
extern void C2_HOOK_CDECL BrModelUpdate();
extern int C2_HOOK_FASTCALL SmashFaceMustBeUpdateable(br_material *pMaterial);
extern void C2_HOOK_FASTCALL SpawnPedsOnFace(br_face *pFace, br_model *pModel);
extern br_uint_8 bl;
#endif
#ifdef CARPOCALYPSE2_MATCHING
extern br_actor* C2_HOOK_CDECL BrActorAdd();
extern br_uint_32 C2_HOOK_CDECL BrActorEnum();
extern void C2_HOOK_CDECL BrActorRelink_library();
extern br_bounds* C2_HOOK_CDECL BrActorToBounds();
extern C2_NORETURN void C2_HOOK_CDECL BrFailure();
extern void C2_HOOK_CDECL BrMatrix34ApplyV();
extern void* C2_HOOK_CDECL BrMemAllocate();
extern br_model* C2_HOOK_CDECL BrModelAdd();
extern void* C2_HOOK_CDECL BrResAllocate();
extern void C2_HOOK_CDECL BrResFree();
extern void C2_HOOK_CDECL BrZbsSceneRenderAdd();
extern void C2_HOOK_FASTCALL DRModelUpdateAndKevificateMaterials(br_model* pModel, br_uint_16 pFlags);
extern void C2_NORETURN C2_HOOK_FASTCALL PDFatalError(const char* pThe_str);
extern void C2_HOOK_FASTCALL SetChangingPowerup(br_actor* pActor);
extern void C2_HOOK_FASTCALL SetSmashableModel(br_actor* pActor);
extern void C2_HOOK_FASTCALL SetSpinningPowerup(br_actor* pActor, int pOpacity);
extern void C2_HOOK_FASTCALL SmoothificatePowerups(br_actor* pActor);
extern void __cdecl _ftol();
extern void C2_HOOK_CDECL dr_dprintf();
extern int gAusterity_mode;
extern char gFloat_0058935c[];
#endif
// FUNCTION: CARMA2_HW 0x0040d530
void C2_NAKED C2_HOOK_FASTCALL MungeFaces(br_actor* pActor, br_model* pModel) {
#ifndef CARPOCALYPSE2_MATCHING
    int i;
    int j;
    int k;
    br_face* face;
    br_face* blendy_face;
    int changed_one;

    changed_one = 0;

    for (i = 0; i < pModel->nfaces; i++) {
        face = &pModel->faces[i];

        SpawnPedsOnFace(face, pModel);
        if (SmashFaceMustBeUpdateable(face->material)) {
            if (gMr_blendy == NULL) {
                gMr_blendy = BrActorAllocate(BR_ACTOR_MODEL, NULL);
                gMr_blendy->render_style = BR_RSTYLE_FACES;
                gMr_blendy->model = BrModelAllocate(NULL, 2000, 2000);
                gMr_blendy->model->nfaces = 0;
                gMr_blendy->model->nvertices = 0;
                gMr_blendy->model->flags |= BR_MODF_UPDATEABLE;
            }
            blendy_face = &gMr_blendy->model->faces[gMr_blendy->model->nfaces];
            gMr_blendy->model->nfaces += 1;
            memcpy(blendy_face, face, sizeof(br_face));
            for (j = 0; j < 3; j++) {
                br_vertex *vertex_j = &pModel->vertices[face->vertices[j]];
                for (k = 0; k < gMr_blendy->model->nvertices; k++) {
                    if (Vector3Equals(&gMr_blendy->model->vertices[k].p, &vertex_j->p)
                            && Vector2Equals(&gMr_blendy->model->vertices[k].map, &vertex_j->map)) {
                        blendy_face->vertices[j] = k;
                        vertex_j = NULL;
                        break;
                    }
                }
                if (vertex_j != NULL) {
                    blendy_face->vertices[j] = gMr_blendy->model->nvertices;
                    memcpy(&gMr_blendy->model->vertices[gMr_blendy->model->nvertices], vertex_j, sizeof(br_vertex));
                    gMr_blendy->model->nvertices += 1;
                }
            }
            if (i < pModel->nfaces - 1) {
                memmove(&pModel->faces[i], &pModel->faces[i + 1], (pModel->nfaces - (i + 1)) * sizeof(br_vertex));
            }
            pModel->nfaces -= 1;
            i -= 1;
            changed_one = 1;
        }
    }
    if (changed_one) {
        if (pModel->nfaces != 0) {
            BrModelUpdate(pModel, BR_MODU_ALL);
        } else {
            pActor->model = NULL;
            pActor->render_style = BR_RSTYLE_NONE;
        }
    }
#else
    __asm {
            sub esp, 0x1c
            push ebx
            push ebp
            push esi
            mov esi, edx
            push edi
            xor ebx, ebx
            mov ebp, dword ptr [esi + 0xc]
            xor edi, edi
            cmp word ptr [esi + 0x12], bx
            mov dword ptr [esp + 0x1c], esi
            mov dword ptr [esp + 0x28], ecx
            mov dword ptr [esp + 0x10], ebx
            jbe L0040d789
            mov dword ptr [esp + 0x14], ebx
            mov dword ptr [esp + 0x18], 0x28
    L0040d562:
            mov edx, esi
            mov ecx, ebp
            call SpawnPedsOnFace
            mov ecx, dword ptr [ebp + 8]
            mov edx, esi
            call SmashFaceMustBeUpdateable
            test eax, eax
            je L0040d753
            cmp dword ptr [gMr_blendy], ebx
            jne L0040d5db
            push ebx
            push 1
            call BrActorAllocate
            add esp, 8
            mov dword ptr [gMr_blendy], eax
            mov byte ptr [eax + 0x20], 4
            push 0x7d0
            push 0x7d0
            push ebx
            call BrModelAllocate
            mov ecx, dword ptr [gMr_blendy]
            add esp, 0xc
            mov dword ptr [ecx + 0x18], eax
            mov edx, dword ptr [gMr_blendy]
            mov eax, dword ptr [edx + 0x18]
            mov word ptr [eax + 0x12], bx
            mov ecx, dword ptr [gMr_blendy]
            mov edx, dword ptr [ecx + 0x18]
            mov word ptr [edx + 0x10], bx
            mov eax, dword ptr [gMr_blendy]
            mov eax, dword ptr [eax + 0x18]
            or byte ptr [eax + 0x20], 0x80
    L0040d5db:
            mov ecx, dword ptr [gMr_blendy]
            mov dword ptr [esp + 0x20], 3
            mov ecx, dword ptr [ecx + 0x18]
            mov dx, word ptr [ecx + 0x12]
            mov esi, dword ptr [ecx + 0xc]
            mov eax, edx
            and eax, 0xffff
            inc edx
            mov word ptr [ecx + 0x12], dx
            mov ecx, 0xa
            lea eax, [eax + eax*4]
            lea eax, [esi + eax*8]
            mov esi, ebp
            mov edi, eax
            sub ebp, eax
            rep movsd
            mov ebx, eax
            mov dword ptr [esp + 0x24], ebp
            jmp L0040d61e
    L0040d61a:
            mov ebp, dword ptr [esp + 0x24]
    L0040d61e:
            xor eax, eax
            xor edi, edi
            mov ax, word ptr [ebp + ebx]
            mov ebp, dword ptr [esp + 0x1c]
            mov edx, dword ptr [ebp + 8]
            lea ecx, [eax + eax*4]
            mov eax, dword ptr [gMr_blendy]
            lea esi, [edx + ecx*8]
            xor ecx, ecx
            mov eax, dword ptr [eax + 0x18]
            mov cx, word ptr [eax + 0x10]
            mov edx, dword ptr [eax + 8]
            test ecx, ecx
            jle L0040d697
    L0040d649:
            fld dword ptr [edx]
            fcomp dword ptr [esi]
            fnstsw ax
            test ah, 0x40
            je L0040d688
            fld dword ptr [edx + 4]
            fcomp dword ptr [esi + 4]
            fnstsw ax
            test ah, 0x40
            je L0040d688
            fld dword ptr [edx + 8]
            fcomp dword ptr [esi + 8]
            fnstsw ax
            test ah, 0x40
            je L0040d688
            fld dword ptr [edx + 0xc]
            fcomp dword ptr [esi + 0xc]
            fnstsw ax
            test ah, 0x40
            je L0040d688
            fld dword ptr [edx + 0x10]
            fcomp dword ptr [esi + 0x10]
            fnstsw ax
            test ah, 0x40
            jne L0040d692
    L0040d688:
            inc edi
            add edx, 0x28
            cmp edi, ecx
            jl L0040d649
            jmp L0040d697
    L0040d692:
            mov word ptr [ebx], di
            xor esi, esi
    L0040d697:
            test esi, esi
            je L0040d6d7
            mov ecx, dword ptr [gMr_blendy]
            mov edx, dword ptr [ecx + 0x18]
            mov ax, word ptr [edx + 0x10]
            mov word ptr [ebx], ax
            mov ecx, dword ptr [gMr_blendy]
            xor eax, eax
            mov ecx, dword ptr [ecx + 0x18]
            mov ax, word ptr [ecx + 0x10]
            lea edx, [eax + eax*4]
            mov eax, dword ptr [ecx + 8]
            mov ecx, 0xa
            lea edi, [eax + edx*8]
            rep movsd
            mov ecx, dword ptr [gMr_blendy]
            mov eax, dword ptr [ecx + 0x18]
            inc word ptr [eax + 0x10]
    L0040d6d7:
            mov eax, dword ptr [esp + 0x20]
            add ebx, 2
            dec eax
            mov dword ptr [esp + 0x20], eax
            jne L0040d61a
            mov ecx, dword ptr [esp + 0x10]
            xor eax, eax
            mov ax, word ptr [ebp + 0x12]
            lea edx, [eax - 1]
            cmp ecx, edx
            jge L0040d71e
            mov ecx, dword ptr [ebp + 0xc]
            lea eax, [eax + eax*4]
            shl eax, 3
            mov edx, eax
            mov eax, dword ptr [esp + 0x18]
            sub edx, eax
            add eax, ecx
            push edx
            push eax
            mov eax, dword ptr [esp + 0x1c]
            add eax, ecx
            push eax
            call memmove
            add esp, 0xc
    L0040d71e:
            mov ecx, dword ptr [esp + 0x18]
            dec word ptr [ebp + 0x12]
            mov ebp, dword ptr [esp + 0x14]
            mov esi, dword ptr [esp + 0x10]
            sub ecx, 0x28
            sub ebp, 0x28
            mov dword ptr [esp + 0x18], ecx
            mov ecx, dword ptr [esp + 0x1c]
            dec esi
            mov dword ptr [esp + 0x14], ebp
            mov eax, dword ptr [ecx + 0xc]
            mov dword ptr [esp + 0x10], esi
            add ebp, eax
            mov edi, 1
            xor ebx, ebx
            mov esi, ecx
    L0040d753:
            mov edx, dword ptr [esp + 0x14]
            mov ecx, 0x28
            mov eax, dword ptr [esp + 0x10]
            add edx, ecx
            mov dword ptr [esp + 0x14], edx
            mov edx, dword ptr [esp + 0x18]
            add edx, ecx
            inc eax
            mov dword ptr [esp + 0x18], edx
            xor edx, edx
            mov dx, word ptr [esi + 0x12]
            add ebp, ecx
            cmp eax, edx
            mov dword ptr [esp + 0x10], eax
            jl L0040d562
            mov ecx, dword ptr [esp + 0x28]
    L0040d789:
            cmp edi, ebx
            je L0040d7b0
            cmp word ptr [esi + 0x12], bx
            je L0040d7a9
            push 0x7fff
            push esi
            call BrModelUpdate
            add esp, 8
            pop edi
            pop esi
            pop ebp
            pop ebx
            add esp, 0x1c
            ret
    L0040d7a9:
            mov dword ptr [ecx + 0x18], ebx
            mov byte ptr [ecx + 0x20], 1
    L0040d7b0:
            pop edi
            pop esi
            pop ebp
            pop ebx
            add esp, 0x1c
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

// FUNCTION: CARMA2_HW 0x0040d1f0
intptr_t C2_NAKED C2_HOOK_CDECL FindNonCarsCB(br_actor* pActor, void* pData) {
#ifndef CARPOCALYPSE2_MATCHING
    char s[256];
    tTrack_spec* pTrack_spec = pData;

    SetSmashableModel(pActor);
    if (pActor->identifier[0] == '&' && pActor->model == NULL) {
        sprintf(s, "Accessory %s has no model. Well how does it pose? Awful!", pActor->identifier);
        PDFatalError(s);
    }
    if (pActor->identifier[0] == '&' && '0' <= pActor->identifier[1] && pActor->identifier[1] <= '9') {
        int id;
        br_scalar n0;
        br_scalar n1;
        br_scalar n2;

        id = TEXT_TO_NUMBER_4(&pActor->identifier[4]);
        if (id < 0 || id >= pTrack_spec->count_non_cars) {
            return 1;
        }
        n0 = BrVector3LengthSquared((br_vector3*)pActor->t.t.mat.m[0]);
        n1 = BrVector3LengthSquared((br_vector3*)pActor->t.t.mat.m[1]);
        n2 = BrVector3LengthSquared((br_vector3*)pActor->t.t.mat.m[2]);
        if (n0 < 0.999f || n1 < 0.999f || n2 < 0.999f) {
            dr_dprintf("non car was scaled down %s", pActor->identifier);
            pActor->t.t.translate.t.v[0] += 2000.f;
        }
        if (n0 > 1.001f || n1 > 1.001f || n2 > 1.001f) {
            BrVector3InvScale((br_vector3*)pActor->t.t.mat.m[0],
                (br_vector3*)pActor->t.t.mat.m[0],
                (float)sqrt((double)n0));
            BrVector3InvScale((br_vector3*)pActor->t.t.mat.m[1],
                (br_vector3*)pActor->t.t.mat.m[1],
                (float)sqrt((double)n1));
            BrVector3InvScale((br_vector3*)pActor->t.t.mat.m[2],
                (br_vector3*)pActor->t.t.mat.m[2],
                (float)sqrt((double)n2));
            dr_dprintf("non car was scaled up %s", pActor->identifier);
        }
        pTrack_spec->non_car_list[id] = pActor;
        pActor->type_data = NULL;
        return 0;
    }
    if (pActor != NULL && pActor->identifier != NULL && pActor->identifier[1] == '\xa3') {
        int id;

        SmoothificatePowerups(pActor);
        id = TEXT_TO_NUMBER_2(&pActor->identifier[2]);
        if (66 <= id && id < 86) {
            SetSpinningPowerup(pActor, 0);
        } else if (86 <= id && id < 88) {
            SetChangingPowerup(pActor);
        }
    }
    return BrActorEnum(pActor, FindNonCarsCB, pTrack_spec);
#else
    __asm {
            sub esp, 0x10c
            push ebx
            push esi
            mov esi, dword ptr [esp + 0x118]
            push edi
            mov ecx, esi
            call SetSmashableModel
            mov eax, dword ptr [esi + 0x14]
            cmp byte ptr [eax], 0x26
            jne L0040d232
            mov ecx, dword ptr [esi + 0x18]
            test ecx, ecx
            jne L0040d232
            push eax
            lea eax, [esp + 0x1c]
            push offset gRender_alternative_track_actors+0x118
            push eax
            call sprintf
            add esp, 0xc
            lea ecx, [esp + 0x18]
            call PDFatalError
    L0040d232:
            mov ecx, dword ptr [esi + 0x14]
            cmp byte ptr [ecx], 0x26
            jne L0040d4bd
            mov al, byte ptr [ecx + 1]
            cmp al, 0x30
            jl L0040d4bd
            cmp al, 0x39
            jg L0040d4bd
            movsx eax, byte ptr [ecx + 4]
            lea edx, [eax + eax*4]
            movsx eax, byte ptr [ecx + 5]
            lea eax, [eax + edx*2]
            lea edx, [eax + eax*4]
            movsx eax, byte ptr [ecx + 6]
            lea eax, [eax + edx*2]
            lea edx, [eax + eax*4]
            movsx eax, byte ptr [ecx + 7]
            lea edi, [eax + edx*2 - 0xd050]
            test edi, edi
            jl L0040d4ae
            mov ebx, dword ptr [esp + 0x120]
            cmp edi, dword ptr [ebx + 0x1c]
            jge L0040d4ae
            fld dword ptr [esi + 0x2c]
            fld dword ptr [esi + 0x30]
            fld dword ptr [esi + 0x34]
            fxch st(2)
            fst dword ptr [esp + 0xc]
            fld st(1)
            fxch st(1)
            fmul dword ptr [esp + 0xc]
            fxch st(1)
            fmul st(0), st(2)
            fxch st(3)
            fst dword ptr [esp + 0xc]
            fmul dword ptr [esp + 0xc]
            fld dword ptr [esi + 0x38]
            fxch st(4)
            faddp st(2), st(0)
            fld dword ptr [esi + 0x3c]
            fld dword ptr [esi + 0x44]
            fld dword ptr [esi + 0x48]
            fxch st(3)
            faddp st(4), st(0)
            fxch st(5)
            fst dword ptr [esp + 0xc]
            fmul dword ptr [esp + 0xc]
            fxch st(3)
            fst dword ptr [esp + 0x14]
            fcomp qword ptr [gFloat_0058935c+0x6c]
            fst dword ptr [esp + 0xc]
            fmul dword ptr [esp + 0xc]
            fxch st(4)
            fst dword ptr [esp + 0xc]
            fmul dword ptr [esp + 0xc]
            fxch st(4)
            faddp st(2), st(0)
            fld dword ptr [esi + 0x40]
            fld dword ptr [esi + 0x4c]
            fxch st(2)
            fst dword ptr [esp + 0xc]
            fmul dword ptr [esp + 0xc]
            fxch st(1)
            fst dword ptr [esp + 0xc]
            fxch st(2)
            fst dword ptr [esp + 0x10]
            fxch st(2)
            fmul dword ptr [esp + 0xc]
            fxch st(1)
            faddp st(5), st(0)
            fxch st(1)
            fmul dword ptr [esp + 0x10]
            fxch st(1)
            faddp st(2), st(0)
            fnstsw ax
            faddp st(3), st(0)
            test ah, 1
            fstp dword ptr [esp + 0x10]
            fxch st(1)
            fstp dword ptr [esp + 0xc]
            fstp st(0)
            jne L0040d35b
            fld dword ptr [esp + 0x10]
            fcomp qword ptr [gFloat_0058935c+0x6c]
            fnstsw ax
            test ah, 1
            jne L0040d35b
            fld dword ptr [esp + 0xc]
            fcomp qword ptr [gFloat_0058935c+0x6c]
            fnstsw ax
            test ah, 1
            je L0040d375
    L0040d35b:
            push ecx
            push offset gRender_alternative_track_actors+0xfc
            call dr_dprintf
            fld dword ptr [esi + 0x50]
            fsub dword ptr [gFloat_0058935c+0x74]
            add esp, 8
            fstp dword ptr [esi + 0x50]
    L0040d375:
            fld dword ptr [esp + 0x14]
            fcomp qword ptr [gFloat_0058935c+0x7c]
            fnstsw ax
            test ah, 0x41
            je L0040d3ac
            fld dword ptr [esp + 0x10]
            fcomp qword ptr [gFloat_0058935c+0x7c]
            fnstsw ax
            test ah, 0x41
            je L0040d3ac
            fld dword ptr [esp + 0xc]
            fcomp qword ptr [gFloat_0058935c+0x7c]
            fnstsw ax
            test ah, 0x41
            jne L0040d495
    L0040d3ac:
            fld dword ptr [esp + 0x14]
            fld dword ptr [esp + 0x10]
            fld dword ptr [esp + 0xc]
            fxch st(2)
            fsqrt
            fxch st(1)
            fsqrt
            fxch st(2)
            fsqrt
            fld st(1)
            fxch st(3)
            fst dword ptr [esp + 0xc]
            fxch st(1)
            fst dword ptr [esp + 0xc]
            fxch st(3)
            fdivr qword ptr [gFloat_0058935c+0x84]
            fxch st(1)
            fdivr qword ptr [gFloat_0058935c+0x84]
            fxch st(3)
            fdivr qword ptr [gFloat_0058935c+0x84]
            fxch st(1)
            fst dword ptr [esp + 0xc]
            fxch st(3)
            fst dword ptr [esp + 0xc]
            fxch st(1)
            fst dword ptr [esp + 0xc]
            fld dword ptr [esi + 0x2c]
            fld dword ptr [esi + 0x30]
            fld dword ptr [esi + 0x34]
            fxch st(6)
            fstp dword ptr [esp + 0x14]
            fld dword ptr [esi + 0x38]
            fxch st(4)
            fstp dword ptr [esp + 0x10]
            fld dword ptr [esi + 0x3c]
            fxch st(3)
            fstp dword ptr [esp + 0xc]
            fld dword ptr [esi + 0x40]
            fxch st(2)
            fmul dword ptr [esp + 0x14]
            fstp dword ptr [esi + 0x2c]
            fld dword ptr [esi + 0x44]
            fxch st(1)
            fmul dword ptr [esp + 0x14]
            fstp dword ptr [esi + 0x30]
            fld dword ptr [esi + 0x48]
            fxch st(6)
            fmul dword ptr [esp + 0x14]
            fstp dword ptr [esi + 0x34]
            fld dword ptr [esi + 0x4c]
            fxch st(4)
            fmul dword ptr [esp + 0x10]
            fstp dword ptr [esi + 0x38]
            fxch st(2)
            fmul dword ptr [esp + 0x10]
            fxch st(1)
            fmul dword ptr [esp + 0x10]
            fxch st(2)
            fmul dword ptr [esp + 0xc]
            fxch st(5)
            fmul dword ptr [esp + 0xc]
            fxch st(3)
            fmul dword ptr [esp + 0xc]
            fxch st(1)
            fstp dword ptr [esi + 0x3c]
            fxch st(1)
            fstp dword ptr [esi + 0x40]
            mov ecx, dword ptr [esi + 0x14]
            fxch st(3)
            fstp dword ptr [esi + 0x44]
            fstp dword ptr [esi + 0x48]
            fxch st(1)
            fstp dword ptr [esi + 0x4c]
            push ecx
            push offset gRender_alternative_track_actors+0xe0
            fstp st(0)
            call dr_dprintf
            add esp, 8
    L0040d495:
            mov edx, dword ptr [ebx + 0x20]
            xor eax, eax
            mov dword ptr [edx + edi*4], esi
            mov dword ptr [esi + 0x5c], 0
            pop edi
            pop esi
            pop ebx
            add esp, 0x10c
            ret
    L0040d4ae:
            mov eax, 1
            pop edi
            pop esi
            pop ebx
            add esp, 0x10c
            ret
    L0040d4bd:
            test esi, esi
            je L0040d50b
            test ecx, ecx
            je L0040d50b
            cmp byte ptr [ecx + 1], 0xa3
            jne L0040d50b
            mov ecx, esi
            call SmoothificatePowerups
            mov eax, dword ptr [esi + 0x14]
            movsx ecx, byte ptr [eax + 2]
            movsx edx, byte ptr [eax + 3]
            lea ecx, [ecx + ecx*4]
            lea edi, [edx + ecx*2 - 0x210]
            cmp edi, 0x42
            jl L0040d4fa
            cmp edi, 0x55
            jg L0040d4fa
            xor edx, edx
            mov ecx, esi
            call SetSpinningPowerup
    L0040d4fa:
            cmp edi, 0x56
            jl L0040d50b
            cmp edi, 0x57
            jg L0040d50b
            mov ecx, esi
            call SetChangingPowerup
    L0040d50b:
            mov eax, dword ptr [esp + 0x120]
            push eax
            push offset FindNonCarsCB
            push esi
            call BrActorEnum
            add esp, 0xc
            pop edi
            pop esi
            pop ebx
            add esp, 0x10c
            ret
    }
#endif
}

// FUNCTION: CARMA2_HW 0x004f5ae0
void C2_HOOK_FASTCALL GetModelTextureArea(br_model* pModel, int* pArea_1, int* pArea_2) {
    int i;

    *pArea_1 = 0;
    *pArea_2 = 0;

    for (i = 0; i < V11MODEL(pModel)->ngroups; i++) {
        int j;
        fmt_vertex* vertices = V11MODEL(pModel)->groups[i].vertices;
        v11face* v11f = V11MODEL(pModel)->groups[i].faces;

        for (j = V11MODEL(pModel)->groups[i].nfaces; j > 0; j--, v11f++) {
            int k;
            br_vector2 min = {{ 10000.f, 10000.f }};
            br_vector2 max = {{ -10000.f, -10000.f }};
            int dx;
            int dy;
            int area;

            for (k = 0; k < 3; k++) {
                fmt_vertex* vertex = vertices + v11f->vertices[k];

                if (vertex->map.v[0] < min.v[0]) {
                    min.v[0] = vertex->map.v[0];
                }
                if (vertex->map.v[0] > max.v[0]) {
                    max.v[0] = vertex->map.v[0];
                }
                if (vertex->map.v[1] < min.v[1]) {
                    min.v[1] = vertex->map.v[1];
                }
                if (vertex->map.v[1] > max.v[1]) {
                    max.v[1] = vertex->map.v[1];
                }
            }
            /* The originals subtract a negative double-precision constant. */
            dy = (int)(max.v[1] - min.v[1] - -0.9);
            dx = (int)(max.v[0] - min.v[0] - -0.9);
            area = dx * dy;
            *pArea_1 += area;
            *pArea_2 += area;
        }
    }
    *pArea_1 += 50;
    *pArea_2 += 50;
    if (*pArea_1 > 250) {
        *pArea_1 = 250;
    }
    if (*pArea_2 > 250) {
        *pArea_2 = 250;
    }
}

// FUNCTION: CARMA2_HW 0x004f5c70
void C2_HOOK_FASTCALL GetGlassFragmentCounts(int* pVisible_count, int* pActive_count) {
    tU32 now;
    tSmash_vertex* fragment;
    int i;

    *pVisible_count = 0;
    *pActive_count = 0;
    now = GetTotalTime();
    fragment = gSmash_glass_fragments;
    for (i = 0; i < 200; i++) {
        if (fragment->end_time != 0) {
            *pActive_count = *pActive_count + 1;
            if (now < fragment->field_0x10) {
                *pVisible_count = *pVisible_count + 1;
            }
        }
        fragment++;
    }
}

// FUNCTION: CARMA2_HW 0x004f5cb0
void C2_HOOK_FASTCALL SetSmashableModel(br_actor* pActor) {
    int i;
    tSmashable_item_spec* spec;

    if (pActor->model == NULL || pActor->model->identifier == NULL) {
        return;
    }
    i = 0;
    if (i < gCount_track_smashable_environment_specs) {
        spec = gTrack_smashable_environment_specs;
        do {
            if (spec->trigger_type == kSmashableTrigger_Model
                    && spec->trigger_object.model == pActor->model) {
                char s[64];
                char s2[64];
                int len;

                strcpy(s, pActor->identifier);
                len = strlen(s);
                if (len < 4) {
                    sprintf(s, "Smash material %s has a name that is less than 4 characters long", pActor->identifier);
                    PDFatalError(s);
                }
                strcpy(s2, &s[len - 4]);
                sprintf(&s[len - 4], "        ");
                sprintf(&s[5], "%cx", '|');
                s[6] = i + 1;
                strcat(s, s2);
                s[11] = '\0';
                if (strlen(pActor->identifier) < 12) {
                    BrResFree(pActor->identifier);
                    pActor->identifier = BrResStrDup(pActor, s);
                } else {
                    strcpy(pActor->identifier, s);
                }

                if ((gCurrent_race.race_spec->race_type >= kRaceType_Smash && (tU16)spec->mode_data.shrapnel.connotations.variable_changes.runtime_changes[0].field_0x2 == gCurrent_race.race_spec->options.smash_and_peds.var_smash_number)
                        || (gNet_mode != eNet_mode_none && gCurrent_net_game->type == eNet_game_type_2 && spec->mode_data.shrapnel.connotations.variable_changes.runtime_changes[0].field_0x2 == 1)) {
                    AddSmashableRaceTarget(pActor->model, pActor, -1);
                }
            }
            i++;
            spec++;
        } while (i < gCount_track_smashable_environment_specs);
    }
}

// FUNCTION: CARMA2_HW 0x0040cf10
intptr_t C2_NAKED C2_HOOK_CDECL ProcessModelsCB(br_actor* pActor, void* data) {
#ifndef CARPOCALYPSE2_MATCHING
    unsigned int x;
    unsigned int z;
    tTrack_spec* pTrack_spec = data;

    if (sscanf(pActor->identifier, "%u%u", &x, &z) == 2 && x < pTrack_spec->ncolumns_x && z < pTrack_spec->ncolumns_z) {
        pActor->material = gDefault_track_material;
        pTrack_spec->columns[z][x].actor_0x0 = BrActorAllocate(BR_ACTOR_NONE, NULL);
        BrActorAdd(pActor->parent, pTrack_spec->columns[z][x].actor_0x0);
        pTrack_spec->columns[z][x].actor_0x4 = pActor;
        pTrack_spec->columns[z][x].actor_0x8 = NULL;
        BrActorRelink_library(pTrack_spec->columns[z][x].actor_0x0, pActor);
        gMr_blendy = NULL;
        if (pActor->model != NULL && !gAusterity_mode) {
            MungeFaces(pActor, pActor->model);
        } else {
            br_actor *child;

            for (child = pActor->children; child != NULL; child = child->next) {
                if (child->identifier[0] != '&') {
                    MungeFaces(child, child->model);
                    break;
                }
            }
        }
        BrActorEnum(pActor, (br_actor_enum_cbfn*)FindNonCarsCB, pTrack_spec);
        if (gMr_blendy != NULL) {
            int area1, area2;
            br_vertex* new_vertices;
            br_face* new_faces;

            BrActorAdd(pTrack_spec->columns[z][x].actor_0x0, gMr_blendy);
            gMr_blendy->model->flags |= BR_MODU_FACES;
            BrModelAdd(gMr_blendy->model);
            GetModelTextureArea(gMr_blendy->model, &area1, &area2);

            new_vertices = BrResAllocate(gMr_blendy->model,
                (gMr_blendy->model->nvertices + area2) * sizeof(br_vertex), BR_MEMORY_VERTICES);
            memcpy(new_vertices, gMr_blendy->model->vertices,
                gMr_blendy->model->nvertices * sizeof(br_vertex));
            BrResFree(gMr_blendy->model->vertices);
            gMr_blendy->model->vertices = new_vertices;

            new_faces = BrResAllocate(gMr_blendy->model,
                (gMr_blendy->model->nfaces + area1) * sizeof(br_face), BR_MEMORY_FACES);
            memcpy(new_faces, gMr_blendy->model->faces, gMr_blendy->model->nfaces * sizeof(br_face));
            BrResFree(gMr_blendy->model->faces);
            gMr_blendy->model->faces = new_faces;

            DRModelUpdateAndKevificateMaterials(gMr_blendy->model, BR_MODU_ALL);
            pTrack_spec->columns[z][x].actor_0x8 = gMr_blendy;
        }
    } else {
        BrActorEnum(pActor, ProcessModelsCB, pTrack_spec);
    }
    SetSmashableModel(pActor);
    return 0;
#else
    __asm {
            sub esp, 0x10
            lea eax, [esp + 4]
            lea ecx, [esp]
            push ebx
            push ebp
            push esi
            mov esi, dword ptr [esp + 0x20]
            push edi
            push eax
            mov edx, dword ptr [esi + 0x14]
            push ecx
            push offset gRender_alternative_track_actors+0xd8
            push edx
            call sscanf
            mov ebp, dword ptr [esp + 0x38]
            add esp, 0x10
            cmp eax, 2
            jne L0040d1cb
            mov ecx, dword ptr [esp + 0x10]
            xor eax, eax
            mov al, byte ptr [ebp]
            cmp ecx, eax
            jae L0040d1cb
            mov eax, dword ptr [esp + 0x14]
            xor ecx, ecx
            mov cl, byte ptr [ebp + 1]
            cmp eax, ecx
            jae L0040d1cb
            mov edx, dword ptr [gDefault_track_material]
            xor edi, edi
            push edi
            push edi
            mov dword ptr [esi + 0x1c], edx
            call BrActorAllocate
            mov edx, dword ptr [ebp + 0x18]
            mov ecx, dword ptr [esp + 0x18]
            mov ebx, dword ptr [esp + 0x1c]
            add esp, 8
            lea ecx, [ecx + ecx*2]
            mov edx, dword ptr [edx + ebx*4]
            mov dword ptr [edx + ecx*4], eax
            mov eax, dword ptr [esp + 0x10]
            mov ecx, dword ptr [ebp + 0x18]
            lea edx, [eax + eax*2]
            mov eax, dword ptr [esp + 0x14]
            mov ecx, dword ptr [ecx + eax*4]
            mov eax, dword ptr [esi + 0xc]
            mov edx, dword ptr [ecx + edx*4]
            push edx
            push eax
            call BrActorAdd
            mov eax, dword ptr [esp + 0x18]
            mov ecx, dword ptr [ebp + 0x18]
            add esp, 8
            lea edx, [eax + eax*2]
            mov eax, dword ptr [esp + 0x14]
            push esi
            mov ecx, dword ptr [ecx + eax*4]
            mov dword ptr [ecx + edx*4 + 4], esi
            mov edx, dword ptr [ebp + 0x18]
            mov eax, dword ptr [esp + 0x14]
            mov ecx, dword ptr [esp + 0x18]
            mov edx, dword ptr [edx + ecx*4]
            lea eax, [eax + eax*2]
            mov dword ptr [edx + eax*4 + 8], edi
            mov eax, dword ptr [esp + 0x14]
            mov ecx, dword ptr [ebp + 0x18]
            lea edx, [eax + eax*2]
            mov eax, dword ptr [esp + 0x18]
            mov ecx, dword ptr [ecx + eax*4]
            mov edx, dword ptr [ecx + edx*4]
            push edx
            call BrActorRelink_library
            mov dword ptr [gMr_blendy], edi
            mov edx, dword ptr [esi + 0x18]
            add esp, 8
            cmp edx, edi
            je L0040d010
            cmp dword ptr [gAusterity_mode], edi
            jne L0040d010
            mov ecx, esi
            jmp L0040d02c
    L0040d010:
            mov ecx, dword ptr [esi + 8]
            cmp ecx, edi
            je L0040d031
    L0040d017:
            mov eax, dword ptr [ecx + 0x14]
            cmp byte ptr [eax], 0x26
            jne L0040d025
            mov ecx, dword ptr [ecx]
            cmp ecx, edi
            jne L0040d017
    L0040d025:
            cmp ecx, edi
            je L0040d031
            mov edx, dword ptr [ecx + 0x18]
    L0040d02c:
            call MungeFaces
    L0040d031:
            push ebp
            push offset FindNonCarsCB
            push esi
            call BrActorEnum
            mov eax, dword ptr [gMr_blendy]
            add esp, 0xc
            cmp eax, edi
            je L0040d1da
            mov esi, dword ptr [eax + 0x18]
            mov ecx, dword ptr [ebp + 0x18]
            push eax
            mov eax, dword ptr [esp + 0x14]
            lea edx, [eax + eax*2]
            mov eax, dword ptr [esp + 0x18]
            mov ecx, dword ptr [ecx + eax*4]
            mov edx, dword ptr [ecx + edx*4]
            push edx
            call BrActorAdd
            mov eax, dword ptr [gMr_blendy]
            add esp, 8
            mov eax, dword ptr [eax + 0x18]
            or byte ptr [eax + 0x20], 0x80
            mov ecx, dword ptr [gMr_blendy]
            mov edx, dword ptr [ecx + 0x18]
            push edx
            call BrModelAdd
            mov ecx, dword ptr [gMr_blendy]
            add esp, 4
            lea eax, [esp + 0x1c]
            lea edx, [esp + 0x18]
            mov ecx, dword ptr [ecx + 0x18]
            push eax
            call GetModelTextureArea
            mov edi, dword ptr [esp + 0x1c]
            mov ecx, dword ptr [esp + 0x18]
            xor eax, eax
            xor ebx, ebx
            mov ax, word ptr [esi + 0x10]
            mov bx, word ptr [esi + 0x12]
            add eax, edi
            add ebx, ecx
            push 4
            lea edx, [eax + eax*4]
            mov eax, dword ptr [gMr_blendy]
            shl edx, 3
            mov ecx, dword ptr [eax + 0x18]
            push edx
            push ecx
            call BrResAllocate
            mov edx, dword ptr [gMr_blendy]
            mov edi, eax
            xor eax, eax
            mov dword ptr [esp + 0x34], edi
            mov edx, dword ptr [edx + 0x18]
            add esp, 0xc
            mov ax, word ptr [edx + 0x10]
            mov esi, dword ptr [edx + 8]
            lea ecx, [eax + eax*4]
            shl ecx, 3
            mov eax, ecx
            shr ecx, 2
            rep movsd
            mov ecx, eax
            and ecx, 3
            rep movsb
            mov ecx, dword ptr [gMr_blendy]
            mov edx, dword ptr [ecx + 0x18]
            mov eax, dword ptr [edx + 8]
            push eax
            call BrResFree
            mov ecx, dword ptr [gMr_blendy]
            mov eax, dword ptr [esp + 0x2c]
            add esp, 4
            mov edx, dword ptr [ecx + 0x18]
            lea ecx, [ebx + ebx*4]
            shl ecx, 3
            mov dword ptr [edx + 8], eax
            mov edx, dword ptr [gMr_blendy]
            push 5
            push ecx
            mov eax, dword ptr [edx + 0x18]
            push eax
            call BrResAllocate
            mov ecx, dword ptr [gMr_blendy]
            add esp, 0xc
            mov ebx, eax
            xor eax, eax
            mov edx, dword ptr [ecx + 0x18]
            mov ax, word ptr [edx + 0x12]
            mov esi, dword ptr [edx + 0xc]
            lea ecx, [eax + eax*4]
            shl ecx, 3
            mov edx, ecx
            mov edi, ebx
            shr ecx, 2
            rep movsd
            mov ecx, edx
            and ecx, 3
            rep movsb
            mov eax, dword ptr [gMr_blendy]
            mov ecx, dword ptr [eax + 0x18]
            mov edx, dword ptr [ecx + 0xc]
            push edx
            call BrResFree
            mov eax, dword ptr [gMr_blendy]
            add esp, 4
            mov edx, 0x7fff
            mov ecx, dword ptr [eax + 0x18]
            mov dword ptr [ecx + 0xc], ebx
            mov eax, dword ptr [gMr_blendy]
            mov ecx, dword ptr [eax + 0x18]
            call DRModelUpdateAndKevificateMaterials
            mov eax, dword ptr [esp + 0x10]
            mov ecx, dword ptr [ebp + 0x18]
            mov esi, dword ptr [esp + 0x24]
            lea edx, [eax + eax*2]
            mov eax, dword ptr [esp + 0x14]
            mov ecx, dword ptr [ecx + eax*4]
            mov eax, dword ptr [gMr_blendy]
            mov dword ptr [ecx + edx*4 + 8], eax
            mov ecx, esi
            call SetSmashableModel
            xor eax, eax
            pop edi
            pop esi
            pop ebp
            pop ebx
            add esp, 0x10
            ret
    L0040d1cb:
            push ebp
            push offset ProcessModelsCB
            push esi
            call BrActorEnum
            add esp, 0xc
    L0040d1da:
            mov ecx, esi
            call SetSmashableModel
            pop edi
            pop esi
            pop ebp
            xor eax, eax
            pop ebx
            add esp, 0x10
            ret
    }
#endif
}

void C2_HOOK_FASTCALL ProcessModels(tTrack_spec* pTrack_spec) {

    BrActorEnum(pTrack_spec->the_actor, ProcessModelsCB, pTrack_spec);
}

void C2_HOOK_FASTCALL AllocateActorMatrix(tTrack_spec* pTrack_spec, tTrack_square*** pDst) {
    tU16 z;

    *pDst = BrMemAllocate(pTrack_spec->ncolumns_z * sizeof(tTrack_square*) , kMem_columns_z);
    for (z = 0; z < pTrack_spec->ncolumns_z; z++) {
        (*pDst)[z] = BrMemAllocate(pTrack_spec->ncolumns_x * sizeof(tTrack_square), kMem_columns_x);
        C2_HOOK_BUG_ON(sizeof(tTrack_square) != 12);

        memset((*pDst)[z], 0, sizeof(tTrack_square) * pTrack_spec->ncolumns_x);
    }
}

void AssertNonCars(br_actor** pNon_cars, int* pCount_non_cars, int* pTrack_count_non_cars, int* pTrack_capacity_non_cars) {
    int i;
    int count_nulls = 0;
    int count_non_nulls = 0;

    for (i = 0; i < *pCount_non_cars; i++) {

        if (pNon_cars[i] == NULL) {
            count_nulls += 1;
        } else {
            pNon_cars[count_non_nulls] = pNon_cars[i];
            sprintf(&pNon_cars[count_non_nulls]->identifier[4], "%04d", count_non_nulls);
            count_non_nulls += 1;
        }
    }
    *pTrack_count_non_cars -= count_nulls;
    *pTrack_capacity_non_cars -= count_nulls;
    *pCount_non_cars -= count_nulls;
}

// FUNCTION: CARMA2_HW 0x0040cc50
void C2_NAKED C2_HOOK_FASTCALL ExtractColumns(tTrack_spec* pTrack_spec) {
#ifndef CARPOCALYPSE2_MATCHING
    const char* identifier;
    char s[256];
    unsigned int x;
    unsigned int z;
    int ad;
    br_scalar extra_room;
    br_bounds bounds;
    int count;

    /* e.g. newcity1: "PP01 16 16 1.000 1126" */
    identifier = pTrack_spec->the_actor->identifier;
    if (identifier[0] == 'P' && identifier[1] == 'P') {
        if (identifier[3] != '1') {
            sprintf(s, "The World has officially ended: Wrong format of track!!! '");
            strncat(s, pTrack_spec->the_actor->identifier, 4);
            strcat(s, "'");
            PDFatalError(s);
        }
        identifier = &identifier[5];
    }

    count = sscanf(identifier, "%u%u%f%d", &x, &z, &extra_room, &ad);

    if (count == 3) {
        BrFailure(
            "Attempt to extract columns from invalid track\n"
            "(It might have been produced by an ancient preproc.\n"
            "This is no longer supported.\n");
    }
    if (count == 4) {
        pTrack_spec->count_non_cars = ad; /* FIXME: pTrack_spec->ampersand_digits = ad; */
    } else {
        x = 1;
        z = 1;
        extra_room = 0.f;
        pTrack_spec->count_non_cars = 0; /* FIXME: pTrack_spec->ampersand_digits = 0; */
    }

    pTrack_spec->ncolumns_x = x;
    pTrack_spec->ncolumns_z = z;

    BrActorToBounds(&bounds, pTrack_spec->the_actor);
    pTrack_spec->column_size_x = (bounds.max.v[0] - bounds.min.v[0] + 2.f * extra_room) / (float)pTrack_spec->ncolumns_x;
    pTrack_spec->column_size_z = (bounds.max.v[2] - bounds.min.v[2] + 2.f * extra_room) / (float)pTrack_spec->ncolumns_z;
    pTrack_spec->origin_x = bounds.min.v[0] - extra_room;
    pTrack_spec->origin_z = bounds.min.v[2] - extra_room;

    AllocateActorMatrix(pTrack_spec, &pTrack_spec->columns);

    gMax_count_non_cars = pTrack_spec->count_non_cars + 250;
    gCount_track_non_cars = pTrack_spec->count_non_cars;
    pTrack_spec->non_car_list = BrMemAllocate(gMax_count_non_cars * sizeof(br_actor*), kMem_non_car_list);
    if (count == 4) {
        ProcessModels(pTrack_spec);
    } else {
        pTrack_spec->columns[0][0].actor_0x0 = pTrack_spec->the_actor;
    }

    AssertNonCars(pTrack_spec->non_car_list, &pTrack_spec->count_non_cars, &gCount_track_non_cars, &gMax_count_non_cars);
#else
    __asm {
            sub esp, 0x134
            push ebx
            mov ebx, ecx
            push ebp
            xor ebp, ebp
            mov eax, dword ptr [ebx + 0x14]
            push esi
            push edi
            mov dword ptr [esp + 0x14], ebp
            mov eax, dword ptr [eax + 0x14]
            cmp byte ptr [eax], 0x50
            jne L0040cce1
            cmp byte ptr [eax + 1], 0x50
            jne L0040cce1
            cmp byte ptr [eax + 3], 0x31
            jne L0040cc7e
            add eax, 5
            jmp L0040cce1
    L0040cc7e:
            lea ecx, [esp + 0x44]
            push offset gRender_alternative_track_actors+0x9c
            push ecx
            call sprintf
            mov edx, dword ptr [ebx + 0x14]
            add esp, 8
            lea ecx, [esp + 0x44]
            mov eax, dword ptr [edx + 0x14]
            push 4
            push eax
            push ecx
            call strncat
            mov edi, offset gRender_alternative_track_actors+0x98
            or ecx, 0xffffffff
            xor eax, eax
            add esp, 0xc
            repne scasb
            not ecx
            sub edi, ecx
            lea edx, [esp + 0x44]
            mov esi, edi
            mov edi, edx
            mov edx, ecx
            or ecx, 0xffffffff
            repne scasb
            mov ecx, edx
            dec edi
            shr ecx, 2
            rep movsd
            mov ecx, edx
            and ecx, 3
            rep movsb
            lea ecx, [esp + 0x44]
            call PDFatalError
            mov eax, dword ptr [esp + 0x18]
    L0040cce1:
            lea ecx, [esp + 0x28]
            lea edx, [esp + 0x1c]
            push ecx
            push edx
            lea ecx, [esp + 0x2c]
            lea edx, [esp + 0x28]
            push ecx
            push edx
            push offset gRender_alternative_track_actors+0x8c
            push eax
            call sscanf
            add esp, 0x18
            sub eax, 3
            je L0040cd32
            dec eax
            je L0040cd29
            mov eax, 1
            mov dword ptr [esp + 0x1c], 0
            mov dword ptr [esp + 0x14], eax
            mov dword ptr [esp + 0x20], eax
            mov dword ptr [esp + 0x24], eax
            mov dword ptr [ebx + 0x1c], ebp
            jmp L0040cd3f
    L0040cd29:
            mov eax, dword ptr [esp + 0x28]
            mov dword ptr [ebx + 0x1c], eax
            jmp L0040cd3f
    L0040cd32:
            push offset gRender_alternative_track_actors+0xc
            call BrFailure
            add esp, 4
    L0040cd3f:
            mov cl, byte ptr [esp + 0x20]
            mov dl, byte ptr [esp + 0x24]
            mov eax, dword ptr [esp + 0x1c]
            mov byte ptr [ebx], cl
            mov ecx, dword ptr [ebx + 0x14]
            mov byte ptr [ebx + 1], dl
            lea edx, [esp + 0x2c]
            push ecx
            push edx
            mov dword ptr [esp + 0x18], eax
            call BrActorToBounds
            fld dword ptr [esp + 0x18]
            fmul dword ptr [gFloat_0058935c+0x64]
            fld dword ptr [esp + 0x40]
            fsub dword ptr [esp + 0x34]
            xor eax, eax
            add esp, 8
            mov al, byte ptr [ebx]
            mov dword ptr [esp + 0x18], eax
            xor eax, eax
            fsub st(0), st(1)
            fidiv dword ptr [esp + 0x18]
            mov al, byte ptr [ebx + 1]
            push 0x86
            mov dword ptr [esp + 0x1c], eax
            lea ecx, [eax*4]
            push ecx
            fstp dword ptr [ebx + 4]
            fld dword ptr [esp + 0x48]
            fsub dword ptr [esp + 0x3c]
            fsub st(0), st(1)
            fidiv dword ptr [esp + 0x20]
            fstp dword ptr [ebx + 8]
            fstp st(0)
            fld dword ptr [esp + 0x34]
            fsub dword ptr [esp + 0x18]
            fstp dword ptr [ebx + 0xc]
            fld dword ptr [esp + 0x3c]
            fsub dword ptr [esp + 0x18]
            fstp dword ptr [ebx + 0x10]
            call BrMemAllocate
            mov dword ptr [ebx + 0x18], eax
            mov al, byte ptr [ebx + 1]
            add esp, 8
            test al, al
            je L0040ce2d
    L0040cdd9:
            xor eax, eax
            mov esi, ebp
            mov al, byte ptr [ebx]
            and esi, 0xffff
            push 0x87
            lea edx, [eax + eax*2]
            shl edx, 2
            push edx
            shl esi, 2
            call BrMemAllocate
            mov ecx, dword ptr [ebx + 0x18]
            add esp, 8
            mov dword ptr [ecx + esi], eax
            mov edx, dword ptr [ebx + 0x18]
            xor eax, eax
            mov al, byte ptr [ebx]
            mov edi, dword ptr [edx + esi]
            lea ecx, [eax + eax*2]
            xor eax, eax
            shl ecx, 2
            mov edx, ecx
            shr ecx, 2
            rep stosd
            mov ecx, edx
            and ecx, 3
            inc ebp
            rep stosb
            movzx ax, byte ptr [ebx + 1]
            cmp bp, ax
            jne L0040cdd9
    L0040ce2d:
            mov eax, dword ptr [ebx + 0x1c]
            push 0x88
            add eax, 0xfa
            mov dword ptr [gMax_count_non_cars], eax
            mov ecx, dword ptr [ebx + 0x1c]
            lea edx, [eax*4]
            mov dword ptr [gCount_track_non_cars], ecx
            push edx
            call BrMemAllocate
            mov dword ptr [ebx + 0x20], eax
            mov eax, dword ptr [esp + 0x1c]
            xor esi, esi
            add esp, 8
            cmp eax, esi
            je L0040ce71
            mov eax, dword ptr [ebx + 0x18]
            mov edx, dword ptr [ebx + 0x14]
            mov ecx, dword ptr [eax]
            mov dword ptr [ecx], edx
            jmp L0040ce83
    L0040ce71:
            mov eax, dword ptr [ebx + 0x14]
            push ebx
            push offset ProcessModelsCB
            push eax
            call BrActorEnum
            add esp, 0xc
    L0040ce83:
            mov eax, dword ptr [ebx + 0x1c]
            mov edi, dword ptr [ebx + 0x20]
            cmp eax, esi
            mov dword ptr [esp + 0x10], esi
            mov dword ptr [esp + 0x14], esi
            jle L0040ced2
            mov ebp, edi
    L0040ce97:
            mov eax, dword ptr [ebp]
            test eax, eax
            jne L0040cea4
            inc dword ptr [esp + 0x10]
            jmp L0040cebf
    L0040cea4:
            mov dword ptr [edi], eax
            mov ecx, dword ptr [eax + 0x14]
            push esi
            add ecx, 4
            push offset gRender_alternative_track_actors+0x4
            push ecx
            call sprintf
            add esp, 0xc
            inc esi
            add edi, 4
    L0040cebf:
            mov eax, dword ptr [esp + 0x14]
            mov ecx, dword ptr [ebx + 0x1c]
            inc eax
            add ebp, 4
            cmp eax, ecx
            mov dword ptr [esp + 0x14], eax
            jl L0040ce97
    L0040ced2:
            mov eax, dword ptr [esp + 0x10]
            mov esi, dword ptr [gMax_count_non_cars]
            mov edx, dword ptr [gCount_track_non_cars]
            sub esi, eax
            sub edx, eax
            mov dword ptr [gMax_count_non_cars], esi
            mov dword ptr [gCount_track_non_cars], edx
            mov ecx, dword ptr [ebx + 0x1c]
            sub ecx, eax
            pop edi
            pop esi
            mov dword ptr [ebx + 0x1c], ecx
            pop ebp
            pop ebx
            add esp, 0x134
            ret
    }
#endif
}

void C2_HOOK_FASTCALL DisposeRuntimeBuiltModels(tTrack_spec* pTrack_spec) {
    int z;

    for (z = 0; z < pTrack_spec->ncolumns_z; z++) {
        int x;

        for (x = 0; x < pTrack_spec->ncolumns_x; x++) {
            br_actor* a;

            a = pTrack_spec->columns[z][x].actor_0x8;
            if (a != NULL && a->model != NULL) {
                BrModelRemove(a->model);
                BrModelFree(a->model);
            }
        }
    }
}

void C2_HOOK_FASTCALL DisposeActorMatrix(tTrack_spec* pTrack_spec, tTrack_square** pColumns) {

    if (pColumns != NULL) {
        int z;

        for (z = 0; z < pTrack_spec->ncolumns_z; z++) {
            BrMemFree(pColumns[z]);
        }
        BrMemFree(pColumns);
    }
}

// FUNCTION: CARMA2_HW 0x0040ca90
void C2_HOOK_FASTCALL DisposeColumns(tTrack_spec* pTrack_spec) {
    int z;
    int x;
    tTrack_square** columns;
    tU16 row;
    int i;

    for (z = 0; z != pTrack_spec->ncolumns_z; z++) {
        for (x = 0; x != pTrack_spec->ncolumns_x; x++) {
            br_actor* actor = pTrack_spec->columns[z][x].actor_0x8;

            if (actor != NULL && actor->model != NULL) {
                BrModelRemove(actor->model);
                BrModelFree(actor->model);
            }
        }
    }
    columns = pTrack_spec->columns;
    if (columns != NULL) {
        for (row = 0; row != pTrack_spec->ncolumns_z; row++) {
            BrMemFree(columns[row]);
        }
        BrMemFree(columns);
    }
    if (pTrack_spec->non_car_list != NULL) {
        for (i = gCount_track_non_cars; i < pTrack_spec->count_non_cars; i++) {
            if (pTrack_spec->non_car_list[i] != NULL && pTrack_spec->non_car_list[i]->parent != gNon_track_actor) {
                BrActorRemove(pTrack_spec->non_car_list[i]);
                BrActorFree(pTrack_spec->non_car_list[i]);
            }
        }
        BrMemFree(pTrack_spec->non_car_list);
    }
}


// FUNCTION: CARMA2_HW 0x0040e000
int C2_HOOK_FASTCALL ProcessNearbyActors(tTrack_spec* pTrack, br_vector3* pPos, float pMax_dist, int pMatch_type, char pIdentifier_value, tS8 pIdentifier_index, tU16 pMatch_flags, tNearbyActors_cbfn* pCallback, void* pContext) {

    tFoundAnActor_context found_an_actor_context;
    float p;
    float column_origin;
    int x, xmin, xmax;
    int z, zmin, zmax;

    found_an_actor_context.match_type = pMatch_type;
    found_an_actor_context.identifier_value = pIdentifier_value;
    found_an_actor_context.identifier_index = pIdentifier_index;
    found_an_actor_context.match_flags = pMatch_flags;
    found_an_actor_context.pos = pPos;
    found_an_actor_context.callback_context = pContext;
    found_an_actor_context.callback = (int (C2_HOOK_FASTCALL *)(br_actor*, void*))pCallback;
    found_an_actor_context.max_dist_axis = pMax_dist;
    x = (int)((pPos->v[0] - pTrack->origin_x) / pTrack->column_size_x);
    found_an_actor_context.max_dist_squared = pMax_dist * pMax_dist;
    column_origin = pTrack->origin_x + (float)x * pTrack->column_size_x;
    p = column_origin;
    xmin = x;
    while (p + pMax_dist > pPos->v[0]) {
        if (xmin == 0) {
            break;
        }
        p -= pTrack->column_size_x;
        xmin -= 1;
    }
    p = pTrack->column_size_x + column_origin;
    xmax = x;
    while (pPos->v[0] + pMax_dist > p) {
        if (xmax + 1 >= pTrack->ncolumns_x) {
            break;
        }
        p += pTrack->column_size_x;
        xmax += 1;
    }

    z = (int)((pPos->v[2] - pTrack->origin_z) / pTrack->column_size_z);
    column_origin = pTrack->origin_z + (float)z * pTrack->column_size_z;
    p = column_origin;
    zmin = z;
    while (p + pMax_dist > pPos->v[2]) {
        if (zmin == 0) {
            break;
        }
        p -= pTrack->column_size_z;
        zmin -= 1;
    }
    p = pTrack->column_size_z + column_origin;
    zmax = z;
    while (pPos->v[2] + pMax_dist > p) {
        if (zmax + 1 >= pTrack->ncolumns_z) {
            break;
        }
        p += pTrack->column_size_z;
        zmax += 1;
    }
    if (xmin < 0) {
        xmin = 0;
    } else if (xmin >= pTrack->ncolumns_x) {
        xmin = pTrack->ncolumns_x - 1;
    }
    if (xmax < 0) {
        xmax = 0;
    } else if (xmax >= pTrack->ncolumns_x) {
        xmax = pTrack->ncolumns_x - 1;
    }
    if (zmin < 0) {
        zmin = 0;
    } else if (zmin >= pTrack->ncolumns_z) {
        zmin = pTrack->ncolumns_z - 1;
    }
    if (zmax < 0) {
        zmax = 0;
    } else if (zmax >= pTrack->ncolumns_z) {
        zmax = pTrack->ncolumns_z - 1;
    }

    for (x = xmin; x <= xmax; x++) {
        for (z = zmin; z <= zmax; z++) {
            if (pTrack->columns[z][x].actor_0x0 != NULL) {
                p = (float)(tU32)DRActorEnumRecurse(pTrack->columns[z][x].actor_0x0, FoundAnActor, &found_an_actor_context);
                if (p != 0.0f) {
                    return (int)p;
                }
            }
        }
    }
    return 0;
}

// FUNCTION: CARMA2_HW 0x0040e290
intptr_t C2_NAKED C2_HOOK_CDECL FoundAnActor(br_actor* pActor, void* pContext) {
#ifndef CARPOCALYPSE2_MATCHING
    tFoundAnActor_context* context = pContext;
    char* identifier;

    if ((float)fabs(pActor->t.t.mat.m[3][0] - context->pos->v[0]) > context->max_dist_axis
            || (float)fabs(pActor->t.t.mat.m[3][1] - context->pos->v[1]) > context->max_dist_axis
            || (float)fabs(pActor->t.t.mat.m[3][2] - context->pos->v[2]) > context->max_dist_axis) {
        return 0;
    }
    if (pActor->type != BR_ACTOR_MODEL && (context->match_flags & 0x1000)) {
        return 0;
    }
    identifier = pActor->identifier;
    if (identifier == NULL && !(context->match_flags & 0x4)) {
        return 0;
    }
    if (identifier != NULL && identifier[0] == '&' && !(context->match_flags & 0x3)) {
        return 0;
    }
    if (identifier != NULL && identifier[0] != '&' && !(context->match_flags & 0x4)) {
        return 0;
    }
    if (context->match_type == 1) {
        if (pActor->type != BR_ACTOR_MODEL || pActor->model == NULL || pActor->model->identifier == NULL) {
            return 0;
        }
        if (context->identifier_index >= 0 && pActor->model->identifier[context->identifier_index] != context->identifier_value) {
            return 0;
        }
        if (context->identifier_index < 0 && pActor->model->identifier[strlen(pActor->model->identifier) + context->identifier_index] != context->identifier_value) {
            return 0;
        }
    }
    if (context->match_type == 2) {
        if (identifier == NULL) {
            return 0;
        }
        if (context->identifier_index >= 0 && identifier[context->identifier_index] != context->identifier_value) {
            return 0;
        }
        if (context->identifier_index < 0 && pActor->identifier[strlen(pActor->identifier) + context->identifier_index] != context->identifier_value) {
            return 0;
        }
    }
    if (Vector3DistanceSquared(context->pos, (br_vector3*)pActor->t.t.mat.m[3]) >= context->max_dist_squared) {
        return 0;
    }
    return context->callback(pActor, context->callback_context);
#else
    __asm {
            push ebx
            push ebp
            push esi
            mov esi, dword ptr [esp + 0x14]
            push edi
            mov edi, dword ptr [esp + 0x14]
            mov edx, dword ptr [esi + 0x10]
            fld dword ptr [edi + 0x50]
            fsub dword ptr [edx]
            fabs
            fcomp dword ptr [esi + 8]
            fnstsw ax
            test ah, 0x41
            je L0040e428
            fld dword ptr [edi + 0x54]
            fsub dword ptr [edx + 4]
            fabs
            fcomp dword ptr [esi + 8]
            fnstsw ax
            test ah, 0x41
            je L0040e428
            fld dword ptr [edi + 0x58]
            fsub dword ptr [edx + 8]
            fabs
            fcomp dword ptr [esi + 8]
            fnstsw ax
            test ah, 0x41
            je L0040e428
            mov cl, byte ptr [edi + 0x12]
            cmp cl, 1
            je L0040e2f2
            test byte ptr [esi + 7], 0x10
            jne L0040e428
    L0040e2f2:
            mov ebp, dword ptr [edi + 0x14]
            mov al, 4
            test ebp, ebp
            jne L0040e308
            test byte ptr [esi + 6], al
            je L0040e428
            test ebp, ebp
            je L0040e32b
    L0040e308:
            cmp byte ptr [ebp], 0x26
            jne L0040e318
            test byte ptr [esi + 6], 3
            je L0040e428
    L0040e318:
            test ebp, ebp
            je L0040e32b
            cmp byte ptr [ebp], 0x26
            je L0040e32b
            test byte ptr [esi + 6], al
            je L0040e428
    L0040e32b:
            cmp dword ptr [esi], 1
            jne L0040e38e
            cmp cl, 1
            jne L0040e428
            mov eax, dword ptr [edi + 0x18]
            test eax, eax
            je L0040e428
            mov edx, dword ptr [eax + 4]
            test edx, edx
            je L0040e428
            mov bl, byte ptr [esi + 5]
            test bl, bl
            jl L0040e36b
            movsx eax, bl
            mov cl, byte ptr [edx + eax]
            mov al, byte ptr [esi + 4]
            cmp cl, al
            jne L0040e428
            test bl, bl
            jge L0040e38e
    L0040e36b:
            mov edi, edx
            or ecx, 0xffffffff
            xor eax, eax
            repne scasb
            not ecx
            dec ecx
            movsx eax, bl
            add edx, ecx
            mov cl, byte ptr [eax + edx]
            mov al, byte ptr [esi + 4]
            cmp cl, al
            jne L0040e428
            mov edi, dword ptr [esp + 0x14]
    L0040e38e:
            cmp dword ptr [esi], 2
            jne L0040e3d3
            test ebp, ebp
            je L0040e428
            mov dl, byte ptr [esi + 5]
            test dl, dl
            jl L0040e3b4
            movsx eax, dl
            mov cl, byte ptr [ebp + eax]
            mov al, byte ptr [esi + 4]
            cmp cl, al
            jne L0040e428
            test dl, dl
            jge L0040e3d3
    L0040e3b4:
            mov edi, ebp
            or ecx, 0xffffffff
            xor eax, eax
            repne scasb
            not ecx
            dec ecx
            movsx edx, dl
            add ebp, ecx
            mov cl, byte ptr [esi + 4]
            mov al, byte ptr [edx + ebp]
            cmp al, cl
            jne L0040e428
            mov edi, dword ptr [esp + 0x14]
    L0040e3d3:
            mov eax, dword ptr [esi + 0x10]
            fld dword ptr [eax + 4]
            fld dword ptr [eax]
            fsub dword ptr [edi + 0x50]
            fxch st(1)
            fsub dword ptr [edi + 0x54]
            fld dword ptr [eax + 8]
            fsub dword ptr [edi + 0x58]
            fxch st(2)
            fst dword ptr [esp + 0x14]
            fld st(1)
            fxch st(1)
            fmul dword ptr [esp + 0x14]
            fxch st(1)
            fmul st(0), st(2)
            fxch st(3)
            fst dword ptr [esp + 0x14]
            fmul dword ptr [esp + 0x14]
            fxch st(3)
            faddp st(1), st(0)
            fxch st(2)
            faddp st(2), st(0)
            fxch st(1)
            fcomp dword ptr [esi + 0xc]
            fnstsw ax
            test ah, 1
            fstp st(0)
            je L0040e428
            mov edx, dword ptr [esi + 0x18]
            mov ecx, edi
            call dword ptr [esi + 0x14]
            pop edi
            pop esi
            pop ebp
            pop ebx
            ret
    L0040e428:
            pop edi
            pop esi
            pop ebp
            xor eax, eax
            pop ebx
            ret
            nop
    }
#endif
}

// FUNCTION: CARMA2_HW 0x0040d7c0
void C2_NAKED C2_HOOK_FASTCALL RenderTrack(br_actor* pWorld, tTrack_spec* pTrack_spec, br_actor* pCamera, br_matrix34* pCamera_to_world) {
#ifndef CARPOCALYPSE2_MATCHING
    tU8 column_x;
    tU8 column_z;
    tU8 min_x;
    tU8 max_x;
    tU8 min_z;
    tU8 max_z;
    br_vector3 edge_before;
    br_vector3 edge_after;
    br_camera* camera;
    br_scalar tan_fov_ish;
    br_scalar scale;
    tU8 x;
    tU8 z;
    tU8 idx_x;
    tU8 idx_z;
    br_actor* a;

    camera = (br_camera*)pCamera->type_data;
    if (pTrack_spec->columns == NULL) {
        BrZbsSceneRenderAdd(pWorld);
    } else {
        XZToColumnXZ(&column_x, &column_z, pCamera_to_world->m[3][0], pCamera_to_world->m[3][2], pTrack_spec);
        min_x = column_x;
        max_x = column_x;
        min_z = column_z;
        max_z = column_z;
        tan_fov_ish = BR_SIN(camera->field_of_view / 2) / BR_COS(camera->field_of_view / 2);

        edge_after.v[1] = tan_fov_ish;
        edge_after.v[0] = tan_fov_ish * camera->aspect;
        edge_after.v[2] = -1.0;
        scale = camera->yon_z * gYon_factor;
        BrVector3Scale(&edge_before, &edge_after, scale);
        BrMatrix34ApplyV(&edge_after, &edge_before, pCamera_to_world);
        XZToColumnXZ(&column_x, &column_z, pCamera_to_world->m[3][0] + edge_after.v[0], pCamera_to_world->m[3][2] + edge_after.v[2], pTrack_spec);
        if (column_x < min_x) {
            min_x = column_x;
        } else if (column_x > max_x) {
            max_x = column_x;
        }
        if (column_z < min_z) {
            min_z = column_z;
        } else if (column_z > max_z) {
            max_z = column_z;
        }
        edge_before.v[0] = -edge_before.v[0];
        BrMatrix34ApplyV(&edge_after, &edge_before, pCamera_to_world);
        XZToColumnXZ(&column_x, &column_z, pCamera_to_world->m[3][0] + edge_after.v[0], pCamera_to_world->m[3][2] + edge_after.v[2], pTrack_spec);
        if (column_x < min_x) {
            min_x = column_x;
        } else if (column_x > max_x) {
            max_x = column_x;
        }
        if (column_z < min_z) {
            min_z = column_z;
        } else {
            if (column_z > max_z) {
                max_z = column_z;
            }
        }
        edge_before.v[1] = -edge_before.v[1];
        BrMatrix34ApplyV(&edge_after, &edge_before, pCamera_to_world);
        XZToColumnXZ(&column_x, &column_z, pCamera_to_world->m[3][0] + edge_after.v[0], pCamera_to_world->m[3][2] + edge_after.v[2], pTrack_spec);
        if (column_x < min_x) {
            min_x = column_x;
        } else if (column_x > max_x) {
            max_x = column_x;
        }
        if (column_z < min_z) {
            min_z = column_z;
        } else if (column_z > max_z) {
            max_z = column_z;
        }
        edge_before.v[0] = -edge_before.v[0];
        BrMatrix34ApplyV(&edge_after, &edge_before, pCamera_to_world);
        XZToColumnXZ(&column_x, &column_z, pCamera_to_world->m[3][0] + edge_after.v[0], pCamera_to_world->m[3][2] + edge_after.v[2], pTrack_spec);
        if (column_x < min_x) {
            min_x = column_x;
        } else if (column_x > max_x) {
            max_x = column_x;
        }
        if (column_z < min_z) {
            min_z = column_z;
        } else if (column_z > max_z) {
            max_z = column_z;
        }
        if (min_x > 0) {
            min_x--;
        }
        if (max_x < pTrack_spec->ncolumns_x - 1) {
            max_x++;
        }
        if (min_z > 0) {
            min_z--;
        }
        if (max_z < pTrack_spec->ncolumns_z - 1) {
            max_z++;
        }
        if ((float)fabs(pCamera_to_world->m[2][0]) > (float)fabs(pCamera_to_world->m[2][2])) {
            for (x = min_x; x <= max_x; x++) {
                for (z = min_z; z <= max_z; z++) {
                    idx_x = (tU8)(pCamera_to_world->m[2][0] > 0.f ? x : min_x + max_x - x);
                    idx_z = (tU8)(pCamera_to_world->m[2][2] > 0.f ? z : min_z + max_z - z);
                    a = pTrack_spec->columns[idx_z][idx_x].actor_0x0;
                    if (a != NULL) {
                        if (gRender_alternative_track_actors) {
                            BrZbsSceneRenderAdd(a);
                        } else {
                            BrZbsSceneRenderAdd(pTrack_spec->columns[idx_z][idx_x].actor_0x4);
                        }
                    }
                }
            }
        } else {
            for (z = min_z; z <= max_z; z++) {
                for (x = min_x; x <= max_x; x++) {
                    idx_x = (tU8)(pCamera_to_world->m[2][0] > 0.f ? x : min_x + max_x - x);
                    idx_z = (tU8)(pCamera_to_world->m[2][2] > 0.f ? z : min_z + max_z - z);
                    a = pTrack_spec->columns[idx_z][idx_x].actor_0x0;
                    if (a != NULL) {
                        if (gRender_alternative_track_actors) {
                            BrZbsSceneRenderAdd(a);
                        } else {
                            BrZbsSceneRenderAdd(pTrack_spec->columns[idx_z][idx_x].actor_0x4);
                        }
                    }
                }
            }
        }
    }
#else
    __asm {
            mov eax, dword ptr [esp + 4]
            sub esp, 0x3c
            push ebx
            push ebp
            mov ebp, edx
            push esi
            mov esi, dword ptr [eax + 0x5c]
            push edi
            mov eax, dword ptr [ebp + 0x18]
            test eax, eax
            jne L0040d7ea
            push ecx
            call BrZbsSceneRenderAdd
            add esp, 4
            pop edi
            pop esi
            pop ebp
            pop ebx
            add esp, 0x3c
            ret 8
    L0040d7ea:
            mov edi, dword ptr [esp + 0x54]
            fld dword ptr [edi + 0x24]
            fsub dword ptr [ebp + 0xc]
            fdiv dword ptr [ebp + 4]
            fld dword ptr [edi + 0x2c]
            fsub dword ptr [ebp + 0x10]
            fdiv dword ptr [ebp + 8]
            fxch st(1)
            fst dword ptr [esp + 0x50]
            fxch st(1)
            fxch st(1)
            fcomp dword ptr [gFloat_0058935c+0x5c]
            fnstsw ax
            test ah, 1
            je L0040d81f
            mov dword ptr [esp + 0x50], 0
    L0040d81f:
            xor ecx, ecx
            mov cl, byte ptr [ebp]
            mov dword ptr [esp + 0x2c], ecx
            fild dword ptr [esp + 0x2c]
            fld dword ptr [esp + 0x50]
            fcomp st(1)
            fnstsw ax
            test ah, 1
            jne L0040d845
            fsub dword ptr [gFloat_0058935c+0x60]
            fstp dword ptr [esp + 0x50]
            jmp L0040d847
    L0040d845:
            fstp st(0)
    L0040d847:
            fcom dword ptr [gFloat_0058935c+0x5c]
            fnstsw ax
            test ah, 1
            je L0040d85c
            fstp st(0)
            fld dword ptr [gFloat_0058935c+0x5c]
    L0040d85c:
            xor edx, edx
            mov dl, byte ptr [ebp + 1]
            mov dword ptr [esp + 0x2c], edx
            fild dword ptr [esp + 0x2c]
            fstp dword ptr [esp + 0x14]
            fcom dword ptr [esp + 0x14]
            fnstsw ax
            test ah, 1
            jne L0040d884
            fstp st(0)
            fld dword ptr [esp + 0x14]
            fsub dword ptr [gFloat_0058935c+0x60]
    L0040d884:
            fld dword ptr [esp + 0x50]
            call _ftol
            mov bl, al
            call _ftol
            mov cx, word ptr [esi + 6]
            lea edx, [esp + 0x40]
            fld dword ptr [esi + 0x10]
            shr cx, 1
            and ecx, 0xffff
            push edi
            mov dword ptr [esp + 0x54], ecx
            lea ecx, [esp + 0x38]
            fild dword ptr [esp + 0x54]
            push edx
            push ecx
            mov byte ptr [esp + 0x24], bl
            mov byte ptr [esp + 0x1c], bl
            fmul qword ptr [gFloat_0058935c+0x8c]
            fld dword ptr [gFloat_0058935c+0x94]
            fld st(1)
            fsin
            mov byte ptr [esp + 0x28], al
            mov byte ptr [esp + 0x5c], al
            fxch st(2)
            fcos
            fxch st(2)
            fst dword ptr [esp + 0x38]
            fxch st(2)
            fst dword ptr [esp + 0x38]
            fdivp st(2), st(0)
            fxch st(2)
            fmul st(0), st(1)
            fld st(1)
            fxch st(3)
            fstp dword ptr [esp + 0x48]
            fstp dword ptr [esp + 0x40]
            fxch st(1)
            fstp dword ptr [esp + 0x44]
            fld dword ptr [esi + 0xc]
            fmul dword ptr [gYon_factor]
            fmul dword ptr [esp + 0x40]
            fstp dword ptr [esp + 0x4c]
            fld dword ptr [esi + 0xc]
            fmul dword ptr [gYon_factor]
            fmul st(0), st(1)
            fstp dword ptr [esp + 0x50]
            fstp st(0)
            fld dword ptr [esi + 0xc]
            fmul dword ptr [gYon_factor]
            fmul dword ptr [gFloat_0058935c+0x94]
            fstp dword ptr [esp + 0x54]
            call BrMatrix34ApplyV
            fld dword ptr [esp + 0x40]
            fadd dword ptr [edi + 0x24]
            fsub dword ptr [ebp + 0xc]
            fdiv dword ptr [ebp + 4]
            add esp, 0xc
            fst dword ptr [esp + 0x20]
            fld dword ptr [esp + 0x3c]
            fadd dword ptr [edi + 0x2c]
            fsub dword ptr [ebp + 0x10]
            fxch st(1)
            fcomp dword ptr [gFloat_0058935c+0x5c]
            fdiv dword ptr [ebp + 8]
            fnstsw ax
            test ah, 1
            je L0040d971
            mov dword ptr [esp + 0x20], 0
    L0040d971:
            xor edx, edx
            mov dl, byte ptr [ebp]
            mov dword ptr [esp + 0x2c], edx
            fild dword ptr [esp + 0x2c]
            fld dword ptr [esp + 0x20]
            fcomp st(1)
            fnstsw ax
            test ah, 1
            jne L0040d997
            fsub dword ptr [gFloat_0058935c+0x60]
            fstp dword ptr [esp + 0x20]
            jmp L0040d999
    L0040d997:
            fstp st(0)
    L0040d999:
            fcom dword ptr [gFloat_0058935c+0x5c]
            fnstsw ax
            test ah, 1
            je L0040d9ae
            fstp st(0)
            fld dword ptr [gFloat_0058935c+0x5c]
    L0040d9ae:
            xor eax, eax
            mov al, byte ptr [ebp + 1]
            mov dword ptr [esp + 0x2c], eax
            fild dword ptr [esp + 0x2c]
            fstp dword ptr [esp + 0x14]
            fcom dword ptr [esp + 0x14]
            fnstsw ax
            test ah, 1
            jne L0040d9d6
            fstp st(0)
            fld dword ptr [esp + 0x14]
            fsub dword ptr [gFloat_0058935c+0x60]
    L0040d9d6:
            fld dword ptr [esp + 0x20]
            call _ftol
            mov bl, al
            call _ftol
            cmp bl, byte ptr [esp + 0x18]
            jae L0040d9f2
            mov byte ptr [esp + 0x18], bl
            jmp L0040d9fc
    L0040d9f2:
            cmp bl, byte ptr [esp + 0x10]
            jbe L0040d9fc
            mov byte ptr [esp + 0x10], bl
    L0040d9fc:
            cmp al, byte ptr [esp + 0x1c]
            jae L0040da08
            mov byte ptr [esp + 0x1c], al
            jmp L0040da12
    L0040da08:
            cmp al, byte ptr [esp + 0x50]
            jbe L0040da12
            mov byte ptr [esp + 0x50], al
    L0040da12:
            fld dword ptr [esp + 0x40]
            lea ecx, [esp + 0x40]
            push edi
            lea edx, [esp + 0x38]
            push ecx
            fchs
            fstp dword ptr [esp + 0x48]
            push edx
            call BrMatrix34ApplyV
            fld dword ptr [esp + 0x40]
            fadd dword ptr [edi + 0x24]
            fld dword ptr [ebp + 4]
            fld dword ptr [esp + 0x48]
            fxch st(2)
            fsub dword ptr [ebp + 0xc]
            fxch st(2)
            fadd dword ptr [edi + 0x2c]
            fxch st(1)
            fdivp st(2), st(0)
            add esp, 0xc
            fsub dword ptr [ebp + 0x10]
            fdiv dword ptr [ebp + 8]
            fxch st(1)
            fst dword ptr [esp + 0x20]
            fxch st(1)
            fxch st(1)
            fcomp dword ptr [gFloat_0058935c+0x5c]
            fnstsw ax
            test ah, 1
            je L0040da70
            mov dword ptr [esp + 0x20], 0
    L0040da70:
            xor eax, eax
            mov al, byte ptr [ebp]
            mov dword ptr [esp + 0x2c], eax
            fild dword ptr [esp + 0x2c]
            fld dword ptr [esp + 0x20]
            fcomp st(1)
            fnstsw ax
            test ah, 1
            jne L0040da96
            fsub dword ptr [gFloat_0058935c+0x60]
            fstp dword ptr [esp + 0x20]
            jmp L0040da98
    L0040da96:
            fstp st(0)
    L0040da98:
            fcom dword ptr [gFloat_0058935c+0x5c]
            fnstsw ax
            test ah, 1
            je L0040daad
            fstp st(0)
            fld dword ptr [gFloat_0058935c+0x5c]
    L0040daad:
            xor ecx, ecx
            mov cl, byte ptr [ebp + 1]
            mov dword ptr [esp + 0x2c], ecx
            fild dword ptr [esp + 0x2c]
            fstp dword ptr [esp + 0x14]
            fcom dword ptr [esp + 0x14]
            fnstsw ax
            test ah, 1
            jne L0040dad5
            fstp st(0)
            fld dword ptr [esp + 0x14]
            fsub dword ptr [gFloat_0058935c+0x60]
    L0040dad5:
            fld dword ptr [esp + 0x20]
            call _ftol
            mov bl, al
            call _ftol
            cmp bl, byte ptr [esp + 0x18]
            jae L0040daf1
            mov byte ptr [esp + 0x18], bl
            jmp L0040dafb
    L0040daf1:
            cmp bl, byte ptr [esp + 0x10]
            jbe L0040dafb
            mov byte ptr [esp + 0x10], bl
    L0040dafb:
            cmp al, byte ptr [esp + 0x1c]
            jae L0040db07
            mov byte ptr [esp + 0x1c], al
            jmp L0040db11
    L0040db07:
            cmp al, byte ptr [esp + 0x50]
            jbe L0040db11
            mov byte ptr [esp + 0x50], al
    L0040db11:
            fld dword ptr [esp + 0x44]
            lea edx, [esp + 0x40]
            push edi
            lea eax, [esp + 0x38]
            push edx
            fchs
            fstp dword ptr [esp + 0x4c]
            push eax
            call BrMatrix34ApplyV
            fld dword ptr [esp + 0x40]
            fadd dword ptr [edi + 0x24]
            fld dword ptr [ebp + 4]
            fld dword ptr [esp + 0x48]
            fxch st(2)
            fsub dword ptr [ebp + 0xc]
            fxch st(2)
            fadd dword ptr [edi + 0x2c]
            fxch st(1)
            fdivp st(2), st(0)
            add esp, 0xc
            fsub dword ptr [ebp + 0x10]
            fdiv dword ptr [ebp + 8]
            fxch st(1)
            fst dword ptr [esp + 0x20]
            fxch st(1)
            fxch st(1)
            fcomp dword ptr [gFloat_0058935c+0x5c]
            fnstsw ax
            test ah, 1
            je L0040db6f
            mov dword ptr [esp + 0x20], 0
    L0040db6f:
            xor ecx, ecx
            mov cl, byte ptr [ebp]
            mov dword ptr [esp + 0x2c], ecx
            fild dword ptr [esp + 0x2c]
            fld dword ptr [esp + 0x20]
            fcomp st(1)
            fnstsw ax
            test ah, 1
            jne L0040db95
            fsub dword ptr [gFloat_0058935c+0x60]
            fstp dword ptr [esp + 0x20]
            jmp L0040db97
    L0040db95:
            fstp st(0)
    L0040db97:
            fcom dword ptr [gFloat_0058935c+0x5c]
            fnstsw ax
            test ah, 1
            je L0040dbac
            fstp st(0)
            fld dword ptr [gFloat_0058935c+0x5c]
    L0040dbac:
            xor edx, edx
            mov dl, byte ptr [ebp + 1]
            mov dword ptr [esp + 0x2c], edx
            fild dword ptr [esp + 0x2c]
            fstp dword ptr [esp + 0x14]
            fcom dword ptr [esp + 0x14]
            fnstsw ax
            test ah, 1
            jne L0040dbd4
            fstp st(0)
            fld dword ptr [esp + 0x14]
            fsub dword ptr [gFloat_0058935c+0x60]
    L0040dbd4:
            fld dword ptr [esp + 0x20]
            call _ftol
            mov bl, al
            call _ftol
            cmp bl, byte ptr [esp + 0x18]
            jae L0040dbf0
            mov byte ptr [esp + 0x18], bl
            jmp L0040dbfa
    L0040dbf0:
            cmp bl, byte ptr [esp + 0x10]
            jbe L0040dbfa
            mov byte ptr [esp + 0x10], bl
    L0040dbfa:
            cmp al, byte ptr [esp + 0x1c]
            jae L0040dc06
            mov byte ptr [esp + 0x1c], al
            jmp L0040dc10
    L0040dc06:
            cmp al, byte ptr [esp + 0x50]
            jbe L0040dc10
            mov byte ptr [esp + 0x50], al
    L0040dc10:
            fld dword ptr [esp + 0x40]
            lea eax, [esp + 0x40]
            push edi
            lea ecx, [esp + 0x38]
            push eax
            fchs
            fstp dword ptr [esp + 0x48]
            push ecx
            call BrMatrix34ApplyV
            fld dword ptr [esp + 0x40]
            fadd dword ptr [edi + 0x24]
            fld dword ptr [ebp + 4]
            fld dword ptr [esp + 0x48]
            fxch st(2)
            fsub dword ptr [ebp + 0xc]
            fxch st(2)
            fadd dword ptr [edi + 0x2c]
            fxch st(1)
            fdivp st(2), st(0)
            add esp, 0xc
            fsub dword ptr [ebp + 0x10]
            fdiv dword ptr [ebp + 8]
            fxch st(1)
            fst dword ptr [esp + 0x20]
            fxch st(1)
            fxch st(1)
            fcomp dword ptr [gFloat_0058935c+0x5c]
            fnstsw ax
            test ah, 1
            je L0040dc6e
            mov dword ptr [esp + 0x20], 0
    L0040dc6e:
            xor edx, edx
            mov dl, byte ptr [ebp]
            mov esi, edx
            mov dword ptr [esp + 0x2c], esi
            fild dword ptr [esp + 0x2c]
            fld dword ptr [esp + 0x20]
            fcomp st(1)
            fnstsw ax
            test ah, 1
            jne L0040dc96
            fsub dword ptr [gFloat_0058935c+0x60]
            fstp dword ptr [esp + 0x20]
            jmp L0040dc98
    L0040dc96:
            fstp st(0)
    L0040dc98:
            fcom dword ptr [gFloat_0058935c+0x5c]
            fnstsw ax
            test ah, 1
            je L0040dcad
            fstp st(0)
            fld dword ptr [gFloat_0058935c+0x5c]
    L0040dcad:
            xor eax, eax
            mov al, byte ptr [ebp + 1]
            mov edi, eax
            mov dword ptr [esp + 0x2c], edi
            fild dword ptr [esp + 0x2c]
            fstp dword ptr [esp + 0x14]
            fcom dword ptr [esp + 0x14]
            fnstsw ax
            test ah, 1
            jne L0040dcd7
            fstp st(0)
            fld dword ptr [esp + 0x14]
            fsub dword ptr [gFloat_0058935c+0x60]
    L0040dcd7:
            fld dword ptr [esp + 0x20]
            call _ftol
            mov bl, al
            call _ftol
            cmp bl, byte ptr [esp + 0x18]
            jae L0040dcf3
            mov byte ptr [esp + 0x18], bl
            jmp L0040dcfd
    L0040dcf3:
            cmp bl, byte ptr [esp + 0x10]
            jbe L0040dcfd
            mov byte ptr [esp + 0x10], bl
    L0040dcfd:
            mov cl, byte ptr [esp + 0x1c]
            cmp al, cl
            jae L0040dd0d
            mov cl, al
            mov byte ptr [esp + 0x1c], cl
            jmp L0040dd17
    L0040dd0d:
            cmp al, byte ptr [esp + 0x50]
            jbe L0040dd17
            mov byte ptr [esp + 0x50], al
    L0040dd17:
            mov dl, byte ptr [esp + 0x18]
            test dl, dl
            jbe L0040dd25
            dec dl
            mov byte ptr [esp + 0x18], dl
    L0040dd25:
            mov eax, dword ptr [esp + 0x10]
            dec esi
            and eax, 0xff
            cmp eax, esi
            jge L0040dd37
            inc byte ptr [esp + 0x10]
    L0040dd37:
            test cl, cl
            jbe L0040dd41
            dec cl
            mov byte ptr [esp + 0x1c], cl
    L0040dd41:
            mov eax, dword ptr [esp + 0x50]
            dec edi
            and eax, 0xff
            cmp eax, edi
            jge L0040dd53
            inc byte ptr [esp + 0x50]
    L0040dd53:
            mov eax, dword ptr [esp + 0x54]
            fld dword ptr [eax + 0x20]
            fld dword ptr [eax + 0x18]
            fabs
            fxch st(1)
            fabs
            fst dword ptr [esp + 0x2c]
            fcompp
            fnstsw ax
            test ah, 1
            je L0040dea5
            cmp dl, byte ptr [esp + 0x10]
            ja L0040dfcc
            mov ecx, dword ptr [esp + 0x10]
            mov eax, dword ptr [esp + 0x18]
            and ecx, 0xff
            and eax, 0xff
            mov dword ptr [esp + 0x28], ecx
            sub ecx, eax
            inc ecx
            mov dword ptr [esp + 0x30], eax
            mov dword ptr [esp + 0x24], eax
            mov dword ptr [esp + 0x10], ecx
    L0040dda4:
            mov cl, byte ptr [esp + 0x1c]
            mov al, byte ptr [esp + 0x50]
            cmp cl, al
            ja L0040de83
            mov ecx, dword ptr [esp + 0x50]
            mov ebx, dword ptr [esp + 0x1c]
            and ecx, 0xff
            and ebx, 0xff
            mov esi, ecx
            mov dword ptr [esp + 0x18], ebx
            sub esi, ebx
            mov dword ptr [esp + 0x2c], ecx
            inc esi
            jmp L0040dddb
    L0040ddd7:
            mov ecx, dword ptr [esp + 0x2c]
    L0040dddb:
            mov edi, dword ptr [esp + 0x54]
            fld dword ptr [edi + 0x18]
            fcomp dword ptr [gFloat_0058935c+0x5c]
            fnstsw ax
            test ah, 0x41
            jne L0040ddf9
            mov dl, byte ptr [esp + 0x24]
            mov byte ptr [esp + 0x20], dl
            jmp L0040de0d
    L0040ddf9:
            mov al, byte ptr [esp + 0x30]
            mov dl, byte ptr [esp + 0x28]
            add al, dl
            mov dl, byte ptr [esp + 0x24]
            sub al, dl
            mov byte ptr [esp + 0x20], al
    L0040de0d:
            fld dword ptr [edi + 0x20]
            fcomp dword ptr [gFloat_0058935c+0x5c]
            fnstsw ax
            test ah, 0x41
            jne L0040de27
            mov cl, byte ptr [esp + 0x18]
            mov byte ptr [esp + 0x14], cl
            jmp L0040de35
    L0040de27:
            mov al, byte ptr [esp + 0x18]
            mov dl, bl
            add dl, cl
            sub dl, al
            mov byte ptr [esp + 0x14], dl
    L0040de35:
            mov ecx, dword ptr [esp + 0x14]
            mov eax, dword ptr [esp + 0x20]
            mov edx, dword ptr [ebp + 0x18]
            and ecx, 0xff
            and eax, 0xff
            mov ecx, dword ptr [edx + ecx*4]
            lea eax, [eax + eax*2]
            lea eax, [ecx + eax*4]
            mov ecx, dword ptr [eax]
            test ecx, ecx
            je L0040de73
            mov edx, dword ptr [gRender_alternative_track_actors]
            test edx, edx
            je L0040de67
            push ecx
            jmp L0040de6b
    L0040de67:
            mov edx, dword ptr [eax + 4]
            push edx
    L0040de6b:
            call BrZbsSceneRenderAdd
            add esp, 4
    L0040de73:
            mov ecx, dword ptr [esp + 0x18]
            inc ecx
            dec esi
            mov dword ptr [esp + 0x18], ecx
            jne L0040ddd7
    L0040de83:
            mov ecx, dword ptr [esp + 0x24]
            mov eax, dword ptr [esp + 0x10]
            inc ecx
            dec eax
            mov dword ptr [esp + 0x24], ecx
            mov dword ptr [esp + 0x10], eax
            jne L0040dda4
            pop edi
            pop esi
            pop ebp
            pop ebx
            add esp, 0x3c
            ret 8
    L0040dea5:
            cmp cl, byte ptr [esp + 0x50]
            ja L0040dfcc
            mov ecx, dword ptr [esp + 0x50]
            mov eax, dword ptr [esp + 0x1c]
            and ecx, 0xff
            and eax, 0xff
            mov dword ptr [esp + 0x2c], ecx
            sub ecx, eax
            inc ecx
            mov dword ptr [esp + 0x30], eax
            mov dword ptr [esp + 0x24], eax
            mov dword ptr [esp + 0x1c], ecx
    L0040ded5:
            mov al, byte ptr [esp + 0x18]
            mov cl, byte ptr [esp + 0x10]
            cmp al, cl
            ja L0040dfb4
            mov ecx, dword ptr [esp + 0x10]
            mov ebx, dword ptr [esp + 0x18]
            and ecx, 0xff
            and ebx, 0xff
            mov esi, ecx
            mov dword ptr [esp + 0x50], ebx
            sub esi, ebx
            mov dword ptr [esp + 0x28], ecx
            inc esi
            jmp L0040df0c
    L0040df08:
            mov ecx, dword ptr [esp + 0x28]
    L0040df0c:
            mov edi, dword ptr [esp + 0x54]
            fld dword ptr [edi + 0x18]
            fcomp dword ptr [gFloat_0058935c+0x5c]
            fnstsw ax
            test ah, 0x41
            jne L0040df2a
            mov cl, byte ptr [esp + 0x50]
            mov byte ptr [esp + 0x20], cl
            jmp L0040df38
    L0040df2a:
            mov al, byte ptr [esp + 0x50]
            mov dl, bl
            add dl, cl
            sub dl, al
            mov byte ptr [esp + 0x20], dl
    L0040df38:
            fld dword ptr [edi + 0x20]
            fcomp dword ptr [gFloat_0058935c+0x5c]
            fnstsw ax
            test ah, 0x41
            jne L0040df52
            mov al, byte ptr [esp + 0x24]
            mov byte ptr [esp + 0x14], al
            jmp L0040df66
    L0040df52:
            mov cl, byte ptr [esp + 0x30]
            mov dl, byte ptr [esp + 0x2c]
            mov al, byte ptr [esp + 0x24]
            add cl, dl
            sub cl, al
            mov byte ptr [esp + 0x14], cl
    L0040df66:
            mov edx, dword ptr [esp + 0x14]
            mov eax, dword ptr [esp + 0x20]
            mov ecx, dword ptr [ebp + 0x18]
            and edx, 0xff
            and eax, 0xff
            mov ecx, dword ptr [ecx + edx*4]
            lea eax, [eax + eax*2]
            lea eax, [ecx + eax*4]
            mov ecx, dword ptr [eax]
            test ecx, ecx
            je L0040dfa4
            mov edx, dword ptr [gRender_alternative_track_actors]
            test edx, edx
            je L0040df98
            push ecx
            jmp L0040df9c
    L0040df98:
            mov edx, dword ptr [eax + 4]
            push edx
    L0040df9c:
            call BrZbsSceneRenderAdd
            add esp, 4
    L0040dfa4:
            mov ecx, dword ptr [esp + 0x50]
            inc ecx
            dec esi
            mov dword ptr [esp + 0x50], ecx
            jne L0040df08
    L0040dfb4:
            mov ecx, dword ptr [esp + 0x24]
            mov eax, dword ptr [esp + 0x1c]
            inc ecx
            dec eax
            mov dword ptr [esp + 0x24], ecx
            mov dword ptr [esp + 0x1c], eax
            jne L0040ded5
    L0040dfcc:
            pop edi
            pop esi
            pop ebp
            pop ebx
            add esp, 0x3c
            ret 8
    }
#endif
}
// AssertNoncars

// AllocateActorMatrix

// DisposeActorMatrix

// DisposeRuntimeBuiltModels

// DisposeColumns


// FUNCTION: CARMA2_HW 0x0040cb90
void C2_HOOK_FASTCALL XZToColumnXZ(tU8* pColumn_x, tU8* pColumn_z, br_scalar pX, br_scalar pZ, tTrack_spec* pTrack_spec) {
    br_scalar x;
    br_scalar z;
    br_scalar size_x = pTrack_spec->column_size_x;
    br_scalar size_z = pTrack_spec->column_size_z;

    x = (pX - pTrack_spec->origin_x) / size_x;
    z = (pZ - pTrack_spec->origin_z) / size_z;
    if (x < 0.0f) {
        x = 0.0f;
    }
    if (x >= pTrack_spec->ncolumns_x) {
        x = pTrack_spec->ncolumns_x - 1.0f;
    }
    if (z < 0.0f) {
        z = 0.0f;
    }
    if (z >= pTrack_spec->ncolumns_z) {
        z = pTrack_spec->ncolumns_z - 1.0f;
    }
    *pColumn_x = (tU8)x;
    *pColumn_z = (tU8)z;
}

// FindNonCarsCB

// MungeFaces

// ProcessModelsCB

// ProcessModels

// ExtractColumns

// RenderTrack

// FUNCTION: CARMA2_HW 0x0040dfe0
br_scalar C2_HOOK_STDCALL GetYonFactor(void) {
    return gYon_factor;
}

// FUNCTION: CARMA2_HW 0x0040dff0
void C2_HOOK_STDCALL SetYonFactor(br_scalar pNew) {
    gYon_factor = pNew;
}

// FoundAnActor

// ProcessNearbyActors