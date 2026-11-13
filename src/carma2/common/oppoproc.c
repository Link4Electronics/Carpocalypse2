#include "oppoproc.h"

#include <math.h>
#include <string.h>

#include "loading.h"
#include "opponent.h"
#include "platform.h"
#include "sound.h"
#include "flicplay.h"
#include "grafdata.h"
#include "displays.h"
#include "graphics2.h"
#include "globvars.h"
#include "utility.h"

#include "carpocalypse2_macros.h"
#include "oppoproc.h"

// GLOBAL: CARMA2_HW 0x00691790
br_pixelmap* gDials_pix;

// GLOBAL: CARMA2_HW 0x0058a61c
const br_scalar gFLOAT_0058a61c = 0.0f;

// GLOBAL: CARMA2_HW 0x0058a630
const double gDOUBLE_0058a630 = 1.0;

// GLOBAL: CARMA2_HW 0x0058a638
const br_scalar gFLOAT_0058a638 = 1.0f;

// GLOBAL: CARMA2_HW 0x0058a644
const br_scalar gFLOAT_0058a644 = -1.0f;

// GLOBAL: CARMA2_HW 0x0058a64c
const br_scalar gFLOAT_0058a64c = 0.1f;

// GLOBAL: CARMA2_HW 0x0058a670
const br_scalar gFLOAT_0058a670 = 2.384185791015625e-7f;

// GLOBAL: CARMA2_HW 0x0058a688
const br_scalar gFLOAT_0058a688 = 0.001f;

// GLOBAL: CARMA2_HW 0x0058a68c
const br_scalar gFLOAT_0058a68c = -2.0f;

// GLOBAL: CARMA2_HW 0x0058a690
const br_scalar gFLOAT_0058a690 = 9.0f;

// GLOBAL: CARMA2_HW 0x0058a694
const br_scalar gFLOAT_0058a694 = 0.3333333432674408f;

// GLOBAL: CARMA2_HW 0x0058a698
const br_scalar gFLOAT_0058a698 = 0.7f;

double C2_HOOK_CDECL NavigationFloatDifference(const float* pA, const float* pB) {
    double result;
#if defined(_MSC_VER) && defined(_M_IX86)
    __asm {
        mov eax, pA
        mov edx, pB
        fld dword ptr [eax]
        fsub dword ptr [edx]
        fstp qword ptr [result]
    }
#elif defined(__GNUC__) && defined(__i386__)
    __asm__ volatile ("flds %1; fsubs %2; fstpl %0" : "=m"(result) : "m"(*pA), "m"(*pB) : "st");
#else
    result = (double)*pA - (double)*pB;
#endif
    return result;
}

double C2_HOOK_CDECL NavigationAdd(double pA, double pB) {
    double result;
#if defined(_MSC_VER) && defined(_M_IX86)
    __asm {
        fld qword ptr [pA]
        fadd qword ptr [pB]
        fstp qword ptr [result]
    }
#elif defined(__GNUC__) && defined(__i386__)
    __asm__ volatile ("fldl %1; faddl %2; fstpl %0" : "=m"(result) : "m"(pA), "m"(pB) : "st");
#else
    result = pA + pB;
#endif
    return result;
}

double C2_HOOK_CDECL NavigationSubtract(double pA, double pB) {
    double result;
#if defined(_MSC_VER) && defined(_M_IX86)
    __asm {
        fld qword ptr [pA]
        fsub qword ptr [pB]
        fstp qword ptr [result]
    }
#elif defined(__GNUC__) && defined(__i386__)
    __asm__ volatile ("fldl %1; fsubl %2; fstpl %0" : "=m"(result) : "m"(pA), "m"(pB) : "st");
#else
    result = pA - pB;
#endif
    return result;
}

double C2_HOOK_CDECL NavigationMultiply(double pA, double pB) {
    double result;
#if defined(_MSC_VER) && defined(_M_IX86)
    __asm {
        fld qword ptr [pA]
        fmul qword ptr [pB]
        fstp qword ptr [result]
    }
#elif defined(__GNUC__) && defined(__i386__)
    __asm__ volatile ("fldl %1; fmull %2; fstpl %0" : "=m"(result) : "m"(pA), "m"(pB) : "st");
#else
    result = pA * pB;
#endif
    return result;
}

double C2_HOOK_CDECL NavigationDivide(double pA, double pB) {
    double result;
#if defined(_MSC_VER) && defined(_M_IX86)
    __asm {
        fld qword ptr [pA]
        fdiv qword ptr [pB]
        fstp qword ptr [result]
    }
#elif defined(__GNUC__) && defined(__i386__)
    __asm__ volatile ("fldl %1; fdivl %2; fstpl %0" : "=m"(result) : "m"(pA), "m"(pB) : "st");
#else
    result = pA / pB;
#endif
    return result;
}

double C2_HOOK_CDECL NavigationSqrt(double pA) {
    double result;
#if defined(_MSC_VER) && defined(_M_IX86)
    __asm {
        fld qword ptr [pA]
        fsqrt
        fstp qword ptr [result]
    }
#elif defined(__GNUC__) && defined(__i386__)
    __asm__ volatile ("fldl %1; fsqrt; fstpl %0" : "=m"(result) : "m"(pA) : "st");
#else
    result = sqrt(pA);
#endif
    return result;
}

float C2_HOOK_CDECL NavigationScalarRound(double pA) {
    volatile union { float scalar; tU32 bits; } stored;
    union { float scalar; tU32 bits; } result;

    stored.scalar = (float)pA;
    result.bits = stored.bits;
    return result.scalar;
}

int C2_HOOK_CDECL NavigationOrderedGreater(double pA, double pB) {
    unsigned short status;
#if defined(_MSC_VER) && defined(_M_IX86)
    __asm {
        fld qword ptr [pA]
        fcomp qword ptr [pB]
        fnstsw status
    }
#elif defined(__GNUC__) && defined(__i386__)
    __asm__ volatile ("fldl %1; fcompl %2; fnstsw %0" : "=m"(status) : "m"(pA), "m"(pB) : "st");
#else
    return pA > pB;
#endif
    return (status & 0x4100) == 0;
}

int C2_HOOK_CDECL NavigationLessOrUnordered(double pA, double pB) {
    unsigned short status;
#if defined(_MSC_VER) && defined(_M_IX86)
    __asm {
        fld qword ptr [pA]
        fcomp qword ptr [pB]
        fnstsw status
    }
#elif defined(__GNUC__) && defined(__i386__)
    __asm__ volatile ("fldl %1; fcompl %2; fnstsw %0" : "=m"(status) : "m"(pA), "m"(pB) : "st");
#else
    return pA < pB || pA != pA || pB != pB;
#endif
    return (status & 0x100) != 0;
}

int C2_HOOK_CDECL NavigationTruncate(double pValue) {
    br_int_64 result;
#if defined(_MSC_VER) && defined(_M_IX86)
    unsigned short control;
    unsigned short truncate_control;
    __asm { fnstcw control }
    truncate_control = control | 0xc00;
    __asm {
        fldcw truncate_control
        fld qword ptr [pValue]
        fistp qword ptr [result]
        fldcw control
    }
#elif defined(__GNUC__) && defined(__i386__)
    unsigned short control;
    unsigned short truncate_control;
    __asm__ volatile ("fnstcw %0" : "=m"(control));
    truncate_control = control | 0xc00;
    __asm__ volatile ("fldcw %1; fldl %2; fistpll %0; fldcw %3" : "=m"(result) : "m"(truncate_control), "m"(pValue), "m"(control) : "st");
#else
    result = (br_int_64)pValue;
#endif
    return (int)(tU32)result;
}

double C2_HOOK_CDECL NavigationCornerSectionLength(tOpponent_spec* pOpponent_spec, tCorner* pCorner, int pSection) {
    const br_vector3* start = GetOpponentsSectionStartNodePoint(pOpponent_spec, (tS16)pCorner->section);
    const br_vector3* finish = GetOpponentsSectionFinishNodePoint(pOpponent_spec, (tS16)pSection);
    double x = NavigationFloatDifference(&finish->v[0], &start->v[0]);
    double z = NavigationFloatDifference(&finish->v[2], &start->v[2]);
    return NavigationSqrt(NavigationAdd(NavigationMultiply(x, NavigationScalarRound(x)), NavigationMultiply(z, z)));
}

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
#ifdef CARPOCALYPSE2_MATCHING
#endif
#ifdef CARPOCALYPSE2_MATCHING
extern int C2_HOOK_FASTCALL GetOpponentsNextSection(const tOpponent_spec* pOpponent_spec, tS16 pCurrent_section);
extern const br_vector3* C2_HOOK_FASTCALL GetOpponentsSectionFinishNodePoint(const tOpponent_spec* pOpponent_spec, tS16 pSection);
extern const br_vector3* C2_HOOK_FASTCALL GetOpponentsSectionStartNodePoint(const tOpponent_spec* pOpponent_spec, tS16 pSection);
extern float C2_HOOK_FASTCALL GetOpponentsSectionWidth(const tOpponent_spec* pOpponent_spec, tS16 pSection);
extern int C2_HOOK_FASTCALL IRandomBetween(int pA, int pB);
#endif
#ifdef CARPOCALYPSE2_MATCHING
extern void C2_HOOK_FASTCALL DRPixelmapRectangleMaskedCopy(br_pixelmap* pDest, br_int_16 pDest_x, br_int_16 pDest_y, const br_pixelmap* pSource, br_int_16 pSource_x, br_int_16 pSource_y, br_int_16 pWidth, br_int_16 pHeight);
extern tS3_sound_tag C2_HOOK_FASTCALL DRS3StartSound(tS3_outlet* pOutlet, tS3_sound_id pSound);
extern int C2_HOOK_FASTCALL DoMouseCursor(void);
extern void C2_HOOK_FASTCALL DontLetFlicFuckWithPalettes(void);
extern int C2_HOOK_FASTCALL GetOpponentsSectionMaxSpeed(tOpponent_spec* pOpponent_spec, tS16 pSection, int pTowards_finish);
extern void C2_HOOK_FASTCALL LetFlicFuckWithPalettes(void);
extern tU32 C2_HOOK_FASTCALL PDGetTotalTime(void);
extern void C2_HOOK_FASTCALL PDScreenBufferSwap(int pRendering_area_only);
extern void C2_HOOK_FASTCALL ProcessFlicQueue(tU32 pInterval);
extern void C2_HOOK_FASTCALL RemoveTransientBitmaps(int pGraphically_remove_them);
extern void C2_HOOK_FASTCALL RunFlicAt(int pIndex, int pX, int pY);
extern void C2_HOOK_FASTCALL SetSoundVolumes(int pCD_audio);
extern void C2_HOOK_FASTCALL TurnFlicTransparencyOff(void);
extern void C2_HOOK_FASTCALL TurnFlicTransparencyOn(void);
extern void __cdecl _ftol();
extern br_pixelmap* gBack_screen;
extern tGraf_data* gCurrent_graf_data;
extern tS3_outlet* gEffects_outlet;
extern char gFLOAT_0058a620[];
extern char gFLOAT_0058a6a4[];
extern tProgram_state gProgram_state;
extern int gReal_graf_data_index;
extern char gUNK_0065bd80[];
extern char gUNK_0065bee0[];
#endif
// FUNCTION: CARMA2_HW 0x004b13c0
void C2_HOOK_CDECL DoNotDprintf_oppoproc(const char* pMessage, ...) {
}

