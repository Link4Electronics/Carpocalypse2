#ifndef GUARD_AIWORLD_H
#define GUARD_AIWORLD_H

#include "c2_hooks.h"
#include "carpocalypse2_types.h"

// MakeVertexAndOffsetIt

extern void C2_HOOK_FASTCALL MakeFaceAndTextureIt(br_model* pModel, int pFace_num, br_uint_16 pV0, br_uint_16 pV1, br_uint_16 pV2, br_material* pMaterial);

extern br_material* C2_HOOK_FASTCALL CreateSimpleMaterial(int pIndex);

extern void C2_HOOK_FASTCALL InitPanGameAIWorld(void);

// LoadAIWorldTrackInfo

// LoadAIWorldForRace

// InitialiseAIWorldForRace

extern void C2_HOOK_FASTCALL DisposeAIWorldRaceStuff(void);

// MungeAIWorld

// PointActorAlongThisBloodyVector

// MakeAIEditCubeHere

// CalcNegativeXVector

// MakeAISimpleEditSectionHere

#endif // GUARD_AIWORLD_H
