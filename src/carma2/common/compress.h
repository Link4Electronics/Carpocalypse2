#ifndef CARPOCALYPSE2_COMPRESS_H
#define CARPOCALYPSE2_COMPRESS_H

#include "carpocalypse2_types.h"

#include "c2_hooks.h"

tU16 C2_HOOK_FASTCALL DRScalarToU16(float pValue, float pMin, float pMax);

br_scalar C2_HOOK_FASTCALL DRU16ToScalar(tU16 pValue, float pMin, float pMax);

void C2_HOOK_FASTCALL CompressVector3(tCompressed_vector3* pDest, const br_vector3* pSrc, float pMin, float pMax);

void C2_HOOK_FASTCALL ExpandVector3(br_vector3* pDest, const tCompressed_vector3 *pSrc, float pMin, float pMax);

void C2_HOOK_FASTCALL CompressMatrix34(tCompressed_matrix3* pCompressed_matrix3, int* pInactive, const br_matrix34* pMatrix);

void C2_HOOK_FASTCALL ExpandMatrix34(br_matrix34* pMat, const tCompressed_matrix3* pCompressed, int pInactive);

#endif //CARPOCALYPSE2_COMPRESS_H