br_scalar C2_HOOK_FASTCALL Vector2Cross(const br_vector2* pV1, const br_vector2* pV2) {

    return pV1->v[0] * pV2->v[1] - pV1->v[1] * pV2->v[0];
}

tFollow_path_result FollowCheatyPath(tOpponent_spec* pOpponent_spec) {
    tFollow_path_data* data;
    br_vector3 a;
    br_vector3 p;
    br_vector3 section_v;
    br_vector3 car_to_end;
    br_vector3 car_to_intersect;
    const br_vector3* start;
    const br_vector3* finish;
    br_scalar t;
    br_scalar frame_period_in_secs;
    br_scalar distance_to_end;
    br_scalar distance_to_intersect;
    br_scalar desired_speed_BRU;

    data = &pOpponent_spec->follow_path_data;
    start = GetOpponentsSectionStartNodePoint(pOpponent_spec, data->section_no);
    finish = GetOpponentsSectionFinishNodePoint(pOpponent_spec, data->section_no);
    if (pOpponent_spec->follow_path_data.cheating != pOpponent_spec->cheating) {
        data->cheating = pOpponent_spec->cheating;
        if (data->cheating) {
            DoNotDprintf_oppoproc("%s: Dematerialising", pOpponent_spec->car_spec->driver_name);
            BrVector3Sub(&section_v, finish, start);
            BrVector3Sub(&p, &pOpponent_spec->car_spec->car_master_actor->t.t.translate.t, start);
            t = BrVector3Dot(&section_v, &p) / BrVector3LengthSquared(&section_v);
            if (t < 0.0f) {
                BrVector3Copy(&data->cheaty_intersect, start);
            } else if (t > 1.f) {
                BrVector3Copy(&data->cheaty_intersect, finish);
            } else {
                BrVector3Scale(&data->cheaty_intersect, &section_v, t);
                BrVector3Accumulate(&data->cheaty_intersect, start);
            }
            BrVector3Sub(&car_to_intersect, &data->cheaty_intersect,
                &pOpponent_spec->car_spec->car_master_actor->t.t.translate.t);
            distance_to_intersect = BrVector3Length(&car_to_intersect);
            frame_period_in_secs = gFrame_period_for_this_munging / 1000.0f * 6.0f;
            TurnOpponentPhysicsOff(pOpponent_spec);
            if (distance_to_intersect >= frame_period_in_secs) {
                data->moving_to_intersect = 1;
                BrVector3Normalise(&car_to_intersect, &car_to_intersect);
                BrVector3Scale(&car_to_intersect, &car_to_intersect, frame_period_in_secs);
                BrVector3Accumulate(&pOpponent_spec->car_spec->car_master_actor->t.t.translate.t, &car_to_intersect);
                return eFPR_OK;
            } else {
                data->moving_to_intersect = 0;
                BrVector3Copy(&pOpponent_spec->car_spec->car_master_actor->t.t.translate.t, &data->cheaty_intersect);
                return eFPR_OK;
            }
        }
        BrVector3Sub(&car_to_end, finish, &pOpponent_spec->car_spec->car_master_actor->t.t.translate.t);
        distance_to_end = BrVector3Length(&car_to_end);
        DoNotDprintf_oppoproc("%s: Rematerialising from FollowCheatyPath()...", pOpponent_spec->car_spec->driver_name);
        if (pOpponent_spec->follow_path_data.section_no < 0) {
            PDFatalError("No useful section number in ProcessCheatyPath()");
        }
        if (GetOpponentsSectionMaxSpeed(pOpponent_spec, pOpponent_spec->follow_path_data.section_no, 1) < 255) {
            desired_speed_BRU = GetOpponentsSectionMaxSpeed(pOpponent_spec, pOpponent_spec->follow_path_data.section_no, 1) / WORLD_SCALE;
        } else if (GetOpponentsSectionMinSpeed(pOpponent_spec, pOpponent_spec->follow_path_data.section_no, 1) > 0) {
            desired_speed_BRU = GetOpponentsSectionMinSpeed(pOpponent_spec, pOpponent_spec->follow_path_data.section_no, 1) / WORLD_SCALE;
        } else {
            desired_speed_BRU = MIN(7.0f, MAX(1.0f, 2.f * distance_to_end));
        }

        if (RematerialiseOpponentOnThisSection(pOpponent_spec, data->section_no, desired_speed_BRU)) {
            pOpponent_spec->car_spec->brake_force = 0.f;
            pOpponent_spec->car_spec->acc_force = 0.f;
            return eFPR_OK;
        }
        data->made_it = 1;
    }
    frame_period_in_secs = gFrame_period_for_this_munging / 1000.0f * 6.0f;
    BrVector3Set(&pOpponent_spec->car_spec->collision_info->v, 0.f, 41.4f, 0.f);

    if (data->moving_to_intersect) {
        BrVector3Sub(&car_to_intersect, &data->cheaty_intersect, &pOpponent_spec->car_spec->car_master_actor->t.t.translate.t);
        distance_to_intersect = BrVector3Length(&car_to_intersect);
        if (distance_to_intersect < frame_period_in_secs) {
            data->moving_to_intersect = 0;
            BrVector3Copy(&pOpponent_spec->car_spec->car_master_actor->t.t.translate.t, &data->cheaty_intersect);
            BrVector3Sub(&car_to_end, finish, start);
        } else {
            BrVector3Normalise(&car_to_intersect, &car_to_intersect);
            BrVector3Scale(&car_to_intersect, &car_to_intersect, frame_period_in_secs);
            BrVector3Accumulate(&pOpponent_spec->car_spec->car_master_actor->t.t.translate.t, &car_to_intersect);
        }
        PointActorAlongThisBloodyVector(pOpponent_spec->car_spec->car_master_actor, &car_to_end);
    } else {
        while (frame_period_in_secs > 0.0f) {
            BrVector3Sub(&a, finish, &pOpponent_spec->car_spec->car_master_actor->t.t.translate.t);
            distance_to_end = BrVector3Length(&a);
            if (distance_to_end < frame_period_in_secs) {
                BrVector3Copy(&pOpponent_spec->car_spec->car_master_actor->t.t.translate.t, finish);
                DoNotDprintf_oppoproc("%s: ProcessFollowPath() - current section %d(#%d) (cheating)", pOpponent_spec->car_spec->driver_name, data->section_no, GetOpponentsRealSection(pOpponent_spec, data->section_no));
                data->section_no = GetOpponentsNextSection(pOpponent_spec, data->section_no);
                if (data->section_no == -1) {
                    pOpponent_spec->car_spec->acc_force = 0.f;
                    pOpponent_spec->car_spec->brake_force = 0.f;
                    DoNotDprintf_oppoproc("%s: ProcessFollowPath() - reached end of path while cheating", pOpponent_spec->car_spec->driver_name);
                    return eFPR_end_of_path;
                }
                DoNotDprintf_oppoproc("%s: ProcessFollowPath() - next section %d(#%d) (cheating)", pOpponent_spec->car_spec->driver_name, data->section_no, GetOpponentsRealSection(pOpponent_spec, data->section_no));
                start = GetOpponentsSectionStartNodePoint(pOpponent_spec, data->section_no);
                finish = GetOpponentsSectionFinishNodePoint(pOpponent_spec, data->section_no);
                frame_period_in_secs -= distance_to_end;
            } else {
                BrVector3Scale(&p, &a, frame_period_in_secs / distance_to_end);
                BrVector3Accumulate(&pOpponent_spec->car_spec->car_master_actor->t.t.translate.t, &p);
                frame_period_in_secs = 0.0;
            }
        }
        BrVector3Sub(&car_to_end, finish, &pOpponent_spec->car_spec->car_master_actor->t.t.translate.t);
        PointActorAlongThisBloodyVector(pOpponent_spec->car_spec->car_master_actor, &car_to_end);
    }
    return eFPR_OK;
}

