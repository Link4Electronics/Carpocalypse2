#ifndef __FILES___
#define __FILES___

#include "MacTypes.h"

#ifdef _MSC_VER
#pragma pack(push, 2)
#endif
typedef struct FSSpec {
    short               vRefNum;
    long                parID;
    StrFileName         name;
}
#if defined(__GNUC__) && !defined(_MSC_VER)
__attribute__((packed))
#endif
FSSpec;
#ifdef _MSC_VER
#pragma pack(pop)
#endif

#endif /* __FILES___ */
