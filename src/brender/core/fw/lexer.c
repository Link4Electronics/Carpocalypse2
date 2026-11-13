#include "lexer.h"

#include "brprintf.h"
#include "file.h"
#include "fwsetup.h"
#include "resource.h"

#include "core/math/fixed.h"
#include "core/std/brstdlib.h"

#include "c2_stdlib.h"

#ifdef CARPOCALYPSE2_MATCHING
extern br_boolean C2_HOOK_CDECL BrIsPrint();
extern br_int_32 C2_HOOK_CDECL BrSprintfN();
extern char IID_IDirectInputDevice2A[];
extern void __cdecl _ftol();
extern char tokenTypes[];
#endif
// FUNCTION: CARMA2_HW 0x0052f3f0
void C2_HOOK_CDECL lexerError(br_lexer* l, const char* string) {
}

// FUNCTION: CARMA2_HW 0x0052f390
br_lexer* C2_HOOK_CDECL BrLexerAllocate(br_lexer_keyword* keywords, int nkeywords) {
    br_lexer* l;

    l = BrResAllocate(fw.res, sizeof(br_lexer), BR_MEMORY_LEXER);
    if (l == NULL) {
        return NULL;
    }
    l->keywords = keywords;
    l->nkeywords = nkeywords;
    l->advance = lexerAdvance;
    l->source = NULL;
    l->string_buffer = BrResAllocate(l, 256, BR_MEMORY_LEXER);
    l->string_buffer_size = 256;
    l->error = lexerError;
    return l;
}

// FUNCTION: CARMA2_HW 0x0052f400
char C2_HOOK_CDECL BrLexerCommentSet(br_lexer* l, char eol_comment) {
    char old;

    old = l->eol_comment;
    l->eol_comment = eol_comment;
    return old;
}

// FUNCTION: CARMA2_HW 0x0052f410
br_lexer_error_cbfn* C2_HOOK_CDECL BrLexerErrorSet(br_lexer* l, br_lexer_error_cbfn* error) {
    br_lexer_error_cbfn* old;

    old = l->error;
    if (error == NULL) {
        l->error = lexerError;
    } else {
        l->error = error;
    }
    return old == lexerError ? NULL : old;
}

// FUNCTION: CARMA2_HW 0x0052f450
void C2_HOOK_CDECL BrLexerFree(br_lexer* l) {

    BrResFree(l);
}

// FUNCTION: CARMA2_HW 0x0052f4f0
void C2_HOOK_CDECL fileGetchar(br_lexer_source* source) {

    source->next = BrFileGetChar(source->ptr);
}

// FUNCTION: CARMA2_HW 0x0052f460
br_error C2_HOOK_CDECL BrLexerPushFile(br_lexer* l, char* file) {
    br_lexer_source* s;
    void* f;
    int mode;

    mode = BR_FS_MODE_TEXT;
    f = BrFileOpenRead(file, 0, NULL, &mode);
    if (f == NULL) {
        return 0x1002;
    }
    s = BrResAllocate(l, sizeof(br_lexer_source), BR_MEMORY_LEXER);
    if (s == NULL) {
        return 0x1003;
    }
    BrResAdd(s, f);
    s->name = file;
    s->line = 1;
    s->getchar = fileGetchar;
    s->ptr = f;
    s->getchar(s);
    s->prev = l->source;
    l->source = s;
    return 0;
}

// FUNCTION: CARMA2_HW 0x0052f570
void C2_HOOK_CDECL stringGetchar(br_lexer_source* source) {
    char* cp;

    cp = source->ptr;
    if (cp != NULL && *cp != '\0') {
        source->next = *cp;
        source->ptr = cp + 1;
    } else {
        source->next = -1;
    }
}