// FUNCTION: CARMA2_HW 0x004b13d0
int C2_NAKED C2_HOOK_FASTCALL GetStraight(br_vector2* pStart, br_vector2* pFinish, float* pWidth, int pSection, tOpponent_spec* pOpponent_spec) {
#ifndef CARPOCALYPSE2_MATCHING
    int section;
    float width;
    br_vector2 finish;
    double x, y, dx, dy;
    double cross, length;
    float rounded_x;

    memcpy(&pStart->v[0], &GetOpponentsSectionStartNodePoint(pOpponent_spec, (tS16)pSection)->v[2], 4);
    memcpy(&pStart->v[1], &GetOpponentsSectionStartNodePoint(pOpponent_spec, (tS16)pSection)->v[0], 4);
    memcpy(&pFinish->v[0], &GetOpponentsSectionFinishNodePoint(pOpponent_spec, (tS16)pSection)->v[2], 4);
    memcpy(&pFinish->v[1], &GetOpponentsSectionFinishNodePoint(pOpponent_spec, (tS16)pSection)->v[0], 4);
    *pWidth = NavigationScalarRound(GetOpponentsSectionWidth(pOpponent_spec, (tS16)pSection));
    section = GetOpponentsNextSection(pOpponent_spec, (tS16)pSection);
    while (section != -1) {
        memcpy(&finish.v[0], &GetOpponentsSectionFinishNodePoint(pOpponent_spec, (tS16)section)->v[2], 4);
        memcpy(&finish.v[1], &GetOpponentsSectionFinishNodePoint(pOpponent_spec, (tS16)section)->v[0], 4);
        dx = NavigationFloatDifference(&pFinish->v[0], &pStart->v[0]);
        dy = NavigationFloatDifference(&pFinish->v[1], &pStart->v[1]);
        x = NavigationAdd(NavigationFloatDifference(&finish.v[0], &pFinish->v[0]), dx);
        y = NavigationAdd(NavigationFloatDifference(&finish.v[1], &pFinish->v[1]), dy);
        rounded_x = NavigationScalarRound(x);
        if (NavigationLessOrUnordered(NavigationAdd(NavigationMultiply(x, dx), NavigationMultiply(y, dy)), 0.0)) {
            break;
        }
        length = NavigationSqrt(NavigationAdd(NavigationMultiply(rounded_x, rounded_x), NavigationMultiply(y, y)));
        cross = NavigationAdd(NavigationMultiply(-dy, rounded_x), NavigationMultiply(dx, y));
        cross = fabs(NavigationDivide(cross, length));
        if (!NavigationLessOrUnordered(cross, 0.1f)) {
            break;
        }
        pFinish->v[0] = NavigationScalarRound(finish.v[0]);
        pFinish->v[1] = NavigationScalarRound(finish.v[1]);
        width = NavigationScalarRound(GetOpponentsSectionWidth(pOpponent_spec, (tS16)section));
        if (NavigationOrderedGreater(*pWidth, width)) {
            memcpy(pWidth, &width, 4);
        }
        section = GetOpponentsNextSection(pOpponent_spec, (tS16)section);
    }
    return section;
#else
    __asm {
            sub esp, 0x10
            push ebx
            mov ebx, dword ptr [esp + 0x1c]
            push ebp
            push esi
            push edi
            mov edi, dword ptr [esp + 0x2c]
            mov esi, edx
            mov ebp, ecx
            mov edx, ebx
            mov ecx, edi
            call GetOpponentsSectionStartNodePoint
            mov eax, dword ptr [eax + 8]
            mov edx, ebx
            mov ecx, edi
            mov dword ptr [ebp], eax
            call GetOpponentsSectionStartNodePoint
            mov ecx, dword ptr [eax]
            mov edx, ebx
            mov dword ptr [ebp + 4], ecx
            mov ecx, edi
            call GetOpponentsSectionFinishNodePoint
            mov edx, dword ptr [eax + 8]
            mov ecx, edi
            mov dword ptr [esi], edx
            mov edx, ebx
            call GetOpponentsSectionFinishNodePoint
            mov eax, dword ptr [eax]
            mov edx, ebx
            mov ecx, edi
            mov dword ptr [esi + 4], eax
            call GetOpponentsSectionWidth
            mov ecx, dword ptr [esp + 0x24]
            mov edx, ebx
            fstp dword ptr [ecx]
            mov ecx, edi
            call GetOpponentsNextSection
            mov ebx, eax
            cmp ebx, -1
            je L004b14fc
    L004b143f:
            mov edx, ebx
            mov ecx, edi
            call GetOpponentsSectionFinishNodePoint
            mov eax, dword ptr [eax + 8]
            mov edx, ebx
            mov ecx, edi
            mov dword ptr [esp + 0x18], eax
            call GetOpponentsSectionFinishNodePoint
            fld dword ptr [esi]
            fld dword ptr [eax]
            fld dword ptr [esi + 4]
            fld dword ptr [esp + 0x18]
            fxch st(3)
            fsub dword ptr [ebp]
            fxch st(1)
            fsub dword ptr [ebp + 4]
            fxch st(3)
            fsub dword ptr [esi]
            fld st(2)
            fsub dword ptr [esi + 4]
            fxch st(1)
            fadd st(0), st(2)
            fxch st(1)
            fadd st(0), st(4)
            fld st(1)
            fmul st(0), st(3)
            fxch st(1)
            fst dword ptr [esp + 0x14]
            fmul st(0), st(5)
            faddp st(1), st(0)
            fcomp dword ptr [gFLOAT_0058a61c]
            fxch st(2)
            fxch st(3)
            fxch st(1)
            fxch st(2)
            fnstsw ax
            test ah, 1
            je L004b1508
            fstp st(0)
            fstp st(0)
            fstp st(0)
    L004b14a7:
            xor eax, eax
    L004b14a9:
            test eax, eax
            je L004b14fa
            fld dword ptr [esp + 0x18]
            mov edx, ebx
            mov ecx, edi
            fstp dword ptr [esi]
            fstp dword ptr [esi + 4]
            call GetOpponentsSectionWidth
            mov ecx, dword ptr [esp + 0x24]
            fld dword ptr [ecx]
            fxch st(1)
            fstp dword ptr [esp + 0x28]
            fcomp dword ptr [esp + 0x28]
            fnstsw ax
            test ah, 0x41
            jne L004b14dc
            mov edx, dword ptr [esp + 0x28]
            mov dword ptr [ecx], edx
    L004b14dc:
            mov edx, ebx
            mov ecx, edi
            call GetOpponentsNextSection
            mov ebx, eax
            cmp ebx, -1
            jne L004b143f
            pop edi
            pop esi
            pop ebp
            pop ebx
            add esp, 0x10
            ret 0xc
    L004b14fa:
            fstp st(0)
    L004b14fc:
            pop edi
            pop esi
            mov eax, ebx
            pop ebp
            pop ebx
            add esp, 0x10
            ret 0xc
    L004b1508:
            fld st(0)
            fld dword ptr [esp + 0x14]
            fxch st(1)
            fmul st(0), st(2)
            fxch st(1)
            fmul dword ptr [esp + 0x14]
            fld dword ptr [esp + 0x14]
            fxch st(4)
            fchs
            fxch st(1)
            faddp st(2), st(0)
            fmul st(0), st(2)
            fxch st(1)
            fsqrt
            fxch st(3)
            fmul st(0), st(4)
            fxch st(3)
            fstp st(4)
            fxch st(2)
            faddp st(2), st(0)
            fxch st(2)
            fdivp st(1), st(0)
            fabs
            fxch st(1)
            fstp st(0)
            fcomp dword ptr [gFLOAT_0058a64c]
            fnstsw ax
            test ah, 1
            je L004b14a7
            mov eax, 1
            jmp L004b14a9
            nop
            nop
            nop
            nop
            nop
    }
#endif
}

