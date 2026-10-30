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
int C2_HOOK_FASTCALL GetStraight(br_vector2* pStart, br_vector2* pFinish, float* pWidth, int pSection, tOpponent_spec* pOpponent_spec) {
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
}

// FUNCTION: CARMA2_HW 0x004b1560
int C2_HOOK_FASTCALL CalcCorners(tCorner* pCorners, int pSection, float pWidth, const br_vector2* pStart, tOpponent_spec* pOpponent_spec) {
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
}

// FUNCTION: CARMA2_HW 0x004b1ab0
int C2_HOOK_FASTCALL CalcSOCs(int pNext_section, int pCount_corners, tCorner* pCorners, tOpponent_spec* pOpponent_spec, tSOC* pSocs, tCar_spec* pCar) {
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
void C2_HOOK_FASTCALL DrawDial(int pDial_index, volatile int pFrom_value, int pTo_value) {
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
}

#undef DRAW_DIAL_FRAME

// GLOBAL: CARMA2_HW 0x0075bbf0
int gUNK_0075bbf0;

// GLOBAL: CARMA2_HW 0x0075bbf4
int gUNK_0075bbf4;

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
    DrawDial(0, 0, gUNK_0075bbf0 * 4);
    DrawDial(1, 0, gUNK_0075bbf4 * 4);
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

// C2PORT-NONMATCHING: best 72.16% | difftest PASS 2000 trials (spec difftest/specs/004b24f0_FUN_004b24f0.json) | reason register-allocation/struct-interior-operands
// FUNCTION: CARMA2_HW 0x004b24f0
int C2_HOOK_FASTCALL FUN_004b24f0(int* pDial_index) {
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
}
// C2PORT-NONMATCHING: best 51.16% | difftest PASS 2000 trials (spec difftest/specs/004b2740_FUN_004b2740.json) | reason unknown-source-shape
// FUNCTION: CARMA2_HW 0x004b2740
int C2_HOOK_FASTCALL FUN_004b2740(int* pArg1) {
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
}

// C2PORT-NONMATCHING: best 79.17% | difftest PASS 2000 trials (spec difftest/specs/004b2430_FUN_004b2430.json) | reason register-allocation
// FUNCTION: CARMA2_HW 0x004b2430
int C2_HOOK_FASTCALL FUN_004b2430(int* pDial_index) {
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
}

// C2PORT-NONMATCHING: best 78.87% | difftest PASS 2000 trials (spec difftest/specs/004b2640_FUN_004b2640.json) | reason vc5-loop-register-layout-and-struct-interior-operands
// FUNCTION: CARMA2_HW 0x004b2640
int C2_HOOK_FAKE_THISCALL FUN_004b2640(int* pDial_index, undefined4 pArg2, int pX_offset, undefined4 pArg3) {
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