// FUNCTION: CARMA2_HW 0x0052f510
br_error C2_HOOK_CDECL BrLexerPushString(br_lexer* l, char* string, char* name) {
    br_lexer_source* s;

    s = BrResAllocate(l, sizeof(br_lexer_source), BR_MEMORY_LEXER);
    if (s == NULL) {
        return 0x1003;
    }
    s->name = name;
    s->line = 1;
    s->getchar = stringGetchar;
    s->ptr = string;
    s->getchar(s);
    s->prev = l->source;
    l->source = s;
    return 0;
}

// FUNCTION: CARMA2_HW 0x0052f5a0
br_lexer_source* C2_HOOK_CDECL BrLexerPop(br_lexer* l) {
    br_lexer_source* s;

    s = l->source;
    if (s == NULL) {
        return NULL;
    }
    l->source = s->prev;
    BrResFree(s);
    return s;
}

// FUNCTION: CARMA2_HW 0x0052f5c0
void C2_HOOK_STDCALL lexerAdvance(br_lexer* l) {
    int n;
    br_boolean got_point;

    while (1) {
        if (l->source == NULL) {
            l->current.id = T_EOF;
            return;
        }
        if (l->source->next == -1) {
            BrLexerPop(l);
            continue;
        }
        if (l->eol_comment != '\0' && l->eol_comment == l->source->next) {
            l->source->getchar(l->source);
            while (l->source->next != '\n' && l->source->next != -1) {
                l->source->getchar(l->source);
            }
            continue;
        }
        if (l->source->next == '\n') {
            l->source->line++;
            l->source->getchar(l->source);
            continue;
        }
        if (l->source->next == '\"') {
            for (n = 0; n < l->string_buffer_size; n++) {
                l->source->getchar(l->source);
                l->string_buffer[n] = l->source->next;
                if ((l->source->next == '\"') || (l->source->next == '\n') || (l->source->next == -1)) {
                    break;
                }
            }
            if (l->source->next != '\"') {
                l->current.id = T_ERROR;
                l->current.v.string = "Unterminated string";
            } else {
                l->string_buffer[n] = '\0';
                l->current.id = T_STRING;
                l->current.v.string = l->string_buffer;
            }
            l->source->getchar(l->source);
            return;
        }
        if (BrIsSpace(l->source->next) || l->source->next == '\x1a') {
            l->source->getchar(l->source);
            continue;
        }
        if (BrIsAlpha(l->source->next) != 0 || l->source->next == '_') {
            l->string_buffer[0] = l->source->next;
            for (n = 1; n < l->string_buffer_size - 1; n++) {
                l->source->getchar(l->source);
                if (BrIsAlpha(l->source->next) == 0 && BrIsDigit(l->source->next) == 0 && l->source->next != '_') {
                    break;
                }
                l->string_buffer[n] = l->source->next;
            }
            l->string_buffer[n] = '\0';
            l->current.v.string = l->string_buffer;
            for (n = 0; n < l->nkeywords; n++) {
                if ((l->keywords[n].name[0] == l->string_buffer[0]) && (BrStrCmp(l->keywords[n].name, l->string_buffer) == 0)) {
                    l->current.id = l->keywords[n].id;
                    return;
                }
            }
            l->current.id = T_IDENT;
            return;
        }
        if (BrIsDigit(l->source->next) != 0) {
            got_point = 0;
            l->string_buffer[0] = l->source->next;
            for (n = 1; n < l->string_buffer_size - 1; n++) {
                l->source->getchar(l->source);
                if (l->source->next == '.') {
                    got_point = 1;
                } else if (!(('0' <= l->source->next && l->source->next <= '9')
                    || ('A' <= l->source->next && l->source->next <= 'F') || (l->source->next == 'X')
                    || ('a' <= l->source->next && l->source->next <= 'f') || (l->source->next == 'x'))) {
                    break;
                }
                l->string_buffer[n] = l->source->next;
            }
            l->string_buffer[n] = '\0';
            if (got_point != 0) {
                l->current.v.real = BrStrToF(l->string_buffer, NULL);
                l->current.id = T_REAL;
            } else {
                l->current.v.integer = BrStrToL(l->string_buffer, NULL, 0);
                l->current.id = T_INTEGER;
            }
            return;
        }
        if (BrIsPrint(l->source->next) != 0) {
            l->current.id = l->source->next;
            l->source->getchar(l->source);
            return;
        }
        l->current.id = T_ERROR;
        l->current.v.string = "Unexpected character";
        l->source->getchar(l->source);
        return;
    }
}