// FUNCTION: CARMA2_HW 0x004b1560
int C2_NAKED C2_HOOK_FASTCALL CalcCorners(tCorner* pCorners, int pSection, float pWidth, const br_vector2* pStart, tOpponent_spec* pOpponent_spec) {
#ifndef CARPOCALYPSE2_MATCHING
    int count = 0;
    br_vector2 previous;
    br_vector2 start;
    br_vector2 finish;
    float width;
    float ax, ay, bx, by, negative_by;
    float previous_length, next_length;
    float rounded_base;
    double x, y, dot, cross, norm, cosine, sine, ratio, base, extent, radius, previous_length_wide;
    tCorner* corner;

    memcpy(&previous, pStart, sizeof(previous));
    while (pSection != -1 && count < 4) {
        corner = &pCorners[count];
        corner->section = (tU32)pSection;
        pSection = GetStraight(&start, &finish, &width, pSection, pOpponent_spec);
        x = NavigationFloatDifference(&previous.v[0], &start.v[0]);
        y = NavigationFloatDifference(&previous.v[1], &start.v[1]);
        ax = NavigationScalarRound(x);
        ay = NavigationScalarRound(y);
        previous_length_wide = NavigationAdd(NavigationMultiply(x, ax), NavigationMultiply(y, ay));
        previous_length = NavigationScalarRound(previous_length_wide);
        x = NavigationFloatDifference(&finish.v[0], &start.v[0]);
        y = NavigationFloatDifference(&finish.v[1], &start.v[1]);
        bx = NavigationScalarRound(x);
        by = NavigationScalarRound(y);
        next_length = NavigationScalarRound(NavigationAdd(NavigationMultiply(x, bx), NavigationMultiply(y, by)));
        if (previous_length_wide == 0.0 || previous_length_wide != previous_length_wide || next_length == 0.f || next_length != next_length) {
            goto fudge;
        }
        dot = NavigationAdd(NavigationMultiply(bx, ax), NavigationMultiply(by, ay));
        negative_by = NavigationScalarRound(-(double)by);
        cross = NavigationAdd(NavigationMultiply(ax, negative_by), NavigationMultiply(bx, ay));
        norm = NavigationSqrt(NavigationAdd(NavigationMultiply(dot, dot), NavigationMultiply(cross, cross)));
        if (NavigationOrderedGreater(norm, 2.384185791015625e-7f)) {
            norm = NavigationDivide(1.0, norm);
            cosine = NavigationMultiply(norm, dot);
            sine = NavigationMultiply(norm, cross);
        } else {
            cosine = 1.0;
            sine = 0.0;
        }
        if (NavigationOrderedGreater(sine, 0.0)) {
            corner->left_not_right = 1;
        } else {
            corner->left_not_right = 0;
            sine = -sine;
        }
        if (NavigationLessOrUnordered(sine, 0.001f)) {
fudge:
            memcpy(&corner->field_0x00, &start, sizeof(start));
            if (NavigationLessOrUnordered(pWidth, width)) {
                memcpy(&corner->field_0x08, &pWidth, 4);
            } else {
                memcpy(&corner->field_0x08, &width, 4);
            }
            corner->field_0x0c = NavigationScalarRound(NavigationMultiply(corner->field_0x08, 0.7f));
            corner->left_not_right = (tU32)IRandomBetween(0, 1);
        } else {
            base = NavigationDivide(NavigationAdd(NavigationMultiply(cosine, width), pWidth), sine);
            ratio = NavigationDivide(sine, NavigationSubtract(cosine, -1.f));
            rounded_base = NavigationScalarRound(base);
            extent = NavigationMultiply(ratio, width);
            x = NavigationMultiply(ratio, ratio);
            x = NavigationMultiply(x, width);
            x = NavigationSubtract(x, NavigationMultiply(NavigationMultiply(base, ratio), -2.f));
            x = NavigationMultiply(NavigationSubtract(x, width), width);
            extent = NavigationAdd(NavigationAdd(NavigationSqrt(x), extent), rounded_base);
            corner->field_0x0c = NavigationScalarRound(extent);
            x = NavigationMultiply(NavigationMultiply(extent, extent), 9.f);
            if (NavigationOrderedGreater(x, previous_length)) {
                corner->field_0x0c = NavigationScalarRound(NavigationMultiply(NavigationSqrt(previous_length), 0.3333333432674408f));
            }
            x = NavigationMultiply(NavigationMultiply(corner->field_0x0c, corner->field_0x0c), 9.f);
            if (NavigationOrderedGreater(x, next_length)) {
                corner->field_0x0c = NavigationScalarRound(NavigationMultiply(NavigationSqrt(next_length), 0.3333333432674408f));
            }
            radius = NavigationMultiply(ratio, corner->field_0x0c);
            corner->field_0x08 = NavigationScalarRound(radius);
            if (corner->left_not_right != 0) {
                x = NavigationAdd(NavigationMultiply(bx, corner->field_0x0c), NavigationMultiply(radius, negative_by));
                corner->field_0x00.v[0] = NavigationScalarRound(x);
                y = NavigationAdd(NavigationMultiply(bx, corner->field_0x08), NavigationMultiply(by, corner->field_0x0c));
            } else {
                x = NavigationAdd(NavigationMultiply(bx, corner->field_0x0c), NavigationMultiply(by, radius));
                corner->field_0x00.v[0] = NavigationScalarRound(x);
                y = NavigationAdd(NavigationMultiply(by, corner->field_0x0c), NavigationMultiply(-corner->field_0x08, bx));
            }
            corner->field_0x00.v[1] = NavigationScalarRound(y);
            norm = NavigationSqrt(next_length);
            x = NavigationAdd(NavigationDivide(corner->field_0x00.v[0], norm), start.v[0]);
            y = NavigationAdd(NavigationDivide(corner->field_0x00.v[1], norm), start.v[1]);
            corner->field_0x00.v[0] = NavigationScalarRound(x);
            corner->field_0x00.v[1] = NavigationScalarRound(y);
        }
        count++;
        memcpy(&previous, &start, sizeof(previous));
        memcpy(&pWidth, &width, 4);
    }
    return count;
#else
    __asm {
            sub esp, 0x38
            mov eax, dword ptr [esp + 0x3c]
            push ebx
            mov dword ptr [esp + 4], eax
            mov eax, dword ptr [esp + 0x44]
            push ebp
            push esi
            mov esi, dword ptr [eax]
            mov eax, dword ptr [eax + 4]
            xor ebx, ebx
            push edi
            cmp edx, -1
            mov edi, edx
            mov dword ptr [esp + 0x38], esi
            mov dword ptr [esp + 0x3c], eax
            je L004b18a3
            mov ebp, dword ptr [esp + 0x54]
            lea esi, [ecx + 8]
    L004b1594:
            cmp ebx, 4
            jge L004b18a3
            push ebp
            lea ecx, [esp + 0x50]
            push edi
            push ecx
            lea edx, [esp + 0x4c]
            lea ecx, [esp + 0x34]
            mov dword ptr [esi + 0xc], edi
            call GetStraight
            mov edi, eax
            fld dword ptr [esp + 0x3c]
            fld dword ptr [esp + 0x38]
            fsub dword ptr [esp + 0x28]
            fxch st(1)
            fsub dword ptr [esp + 0x2c]
            fld dword ptr [esp + 0x40]
            fld dword ptr [esp + 0x44]
            fxch st(3)
            fst dword ptr [esp + 0x30]
            fxch st(2)
            fst dword ptr [esp + 0x34]
            fxch st(2)
            fmul dword ptr [esp + 0x30]
            fxch st(1)
            fsub dword ptr [esp + 0x28]
            fxch st(2)
            fmul dword ptr [esp + 0x34]
            fxch st(3)
            fsub dword ptr [esp + 0x2c]
            fxch st(3)
            faddp st(1), st(0)
            fxch st(1)
            fst dword ptr [esp + 0x20]
            fxch st(2)
            fst dword ptr [esp + 0x24]
            fxch st(1)
            fst dword ptr [esp + 0x54]
            fxch st(2)
            fmul dword ptr [esp + 0x20]
            fxch st(2)
            fcomp dword ptr [gFLOAT_0058a61c]
            fmul dword ptr [esp + 0x24]
            fnstsw ax
            faddp st(1), st(0)
            test ah, 0x40
            fstp dword ptr [esp + 0x50]
            jne L004b1839
            fld dword ptr [esp + 0x50]
            fcomp dword ptr [gFLOAT_0058a61c]
            fnstsw ax
            test ah, 0x40
            jne L004b1839
            fld dword ptr [esp + 0x24]
            fld dword ptr [esp + 0x20]
            fld dword ptr [esp + 0x24]
            fld dword ptr [esp + 0x30]
            fld dword ptr [esp + 0x20]
            fxch st(3)
            fmul dword ptr [esp + 0x30]
            fxch st(4)
            fchs
            fxch st(2)
            fmul dword ptr [esp + 0x34]
            fxch st(2)
            fstp dword ptr [esp + 0x18]
            fmul dword ptr [esp + 0x18]
            fxch st(1)
            faddp st(3), st(0)
            fxch st(1)
            fmul dword ptr [esp + 0x34]
            faddp st(1), st(0)
            fld st(1)
            fld st(1)
            fxch st(1)
            fmul st(0), st(3)
            fxch st(1)
            fmul st(0), st(2)
            faddp st(1), st(0)
            fsqrt
            fcom dword ptr [gFLOAT_0058a670]
            fnstsw ax
            test ah, 0x41
            jne L004b16a7
            fdivr qword ptr [gDOUBLE_0058a630]
            fld st(0)
            fmulp st(3), st(0)
            fmulp st(1), st(0)
            jmp L004b16b9
    L004b16a7:
            fstp st(0)
            fstp st(0)
            fstp st(0)
            fld dword ptr [gFLOAT_0058a638]
            fld dword ptr [gFLOAT_0058a61c]
    L004b16b9:
            fcom dword ptr [gFLOAT_0058a61c]
            fnstsw ax
            test ah, 0x41
            jne L004b16cf
            mov dword ptr [esi + 8], 1
            jmp L004b16d8
    L004b16cf:
            mov dword ptr [esi + 8], 0
            fchs
    L004b16d8:
            fld st(1)
            fld st(1)
            fxch st(1)
            fmul dword ptr [esp + 0x4c]
            fxch st(1)
            fcomp dword ptr [gFLOAT_0058a688]
            fnstsw ax
            fadd dword ptr [esp + 0x10]
            test ah, 1
            jne L004b1833
            fxch st(2)
            fsub dword ptr [gFLOAT_0058a644]
            fld st(2)
            fdiv st(0), st(2)
            fxch st(1)
            fdivr st(0), st(2)
            fxch st(3)
            fxch st(1)
            fld st(3)
            fmul st(0), st(4)
            fxch st(1)
            fst dword ptr [esp + 0x14]
            fmul st(0), st(4)
            fxch st(1)
            fmul dword ptr [esp + 0x4c]
            fxch st(1)
            fmul dword ptr [gFLOAT_0058a68c]
            fld st(4)
            fxch st(1)
            fsubp st(2), st(0)
            fmul dword ptr [esp + 0x4c]
            fxch st(1)
            fsub dword ptr [esp + 0x4c]
            fmul dword ptr [esp + 0x4c]
            fsqrt
            fst dword ptr [esp + 0x1c]
            fxch st(1)
            faddp st(1), st(0)
            fadd dword ptr [esp + 0x14]
            fstp st(2)
            fstp st(0)
            fld st(0)
            fmul st(0), st(1)
            fmul dword ptr [gFLOAT_0058a690]
            fxch st(1)
            fstp dword ptr [esi + 4]
            fcomp dword ptr [esp + 0x54]
            fnstsw ax
            test ah, 0x41
            jne L004b1776
            fld dword ptr [esp + 0x54]
            fsqrt
            fmul dword ptr [gFLOAT_0058a694]
            fstp dword ptr [esi + 4]
    L004b1776:
            fld dword ptr [esi + 4]
            fld st(0)
            fmul st(0), st(1)
            fmul dword ptr [gFLOAT_0058a690]
            fcomp dword ptr [esp + 0x50]
            fnstsw ax
            test ah, 0x41
            fstp st(0)
            jne L004b179f
            fld dword ptr [esp + 0x50]
            fsqrt
            fmul dword ptr [gFLOAT_0058a694]
            fstp dword ptr [esi + 4]
    L004b179f:
            fmul dword ptr [esi + 4]
            fst dword ptr [esi]
            mov eax, dword ptr [esi + 8]
            fld dword ptr [esp + 0x20]
            test eax, eax
            je L004b17d9
            fmul dword ptr [esi + 4]
            fld dword ptr [esp + 0x20]
            fxch st(2)
            fmul dword ptr [esp + 0x18]
            fld dword ptr [esp + 0x24]
            fxch st(1)
            faddp st(2), st(0)
            fxch st(1)
            fstp dword ptr [esi - 8]
            fxch st(1)
            fmul dword ptr [esi]
            fxch st(1)
            fmul dword ptr [esi + 4]
            faddp st(1), st(0)
            fstp dword ptr [esi - 4]
            jmp L004b1809
    L004b17d9:
            fld dword ptr [esp + 0x24]
            fmul st(0), st(2)
            fxch st(1)
            fmul dword ptr [esi + 4]
            fld dword ptr [esp + 0x24]
            fxch st(1)
            faddp st(2), st(0)
            fxch st(1)
            fstp dword ptr [esi - 8]
            fld dword ptr [esi]
            fxch st(1)
            fmul dword ptr [esi + 4]
            fxch st(1)
            fchs
            fmul dword ptr [esp + 0x20]
            fxch st(1)
            faddp st(1), st(0)
            fstp dword ptr [esi - 4]
            fstp st(0)
    L004b1809:
            fld dword ptr [esp + 0x50]
            fsqrt
            fld dword ptr [esi - 4]
            fld dword ptr [esi - 8]
            fdiv st(0), st(2)
            fxch st(1)
            fdiv st(0), st(2)
            fxch st(1)
            fadd dword ptr [esp + 0x28]
            fxch st(1)
            fadd dword ptr [esp + 0x2c]
            fxch st(1)
            fstp dword ptr [esi - 8]
            fstp dword ptr [esi - 4]
            fstp st(0)
            jmp L004b187e
    L004b1833:
            fstp st(0)
            fstp st(0)
            fstp st(0)
    L004b1839:
            fld dword ptr [esp + 0x10]
            fcomp dword ptr [esp + 0x4c]
            mov edx, dword ptr [esp + 0x28]
            mov eax, dword ptr [esp + 0x2c]
            mov dword ptr [esi - 8], edx
            mov dword ptr [esi - 4], eax
            fnstsw ax
            test ah, 1
            je L004b185e
            mov ecx, dword ptr [esp + 0x10]
            mov dword ptr [esi], ecx
            jmp L004b1864
    L004b185e:
            mov edx, dword ptr [esp + 0x4c]
            mov dword ptr [esi], edx
    L004b1864:
            fld dword ptr [esi]
            fmul dword ptr [gFLOAT_0058a698]
            mov edx, 1
            xor ecx, ecx
            fstp dword ptr [esi + 4]
            call IRandomBetween
            mov dword ptr [esi + 8], eax
    L004b187e:
            mov eax, dword ptr [esp + 0x4c]
            mov ecx, dword ptr [esp + 0x28]
            mov edx, dword ptr [esp + 0x2c]
            inc ebx
            add esi, 0x1c
            cmp edi, -1
            mov dword ptr [esp + 0x10], eax
            mov dword ptr [esp + 0x38], ecx
            mov dword ptr [esp + 0x3c], edx
            jne L004b1594
    L004b18a3:
            pop edi
            pop esi
            mov eax, ebx
            pop ebp
            pop ebx
            add esp, 0x38
            ret 0xc
            nop
    }
#endif
}

