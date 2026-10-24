#include "QTML.h"

#include "Files.h"
#include "MacTypes.h"

#include "c2_hooks.h"

#include <stdlib.h>

typedef struct MediaType** Media;
typedef struct MovieType** Movie;
typedef struct TrackType** Track;
typedef long ImageSequence;

enum {
    flattenAddMovieToDataFork = 1L << 0,
};
enum {
    createMovieFileDeleteCurFile = 1L << 31,
};

typedef void (C2_HOOK_CDECL *tQuickTime_trap_cbfn)(void);

C2_NAKED static void C2_HOOK_CDECL QuickTime_default_dispatcher(void) {
#ifdef CARPOCALYPSE2_MATCHING
    __asm {
        mov eax, 1
        ret
    }
#endif
}

// GLOBAL: CARMA2_HW 0x006736a8
tQuickTime_trap_cbfn gQuickTime_trap_callback = NULL;

// GLOBAL: CARMA2_HW 0x006736b4
tQuickTime_trap_cbfn gQuickTime_trap_dispatcher = QuickTime_default_dispatcher;

// STUB: CARMA2_HW 0x00575950
OSErr C2_HOOK_CDECL InitializeQTML(long flag) {
    (void)flag;
    return (OSErr)0xf7d3;
}

// STUB: CARMA2_HW 0x00575340
C2_NAKED StringPtr C2_HOOK_CDECL c2pstr(char *aStr) {
#ifdef CARPOCALYPSE2_MATCHING
    __asm {
        mov eax, dword ptr [gQuickTime_trap_callback]
        or eax, eax
        je skip
        call dword ptr [gQuickTime_trap_callback]
skip:
        mov eax, 0x280017
        jmp dword ptr [gQuickTime_trap_dispatcher]
    }
#else
    (void)aStr;
    return NULL;
#endif
}

// STUB: CARMA2_HW 0x00570ac0
C2_NAKED void C2_HOOK_CDECL DisposeHandle(Handle h) {
#ifdef CARPOCALYPSE2_MATCHING
    __asm {
        mov eax, dword ptr [gQuickTime_trap_callback]
        or eax, eax
        je skip
        call dword ptr [gQuickTime_trap_callback]
skip:
        mov eax, 0x150032
        jmp dword ptr [gQuickTime_trap_dispatcher]
    }
#else
    (void)h;
#endif
}

// STUB: CARMA2_HW 0x0056a8e0
C2_NAKED OSErr C2_HOOK_CDECL EnterMovies(void) {
#ifdef CARPOCALYPSE2_MATCHING
    __asm {
        mov eax, dword ptr [gQuickTime_trap_callback]
        or eax, eax
        je skip
        call dword ptr [gQuickTime_trap_callback]
skip:
        mov eax, 0x20001
        jmp dword ptr [gQuickTime_trap_dispatcher]
    }
#else
    return 0x1;
#endif
}

// STUB: CARMA2_HW 0x0056baa0
C2_NAKED OSErr C2_HOOK_CDECL CloseMovieFile(short resRefNum) {
#ifdef CARPOCALYPSE2_MATCHING
    __asm {
        mov eax, dword ptr [gQuickTime_trap_callback]
        or eax, eax
        je skip
        call dword ptr [gQuickTime_trap_callback]
skip:
        mov eax, 0x200d5
        jmp dword ptr [gQuickTime_trap_dispatcher]
    }
#else
    (void)resRefNum;
    return 0x1;
#endif
}

// STUB: CARMA2_HW 0x0056abe0
C2_NAKED void C2_HOOK_CDECL DisposeMovie(Movie theMovie) {
#ifdef CARPOCALYPSE2_MATCHING
    __asm {
        mov eax, dword ptr [gQuickTime_trap_callback]
        or eax, eax
        je skip
        call dword ptr [gQuickTime_trap_callback]
skip:
        mov eax, 0x20023
        jmp dword ptr [gQuickTime_trap_dispatcher]
    }
#else
    (void)theMovie;
#endif
}