void C2_HOOK_STDCALL lexerAdvanceDump(br_lexer* l) {
    abort();
}

// FUNCTION: CARMA2_HW 0x0052f950
br_error C2_HOOK_CDECL BrLexerDumpSet(br_lexer* l, br_putline_cbfn* putline, void* putline_arg) {

    return 0;
}

// FUNCTION: CARMA2_HW 0x0052f960
void C2_NAKED C2_HOOK_CDECL BrLexerTokenError(br_lexer* l, br_lexer_token_id t) {
#ifndef CARPOCALYPSE2_MATCHING
    int i;
    char tmp[256];

    if (l->current.id != T_ERROR) {
        switch (t) {
        case T_EOF:
            l->error(l, "expected end of file");
            break;
        case T_ERROR:
            l->error(l, l->current.v.string);
            break;
        case T_IDENT:
            l->error(l, "expected an identifier");
            break;
        case T_STRING:
            l->error(l, "expected a string");
            break;
        case T_INTEGER:
            l->error(l, "expected an integer");
            break;
        case T_REAL:
            l->error(l, "expected a real number");
            break;
        default:
            if (t < 0x80) {
                if (BrIsPrint(t) != 0) {
                    BrSprintfN(tmp, sizeof(tmp), "expected '%c'", t);
                } else {
                    BrSprintfN(tmp, sizeof(tmp), "expected unknown token %d", t);
                }
            } else {
                for (i = 0; i < l->nkeywords; i++) {
                    if (l->keywords[i].id == t) {
                        break;
                    }
                }
                if (i < l->nkeywords) {
                    BrSprintfN(tmp, sizeof(tmp), "expected '%s'", l->keywords[i].name);
                } else {
                    BrSprintfN(tmp, sizeof(tmp), "expected unknown keyword %d", t);
                }
            }
            l->error(l, tmp);
            break;
        }
    }

    while (l->current.id != t && l->current.id != T_EOF) {
        l->advance(l);
    }
#else
    __asm {
            sub esp, 0x100
            push esi
            mov esi, dword ptr [esp + 0x108]
            push edi
            mov edi, dword ptr [esp + 0x110]
            cmp dword ptr [esi + 4], 1
            je L0052fa50
            cmp edi, 5
            ja L0052f9c7
            jmp dword ptr [edi*4 + 0x52fa70]
            push offset tokenTypes+0x1ca4
            jmp L0052fa49
            mov eax, dword ptr [esi + 8]
            push eax
            jmp L0052fa49
            push offset tokenTypes+0x1cbc
            jmp L0052fa49
            push offset tokenTypes+0x1cd4
            jmp L0052fa49
            push offset tokenTypes+0x1ce8
            jmp L0052fa49
            push offset tokenTypes+0x1cfc
            jmp L0052fa49
    L0052f9c7:
            cmp edi, 0x80
            jl L0052fa0e
            mov edx, dword ptr [esi + 0x10]
            xor eax, eax
            test edx, edx
            jle L0052f9ea
            mov ecx, dword ptr [esi + 0xc]
            add ecx, 4
    L0052f9de:
            cmp dword ptr [ecx], edi
            je L0052f9ea
            inc eax
            add ecx, 8
            cmp eax, edx
            jl L0052f9de
    L0052f9ea:
            cmp eax, edx
            jge L0052f9fc
            mov ecx, dword ptr [esi + 0xc]
            mov edx, dword ptr [ecx + eax*8]
            push edx
            push offset tokenTypes+0x1d14
            jmp L0052fa32
    L0052f9fc:
            push edi
            push offset tokenTypes+0x1d24
            lea ecx, [esp + 0x10]
            push 0x100
            push ecx
            jmp L0052fa3c
    L0052fa0e:
            push edi
            call BrIsPrint
            add esp, 4
            test eax, eax
            push edi
            je L0052fa2d
            push offset tokenTypes+0x1d40
            lea edx, [esp + 0x10]
            push 0x100
            push edx
            jmp L0052fa3c
    L0052fa2d:
            push offset tokenTypes+0x1d50
    L0052fa32:
            lea eax, [esp + 0x10]
            push 0x100
            push eax
    L0052fa3c:
            call BrSprintfN
            add esp, 0x10
            lea ecx, [esp + 8]
            push ecx
    L0052fa49:
            push esi
            call dword ptr [esi + 0x2c]
            add esp, 8
    L0052fa50:
            mov eax, dword ptr [esi + 4]
            cmp eax, edi
            je L0052fa66
    L0052fa57:
            test eax, eax
            je L0052fa66
            push esi
            call dword ptr [esi + 0x18]
            mov eax, dword ptr [esi + 4]
            cmp eax, edi
            jne L0052fa57
    L0052fa66:
            pop edi
            pop esi
            add esp, 0x100
            ret
    }
#endif
}

