#include "tokenval.h"

#include "brprintf.h"
#include "lexer.h"
#include "resource.h"
#include "token.h"

#include "core/math/fixed.h"
#include "core/std/brstdlib.h"

#include "c2_stdlib.h"

static br_error parseTokenValue(br_lexer* l, br_token_value* tv, br_size_t size);

#ifdef CARPOCALYPSE2_MATCHING
extern void* C2_HOOK_CDECL BrResAllocate();
extern br_token C2_HOOK_CDECL BrTokenCreate();
#endif
#ifdef CARPOCALYPSE2_MATCHING
extern br_lexer* C2_HOOK_CDECL BrLexerAllocate();
extern void C2_HOOK_CDECL BrLexerFree();
extern br_error C2_HOOK_CDECL BrLexerPushString();
extern int C2_HOOK_CDECL BrMemCmp();
extern void* C2_HOOK_CDECL BrMemCpy();
extern int C2_HOOK_CDECL BrStrCmp();
extern char* C2_HOOK_CDECL BrStrCpy();
extern br_size_t C2_HOOK_CDECL BrStrLen();
extern char* C2_HOOK_CDECL BrStrNCpy();
extern br_token C2_HOOK_CDECL BrTokenFind();
extern br_token C2_HOOK_CDECL BrTokenFindType();
extern br_token C2_HOOK_CDECL BrTokenType();
extern void __cdecl _ftol();
#endif
// FUNCTION: CARMA2_HW 0x0052d740
br_tv_template* C2_HOOK_CDECL BrTVTemplateAllocate(void* res, br_tv_template_entry* entries, int n_entries) {
    br_tv_template* t;

    t = BrResAllocate(res, sizeof(br_tv_template), BR_MEMORY_TOKEN_TEMPLATE);
    t->res = t;
    t->entries = entries;
    t->n_entries = n_entries;
    return t;
}

// FUNCTION: CARMA2_HW 0x0052d770
void C2_HOOK_CDECL BrTVTemplateFree(br_tv_template* t) {

    BrResFree(t);
}

void C2_HOOK_STDCALL templateResolveNames(br_tv_template* template) {
    br_tv_template_entry* tp;
    int n;

    for (n = 0; n < template->n_map_entries; n++) {
        tp = &template->entries[n];
        if (tp->name != NULL) {
            tp->token = BrTokenCreate(tp->name, BR_NULL_TOKEN);
        }
    }
}

