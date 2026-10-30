/* C11 <stdalign.h> for Windows SDKs that lack it (MSVC supports the keywords in /std:c11). */
#ifndef C2PORT_STDALIGN_H
#define C2PORT_STDALIGN_H
#ifndef alignas
#define alignas _Alignas
#endif
#ifndef alignof
#define alignof _Alignof
#endif
#define __alignas_is_defined 1
#define __alignof_is_defined 1
#endif