// FUNCTION: CARMA2_HW 0x0052fa90
void C2_HOOK_CDECL BrLexerPosition(br_lexer* l, char* buf, br_size_t buf_size) {

    if ((l->source != NULL) && (l->source->name != NULL)) {
        BrSprintfN(buf, buf_size, "%s:%d ", l->source->name, l->source->line);
    } else {
        *buf = '\0';
    }
}

// FUNCTION: CARMA2_HW 0x0052fad0
br_fixed_ls C2_NAKED C2_HOOK_CDECL BrParseFixed(br_lexer* l) {
#ifndef CARPOCALYPSE2_MATCHING
    br_boolean neg;
    br_fixed_ls x;

    if (l->current.id == T_PLUS) {
        l->advance(l);
    }
    neg = l->current.id == T_DASH;
    if (neg) {
        l->advance(l);
    }
    if (l->current.id == T_REAL) {
        x = BrFloatToFixed(l->current.v.real);
        l->advance(l);
    } else if (l->current.id == T_INTEGER) {
        x = BrIntToFixed(l->current.v.integer);
        l->advance(l);
    } else {
        l->error(l, "expecting a fixed");
        l->advance(l);
        x = 0;
    }
    if (neg) {
        x = -x;
    }
    return x;
#else
    __asm {
            push ebx
            push esi
            mov esi, dword ptr [esp + 0xc]
            xor ebx, ebx
            push edi
            cmp dword ptr [esi + 4], 0x2b
            jne L0052fae3
            push esi
            call dword ptr [esi + 0x18]
    L0052fae3:
            cmp dword ptr [esi + 4], 0x2d
            jne L0052faf2
            push esi
            mov ebx, 1
            call dword ptr [esi + 0x18]
    L0052faf2:
            mov eax, dword ptr [esi + 4]
            cmp eax, 5
            jne L0052fb10
            fld dword ptr [esi + 8]
            fmul dword ptr [IID_IDirectInputDevice2A+0x4e0]
            call _ftol
            push esi
            mov edi, eax
            call dword ptr [esi + 0x18]
            jmp L0052fb33
    L0052fb10:
            cmp eax, 4
            jne L0052fb21
            mov edi, dword ptr [esi + 8]
            push esi
            shl edi, 0x10
            call dword ptr [esi + 0x18]
            jmp L0052fb33
    L0052fb21:
            push offset tokenTypes+0x1d74
            push esi
            call dword ptr [esi + 0x2c]
            add esp, 8
            push esi
            call dword ptr [esi + 0x18]
            xor edi, edi
    L0052fb33:
            test ebx, ebx
            mov eax, edi
            je L0052fb3b
            neg eax
    L0052fb3b:
            pop edi
            pop esi
            pop ebx
            ret
            nop
    }
#endif
}

