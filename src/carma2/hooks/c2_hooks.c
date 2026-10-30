#include "c2_hooks.h"

#include <stdio.h>
#include <stdlib.h>

// Annotations for CRT/library functions the recompiled binary pulls from its own
// runtime but that exist in the retail binary at a different address. reccmp
// matches these to the recompiled PDB symbols by name, so calls line up in the
// assembly diff.

// LIBRARY: CARMA2_HW 0x00575b80
// __ftol

// LIBRARY: CARMA2_HW 0x00576bca
// __CIfmod

#ifdef _WIN32
#include <windows.h>
#endif

/* porttest: log each distinct NOT_IMPLEMENTED site once, in order of first hit. */
void carpocalypse2_not_implemented(const char *function, const char *file, int line) {
    static struct { const char *function; int line; } seen[1024];
    static int nseen;
    static int seq;
    int i;
    FILE *f;

    for (i = 0; i < nseen; i++) {
        if (seen[i].function == function && seen[i].line == line) {
            return;
        }
    }
    if (nseen < 1024) {
        seen[nseen].function = function;
        seen[nseen].line = line;
        nseen++;
    }
    seq++;
    f = fopen("porttest_notimpl.log", "a");
    if (f != NULL) {
        fprintf(f, "NOT_IMPLEMENTED #%d: %s (%s:%d)\n", seq, function, file, line);
        fclose(f);
    }
    fprintf(stderr, "NOT_IMPLEMENTED #%d: %s (%s:%d)\n", seq, function, file, line);
}

extern C2_NORETURN void carpocalypse2_error(const char *reason, const char *function, const char *file, int line) {
    char buffer[512];

    sprintf(buffer, "!! CRITICAL ERROR !!\n"
        "*   reason = %s\n"
        "* function = %s\n"
        "* location = %s:%d", reason, function, file, line);

    fprintf(stderr, "%s\n", buffer);

#ifdef _WIN32
    ShowCursor(TRUE);
    MessageBoxA(NULL, buffer, "Hook Fatal Error", MB_ICONERROR | MB_OK);
#endif

    abort();
}