// FUNCTION: CARMA2_HW 0x004b1ab0
int C2_NAKED C2_HOOK_FASTCALL CalcSOCs(int pNext_section, int pCount_corners, tCorner* pCorners, tOpponent_spec* pOpponent_spec, tSOC* pSocs, tCar_spec* pCar) {
#ifndef CARPOCALYPSE2_MATCHING
    int count = 0;
    int i;
    int speed;
    double length;

    (void)pCar;
    for (i = 0; i < pCount_corners; i++) {
        while (pNext_section != -1 && count < 10 && pNext_section != (int)pCorners[i].section) {
            length = NavigationCornerSectionLength(pOpponent_spec, &pCorners[i], pNext_section);
            if (!NavigationOrderedGreater(length, pCorners[i].field_0x0c)) {
                break;
            }
            pSocs[count].field_0x0 = -1;
            pSocs[count].field_0x4 = pNext_section;
            count++;
            pNext_section = GetOpponentsNextSection(pOpponent_spec, (tS16)pNext_section);
        }
        if (count < 10) {
            pSocs[count].field_0x0 = 0;
            pSocs[count].field_0x4 = i;
            length = NavigationMultiply(NavigationMultiply(NavigationMultiply(pCorners[i].field_0x08, 6.9f), 7.5f), 1.9600000381469727f);
            pCorners[i].field_0x18 = (tU32)NavigationTruncate(NavigationSqrt(length));
            while (pNext_section != (int)pCorners[i].section) {
                speed = GetOpponentsSectionMaxSpeed(pOpponent_spec, (tS16)pNext_section, 1);
                if (speed < (int)pCorners[i].field_0x18) {
                    pCorners[i].field_0x18 = (tU32)(int)GetOpponentsSectionMaxSpeed(pOpponent_spec, (tS16)pNext_section, 1);
                }
                pNext_section = GetOpponentsNextSection(pOpponent_spec, (tS16)pNext_section);
            }
            while (pNext_section != -1) {
                length = NavigationCornerSectionLength(pOpponent_spec, &pCorners[i], pNext_section);
                if (!NavigationLessOrUnordered(length, pCorners[i].field_0x0c)) {
                    break;
                }
                speed = GetOpponentsSectionMaxSpeed(pOpponent_spec, (tS16)pNext_section, 1);
                if (speed < (int)pCorners[i].field_0x18) {
                    pCorners[i].field_0x18 = (tU32)(int)GetOpponentsSectionMaxSpeed(pOpponent_spec, (tS16)pNext_section, 1);
                }
                pNext_section = GetOpponentsNextSection(pOpponent_spec, (tS16)pNext_section);
            }
            count++;
        }
    }
    return count;
#else
    __asm {
            sub esp, 0x24
            xor eax, eax
            push esi
            cmp edx, eax
            mov dword ptr [esp + 0xc], edx
            mov dword ptr [esp + 4], eax
            mov esi, ecx
            mov dword ptr [esp + 8], eax
            jle L004b1c8a
            mov ecx, dword ptr [esp + 0x2c]
            push edi
            mov edi, dword ptr [esp + 0x34]
            push ebp
            push ebx
            lea ebp, [ecx + 0x14]
    L004b1ada:
            cmp esi, -1
            je L004b1b75
            mov edx, dword ptr [esp + 0x40]
            lea ebx, [edx + eax*8]
            jmp L004b1af0
    L004b1aec:
            mov eax, dword ptr [esp + 0x10]
    L004b1af0:
            cmp eax, 0xa
            jge L004b1c6f
            cmp esi, dword ptr [ebp]
            je L004b1b75
            mov dx, word ptr [ebp]
            mov ecx, edi
            call GetOpponentsSectionStartNodePoint
            mov edx, esi
            mov ecx, edi
            mov dword ptr [esp + 0x38], eax
            call GetOpponentsSectionFinishNodePoint
            fld dword ptr [eax + 8]
            _emit 0xd9
        _emit 0x00
            mov eax, dword ptr [esp + 0x38]
            _emit 0xd8
        _emit 0x20
            fxch st(1)
            fsub dword ptr [eax + 8]
            fxch st(1)
            fst dword ptr [esp + 0x1c]
            fld st(1)
            fxch st(1)
            fmul dword ptr [esp + 0x1c]
            fxch st(1)
            fmul st(0), st(2)
            faddp st(1), st(0)
            fsqrt
            fstp st(1)
            fcomp dword ptr [ebp - 8]
            fnstsw ax
            test ah, 0x41
            jne L004b1b71
            mov eax, dword ptr [ebx]
            mov edx, dword ptr [esp + 0x10]
            or al, 1
            inc edx
            mov dword ptr [ebx], eax
            mov dword ptr [esp + 0x10], edx
            mov dword ptr [ebx + 4], esi
            mov edx, esi
            mov ecx, edi
            add ebx, 8
            call GetOpponentsNextSection
            mov esi, eax
            cmp esi, -1
            jne L004b1aec
    L004b1b71:
            mov eax, dword ptr [esp + 0x10]
    L004b1b75:
            cmp eax, 0xa
            jge L004b1c6f
            mov ecx, dword ptr [esp + 0x40]
            lea ebx, [ebp + 4]
            mov edx, dword ptr [ecx + eax*8]
            and edx, 0xfffffffe
            mov dword ptr [ecx + eax*8], edx
            mov edx, dword ptr [esp + 0x14]
            mov dword ptr [ecx + eax*8 + 4], edx
            fld dword ptr [ebp - 0xc]
            fmul dword ptr [gFLOAT_0058a620]
            fmul dword ptr [gFLOAT_0058a6a4+0x4]
            fmul dword ptr [gFLOAT_0058a6a4+0x8]
            fsqrt
            call _ftol
            mov dword ptr [ebx], eax
            mov eax, dword ptr [ebp]
            cmp esi, eax
            je L004b1beb
    L004b1bbb:
            push 1
            mov edx, esi
            mov ecx, edi
            call GetOpponentsSectionMaxSpeed
            mov ecx, ebx
            cmp dword ptr [ecx], eax
            jle L004b1bd9
            push 1
            mov edx, esi
            mov ecx, edi
            call GetOpponentsSectionMaxSpeed
            mov dword ptr [ebx], eax
    L004b1bd9:
            mov edx, esi
            mov ecx, edi
            call GetOpponentsNextSection
            mov esi, eax
            mov eax, dword ptr [ebp]
            cmp esi, eax
            jne L004b1bbb
    L004b1beb:
            cmp esi, -1
            je L004b1c66
    L004b1bf0:
            mov dx, word ptr [ebp]
            mov ecx, edi
            call GetOpponentsSectionStartNodePoint
            mov edx, esi
            mov ecx, edi
            mov dword ptr [esp + 0x38], eax
            call GetOpponentsSectionFinishNodePoint
            fld dword ptr [eax + 8]
            _emit 0xd9
        _emit 0x00
            mov eax, dword ptr [esp + 0x38]
            _emit 0xd8
        _emit 0x20
            fxch st(1)
            fsub dword ptr [eax + 8]
            fxch st(1)
            fst dword ptr [esp + 0x28]
            fld st(1)
            fxch st(1)
            fmul dword ptr [esp + 0x28]
            fxch st(1)
            fmul st(0), st(2)
            faddp st(1), st(0)
            fsqrt
            fstp st(1)
            fcomp dword ptr [ebp - 8]
            fnstsw ax
            test ah, 1
            je L004b1c66
            push 1
            mov edx, esi
            mov ecx, edi
            call GetOpponentsSectionMaxSpeed
            cmp dword ptr [ebx], eax
            jle L004b1c56
            push 1
            mov edx, esi
            mov ecx, edi
            call GetOpponentsSectionMaxSpeed
            mov dword ptr [ebx], eax
    L004b1c56:
            mov edx, esi
            mov ecx, edi
            call GetOpponentsNextSection
            mov esi, eax
            cmp esi, -1
            jne L004b1bf0
    L004b1c66:
            mov eax, dword ptr [esp + 0x10]
            inc eax
            mov dword ptr [esp + 0x10], eax
    L004b1c6f:
            mov ecx, dword ptr [esp + 0x14]
            mov edx, dword ptr [esp + 0x18]
            inc ecx
            add ebp, 0x1c
            cmp ecx, edx
            mov dword ptr [esp + 0x14], ecx
            jl L004b1ada
            pop ebx
            pop ebp
            pop edi
    L004b1c8a:
            pop esi
            add esp, 0x24
            ret 0x10
            nop
            nop
            nop
            nop
            nop
            nop
            nop
            nop
            nop
            nop
            nop
            nop
            nop
            nop
            nop
    }
#endif
}

#define DRAW_DIAL_FRAME(pY) \
    RemoveTransientBitmaps(1); \
    DRPixelmapRectangleMaskedCopy(gBack_screen, \
        *(br_int_16*)((int*)&gCurrent_graf_data->field_0x114 + pDial_index), \
        *(br_int_16*)((int*)&gCurrent_graf_data->field_0x11c + pDial_index), \
        gDials_pix, 0, (pY), gDials_pix->width, 64); \
    ProcessFlicQueue(gFrame_period); \
    DoMouseCursor(); \
    PDScreenBufferSwap(0)

