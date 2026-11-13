#include "onscreen.h"

#define MAKE_EQN(sign,row)                                                      \
    do {                                                                        \
        eqn.v[0] =   model_to_screen->m[0][3] sign model_to_screen->m[0][row];  \
        eqn.v[1] =   model_to_screen->m[1][3] sign model_to_screen->m[1][row];  \
        eqn.v[2] =   model_to_screen->m[2][3] sign model_to_screen->m[2][row];  \
        eqn.v[3] = -(model_to_screen->m[3][3] sign model_to_screen->m[3][row]); \
    } while (0)

#define TEST_NOT_IN                                                             \
    (                                                                           \
        BR_MAC3(                                                                \
            eqn.v[0], ((eqn.v[0] > 0.f) ? bounds->min.v[0] : bounds->max.v[0]), \
            eqn.v[1], ((eqn.v[1] > 0.f) ? bounds->min.v[1] : bounds->max.v[1]), \
            eqn.v[2], ((eqn.v[2] > 0.f) ? bounds->min.v[2] : bounds->max.v[2])  \
        ) < eqn.v[3]                                                            \
    )

#define TEST_OUT                                                                \
    (                                                                           \
        BR_MAC3(                                                                \
            eqn.v[0], ((eqn.v[0] > 0) ? bounds->max.v[0] : (bounds->min.v[0])), \
            eqn.v[1], ((eqn.v[1] > 0) ? bounds->max.v[1] : (bounds->min.v[1])), \
            eqn.v[2], ((eqn.v[2] > 0) ? bounds->max.v[2] : (bounds->min.v[2]))  \
        ) < eqn.v[3]                                                            \
    )

