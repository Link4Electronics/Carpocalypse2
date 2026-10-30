#ifndef GUARD_OPPOPROC_H
#define GUARD_OPPOPROC_H

#include "c2_hooks.h"
#include "carpocalypse2_types.h"
#include <brender/brender.h>

// DoNotDprintf

// StraightestArcForCorner2D

// CornerFudge

// MaxCurvatureForCarSpeed

// Vector2Cross

// RoughlyColinear

// GetStraight

// TimeToStopStruggling

// TimeToBeBrakingWhenStruggling

// CalcCorners

// Distance2D

// SectionLength2D

// NearestSectionInStraight

// SetMaxSpeedFromSOCs

// CalcSOCs

// StuffDataFromCorner

// SetCurvature

// ProcessFollowPath

// FollowCheatyPath

#define CARPOCALYPSE2_OPPOPROC_H

#include <brender/brender.h>
#include "carpocalypse2_types.h"

#include "c2_hooks.h"

typedef struct tDial_flic_option {
    int count;
    int y;
    int selected;
    int flic_frames[5];
} tDial_flic_option;
double C2_HOOK_CDECL NavigationFloatDifference(const float* pA, const float* pB);
double C2_HOOK_CDECL NavigationAdd(double pA, double pB);
double C2_HOOK_CDECL NavigationSubtract(double pA, double pB);
double C2_HOOK_CDECL NavigationMultiply(double pA, double pB);
double C2_HOOK_CDECL NavigationDivide(double pA, double pB);
double C2_HOOK_CDECL NavigationSqrt(double pA);
float C2_HOOK_CDECL NavigationScalarRound(double pA);
int C2_HOOK_CDECL NavigationOrderedGreater(double pA, double pB);
int C2_HOOK_CDECL NavigationLessOrUnordered(double pA, double pB);
int C2_HOOK_CDECL NavigationTruncate(double pValue);
double C2_HOOK_CDECL NavigationCornerSectionLength(tOpponent_spec* pOpponent_spec, tCorner* pCorner, int pSection);



extern tDial_flic_option gDial_flic_options[13];

void C2_HOOK_CDECL DoNotDprintf_oppoproc(const char* pMessage, ...);

br_scalar C2_HOOK_FASTCALL Vector2Cross(const br_vector2* pV1, const br_vector2* pV2);

tFollow_path_result FollowCheatyPath(tOpponent_spec* pOpponent_spec);

int C2_HOOK_FASTCALL GetStraight(br_vector2* pStart, br_vector2* pFinish, float* pWidth, int pSection, tOpponent_spec* pOpponent_spec);

int C2_HOOK_FASTCALL CalcCorners(tCorner* pCorners, int pSection, float pWidth, const br_vector2* pStart, tOpponent_spec* pOpponent_spec);

int C2_HOOK_FASTCALL CalcSOCs(int pNext_section, int pCount_corners, tCorner *pCorners, tOpponent_spec *pOpponent_spec, tSOC* pSocs, tCar_spec* pCar);

int C2_HOOK_FASTCALL FUN_004b24f0(int* pDial_index);
int C2_HOOK_FASTCALL FUN_004b2430(int* pDial_index);

int C2_HOOK_FAKE_THISCALL FUN_004b2640(int* pDial_index, undefined4 pArg2, int pX_offset, undefined4 pArg3);

int C2_HOOK_FASTCALL FUN_004b25c0(int* pArg1, int* pArg2);

#endif // GUARD_OPPOPROC_H
