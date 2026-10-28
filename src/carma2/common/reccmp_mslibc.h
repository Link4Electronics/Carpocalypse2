#ifndef C2_RECCMP_MSLIBC_H
#define C2_RECCMP_MSLIBC_H

/*
 * Annotations for the Microsoft C runtime data used by the recompiled
 * functions (ctype tables, locale data, ...). The declarations live behind
 * "#if 0" because the runtime owns the storage: these entries only tell
 * reccmp which original address a referenced global had, so that constant
 * offsets in diffs show up as the real symbol name.
 */

#if 0

// GLOBAL: CARMA2_HW 0x00673a50
extern unsigned short _pctype[];

// GLOBAL: CARMA2_HW 0x00673c5c
extern int __mb_cur_max;

// GLOBAL: CARMA2_HW 0x007a0884
// __crtheap

// GLOBAL: CARMA2_HW 0x006b2ea8
// __newmode

// GLOBAL: CARMA2_HW 0x0067607c
// ___sbh_threshold

// GLOBAL: CARMA2_HW 0x00674058
// ___small_block_heap

// GLOBAL: CARMA2_HW 0x006b2eac
// ?_pnhHeap@@3P6AHI@ZA

// GLOBAL: CARMA2_HW 0x007a18a0
// __nstream

// GLOBAL: CARMA2_HW 0x007a0898
// ___piob

// GLOBAL: CARMA2_HW 0x006b3374
// __fmode

// GLOBAL: CARMA2_HW 0x006b2e08
// __umaskval

// GLOBAL: CARMA2_HW 0x006b2e04
// __doserrno

// GLOBAL: CARMA2_HW 0x007a0880
// __nhandle

// GLOBAL: CARMA2_HW 0x007a0780
// ___pioinfo

// GLOBAL: CARMA2_HW 0x006b2e70
// ___lc_handle

// GLOBAL: CARMA2_HW 0x00673c84
// ___app_type

// GLOBAL: CARMA2_HW 0x006b2e88
// ___lc_codepage

// GLOBAL: CARMA2_HW 0x006b2ef4
// __pxcptinfoptrs

// GLOBAL: CARMA2_HW 0x00676278
// __First_FPE_Indx

// GLOBAL: CARMA2_HW 0x0067627c
// __Num_FPE

// GLOBAL: CARMA2_HW 0x00676284
// __fpecode

// GLOBAL: CARMA2_HW 0x006b2e0c
// __osver

// GLOBAL: CARMA2_HW 0x006b2e10
// __winver

// GLOBAL: CARMA2_HW 0x006b2e14
// __winmajor

// GLOBAL: CARMA2_HW 0x006b2e18
// __winminor

// GLOBAL: CARMA2_HW 0x007a088c
// __acmdln

// GLOBAL: CARMA2_HW 0x006b2e4c
// __aenvptr

// GLOBAL: CARMA2_HW 0x006b2e28
// __environ

// GLOBAL: CARMA2_HW 0x006b2e30
// __wenviron

// GLOBAL: CARMA2_HW 0x006b3210
// ___mbcodepage

// GLOBAL: CARMA2_HW 0x006b3214
// ___mblcid

// GLOBAL: CARMA2_HW 0x006b3280
// __commode

// GLOBAL: CARMA2_HW 0x006b2de4
// __cflush

// GLOBAL: CARMA2_HW 0x006b2e54
// ___error_mode

// GLOBAL: CARMA2_HW 0x006b3228
// __adbgmsg

// GLOBAL: CARMA2_HW 0x006b2e1c
// ___argc

// GLOBAL: CARMA2_HW 0x006b2e20
// ___argv

// GLOBAL: CARMA2_HW 0x006b2e38
// __pgmptr

// GLOBAL: CARMA2_HW 0x00673724
// __FPinit

// GLOBAL: CARMA2_HW 0x00676460
// __matherr_flag

// GLOBAL: CARMA2_HW 0x006b2e2c
// ___initenv

// GLOBAL: CARMA2_HW 0x00673d00
// __fptrap

// GLOBAL: CARMA2_HW 0x00673c60
// ___decimal_point

// GLOBAL: CARMA2_HW 0x00676420
// __HUGE

// GLOBAL: CARMA2_HW 0x00676590
// __pow10pos

// GLOBAL: CARMA2_HW 0x006766f0
// __pow10neg

// GLOBAL: CARMA2_HW 0x00673e00
// __d_inf

// GLOBAL: CARMA2_HW 0x00673e10
// __d_max

// GLOBAL: CARMA2_HW 0x006b3008
// _mbctype

// GLOBAL: CARMA2_HW 0x00676468
// __timezone

// GLOBAL: CARMA2_HW 0x0067646c
// __daylight

// GLOBAL: CARMA2_HW 0x00676470
// __dstbias

// GLOBAL: CARMA2_HW 0x00676520
// __lpdays

// GLOBAL: CARMA2_HW 0x00676558
// __days

// GLOBAL: CARMA2_HW 0x006b3290
// _tzinfo

// GLOBAL: CARMA2_HW 0x006b3288
// _tzapiused

// GLOBAL: CARMA2_HW 0x006764f8
// __tzname

// GLOBAL: CARMA2_HW 0x006b2ef8
// _pgmname

// GLOBAL: CARMA2_HW 0x0058d878
// __DEFAULT_CW_in_mem

// GLOBAL: CARMA2_HW 0x006b2dd8
// ___fastflag

// GLOBAL: CARMA2_HW 0x00673e50
// __indefinite

// GLOBAL: CARMA2_HW 0x00673e8a
// __minfinity

// GLOBAL: CARMA2_HW 0x00673ec4
// __OP_EXPjmptab

// GLOBAL: CARMA2_HW 0x006b3110
// __mbcasemap

// GLOBAL: CARMA2_HW 0x0058f000
// ___xc_a

// GLOBAL: CARMA2_HW 0x0058f004
// ___xc_z

// GLOBAL: CARMA2_HW 0x0058f008
// ___xi_a

// GLOBAL: CARMA2_HW 0x0058f010
// ___xi_z

#endif

#endif /* C2_RECCMP_MSLIBC_H */
