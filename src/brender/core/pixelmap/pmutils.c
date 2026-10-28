#include "pmutils.h"

#if defined(__I86__) || defined(_M_IX86)
C2_NAKED
#endif
// FUNCTION: CARMA2_HW 0x0053e682
br_uint_16 C2_HOOK_CDECL GetSysQual(void) {
#if defined(__I86__) || defined(_M_IX86)
#if defined(_MSC_VER)
    __asm {
        mov ax, ds;
        ret 0;
    }
#else
    br_uint_16 result;

    __asm__("mov %%ds, %0" : "=a"(result));
    return result;
#endif
#else
    return 0;
#endif
}
