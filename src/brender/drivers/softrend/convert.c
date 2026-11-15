#include "convert.h"

#include "setup.h"

#define FCONV       (float)
#define XCONV(V)    (br_fixed_ls)((V)*65536.f)
#define ICONV       (br_int_32)

#ifdef CARPOCALYPSE2_MATCHING
extern void __cdecl _ftol();
extern rend_block_soft rend;
extern char rendererStateStoredDispatch[];
#endif
// FUNCTION: CARMA2_HW 0x005460e0
void C2_NAKED C2_HOOK_STDCALL ConvertVertex(brp_vertex* dest, brp_vertex* src) {
#ifndef CARPOCALYPSE2_MATCHING
    int c;
    br_uint_32 m;

    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(brp_block, convert_mask_f, 0x20);
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(brp_block, convert_mask_x, 0x24);
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(brp_block, convert_mask_i, 0x28);

    m = rend.block->convert_mask_f;
    if (m != 0) {
        for (c = 0; m != 0; c++, m >>= 1) {
            if (m & 0x1) {
                dest[0].comp_f[c] = FCONV(src->comp[c]);
            }
        }
    }

    m = rend.block->convert_mask_x;
    if (m != 0) {
        for (c = 0; m != 0; c++, m >>= 1) {
            if (m & 0x1) {
                dest[0].comp_x[c] = XCONV(src->comp[c]);
            }
        }
    }

    m = rend.block->convert_mask_i;
    if (m != 0) {
        for (c = 0; m != 0; c++, m >>= 1) {
            if (m & 0x1) {
                dest[0].comp_i[c] = ICONV(src->comp[c]);
            }
        }
    }

    m = 0x0000ffff ^ (rend.block->convert_mask_i | rend.block->convert_mask_f | rend.block->convert_mask_x);
    if (m != 0) {
        for (c = 0; m != 0; c++, m >>= 1) {
            if (m & 0x1) {
                dest[0].comp[c] = src->comp[c];
            }
        }
    }
#else
    __asm {
            mov eax, dword ptr [rend+0x4c]
            push ebx
            push ebp
            mov ebp, dword ptr [esp + 0xc]
            mov ecx, dword ptr [eax + 0x20]
            push esi
            test ecx, ecx
            push edi
            je L0054610d
            mov edx, dword ptr [esp + 0x18]
            mov eax, ebp
            sub edx, ebp
    L005460fc:
            test cl, 1
            je L00546106
            fld dword ptr [edx + eax]
            _emit 0xd9
        _emit 0x18
    L00546106:
            add eax, 4
            shr ecx, 1
            jne L005460fc
    L0054610d:
            mov ecx, dword ptr [rend+0x4c]
            mov ebx, dword ptr [ecx + 0x24]
            test ebx, ebx
            je L0054613e
            mov edi, dword ptr [esp + 0x18]
            mov esi, ebp
            sub edi, ebp
    L00546122:
            test bl, 1
            je L00546137
            fld dword ptr [esi + edi]
            _emit 0xd8
        _emit 0x0d
        _emit 0x20
        _emit 0xc9
        _emit 0x58
        _emit 0x00
            call _ftol
            mov dword ptr [esi], eax
    L00546137:
            add esi, 4
            shr ebx, 1
            jne L00546122
    L0054613e:
            mov edx, dword ptr [rend+0x4c]
            mov ebx, dword ptr [edx + 0x28]
            test ebx, ebx
            je L00546169
            mov edi, dword ptr [esp + 0x18]
            mov esi, ebp
            sub edi, ebp
    L00546153:
            test bl, 1
            je L00546162
            fld dword ptr [esi + edi]
            call _ftol
            mov dword ptr [esi], eax
    L00546162:
            add esi, 4
            shr ebx, 1
            jne L00546153
    L00546169:
            mov eax, dword ptr [rend+0x4c]
            mov ecx, dword ptr [eax + 0x28]
            mov esi, dword ptr [eax + 0x20]
            mov edx, dword ptr [eax + 0x24]
            or ecx, esi
            or ecx, edx
            xor ecx, 0xffff
            je L0054619c
            mov edx, dword ptr [esp + 0x18]
            mov eax, ebp
            sub edx, ebp
    L0054618b:
            test cl, 1
            je L00546195
            mov esi, dword ptr [eax + edx]
            mov dword ptr [eax], esi
    L00546195:
            add eax, 4
            shr ecx, 1
            jne L0054618b
    L0054619c:
            pop edi
            pop esi
            pop ebp
            pop ebx
            ret 8
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

// FUNCTION: CARMA2_HW 0x005461b0
void C2_HOOK_CDECL RenderConvert1(brp_block* block, brp_vertex* v0) {
    int c;
    br_uint_32 m;
    brp_vertex outv[1];

    m = rend.block->convert_mask_f;
    if (m != 0) {
        for (c = 0; m != 0; c++, m >>= 1) {
            if (m & 1) {
                outv[0].comp_f[c] = FCONV(v0->comp[c]);
            }
        }
    }

    m = rend.block->convert_mask_x;
    if (m != 0) {
        for (c = 0; m != 0; c++, m >>= 1) {
            if (m & 1) {
                outv[0].comp_x[c] = XCONV(v0->comp[c]);
            }
        }
    }

    m = rend.block->convert_mask_i;
    if (m != 0) {
        for (c = 0; m != 0; c++, m >>= 1) {
            if (m & 1) {
                outv[0].comp_i[c] = ICONV(v0->comp[c]);
            }
        }
    }

    m = 0x0000ffff ^ (rend.block->convert_mask_i | rend.block->convert_mask_x | rend.block->convert_mask_f);
    if (m != 0) {
        for (c = 0; m != 0; c++, m >>= 1) {
            if (m & 1) {
                outv[0].comp_i[c] = v0->comp_i[c];
            }
        }
    }

    block->chain->render(block->chain, outv);
}

// FUNCTION: CARMA2_HW 0x00546290
void C2_HOOK_CDECL RenderConvert2(brp_block* block, brp_vertex* v0, brp_vertex* v1) {
    int c;
    br_uint_32 m;
    brp_vertex outv[2];

    m = rend.block->convert_mask_f;
    if (m != 0) {
        for (c = 0; m != 0; c++, m >>= 1) {
            if (m & 1) {
                outv[0].comp_f[c] = FCONV(v0->comp[c]);
                outv[1].comp_f[c] = FCONV(v1->comp[c]);
            }
        }
    }

    m = rend.block->convert_mask_x;
    if (m != 0) {
        for (c = 0; m != 0; c++, m >>= 1) {
            if (m & 1) {
                outv[0].comp_x[c] = XCONV(v0->comp[c]);
                outv[1].comp_x[c] = XCONV(v1->comp[c]);
            }
        }
    }

    m = rend.block->convert_mask_i;
    if (m != 0) {
        for (c = 0; m != 0; c++, m >>= 1) {
            if (m & 1) {
                outv[0].comp_i[c] = ICONV(v0->comp[c]);
                outv[1].comp_i[c] = ICONV(v1->comp[c]);
            }
        }
    }

    m = 0x0000ffff ^ (rend.block->convert_mask_i | rend.block->convert_mask_x | rend.block->convert_mask_f);
    if (m != 0) {
        for (c = 0; m != 0; c++, m >>= 1) {
            if (m & 1) {
                outv[0].comp_i[c] = v0->comp_i[c];
                outv[1].comp_i[c] = v1->comp_i[c];
            }
        }
    }

    block->chain->render(block->chain, &outv[0], &outv[1]);
}

// FUNCTION: CARMA2_HW 0x005463f0
void C2_HOOK_CDECL RenderConvert3(brp_block* block, brp_vertex* v0, brp_vertex* v1, brp_vertex* v2) {
    int c;
    br_uint_32 m;
    brp_vertex outv[3];

    m = rend.block->convert_mask_f;
    if (m != 0) {
        for (c = 0; m != 0; c++, m >>= 1) {
            if (m & 1) {
                outv[0].comp_f[c] = FCONV(v0->comp[c]);
                outv[1].comp_f[c] = FCONV(v1->comp[c]);
                outv[2].comp_f[c] = FCONV(v2->comp[c]);
            }
        }
    }

    m = rend.block->convert_mask_x;
    if (m != 0) {
        for (c = 0; m != 0; c++, m >>= 1) {
            if (m & 1) {
                outv[0].comp_x[c] = XCONV(v0->comp[c]);
                outv[1].comp_x[c] = XCONV(v1->comp[c]);
                outv[2].comp_x[c] = XCONV(v2->comp[c]);
            }
        }
    }

    m = rend.block->convert_mask_i;
    if (m != 0) {
        for (c = 0; m != 0; c++, m >>= 1) {
            if (m & 1) {
                outv[0].comp_i[c] = ICONV (v0->comp[c]);
                outv[1].comp_i[c] = ICONV (v1->comp[c]);
                outv[2].comp_i[c] = ICONV (v2->comp[c]);
            }
        }
    }


    m = 0x0000ffff ^ (rend.block->convert_mask_i | rend.block->convert_mask_x | rend.block->convert_mask_f);
    if (m != 0) {
        for (c = 0; m != 0; c++, m >>= 1) {
            if (m & 1) {
                outv[0].comp_i[c] = v0->comp_i[c];
                outv[1].comp_i[c] = v1->comp_i[c];
                outv[2].comp_i[c] = v2->comp_i[c];
            }
        }
    }

    block->chain->render(block->chain, &outv[0], &outv[1], &outv[2]);
}

// FUNCTION: CARMA2_HW 0x005465e0
void C2_HOOK_CDECL RenderConvert4(brp_block* block, brp_vertex* v0, brp_vertex* v1, brp_vertex* v2, brp_vertex* v3) {
    int c;
    br_uint_32 m;
    brp_vertex outv[4];

    m = rend.block->convert_mask_f;
    if (m != 0) {
        for (c = 0; m != 0; c++, m >>= 1) {
            if (m & 1) {
                outv[0].comp_f[c] = FCONV(v0->comp[c]);
                outv[1].comp_f[c] = FCONV(v1->comp[c]);
                outv[2].comp_f[c] = FCONV(v2->comp[c]);
                outv[3].comp_f[c] = FCONV(v3->comp[c]);
            }
        }
    }

    m = rend.block->convert_mask_x;
    if (m != 0) {
        for (c = 0; m != 0; c++, m >>= 1) {
            if (m & 1) {
                outv[0].comp_x[c] = XCONV(v0->comp[c]);
                outv[1].comp_x[c] = XCONV(v1->comp[c]);
                outv[2].comp_x[c] = XCONV(v2->comp[c]);
                outv[3].comp_x[c] = XCONV(v3->comp[c]);
            }
        }
    }

    m = rend.block->convert_mask_i;
    if (m != 0) {
        for (c = 0; m != 0; c++, m >>= 1) {
            if (m & 1) {
                outv[0].comp_i[c] = ICONV (v0->comp[c]);
                outv[1].comp_i[c] = ICONV (v1->comp[c]);
                outv[2].comp_i[c] = ICONV (v2->comp[c]);
                outv[3].comp_i[c] = ICONV (v3->comp[c]);
            }
        }
    }

    m = 0x0000ffff ^ (rend.block->convert_mask_i | rend.block->convert_mask_x | rend.block->convert_mask_f);
    if (m != 0) {
        for (c = 0; m != 0; c++, m >>= 1) {
            if(m & 1) {
                outv[0].comp_i[c] = v0->comp_i[c];
                outv[1].comp_i[c] = v1->comp_i[c];
                outv[2].comp_i[c] = v2->comp_i[c];
                outv[3].comp_i[c] = v3->comp_i[c];
            }
        }
    }

    block->chain->render(block->chain, &outv[0], &outv[1], &outv[2], &outv[3]);
}