// FUNCTION: CARMA2_HW 0x0052fb40
br_float C2_HOOK_CDECL BrParseFloat(br_lexer* l) {
    br_boolean neg;
    br_float f;

    neg = 0;
    if (l->current.id == T_PLUS) {
        l->advance(l);
    }
    if (l->current.id == T_DASH) {
        neg = 1;
        l->advance(l);
    }
    if (l->current.id == T_REAL) {
        f = l->current.v.real;
        l->advance(l);
    } else if (l->current.id == T_INTEGER) {
        f = (float)l->current.v.integer;
        l->advance(l);
    } else {
        l->error(l, "expecting a float");
        l->advance(l);
        f = 0.0f;
    }
    if (neg) {
        f = -f;
    }
    return f;
}

// FUNCTION: CARMA2_HW 0x0052fbb0
br_int_32 C2_NAKED C2_HOOK_CDECL BrParseInteger(br_lexer* l) {
#ifndef CARPOCALYPSE2_MATCHING
    br_boolean neg;
    br_int_32 i;

    if (l->current.id == T_PLUS) {
        l->advance(l);
    }
    neg = l->current.id == T_DASH;
    if (neg) {
        l->advance(l);
    }
    if (l->current.id == T_INTEGER) {
        i = l->current.v.integer;
        l->advance(l);
    } else {
        l->error(l, "expecting an integer");
        l->advance(l);
        i = 0;
    }
    if (neg) {
        i = -i;
    }
    return i;
#else
    __asm {
            push ebx
            push esi
            mov esi, dword ptr [esp + 0xc]
            xor ebx, ebx
            push edi
            cmp dword ptr [esi + 4], 0x2b
            jne L0052fbc3
            push esi
            call dword ptr [esi + 0x18]
    L0052fbc3:
            cmp dword ptr [esi + 4], 0x2d
            jne L0052fbd2
            push esi
            mov ebx, 1
            call dword ptr [esi + 0x18]
    L0052fbd2:
            cmp dword ptr [esi + 4], 4
            jne L0052fbe1
            mov edi, dword ptr [esi + 8]
            push esi
            call dword ptr [esi + 0x18]
            jmp L0052fbf3
    L0052fbe1:
            push offset tokenTypes+0x1d9c
            push esi
            call dword ptr [esi + 0x2c]
            add esp, 8
            push esi
            call dword ptr [esi + 0x18]
            xor edi, edi
    L0052fbf3:
            test ebx, ebx
            mov eax, edi
            je L0052fbfb
            neg eax
    L0052fbfb:
            pop edi
            pop esi
            pop ebx
            ret
            nop
    }
#endif
}

