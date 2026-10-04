#ifndef C2_HOOKS_H
#define C2_HOOKS_H

#if defined(_MSC_VER)
#ifdef CARPOCALYPSE2_MATCHING
#define C2_NORETURN
#else
#define C2_NORETURN __declspec(noreturn)
#endif
#define C2_NORETURN_FUNCPTR
#ifdef CARPOCALYPSE2_MATCHING
#define C2_NAKED __declspec(naked)
#else
/* x64 MSVC rejects __declspec(naked) (error C2485); only matching builds
 * (x86 Visual C++ 5) need it, and their asm bodies sit behind
 * CARPOCALYPSE2_MATCHING anyway. */
#define C2_NAKED
#endif
#define C2_HOOK_NOINLINE __declspec(noinline)
#else
#define C2_NORETURN __attribute__ ((__noreturn__))
#define C2_NORETURN_FUNCPTR C2_NORETURN
// clang rejects a non-asm body in a naked function outright ("non-ASM
// statement in naked function is not supported"), with no flag to downgrade
// it, and every C2_NAKED body a non-MSVC build sees is plain C: the asm
// bodies sit behind CARPOCALYPSE2_MATCHING, which only Visual C++ 5 reaches.
// gcc tolerates a C body there, so it keeps the attribute and clang drops it
// -- the only cost is a normal prologue on functions no non-matching build is
// reproducing byte for byte anyway.
#if defined(__clang__)
#define C2_NAKED
#else
#define C2_NAKED __attribute__((__naked__))
#endif
#define C2_HOOK_NOINLINE __attribute__((__noinline__))
#endif

#if defined(_MSC_VER) && _MSC_VER < 1300
#define C2_FUNCTION "<unknown>"
#else
#define C2_FUNCTION __FUNCTION__
#endif
extern C2_NORETURN void carpocalypse2_error(const char *reason, const char *function, const char *file, int line);
#ifdef CARPOCALYPSE2_MATCHING
#define NOT_IMPLEMENTED() carpocalypse2_error("Not implemented", C2_FUNCTION, __FILE__, __LINE__)
#else
/* Non-matching builds: stubs become no-ops so boot proceeds past unimplemented code. */
#define NOT_IMPLEMENTED() do { } while (0)
#endif
#define UNUSED() carpocalypse2_error("Unused", C2_FUNCTION, __FILE__, __LINE__)

#ifdef CARPOCALYPSE2_MATCHING

#define C2_HOOK_CDECL __cdecl
#define C2_HOOK_FASTCALL __fastcall
#define C2_HOOK_STDCALL __stdcall
#define C2_HOOK_THISCALL __thiscall
#define C2_HOOK_FAKE_THISCALL __fastcall

// FIXME: rewrite to C2_HOOK_STATIC_ASSERT
#define C2_HOOK_BUG_ON(condition) ((void)sizeof(char[1 - 2*!!(condition)]))
#define C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(T, M, O) C2_HOOK_BUG_ON(((uintptr_t)&(((T*)0)->M)) != O)
#define C2_HOOK_STATIC_ASSERT_STRUCT_MEMBER_SIZE(T, M, S) C2_HOOK_BUG_ON(sizeof(((T*)0)->M) != (S))

#define C2_HOOK_ASSERT(condition)
#else

#define C2_HOOK_CDECL
#define C2_HOOK_FASTCALL
#define C2_HOOK_STDCALL
#define C2_HOOK_THISCALL
#define C2_HOOK_FAKE_THISCALL

#define C2_HOOK_ASSERT(condition)
#define C2_HOOK_BUG_ON(condition)
#define C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(T, M, O)
#define C2_HOOK_STATIC_ASSERT_STRUCT_MEMBER_SIZE(T, M, O)
#endif

typedef unsigned char undefined;
typedef unsigned short undefined2;
typedef unsigned int undefined4;

#endif // C2_HOOKS_H