// FUNCTION: CARMA2_HW 0x0052d7f0
void C2_NAKED C2_HOOK_STDCALL templateMakeMap(br_tv_template* template) {
#ifndef CARPOCALYPSE2_MATCHING
    br_tv_template_entry* tp;
    br_int_32 min;
    br_int_32 max;
    int i;
    int n;
    int e;

    if (!template->names_resolved) {
        templateResolveNames(template);
    }
    max = min = template->entries[0].token;
    for (i = 0; i < template->n_entries; i++) {
        tp = &template->entries[i];
        if (tp->token < min) {
            min = tp->token;
        }
        if (tp->token > max) {
            max = tp->token;
        }
    }
    n = max - min + 1;
    template->map_base = min;
    template->n_map_entries = n;

    template->map_query_entry = BrResAllocate(template->res, 2 * n * sizeof(br_tv_template_entry*), BR_MEMORY_TOKEN_MAP);
    template->map_set_entry = &template->map_query_entry[n];
    for (i = 0; i < n; i++) {
        template->map_query_entry[i] = NULL;
        template->map_set_entry[i] = NULL;
    }

    for (i = 0; i < template->n_entries; i++) {
        tp = &template->entries[i];
        e = tp->token - template->map_base;
        if (tp->flags & 0x1) {
            template->map_query_entry[e] = tp;
        }
        if (tp->flags & 0x2) {
            template->map_set_entry[e] = tp;
        }
    }
#else
    __asm {
            push ebx
            push esi
            mov esi, dword ptr [esp + 0xc]
            push edi
            mov eax, dword ptr [esi + 0x18]
            test eax, eax
            jne L0052d827
            mov eax, dword ptr [esi]
            mov edi, dword ptr [esi + 4]
            xor ebx, ebx
            test eax, eax
            jle L0052d827
    L0052d809:
            mov eax, dword ptr [edi + 4]
            test eax, eax
            je L0052d81d
            push 0
            push eax
            call BrTokenCreate
            add esp, 8
            mov dword ptr [edi], eax
    L0052d81d:
            mov eax, dword ptr [esi]
            add edi, 0x18
            inc ebx
            cmp ebx, eax
            jl L0052d809
    L0052d827:
            mov eax, dword ptr [esi + 4]
            mov edx, eax
            mov edi, dword ptr [eax]
            mov eax, dword ptr [esi]
            test eax, eax
            mov ecx, edi
            jle L0052d850
            mov ebx, eax
    L0052d838:
            mov eax, dword ptr [edx]
            test eax, eax
            je L0052d84a
            cmp eax, ecx
            jge L0052d844
            mov ecx, eax
    L0052d844:
            cmp eax, edi
            jle L0052d84a
            mov edi, eax
    L0052d84a:
            add edx, 0x18
            dec ebx
            jne L0052d838
    L0052d850:
            mov edx, dword ptr [esi + 0x1c]
            sub edi, ecx
            inc edi
            mov dword ptr [esi + 8], ecx
            push 0x21
            mov dword ptr [esi + 0xc], edi
            lea ecx, [edi*8]
            push ecx
            push edx
            call BrResAllocate
            mov dword ptr [esi + 0x10], eax
            lea eax, [eax + edi*4]
            mov dword ptr [esi + 0x14], eax
            add esp, 0xc
            xor eax, eax
            test edi, edi
            jle L0052d899
    L0052d87e:
            mov ecx, dword ptr [esi + 0x10]
            inc eax
            cmp eax, edi
            mov dword ptr [ecx + eax*4 - 4], 0
            mov edx, dword ptr [esi + 0x14]
            mov dword ptr [edx + eax*4 - 4], 0
            jl L0052d87e
    L0052d899:
            mov eax, dword ptr [esi]
            mov ecx, dword ptr [esi + 4]
            xor edx, edx
            test eax, eax
            jle L0052d8d0
            mov bl, 1
    L0052d8a6:
            mov eax, dword ptr [ecx]
            test eax, eax
            je L0052d8c6
            sub eax, dword ptr [esi + 8]
            test byte ptr [ecx + 0xc], bl
            je L0052d8ba
            mov edi, dword ptr [esi + 0x10]
            mov dword ptr [edi + eax*4], ecx
    L0052d8ba:
            test byte ptr [ecx + 0xc], 2
            je L0052d8c6
            mov edi, dword ptr [esi + 0x14]
            mov dword ptr [edi + eax*4], ecx
    L0052d8c6:
            mov eax, dword ptr [esi]
            add ecx, 0x18
            inc edx
            cmp edx, eax
            jl L0052d8a6
    L0052d8d0:
            pop edi
            pop esi
            pop ebx
            ret 4
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

br_fixed_ls* C2_HOOK_STDCALL ConvertFloatToFixed(br_fixed_ls** pextra, br_float* src, br_int_32 count, br_size_t* pextra_space) {
    br_fixed_ls* ret;

    if (pextra == NULL || *pextra == NULL || pextra_space == NULL) {
        return NULL;
    }
    ret = *pextra;
    if (*pextra_space < sizeof(uint32_t) * count) {
        return 0;
    }
    *pextra_space -= sizeof(br_fixed_ls) * count;
    for (; count != 0; count--) {
        **pextra = (br_fixed_ls)(*src * 65536.f);
        (*pextra)++;
        src++;
    }
    return ret;
}

br_float* C2_HOOK_STDCALL ConvertFixedToFloat(br_float** pextra, br_fixed_ls* src, br_int_32 count, br_size_t* pextra_space) {
    br_float* ret;

    if (pextra == NULL || *pextra == NULL || pextra_space == NULL) {
        return NULL;
    }
    ret = *pextra;
    if (*pextra_space < sizeof(float) * count) {
        return 0;
    }
    *pextra_space -= sizeof(float) * count;
    for (; count != 0; count--) {
        **pextra = (float)*src / 65536.f;
        (*pextra)++;
        src++;
    }
    return ret;
}

br_uint_32* C2_HOOK_STDCALL ConvertLongCopy(br_uint_32** pextra, br_uint_32* src, br_int_32 count, br_size_t* pextra_space) {
    br_uint_32* ret;

    if (pextra == NULL || *pextra == NULL || pextra_space == NULL) {
        return NULL;
    }
    ret = *pextra;
    if (*pextra_space < sizeof(uint32_t) * count) {
        return NULL;
    }
    *pextra_space -= sizeof(uint32_t) * count;
    for (; count != 0; count--) {
        **pextra = *src;
        (*pextra)++;
        src++;
    }
    return ret;
}

// FUNCTION: CARMA2_HW 0x0052d8e0
br_error C2_NAKED C2_HOOK_STDCALL ValueQuery(br_token_value* tv, void** pextra, br_size_t* pextra_size, void* block, br_tv_template_entry* tep) {
#ifndef CARPOCALYPSE2_MATCHING
    void* mem;
    br_uint_32* lp;
    int t;
    br_tv_custom* custp;

    if (tep->flags & 0x8) {
        /* absolute */
        mem = &tep->offset;
    } else {
        /* relative */
        mem = (br_uint_8*)block + tep->offset;
    }

    switch (tep->conv) {
    case 1:
        /* const */
        tv->v.u32 = (br_uint_32)tep->conv_arg;
        break;
    case 2:
        /* custom */
        custp = (br_tv_custom*)tep->conv_arg;
        if (custp == NULL || custp->query == NULL) {
            return 0x1001;
        }
        return custp->query(&tv->v.u32, pextra, pextra_size, block, tep);
#ifndef CARPOCALYPSE2_MATCHING
    case 33:
        /* full-width pointer (64-bit hosts) */
        *(void**)&tv->v = *(void**)mem;
        break;
#endif
    case 3:
        /* void* */
        tv->v.u32 = *(br_uint_32*)mem;
        break;
    case 4:
        /* vec2 (default=float) */
        tv->v.p = ConvertLongCopy((br_uint_32**)pextra, (br_uint_32*)mem, 2, pextra_size);
        if (tv->v.p == NULL) {
            return 0x1004;
        }
        break;
    case 5:
        /* vec3 (default=float) */
        tv->v.p = ConvertLongCopy((br_uint_32**)pextra, (br_uint_32*)mem, 3, pextra_size);
        if (tv->v.p == NULL) {
            return 0x1004;
        }
        break;
    case 6:
        /* vec4 (default=float) */
        tv->v.p = ConvertLongCopy((br_uint_32**)pextra, (br_uint_32*)mem, 4, pextra_size);
        if (tv->v.p == NULL) {
            return 0x1004;
        }
        break;
    case 7:
        /* mat23 (default=float) */
        tv->v.p = ConvertLongCopy((br_uint_32**)pextra, (br_uint_32*)mem, 6, pextra_size);
        if (tv->v.p == NULL) {
            return 0x1004;
        }
        break;
    case 8:
        /* mat34 (default=float) */
        tv->v.p = ConvertLongCopy((br_uint_32**)pextra, (br_uint_32*)mem, 12, pextra_size);
        if (tv->v.p == NULL) {
            return 0x1004;
        }
        break;
    case 9:
        /* mat4 (default=float) */
        tv->v.p = ConvertLongCopy((br_uint_32**)pextra, (br_uint_32*)mem, 16, pextra_size);
        if (tv->v.p == NULL) {
            return 0x1004;
        }
        break;
    case 10:
        /* i8 */
        tv->v.i8 = *(br_int_8*)mem;
        break;
    case 11:
        /* i16 */
        tv->v.i16 = *(br_int_16*)mem;
        break;
    case 12:
        /* u8 */
        tv->v.u8 = *(br_uint_8*)mem;
        break;
    case 13:
        /* u16 */
        tv->v.u16 = *(br_uint_16*)mem;
        break;
    case 14:
        /* fixed */
        tv->v.x = (br_fixed_ls)(65536.f * *(float*)mem);
        break;
    case 15:
        /* float */
        tv->v.s = (br_fixed_ls)*(br_fixed_ls*)mem / 65536.f;
        break;
    case 16:
        /* vec2f_to_x */
        tv->v.p = ConvertFloatToFixed((br_fixed_ls**)pextra, (float*)mem, 2, pextra_size);
        if (tv->v.p == NULL) {
            return 0x1004;
        }
        break;
    case 17:
        /* vec2x_to_f */
        tv->v.p = ConvertFixedToFloat((float**)pextra, (br_fixed_ls*)mem, 2, pextra_size);
        if (tv->v.p == NULL) {
            return 0x1004;
        }
        break;
    case 18:
        /* vec3f_to_x */
        tv->v.p = ConvertFloatToFixed((br_fixed_ls**)pextra, (float*)mem, 3, pextra_size);
        if (tv->v.p == NULL) {
            return 0x1004;
        }
        break;
    case 19:
        /* vec3x_to_f */
        tv->v.p = ConvertFixedToFloat((float**)pextra, (br_fixed_ls*)mem, 3, pextra_size);
        if (tv->v.p == NULL) {
            return 0x1004;
        }
        break;
    case 20:
        /* vec4f_to_x */
        tv->v.p = ConvertFloatToFixed((br_fixed_ls**)pextra, (float*)mem, 4, pextra_size);
        if (tv->v.p == NULL) {
            return 0x1004;
        }
        break;
    case 21:
        /* vec4x_to_f */
        tv->v.p = ConvertFixedToFloat((float**)pextra, (br_fixed_ls*)mem, 4, pextra_size);
        if (tv->v.p == NULL) {
            return 0x1004;
        }
        break;
    case 22:
        /* mat23f_to_x */
        tv->v.p = ConvertFloatToFixed((br_fixed_ls**)pextra, (float*)mem, 6, pextra_size);
        if (tv->v.p == NULL) {
            return 0x1004;
        }
        break;
    case 23:
        /* mat23x_to_f */
        tv->v.p = ConvertFixedToFloat((float**)pextra, (br_fixed_ls*)mem, 6, pextra_size);
        if (tv->v.p == NULL) {
            return 0x1004;
        }
        break;
    case 24:
        /* mat34f_to_x */
        tv->v.p = ConvertFloatToFixed((br_fixed_ls**)pextra, (float*)mem, 12, pextra_size);
        if (tv->v.p == NULL) {
            return 0x1004;
        }
        break;
    case 25:
        /* mat34x_to_f */
        tv->v.p = ConvertFixedToFloat((float**)pextra, (br_fixed_ls*)mem, 12, pextra_size);
        if (tv->v.p == NULL) {
            return 0x1004;
        }
        break;
    case 26:
        /* mat4f_to_x */
        tv->v.p = ConvertFloatToFixed((br_fixed_ls**)pextra, (float*)mem, 16, pextra_size);
        if (tv->v.p == NULL) {
            return 0x1004;
        }
        break;
    case 27:
        /* mat4x_to_f */
        tv->v.p = ConvertFixedToFloat((float**)pextra, (br_fixed_ls*)mem, 16, pextra_size);
        if (tv->v.p == NULL) {
            return 0x1004;
        }
        break;
    case 28:
        /* string */
        tv->v.cstr = (char*)ConvertLongCopy((br_uint_32**)pextra, (br_uint_32*)*(char**)mem, (BrStrLen(*(const char**)mem) + 1 + sizeof(br_uint_32)) / sizeof(br_uint_32), pextra_size);
        if (tv->v.cstr == NULL) {
            return 0x1004;
        }
        break;
    case 29:
        /* null terminated array */
        lp = *(br_uint_32**)mem;
        t = 0;
        if (lp != NULL) {
            while (*lp++) {
                t++;
            }
            tv->v.p = ConvertLongCopy((br_uint_32**)pextra, *(br_uint_32**)mem, t + 1, pextra_size);
            if (tv->v.p == NULL) {
                return 0x1004;
            }
        } else {
            tv->v.p = ConvertLongCopy((br_uint_32**)pextra, (br_uint_32*)&mem, 1, pextra_size);
            if (tv->v.p == NULL) {
                return 0x1004;
            }
        }
        break;
    case 30:
        /* Invert Boolean */
        tv->v.b = (*(br_uint_32*)mem) != 0x1;
        break;
    case 31:
        /* Boolean mask */
        tv->v.b = !!((*(br_uint_32*)mem) & tep->conv_arg);
        break;
    case 32:
        /* Boolean not mask */
        tv->v.b = !((*(br_uint_32*)mem) & tep->conv_arg);
        break;
    default:
        return 0x1001;
    }
    return 0;
#else
    __asm {
            mov eax, dword ptr [esp + 0x14]
            mov edx, dword ptr [esp + 0x10]
            push ebx
            push ebp
            mov cl, byte ptr [eax + 0xc]
            push esi
            test cl, 8
            push edi
            je L0052d8f9
            lea esi, [eax + 8]
            jmp L0052d8fe
    L0052d8f9:
            mov esi, dword ptr [eax + 8]
            add esi, edx
    L0052d8fe:
            movsx ecx, word ptr [eax + 0xe]
            dec ecx
            cmp ecx, 0x1f
            ja L0052e1a3
            jmp dword ptr [ecx*4 + 0x52e1b0]
            mov ecx, dword ptr [esp + 0x14]
            mov eax, dword ptr [esi]
            mov dword ptr [ecx + 4], eax
            xor eax, eax
            pop edi
            pop esi
            pop ebp
            pop ebx
            ret 0x14
            mov edx, dword ptr [eax + 0x10]
            mov eax, dword ptr [esp + 0x14]
            mov dword ptr [eax + 4], edx
            xor eax, eax
            pop edi
            pop esi
            pop ebp
            pop ebx
            ret 0x14
            movsx ecx, byte ptr [esi]
            mov edx, dword ptr [esp + 0x14]
            xor eax, eax
            mov dword ptr [edx + 4], ecx
            pop edi
            pop esi
            pop ebp
            pop ebx
            ret 0x14
            movsx eax, word ptr [esi]
            mov ecx, dword ptr [esp + 0x14]
            mov dword ptr [ecx + 4], eax
            xor eax, eax
            pop edi
            pop esi
            pop ebp
            pop ebx
            ret 0x14
            mov eax, dword ptr [esp + 0x14]
            xor edx, edx
            mov dl, byte ptr [esi]
            mov dword ptr [eax + 4], edx
            xor eax, eax
            pop edi
            pop esi
            pop ebp
            pop ebx
            ret 0x14
            mov edx, dword ptr [esp + 0x14]
            xor ecx, ecx
            mov cx, word ptr [esi]
            xor eax, eax
            mov dword ptr [edx + 4], ecx
            pop edi
            pop esi
            pop ebp
            pop ebx
            ret 0x14
            mov eax, dword ptr [eax + 0x10]
            mov edi, dword ptr [esi]
            mov ecx, dword ptr [esp + 0x14]
            and eax, edi
            neg eax
            sbb eax, eax
            neg eax
            mov dword ptr [ecx + 4], eax
            xor eax, eax
            pop edi
            pop esi
            pop ebp
            pop ebx
            ret 0x14
            mov edx, dword ptr [eax + 0x10]
            mov edi, dword ptr [esi]
            mov eax, dword ptr [esp + 0x14]
            and edx, edi
            neg edx
            sbb edx, edx
            inc edx
            mov dword ptr [eax + 4], edx
            xor eax, eax
            pop edi
            pop esi
            pop ebp
            pop ebx
            ret 0x14
            _emit 0xd9
        _emit 0x06
            fmul dword ptr ds:[0x58b868]
            call _ftol
            mov ecx, dword ptr [esp + 0x14]
            mov dword ptr [ecx + 4], eax
            xor eax, eax
            pop edi
            pop esi
            pop ebp
            pop ebx
            ret 0x14
            _emit 0xdb
        _emit 0x06
            mov edx, dword ptr [esp + 0x14]
            xor eax, eax
            fmul qword ptr ds:[0x58b870]
            fstp dword ptr [edx + 4]
            pop edi
            pop esi
            pop ebp
            pop ebx
            ret 0x14
            mov edi, dword ptr [esp + 0x18]
            test edi, edi
            je L0052e143
            mov ebp, dword ptr [edi]
            test ebp, ebp
            je L0052e143
            mov ecx, dword ptr [esp + 0x1c]
            test ecx, ecx
            je L0052e143
            mov eax, dword ptr [ecx]
            cmp eax, 8
            jb L0052e143
            add eax, -8
            mov ebx, 2
            mov dword ptr [ecx], eax
    L0052da2c:
            _emit 0xd9
        _emit 0x06
            fmul dword ptr ds:[0x58b868]
            call _ftol
            mov ecx, dword ptr [edi]
            add esi, 4
            mov dword ptr [ecx], eax
            mov edx, dword ptr [edi]
            add edx, 4
            dec ebx
            mov dword ptr [edi], edx
            jne L0052da2c
            mov eax, ebp
            jmp L0052e145
            mov eax, dword ptr [esp + 0x18]
            test eax, eax
            je L0052e0e7
            mov edi, dword ptr [eax]
            test edi, edi
            je L0052e0e7
            mov edx, dword ptr [esp + 0x1c]
            test edx, edx
            je L0052e0e7
            mov ecx, dword ptr [edx]
            cmp ecx, 8
            jb L0052e0e7
            add ecx, -8
            mov dword ptr [edx], ecx
            mov ecx, 2
    L0052da88:
            _emit 0xdb
        _emit 0x06
            mov edx, dword ptr [eax]
            add esi, 4
            fmul qword ptr ds:[0x58b870]
            _emit 0xd9
        _emit 0x1a
            mov ebx, dword ptr [eax]
            add ebx, 4
            dec ecx
            mov dword ptr [eax], ebx
            jne L0052da88
            mov eax, edi
            jmp L0052e0e9
            mov edi, dword ptr [esp + 0x18]
            test edi, edi
            je L0052e0e7
            mov ebp, dword ptr [edi]
            test ebp, ebp
            je L0052e0e7
            mov ecx, dword ptr [esp + 0x1c]
            test ecx, ecx
            je L0052e0e7
            mov eax, dword ptr [ecx]
            cmp eax, 0xc
            jb L0052e0e7
            add eax, -0xc
            mov ebx, 3
            mov dword ptr [ecx], eax
    L0052dadf:
            _emit 0xd9
        _emit 0x06
            fmul dword ptr ds:[0x58b868]
            call _ftol
            mov edx, dword ptr [edi]
            add esi, 4
            mov dword ptr [edx], eax
            mov edx, dword ptr [edi]
            add edx, 4
            dec ebx
            mov dword ptr [edi], edx
            jne L0052dadf
            mov eax, ebp
            jmp L0052e0e9
            mov eax, dword ptr [esp + 0x18]
            test eax, eax
            je L0052e0e7
            mov edi, dword ptr [eax]
            test edi, edi
            je L0052e0e7
            mov edx, dword ptr [esp + 0x1c]
            test edx, edx
            je L0052e0e7
            mov ecx, dword ptr [edx]
            cmp ecx, 0xc
            jb L0052e0e7
            add ecx, -0xc
            mov dword ptr [edx], ecx
            mov ecx, 3
    L0052db3b:
            _emit 0xdb
        _emit 0x06
            mov edx, dword ptr [eax]
            add esi, 4
            fmul qword ptr ds:[0x58b870]
            _emit 0xd9
        _emit 0x1a
            mov ebx, dword ptr [eax]
            add ebx, 4
            dec ecx
            mov dword ptr [eax], ebx
            jne L0052db3b
            mov eax, edi
            jmp L0052e0e9
            mov edi, dword ptr [esp + 0x18]
            test edi, edi
            je L0052e0e7
            mov ebp, dword ptr [edi]
            test ebp, ebp
            je L0052e0e7
            mov ecx, dword ptr [esp + 0x1c]
            test ecx, ecx
            je L0052e0e7
            mov eax, dword ptr [ecx]
            cmp eax, 0x10
            jb L0052e0e7
            add eax, -0x10
            mov ebx, 4
            mov dword ptr [ecx], eax
    L0052db92:
            _emit 0xd9
        _emit 0x06
            fmul dword ptr ds:[0x58b868]
            call _ftol
            mov edx, dword ptr [edi]
            add esi, 4
            mov dword ptr [edx], eax
            mov edx, dword ptr [edi]
            add edx, 4
            dec ebx
            mov dword ptr [edi], edx
            jne L0052db92
            mov eax, ebp
            jmp L0052e0e9
            mov eax, dword ptr [esp + 0x18]
            test eax, eax
            je L0052e0e7
            mov edi, dword ptr [eax]
            test edi, edi
            je L0052e0e7
            mov edx, dword ptr [esp + 0x1c]
            test edx, edx
            je L0052e0e7
            mov ecx, dword ptr [edx]
            cmp ecx, 0x10
            jb L0052e0e7
            add ecx, -0x10
            mov dword ptr [edx], ecx
            mov ecx, 4
    L0052dbee:
            _emit 0xdb
        _emit 0x06
            mov edx, dword ptr [eax]
            add esi, 4
            fmul qword ptr ds:[0x58b870]
            _emit 0xd9
        _emit 0x1a
            mov ebx, dword ptr [eax]
            add ebx, 4
            dec ecx
            mov dword ptr [eax], ebx
            jne L0052dbee
            mov eax, edi
            jmp L0052e0e9
            mov edi, dword ptr [esp + 0x18]
            test edi, edi
            je L0052e0e7
            mov ebp, dword ptr [edi]
            test ebp, ebp
            je L0052e0e7
            mov ecx, dword ptr [esp + 0x1c]
            test ecx, ecx
            je L0052e0e7
            mov eax, dword ptr [ecx]
            cmp eax, 0x18
            jb L0052e0e7
            add eax, -0x18
            mov ebx, 6
            mov dword ptr [ecx], eax
    L0052dc45:
            _emit 0xd9
        _emit 0x06
            fmul dword ptr ds:[0x58b868]
            call _ftol
            mov edx, dword ptr [edi]
            add esi, 4
            mov dword ptr [edx], eax
            mov edx, dword ptr [edi]
            add edx, 4
            dec ebx
            mov dword ptr [edi], edx
            jne L0052dc45
            mov eax, ebp
            jmp L0052e0e9
            mov eax, dword ptr [esp + 0x18]
            test eax, eax
            je L0052e0e7
            mov edi, dword ptr [eax]
            test edi, edi
            je L0052e0e7
            mov edx, dword ptr [esp + 0x1c]
            test edx, edx
            je L0052e0e7
            mov ecx, dword ptr [edx]
            cmp ecx, 0x18
            jb L0052e0e7
            add ecx, -0x18
            mov dword ptr [edx], ecx
            mov ecx, 6
    L0052dca1:
            _emit 0xdb
        _emit 0x06
            mov edx, dword ptr [eax]
            add esi, 4
            fmul qword ptr ds:[0x58b870]
            _emit 0xd9
        _emit 0x1a
            mov ebx, dword ptr [eax]
            add ebx, 4
            dec ecx
            mov dword ptr [eax], ebx
            jne L0052dca1
            mov eax, edi
            jmp L0052e0e9
            mov edi, dword ptr [esp + 0x18]
            test edi, edi
            je L0052e0e7
            mov ebp, dword ptr [edi]
            test ebp, ebp
            je L0052e0e7
            mov ecx, dword ptr [esp + 0x1c]
            test ecx, ecx
            je L0052e0e7
            mov eax, dword ptr [ecx]
            cmp eax, 0x40
            jb L0052e0e7
            add eax, -0x40
            mov ebx, 0x10
            mov dword ptr [ecx], eax
    L0052dcf8:
            _emit 0xd9
        _emit 0x06
            fmul dword ptr ds:[0x58b868]
            call _ftol
            mov edx, dword ptr [edi]
            add esi, 4
            mov dword ptr [edx], eax
            mov edx, dword ptr [edi]
            add edx, 4
            dec ebx
            mov dword ptr [edi], edx
            jne L0052dcf8
            mov eax, ebp
            jmp L0052e0e9
            mov eax, dword ptr [esp + 0x18]
            test eax, eax
            je L0052e0e7
            mov edi, dword ptr [eax]
            test edi, edi
            je L0052e0e7
            mov edx, dword ptr [esp + 0x1c]
            test edx, edx
            je L0052e0e7
            mov ecx, dword ptr [edx]
            cmp ecx, 0x40
            jb L0052e0e7
            add ecx, -0x40
            mov dword ptr [edx], ecx
            mov ecx, 0x10
    L0052dd54:
            _emit 0xdb
        _emit 0x06
            mov edx, dword ptr [eax]
            add esi, 4
            fmul qword ptr ds:[0x58b870]
            _emit 0xd9
        _emit 0x1a
            mov ebx, dword ptr [eax]
            add ebx, 4
            dec ecx
            mov dword ptr [eax], ebx
            jne L0052dd54
            mov eax, edi
            jmp L0052e0e9
            mov edi, dword ptr [esp + 0x18]
            test edi, edi
            je L0052e0e7
            mov ebp, dword ptr [edi]
            test ebp, ebp
            je L0052e0e7
            mov ecx, dword ptr [esp + 0x1c]
            test ecx, ecx
            je L0052e0e7
            mov eax, dword ptr [ecx]
            cmp eax, 0x30
            jb L0052e0e7
            add eax, -0x30
            mov ebx, 0xc
            mov dword ptr [ecx], eax
    L0052ddab:
            _emit 0xd9
        _emit 0x06
            fmul dword ptr ds:[0x58b868]
            call _ftol
            mov edx, dword ptr [edi]
            add esi, 4
            mov dword ptr [edx], eax
            mov edx, dword ptr [edi]
            add edx, 4
            dec ebx
            mov dword ptr [edi], edx
            jne L0052ddab
            mov eax, ebp
            jmp L0052e0e9
            mov eax, dword ptr [esp + 0x18]
            test eax, eax
            je L0052e0e7
            mov edi, dword ptr [eax]
            test edi, edi
            je L0052e0e7
            mov edx, dword ptr [esp + 0x1c]
            test edx, edx
            je L0052e0e7
            mov ecx, dword ptr [edx]
            cmp ecx, 0x30
            jb L0052e0e7
            add ecx, -0x30
            mov dword ptr [edx], ecx
            mov ecx, 0xc
    L0052de07:
            _emit 0xdb
        _emit 0x06
            mov edx, dword ptr [eax]
            add esi, 4
            fmul qword ptr ds:[0x58b870]
            _emit 0xd9
        _emit 0x1a
            mov ebx, dword ptr [eax]
            add ebx, 4
            dec ecx
            mov dword ptr [eax], ebx
            jne L0052de07
            mov eax, edi
            jmp L0052e0e9
            mov eax, dword ptr [esp + 0x18]
            test eax, eax
            je L0052e0e7
            mov edi, dword ptr [eax]
            test edi, edi
            je L0052e0e7
            mov edx, dword ptr [esp + 0x1c]
            test edx, edx
            je L0052e0e7
            mov ecx, dword ptr [edx]
            cmp ecx, 8
            jb L0052e0e7
            add ecx, -8
            mov dword ptr [edx], ecx
            mov ecx, 2
    L0052de5e:
            mov edx, dword ptr [eax]
            mov ebx, dword ptr [esi]
            add esi, 4
            mov dword ptr [edx], ebx
            mov ebp, dword ptr [eax]
            add ebp, 4
            dec ecx
            mov dword ptr [eax], ebp
            jne L0052de5e
            mov eax, edi
            jmp L0052e0e9
            mov eax, dword ptr [esp + 0x18]
            test eax, eax
            je L0052e0e7
            mov edi, dword ptr [eax]
            test edi, edi
            je L0052e0e7
            mov edx, dword ptr [esp + 0x1c]
            test edx, edx
            je L0052e0e7
            mov ecx, dword ptr [edx]
            cmp ecx, 0xc
            jb L0052e0e7
            add ecx, -0xc
            mov dword ptr [edx], ecx
            mov ecx, 3
    L0052deaf:
            mov edx, dword ptr [eax]
            mov ebx, dword ptr [esi]
            add esi, 4
            mov dword ptr [edx], ebx
            mov ebp, dword ptr [eax]
            add ebp, 4
            dec ecx
            mov dword ptr [eax], ebp
            jne L0052deaf
            mov eax, edi
            jmp L0052e0e9
            mov eax, dword ptr [esp + 0x18]
            test eax, eax
            je L0052e0e7
            mov edi, dword ptr [eax]
            test edi, edi
            je L0052e0e7
            mov edx, dword ptr [esp + 0x1c]
            test edx, edx
            je L0052e0e7
            mov ecx, dword ptr [edx]
            cmp ecx, 0x10
            jb L0052e0e7
            add ecx, -0x10
            mov dword ptr [edx], ecx
            mov ecx, 4
    L0052df00:
            mov edx, dword ptr [eax]
            mov ebx, dword ptr [esi]
            add esi, 4
            mov dword ptr [edx], ebx
            mov ebp, dword ptr [eax]
            add ebp, 4
            dec ecx
            mov dword ptr [eax], ebp
            jne L0052df00
            mov eax, edi
            jmp L0052e0e9
            mov eax, dword ptr [esp + 0x18]
            test eax, eax
            je L0052e0e7
            mov edi, dword ptr [eax]
            test edi, edi
            je L0052e0e7
            mov edx, dword ptr [esp + 0x1c]
            test edx, edx
            je L0052e0e7
            mov ecx, dword ptr [edx]
            cmp ecx, 0x18
            jb L0052e0e7
            add ecx, -0x18
            mov dword ptr [edx], ecx
            mov ecx, 6
    L0052df51:
            mov edx, dword ptr [eax]
            mov ebx, dword ptr [esi]
            add esi, 4
            mov dword ptr [edx], ebx
            mov ebp, dword ptr [eax]
            add ebp, 4
            dec ecx
            mov dword ptr [eax], ebp
            jne L0052df51
            mov eax, edi
            jmp L0052e0e9
            mov eax, dword ptr [esp + 0x18]
            test eax, eax
            je L0052e0e7
            mov edi, dword ptr [eax]
            test edi, edi
            je L0052e0e7
            mov edx, dword ptr [esp + 0x1c]
            test edx, edx
            je L0052e0e7
            mov ecx, dword ptr [edx]
            cmp ecx, 0x40
            jb L0052e0e7
            add ecx, -0x40
            mov dword ptr [edx], ecx
            mov ecx, 0x10
    L0052dfa2:
            mov edx, dword ptr [eax]
            mov ebx, dword ptr [esi]
            add esi, 4
            mov dword ptr [edx], ebx
            mov ebp, dword ptr [eax]
            add ebp, 4
            dec ecx
            mov dword ptr [eax], ebp
            jne L0052dfa2
            mov eax, edi
            jmp L0052e0e9
            mov eax, dword ptr [esp + 0x18]
            test eax, eax
            je L0052e0e7
            mov edi, dword ptr [eax]
            test edi, edi
            je L0052e0e7
            mov edx, dword ptr [esp + 0x1c]
            test edx, edx
            je L0052e0e7
            mov ecx, dword ptr [edx]
            cmp ecx, 0x30
            jb L0052e0e7
            add ecx, -0x30
            mov dword ptr [edx], ecx
            mov ecx, 0xc
    L0052dff3:
            mov edx, dword ptr [eax]
            mov ebx, dword ptr [esi]
            add esi, 4
            mov dword ptr [edx], ebx
            mov ebp, dword ptr [eax]
            add ebp, 4
            dec ecx
            mov dword ptr [eax], ebp
            jne L0052dff3
            mov eax, edi
            jmp L0052e0e9
            mov edx, dword ptr [esi]
            push edx
            call BrStrLen
            add esp, 4
            mov esi, dword ptr [esi]
            inc eax
            lea ecx, [eax + 3]
            mov eax, dword ptr [esp + 0x18]
            shr ecx, 2
            test eax, eax
            je L0052e0e7
            mov ebp, dword ptr [eax]
            test ebp, ebp
            je L0052e0e7
            mov ebx, dword ptr [esp + 0x1c]
            test ebx, ebx
            je L0052e0e7
            mov edx, dword ptr [ebx]
            lea edi, [ecx*4]
            cmp edi, edx
            ja L0052e0e7
            sub edx, edi
            mov dword ptr [ebx], edx
            mov edx, ecx
            dec ecx
            test edx, edx
            je L0052e0e3
            inc ecx
    L0052e064:
            mov edx, dword ptr [eax]
            mov edi, dword ptr [esi]
            add esi, 4
            mov dword ptr [edx], edi
            mov ebx, dword ptr [eax]
            add ebx, 4
            dec ecx
            mov dword ptr [eax], ebx
            jne L0052e064
            mov eax, ebp
            jmp L0052e0e9
            mov ecx, dword ptr [esi]
            xor edx, edx
            mov eax, ecx
            test eax, eax
            mov dword ptr [esp + 0x24], eax
            je L0052e100
            mov esi, dword ptr [eax]
            add eax, 4
            test esi, esi
            je L0052e09c
    L0052e092:
            mov esi, dword ptr [eax]
            inc edx
            add eax, 4
            test esi, esi
            jne L0052e092
    L0052e09c:
            mov eax, dword ptr [esp + 0x18]
            lea edi, [edx + 1]
            test eax, eax
            je L0052e0e7
            mov ebp, dword ptr [eax]
            test ebp, ebp
            je L0052e0e7
            mov ebx, dword ptr [esp + 0x1c]
            test ebx, ebx
            je L0052e0e7
            mov edx, dword ptr [ebx]
            lea esi, [edi*4]
            cmp esi, edx
            ja L0052e0e7
            sub edx, esi
            mov dword ptr [ebx], edx
            mov edx, edi
            dec edi
            test edx, edx
            je L0052e0e3
            lea edx, [edi + 1]
    L0052e0d0:
            mov esi, dword ptr [eax]
            mov edi, dword ptr [ecx]
            add ecx, 4
            mov dword ptr [esi], edi
            mov ebx, dword ptr [eax]
            add ebx, 4
            dec edx
            mov dword ptr [eax], ebx
            jne L0052e0d0
    L0052e0e3:
            mov eax, ebp
            jmp L0052e0e9
    L0052e0e7:
            xor eax, eax
    L0052e0e9:
            mov ecx, dword ptr [esp + 0x14]
            test eax, eax
            mov dword ptr [ecx + 4], eax
            jne L0052e16d
            mov eax, 0x1004
            pop edi
            pop esi
            pop ebp
            pop ebx
            ret 0x14
    L0052e100:
            mov eax, dword ptr [esp + 0x18]
            lea edx, [esp + 0x24]
            test eax, eax
            je L0052e143
            mov edi, dword ptr [eax]
            test edi, edi
            je L0052e143
            mov esi, dword ptr [esp + 0x1c]
            test esi, esi
            je L0052e143
            mov ecx, dword ptr [esi]
            cmp ecx, 4
            jb L0052e143
            add ecx, -4
            mov dword ptr [esi], ecx
            xor ecx, ecx
    L0052e128:
            mov esi, dword ptr [eax]
            mov ebx, dword ptr [edx]
            add edx, 4
            mov dword ptr [esi], ebx
            mov ebx, dword ptr [eax]
            add ebx, 4
            mov esi, ecx
            dec ecx
            mov dword ptr [eax], ebx
            test esi, esi
            jne L0052e128
            mov eax, edi
            jmp L0052e145
    L0052e143:
            xor eax, eax
    L0052e145:
            mov edx, dword ptr [esp + 0x14]
            test eax, eax
            mov dword ptr [edx + 4], eax
            jne L0052e16d
            mov eax, 0x1004
            pop edi
            pop esi
            pop ebp
            pop ebx
            ret 0x14
            mov ecx, dword ptr [esi]
            xor eax, eax
            cmp ecx, 1
            mov ecx, dword ptr [esp + 0x14]
            setne al
            mov dword ptr [ecx + 4], eax
    L0052e16d:
            xor eax, eax
            pop edi
            pop esi
            pop ebp
            pop ebx
            ret 0x14
            mov ecx, dword ptr [eax + 0x10]
            test ecx, ecx
            je L0052e1a3
            mov ecx, dword ptr [ecx]
            test ecx, ecx
            je L0052e1a3
            push eax
            mov eax, dword ptr [esp + 0x1c]
            push edx
            mov edx, dword ptr [esp + 0x24]
            push edx
            mov edx, dword ptr [esp + 0x20]
            add edx, 4
            push eax
            push edx
            call ecx
            add esp, 0x14
            pop edi
            pop esi
            pop ebp
            pop ebx
            ret 0x14
    L0052e1a3:
            pop edi
            pop esi
            pop ebp
            mov eax, 0x1001
            pop ebx
            ret 0x14
            nop
            _emit 0x25
            _emit 0xd9
            _emit 0x52
            _emit 0x00
            _emit 0x76
            _emit 0xe1
            _emit 0x52
            _emit 0x00
            _emit 0x13
            _emit 0xd9
            _emit 0x52
            _emit 0x00
            _emit 0x27
            _emit 0xde
            _emit 0x52
            _emit 0x00
            _emit 0x78
            _emit 0xde
            _emit 0x52
            _emit 0x00
            _emit 0xc9
            _emit 0xde
            _emit 0x52
            _emit 0x00
            _emit 0x1a
            _emit 0xdf
            _emit 0x52
            _emit 0x00
            _emit 0xbc
            _emit 0xdf
            _emit 0x52
            _emit 0x00
            _emit 0x6b
            _emit 0xdf
            _emit 0x52
            _emit 0x00
            _emit 0x38
            _emit 0xd9
            _emit 0x52
            _emit 0x00
            _emit 0x4b
            _emit 0xd9
            _emit 0x52
            _emit 0x00
            _emit 0x5e
            _emit 0xd9
            _emit 0x52
            _emit 0x00
            _emit 0x72
            _emit 0xd9
            _emit 0x52
            _emit 0x00
            _emit 0xc0
            _emit 0xd9
            _emit 0x52
            _emit 0x00
            _emit 0xdd
            _emit 0xd9
            _emit 0x52
            _emit 0x00
            _emit 0xf5
            _emit 0xd9
            _emit 0x52
            _emit 0x00
            _emit 0x51
            _emit 0xda
            _emit 0x52
            _emit 0x00
            _emit 0xa8
            _emit 0xda
            _emit 0x52
            _emit 0x00
            _emit 0x04
            _emit 0xdb
            _emit 0x52
            _emit 0x00
            _emit 0x5b
            _emit 0xdb
            _emit 0x52
            _emit 0x00
            _emit 0xb7
            _emit 0xdb
            _emit 0x52
            _emit 0x00
            _emit 0x0e
            _emit 0xdc
            _emit 0x52
            _emit 0x00
            _emit 0x6a
            _emit 0xdc
            _emit 0x52
            _emit 0x00
            _emit 0x74
            _emit 0xdd
            _emit 0x52
            _emit 0x00
            _emit 0xd0
            _emit 0xdd
            _emit 0x52
            _emit 0x00
            _emit 0xc1
            _emit 0xdc
            _emit 0x52
            _emit 0x00
            _emit 0x1d
            _emit 0xdd
            _emit 0x52
            _emit 0x00
            _emit 0x0d
            _emit 0xe0
            _emit 0x52
            _emit 0x00
            _emit 0x7b
            _emit 0xe0
            _emit 0x52
            _emit 0x00
            _emit 0x5c
            _emit 0xe1
            _emit 0x52
            _emit 0x00
            _emit 0x87
            _emit 0xd9
            _emit 0x52
            _emit 0x00
            _emit 0xa4
            _emit 0xd9
            _emit 0x52
            _emit 0x00
    }
#endif
}

// FUNCTION: CARMA2_HW 0x0052e630
br_error C2_NAKED C2_HOOK_STDCALL ValueSet(void* block, br_token_value* tv, br_tv_template_entry* tep) {
#ifndef CARPOCALYPSE2_MATCHING
    void* mem;
    int i;
    int t;
    br_tv_custom* custp;

    if (tep->flags & 0x8) {
        return 0x1002;
    }
    mem = (br_uint_8*)block + tep->offset;

    switch (tep->conv) {
#ifndef CARPOCALYPSE2_MATCHING
    case 33:
        /* full-width pointer (64-bit hosts) */
        *(void**)mem = *(void**)&tv->v;
        break;
#endif
    case 3:
        /* void* */
        *(br_uint_32*)mem = tv->v.u32;
        break;
    case 10:
        *(br_int_8*)mem = tv->v.i8;
        break;
    case 11:
        *(br_int_16*)mem = tv->v.i16;
        break;
    case 12:
        *(br_uint_8*)mem = tv->v.u8;
        break;
    case 13:
        *(br_uint_16*)mem = tv->v.u16;
        break;
    case 31:
        if (tv->v.b) {
            *(br_uint_32*) mem |= tep->conv_arg;
        } else {
            *(br_uint_32*) mem &= ~tep->conv_arg;
        }
        break;
    case 32:
        if (!tv->v.b) {
            *(br_uint_32*) mem |= tep->conv_arg;
        } else {
            *(br_uint_32*) mem &= ~tep->conv_arg;
        }
        break;
    case 14:
        *(float*)mem = (float)((double)tv->v.x / 65536.0);
        break;
    case 15:
        *(br_fixed_ls*)mem = (br_fixed_ls)(tv->v.f * 65536.0f);
        break;
    case 16:
        t = 2;
        goto store_fixed_as_floats;
    case 17:
        t = 2;
        goto store_floats_as_fixed;
    case 18:
        t = 3;
        goto store_fixed_as_floats;
    case 19:
        t = 3;
        goto store_floats_as_fixed;
    case 20:
        t = 4;
        goto store_fixed_as_floats;
    case 21:
        t = 4;
        goto store_floats_as_fixed;
    case 22:
        t = 6;
        goto store_fixed_as_floats;
    case 23:
        t = 6;
        goto store_floats_as_fixed;
    case 26:
        t = 16;
store_fixed_as_floats:
        for (i = 0; i < t; i++) {
            ((float*)mem)[i] = (float)((br_fixed_ls*)tv->v.p)[i] / 65536.0f;
        }
        break;
    case 27:
        t = 16;
store_floats_as_fixed:
        for (i = 0; i < t; i++) {
            ((br_fixed_ls*)mem)[i] = (br_fixed_ls)(((float*)tv->v.p)[i] * 65536.0f);
        }
        break;
    case 24:
        t = 12;
        goto store_fixed_as_floats;
    case 25:
        t = 12;
        goto store_floats_as_fixed;
    case 4:
        t = 2;
        goto copy_integers;
    case 5:
        t = 3;
        goto copy_integers;
    case 6:
        t = 4;
        goto copy_integers;
    case 7:
        t = 6;
        goto copy_integers;
    case 9:
        t = 16;
copy_integers:
        for (i = 0; i < t; i++) {
            ((br_uint_32*)mem)[i] = ((br_uint_32*)tv->v.p)[i];
        }
        break;
    case 8:
        t = 12;
        goto copy_integers;
    case 28:
        BrStrNCpy(mem, tv->v.cstr, 64);
        return 0;
    case 30:
        if (tv->v.b) {
            *(br_uint_32*) mem = tep->conv_arg;
        } else {
            *(br_uint_32*) mem = 1;
        }
        break;
    case 2:
        custp = (br_tv_custom*)tep->conv_arg;
        if (custp == NULL ||custp->set == NULL) {
            return 0x1001;
        }
        return custp->set(block, &tv->v.u32, tep);
    default:
        return 0x1001;
    }
    return 0;
#else
    __asm {
            mov eax, dword ptr [esp + 0xc]
            push ebx
            push ebp
            push esi
            mov cl, byte ptr [eax + 0xc]
            push edi
            test cl, 8
            je L0052e64c
            mov eax, 0x1002
            pop edi
            pop esi
            pop ebp
            pop ebx
            ret 0xc
    L0052e64c:
            movsx ecx, word ptr [eax + 0xe]
            mov esi, dword ptr [eax + 8]
            mov edx, dword ptr [esp + 0x14]
            add ecx, -2
            add esi, edx
            cmp ecx, 0x1e
            ja L0052e8b4
            jmp dword ptr [ecx*4 + 0x52e8c0]
            mov eax, dword ptr [esp + 0x18]
            mov ecx, dword ptr [eax + 4]
            xor eax, eax
            mov dword ptr [esi], ecx
            pop edi
            pop esi
            pop ebp
            pop ebx
            ret 0xc
            mov edx, dword ptr [esp + 0x18]
            mov al, byte ptr [edx + 4]
            mov byte ptr [esi], al
            xor eax, eax
            pop edi
            pop esi
            pop ebp
            pop ebx
            ret 0xc
            mov ecx, dword ptr [esp + 0x18]
            xor eax, eax
            mov dx, word ptr [ecx + 4]
            mov word ptr [esi], dx
            pop edi
            pop esi
            pop ebp
            pop ebx
            ret 0xc
            mov eax, dword ptr [esp + 0x18]
            mov cl, byte ptr [eax + 4]
            xor eax, eax
            mov byte ptr [esi], cl
            pop edi
            pop esi
            pop ebp
            pop ebx
            ret 0xc
            mov edx, dword ptr [esp + 0x18]
            mov ax, word ptr [edx + 4]
            mov word ptr [esi], ax
            xor eax, eax
            pop edi
            pop esi
            pop ebp
            pop ebx
            ret 0xc
            mov ecx, dword ptr [esp + 0x18]
            mov edx, dword ptr [ecx + 4]
            test edx, edx
            je L0052e704
            mov edx, dword ptr [eax + 0x10]
            mov eax, dword ptr [esi]
            or eax, edx
            mov dword ptr [esi], eax
            xor eax, eax
            pop edi
            pop esi
            pop ebp
            pop ebx
            ret 0xc
            mov ecx, dword ptr [esp + 0x18]
            mov edx, dword ptr [ecx + 4]
            test edx, edx
            jne L0052e704
            mov edx, dword ptr [eax + 0x10]
            mov eax, dword ptr [esi]
            or eax, edx
            mov dword ptr [esi], eax
            xor eax, eax
            pop edi
            pop esi
            pop ebp
            pop ebx
            ret 0xc
    L0052e704:
            mov eax, dword ptr [eax + 0x10]
            mov ecx, dword ptr [esi]
            not eax
            and ecx, eax
            xor eax, eax
            mov dword ptr [esi], ecx
            pop edi
            pop esi
            pop ebp
            pop ebx
            ret 0xc
            mov ecx, dword ptr [esp + 0x18]
            xor eax, eax
            fild dword ptr [ecx + 4]
            fmul qword ptr ds:[0x58b870]
            _emit 0xd9
        _emit 0x1e
            pop edi
            pop esi
            pop ebp
            pop ebx
            ret 0xc
            mov edx, dword ptr [esp + 0x18]
            fld dword ptr [edx + 4]
            fmul dword ptr ds:[0x58b868]
            call _ftol
            mov dword ptr [esi], eax
            xor eax, eax
            pop edi
            pop esi
            pop ebp
            pop ebx
            ret 0xc
            mov edx, 2
            jmp L0052e798
            mov ebx, 2
            jmp L0052e7cf
            mov edx, 3
            jmp L0052e798
            mov ebx, 3
            jmp L0052e7cf
            mov edx, 4
            jmp L0052e798
            mov ebx, 4
            jmp L0052e7cf
            mov edx, 6
            jmp L0052e798
            mov ebx, 6
            jmp L0052e7cf
            mov edx, 0x10
            jmp L0052e798
            mov ebx, 0x10
            jmp L0052e7cf
            mov edx, 0xc
    L0052e798:
            xor eax, eax
            test edx, edx
            jle L0052e887
            mov edi, dword ptr [esp + 0x18]
    L0052e7a6:
            mov ebx, dword ptr [edi + 4]
            lea ecx, [eax*4]
            inc eax
            fild dword ptr [ebx + ecx]
            cmp eax, edx
            fmul qword ptr ds:[0x58b870]
            fstp dword ptr [esi + ecx]
            jl L0052e7a6
            xor eax, eax
            pop edi
            pop esi
            pop ebp
            pop ebx
            ret 0xc
            mov ebx, 0xc
    L0052e7cf:
            xor edi, edi
            test ebx, ebx
            jle L0052e887
            mov ebp, dword ptr [esp + 0x18]
    L0052e7dd:
            mov eax, dword ptr [ebp + 4]
            fld dword ptr [eax + edi*4]
            fmul dword ptr ds:[0x58b868]
            call _ftol
            mov dword ptr [esi + edi*4], eax
            inc edi
            cmp edi, ebx
            jl L0052e7dd
            xor eax, eax
            pop edi
            pop esi
            pop ebp
            pop ebx
            ret 0xc
            mov ecx, 2
            jmp L0052e827
            mov ecx, 3
            jmp L0052e827
            mov ecx, 4
            jmp L0052e827
            mov ecx, 6
            jmp L0052e827
            mov ecx, 0x10
            jmp L0052e827
            mov ecx, 0xc
    L0052e827:
            xor eax, eax
            test ecx, ecx
            jle L0052e887
            mov edx, dword ptr [esp + 0x18]
    L0052e831:
            mov edi, dword ptr [edx + 4]
            inc eax
            cmp eax, ecx
            mov edi, dword ptr [edi + eax*4 - 4]
            mov dword ptr [esi + eax*4 - 4], edi
            jl L0052e831
            xor eax, eax
            pop edi
            pop esi
            pop ebp
            pop ebx
            ret 0xc
            mov ecx, dword ptr [esp + 0x18]
            mov eax, dword ptr [esi]
            push 0x40
            mov edx, dword ptr [ecx + 4]
            push edx
            push eax
            call BrStrNCpy
            add esp, 0xc
            xor eax, eax
            pop edi
            pop esi
            pop ebp
            pop ebx
            ret 0xc
            mov ecx, dword ptr [esp + 0x18]
            mov edx, dword ptr [ecx + 4]
            test edx, edx
            je L0052e881
            mov edx, dword ptr [eax + 0x10]
            xor eax, eax
            mov dword ptr [esi], edx
            pop edi
            pop esi
            pop ebp
            pop ebx
            ret 0xc
    L0052e881:
            mov dword ptr [esi], 1
    L0052e887:
            xor eax, eax
            pop edi
            pop esi
            pop ebp
            pop ebx
            ret 0xc
            mov ecx, dword ptr [eax + 0x10]
            test ecx, ecx
            je L0052e8b4
            mov ecx, dword ptr [ecx + 4]
            test ecx, ecx
            je L0052e8b4
            push eax
            mov eax, dword ptr [esp + 0x1c]
            add eax, 4
            push eax
            push edx
            call ecx
            add esp, 0xc
            pop edi
            pop esi
            pop ebp
            pop ebx
            ret 0xc
    L0052e8b4:
            pop edi
            pop esi
            pop ebp
            mov eax, 0x1001
            pop ebx
            ret 0xc
            _emit 0x90
            _emit 0xe8
            _emit 0x52
            _emit 0x00
            _emit 0x6c
            _emit 0xe6
            _emit 0x52
            _emit 0x00
            _emit 0xff
            _emit 0xe7
            _emit 0x52
            _emit 0x00
            _emit 0x06
            _emit 0xe8
            _emit 0x52
            _emit 0x00
            _emit 0x0d
            _emit 0xe8
            _emit 0x52
            _emit 0x00
            _emit 0x14
            _emit 0xe8
            _emit 0x52
            _emit 0x00
            _emit 0x22
            _emit 0xe8
            _emit 0x52
            _emit 0x00
            _emit 0x1b
            _emit 0xe8
            _emit 0x52
            _emit 0x00
            _emit 0x7e
            _emit 0xe6
            _emit 0x52
            _emit 0x00
            _emit 0x90
            _emit 0xe6
            _emit 0x52
            _emit 0x00
            _emit 0xa4
            _emit 0xe6
            _emit 0x52
            _emit 0x00
            _emit 0xb6
            _emit 0xe6
            _emit 0x52
            _emit 0x00
            _emit 0x18
            _emit 0xe7
            _emit 0x52
            _emit 0x00
            _emit 0x30
            _emit 0xe7
            _emit 0x52
            _emit 0x00
            _emit 0x4d
            _emit 0xe7
            _emit 0x52
            _emit 0x00
            _emit 0x54
            _emit 0xe7
            _emit 0x52
            _emit 0x00
            _emit 0x5b
            _emit 0xe7
            _emit 0x52
            _emit 0x00
            _emit 0x62
            _emit 0xe7
            _emit 0x52
            _emit 0x00
            _emit 0x69
            _emit 0xe7
            _emit 0x52
            _emit 0x00
            _emit 0x70
            _emit 0xe7
            _emit 0x52
            _emit 0x00
            _emit 0x77
            _emit 0xe7
            _emit 0x52
            _emit 0x00
            _emit 0x7e
            _emit 0xe7
            _emit 0x52
            _emit 0x00
            _emit 0x93
            _emit 0xe7
            _emit 0x52
            _emit 0x00
            _emit 0xca
            _emit 0xe7
            _emit 0x52
            _emit 0x00
            _emit 0x85
            _emit 0xe7
            _emit 0x52
            _emit 0x00
            _emit 0x8c
            _emit 0xe7
            _emit 0x52
            _emit 0x00
            _emit 0x4a
            _emit 0xe8
            _emit 0x52
            _emit 0x00
            _emit 0xb4
            _emit 0xe8
            _emit 0x52
            _emit 0x00
            _emit 0x68
            _emit 0xe8
            _emit 0x52
            _emit 0x00
            _emit 0xca
            _emit 0xe6
            _emit 0x52
            _emit 0x00
            _emit 0xe7
            _emit 0xe6
            _emit 0x52
            _emit 0x00
            _emit 0x90
            _emit 0x90
            _emit 0x90
            _emit 0x90
    }
#endif
}

// FUNCTION: CARMA2_HW 0x0052e300
br_size_t C2_NAKED C2_HOOK_STDCALL ValueExtraSize(void* block, br_tv_template_entry* tep) {
#ifndef CARPOCALYPSE2_MATCHING
    void* mem;
    br_uint_32* lp;
    int t;
    br_tv_custom* custp;

    if (tep->flags & 0x8) {
        mem = (void*)&tep->offset;
    } else {
        mem = (br_uint_8*)block + tep->offset;
    }
    switch (tep->conv) {
    case 2:
        custp = (br_tv_custom*)tep->conv_arg;
        if (custp == NULL || custp->extra_size == NULL) {
            return 0;
        }
        return custp->extra_size(block, tep);
    case 4:
        return 2 * sizeof(int);
    case 5:
        return 3 * sizeof(int);
    case 6:
        return 4 * sizeof(int);
    case 7:
        return 6 * sizeof(int);
    case 8:
        return 12 * sizeof(int);
    case 9:
        return 16 * sizeof(int);
    case 16:
        return 2 * sizeof(float);
    case 17:
        return 2 * sizeof(br_fixed_ls);
    case 18:
        return 3 * sizeof(float);
    case 19:
        return 3 * sizeof(br_fixed_ls);
    case 20:
        return 4 * sizeof(float);
    case 21:
        return 4 * sizeof(br_fixed_ls);
    case 22:
        return 6 * sizeof(float);
    case 23:
        return 6 * sizeof(br_fixed_ls);
    case 24:
        return 12 * sizeof(float);
    case 25:
        return 12 * sizeof(br_fixed_ls);
    case 26:
        return 16 * sizeof(float);
    case 27:
        return 16 * sizeof(br_fixed_ls);
    case 28:
        return (BrStrLen(*(char**)mem) + 1 + sizeof(int) - 1) / sizeof(int);
    case 29:
        t = 1;
        lp = *(br_uint_32**)mem;
        if (lp != NULL) {
            while (*lp++ != 0) {
                t++;
            }
        }
        return t * sizeof(int);
    default:
        return 0;
    }
#else
    __asm {
            mov eax, dword ptr [esp + 8]
            push esi
            push edi
            mov edi, dword ptr [esp + 0xc]
            test byte ptr [eax + 0xc], 8
            mov ecx, dword ptr [eax + 8]
            jne L0052e315
            add ecx, edi
    L0052e315:
            movsx edx, word ptr [eax + 0xe]
            lea esi, [edx - 2]
            cmp esi, 0x1b
            ja L0052e3cd
            xor edx, edx
            mov dl, byte ptr [esi + 0x52e414]
            jmp dword ptr [edx*4 + 0x52e3d4]
            mov eax, 8
            pop edi
            pop esi
            ret 8
            mov eax, 0xc
            pop edi
            pop esi
            ret 8
            mov eax, 0x10
            pop edi
            pop esi
            ret 8
            mov eax, 0x18
            pop edi
            pop esi
            ret 8
            mov eax, 0x40
            pop edi
            pop esi
            ret 8
            mov eax, 0x30
            pop edi
            pop esi
            ret 8
            mov eax, dword ptr [ecx]
            push eax
            call BrStrLen
            add eax, 4
            add esp, 4
            shr eax, 2
            shl eax, 2
            pop edi
            pop esi
            ret 8
            mov eax, dword ptr [ecx]
            mov ecx, 1
            test eax, eax
            je L0052e3a7
            mov edx, dword ptr [eax]
            add eax, 4
            test edx, edx
            je L0052e3a7
    L0052e39d:
            mov edx, dword ptr [eax]
            inc ecx
            add eax, 4
            test edx, edx
            jne L0052e39d
    L0052e3a7:
            lea eax, [ecx*4]
            pop edi
            pop esi
            ret 8
            mov ecx, dword ptr [eax + 0x10]
            test ecx, ecx
            je L0052e3cd
            mov ecx, dword ptr [ecx + 8]
            test ecx, ecx
            je L0052e3cd
            push eax
            push edi
            call ecx
            add esp, 8
            pop edi
            pop esi
            ret 8
    L0052e3cd:
            pop edi
            xor eax, eax
            pop esi
            ret 8
            _emit 0xb3
            _emit 0xe3
            _emit 0x52
            _emit 0x00
            _emit 0x34
            _emit 0xe3
            _emit 0x52
            _emit 0x00
            _emit 0x3e
            _emit 0xe3
            _emit 0x52
            _emit 0x00
            _emit 0x48
            _emit 0xe3
            _emit 0x52
            _emit 0x00
            _emit 0x52
            _emit 0xe3
            _emit 0x52
            _emit 0x00
            _emit 0x66
            _emit 0xe3
            _emit 0x52
            _emit 0x00
            _emit 0x5c
            _emit 0xe3
            _emit 0x52
            _emit 0x00
            _emit 0x34
            _emit 0xe3
            _emit 0x52
            _emit 0x00
            _emit 0x3e
            _emit 0xe3
            _emit 0x52
            _emit 0x00
            _emit 0x48
            _emit 0xe3
            _emit 0x52
            _emit 0x00
            _emit 0x52
            _emit 0xe3
            _emit 0x52
            _emit 0x00
            _emit 0x66
            _emit 0xe3
            _emit 0x52
            _emit 0x00
            _emit 0x5c
            _emit 0xe3
            _emit 0x52
            _emit 0x00
            _emit 0x70
            _emit 0xe3
            _emit 0x52
            _emit 0x00
            _emit 0x89
            _emit 0xe3
            _emit 0x52
            _emit 0x00
            _emit 0xcd
            _emit 0xe3
            _emit 0x52
            _emit 0x00
            _emit 0x00
            _emit 0x0f
            _emit 0x01
            _emit 0x02
            _emit 0x03
            _emit 0x04
            _emit 0x05
            _emit 0x06
            _emit 0x0f
            _emit 0x0f
            _emit 0x0f
            _emit 0x0f
            _emit 0x0f
            _emit 0x0f
            _emit 0x07
            _emit 0x07
            _emit 0x08
            _emit 0x08
            _emit 0x09
            _emit 0x09
            _emit 0x0a
            _emit 0x0a
            _emit 0x0b
            _emit 0x0b
            _emit 0x0c
            _emit 0x0c
            _emit 0x0d
            _emit 0x0e
    }
#endif
}

// FUNCTION: CARMA2_HW 0x0052d780
br_error C2_HOOK_CDECL BrTokenValueQuery(br_uint_32* pvalue, br_uint_32* extra, br_size_t extra_size, br_token t, void* block, br_tv_template* template) {
    br_int_32 o;
    br_tv_template_entry* tep;
    br_token_value tv;
    br_error r;

    if (template->n_map_entries == 0) {
        templateMakeMap(template);
    }
    o = t - template->map_base;
    if (o < 0 || o >= template->n_map_entries) {
        return 0x1001;
    }
    tep = template->map_query_entry[o];
    if (tep != NULL) {
        r = ValueQuery(&tv, (void**)&extra, &extra_size, block, tep);
#ifndef CARPOCALYPSE2_MATCHING
        /*
         * Port-only: this single-value query is used to read object pointers
         * (BRT_OUTPUT_FACILITY_O, BRT_RENDERER_FACILITY_O, ...). Writing only
         * tv.v.u32 truncates them on 64-bit hosts; the caller always passes a
         * pointer-sized slot for pointer-valued tokens.
         */
        if (tep->conv == 33 /* BRTV_CONV_POINTER */) {
            *(void**)pvalue = *(void**)&tv.v;
            return r;
        }
#endif
        *pvalue = tv.v.u32;
        return r;
    }
    return 0x1001;
}

// FUNCTION: CARMA2_HW 0x0052e230
br_error C2_HOOK_CDECL BrTokenValueQueryMany(br_token_value* tv, void* extra, br_size_t extra_size, br_int_32* pcount, void* block, br_tv_template* template) {
    br_int_32 o;
    br_tv_template_entry* tep;
    br_error r;

    if (template->n_map_entries == 0) {
        templateMakeMap(template);
    }
    *pcount = 0;

    for (; tv->t != BR_NULL_TOKEN; tv++) {
        o = tv->t - template->map_base;
        if (o < 0 || o >= template->n_map_entries) {
            continue;
        }
        tep = template->map_query_entry[o];
        if (tep == NULL) {
            continue;
        }
        r = ValueQuery(tv, &extra, &extra_size, block, tep);
        if (r == 0) {
            (*pcount)++;
        }
    }
    return 0;
}

// FUNCTION: CARMA2_HW 0x0052e2a0
br_error C2_HOOK_CDECL BrTokenValueQueryManySize(br_size_t* psize, br_token_value* tv, void* block, br_tv_template* template) {
    br_int_32 o;
    br_size_t extra_size;

    extra_size = 0;

    if (template->n_map_entries == 0) {
        templateMakeMap(template);
    }

    for (; tv->t != 0; tv++) {
        o = tv->t - template->map_base;
        if (o < 0 || o >= template->n_map_entries) {
            continue;
        }
        if (template->map_query_entry[o]) {
            extra_size += ValueExtraSize(block, template->map_query_entry[o]);
        }
    }
    *psize = extra_size;
    return 0;
}

// FUNCTION: CARMA2_HW 0x0052e430
br_error C2_NAKED C2_HOOK_CDECL BrTokenValueQueryAll(br_token_value* buffer, br_size_t buffer_size, void* block, br_tv_template* template) {
#ifndef CARPOCALYPSE2_MATCHING
    br_tv_template_entry* tp;
    int n;
    int e;
    br_token_value* tv;
    char* extra;
    br_size_t extra_size;
    br_error r;

    if (!template->names_resolved) {
        templateResolveNames(template);
    }

    n = 1;
    for (e = 0; e < template->n_entries; e++) {
        tp = &template->entries[e];
        if (tp->token != BR_NULL_TOKEN && (tp->flags & 0x4)) {
            n++;
        }
    }
    extra = (char *)(buffer + n);
    if (buffer_size < sizeof(*buffer) * n) {
        return 0x1004;
    }
    tv = buffer;
    extra_size = buffer_size - sizeof(*buffer) * n;
    for (e = 0; e < template->n_entries; e++) {
        tp = &template->entries[e];
        if (tp->token == BR_NULL_TOKEN) {
            continue;
        }
        if ((tp->flags & 4) == 0) {
            continue;
        }
        tv->t = tp->token;
        r = ValueQuery(tv, (void*)&extra, &extra_size, block, tp);
        if (r != 0) {
            return r;
        }
        tv++;
    }
    tv->t = BR_NULL_TOKEN;
    return 0;
#else
    __asm {
            push ecx
            push ebx
            mov ebx, dword ptr [esp + 0x18]
            push ebp
            mov ebp, dword ptr [esp + 0x10]
            mov eax, dword ptr [ebx + 0x18]
            push esi
            test eax, eax
            push edi
            mov dword ptr [esp + 0x18], ebp
            jne L0052e471
            mov eax, dword ptr [ebx]
            mov esi, dword ptr [ebx + 4]
            xor edi, edi
            test eax, eax
            jle L0052e471
    L0052e453:
            mov eax, dword ptr [esi + 4]
            test eax, eax
            je L0052e467
            push 0
            push eax
            call BrTokenCreate
            add esp, 8
            mov dword ptr [esi], eax
    L0052e467:
            mov eax, dword ptr [ebx]
            add esi, 0x18
            inc edi
            cmp edi, eax
            jl L0052e453
    L0052e471:
            mov edx, dword ptr [ebx]
            mov esi, dword ptr [ebx + 4]
            test edx, edx
            mov edi, 1
            mov eax, esi
            jle L0052e495
            mov ecx, edx
    L0052e483:
            cmp dword ptr [eax], 0
            je L0052e48f
            test byte ptr [eax + 0xc], 4
            je L0052e48f
            inc edi
    L0052e48f:
            add eax, 0x18
            dec ecx
            jne L0052e483
    L0052e495:
            mov eax, dword ptr [esp + 0x1c]
            lea ecx, [edi*8]
            add ebp, ecx
            cmp eax, ecx
            mov dword ptr [esp + 0x10], ebp
            jae L0052e4b5
            mov eax, 0x1004
            pop edi
            pop esi
            pop ebp
            pop ebx
            pop ecx
            ret
    L0052e4b5:
            sub eax, ecx
            xor edi, edi
            test edx, edx
            mov dword ptr [esp + 0x24], eax
            jle L0052e4fc
            mov ebp, dword ptr [esp + 0x20]
    L0052e4c5:
            mov eax, dword ptr [esi]
            test eax, eax
            je L0052e4f2
            test byte ptr [esi + 0xc], 4
            je L0052e4f2
            mov ecx, dword ptr [esp + 0x18]
            push esi
            lea edx, [esp + 0x28]
            push ebp
            mov dword ptr [ecx], eax
            lea eax, [esp + 0x18]
            push edx
            push eax
            push ecx
            call ValueQuery
            test eax, eax
            jne L0052e508
            add dword ptr [esp + 0x18], 8
    L0052e4f2:
            mov eax, dword ptr [ebx]
            add esi, 0x18
            inc edi
            cmp edi, eax
            jl L0052e4c5
    L0052e4fc:
            mov ecx, dword ptr [esp + 0x18]
            xor eax, eax
            mov dword ptr [ecx], 0
    L0052e508:
            pop edi
            pop esi
            pop ebp
            pop ebx
            pop ecx
            ret
    }
#endif
}

// FUNCTION: CARMA2_HW 0x0052e510
br_error C2_NAKED C2_HOOK_CDECL BrTokenValueQueryAllSize(br_size_t* psize, void* block, br_tv_template* template) {
#ifndef CARPOCALYPSE2_MATCHING
    br_tv_template_entry* tp;
    int e;
    int n;
    br_size_t extra_size;

    if (!template->names_resolved) {
        templateResolveNames(template);
    }
    extra_size = 0;
    n = 1;
    for (e = 0; e < template->n_entries; e++) {
        tp = &template->entries[e];
        if (tp->token == BR_NULL_TOKEN) {
            continue;
        }
        if ((tp->flags & 4) == 0) {
            continue;
        }
        n++;
        extra_size += ValueExtraSize(block, tp);
    }
    *psize = extra_size + sizeof(br_token_value) * n;
    return 0;
#else
    __asm {
            sub esp, 8
            push ebx
            push ebp
            mov ebp, dword ptr [esp + 0x1c]
            push esi
            push edi
            xor edi, edi
            mov eax, dword ptr [ebp + 0x18]
            mov dword ptr [esp + 0x14], 1
            cmp eax, edi
            mov dword ptr [esp + 0x10], edi
            jne L0052e559
            mov eax, dword ptr [ebp]
            mov esi, dword ptr [ebp + 4]
            cmp eax, edi
            jle L0052e559
    L0052e53a:
            mov eax, dword ptr [esi + 4]
            test eax, eax
            je L0052e54e
            push 0
            push eax
            call BrTokenCreate
            add esp, 8
            mov dword ptr [esi], eax
    L0052e54e:
            mov eax, dword ptr [ebp]
            add esi, 0x18
            inc edi
            cmp edi, eax
            jl L0052e53a
    L0052e559:
            mov eax, dword ptr [ebp]
            mov esi, dword ptr [ebp + 4]
            xor edi, edi
            test eax, eax
            jle L0052e593
            mov ebx, dword ptr [esp + 0x20]
    L0052e569:
            cmp dword ptr [esi], 0
            je L0052e588
            test byte ptr [esi + 0xc], 4
            je L0052e588
            mov ecx, dword ptr [esp + 0x14]
            push esi
            inc ecx
            push ebx
            mov dword ptr [esp + 0x1c], ecx
            call ValueExtraSize
            add dword ptr [esp + 0x10], eax
    L0052e588:
            mov eax, dword ptr [ebp]
            add esi, 0x18
            inc edi
            cmp edi, eax
            jl L0052e569
    L0052e593:
            mov ecx, dword ptr [esp + 0x10]
            mov edx, dword ptr [esp + 0x14]
            pop edi
            pop esi
            lea eax, [ecx + edx*8]
            mov ecx, dword ptr [esp + 0x14]
            pop ebp
            pop ebx
            mov dword ptr [ecx], eax
            xor eax, eax
            add esp, 8
            ret
    }
#endif
}

// FUNCTION: CARMA2_HW 0x0052e5b0
br_error C2_HOOK_CDECL BrTokenValueSet(void* mem, br_uint_32* pcombined_mask, br_token t, uintptr_t value, br_tv_template* template) {
    br_int_32 o;
    br_error r;
    br_token_value tv;

    tv.t = t;
#ifdef CARPOCALYPSE2_MATCHING
    tv.v.u32 = value;
#else
    /* 64-bit hosts: value may carry a pointer at full width. */
    *(void**)&tv.v = (void*)value;
#endif
    if (template->n_map_entries == 0) {
        templateMakeMap(template);
    }
    o = t - template->map_base;
    if (o < 0 || o >= template->n_map_entries) {
        return 0x1001;
    }
    if (template->map_set_entry[o] == NULL) {
        return 0x1001;
    }
    r = ValueSet(mem, &tv, template->map_set_entry[o]);
    if (r != 0) {
        return r;
    }
    *pcombined_mask |= template->map_set_entry[o]->mask;
    return 0;
}

// FUNCTION: CARMA2_HW 0x0052e940
br_error C2_NAKED C2_HOOK_CDECL BrTokenValueSetMany(void* mem, br_int_32* pcount, br_uint_32* pcombined_mask, br_token_value* tv, br_tv_template* template) {
#ifndef CARPOCALYPSE2_MATCHING
    br_int_32 o;
    br_int_32 n;
    br_error r;
    br_uint_32 cm;

    if (template->n_map_entries == 0) {
        templateMakeMap(template);
    }
    cm = 0;
    n = 0;
    for (; tv->t != 0; tv++) {
        o = tv->t - template->map_base;
        if (o < 0 || o >= template->n_map_entries) {
            continue;
        }
        if (template->map_set_entry[o])  {
            r = ValueSet(mem, tv, template->map_set_entry[o]);
            if (r != 0) {
                continue;
            }
            cm |= template->map_set_entry[o]->mask;
            n++;
        }
    }
    if (pcount != NULL) {
        *pcount = n;
    }
    if (pcombined_mask != NULL) {
        *pcombined_mask |= cm;
    }
    return 0;
#else
    __asm {
            push ebx
            push ebp
            push esi
            push edi
            mov edi, dword ptr [esp + 0x24]
            xor ebp, ebp
            mov eax, dword ptr [edi + 0xc]
            test eax, eax
            jne L0052e957
            push edi
            call templateMakeMap
    L0052e957:
            mov ebx, dword ptr [esp + 0x20]
            mov dword ptr [esp + 0x24], 0
            mov eax, dword ptr [ebx]
            test eax, eax
            je L0052e9b2
    L0052e969:
            sub eax, dword ptr [edi + 8]
            js L0052e9a8
            cmp eax, dword ptr [edi + 0xc]
            jge L0052e9a8
            lea esi, [eax*4]
            mov eax, dword ptr [edi + 0x14]
            mov eax, dword ptr [eax + esi]
            test eax, eax
            je L0052e9a8
            mov ecx, dword ptr [esp + 0x14]
            push eax
            push ebx
            push ecx
            call ValueSet
            test eax, eax
            jne L0052e9a8
            mov edx, dword ptr [edi + 0x14]
            mov eax, dword ptr [edx + esi]
            mov ecx, dword ptr [eax + 0x14]
            mov eax, dword ptr [esp + 0x24]
            or ebp, ecx
            inc eax
            mov dword ptr [esp + 0x24], eax
    L0052e9a8:
            mov eax, dword ptr [ebx + 8]
            add ebx, 8
            test eax, eax
            jne L0052e969
    L0052e9b2:
            mov eax, dword ptr [esp + 0x18]
            test eax, eax
            je L0052e9c0
            mov ecx, dword ptr [esp + 0x24]
            mov dword ptr [eax], ecx
    L0052e9c0:
            mov eax, dword ptr [esp + 0x1c]
            test eax, eax
            je L0052e9ca
            or dword ptr [eax], ebp
    L0052e9ca:
            pop edi
            pop esi
            pop ebp
            xor eax, eax
            pop ebx
            ret
    }
#endif
}

void C2_HOOK_STDCALL DumpMatrixInteger(br_int_32* ip, int rows, int cols, char* prefix, char* info_0, char* info_n, br_putline_cbfn* putline, void* arg) {
    int i;
    int j;
    char* cp;
    char value[128];

    for (i = 0; i < rows; i++) {
        cp = value;
        cp += BrSprintf(value, "%s%s[", prefix, (i == 0) ? info_0 : info_n);
        for (j = 0; j < cols; j++, ip++) {
            cp += BrSprintf(cp, "%s%d", (j == 0) ? "" : ",", *ip);
        }
        BrStrCpy(cp, "]");
        putline(value, arg);
    }
}

void C2_HOOK_STDCALL DumpMatrixFixed(br_fixed_ls* xp, int rows, int cols, char* prefix, char* info_0, char* info_n, br_putline_cbfn* putline, void* arg) {
    int i;
    int j;
    char* cp;
    char value[128];

    for (i = 0; i < rows; i++) {
        cp = value;
        cp += BrSprintf(value, "%s%s[", prefix, (i == 0) ? info_0 : info_n);
        for (j = 0; j < cols; j++, xp++) {
            cp += BrSprintf(cp, "%s%f", (j == 0) ? "" : ",", (float)*xp / 65536.f);
        }
        BrStrCpy(cp, "]");
        putline(value, arg);
    }
}

void C2_HOOK_STDCALL DumpMatrixFloat(br_float* fp, int rows, int cols, char* prefix, char* info_0, char* info_n, br_putline_cbfn* putline, void* arg) {
    int i;
    int j;
    char* cp;
    char value[128];

    for (i = 0; i < rows; i++) {
        cp = value;
        cp += BrSprintf(value, "%s%s[", prefix, (i == 0) ? info_0 : info_n);
        for (j = 0; j < cols; j++, fp++) {
            cp += BrSprintf(cp, "%s%f", (j == 0) ? "" : ",", *fp);
        }
        BrStrCpy(cp, "]");
        putline(value, arg);
    }
}

void C2_HOOK_STDCALL DumpObject(br_object* h, char* prefix, char* info, br_putline_cbfn* putline, void* arg) {
    char value[128];
    char* dev_ident;
    char* h_ident;
    br_object* dev;

    if (h == NULL) {
        BrSprintf(value, "%s%sNULL OBJECT", prefix, info);
    } else {
        dev = (br_object*)h->dispatch->_device(h);
        if (dev == NULL) {
            dev_ident = "<NULL DEVICE>";
        } else {
            dev_ident = dev->dispatch->_identifier(dev);
        }
        if (dev_ident == NULL) {
            dev_ident = "<NULL>";
        }
        h_ident = h->dispatch->_identifier(h);
        if (h == NULL) {
            h_ident = "<NULL>";
        }
        BrSprintf(value, "%s%s%08x %s:%s  %s", prefix, info, h, dev_ident, h_ident,
                   BrTokenIdentifier(h->dispatch->_type(h)));
    }
    putline(value, arg);
}

// FUNCTION: CARMA2_HW 0x0052e9e0
void C2_HOOK_CDECL BrTokenValueDump(br_token_value* tv, char* prefix, br_putline_cbfn* putline, void* arg) {
#ifdef CARPOCALYPSE2_MATCHING
    (void) tv;
    (void) prefix;
    (void) putline;
    (void) arg;

#else
    char* id;
    char value[128];
    char tmp[128];
    char info_0[64];
    char* info_n;
    char* pvalue;
    int n;
    int i;
    br_object** pph;
    void** ppp;
    br_token* pt;

    info_n = "                                         ";

    for (n = 0; tv->t != BR_NULL_TOKEN; n++, tv++) {

        id = BrTokenIdentifier(tv->t);
        if (id == NULL) {
            BrSprintf(tmp, "%s  %5d:  ??? (%08x) %08x", prefix, n, tv->t, tv->v.p);
            putline(tmp, arg);
            continue;
        }
        BrSprintf(info_0, "%5d:  %-32s ", n, id);
        pvalue = value;
        switch (BrTokenType(tv->t)) {
        case BR_NULL_TOKEN:
            pvalue = "NULL";
            break;
        case BRT_NONE:
            value[0] = 0;
            break;
        case BRT_BOOLEAN:
            pvalue = tv->v.b ? "TRUE" : "FALSE";
            break;
        case BRT_POINTER:
            BrSprintf(value, "%08x");
            break;
        case BRT_TOKEN:
            pvalue = BrTokenIdentifier(tv->v.t);
            if (pvalue == NULL) {
                BrSprintf(value, "??? %d");
                pvalue = value;
            }
            break;
        case BRT_INT_8:
            BrSprintf(value, "%d", tv->v.i8);
            break;
        case BRT_UINT_8:
            BrSprintf(value, "%u (0x%02x)", tv->v.u8, tv->v.u8);
            break;
        case BRT_INT_16:
            BrSprintf(value, "%d", tv->v.i16);
            break;
        case BRT_UINT_16:
            BrSprintf(value, "%u (0x%04x)", tv->v.u16, tv->v.u16);
            break;
        case BRT_INT_32:
            BrSprintf(value, "%d", tv->v.i32);
            break;
        case BRT_UINT_32:
            BrSprintf(value, "%u (0x%08x)", tv->v.u32, tv->v.u32);
            break;
        case BRT_FIXED:
            BrSprintf(value, "%f", (float)tv->v.x / 65536.f);
            break;
        case BRT_FLOAT:
            BrSprintf(value, "%f", tv->v.f);
            break;
        case BRT_ANGLE:
            /* FIXME: is this missing a >>16 ? */
            BrSprintf(value, "%f", (float)BrFixedMul(tv->v.x, 360 << 16));
            break;
        case BRT_COLOUR_RGB:
            BrSprintf(value, "(%d,%d,%d)", (tv->v.rgb >> 16) & 0xff, (tv->v.rgb >> 8) & 0xff, (tv->v.rgb >> 0) & 0xff);
            break;
        case BRT_OBJECT:
            DumpObject(tv->v.o, prefix, info_0, putline, arg);
            continue;
        case BRT_VECTOR2_INTEGER:
            if (tv->v.v2_i == NULL) {
                pvalue = "NULL";
                break;
            }
            DumpMatrixInteger(tv->v.v2_i->v, 1, 2, prefix, info_0, info_n, putline, arg);
            continue;
        case BRT_VECTOR3_INTEGER:
            if (tv->v.v3_i == NULL) {
                pvalue = "NULL";
                break;
            }
            DumpMatrixInteger(tv->v.v3_i->v, 1, 3, prefix, info_0, info_n, putline, arg);
            continue;
        case BRT_VECTOR4_INTEGER:
        if (tv->v.v4_i == NULL) {
            pvalue = "NULL";
            break;
        }
            DumpMatrixInteger(tv->v.v4_i->v, 1, 4, prefix, info_0, info_n, putline, arg);
            continue;
        case BRT_VECTOR2_FIXED:
            if (tv->v.v2_x == NULL) {
                pvalue = "NULL";
                break;
            }
            DumpMatrixFixed(tv->v.v2_x->v, 1, 2, prefix, info_0, info_n, putline, arg);
            continue;
        case BRT_VECTOR3_FIXED:
            if (tv->v.v3_x == NULL) {
                pvalue = "NULL";
                break;
            }
            DumpMatrixFixed(tv->v.v3_x->v, 1, 3, prefix, info_0, info_n, putline, arg);
            continue;
        case BRT_VECTOR4_FIXED:
            if (tv->v.v4_x == NULL) {
                pvalue = "NULL";
                break;
            }
            DumpMatrixFixed(tv->v.v4_x->v, 1, 4, prefix, info_0, info_n, putline, arg);
            continue;
        case BRT_VECTOR2_FLOAT:
            if (tv->v.v2_f == NULL) {
                pvalue = "NULL";
                break;
            }
            DumpMatrixFloat(tv->v.v2_f->v, 1, 2, prefix, info_0, info_n, putline, arg);
            continue;
        case BRT_VECTOR3_FLOAT:
            if (tv->v.v3_f == NULL) {
                pvalue = "NULL";
                break;
            }
            DumpMatrixFloat(tv->v.v3_f->v, 1, 3, prefix, info_0, info_n, putline, arg);
            continue;
        case BRT_VECTOR4_FLOAT:
            if (tv->v.v4_f == NULL) {
                pvalue = "NULL";
                break;
            }
            DumpMatrixFloat(tv->v.v4_f->v, 1, 4, prefix, info_0, info_n, putline, arg);
            continue;
        case BRT_MATRIX23_FIXED:
            if (tv->v.m23_x == NULL) {
                pvalue = "NULL";
                break;
            }
            DumpMatrixFixed(tv->v.m23_x->m[0], 2, 3, prefix, info_0, info_n, putline, arg);
            continue;
        case BRT_MATRIX34_FIXED:
            if (tv->v.m34_x == NULL) {
                pvalue = "NULL";
                break;
            }
            DumpMatrixFixed(tv->v.m34_x->m[0], 3, 4, prefix, info_0, info_n, putline, arg);
            continue;
        case BRT_MATRIX4_FIXED:
            if (tv->v.m4_x == NULL) {
                pvalue = "NULL";
                break;
            }
            DumpMatrixFixed(tv->v.m4_x->m[0], 4, 4, prefix, info_0, info_n, putline, arg);
            continue;
        case BRT_MATRIX23_FLOAT:
            if (tv->v.m23_f == NULL) {
                pvalue = "NULL";
                break;
            }
            DumpMatrixFloat(tv->v.m23_f->m[0], 2, 3, prefix, info_0, info_n, putline, arg);
            continue;
        case BRT_MATRIX34_FLOAT:
            if (tv->v.m34_f == NULL) {
                pvalue = "NULL";
                break;
            }
            DumpMatrixFloat(tv->v.m34_f->m[0], 3, 4, prefix, info_0, info_n, putline, arg);
            continue;
        case BRT_MATRIX4_FLOAT:
            if (tv->v.m4_f == NULL) {
                pvalue = "NULL";
                break;
            }
            DumpMatrixFloat(tv->v.m4_f->m[0], 4, 4, prefix, info_0, info_n, putline, arg);
            continue;
        case BRT_STRING:
            if (tv->v.str == NULL) {
                pvalue = "NULL";
            } else {
                pvalue = tv->v.str;
            }
            break;
        case BRT_CONSTANT_STRING:
            if (tv->v.cstr == NULL) {
                pvalue = "NULL";
            } else {
                pvalue = (char*)tv->v.cstr;
            }
            break;
        case BRT_OBJECT_LIST:
            if (tv->v.ol == NULL) {
                pvalue = "NULL";
                break;
            }
            pph = tv->v.ol;
            if (*pph == NULL) {
                pvalue = "Empty List";
                break;
            }
            for (i = 0; pph[i] != NULL ; i++) {
                DumpObject(pph[i], prefix, (i == 0) ? info_0 : info_n, putline, arg);
            }
            continue;
        case BRT_TOKEN_LIST:
            if (tv->v.tl == NULL) {
                pvalue = "NULL";
                break;
            }
            pt = tv->v.tl;
            if (*pt == BR_NULL_TOKEN) {
                pvalue = "Empty List";
                break;
            }

            for (i = 0; pt[i] != BR_NULL_TOKEN ; i++) {
                pvalue = BrTokenIdentifier(pt[i]);
                if (pvalue == NULL) {
                    BrSprintf(value, "??? %d", pt[i]);
                    pvalue = value;
                }

                BrSprintf(tmp, "%s%s%s", prefix, (i == 0) ? info_0 : info_n, pvalue);
                putline(tmp,arg);
            }
            continue;
        case BRT_POINTER_LIST:
            if (tv->v.pl == NULL) {
                pvalue = "NULL";
                break;
            }
            ppp = tv->v.pl;
            if (*ppp == NULL) {
                pvalue = "Empty List";
                break;
            }
            for (i=0; ppp[i] != NULL ; i++) {
                BrSprintf(value, "%08x", ppp[i]);
                pvalue = value;
                BrSprintf(tmp, "%s%s%s", prefix, (i == 0) ? info_0 : info_n, pvalue);
                putline(tmp, arg);
            }
            continue;
        case BRT_TOKEN_VALUE_LIST:
            if (tv->v.tvl == NULL) {
                pvalue = "NULL";
                break;
            }
            BrTokenValueDump(tv->v.tvl, prefix, putline, arg);
            continue;
        default:
            break;
        }
        BrSprintf(tmp, "%s%s%s", prefix, info_0, pvalue);
        putline(tmp, arg);
    }
#endif
}

#define T_FALSE (T_KEYWORD + 0)
#define T_TRUE  (T_KEYWORD + 1)

// FUNCTION: CARMA2_HW 0x0052e9f0
br_error C2_NAKED C2_HOOK_CDECL BrStringToTokenValue(br_token_value* buffer, br_size_t buffer_size, char* str) {
#ifndef CARPOCALYPSE2_MATCHING
    br_lexer* l;
    br_error r;
    // GLOBAL: CARMA2_HW 0x00667e60
    static br_lexer_keyword keywords[4] = {
        { "true",   T_TRUE  },
        { "t",      T_TRUE  },
        { "false",  T_FALSE },
        { "f",      T_FALSE },
    };

    l = BrLexerAllocate(keywords, BR_ASIZE(keywords));
    if (l == NULL) {
        return 0x1003;
    }
    r = BrLexerPushString(l, str, NULL);
    if (r != 0) {
        BrLexerFree(l);
        return r;
    }
    l->advance(l);
    parseTokenValue(l, buffer, buffer_size);
    BrLexerFree(l);
    return 0;
#else
    __asm {
            sub esp, 0x28
            push ebx
            push ebp
            push esi
            push edi
            push 4
            push 0x667e60
            call BrLexerAllocate
            mov edi, eax
            xor ebx, ebx
            add esp, 8
            cmp edi, ebx
            jne L0052ea1b
            mov eax, 0x1003
            pop edi
            pop esi
            pop ebp
            pop ebx
            add esp, 0x28
            ret
    L0052ea1b:
            mov eax, dword ptr [esp + 0x44]
            push ebx
            push eax
            push edi
            call BrLexerPushString
            add esp, 0xc
            mov esi, eax
            cmp esi, ebx
            push edi
            je L0052ea43
            call BrLexerFree
            add esp, 4
            mov eax, esi
            pop edi
            pop esi
            pop ebp
            pop ebx
            add esp, 0x28
            ret
    L0052ea43:
            call dword ptr [edi + 0x18]
            mov eax, dword ptr [esp + 0x40]
            mov esi, dword ptr [esp + 0x3c]
            cmp eax, 8
            jb L0052ed64
            lea ebp, [eax - 8]
            mov eax, dword ptr [edi + 4]
            cmp eax, ebx
            je L0052ed5f
    L0052ea65:
            cmp eax, 0x2c
            jne L0052ea74
    L0052ea6a:
            push edi
            call dword ptr [edi + 0x18]
            cmp dword ptr [edi + 4], 0x2c
            je L0052ea6a
    L0052ea74:
            cmp dword ptr [edi + 4], 2
            jne L0052ed5f
            cmp ebp, 8
            jbe L0052ed5f
            mov ecx, dword ptr [edi + 8]
            push 0x27
            lea edx, [esp + 0x14]
            push ecx
            push edx
            mov byte ptr [esp + 0x43], bl
            call BrStrNCpy
            add esp, 0xc
            push edi
            call dword ptr [edi + 0x18]
            cmp byte ptr [esp + 0x11], bl
            jne L0052eafd
            mov al, byte ptr [esp + 0x10]
            cmp al, 0x77
            je L0052eaeb
            cmp al, 0x57
            je L0052eaeb
            cmp al, 0x68
            je L0052eadf
            cmp al, 0x48
            je L0052eadf
            cmp al, 0x62
            je L0052ead3
            cmp al, 0x42
            je L0052ead3
            cmp al, 0x6d
            je L0052eacc
            cmp al, 0x4d
            jne L0052eafd
    L0052eacc:
            push 0x667f1c
            jmp L0052eaf0
    L0052ead3:
            lea ecx, [esp + 0x10]
            push 0x667f10
            push ecx
            jmp L0052eaf5
    L0052eadf:
            lea edx, [esp + 0x10]
            push 0x667f08
            push edx
            jmp L0052eaf5
    L0052eaeb:
            push 0x667f00
    L0052eaf0:
            lea eax, [esp + 0x14]
            push eax
    L0052eaf5:
            call BrStrCpy
            add esp, 8
    L0052eafd:
            mov dword ptr [esi], ebx
            mov eax, dword ptr [edi + 4]
            cmp eax, 0x3a
            je L0052eb43
            cmp eax, 0x3d
            je L0052eb43
            push 2
            lea ecx, [esp + 0x14]
            push 0x667ee0
            lea edx, [esp + 0x4c]
            push ecx
            push edx
            call BrTokenFindType
            cmp eax, ebx
            mov dword ptr [esi], eax
            je L0052ed3c
            cmp dword ptr [esp + 0x44], 2
            jne L0052ed3c
            mov dword ptr [esi + 4], 0x81
            jmp L0052ed3c
    L0052eb43:
            push edi
            call dword ptr [edi + 0x18]
            mov eax, dword ptr [edi + 4]
            add eax, -2
            cmp eax, 0x7f
            ja L0052ed2c
            xor ecx, ecx
            mov cl, byte ptr [eax + 0x52ed90]
            jmp dword ptr [ecx*4 + 0x52ed78]
            push 2
            lea edx, [esp + 0x14]
            push 0x667e88
            lea eax, [esp + 0x4c]
            push edx
            push eax
            call BrTokenFindType
            cmp eax, ebx
            mov dword ptr [esi], eax
            je L0052ed38
            mov eax, dword ptr [esp + 0x44]
            sub eax, 0xb
            je L0052eba0
            dec eax
            jne L0052ed38
            fld dword ptr [edi + 8]
            fstp dword ptr [esi + 4]
            jmp L0052ed38
    L0052eba0:
            fld dword ptr [edi + 8]
            fmul dword ptr ds:[0x58b868]
            call _ftol
            mov dword ptr [esi + 4], eax
            jmp L0052ed38
            cmp dword ptr [edi + 8], ebx
            jl L0052ebce
            push 9
            lea ecx, [esp + 0x14]
            push 0x667e90
            lea edx, [esp + 0x4c]
            push ecx
            push edx
            jmp L0052ebdf
    L0052ebce:
            push 6
            lea eax, [esp + 0x14]
            push 0x667eb8
            lea ecx, [esp + 0x4c]
            push eax
            push ecx
    L0052ebdf:
            call BrTokenFindType
            cmp eax, ebx
            mov dword ptr [esi], eax
            je L0052ed38
            mov edx, dword ptr [esp + 0x44]
            lea eax, [edx - 2]
            cmp eax, 0xa
            ja L0052ed38
            jmp dword ptr [eax*4 + 0x52ee10]
            mov eax, dword ptr [edi + 8]
            mov dword ptr [esi + 4], eax
            jmp L0052ed38
            mov ecx, dword ptr [edi + 8]
            mov dword ptr [esi + 4], ecx
            jmp L0052ed38
            mov dx, word ptr [edi + 8]
            mov word ptr [esi + 4], dx
            jmp L0052ed38
            mov ax, word ptr [edi + 8]
            mov word ptr [esi + 4], ax
            jmp L0052ed38
            mov cl, byte ptr [edi + 8]
            mov byte ptr [esi + 4], cl
            jmp L0052ed38
            mov dl, byte ptr [edi + 8]
            mov byte ptr [esi + 4], dl
            jmp L0052ed38
            fild dword ptr [edi + 8]
            fstp dword ptr [esi + 4]
            jmp L0052ed38
            mov eax, dword ptr [edi + 8]
            shl eax, 0x10
            mov dword ptr [esi + 4], eax
            jmp L0052ed38
            mov eax, dword ptr [edi + 8]
            xor ecx, ecx
            cmp eax, ebx
            setne cl
            mov dword ptr [esi + 4], ecx
            jmp L0052ed38
            push 1
            lea edx, [esp + 0x14]
            push 0x667ed0
            lea eax, [esp + 0x4c]
            push edx
            push eax
            call BrTokenFindType
            cmp eax, ebx
            mov dword ptr [esi], eax
            je L0052ed38
            mov ecx, dword ptr [edi + 8]
            push ecx
            call BrTokenFind
            add esp, 4
            cmp eax, ebx
            mov dword ptr [esi + 4], eax
            jne L0052ed38
            mov dword ptr [esi], ebx
            jmp L0052ed38
            push 1
            lea edx, [esp + 0x14]
            push 0x667ed4
            lea eax, [esp + 0x4c]
            push edx
            push eax
            call BrTokenFindType
            cmp eax, ebx
            mov dword ptr [esi], eax
            je L0052ed38
            mov eax, dword ptr [edi + 4]
            xor ecx, ecx
            cmp eax, 0x81
            sete cl
            mov dword ptr [esi + 4], ecx
            jmp L0052ed38
            push 2
            lea edx, [esp + 0x14]
            push 0x667ed8
            lea eax, [esp + 0x4c]
            push edx
            push eax
            call BrTokenFindType
            cmp eax, ebx
            mov dword ptr [esi], eax
            je L0052ed38
            mov ecx, dword ptr [edi + 8]
            push ecx
            call BrStrLen
            add esp, 4
            inc eax
            cmp eax, ebp
            jle L0052ed13
            mov dword ptr [esi], ebx
            jmp L0052ed38
    L0052ed13:
            sub ebp, eax
            push eax
            lea ecx, [ebp + esi]
            mov dword ptr [esi + 4], ecx
            mov edx, dword ptr [edi + 8]
            push edx
            push ecx
            call BrMemCpy
            add esp, 0xc
            jmp L0052ed38
    L0052ed2c:
            push 0x667f24
            push edi
            call dword ptr [edi + 0x2c]
            add esp, 8
    L0052ed38:
            push edi
            call dword ptr [edi + 0x18]
    L0052ed3c:
            cmp dword ptr [esi], ebx
            je L0052ed48
            add esi, 8
            sub ebp, 8
            jmp L0052ed54
    L0052ed48:
            push 0x667f38
            push edi
            call dword ptr [edi + 0x2c]
            add esp, 8
    L0052ed54:
            mov eax, dword ptr [edi + 4]
            cmp eax, ebx
            jne L0052ea65
    L0052ed5f:
            mov dword ptr [esi], ebx
            mov dword ptr [esi + 4], ebx
    L0052ed64:
            push edi
            call BrLexerFree
            add esp, 4
            xor eax, eax
            pop edi
            pop esi
            pop ebp
            pop ebx
            add esp, 0x28
            ret
            nop
            _emit 0x76
            _emit 0xec
            _emit 0x52
            _emit 0x00
            _emit 0xe2
            _emit 0xec
            _emit 0x52
            _emit 0x00
            _emit 0xb6
            _emit 0xeb
            _emit 0x52
            _emit 0x00
            _emit 0x65
            _emit 0xeb
            _emit 0x52
            _emit 0x00
            _emit 0xb4
            _emit 0xec
            _emit 0x52
            _emit 0x00
            _emit 0x2c
            _emit 0xed
            _emit 0x52
            _emit 0x00
            _emit 0x00
            _emit 0x01
            _emit 0x02
            _emit 0x03
            _emit 0x05
            _emit 0x05
            _emit 0x05
            _emit 0x05
            _emit 0x05
            _emit 0x05
            _emit 0x05
            _emit 0x05
            _emit 0x05
            _emit 0x05
            _emit 0x05
            _emit 0x05
            _emit 0x05
            _emit 0x05
            _emit 0x05
            _emit 0x05
            _emit 0x05
            _emit 0x05
            _emit 0x05
            _emit 0x05
            _emit 0x05
            _emit 0x05
            _emit 0x05
            _emit 0x05
            _emit 0x05
            _emit 0x05
            _emit 0x05
            _emit 0x05
            _emit 0x05
            _emit 0x05
            _emit 0x05
            _emit 0x05
            _emit 0x05
            _emit 0x05
            _emit 0x05
            _emit 0x05
            _emit 0x05
            _emit 0x05
            _emit 0x05
            _emit 0x05
            _emit 0x05
            _emit 0x05
            _emit 0x05
            _emit 0x05
            _emit 0x05
            _emit 0x05
            _emit 0x05
            _emit 0x05
            _emit 0x05
            _emit 0x05
            _emit 0x05
            _emit 0x05
            _emit 0x05
            _emit 0x05
            _emit 0x05
            _emit 0x05
            _emit 0x05
            _emit 0x05
            _emit 0x05
            _emit 0x05
            _emit 0x05
            _emit 0x05
            _emit 0x05
            _emit 0x05
            _emit 0x05
            _emit 0x05
            _emit 0x05
            _emit 0x05
            _emit 0x05
            _emit 0x05
            _emit 0x05
            _emit 0x05
            _emit 0x05
            _emit 0x05
            _emit 0x05
            _emit 0x05
            _emit 0x05
            _emit 0x05
            _emit 0x05
            _emit 0x05
            _emit 0x05
            _emit 0x05
            _emit 0x05
            _emit 0x05
            _emit 0x05
            _emit 0x05
            _emit 0x05
            _emit 0x05
            _emit 0x05
            _emit 0x05
            _emit 0x05
            _emit 0x05
            _emit 0x05
            _emit 0x05
            _emit 0x05
            _emit 0x05
            _emit 0x05
            _emit 0x05
            _emit 0x05
            _emit 0x05
            _emit 0x05
            _emit 0x05
            _emit 0x05
            _emit 0x05
            _emit 0x05
            _emit 0x05
            _emit 0x05
            _emit 0x05
            _emit 0x05
            _emit 0x05
            _emit 0x05
            _emit 0x05
            _emit 0x05
            _emit 0x05
            _emit 0x05
            _emit 0x05
            _emit 0x05
            _emit 0x05
            _emit 0x05
            _emit 0x05
            _emit 0x05
            _emit 0x05
            _emit 0x04
            _emit 0x04
            _emit 0x64
            _emit 0xec
            _emit 0x52
            _emit 0x00
            _emit 0x38
            _emit 0xed
            _emit 0x52
            _emit 0x00
            _emit 0x38
            _emit 0xed
            _emit 0x52
            _emit 0x00
            _emit 0x35
            _emit 0xec
            _emit 0x52
            _emit 0x00
            _emit 0x40
            _emit 0xec
            _emit 0x52
            _emit 0x00
            _emit 0x1b
            _emit 0xec
            _emit 0x52
            _emit 0x00
            _emit 0x28
            _emit 0xec
            _emit 0x52
            _emit 0x00
            _emit 0x05
            _emit 0xec
            _emit 0x52
            _emit 0x00
            _emit 0x10
            _emit 0xec
            _emit 0x52
            _emit 0x00
            _emit 0x56
            _emit 0xec
            _emit 0x52
            _emit 0x00
            _emit 0x4b
            _emit 0xec
            _emit 0x52
            _emit 0x00
            _emit 0x90
            _emit 0x90
            _emit 0x90
            _emit 0x90
    }
#endif
}

static br_error parseTokenValue(br_lexer* l, br_token_value* tv, br_size_t size) {
    int len;
    char name[40];
    br_token type;
    br_error r;

    if (size < sizeof(br_token_value)) {
        return 0x1004;
    }

    size -= sizeof(br_token_value);

    while (l->current.id != T_EOF) {
        while (l->current.id == T_COMMA) {
            l->advance(l);
        }
        if (l->current.id != T_IDENT) {
            break;
        }
        if (size <= sizeof(br_token_value)) {
            r = 0x1004;
            break;
        }
        name[BR_ASIZE(name) - 1] = '\0';
        BrStrNCpy(name, l->current.v.string, BR_ASIZE(name) - 1);
        l->advance(l);
        if (name[1] == '\0') {
            if (name[0] == 'w' || name [0] == 'W') {
                BrStrCpy(name, "width");
            } else if(name[0] == 'h' || name [0] == 'H') {
                BrStrCpy(name, "height");
            } else if(name[0] == 'b' || name [0] == 'B') {
                BrStrCpy(name, "pixel_bits");
            } else if(name[0] == 'm' || name [0] == 'M') {
                BrStrCpy(name, "mode");
            }
        }
        tv->t = BR_NULL_TOKEN;

        if (l->current.id == T_COLON || l->current.id == T_EQUAL) {
            l->advance(l);

            switch ((int)l->current.id) {
            case T_IDENT: {
                // GLOBAL: CARMA2_HW 0x00667ed0
                static br_token ident_types[] = { BRT_TOKEN };
                tv->t = BrTokenFindType(&type, name, ident_types, BR_ASIZE(ident_types));
                if (tv->t == BR_NULL_TOKEN) {
                    break;
                }
                tv->v.t = BrTokenFind(l->current.v.string);
                if (tv->v.t == BR_NULL_TOKEN) {
                    tv->t = BR_NULL_TOKEN;
                }
                break;
            }
            case T_STRING: {
                // GLOBAL: CARMA2_HW 0x00667ed8
                static br_token string_types[] = { BRT_STRING, BRT_CONSTANT_STRING };
                tv->t = BrTokenFindType(&type, name, string_types, BR_ASIZE(string_types));
                if (tv->t == BR_NULL_TOKEN) {
                    break;
                }
                len = BrStrLen(l->current.v.string) + 1;
                if (len > (int)size) {
                    tv->t = BR_NULL_TOKEN;
                    r = 0x1004;
                    break;
                }
                size -= len;
                tv->v.str = (char *)tv + size;
                BrMemCpy(tv->v.str, l->current.v.string, len);
                break;
            }
            case T_INTEGER: {
                // GLOBAL: CARMA2_HW 0x00667e90
                static br_token pos_int_types[] = {
                    BRT_INT_32, BRT_UINT_32, BRT_INT_16,
                    BRT_UINT_16, BRT_INT_8, BRT_UINT_8,
                    BRT_FLOAT, BRT_FIXED, BRT_BOOLEAN };
                // GLOBAL: CARMA2_HW 0x00667eb8
                static br_token neg_int_types[] = {
                    BRT_INT_32, BRT_INT_16, BRT_INT_8,
                    BRT_FLOAT, BRT_FIXED, BRT_BOOLEAN };

                if (l->current.v.integer < 0) {
                    tv->t = BrTokenFindType(&type, name, neg_int_types, BR_ASIZE(neg_int_types));
                } else {
                    tv->t = BrTokenFindType(&type, name, pos_int_types, BR_ASIZE(pos_int_types));
                }
                if (tv->t == BR_NULL_TOKEN) {
                    break;
                }
                switch (type) {
                case BRT_BOOLEAN:
                    tv->v.b = l->current.v.integer != 0;
                    break;
                case BRT_INT_8:
                    tv->v.i8 = (br_int_8)l->current.v.integer;
                    break;
                case BRT_UINT_8:
                    tv->v.u8 = (br_uint_8)l->current.v.integer;
                    break;
                case BRT_INT_16:
                    tv->v.i16 = (br_int_16)l->current.v.integer;
                    break;
                case BRT_UINT_16:
                    tv->v.u16 = (br_uint_16)l->current.v.integer;
                    break;
                case BRT_INT_32:
                    tv->v.i32 = l->current.v.integer;
                    break;
                case BRT_UINT_32:
                    tv->v.u32 = (br_uint_32)l->current.v.integer;
                    break;
                case BRT_FIXED:
                    tv->v.x = l->current.v.integer << 16;
                    break;
                case BRT_FLOAT:
                    tv->v.f = (float)l->current.v.integer;
                    break;
                default:
                    abort();
                    break;
                }
                break;
            }
            case T_REAL: {
                // GLOBAL: CARMA2_HW 0x00667e88
                static br_token real_types[] = { BRT_FLOAT, BRT_FIXED };

                tv->t = BrTokenFindType(&type, name, real_types, BR_ASIZE(real_types));
                if (tv->t == BR_NULL_TOKEN) {
                    break;
                }
                switch (type) {
                case BRT_FIXED:
                    tv->v.x = (br_fixed_ls)(l->current.v.real * 65536.f);
                    break;
                case BRT_FLOAT:
                    tv->v.f = l->current.v.real;
                    break;
                default:
                    abort();
                    break;
                }
                break;
            }
            case T_FALSE:
            case T_TRUE: {
                // GLOBAL: CARMA2_HW 0x00667ed4
                static br_token bool_types[] = { BRT_BOOLEAN };

                tv->t = BrTokenFindType(&type, name, bool_types, BR_ASIZE(bool_types));
                if (tv->t == BR_NULL_TOKEN) {
                    break;
                }
                tv->v.b = l->current.id == T_TRUE;;
                break;
            }
            default:
                l->error(l, "expecting a value");
                break;
            }
            l->advance(l);
        } else {
            // GLOBAL: CARMA2_HW 0x00667ee0
            static br_token none_types[] = { BRT_NONE, BRT_BOOLEAN };

            tv->t = BrTokenFindType(&type, name, none_types, BR_ASIZE(none_types));
            if (tv->t != BR_NULL_TOKEN && type == BRT_BOOLEAN) {
                tv->v.b = T_TRUE;
            }
        }

        if (tv->t != BR_NULL_TOKEN) {
            tv++;
            size -= sizeof(br_token_value);
        } else {
            l->error(l, "unknown token");
        }
    }
    tv->t = BR_NULL_TOKEN;
    tv->v.u32 = 0;
    return r;
}

// FUNCTION: CARMA2_HW 0x0052ee40
br_boolean C2_NAKED C2_HOOK_CDECL BrTokenValueCompare(br_token_value* tv1, br_token_value* tv2) {
#ifndef CARPOCALYPSE2_MATCHING
    int i;

    if (tv1 == NULL && tv2 == NULL) {
        return 1;
    }
    if (tv1 == NULL || tv2 == NULL) {
        return 0;
    }
    for (; tv1->t == tv2->t; tv1++, tv2++) {
        if (tv1->t == BR_NULL_TOKEN) {
            return 1;
        }
        switch (BrTokenType(tv1->t)) {
        case BRT_BOOLEAN:
            if (!tv1->v.b != !tv2->v.b) {
                return 0;
            }
            break;
        case BRT_INT_8:
        case BRT_UINT_8:
            if (tv1->v.u8 != tv2->v.u8) {
                return 0;
            }
            break;
        case BRT_INT_16:
        case BRT_UINT_16:
            if (tv1->v.u16 != tv2->v.u16) {
                return 0;
            }
            break;
        case BRT_INT_32:
        case BRT_UINT_32:
        case BRT_COLOUR_RGB:
            if (tv1->v.u32 != tv2->v.u32) {
                return 0;
            }
            break;
        case BRT_FIXED:
            if (tv1->v.x != tv2->v.x) {
                return 0;
            }
            break;
        case BRT_FLOAT:
        case BRT_ANGLE:
            if (tv1->v.f != tv2->v.f) {
                return 0;
            }
            break;
        case BRT_OBJECT:
            if (tv1->v.o != tv2->v.o) {
                return 0;
            }
            break;
        case BRT_VECTOR2_INTEGER:
            if (BrMemCpy(tv1->v.p, tv2->v.p, 2 * sizeof(int)) != 0) {
                return 0;
            }
            break;
        case BRT_VECTOR3_INTEGER:
            if (BrMemCpy(tv1->v.p, tv2->v.p, 3 * sizeof(int)) != 0) {
                return 0;
            }
            break;
        case BRT_VECTOR4_INTEGER:
            if (BrMemCpy(tv1->v.p, tv2->v.p, 3 * sizeof(int)) != 0) {
                return 0;
            }
            break;
        case BRT_VECTOR2_FIXED:
            if (BrMemCpy(tv1->v.p, tv2->v.p, 2 * sizeof(br_fixed_ls)) != 0) {
                return 0;
            }
            break;
        case BRT_VECTOR3_FIXED:
            if (BrMemCpy(tv1->v.p, tv2->v.p, 3 * sizeof(br_fixed_ls)) != 0) {
                return 0;
            }
            break;
        case BRT_VECTOR4_FIXED:
            if (BrMemCpy(tv1->v.p, tv2->v.p, 3 * sizeof(br_fixed_ls)) != 0) {
                return 0;
            }
            break;
        case BRT_VECTOR2_FLOAT:
            if (BrMemCpy(tv1->v.p, tv2->v.p, 2 * sizeof(float)) != 0) {
                return 0;
            }
            break;
        case BRT_VECTOR3_FLOAT:
            if (BrMemCpy(tv1->v.p, tv2->v.p, 3 * sizeof(float)) != 0) {
                return 0;
            }
            break;
        case BRT_VECTOR4_FLOAT:
            if (BrMemCpy(tv1->v.p, tv2->v.p, 3 * sizeof(float)) != 0) {
                return 0;
            }
            break;
        case BRT_MATRIX23_FIXED:
            if (BrMemCpy(tv1->v.p, tv2->v.p, 6 * sizeof(br_fixed_ls)) != 0) {
                return 0;
            }
            break;
        case BRT_MATRIX34_FIXED:
            if (BrMemCpy(tv1->v.p, tv2->v.p, 12 * sizeof(br_fixed_ls)) != 0) {
                return 0;
            }
            break;
        case BRT_MATRIX4_FIXED:
            if (BrMemCpy(tv1->v.p, tv2->v.p, 16 * sizeof(br_fixed_ls)) != 0) {
                return 0;
            }
            break;
        case BRT_MATRIX23_FLOAT:
            if (BrMemCpy(tv1->v.p, tv2->v.p, 6 * sizeof(float)) != 0) {
                return 0;
            }
            break;
        case BRT_MATRIX34_FLOAT:
            if (BrMemCpy(tv1->v.p, tv2->v.p, 12 * sizeof(float)) != 0) {
                return 0;
            }
            break;
        case BRT_MATRIX4_FLOAT:
            if (BrMemCpy(tv1->v.p, tv2->v.p, 16 * sizeof(float)) != 0) {
                return 0;
            }
            break;
        case BRT_STRING:
        case BRT_CONSTANT_STRING:
            if (BrStrCmp(tv1->v.cstr, tv1->v.cstr) != 0) {
                return 0;
            }
            break;
        case BRT_OBJECT_LIST:
        case BRT_POINTER_LIST:
            for (i = 0;; i++) {
#if 1
                if (tv1->v.pl[i] != tv2->v.pl[i]) {
#else
                if (tv1->v.pl[i] != tv1->v.pl[i]) {
#endif
                    return 0;
                }
                if (tv1->v.pl[i] == NULL) {
                    break;
                }
            }
            break;
        case BRT_TOKEN_LIST:
            for (i = 0;; i++) {
#if 1
                if (tv1->v.tl[i] != tv2->v.tl[i]) {
#else
                if (tv1->v.tl[i] != tv1->v.tl[i]) {
#endif
                    return 0;
                }
                if (tv1->v.tl[i] == BR_NULL_TOKEN) {
                    break;
                }
            }
            break;
        default:
            if (tv1->v.p != tv2->v.p) {
                return 0;
            }
            break;
        }
    }
    return 0;
#else
    __asm {
            push esi
            mov esi, dword ptr [esp + 8]
            push edi
            mov edi, dword ptr [esp + 0x10]
            test esi, esi
            jne L0052ee5a
            test edi, edi
            jne L0052ee5a
            mov eax, 1
            pop edi
            pop esi
            ret
    L0052ee5a:
            test esi, esi
            je L0052ef8b
            test edi, edi
            je L0052ef8b
            mov eax, dword ptr [esi]
            mov ecx, dword ptr [edi]
            cmp eax, ecx
            jne L0052ef8b
    L0052ee76:
            test eax, eax
            je L0052ef83
            push eax
            call BrTokenType
            add esp, 4
            dec eax
            cmp eax, 0x22
            ja L0052ef60
            xor ecx, ecx
            mov cl, byte ptr [eax + 0x52efe8]
            jmp dword ptr [ecx*4 + 0x52ef90]
            mov edx, dword ptr [edi + 4]
            mov eax, dword ptr [esi + 4]
            push edx
            push eax
            call BrStrCmp
            add esp, 8
            test eax, eax
            jne L0052ef8b
            jmp L0052ef6a
            mov eax, dword ptr [esi + 4]
    L0052eec0:
            cmp dword ptr [eax], 0
            je L0052ef6a
            add eax, 4
            jmp L0052eec0
            mov ecx, dword ptr [edi + 4]
            mov edx, dword ptr [esi + 4]
            push 8
            push ecx
            push edx
            jmp L0052ef0c
            mov eax, dword ptr [edi + 4]
            mov ecx, dword ptr [esi + 4]
            push 0xc
            push eax
            push ecx
            jmp L0052ef0c
            push 0x10
            jmp L0052ef04
            mov ecx, dword ptr [edi + 4]
            mov edx, dword ptr [esi + 4]
            push 0x18
            push ecx
            push edx
            jmp L0052ef0c
            mov eax, dword ptr [edi + 4]
            mov ecx, dword ptr [esi + 4]
            push 0x30
            push eax
            push ecx
            jmp L0052ef0c
            push 0x40
    L0052ef04:
            mov edx, dword ptr [edi + 4]
            mov eax, dword ptr [esi + 4]
            push edx
            push eax
    L0052ef0c:
            call BrMemCmp
            add esp, 0xc
            test eax, eax
            jne L0052ef8b
            jmp L0052ef6a
            mov edx, dword ptr [esi + 4]
            mov eax, dword ptr [edi + 4]
            xor ecx, ecx
            test edx, edx
            sete cl
            xor edx, edx
            test eax, eax
            sete dl
            cmp ecx, edx
            jne L0052ef8b
            mov cl, byte ptr [esi + 4]
            mov dl, byte ptr [edi + 4]
            xor eax, eax
            test cl, cl
            sete al
            xor ecx, ecx
            test dl, dl
            sete cl
            cmp eax, ecx
            jne L0052ef8b
            xor edx, edx
            cmp word ptr [esi + 4], dx
            sete dl
            xor eax, eax
            cmp word ptr [edi + 4], ax
            sete al
            cmp edx, eax
            jne L0052ef8b
    L0052ef60:
            mov ecx, dword ptr [esi + 4]
            mov eax, dword ptr [edi + 4]
            cmp ecx, eax
            jne L0052ef8b
    L0052ef6a:
            mov eax, dword ptr [esi + 8]
            mov ecx, dword ptr [edi + 8]
            add esi, 8
            add edi, 8
            cmp eax, ecx
            je L0052ee76
            xor eax, eax
            pop edi
            pop esi
            ret
    L0052ef83:
            mov eax, 1
            pop edi
            pop esi
            ret
    L0052ef8b:
            pop edi
            xor eax, eax
            pop esi
            ret
            _emit 0x6a
            _emit 0xef
            _emit 0x52
            _emit 0x00
            _emit 0x1a
            _emit 0xef
            _emit 0x52
            _emit 0x00
            _emit 0x32
            _emit 0xef
            _emit 0x52
            _emit 0x00
            _emit 0x4a
            _emit 0xef
            _emit 0x52
            _emit 0x00
            _emit 0xce
            _emit 0xee
            _emit 0x52
            _emit 0x00
            _emit 0xda
            _emit 0xee
            _emit 0x52
            _emit 0x00
            _emit 0xe6
            _emit 0xee
            _emit 0x52
            _emit 0x00
            _emit 0xce
            _emit 0xee
            _emit 0x52
            _emit 0x00
            _emit 0xda
            _emit 0xee
            _emit 0x52
            _emit 0x00
            _emit 0xe6
            _emit 0xee
            _emit 0x52
            _emit 0x00
            _emit 0xce
            _emit 0xee
            _emit 0x52
            _emit 0x00
            _emit 0xda
            _emit 0xee
            _emit 0x52
            _emit 0x00
            _emit 0xe6
            _emit 0xee
            _emit 0x52
            _emit 0x00
            _emit 0xea
            _emit 0xee
            _emit 0x52
            _emit 0x00
            _emit 0xf6
            _emit 0xee
            _emit 0x52
            _emit 0x00
            _emit 0x02
            _emit 0xef
            _emit 0x52
            _emit 0x00
            _emit 0xea
            _emit 0xee
            _emit 0x52
            _emit 0x00
            _emit 0xf6
            _emit 0xee
            _emit 0x52
            _emit 0x00
            _emit 0x02
            _emit 0xef
            _emit 0x52
            _emit 0x00
            _emit 0xa0
            _emit 0xee
            _emit 0x52
            _emit 0x00
            _emit 0xbd
            _emit 0xee
            _emit 0x52
            _emit 0x00
            _emit 0x60
            _emit 0xef
            _emit 0x52
            _emit 0x00
            _emit 0x00
            _emit 0x01
            _emit 0x15
            _emit 0x15
            _emit 0x02
            _emit 0x02
            _emit 0x03
            _emit 0x03
            _emit 0x15
            _emit 0x15
            _emit 0x15
            _emit 0x15
            _emit 0x15
            _emit 0x15
            _emit 0x15
            _emit 0x04
            _emit 0x05
            _emit 0x06
            _emit 0x07
            _emit 0x08
            _emit 0x09
            _emit 0x0a
            _emit 0x0b
            _emit 0x0c
            _emit 0x0d
            _emit 0x0e
            _emit 0x0f
            _emit 0x10
            _emit 0x11
            _emit 0x12
            _emit 0x13
            _emit 0x13
            _emit 0x14
            _emit 0x14
            _emit 0x14
            _emit 0x90
            _emit 0x90
            _emit 0x90
            _emit 0x90
            _emit 0x90
            _emit 0x8b
            _emit 0x44
            _emit 0x24
            _emit 0x08
            _emit 0x83
            _emit 0x38
            _emit 0x00
            _emit 0x74
            _emit 0x0a
            _emit 0x8b
            _emit 0x48
            _emit 0x08
            _emit 0x83
            _emit 0xc0
            _emit 0x08
            _emit 0x85
            _emit 0xc9
            _emit 0x75
            _emit 0xf6
            _emit 0x33
            _emit 0xc0
            _emit 0xc2
            _emit 0x08
            _emit 0x00
            _emit 0x90
            _emit 0x90
            _emit 0x90
            _emit 0x90
            _emit 0x90
            _emit 0x90
            _emit 0x90
            _emit 0x90
    }
#endif
}

br_error C2_HOOK_CDECL BrTokenValueDup(br_token_value** dstp, br_token_value* src) {
    br_token_value* tv;

	for (tv = src; tv->t != BR_NULL_TOKEN; tv++) {
    }
	return 0;
}