// FUNCTION: CARMA2_HW 0x004b1d80
void C2_NAKED C2_HOOK_FASTCALL DrawDial(int pDial_index, volatile int pFrom_value, int pTo_value) {
#ifndef CARPOCALYPSE2_MATCHING
    int initial_time;
    int interval_start;
    int elapsed;
    int frame_value;
    tU32 elapsed_u;

    DRAW_DIAL_FRAME(pFrom_value * 64);
    initial_time = PDGetTotalTime();
    elapsed = PDGetTotalTime();
    for (elapsed = elapsed - initial_time; elapsed < 100; elapsed = elapsed - initial_time) {
        frame_value = pFrom_value + elapsed * (pTo_value - pFrom_value) / 100;
        DRAW_DIAL_FRAME(frame_value * 64);
        elapsed = PDGetTotalTime();
    }
    DRAW_DIAL_FRAME(pTo_value * 64);

    interval_start = PDGetTotalTime();
    frame_value = pTo_value < 0x18 ? pTo_value + 1 : 0x16;
    DRAW_DIAL_FRAME(frame_value * 64);
    elapsed_u = (tU32)(PDGetTotalTime() - interval_start);
    while (elapsed_u < 0x14) {
        elapsed_u = (tU32)(PDGetTotalTime() - interval_start);
    }

    interval_start = PDGetTotalTime();
    frame_value = pTo_value == 0 ? 2 : pTo_value - 1;
    DRAW_DIAL_FRAME(frame_value * 64);
    elapsed_u = (tU32)(PDGetTotalTime() - interval_start);
    while (elapsed_u < 0x14) {
        elapsed_u = (tU32)(PDGetTotalTime() - interval_start);
    }

    interval_start = PDGetTotalTime();
    frame_value = pTo_value < 0x18 ? pTo_value + 1 : 0x16;
    DRAW_DIAL_FRAME(frame_value * 64);
    elapsed_u = (tU32)(PDGetTotalTime() - interval_start);
    while (elapsed_u < 0x14) {
        elapsed_u = (tU32)(PDGetTotalTime() - interval_start);
    }

    interval_start = PDGetTotalTime();
    DRAW_DIAL_FRAME(initial_time);
    elapsed_u = (tU32)(PDGetTotalTime() - interval_start);
    while (elapsed_u < 0x14) {
        elapsed_u = (tU32)(PDGetTotalTime() - interval_start);
    }

    interval_start = PDGetTotalTime();
    frame_value = pTo_value == 0 ? 2 : pTo_value - 1;
    DRAW_DIAL_FRAME(frame_value * 64);
    elapsed_u = (tU32)(PDGetTotalTime() - interval_start);
    while (elapsed_u < 0x14) {
        elapsed_u = (tU32)(PDGetTotalTime() - interval_start);
    }

    DRAW_DIAL_FRAME(initial_time);
#else
    __asm {
    L004b1d80:
            sub esp, 0xc
            push ebx
            push ebp
            push esi
            mov esi, ecx
            mov ebp, edx
            push edi
            mov ecx, 1
            mov dword ptr [esp + 0x18], ebp
            call RemoveTransientBitmaps
            mov eax, dword ptr [gDials_pix]
            mov edx, ebp
            push 0x40
            lea edi, [esi*4 + 0x11c]
            mov cx, word ptr [eax + 0x34]
            lea ebx, [esi*4 + 0x114]
            shl edx, 6
            push ecx
            push edx
            push 0
            push eax
            mov eax, dword ptr [gCurrent_graf_data]
            mov cx, word ptr [eax + edi]
            mov dx, word ptr [eax + ebx]
            push ecx
            mov ecx, dword ptr [gBack_screen]
            call DRPixelmapRectangleMaskedCopy
            mov ecx, dword ptr [gFrame_period]
            call ProcessFlicQueue
            call DoMouseCursor
            xor ecx, ecx
            call PDScreenBufferSwap
            call PDGetTotalTime
            mov dword ptr [esp + 0x10], eax
            call PDGetTotalTime
            mov ecx, dword ptr [esp + 0x10]
            mov esi, eax
            sub esi, ecx
            cmp esi, 0x64
            jge L004b1e91
            mov eax, dword ptr [esp + 0x20]
            sub eax, ebp
            mov dword ptr [esp + 0x14], eax
    L004b1e15:
            mov ecx, 1
            call RemoveTransientBitmaps
            mov ebp, dword ptr [gDials_pix]
            mov ecx, esi
            imul ecx, dword ptr [esp + 0x14]
            mov dx, word ptr [ebp + 0x34]
            push 0x40
            mov eax, 0x51eb851f
            push edx
            imul ecx
            sar edx, 5
            mov eax, edx
            shr eax, 0x1f
            add edx, eax
            mov eax, dword ptr [esp + 0x20]
            add edx, eax
            mov eax, dword ptr [gCurrent_graf_data]
            shl edx, 6
            mov cx, word ptr [eax + edi]
            push edx
            mov dx, word ptr [eax + ebx]
            push 0
            push ebp
            push ecx
            mov ecx, dword ptr [gBack_screen]
            call DRPixelmapRectangleMaskedCopy
            mov ecx, dword ptr [gFrame_period]
            call ProcessFlicQueue
            call DoMouseCursor
            xor ecx, ecx
            call PDScreenBufferSwap
            call PDGetTotalTime
            sub eax, dword ptr [esp + 0x10]
            mov esi, eax
            cmp esi, 0x64
            jl L004b1e15
    L004b1e91:
            mov ecx, 1
            call RemoveTransientBitmaps
            mov esi, dword ptr [esp + 0x20]
            mov ecx, dword ptr [gDials_pix]
            mov eax, esi
            push 0x40
            mov dx, word ptr [ecx + 0x34]
            shl eax, 6
            push edx
            mov dword ptr [esp + 0x18], eax
            push eax
            mov eax, dword ptr [gCurrent_graf_data]
            push 0
            push ecx
            mov cx, word ptr [eax + edi]
            mov dx, word ptr [eax + ebx]
            push ecx
            mov ecx, dword ptr [gBack_screen]
            call DRPixelmapRectangleMaskedCopy
            mov ecx, dword ptr [gFrame_period]
            call ProcessFlicQueue
            call DoMouseCursor
            xor ecx, ecx
            call PDScreenBufferSwap
            call PDGetTotalTime
            cmp esi, 0x18
            mov ebp, eax
            jge L004b1ef8
            inc esi
            jmp L004b1efd
    L004b1ef8:
            mov esi, 0x16
    L004b1efd:
            mov ecx, 1
            call RemoveTransientBitmaps
            mov eax, dword ptr [gDials_pix]
            push 0x40
            mov dx, word ptr [eax + 0x34]
            shl esi, 6
            push edx
            push esi
            push 0
            push eax
            mov eax, dword ptr [gCurrent_graf_data]
            mov cx, word ptr [eax + edi]
            mov dx, word ptr [eax + ebx]
            push ecx
            mov ecx, dword ptr [gBack_screen]
            call DRPixelmapRectangleMaskedCopy
            mov ecx, dword ptr [gFrame_period]
            call ProcessFlicQueue
            call DoMouseCursor
            xor ecx, ecx
            call PDScreenBufferSwap
            call PDGetTotalTime
            sub eax, ebp
            cmp eax, 0x14
            jae L004b1f62
    L004b1f56:
            call PDGetTotalTime
            sub eax, ebp
            cmp eax, 0x14
            jb L004b1f56
    L004b1f62:
            call PDGetTotalTime
            mov esi, dword ptr [esp + 0x20]
            mov ebp, eax
            test esi, esi
            je L004b1f74
            dec esi
            jmp L004b1f79
    L004b1f74:
            mov esi, 2
    L004b1f79:
            mov ecx, 1
            call RemoveTransientBitmaps
            mov eax, dword ptr [gDials_pix]
            push 0x40
            mov dx, word ptr [eax + 0x34]
            shl esi, 6
            push edx
            push esi
            push 0
            push eax
            mov eax, dword ptr [gCurrent_graf_data]
            mov cx, word ptr [eax + edi]
            mov dx, word ptr [eax + ebx]
            push ecx
            mov ecx, dword ptr [gBack_screen]
            call DRPixelmapRectangleMaskedCopy
            mov ecx, dword ptr [gFrame_period]
            call ProcessFlicQueue
            call DoMouseCursor
            xor ecx, ecx
            call PDScreenBufferSwap
            call PDGetTotalTime
            sub eax, ebp
            cmp eax, 0x14
            jae L004b1fde
    L004b1fd2:
            call PDGetTotalTime
            sub eax, ebp
            cmp eax, 0x14
            jb L004b1fd2
    L004b1fde:
            call PDGetTotalTime
            mov esi, dword ptr [esp + 0x20]
            mov ebp, eax
            cmp esi, 0x18
            jge L004b1ff1
            inc esi
            jmp L004b1ff6
    L004b1ff1:
            mov esi, 0x16
    L004b1ff6:
            mov ecx, 1
            call RemoveTransientBitmaps
            mov eax, dword ptr [gDials_pix]
            push 0x40
            mov dx, word ptr [eax + 0x34]
            shl esi, 6
            push edx
            push esi
            push 0
            push eax
            mov eax, dword ptr [gCurrent_graf_data]
            mov cx, word ptr [eax + edi]
            mov dx, word ptr [eax + ebx]
            push ecx
            mov ecx, dword ptr [gBack_screen]
            call DRPixelmapRectangleMaskedCopy
            mov ecx, dword ptr [gFrame_period]
            call ProcessFlicQueue
            call DoMouseCursor
            xor ecx, ecx
            call PDScreenBufferSwap
            call PDGetTotalTime
            sub eax, ebp
            cmp eax, 0x14
            jae L004b205b
    L004b204f:
            call PDGetTotalTime
            sub eax, ebp
            cmp eax, 0x14
            jb L004b204f
    L004b205b:
            call PDGetTotalTime
            mov ecx, 1
            mov esi, eax
            call RemoveTransientBitmaps
            mov eax, dword ptr [gDials_pix]
            mov ecx, dword ptr [esp + 0x10]
            push 0x40
            mov dx, word ptr [eax + 0x34]
            push edx
            push ecx
            push 0
            mov ecx, dword ptr [gBack_screen]
            push eax
            mov eax, dword ptr [gCurrent_graf_data]
            mov dx, word ptr [eax + edi]
            push edx
            mov dx, word ptr [eax + ebx]
            call DRPixelmapRectangleMaskedCopy
            mov ecx, dword ptr [gFrame_period]
            call ProcessFlicQueue
            call DoMouseCursor
            xor ecx, ecx
            call PDScreenBufferSwap
            call PDGetTotalTime
            sub eax, esi
            cmp eax, 0x14
            jae L004b20c8
    L004b20bc:
            call PDGetTotalTime
            sub eax, esi
            cmp eax, 0x14
            jb L004b20bc
    L004b20c8:
            call PDGetTotalTime
            mov esi, dword ptr [esp + 0x20]
            mov ebp, eax
            test esi, esi
            je L004b20da
            dec esi
            jmp L004b20df
    L004b20da:
            mov esi, 2
    L004b20df:
            mov ecx, 1
            call RemoveTransientBitmaps
            mov eax, dword ptr [gDials_pix]
            push 0x40
            mov cx, word ptr [eax + 0x34]
            shl esi, 6
            push ecx
            push esi
            push 0
            mov ecx, dword ptr [gBack_screen]
            push eax
            mov eax, dword ptr [gCurrent_graf_data]
            mov dx, word ptr [eax + edi]
            push edx
            mov dx, word ptr [eax + ebx]
            call DRPixelmapRectangleMaskedCopy
            mov ecx, dword ptr [gFrame_period]
            call ProcessFlicQueue
            call DoMouseCursor
            xor ecx, ecx
            call PDScreenBufferSwap
            call PDGetTotalTime
            sub eax, ebp
            cmp eax, 0x14
            jae L004b2144
    L004b2138:
            call PDGetTotalTime
            sub eax, ebp
            cmp eax, 0x14
            jb L004b2138
    L004b2144:
            mov ecx, 1
            call RemoveTransientBitmaps
            mov eax, dword ptr [gDials_pix]
            mov edx, dword ptr [esp + 0x10]
            push 0x40
            mov cx, word ptr [eax + 0x34]
            push ecx
            push edx
            push 0
            push eax
            mov eax, dword ptr [gCurrent_graf_data]
            mov cx, word ptr [eax + edi]
            mov dx, word ptr [eax + ebx]
            push ecx
            mov ecx, dword ptr [gBack_screen]
            call DRPixelmapRectangleMaskedCopy
            mov ecx, dword ptr [gFrame_period]
            call ProcessFlicQueue
            call DoMouseCursor
            xor ecx, ecx
            call PDScreenBufferSwap
            pop edi
            pop esi
            pop ebp
            pop ebx
            add esp, 0xc
            ret 4
            nop
            nop
            nop
            nop
            mov eax, dword ptr [gProgram_state+0x70]
            push esi
            mov esi, ecx
            push 0
            lea edx, [eax*4]
            xor ecx, ecx
            call L004b1d80
            mov ecx, dword ptr [gProgram_state+0x74]
            push 0
            lea edx, [ecx*4]
            mov ecx, 1
            call L004b1d80
            mov eax, esi
            pop esi
            ret 0xc
            nop
            nop
            nop
            nop
            nop
            nop
            nop
            nop
            nop
            push ebx
            mov ebx, ecx
            mov eax, dword ptr [ebx]
            cmp eax, 2
            je L004b2238
            push edi
            push esi
            test eax, eax
            mov esi, offset gProgram_state+0x70
            je L004b21fa
            mov esi, offset gProgram_state+0x74
    L004b21fa:
            mov edi, dword ptr [esi]
            lea eax, [edi - 1]
            test eax, eax
            mov dword ptr [esi], eax
            jge L004b220b
            mov dword ptr [esi], 0
    L004b220b:
            xor ecx, ecx
            call SetSoundVolumes
            mov ecx, dword ptr [gEffects_outlet]
            mov edx, 0xbb8
            call DRS3StartSound
            mov eax, dword ptr [esi]
            mov ecx, dword ptr [ebx]
            shl eax, 2
            push eax
            lea edx, [edi*4]
            call L004b1d80
            pop esi
            pop edi
    L004b2238:
            xor eax, eax
            pop ebx
            ret
            nop
            nop
            nop
            nop
            push ebx
            mov ebx, ecx
            mov eax, dword ptr [ebx]
            cmp eax, 2
            je L004b2299
            push edi
            push esi
            test eax, eax
            mov esi, offset gProgram_state+0x70
            je L004b225a
            mov esi, offset gProgram_state+0x74
    L004b225a:
            mov edi, dword ptr [esi]
            lea eax, [edi + 1]
            cmp eax, 6
            mov dword ptr [esi], eax
            jle L004b226c
            mov dword ptr [esi], 6
    L004b226c:
            xor ecx, ecx
            call SetSoundVolumes
            mov ecx, dword ptr [gEffects_outlet]
            mov edx, 0xbb8
            call DRS3StartSound
            mov eax, dword ptr [esi]
            mov ecx, dword ptr [ebx]
            shl eax, 2
            push eax
            lea edx, [edi*4]
            call L004b1d80
            pop esi
            pop edi
    L004b2299:
            xor eax, eax
            pop ebx
            ret
            nop
            nop
            nop
            push ecx
            mov eax, dword ptr [gCurrent_graf_data]
            push ebx
            mov ebx, ecx
            push esi
            mov ecx, dword ptr [esp + 0x10]
            mov edx, dword ptr [eax + 0x124]
            sub ecx, edx
            mov edx, dword ptr [eax + 0x128]
            mov eax, dword ptr [esp + 0x14]
            mov dword ptr [esp + 0x10], ecx
            fild dword ptr [esp + 0x10]
            sub edx, eax
            push edi
            mov dword ptr [esp + 0x14], edx
            fstp dword ptr [esp + 0xc]
            fild dword ptr [esp + 0x14]
            fst dword ptr [esp + 0x14]
            fcomp dword ptr [gFLOAT_0058a6a4+0xc]
            fnstsw ax
            test ah, 0x41
            jne L004b23c2
            fld dword ptr [esp + 0xc]
            fcomp dword ptr [gFLOAT_0058a6a4+0xc]
            fnstsw ax
            test ah, 0x40
            jne L004b230b
            fld dword ptr [esp + 0x14]
            fdiv dword ptr [esp + 0xc]
            fld1
            fpatan
            jmp L004b2311
    L004b230b:
            fld dword ptr [gFLOAT_0058a6a4+0x10]
    L004b2311:
            fcom dword ptr [gFLOAT_0058a6a4+0xc]
            fnstsw ax
            test ah, 1
            je L004b2324
            fsub qword ptr [gFLOAT_0058a6a4+0x14]
    L004b2324:
            fcom qword ptr [gFLOAT_0058a6a4+0x1c]
            fnstsw ax
            test ah, 0x41
            jne L004b23c0
            fcom qword ptr [gFLOAT_0058a6a4+0x24]
            fnstsw ax
            test ah, 1
            je L004b23c0
            mov eax, dword ptr [ebx]
            mov esi, offset gProgram_state+0x70
            test eax, eax
            je L004b2352
            mov esi, offset gProgram_state+0x74
    L004b2352:
            fsub qword ptr [gFLOAT_0058a6a4+0x24]
            mov edi, dword ptr [esi]
            fsubr qword ptr [gFLOAT_0058a6a4+0x2c]
            fmul qword ptr [gFLOAT_0058a6a4+0x34]
            call _ftol
            cmp eax, 6
            mov dword ptr [esi], eax
            jle L004b237a
            mov dword ptr [esi], 6
            jmp L004b2384
    L004b237a:
            test eax, eax
            jge L004b2384
            mov dword ptr [esi], 0
    L004b2384:
            cmp dword ptr [esi], edi
            je L004b23c2
            xor ecx, ecx
            call SetSoundVolumes
            mov eax, dword ptr [esi]
            mov ecx, dword ptr [gEffects_outlet]
            mov edx, 0xbb8
            cmp eax, edi
            call DRS3StartSound
            mov eax, dword ptr [esi]
            mov ecx, dword ptr [ebx]
            shl eax, 2
            push eax
            lea edx, [edi*4]
            call L004b1d80
            xor eax, eax
            pop edi
            pop esi
            pop ebx
            pop ecx
            ret 8
    L004b23c0:
            fstp st(0)
    L004b23c2:
            pop edi
            pop esi
            xor eax, eax
            pop ebx
            pop ecx
            ret 8
            nop
            nop
            nop
            nop
            nop
            push esi
            push edi
            mov ecx, 1
            call RemoveTransientBitmaps
            call DontLetFlicFuckWithPalettes
            call TurnFlicTransparencyOn
            xor edi, edi
            mov esi, offset gUNK_0065bd80
    L004b23ed:
            mov ecx, dword ptr [esi]
            mov eax, dword ptr [esi - 4]
            add ecx, edi
            push eax
            mov edx, dword ptr [ecx*4 + gUNK_0065bd80+0x4]
            mov ecx, 0x120
            call RunFlicAt
            add esi, 0x20
            add edi, 8
            cmp esi, offset gUNK_0065bee0+0x20
            jl L004b23ed
            call TurnFlicTransparencyOff
            call LetFlicFuckWithPalettes
            pop edi
            pop esi
            ret
    }
#endif
}