// STUB: CARMA2_HW 0x0056b360
C2_NAKED OSErr C2_HOOK_CDECL EndMediaEdits(Media theMedia) {
#ifdef CARPOCALYPSE2_MATCHING
    __asm {
        mov eax, dword ptr [gQuickTime_trap_callback]
        or eax, eax
        je skip
        call dword ptr [gQuickTime_trap_callback]
skip:
        mov eax, 0x20073
        jmp dword ptr [gQuickTime_trap_dispatcher]
    }
#else
    (void)theMedia;
    return 1;
#endif
}

// STUB: CARMA2_HW 0x0056b240
C2_NAKED TimeValue C2_HOOK_CDECL GetMediaDuration(Media theMedia) {
#ifdef CARPOCALYPSE2_MATCHING
    __asm {
        mov eax, dword ptr [gQuickTime_trap_callback]
        or eax, eax
        je skip
        call dword ptr [gQuickTime_trap_callback]
skip:
        mov eax, 0x2006a
        jmp dword ptr [gQuickTime_trap_dispatcher]
    }
#else
    (void)theMedia;
    return 0;
#endif
}

// STUB: CARMA2_HW 0x0056dac0
C2_NAKED OSErr C2_HOOK_CDECL CDSequenceEnd(ImageSequence seqID) {
#ifdef CARPOCALYPSE2_MATCHING
    __asm {
        mov eax, dword ptr [gQuickTime_trap_callback]
        or eax, eax
        je skip
        call dword ptr [gQuickTime_trap_callback]
skip:
        mov eax, 0x3001b
        jmp dword ptr [gQuickTime_trap_dispatcher]
    }
#else
    (void)seqID;
    return 1;
#endif
}

// STUB: CARMA2_HW 0x0056c600
C2_NAKED OSErr C2_HOOK_CDECL InsertMediaIntoTrack(Track theTrack, TimeValue trackStart, TimeValue mediaTime, TimeValue mediaDuration, Fixed mediaRate) {
#ifdef CARPOCALYPSE2_MATCHING
    __asm {
        mov eax, dword ptr [gQuickTime_trap_callback]
        or eax, eax
        je skip
        call dword ptr [gQuickTime_trap_callback]
skip:
        mov eax, 0x20183
        jmp dword ptr [gQuickTime_trap_dispatcher]
    }
#else
    (void)theTrack;
    (void)trackStart;
    (void)mediaTime;
    (void)mediaDuration;
    (void)mediaRate;
    return 1;
#endif
}

// STUB: CARMA2_HW 0x0056c800
C2_NAKED Movie C2_HOOK_CDECL FlattenMovieData(Movie theMovie, long movieFlattenFlags, const FSSpec *theFile, OSType creator, ScriptCode scriptTag, long createMovieFileFlags) {
#ifdef CARPOCALYPSE2_MATCHING
    __asm {
        mov eax, dword ptr [gQuickTime_trap_callback]
        or eax, eax
        je skip
        call dword ptr [gQuickTime_trap_callback]
skip:
        mov eax, 0x2019c
        jmp dword ptr [gQuickTime_trap_dispatcher]
    }
#else
    (void)theMovie;
    (void)movieFlattenFlags;
    (void)theFile;
    (void)creator;
    (void)scriptTag;
    (void)createMovieFileFlags;
    return NULL;
#endif
}

// STUB: CARMA2_HW 0x0056c720
C2_NAKED OSErr C2_HOOK_CDECL CreateMovieFile(const FSSpec* fileSpec, OSType creator, ScriptCode scriptTag, long createMovieFileFlags, short* resRefNum, Movie* newmovie) {
#ifdef CARPOCALYPSE2_MATCHING
    __asm {
        mov eax, dword ptr [gQuickTime_trap_callback]
        or eax, eax
        je skip
        call dword ptr [gQuickTime_trap_callback]
skip:
        mov eax, 0x20191
        jmp dword ptr [gQuickTime_trap_dispatcher]
    }
#else
    (void)fileSpec;
    (void)creator;
    (void)scriptTag;
    (void)createMovieFileFlags;
    (void)resRefNum;
    (void)newmovie;
    return 1;
#endif
}
