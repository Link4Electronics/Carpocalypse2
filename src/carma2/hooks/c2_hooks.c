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