#undef DRAW_DIAL_FRAME

// FUNCTION: CARMA2_HW 0x004b1ca0
void C2_HOOK_FASTCALL FUN_004b1ca0(void) {
    RemoveTransientBitmaps(1);
    DRPixelmapRectangleMaskedCopy(gBack_screen, gCurrent_graf_data->field_0x114, gCurrent_graf_data->field_0x11c,
        gDials_pix, 0, 0, gDials_pix->width, 64);
    ProcessFlicQueue(gFrame_period);
    DoMouseCursor();
    PDScreenBufferSwap(0);
    RemoveTransientBitmaps(1);
    DRPixelmapRectangleMaskedCopy(gBack_screen, gCurrent_graf_data->field_0x118, gCurrent_graf_data->field_0x120,
        gDials_pix, 0, 0, gDials_pix->width, 64);
    ProcessFlicQueue(gFrame_period);
    DoMouseCursor();
    PDScreenBufferSwap(0);
    DrawDial(0, 0, gProgram_state.music_volume * 4);
    DrawDial(1, 0, gProgram_state.effects_volume * 4);
}

// FUNCTION: CARMA2_HW 0x004b25c0
int C2_HOOK_FASTCALL FUN_004b25c0(int* pArg1, int* pArg2) {
    if (*pArg2 == 0) {
        *pArg2 = 1;
        *pArg1 = 13;
    } else if (*pArg1 == 1) {
        *pArg2 = 0;
        *pArg1 = 0;
    } else {
        goto failed;
    }
    DRS3StartSound(gEffects_outlet, eSoundId_LeftButton);
    return 1;

failed:
    return 0;
}

// GLOBAL: CARMA2_HW 0x00691788
int gUNK_00691788;

// GLOBAL: CARMA2_HW 0x006918cc
tU8* gUNK_006918cc;

// GLOBAL: CARMA2_HW 0x0065bd78
tDial_flic_option gDial_flic_options[13] = {
    { 4, 36, 0, { 132, 175, 221, 253, 0 } },
    { 3, 45, 0, { 132, 157, 217, 0, 0 } },
    { 3, 54, 0, { 132, 183, 236, 0, 0 } },
    { 3, 67, 0, { 132, 194, 234, 0, 0 } },
    { 3, 76, 0, { 132, 194, 234, 0, 0 } },
    { 4, 89, 0, { 132, 176, 204, 246, 0 } },
    { 4, 98, 0, { 132, 176, 204, 246, 0 } },
    { 2, 111, 0, { 132, 158, 0, 0, 0 } },
    { 2, 120, 0, { 132, 158, 0, 0, 0 } },
    { 2, 129, 0, { 132, 158, 0, 0, 0 } },
    { 2, 138, 0, { 132, 158, 0, 0, 0 } },
    { 3, 150, 0, { 132, 164, 207, 0, 0 } },
    { 4, 153, 0, { 177, 199, 220, 242, 0 } },
};

// GLOBAL: CARMA2_HW 0x00768f68
int gUNK_00768f68;

// FUNCTION: CARMA2_HW 0x004b24f0
int C2_NAKED C2_HOOK_FASTCALL FUN_004b24f0(int* pDial_index) {
#ifndef CARPOCALYPSE2_MATCHING
    int dial_value;
    int current_selection;
    int next_selection;

    DRS3StartSound(gEffects_outlet, 3000);
    dial_value = *pDial_index;
    next_selection = gDial_flic_options[dial_value - 2].selected + 1;
    if (next_selection == gDial_flic_options[dial_value - 2].count) {
        next_selection = 0;
    }
    current_selection = gDial_flic_options[dial_value - 2].selected;
    RemoveTransientBitmaps(1);
    DontLetFlicFuckWithPalettes();
    TurnFlicTransparencyOn();
    RunFlicAt(0x11f, gDial_flic_options[dial_value - 2].flic_frames[current_selection], gDial_flic_options[dial_value - 2].y);
    TurnFlicTransparencyOff();
    LetFlicFuckWithPalettes();
    RemoveTransientBitmaps(1);
    DontLetFlicFuckWithPalettes();
    TurnFlicTransparencyOn();
    RunFlicAt(0x120, gDial_flic_options[dial_value - 2].flic_frames[next_selection], gDial_flic_options[dial_value - 2].y);
    TurnFlicTransparencyOff();
    LetFlicFuckWithPalettes();
    gDial_flic_options[dial_value - 2].selected = next_selection;
    return 1;
#else
    __asm {
            push ebx
            push ebp
            push esi
            mov esi, ecx
            mov ecx, dword ptr [gEffects_outlet]
            push edi
            mov edx, 0xbb8
            call DRS3StartSound
            mov esi, dword ptr [esi]
            lea ebx, [esi - 2]
            mov eax, ebx
            shl eax, 5
            mov edi, dword ptr [eax + gUNK_0065bd80]
            mov ecx, dword ptr [eax + gDial_flic_options]
            inc edi
            cmp edi, ecx
            jne L004b2523
            xor edi, edi
    L004b2523:
            mov esi, ebx
            mov ecx, 1
            shl esi, 5
            mov ebp, dword ptr [esi + gUNK_0065bd80]
            call RemoveTransientBitmaps
            call DontLetFlicFuckWithPalettes
            call TurnFlicTransparencyOn
            mov eax, dword ptr [esi + gDial_flic_options+0x4]
            mov ecx, 0x11f
            shl ebx, 3
            add ebp, ebx
            push eax
            mov edx, dword ptr [ebp*4 + gUNK_0065bd80+0x4]
            call RunFlicAt
            call TurnFlicTransparencyOff
            call LetFlicFuckWithPalettes
            mov ecx, 1
            call RemoveTransientBitmaps
            call DontLetFlicFuckWithPalettes
            call TurnFlicTransparencyOn
            mov ecx, dword ptr [esi + gDial_flic_options+0x4]
            lea edx, [edi + ebx]
            push ecx
            mov ecx, 0x120
            mov edx, dword ptr [edx*4 + gUNK_0065bd80+0x4]
            call RunFlicAt
            call TurnFlicTransparencyOff
            call LetFlicFuckWithPalettes
            mov dword ptr [esi + gUNK_0065bd80], edi
            mov eax, 1
            pop edi
            pop esi
            pop ebp
            pop ebx
            ret
    }
#endif
}
// FUNCTION: CARMA2_HW 0x004b2740
int C2_NAKED C2_HOOK_FASTCALL FUN_004b2740(int* pArg1) {
#ifndef CARPOCALYPSE2_MATCHING
    int idx;
    int cur;
    int next;
    tDial_flic_option* dial;

    DRS3StartSound(gEffects_outlet, 3000);
    idx = *pArg1 - 2;
    dial = &gDial_flic_options[idx];
    next = dial->selected + 1;
    if (next == dial->count) {
        next = 0;
    }
    cur = dial->selected;
    RemoveTransientBitmaps(1);
    DontLetFlicFuckWithPalettes();
    TurnFlicTransparencyOn();
    RunFlicAt(0x11f, dial->flic_frames[cur], dial->y);
    TurnFlicTransparencyOff();
    LetFlicFuckWithPalettes();
    RemoveTransientBitmaps(1);
    DontLetFlicFuckWithPalettes();
    TurnFlicTransparencyOn();
    RunFlicAt(0x120, dial->flic_frames[next], dial->y);
    TurnFlicTransparencyOff();
    LetFlicFuckWithPalettes();
    dial->selected = next;
    return 0;
#else
    __asm {
            push ebx
            push ebp
            push esi
            mov esi, ecx
            mov ecx, dword ptr [gEffects_outlet]
            push edi
            mov edx, 0xbb8
            call DRS3StartSound
            mov esi, dword ptr [esi]
            lea ebx, [esi - 2]
            mov eax, ebx
            shl eax, 5
            mov edi, dword ptr [eax + gUNK_0065bd80]
            mov ecx, dword ptr [eax + gDial_flic_options]
            inc edi
            cmp edi, ecx
            jne L004b2773
            xor edi, edi
    L004b2773:
            mov esi, ebx
            mov ecx, 1
            shl esi, 5
            mov ebp, dword ptr [esi + gUNK_0065bd80]
            call RemoveTransientBitmaps
            call DontLetFlicFuckWithPalettes
            call TurnFlicTransparencyOn
            mov eax, dword ptr [esi + gDial_flic_options+0x4]
            mov ecx, 0x11f
            shl ebx, 3
            add ebp, ebx
            push eax
            mov edx, dword ptr [ebp*4 + gUNK_0065bd80+0x4]
            call RunFlicAt
            call TurnFlicTransparencyOff
            call LetFlicFuckWithPalettes
            mov ecx, 1
            call RemoveTransientBitmaps
            call DontLetFlicFuckWithPalettes
            call TurnFlicTransparencyOn
            mov ecx, dword ptr [esi + gDial_flic_options+0x4]
            lea edx, [edi + ebx]
            push ecx
            mov ecx, 0x120
            mov edx, dword ptr [edx*4 + gUNK_0065bd80+0x4]
            call RunFlicAt
            call TurnFlicTransparencyOff
            call LetFlicFuckWithPalettes
            mov dword ptr [esi + gUNK_0065bd80], edi
            xor eax, eax
            pop edi
            pop esi
            pop ebp
            pop ebx
            ret
    }
#endif
}