#ifdef CARPOCALYPSE2_MATCHING
extern void C2_HOOK_CDECL BrMatrix4TApply();
extern char rendererStateStoredDispatch[];
#endif
// FUNCTION: CARMA2_HW 0x00544830
br_token C2_NAKED C2_HOOK_STDCALL OnScreenCheck(br_soft_renderer* self, br_matrix4* model_to_screen, br_bounds3* bounds) {
#ifndef CARPOCALYPSE2_MATCHING
    int accept = 1;
    br_vector4 eqn;
    int c;

    MAKE_EQN(+, 0);

    if (TEST_OUT) {
        return BRT_REJECT;
    }

    if (TEST_NOT_IN) {
        accept = 0;
    }

    MAKE_EQN(-, 0);

    if (TEST_OUT) {
        return BRT_REJECT;
    }

    if (accept && TEST_NOT_IN) {
        accept = 0;
    }

    MAKE_EQN(+, 1);

    if (TEST_OUT) {
        return BRT_REJECT;
    }

    if (accept && TEST_NOT_IN) {
        accept = 0;
    }

    MAKE_EQN(-, 1);

    if (TEST_OUT) {
        return BRT_REJECT;
    }

    if (accept && TEST_NOT_IN) {
        accept = 0;
    }

    MAKE_EQN(+, 2);

    if (TEST_OUT) {
        return BRT_REJECT;
    }

    if (accept && TEST_NOT_IN) {
        accept = 0;
    }

    MAKE_EQN(-, 2);

    if (TEST_OUT) {
        return BRT_REJECT;
    }

    if (accept && TEST_NOT_IN) {
        accept = 0;
    }

    for (c = 0; c < MAX_STATE_CLIP_PLANES; c++) {

        if (self->state.clip[c].type != BRT_PLANE) {
            continue;
        }

        BrMatrix4TApply(&eqn, &self->state.clip[c].plane, model_to_screen);
        eqn.v[3] = -eqn.v[3];

        if (TEST_OUT) {
            return BRT_REJECT;
        }

        if (accept && TEST_NOT_IN) {
            accept = 0;
        }
    }

    return accept ? BRT_ACCEPT : BRT_PARTIAL;
#else
    __asm {
            sub esp, 0x10
            push ebx
            push ebp
            mov ebp, dword ptr [esp + 0x20]
            mov ecx, 1
            push esi
            mov esi, dword ptr [esp + 0x28]
            _emit 0xd9
        _emit 0x45
        _emit 0x00
            fadd dword ptr [ebp + 0xc]
            fld dword ptr [ebp + 0x3c]
            fld dword ptr [ebp + 0x1c]
            fld st(2)
            fcomp dword ptr [rendererStateStoredDispatch+0x6c]
            fld dword ptr [ebp + 0x20]
            fxch st(2)
            fadd dword ptr [ebp + 0x30]
            fxch st(1)
            fadd dword ptr [ebp + 0x10]
            fxch st(2)
            fadd dword ptr [ebp + 0x2c]
            fxch st(1)
            fchs
            fxch st(1)
            fstp dword ptr [esp + 0x14]
            fnstsw ax
            push edi
            mov dword ptr [esp + 0x28], ecx
            fstp dword ptr [esp + 0x1c]
            test ah, 0x41
            jne L00544888
            fld dword ptr [esi + 0xc]
            jmp L0054488a
    L00544888:
            _emit 0xd9
        _emit 0x06
    L0054488a:
            fld st(1)
            fcomp dword ptr [rendererStateStoredDispatch+0x6c]
            fnstsw ax
            test ah, 0x41
            jne L0054489e
            fld dword ptr [esi + 0x10]
            jmp L005448a1
    L0054489e:
            fld dword ptr [esi + 4]
    L005448a1:
            fld dword ptr [esp + 0x18]
            fcomp dword ptr [rendererStateStoredDispatch+0x6c]
            fnstsw ax
            test ah, 0x41
            jne L005448b7
            fld dword ptr [esi + 0x14]
            jmp L005448ba
    L005448b7:
            fld dword ptr [esi + 8]
    L005448ba:
            fld st(3)
            fld st(5)
            fxch st(1)
            fmul st(0), st(3)
            fld dword ptr [esp + 0x18]
            fxch st(2)
            fmul st(0), st(5)
            fxch st(2)
            fmul st(0), st(3)
            fxch st(2)
            faddp st(1), st(0)
            fxch st(1)
            faddp st(1), st(0)
            fcomp dword ptr [esp + 0x1c]
            fstp st(0)
            fnstsw ax
            fstp st(0)
            test ah, 1
            fstp st(0)
            je L005448fa
            fstp st(0)
            fstp st(0)
            mov eax, 0x115
            pop edi
            pop esi
            pop ebp
            pop ebx
            add esp, 0x10
            ret 0xc
    L005448fa:
            fld st(1)
            fcomp dword ptr [rendererStateStoredDispatch+0x6c]
            fnstsw ax
            test ah, 0x41
            jne L0054490d
            _emit 0xd9
        _emit 0x06
            jmp L00544910
    L0054490d:
            fld dword ptr [esi + 0xc]
    L00544910:
            fld st(1)
            fcomp dword ptr [rendererStateStoredDispatch+0x6c]
            fnstsw ax
            test ah, 0x41
            jne L00544924
            fld dword ptr [esi + 4]
            jmp L00544927
    L00544924:
            fld dword ptr [esi + 0x10]
    L00544927:
            fld dword ptr [esp + 0x18]
            fcomp dword ptr [rendererStateStoredDispatch+0x6c]
            fnstsw ax
            test ah, 0x41
            jne L0054493d
            fld dword ptr [esi + 8]
            jmp L00544940
    L0054493d:
            fld dword ptr [esi + 0x14]
    L00544940:
            fxch st(3)
            fmul st(0), st(1)
            fld dword ptr [esp + 0x18]
            fxch st(5)
            fmul st(0), st(3)
            fxch st(5)
            fmul st(0), st(4)
            fxch st(5)
            faddp st(1), st(0)
            fxch st(4)
            xor ebx, ebx
            faddp st(4), st(0)
            fxch st(3)
            fcomp dword ptr [esp + 0x1c]
            fxch st(1)
            fstp st(0)
            fxch st(1)
            fstp st(0)
            fnstsw ax
            test ah, 1
            fstp st(0)
            je L00544977
            xor ecx, ecx
            mov dword ptr [esp + 0x28], ecx
    L00544977:
            fld dword ptr [ebp + 0xc]
            _emit 0xd8
        _emit 0x65
        _emit 0x00
            fld dword ptr [ebp + 0x3c]
            fld dword ptr [ebp + 0x1c]
            fld dword ptr [ebp + 0x2c]
            fxch st(3)
            fst dword ptr [esp + 0x10]
            fcomp dword ptr [rendererStateStoredDispatch+0x6c]
            fxch st(1)
            fsub dword ptr [ebp + 0x30]
            fxch st(1)
            fsub dword ptr [ebp + 0x10]
            fxch st(2)
            fsub dword ptr [ebp + 0x20]
            fxch st(1)
            fchs
            fxch st(2)
            fstp dword ptr [esp + 0x14]
            fnstsw ax
            fstp dword ptr [esp + 0x18]
            test ah, 0x41
            fstp dword ptr [esp + 0x1c]
            jne L005449bf
            fld dword ptr [esi + 0xc]
            jmp L005449c1
    L005449bf:
            _emit 0xd9
        _emit 0x06
    L005449c1:
            fld dword ptr [esp + 0x14]
            fcomp dword ptr [rendererStateStoredDispatch+0x6c]
            fnstsw ax
            test ah, 0x41
            jne L005449d7
            fld dword ptr [esi + 0x10]
            jmp L005449da
    L005449d7:
            fld dword ptr [esi + 4]
    L005449da:
            fld dword ptr [esp + 0x18]
            fcomp dword ptr [rendererStateStoredDispatch+0x6c]
            fnstsw ax
            test ah, 0x41
            jne L005449f0
            fld dword ptr [esi + 0x14]
            jmp L005449f3
    L005449f0:
            fld dword ptr [esi + 8]
    L005449f3:
            fld dword ptr [esp + 0x10]
            fld dword ptr [esp + 0x14]
            fmul st(0), st(3)
            fld dword ptr [esp + 0x18]
            fxch st(2)
            fmul st(0), st(5)
            fxch st(2)
            fmul st(0), st(3)
            fxch st(2)
            faddp st(1), st(0)
            fxch st(1)
            faddp st(1), st(0)
            fcomp dword ptr [esp + 0x1c]
            fstp st(0)
            fnstsw ax
            fstp st(0)
            test ah, 1
            fstp st(0)
            je L00544a31
            mov eax, 0x115
            pop edi
            pop esi
            pop ebp
            pop ebx
            add esp, 0x10
            ret 0xc
    L00544a31:
            cmp ecx, ebx
            je L00544ab4
            fld dword ptr [esp + 0x10]
            fcomp dword ptr [rendererStateStoredDispatch+0x6c]
            fnstsw ax
            test ah, 0x41
            jne L00544a4a
            _emit 0xd9
        _emit 0x06
            jmp L00544a4d
    L00544a4a:
            fld dword ptr [esi + 0xc]
    L00544a4d:
            fld dword ptr [esp + 0x14]
            fcomp dword ptr [rendererStateStoredDispatch+0x6c]
            fnstsw ax
            test ah, 0x41
            jne L00544a63
            fld dword ptr [esi + 4]
            jmp L00544a66
    L00544a63:
            fld dword ptr [esi + 0x10]
    L00544a66:
            fld dword ptr [esp + 0x18]
            fcomp dword ptr [rendererStateStoredDispatch+0x6c]
            fnstsw ax
            test ah, 0x41
            jne L00544a7c
            fld dword ptr [esi + 8]
            jmp L00544a7f
    L00544a7c:
            fld dword ptr [esi + 0x14]
    L00544a7f:
            fld dword ptr [esp + 0x10]
            fld dword ptr [esp + 0x14]
            fmul st(0), st(3)
            fld dword ptr [esp + 0x18]
            fxch st(2)
            fmul st(0), st(5)
            fxch st(2)
            fmul st(0), st(3)
            fxch st(2)
            faddp st(1), st(0)
            fxch st(1)
            faddp st(1), st(0)
            fcomp dword ptr [esp + 0x1c]
            fstp st(0)
            fnstsw ax
            fstp st(0)
            test ah, 1
            fstp st(0)
            je L00544ab4
            xor ecx, ecx
            mov dword ptr [esp + 0x28], ecx
    L00544ab4:
            fld dword ptr [ebp + 0xc]
            fadd dword ptr [ebp + 4]
            fld dword ptr [ebp + 0x3c]
            fld dword ptr [ebp + 0x1c]
            fld dword ptr [ebp + 0x2c]
            fxch st(3)
            fst dword ptr [esp + 0x10]
            fcomp dword ptr [rendererStateStoredDispatch+0x6c]
            fxch st(1)
            fadd dword ptr [ebp + 0x34]
            fxch st(1)
            fadd dword ptr [ebp + 0x14]
            fxch st(2)
            fadd dword ptr [ebp + 0x24]
            fxch st(1)
            fchs
            fxch st(2)
            fstp dword ptr [esp + 0x14]
            fnstsw ax
            fstp dword ptr [esp + 0x18]
            test ah, 0x41
            fstp dword ptr [esp + 0x1c]
            jne L00544afc
            fld dword ptr [esi + 0xc]
            jmp L00544afe
    L00544afc:
            _emit 0xd9
        _emit 0x06
    L00544afe:
            fld dword ptr [esp + 0x14]
            fcomp dword ptr [rendererStateStoredDispatch+0x6c]
            fnstsw ax
            test ah, 0x41
            jne L00544b14
            fld dword ptr [esi + 0x10]
            jmp L00544b17
    L00544b14:
            fld dword ptr [esi + 4]
    L00544b17:
            fld dword ptr [esp + 0x18]
            fcomp dword ptr [rendererStateStoredDispatch+0x6c]
            fnstsw ax
            test ah, 0x41
            jne L00544b2d
            fld dword ptr [esi + 0x14]
            jmp L00544b30
    L00544b2d:
            fld dword ptr [esi + 8]
    L00544b30:
            fld dword ptr [esp + 0x10]
            fld dword ptr [esp + 0x14]
            fmul st(0), st(3)
            fld dword ptr [esp + 0x18]
            fxch st(2)
            fmul st(0), st(5)
            fxch st(2)
            fmul st(0), st(3)
            fxch st(2)
            faddp st(1), st(0)
            fxch st(1)
            faddp st(1), st(0)
            fcomp dword ptr [esp + 0x1c]
            fstp st(0)
            fnstsw ax
            fstp st(0)
            test ah, 1
            fstp st(0)
            je L00544b6e
            mov eax, 0x115
            pop edi
            pop esi
            pop ebp
            pop ebx
            add esp, 0x10
            ret 0xc
    L00544b6e:
            cmp ecx, ebx
            je L00544bf1
            fld dword ptr [esp + 0x10]
            fcomp dword ptr [rendererStateStoredDispatch+0x6c]
            fnstsw ax
            test ah, 0x41
            jne L00544b87
            _emit 0xd9
        _emit 0x06
            jmp L00544b8a
    L00544b87:
            fld dword ptr [esi + 0xc]
    L00544b8a:
            fld dword ptr [esp + 0x14]
            fcomp dword ptr [rendererStateStoredDispatch+0x6c]
            fnstsw ax
            test ah, 0x41
            jne L00544ba0
            fld dword ptr [esi + 4]
            jmp L00544ba3
    L00544ba0:
            fld dword ptr [esi + 0x10]
    L00544ba3:
            fld dword ptr [esp + 0x18]
            fcomp dword ptr [rendererStateStoredDispatch+0x6c]
            fnstsw ax
            test ah, 0x41
            jne L00544bb9
            fld dword ptr [esi + 8]
            jmp L00544bbc
    L00544bb9:
            fld dword ptr [esi + 0x14]
    L00544bbc:
            fld dword ptr [esp + 0x10]
            fld dword ptr [esp + 0x14]
            fmul st(0), st(3)
            fld dword ptr [esp + 0x18]
            fxch st(2)
            fmul st(0), st(5)
            fxch st(2)
            fmul st(0), st(3)
            fxch st(2)
            faddp st(1), st(0)
            fxch st(1)
            faddp st(1), st(0)
            fcomp dword ptr [esp + 0x1c]
            fstp st(0)
            fnstsw ax
            fstp st(0)
            test ah, 1
            fstp st(0)
            je L00544bf1
            xor ecx, ecx
            mov dword ptr [esp + 0x28], ecx
    L00544bf1:
            fld dword ptr [ebp + 0xc]
            fsub dword ptr [ebp + 4]
            fld dword ptr [ebp + 0x3c]
            fld dword ptr [ebp + 0x1c]
            fld dword ptr [ebp + 0x2c]
            fxch st(3)
            fst dword ptr [esp + 0x10]
            fcomp dword ptr [rendererStateStoredDispatch+0x6c]
            fxch st(1)
            fsub dword ptr [ebp + 0x34]
            fxch st(1)
            fsub dword ptr [ebp + 0x14]
            fxch st(2)
            fsub dword ptr [ebp + 0x24]
            fxch st(1)
            fchs
            fxch st(2)
            fstp dword ptr [esp + 0x14]
            fnstsw ax
            fstp dword ptr [esp + 0x18]
            test ah, 0x41
            fstp dword ptr [esp + 0x1c]
            jne L00544c39
            fld dword ptr [esi + 0xc]
            jmp L00544c3b
    L00544c39:
            _emit 0xd9
        _emit 0x06
    L00544c3b:
            fld dword ptr [esp + 0x14]
            fcomp dword ptr [rendererStateStoredDispatch+0x6c]
            fnstsw ax
            test ah, 0x41
            jne L00544c51
            fld dword ptr [esi + 0x10]
            jmp L00544c54
    L00544c51:
            fld dword ptr [esi + 4]
    L00544c54:
            fld dword ptr [esp + 0x18]
            fcomp dword ptr [rendererStateStoredDispatch+0x6c]
            fnstsw ax
            test ah, 0x41
            jne L00544c6a
            fld dword ptr [esi + 0x14]
            jmp L00544c6d
    L00544c6a:
            fld dword ptr [esi + 8]
    L00544c6d:
            fld dword ptr [esp + 0x10]
            fld dword ptr [esp + 0x14]
            fmul st(0), st(3)
            fld dword ptr [esp + 0x18]
            fxch st(2)
            fmul st(0), st(5)
            fxch st(2)
            fmul st(0), st(3)
            fxch st(2)
            faddp st(1), st(0)
            fxch st(1)
            faddp st(1), st(0)
            fcomp dword ptr [esp + 0x1c]
            fstp st(0)
            fnstsw ax
            fstp st(0)
            test ah, 1
            fstp st(0)
            je L00544cab
            mov eax, 0x115
            pop edi
            pop esi
            pop ebp
            pop ebx
            add esp, 0x10
            ret 0xc
    L00544cab:
            cmp ecx, ebx
            je L00544d2e
            fld dword ptr [esp + 0x10]
            fcomp dword ptr [rendererStateStoredDispatch+0x6c]
            fnstsw ax
            test ah, 0x41
            jne L00544cc4
            _emit 0xd9
        _emit 0x06
            jmp L00544cc7
    L00544cc4:
            fld dword ptr [esi + 0xc]
    L00544cc7:
            fld dword ptr [esp + 0x14]
            fcomp dword ptr [rendererStateStoredDispatch+0x6c]
            fnstsw ax
            test ah, 0x41
            jne L00544cdd
            fld dword ptr [esi + 4]
            jmp L00544ce0
    L00544cdd:
            fld dword ptr [esi + 0x10]
    L00544ce0:
            fld dword ptr [esp + 0x18]
            fcomp dword ptr [rendererStateStoredDispatch+0x6c]
            fnstsw ax
            test ah, 0x41
            jne L00544cf6
            fld dword ptr [esi + 8]
            jmp L00544cf9
    L00544cf6:
            fld dword ptr [esi + 0x14]
    L00544cf9:
            fld dword ptr [esp + 0x10]
            fld dword ptr [esp + 0x14]
            fmul st(0), st(3)
            fld dword ptr [esp + 0x18]
            fxch st(2)
            fmul st(0), st(5)
            fxch st(2)
            fmul st(0), st(3)
            fxch st(2)
            faddp st(1), st(0)
            fxch st(1)
            faddp st(1), st(0)
            fcomp dword ptr [esp + 0x1c]
            fstp st(0)
            fnstsw ax
            fstp st(0)
            test ah, 1
            fstp st(0)
            je L00544d2e
            xor ecx, ecx
            mov dword ptr [esp + 0x28], ecx
    L00544d2e:
            fld dword ptr [ebp + 0xc]
            fadd dword ptr [ebp + 8]
            fld dword ptr [ebp + 0x3c]
            fld dword ptr [ebp + 0x1c]
            fld dword ptr [ebp + 0x2c]
            fxch st(3)
            fst dword ptr [esp + 0x10]
            fcomp dword ptr [rendererStateStoredDispatch+0x6c]
            fxch st(1)
            fadd dword ptr [ebp + 0x38]
            fxch st(1)
            fadd dword ptr [ebp + 0x18]
            fxch st(2)
            fadd dword ptr [ebp + 0x28]
            fxch st(1)
            fchs
            fxch st(2)
            fstp dword ptr [esp + 0x14]
            fnstsw ax
            fstp dword ptr [esp + 0x18]
            test ah, 0x41
            fstp dword ptr [esp + 0x1c]
            jne L00544d76
            fld dword ptr [esi + 0xc]
            jmp L00544d78
    L00544d76:
            _emit 0xd9
        _emit 0x06
    L00544d78:
            fld dword ptr [esp + 0x14]
            fcomp dword ptr [rendererStateStoredDispatch+0x6c]
            fnstsw ax
            test ah, 0x41
            jne L00544d8e
            fld dword ptr [esi + 0x10]
            jmp L00544d91
    L00544d8e:
            fld dword ptr [esi + 4]
    L00544d91:
            fld dword ptr [esp + 0x18]
            fcomp dword ptr [rendererStateStoredDispatch+0x6c]
            fnstsw ax
            test ah, 0x41
            jne L00544da7
            fld dword ptr [esi + 0x14]
            jmp L00544daa
    L00544da7:
            fld dword ptr [esi + 8]
    L00544daa:
            fld dword ptr [esp + 0x10]
            fld dword ptr [esp + 0x14]
            fmul st(0), st(3)
            fld dword ptr [esp + 0x18]
            fxch st(2)
            fmul st(0), st(5)
            fxch st(2)
            fmul st(0), st(3)
            fxch st(2)
            faddp st(1), st(0)
            fxch st(1)
            faddp st(1), st(0)
            fcomp dword ptr [esp + 0x1c]
            fstp st(0)
            fnstsw ax
            fstp st(0)
            test ah, 1
            fstp st(0)
            je L00544de8
            mov eax, 0x115
            pop edi
            pop esi
            pop ebp
            pop ebx
            add esp, 0x10
            ret 0xc
    L00544de8:
            cmp ecx, ebx
            je L00544e6b
            fld dword ptr [esp + 0x10]
            fcomp dword ptr [rendererStateStoredDispatch+0x6c]
            fnstsw ax
            test ah, 0x41
            jne L00544e01
            _emit 0xd9
        _emit 0x06
            jmp L00544e04
    L00544e01:
            fld dword ptr [esi + 0xc]
    L00544e04:
            fld dword ptr [esp + 0x14]
            fcomp dword ptr [rendererStateStoredDispatch+0x6c]
            fnstsw ax
            test ah, 0x41
            jne L00544e1a
            fld dword ptr [esi + 4]
            jmp L00544e1d
    L00544e1a:
            fld dword ptr [esi + 0x10]
    L00544e1d:
            fld dword ptr [esp + 0x18]
            fcomp dword ptr [rendererStateStoredDispatch+0x6c]
            fnstsw ax
            test ah, 0x41
            jne L00544e33
            fld dword ptr [esi + 8]
            jmp L00544e36
    L00544e33:
            fld dword ptr [esi + 0x14]
    L00544e36:
            fld dword ptr [esp + 0x10]
            fld dword ptr [esp + 0x14]
            fmul st(0), st(3)
            fld dword ptr [esp + 0x18]
            fxch st(2)
            fmul st(0), st(5)
            fxch st(2)
            fmul st(0), st(3)
            fxch st(2)
            faddp st(1), st(0)
            fxch st(1)
            faddp st(1), st(0)
            fcomp dword ptr [esp + 0x1c]
            fstp st(0)
            fnstsw ax
            fstp st(0)
            test ah, 1
            fstp st(0)
            je L00544e6b
            xor ecx, ecx
            mov dword ptr [esp + 0x28], ecx
    L00544e6b:
            fld dword ptr [ebp + 0xc]
            fsub dword ptr [ebp + 8]
            fld dword ptr [ebp + 0x3c]
            fld dword ptr [ebp + 0x1c]
            fld dword ptr [ebp + 0x2c]
            fxch st(3)
            fst dword ptr [esp + 0x10]
            fcomp dword ptr [rendererStateStoredDispatch+0x6c]
            fxch st(1)
            fsub dword ptr [ebp + 0x38]
            fxch st(1)
            fsub dword ptr [ebp + 0x18]
            fxch st(2)
            fsub dword ptr [ebp + 0x28]
            fxch st(1)
            fchs
            fxch st(2)
            fstp dword ptr [esp + 0x14]
            fnstsw ax
            fstp dword ptr [esp + 0x18]
            test ah, 0x41
            fstp dword ptr [esp + 0x1c]
            jne L00544eb3
            fld dword ptr [esi + 0xc]
            jmp L00544eb5
    L00544eb3:
            _emit 0xd9
        _emit 0x06
    L00544eb5:
            fld dword ptr [esp + 0x14]
            fcomp dword ptr [rendererStateStoredDispatch+0x6c]
            fnstsw ax
            test ah, 0x41
            jne L00544ecb
            fld dword ptr [esi + 0x10]
            jmp L00544ece
    L00544ecb:
            fld dword ptr [esi + 4]
    L00544ece:
            fld dword ptr [esp + 0x18]
            fcomp dword ptr [rendererStateStoredDispatch+0x6c]
            fnstsw ax
            test ah, 0x41
            jne L00544ee4
            fld dword ptr [esi + 0x14]
            jmp L00544ee7
    L00544ee4:
            fld dword ptr [esi + 8]
    L00544ee7:
            fld dword ptr [esp + 0x10]
            fld dword ptr [esp + 0x14]
            fmul st(0), st(3)
            fld dword ptr [esp + 0x18]
            fxch st(2)
            fmul st(0), st(5)
            fxch st(2)
            fmul st(0), st(3)
            fxch st(2)
            faddp st(1), st(0)
            fxch st(1)
            faddp st(1), st(0)
            fcomp dword ptr [esp + 0x1c]
            fstp st(0)
            fnstsw ax
            fstp st(0)
            test ah, 1
            fstp st(0)
            je L00544f25
            mov eax, 0x115
            pop edi
            pop esi
            pop ebp
            pop ebx
            add esp, 0x10
            ret 0xc
    L00544f25:
            cmp ecx, ebx
            je L00544fa6
            fld dword ptr [esp + 0x10]
            fcomp dword ptr [rendererStateStoredDispatch+0x6c]
            fnstsw ax
            test ah, 0x41
            jne L00544f3e
            _emit 0xd9
        _emit 0x06
            jmp L00544f41
    L00544f3e:
            fld dword ptr [esi + 0xc]
    L00544f41:
            fld dword ptr [esp + 0x14]
            fcomp dword ptr [rendererStateStoredDispatch+0x6c]
            fnstsw ax
            test ah, 0x41
            jne L00544f57
            fld dword ptr [esi + 4]
            jmp L00544f5a
    L00544f57:
            fld dword ptr [esi + 0x10]
    L00544f5a:
            fld dword ptr [esp + 0x18]
            fcomp dword ptr [rendererStateStoredDispatch+0x6c]
            fnstsw ax
            test ah, 0x41
            jne L00544f70
            fld dword ptr [esi + 8]
            jmp L00544f73
    L00544f70:
            fld dword ptr [esi + 0x14]
    L00544f73:
            fld dword ptr [esp + 0x10]
            fld dword ptr [esp + 0x14]
            fmul st(0), st(3)
            fld dword ptr [esp + 0x18]
            fxch st(2)
            fmul st(0), st(5)
            fxch st(2)
            fmul st(0), st(3)
            fxch st(2)
            faddp st(1), st(0)
            fxch st(1)
            faddp st(1), st(0)
            fcomp dword ptr [esp + 0x1c]
            fstp st(0)
            fnstsw ax
            fstp st(0)
            test ah, 1
            fstp st(0)
            je L00544fa6
            mov dword ptr [esp + 0x28], ebx
    L00544fa6:
            mov eax, dword ptr [esp + 0x24]
            lea edi, [eax + 0x434]
    L00544fb0:
            cmp dword ptr [edi - 4], 0xe1
            jne L005450e0
            push ebp
            lea ecx, [esp + 0x14]
            push edi
            push ecx
            call BrMatrix4TApply
            fld dword ptr [esp + 0x28]
            fchs
            fstp dword ptr [esp + 0x28]
            fld dword ptr [esp + 0x1c]
            fcomp dword ptr [rendererStateStoredDispatch+0x6c]
            add esp, 0xc
            fnstsw ax
            test ah, 0x41
            jne L00544fec
            fld dword ptr [esi + 0xc]
            jmp L00544fee
    L00544fec:
            _emit 0xd9
        _emit 0x06
    L00544fee:
            fld dword ptr [esp + 0x14]
            fcomp dword ptr [rendererStateStoredDispatch+0x6c]
            fnstsw ax
            test ah, 0x41
            jne L00545004
            fld dword ptr [esi + 0x10]
            jmp L00545007
    L00545004:
            fld dword ptr [esi + 4]
    L00545007:
            fld dword ptr [esp + 0x18]
            fcomp dword ptr [rendererStateStoredDispatch+0x6c]
            fnstsw ax
            test ah, 0x41
            jne L0054501d
            fld dword ptr [esi + 0x14]
            jmp L00545020
    L0054501d:
            fld dword ptr [esi + 8]
    L00545020:
            fld dword ptr [esp + 0x10]
            fld dword ptr [esp + 0x14]
            fmul st(0), st(3)
            fld dword ptr [esp + 0x18]
            fxch st(2)
            fmul st(0), st(5)
            fxch st(2)
            fmul st(0), st(3)
            fxch st(2)
            faddp st(1), st(0)
            fxch st(1)
            faddp st(1), st(0)
            fcomp dword ptr [esp + 0x1c]
            fstp st(0)
            fnstsw ax
            fstp st(0)
            test ah, 1
            fstp st(0)
            jne L00545107
            mov eax, dword ptr [esp + 0x28]
            test eax, eax
            je L005450e0
            fld dword ptr [esp + 0x10]
            fcomp dword ptr [rendererStateStoredDispatch+0x6c]
            fnstsw ax
            test ah, 0x41
            jne L00545074
            _emit 0xd9
        _emit 0x06
            jmp L00545077
    L00545074:
            fld dword ptr [esi + 0xc]
    L00545077:
            fld dword ptr [esp + 0x14]
            fcomp dword ptr [rendererStateStoredDispatch+0x6c]
            fnstsw ax
            test ah, 0x41
            jne L0054508d
            fld dword ptr [esi + 4]
            jmp L00545090
    L0054508d:
            fld dword ptr [esi + 0x10]
    L00545090:
            fld dword ptr [esp + 0x18]
            fcomp dword ptr [rendererStateStoredDispatch+0x6c]
            fnstsw ax
            test ah, 0x41
            jne L005450a6
            fld dword ptr [esi + 8]
            jmp L005450a9
    L005450a6:
            fld dword ptr [esi + 0x14]
    L005450a9:
            fld dword ptr [esp + 0x10]
            fld dword ptr [esp + 0x14]
            fmul st(0), st(3)
            fld dword ptr [esp + 0x18]
            fxch st(2)
            fmul st(0), st(5)
            fxch st(2)
            fmul st(0), st(3)
            fxch st(2)
            faddp st(1), st(0)
            fxch st(1)
            faddp st(1), st(0)
            fcomp dword ptr [esp + 0x1c]
            fstp st(0)
            fnstsw ax
            fstp st(0)
            test ah, 1
            fstp st(0)
            je L005450e0
            mov dword ptr [esp + 0x28], 0
    L005450e0:
            inc ebx
            add edi, 0x14
            cmp ebx, 6
            jl L00544fb0
            mov edx, dword ptr [esp + 0x28]
            xor eax, eax
            test edx, edx
            setne al
            add eax, 0x113
            pop edi
            pop esi
            pop ebp
            pop ebx
            add esp, 0x10
            ret 0xc
    L00545107:
            pop edi
            pop esi
            pop ebp
            mov eax, 0x115
            pop ebx
            add esp, 0x10
            ret 0xc
    }
#endif
}
