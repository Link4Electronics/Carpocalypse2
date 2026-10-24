#ifndef __IMAGECOOMPRESSION__
#define __IMAGECOOMPRESSION__

#include "c2_hooks.h"
#include "MacTypes.h"

typedef long                            ImageSequence;

extern OSErr C2_HOOK_CDECL CDSequenceEnd(ImageSequence seqID);

#endif /* __IMAGECOOMPRESSION__ */