// FUNCTION: CARMA2_HW 0x0052fc00
br_int_32 C2_NAKED C2_HOOK_CDECL BrParseVectorFixed(br_lexer* l, br_fixed_ls* v, br_int_32 max) {
#ifndef CARPOCALYPSE2_MATCHING
    int n;

    if (l->current.id == T_LSQUARE) {
        l->advance(l);
    } else {
        BrLexerTokenError(l, T_LSQUARE);
    }
    for (n = 0; n < max; n++) {
#ifdef BRENDER_FIX_BUGS
        if (l->current.id == T_RSQUARE) {
            break;
        }
#endif
        *v = BrParseFixed(l);
        v++;
        if (l->current.id == T_RSQUARE) {
            n++;
            break;
        }
        if (n != max - 1) {
            if (l->current.id == T_COMMA) {
                l->advance(l);
            } else {
                BrLexerTokenError(l, T_COMMA);
            }
        }
    }
    if (l->current.id == T_RSQUARE) {
        l->advance(l);
    } else {
        BrLexerTokenError(l, T_RSQUARE);
    }
    return n;
#else
    __asm {
            push ebx
            push ebp
            push esi
            mov esi, dword ptr [esp + 0x10]
            push edi
            cmp dword ptr [esi + 4], 0x5b
            je L0052fc1b
            push 0x5b
            push esi
            call BrLexerTokenError
            add esp, 8
            jmp L0052fc1f
    L0052fc1b:
            push esi
            call dword ptr [esi + 0x18]
    L0052fc1f:
            mov ebp, dword ptr [esp + 0x1c]
            xor ebx, ebx
            test ebp, ebp
            jle L0052fc5e
            mov edi, dword ptr [esp + 0x18]
    L0052fc2d:
            push esi
            call BrParseFixed
            mov dword ptr [edi], eax
            mov eax, dword ptr [esi + 4]
            add esp, 4
            add edi, 4
            cmp eax, 0x5d
            je L0052fc5e
            cmp eax, 0x2c
            je L0052fc55
            push 0x2c
            push esi
            call BrLexerTokenError
            add esp, 8
            jmp L0052fc59
    L0052fc55:
            push esi
            call dword ptr [esi + 0x18]
    L0052fc59:
            inc ebx
            cmp ebx, ebp
            jl L0052fc2d
    L0052fc5e:
            cmp dword ptr [esi + 4], 0x5d
            je L0052fc77
            push 0x5d
            push esi
            call BrLexerTokenError
            add esp, 8
            lea eax, [ebx + 1]
            pop edi
            pop esi
            pop ebp
            pop ebx
            ret
    L0052fc77:
            push esi
            call dword ptr [esi + 0x18]
            pop edi
            pop esi
            lea eax, [ebx + 1]
            pop ebp
            pop ebx
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
            nop
            nop
            nop
    }
#endif
}

// FUNCTION: CARMA2_HW 0x0052fc90
br_int_32 C2_NAKED C2_HOOK_CDECL BrParseVectorFloat(br_lexer* l, br_float* v, br_int_32 max) {
#ifndef CARPOCALYPSE2_MATCHING
    int n;

    if (l->current.id == T_LSQUARE) {
        l->advance(l);
    } else {
        BrLexerTokenError(l, T_LSQUARE);
    }
    for (n = 0; n < max; n++) {
#ifdef BRENDER_FIX_BUGS
        if (l->current.id == T_RSQUARE) {
            break;
        }
#endif
        *v = BrParseFloat(l);
        v++;
        if (l->current.id == T_RSQUARE) {
            n++;
            break;
        }
        if (n != max - 1) {
            if (l->current.id == T_COMMA) {
                l->advance(l);
            } else {
                BrLexerTokenError(l, T_COMMA);
            }
        }
    }
    if (l->current.id == T_RSQUARE) {
        l->advance(l);
    } else {
        BrLexerTokenError(l, T_RSQUARE);
    }
    return n;
#else
    __asm {
            push ebx
            push ebp
            push esi
            mov esi, dword ptr [esp + 0x10]
            push edi
            cmp dword ptr [esi + 4], 0x5b
            je L0052fcab
            push 0x5b
            push esi
            call BrLexerTokenError
            add esp, 8
            jmp L0052fcaf
    L0052fcab:
            push esi
            call dword ptr [esi + 0x18]
    L0052fcaf:
            mov eax, dword ptr [esp + 0x1c]
            xor ebp, ebp
            test eax, eax
            jle L0052fd59
            mov ebx, dword ptr [esp + 0x18]
    L0052fcc1:
            mov eax, dword ptr [esi + 4]
            xor edi, edi
            cmp eax, 0x2b
            jne L0052fccf
            push esi
            call dword ptr [esi + 0x18]
    L0052fccf:
            cmp dword ptr [esi + 4], 0x2d
            jne L0052fcde
            push esi
            mov edi, 1
            call dword ptr [esi + 0x18]
    L0052fcde:
            mov eax, dword ptr [esi + 4]
            cmp eax, 5
            jne L0052fcf7
            fld dword ptr [esi + 8]
            fstp dword ptr [esp + 0x14]
            push esi
            call dword ptr [esi + 0x18]
            fld dword ptr [esp + 0x14]
            jmp L0052fd23
    L0052fcf7:
            cmp eax, 4
            jne L0052fd0d
            fild dword ptr [esi + 8]
            push esi
            fstp dword ptr [esp + 0x18]
            call dword ptr [esi + 0x18]
            fld dword ptr [esp + 0x14]
            jmp L0052fd23
    L0052fd0d:
            push offset tokenTypes+0x1d88
            push esi
            call dword ptr [esi + 0x2c]
            add esp, 8
            push esi
            call dword ptr [esi + 0x18]
            fld dword ptr [IID_IDirectInputDevice2A+0x4e4]
    L0052fd23:
            test edi, edi
            je L0052fd29
            fchs
    L0052fd29:
            _emit 0xd9
        _emit 0x1b
            mov eax, dword ptr [esi + 4]
            add ebx, 4
            cmp eax, 0x5d
            je L0052fd59
            cmp eax, 0x2c
            je L0052fd48
            push 0x2c
            push esi
            call BrLexerTokenError
            add esp, 8
            jmp L0052fd4c
    L0052fd48:
            push esi
            call dword ptr [esi + 0x18]
    L0052fd4c:
            mov eax, dword ptr [esp + 0x1c]
            inc ebp
            cmp ebp, eax
            jl L0052fcc1
    L0052fd59:
            cmp dword ptr [esi + 4], 0x5d
            je L0052fd72
            push 0x5d
            push esi
            call BrLexerTokenError
            add esp, 8
            lea eax, [ebp + 1]
            pop edi
            pop esi
            pop ebp
            pop ebx
            ret
    L0052fd72:
            push esi
            call dword ptr [esi + 0x18]
            pop edi
            lea eax, [ebp + 1]
            pop esi
            pop ebp
            pop ebx
            ret
    }
#endif
}

