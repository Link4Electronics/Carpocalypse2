#ifndef GUARD_OIL_H
#define GUARD_OIL_H

#include "carpocalypse2_types.h"


#include "c2_hooks.h"

extern void C2_HOOK_FASTCALL InitOilSpills(void);

// ResetOilSpills

void C2_HOOK_FASTCALL QueueOilSpill2(
    tCar_spec* pCar,
    br_actor* pActor,
    br_model* pModel,
    int pSpill_time_offset,
    float pFull_size_min,
    float pFull_size_max,
    float pGrow_size_min,
    float pGrow_size_max,
    float pSize,
    float pField_0x48,
    float pField_0x4c,
    float pField_0x40,
    float pField_0x44,
    br_material* pMaterial,
    int pField_0x50);

void C2_HOOK_FASTCALL QueueOilSpill(tCar_spec* pCar);

// OKToSpillOil

// MungeOilsHeightAboveGround

// MungeIndexedOilsHeightAboveGround

// SetInitialOilStuff

// ProcessOilSpills

// GetOilSpillCount

// GetOilSpillDetails

// PointInSpill

// GetOilFrictionFactors

extern void C2_HOOK_FASTCALL AdjustOilSpill(int pIndex, br_matrix34* pMat, br_scalar pFull_size, br_scalar pGrow_rate, tU32 pSpill_time, tU32 pStop_time, tCar_spec* pCar, br_vector3* pOriginal_pos, br_pixelmap* pPixelmap);

extern void C2_HOOK_FASTCALL ReceivedOilSpill(tNet_message_chunk* pContents);

extern void C2_HOOK_FASTCALL RemoveCarsOilSlicks(tCar_spec* pCar);

#define CARPOCALYPSE2_OIL_H

#include "carpocalypse2_types.h"

#include "c2_hooks.h"


extern int gNext_oil_pixie;

void C2_HOOK_FASTCALL ResetOilSpills(void);

void C2_HOOK_FASTCALL SetInitialOilStuff(tOil_spill_info* pOil, br_model* pModel);

void C2_HOOK_FASTCALL MungeOilsHeightAeGround(tOil_spill_info* pThe_spill);

int C2_HOOK_FASTCALL OKToSpillOil(tOil_spill_info* pOil);

void C2_HOOK_FASTCALL MungeOilsHeightAboveGround(tOil_spill_info* pOil);

void C2_HOOK_FASTCALL ProcessOilSpills(tU32 pFrame_period);

int C2_HOOK_FASTCALL GetOilSpillCount(void);

void C2_HOOK_FASTCALL GetOilSpillDetails(int pIndex, br_actor** pActor, br_scalar* pSize);

void C2_HOOK_FASTCALL GetOilFrictionFactors(tCar_spec* pCar, br_scalar* pFl, br_scalar* pFr, br_scalar* pRl, br_scalar* pRr);

int C2_HOOK_FASTCALL PointInSpill(br_vector3* pPoint, int pIndex);

void C2_HOOK_FASTCALL MungeIndexedOilsHeightAboveGround(int pIndex);

#endif // GUARD_OIL_H