// FUNCTION: CARMA2_HW 0x004b2430
int C2_NAKED C2_HOOK_FASTCALL FUN_004b2430(int* pDial_index) {
#ifndef CARPOCALYPSE2_MATCHING
    int dial_value;
    int previous_selection;
    int selection;

    DRS3StartSound(gEffects_outlet, 3000);
    dial_value = *pDial_index;
    selection = gDial_flic_options[dial_value - 2].selected - 1;
    if (selection < 0) {
        selection = gDial_flic_options[dial_value - 2].count - 1;
    }
    previous_selection = gDial_flic_options[dial_value - 2].selected;
    RemoveTransientBitmaps(1);
    DontLetFlicFuckWithPalettes();
    TurnFlicTransparencyOn();
    RunFlicAt(0x11f, gDial_flic_options[dial_value - 2].flic_frames[previous_selection], gDial_flic_options[dial_value - 2].y);
    TurnFlicTransparencyOff();
    LetFlicFuckWithPalettes();
    RemoveTransientBitmaps(1);
    DontLetFlicFuckWithPalettes();
    TurnFlicTransparencyOn();
    RunFlicAt(0x120, gDial_flic_options[dial_value - 2].flic_frames[selection], gDial_flic_options[dial_value - 2].y);
    TurnFlicTransparencyOff();
    LetFlicFuckWithPalettes();
    gDial_flic_options[dial_value - 2].selected = selection;
    return 1;
#else
    __asm {
            push ebx
            push ebp
            push esi
            mov esi, ecx
            mov ecx, dword ptr [gEffects_outlet]
            push edi
            mov edx, 0xbb8
            call DRS3StartSound
            mov esi, dword ptr [esi]
            lea ebx, [esi - 2]
            mov eax, ebx
            shl eax, 5
            mov edi, dword ptr [eax + gUNK_0065bd80]
            dec edi
            jns L004b2460
            mov edi, dword ptr [eax + gDial_flic_options]
            dec edi
    L004b2460:
            mov esi, ebx
            mov ecx, 1
            shl esi, 5
            mov ebp, dword ptr [esi + gUNK_0065bd80]
            call RemoveTransientBitmaps
            call DontLetFlicFuckWithPalettes
            call TurnFlicTransparencyOn
            mov eax, dword ptr [esi + gDial_flic_options+0x4]
            mov ecx, 0x11f
            shl ebx, 3
            add ebp, ebx
            push eax
            mov edx, dword ptr [ebp*4 + gUNK_0065bd80+0x4]
            call RunFlicAt
            call TurnFlicTransparencyOff
            call LetFlicFuckWithPalettes
            mov ecx, 1
            call RemoveTransientBitmaps
            call DontLetFlicFuckWithPalettes
            call TurnFlicTransparencyOn
            mov ecx, dword ptr [esi + gDial_flic_options+0x4]
            lea edx, [edi + ebx]
            push ecx
            mov ecx, 0x120
            mov edx, dword ptr [edx*4 + gUNK_0065bd80+0x4]
            call RunFlicAt
            call TurnFlicTransparencyOff
            call LetFlicFuckWithPalettes
            mov dword ptr [esi + gUNK_0065bd80], edi
            mov eax, 1
            pop edi
            pop esi
            pop ebp
            pop ebx
            ret
    }
#endif
}

// FUNCTION: CARMA2_HW 0x004b2640
int C2_NAKED C2_HOOK_FAKE_THISCALL FUN_004b2640(int* pDial_index, undefined4 pArg2, int pX_offset, undefined4 pArg3) {
#ifndef CARPOCALYPSE2_MATCHING
    int dial_index;
    int selection;
    int previous_selection;
    int left_edge;
    int* pFrame;
    tU8* pGraf_data;

    CARPOCALYPSE2_THISCALL_UNUSED(pArg2);
    CARPOCALYPSE2_THISCALL_UNUSED(pArg3);
    dial_index = *pDial_index + -2;
    selection = gDial_flic_options[dial_index].count - 1;
    if (selection >= 0) {
        pGraf_data = *(tU8**)((tU8*)gUNK_006918cc + 0x120);
        left_edge = *(int*)(pGraf_data + gReal_graf_data_index * 4 + 0x60) + pX_offset + 3;
        pFrame = &gDial_flic_options[dial_index].flic_frames[selection];
        while (left_edge < *pFrame) {
            selection--;
            pFrame--;
            if (selection < 0) {
                return 0;
            }
        }

        DRS3StartSound(gEffects_outlet, 3000);
        dial_index = *pDial_index - 2;
        previous_selection = gDial_flic_options[dial_index].selected;
        RemoveTransientBitmaps(1);
        DontLetFlicFuckWithPalettes();
        TurnFlicTransparencyOn();
        RunFlicAt(0x11f, gDial_flic_options[dial_index].flic_frames[previous_selection], gDial_flic_options[dial_index].y);
        TurnFlicTransparencyOff();
        LetFlicFuckWithPalettes();
        RemoveTransientBitmaps(1);
        DontLetFlicFuckWithPalettes();
        TurnFlicTransparencyOn();
        RunFlicAt(0x120, gDial_flic_options[dial_index].flic_frames[selection], gDial_flic_options[dial_index].y);
        TurnFlicTransparencyOff();
        LetFlicFuckWithPalettes();
        gDial_flic_options[dial_index].selected = selection;
    }
    return 0;
#else
    __asm {
            push ebx
            push ebp
            push esi
            push edi
            mov edi, ecx
            mov eax, dword ptr [edi]
            add eax, -2
            mov ecx, eax
            shl ecx, 5
            mov esi, dword ptr [ecx + gDial_flic_options]
            dec esi
            js L004b2734
            mov edx, dword ptr [gUNK_006918cc]
            lea eax, [esi + eax*8]
            mov ecx, dword ptr [edx + 0x120]
            mov edx, dword ptr [gReal_graf_data_index]
            lea eax, [eax*4 + gUNK_0065bd80+0x4]
            mov ecx, dword ptr [ecx + edx*4 + 0x60]
            mov edx, dword ptr [esp + 0x14]
            lea ecx, [ecx + edx + 3]
    L004b2685:
            cmp ecx, dword ptr [eax]
            jge L004b269a
            dec esi
            sub eax, 4
            test esi, esi
            jge L004b2685
            xor eax, eax
            pop edi
            pop esi
            pop ebp
            pop ebx
            ret 8
    L004b269a:
            mov ecx, dword ptr [gEffects_outlet]
            mov edx, 0xbb8
            call DRS3StartSound
            mov ebx, dword ptr [edi]
            mov ecx, 1
            sub ebx, 2
            mov edi, ebx
            shl edi, 5
            mov ebp, dword ptr [edi + gUNK_0065bd80]
            call RemoveTransientBitmaps
            call DontLetFlicFuckWithPalettes
            call TurnFlicTransparencyOn
            mov ecx, dword ptr [edi + gDial_flic_options+0x4]
            shl ebx, 3
            add ebp, ebx
            push ecx
            mov ecx, 0x11f
            mov edx, dword ptr [ebp*4 + gUNK_0065bd80+0x4]
            call RunFlicAt
            call TurnFlicTransparencyOff
            call LetFlicFuckWithPalettes
            mov ecx, 1
            call RemoveTransientBitmaps
            call DontLetFlicFuckWithPalettes
            call TurnFlicTransparencyOn
            mov edx, dword ptr [edi + gDial_flic_options+0x4]
            lea eax, [esi + ebx]
            push edx
            mov ecx, 0x120
            mov edx, dword ptr [eax*4 + gUNK_0065bd80+0x4]
            call RunFlicAt
            call TurnFlicTransparencyOff
            call LetFlicFuckWithPalettes
            mov dword ptr [edi + gUNK_0065bd80], esi
    L004b2734:
            pop edi
            pop esi
            pop ebp
            xor eax, eax
            pop ebx
            ret 8
            nop
            nop
            nop
    }
#endif
}

// FUNCTION: CARMA2_HW 0x004b2800
void C2_HOOK_FASTCALL FUN_004b2800(int pIndex) {
    tU8* pGraf_data;

    if (gUNK_00691788 != pIndex) {
        if (gUNK_00691788 - 2 >= 0) {
            pGraf_data = *(tU8**)((tU8*)gUNK_006918cc + 0x120);
            DrawRectangleOutline(gBack_screen,
                *(int*)(pGraf_data + gReal_graf_data_index * 4 + 0x60) - 6,
                gDial_flic_options[gUNK_00691788 - 2].y - 3,
                *(int*)(pGraf_data + gReal_graf_data_index * 4 + 0x70) + 3,
                gUNK_00768f68 + 2 + gDial_flic_options[gUNK_00691788 - 2].y, 0);
        }
        if (pIndex - 2 >= 0) {
            pGraf_data = *(tU8**)((tU8*)gUNK_006918cc + 0x120);
            DrawRectangleOutline(gBack_screen,
                *(int*)(pGraf_data + gReal_graf_data_index * 4 + 0x60) - 6,
                gDial_flic_options[pIndex - 2].y - 3,
                *(int*)(pGraf_data + gReal_graf_data_index * 4 + 0x70) + 3,
                gUNK_00768f68 + 2 + gDial_flic_options[pIndex - 2].y, 0x2d);
        }
        gUNK_00691788 = pIndex;
    }
}