// FUNCTION: CARMA2_HW 0x0052fd80
br_int_32 C2_NAKED C2_HOOK_CDECL BrParseMatrixFixed(br_lexer* l, br_fixed_ls* m, br_int_32 width, br_int_32 max_h) {
#ifndef CARPOCALYPSE2_MATCHING
    int n;

    if (l->current.id == T_LSQUARE) {
        l->advance(l);
    } else {
        BrLexerTokenError(l, T_LSQUARE);
    }
    for (n = 0; n < max_h; n++, m = m + width) {
#ifdef BRENDER_FIX_BUGS
        if (l->current.id == T_RSQUARE) {
            break;
        }
#endif
        BrParseVectorFixed(l, m, width);
        if (l->current.id == T_RSQUARE) {
            n++;
            break;
        }
        if (n != max_h - 1) {
            if (l->current.id == T_COMMA) {
                l->advance(l);
            } else {
                BrLexerTokenError(l, T_COMMA);
            }
        }
    }
    if (l->current.id == T_RSQUARE) {
        l->advance(l);
    } else {
        BrLexerTokenError(l, T_RSQUARE);
    }
    return n;
#else
    __asm {
            push ebx
            push ebp
            push esi
            mov esi, dword ptr [esp + 0x10]
            push edi
            cmp dword ptr [esi + 4], 0x5b
            je L0052fd9b
            push 0x5b
            push esi
            call BrLexerTokenError
            add esp, 8
            jmp L0052fd9f
    L0052fd9b:
            push esi
            call dword ptr [esi + 0x18]
    L0052fd9f:
            mov eax, dword ptr [esp + 0x20]
            xor ebp, ebp
            test eax, eax
            jle L0052fde6
            mov ebx, dword ptr [esp + 0x1c]
            mov edi, dword ptr [esp + 0x18]
    L0052fdb1:
            push ebx
            push edi
            push esi
            call BrParseVectorFixed
            mov eax, dword ptr [esi + 4]
            add esp, 0xc
            cmp eax, 0x5d
            je L0052fde6
            cmp eax, 0x2c
            je L0052fdd6
            push 0x2c
            push esi
            call BrLexerTokenError
            add esp, 8
            jmp L0052fdda
    L0052fdd6:
            push esi
            call dword ptr [esi + 0x18]
    L0052fdda:
            mov eax, dword ptr [esp + 0x20]
            inc ebp
            cmp ebp, eax
            lea edi, [edi + ebx*4]
            jl L0052fdb1
    L0052fde6:
            cmp dword ptr [esi + 4], 0x5d
            je L0052fdfe
            push 0x5d
            push esi
            call BrLexerTokenError
            add esp, 8
            mov eax, ebp
            pop edi
            pop esi
            pop ebp
            pop ebx
            ret
    L0052fdfe:
            push esi
            call dword ptr [esi + 0x18]
            pop edi
            mov eax, ebp
            pop esi
            pop ebp
            pop ebx
            ret
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

// FUNCTION: CARMA2_HW 0x0052fe10
br_int_32 C2_NAKED C2_HOOK_CDECL BrParseMatrixFloat(br_lexer* l, br_float* m, br_int_32 width, br_int_32 max_h) {
#ifndef CARPOCALYPSE2_MATCHING
    int n;

    if (l->current.id == T_LSQUARE) {
        l->advance(l);
    } else {
        BrLexerTokenError(l, T_LSQUARE);
    }
    for (n = 0; n < max_h; n++, m = m + width) {
#ifdef BRENDER_FIX_BUGS
        if (l->current.id == T_RSQUARE) {
            break;
        }
#endif
        BrParseVectorFloat(l, m, width);
        if (l->current.id == T_RSQUARE) {
            n++;
            break;
        }
        if (n != max_h - 1) {
            if (l->current.id == T_COMMA) {
                l->advance(l);
            } else {
                BrLexerTokenError(l, T_COMMA);
            }
        }
    }
    if (l->current.id == T_RSQUARE) {
        l->advance(l);
    } else {
        BrLexerTokenError(l, T_RSQUARE);
    }
    return n;
#else
    __asm {
            push ebx
            push ebp
            push esi
            mov esi, dword ptr [esp + 0x10]
            push edi
            cmp dword ptr [esi + 4], 0x5b
            je L0052fe2b
            push 0x5b
            push esi
            call BrLexerTokenError
            add esp, 8
            jmp L0052fe2f
    L0052fe2b:
            push esi
            call dword ptr [esi + 0x18]
    L0052fe2f:
            mov eax, dword ptr [esp + 0x20]
            xor ebp, ebp
            test eax, eax
            jle L0052fe76
            mov ebx, dword ptr [esp + 0x1c]
            mov edi, dword ptr [esp + 0x18]
    L0052fe41:
            push ebx
            push edi
            push esi
            call BrParseVectorFloat
            mov eax, dword ptr [esi + 4]
            add esp, 0xc
            cmp eax, 0x5d
            je L0052fe76
            cmp eax, 0x2c
            je L0052fe66
            push 0x2c
            push esi
            call BrLexerTokenError
            add esp, 8
            jmp L0052fe6a
    L0052fe66:
            push esi
            call dword ptr [esi + 0x18]
    L0052fe6a:
            mov eax, dword ptr [esp + 0x20]
            inc ebp
            cmp ebp, eax
            lea edi, [edi + ebx*4]
            jl L0052fe41
    L0052fe76:
            cmp dword ptr [esi + 4], 0x5d
            je L0052fe8e
            push 0x5d
            push esi
            call BrLexerTokenError
            add esp, 8
            mov eax, ebp
            pop edi
            pop esi
            pop ebp
            pop ebx
            ret
    L0052fe8e:
            push esi
            call dword ptr [esi + 0x18]
            pop edi
            mov eax, ebp
            pop esi
            pop ebp
            pop ebx
            ret
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
