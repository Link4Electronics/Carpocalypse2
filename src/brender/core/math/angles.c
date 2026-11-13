#include "angles.h"

#include "matrix34.h"
#include "matrix4.h"
#include "quat.h"

order_info OrderAxes[32] = {
    { 0, 1, 2, },
    { 1, 2, 0, },
    { 2, 0, 1, },
    { 0, 0, 0, },
    { 0, 2, 1, },
    { 1, 0, 2, },
    { 2, 1, 0, },
    { 0, 0, 0, },
    { 0, 1, 2, },
    { 1, 2, 0, },
    { 2, 0, 1, },
    { 0, 0, 0, },
    { 0, 2, 1, },
    { 1, 0, 2, },
    { 2, 1, 0, },
    { 0, 0, 0, },
    { 0, 1, 2, },
    { 1, 2, 0, },
    { 2, 0, 1, },
    { 0, 0, 0, },
    { 0, 2, 1, },
    { 1, 0, 2, },
    { 2, 1, 0, },
    { 0, 0, 0, },
    { 0, 1, 2, },
    { 1, 2, 0, },
    { 2, 0, 1, },
    { 0, 0, 0, },
    { 0, 2, 1, },
    { 1, 0, 2, },
    { 2, 1, 0, },
    { 0, 0, 0, },
};

#ifdef CARPOCALYPSE2_MATCHING
extern char IID_IDirectInputDevice2A[];
extern br_uint_8 _CombineTransforms[7][7];
extern void __cdecl _ftol();
#endif
// FUNCTION: CARMA2_HW 0x00531de0
C2_NAKED br_matrix34* C2_HOOK_CDECL BrEulerToMatrix34(br_matrix34* mat, const br_euler* euler) {
#ifndef CARPOCALYPSE2_MATCHING
    br_uint_8 o;
    br_angle ti;
    br_angle tj;
    br_angle th;
    br_scalar ci;
    br_scalar cj;
    br_scalar ch;
    br_scalar si;
    br_scalar sj;
    br_scalar sh;
    br_scalar cc;
    br_scalar cs;
    br_scalar sc;
    br_scalar ss;
    int a0;
    int a1;
    int a2;

    o = euler->order;
    if (o & 0x10) {
        ti = euler->c;
        tj = euler->b;;
        th = euler->a;
    } else {
        ti = euler->c;
        tj = euler->b;
        th = euler->a;
    }
    if (o & 0x4) {
        ti = -ti;
        tj = -tj;
        th = -th;
    }
    ci = BR_COS(ti);
    cj = BR_COS(tj);
    ch = BR_COS(th);

    si = BR_SIN(ti);
    sj = BR_SIN(tj);
    sh = BR_SIN(th);

    cc = ci * ch;
    cs = ci * sh;
    sc = si * ch;
    ss = si * sh;

    a0 = OrderAxes[o].a0;
    a1 = OrderAxes[o].a1;
    a2 = OrderAxes[o].a2;

    if (o & 0x8) {
        mat->m[a0][a0] =              cj;
        mat->m[a1][a0] =   sj * si;
        mat->m[a2][a0] =   sj * ci;
        mat->m[a0][a1] =   sj * sh;
        mat->m[a1][a1] = -(cj * ss) + cc;
        mat->m[a2][a1] = -(cj * cs) - sc;
        mat->m[a0][a2] = -(sj * ch);
        mat->m[a1][a2] =   cj * sc + cs;
        mat->m[a2][a2] =   cj * cc - ss;
    } else {
        mat->m[a0][a0] =  cj * ch;
        mat->m[a1][a0] =  sj * sc - cs;
        mat->m[a2][a0] =  sj * cc + ss;
        mat->m[a0][a1] =  cj * sh;
        mat->m[a1][a1] =  sj * ss + cc;
        mat->m[a2][a1] =  sj * cs - sc;
        mat->m[a0][a2] =          - sj;
        mat->m[a1][a2] =  cj * si;
        mat->m[a2][a2] =  cj * ci;
    }

    mat->m[3][0] = 0.f;
    mat->m[3][1] = 0.f;
    mat->m[3][2] = 0.f;

    return mat;
#else
    __asm {
            sub esp, 0x20
            mov edx, dword ptr [esp + 0x28]
            push ebx
            push ebp
            push esi
            mov bl, byte ptr [edx + 6]
            push edi
            test bl, 0x10
            mov byte ptr [esp + 0x28], bl
            je L00531e04
            mov ax, word ptr [edx + 4]
            mov cx, word ptr [edx + 2]
            mov si, word ptr [edx]
            jmp L00531e0f
    L00531e04:
            mov ax, word ptr [edx]
            mov cx, word ptr [edx + 2]
            mov si, word ptr [edx + 4]
    L00531e0f:
            test bl, 4
            je L00531e1a
            neg eax
            neg ecx
            neg esi
    L00531e1a:
            and esi, 0xffff
            and eax, 0xffff
            mov dword ptr [esp + 0x2c], esi
            mov dword ptr [esp + 0x38], eax
            fild dword ptr [esp + 0x2c]
            fild dword ptr [esp + 0x38]
            and ecx, 0xffff
            mov esi, dword ptr [esp + 0x28]
            mov dword ptr [esp + 0x38], ecx
            and esi, 0xff
            fmul qword ptr [IID_IDirectInputDevice2A+0x500]
            fxch st(1)
            fmul qword ptr [IID_IDirectInputDevice2A+0x500]
            fild dword ptr [esp + 0x38]
            fxch st(2)
            fst dword ptr [esp + 0x38]
            fld st(1)
            fld dword ptr [esp + 0x38]
            fxch st(4)
            fmul qword ptr [IID_IDirectInputDevice2A+0x500]
            fxch st(1)
            fcos
            shl esi, 2
            xor ecx, ecx
            xor edx, edx
            mov cl, byte ptr [esi + _CombineTransforms+0x38]
            fxch st(2)
            fsin
            fxch st(3)
            fsin
            fxch st(4)
            fcos
            fxch st(1)
            fst dword ptr [esp + 0x38]
            fld dword ptr [esp + 0x38]
            fxch st(2)
            fst dword ptr [esp + 0x38]
            fxch st(4)
            fxch st(1)
            fcos
            fxch st(2)
            fsin
            fxch st(3)
            fst dword ptr [esp + 0x38]
            fxch st(1)
            fst dword ptr [esp + 0x38]
            fxch st(5)
            fst dword ptr [esp + 0x38]
            fxch st(1)
            fst dword ptr [esp + 0x14]
            fxch st(5)
            fst dword ptr [esp + 0x24]
            fxch st(1)
            fst dword ptr [esp + 0x18]
            fld dword ptr [esp + 0x18]
            fxch st(3)
            fst dword ptr [esp + 0x38]
            fstp dword ptr [esp + 0x10]
            fxch st(5)
            fmul st(0), st(4)
            fxch st(3)
            fstp dword ptr [esp + 0x38]
            fxch st(4)
            fmul st(0), st(3)
            fld dword ptr [esp + 0x38]
            fxch st(5)
            fmul dword ptr [esp + 0x14]
            fxch st(2)
            fmul dword ptr [esp + 0x24]
            fxch st(5)
            fstp dword ptr [esp + 0x38]
            fxch st(2)
            fstp dword ptr [esp + 0x28]
            mov dl, byte ptr [esi + _CombineTransforms+0x39]
            xor eax, eax
            mov al, byte ptr [esi + _CombineTransforms+0x3a]
            fxch st(1)
            fstp dword ptr [esp + 0x1c]
            fstp dword ptr [esp + 0x2c]
            fxch st(1)
            fstp dword ptr [esp + 0x20]
            test bl, 8
            mov esi, eax
            je L00531ff1
            fld dword ptr [esp + 0x20]
            fld dword ptr [esp + 0x1c]
            fmul dword ptr [esp + 0x10]
            fld dword ptr [esp + 0x38]
            fld dword ptr [esp + 0x38]
            fxch st(3)
            fmul dword ptr [esp + 0x10]
            fld dword ptr [esp + 0x2c]
            fxch st(2)
            fmul dword ptr [esp + 0x24]
            fld dword ptr [esp + 0x18]
            fld dword ptr [esp + 0x10]
            mov eax, dword ptr [esp + 0x34]
            mov ebx, ecx
            lea edi, [edx + edx*2]
            shl ebx, 4
            lea ebp, [ecx + edi]
            fstp dword ptr [ebx + eax]
            fxch st(1)
            fstp dword ptr [eax + ebp*4]
            fld dword ptr [esp + 0x28]
            fxch st(5)
            fmul st(0), st(6)
            lea ebx, [esi + esi*2]
            lea ebp, [ecx + ecx*2]
            add ecx, ebx
            mov dword ptr [esp + 0x34], ebp
            fstp dword ptr [eax + ecx*4]
            fld dword ptr [esp + 0x38]
            fxch st(1)
            fmul dword ptr [esp + 0x38]
            lea ecx, [edx + ebp]
            mov ebp, edx
            shl ebp, 4
            fstp dword ptr [eax + ecx*4]
            fxch st(2)
            fmul dword ptr [esp + 0x10]
            fxch st(4)
            fmul dword ptr [esp + 0x10]
            fxch st(3)
            fchs
            fxch st(2)
            fmul dword ptr [esp + 0x14]
            fxch st(2)
            fsub dword ptr [esp + 0x2c]
            fxch st(1)
            fsubr dword ptr [esp + 0x28]
            fxch st(4)
            fadd dword ptr [esp + 0x1c]
            fxch st(3)
            fsub dword ptr [esp + 0x20]
            fxch st(4)
            fstp dword ptr [ebp + eax]
            mov ecx, dword ptr [esp + 0x34]
            add edx, ebx
            fxch st(1)
            fchs
            fxch st(1)
            fstp dword ptr [eax + edx*4]
            add ecx, esi
            mov ebx, esi
            add esi, edi
            fstp dword ptr [eax + ecx*4]
            fstp dword ptr [eax + esi*4]
            shl ebx, 4
            jmp L005320b7
    L00531ff1:
            fld dword ptr [esp + 0x28]
            fld dword ptr [esp + 0x2c]
            fmul dword ptr [esp + 0x38]
            fld dword ptr [esp + 0x20]
            fxch st(2)
            fmul dword ptr [esp + 0x38]
            fld dword ptr [esp + 0x14]
            fld dword ptr [esp + 0x1c]
            fxch st(4)
            fmul dword ptr [esp + 0x38]
            fxch st(1)
            fmul dword ptr [esp + 0x10]
            fld dword ptr [esp + 0x18]
            fxch st(1)
            mov eax, dword ptr [esp + 0x34]
            mov ebx, ecx
            shl ebx, 4
            lea edi, [edx + edx*2]
            lea ebp, [ecx + ecx*2]
            fstp dword ptr [ebx + eax]
            fxch st(4)
            fmul dword ptr [esp + 0x38]
            fxch st(3)
            fsub dword ptr [esp + 0x1c]
            fxch st(4)
            fmul dword ptr [esp + 0x10]
            fxch st(2)
            fadd dword ptr [esp + 0x20]
            fld dword ptr [esp + 0x24]
            fxch st(5)
            mov dword ptr [esp + 0x34], ebp
            lea ebp, [ecx + edi]
            lea ebx, [esi + esi*2]
            fstp dword ptr [eax + ebp*4]
            fld dword ptr [esp + 0x10]
            fxch st(1)
            add ecx, ebx
            mov ebp, edx
            shl ebp, 4
            fstp dword ptr [eax + ecx*4]
            mov ecx, dword ptr [esp + 0x34]
            fxch st(1)
            fadd dword ptr [esp + 0x28]
            fld dword ptr [esp + 0x38]
            fxch st(3)
            add ecx, edx
            add edx, ebx
            mov ebx, esi
            fstp dword ptr [eax + ecx*4]
            fxch st(4)
            fmul dword ptr [esp + 0x10]
            fxch st(3)
            fsub dword ptr [esp + 0x2c]
            fxch st(1)
            fmul st(0), st(5)
            fxch st(4)
            fstp dword ptr [ebp + eax]
            mov ecx, dword ptr [esp + 0x34]
            fxch st(1)
            fchs
            fxch st(1)
            fstp dword ptr [eax + edx*4]
            add ecx, esi
            add esi, edi
            fstp dword ptr [eax + ecx*4]
            shl ebx, 4
            fstp dword ptr [eax + esi*4]
    L005320b7:
            fstp dword ptr [ebx + eax]
            pop edi
            xor ecx, ecx
            pop esi
            pop ebp
            fstp st(0)
            mov dword ptr [eax + 0x2c], ecx
            mov dword ptr [eax + 0x28], ecx
            mov dword ptr [eax + 0x24], ecx
            pop ebx
            add esp, 0x20
            ret
    }
#endif
}

// FUNCTION: CARMA2_HW 0x005320d0
C2_NAKED br_euler* C2_HOOK_CDECL BrMatrix34ToEuler(br_euler* euler, const br_matrix34* mat) {
#ifndef CARPOCALYPSE2_MATCHING
    br_uint_8 o;
    int a0;
    int a1;
    int a2;

    o = euler->order;

    a0 = OrderAxes[o].a0;
    a1 = OrderAxes[o].a1;
    a2 = OrderAxes[o].a2;

    if (o & 0x8) {
        br_scalar sy;

        sy = (br_scalar)sqrt(mat->m[a1][a0] * mat->m[a1][a0] + mat->m[a2][a0] * mat->m[a2][a0]);

        if (sy > 16 * BR_SCALAR_EPSILON) {
            euler->a = BR_ATAN2( mat->m[a1][a0],  mat->m[a2][a0]);
            euler->b = BR_ATAN2( sy,              mat->m[a0][a0]);
            euler->c = BR_ATAN2( mat->m[a0][a1], -mat->m[a0][a2]);
        } else {
            euler->a = BR_ATAN2(-mat->m[a2][a1],  mat->m[a1][a1]);
            euler->b = BR_ATAN2( sy,              mat->m[a0][a0]);
            euler->c = 0;
        }
    } else {
        br_scalar cy;

        cy = (br_scalar)sqrt(mat->m[a0][a0] * mat->m[a0][a0] + mat->m[a0][a1] * mat->m[a0][a1]);

        if (cy > 16 * BR_SCALAR_EPSILON) {
            euler->a = BR_ATAN2( mat->m[a1][a2], mat->m[a2][a2]);
            euler->b = BR_ATAN2(-mat->m[a0][a2], cy);
            euler->c = BR_ATAN2( mat->m[a0][a1], mat->m[a0][a0]);
        } else {
            euler->a = BR_ATAN2(-mat->m[a2][a1], mat->m[a1][a1]);
            euler->b = BR_ATAN2(-mat->m[a0][a2], cy);
            euler->c = 0;
        }
    }

    if (o & 4) {
        euler->a = -euler->a;
        euler->b = -euler->b;
        euler->c = -euler->c;
    }

    if (o & 0x10) {
        br_angle t;

        t  = euler->a;
        euler->a = euler->c;
        euler->c = t;
    }

    return euler;
#else
    __asm {
            sub esp, 0xc
            push ebx
            push ebp
            push esi
            mov esi, dword ptr [esp + 0x1c]
            xor edx, edx
            xor ebx, ebx
            mov cl, byte ptr [esi + 6]
            push edi
            mov byte ptr [esp + 0x10], cl
            mov eax, dword ptr [esp + 0x10]
            and eax, 0xff
            shl eax, 2
            mov dl, byte ptr [eax + _CombineTransforms+0x38]
            mov bl, byte ptr [eax + _CombineTransforms+0x3a]
            mov ebp, edx
            xor edx, edx
            mov dl, byte ptr [eax + _CombineTransforms+0x39]
            test cl, 8
            mov edi, edx
            je L005321db
            lea eax, [ebx + ebx*2]
            lea ecx, [ebp + edi*2]
            mov dword ptr [esp + 0x18], eax
            add edx, ecx
            mov ecx, dword ptr [esp + 0x24]
            add eax, ebp
            fld dword ptr [ecx + eax*4]
            fld dword ptr [ecx + edx*4]
            fst dword ptr [esp + 0x20]
            lea eax, [ecx + eax*4]
            lea edx, [ecx + edx*4]
            fld st(1)
            fxch st(1)
            fmul dword ptr [esp + 0x20]
            fxch st(1)
            fmul st(0), st(2)
            mov dword ptr [esp + 0x14], eax
            faddp st(1), st(0)
            fsqrt
            fstp st(1)
            fcom dword ptr [IID_IDirectInputDevice2A+0x508]
            fnstsw ax
            test ah, 0x41
            jne L005321a7
            mov ecx, dword ptr [esp + 0x14]
            _emit 0xd9
        _emit 0x02
            _emit 0xd9
        _emit 0x01
            fpatan
            fmul qword ptr [IID_IDirectInputDevice2A+0x510]
            call _ftol
            mov edx, ebp
            mov word ptr [esi], ax
            mov eax, dword ptr [esp + 0x24]
            shl edx, 4
            fld dword ptr [edx + eax]
            fpatan
            fmul qword ptr [IID_IDirectInputDevice2A+0x510]
            call _ftol
            lea ebp, [ebp + ebp*2]
            mov word ptr [esi + 2], ax
            mov eax, dword ptr [esp + 0x24]
            add ebx, ebp
            add edi, ebp
            fld dword ptr [eax + edi*4]
            fld dword ptr [eax + ebx*4]
            fchs
            jmp L00532277
    L005321a7:
            mov edx, dword ptr [esp + 0x18]
            lea eax, [edi + edx]
            shl edi, 4
            fld dword ptr [edi + ecx]
            fld dword ptr [ecx + eax*4]
            fchs
            fxch st(1)
            fpatan
            fmul qword ptr [IID_IDirectInputDevice2A+0x510]
            call _ftol
            mov ecx, dword ptr [esp + 0x24]
            mov word ptr [esi], ax
            shl ebp, 4
            fld dword ptr [ebp + ecx]
            jmp L005322bb
    L005321db:
            mov edx, ebp
            lea eax, [ebp + ebp*2]
            mov ebp, dword ptr [esp + 0x24]
            mov dword ptr [esp + 0x20], eax
            shl edx, 4
            add eax, edi
            fld dword ptr [edx + ebp]
            fld dword ptr [ebp + eax*4]
            fst dword ptr [esp + 0x24]
            lea ecx, [edx + ebp]
            lea eax, [ebp + eax*4]
            fld st(1)
            fxch st(1)
            fmul dword ptr [esp + 0x24]
            fxch st(1)
            fmul st(0), st(2)
            mov dword ptr [esp + 0x18], eax
            mov dword ptr [esp + 0x14], ecx
            faddp st(1), st(0)
            fsqrt
            fstp st(1)
            fcom dword ptr [IID_IDirectInputDevice2A+0x508]
            fnstsw ax
            test ah, 0x41
            jne L0053228a
            mov ecx, ebx
            mov edx, edi
            lea eax, [ebx + edi*2]
            shl ecx, 4
            add edx, eax
            fld dword ptr [ecx + ebp]
            fld dword ptr [ebp + edx*4]
            fxch st(1)
            fpatan
            fmul qword ptr [IID_IDirectInputDevice2A+0x510]
            call _ftol
            mov ecx, dword ptr [esp + 0x20]
            mov word ptr [esi], ax
            add ebx, ecx
            fld dword ptr [ebp + ebx*4]
            fchs
            fxch st(1)
            fpatan
            fmul qword ptr [IID_IDirectInputDevice2A+0x510]
            call _ftol
            mov edx, dword ptr [esp + 0x18]
            mov word ptr [esi + 2], ax
            mov eax, dword ptr [esp + 0x14]
            _emit 0xd9
        _emit 0x02
            _emit 0xd9
        _emit 0x00
    L00532277:
            fpatan
            fmul qword ptr [IID_IDirectInputDevice2A+0x510]
            call _ftol
            mov word ptr [esi + 4], ax
            jmp L005322d2
    L0053228a:
            lea ecx, [edi + ebx*2]
            mov edx, ebx
            add edx, ecx
            shl edi, 4
            fld dword ptr [ebp + edx*4]
            fchs
            fld dword ptr [edi + ebp]
            fpatan
            fmul qword ptr [IID_IDirectInputDevice2A+0x510]
            call _ftol
            mov word ptr [esi], ax
            mov eax, dword ptr [esp + 0x20]
            add ebx, eax
            fld dword ptr [ebp + ebx*4]
            fchs
            fxch st(1)
    L005322bb:
            fpatan
            fmul qword ptr [IID_IDirectInputDevice2A+0x510]
            call _ftol
            mov word ptr [esi + 2], ax
            mov word ptr [esi + 4], 0
    L005322d2:
            mov al, byte ptr [esp + 0x10]
            test al, 4
            je L005322f9
            mov cx, word ptr [esi]
            mov dx, word ptr [esi + 2]
            neg cx
            mov word ptr [esi], cx
            mov cx, word ptr [esi + 4]
            neg dx
            neg cx
            mov word ptr [esi + 2], dx
            mov word ptr [esi + 4], cx
    L005322f9:
            test al, 0x10
            je L0053230b
            mov ax, word ptr [esi]
            mov dx, word ptr [esi + 4]
            mov word ptr [esi], dx
            mov word ptr [esi + 4], ax
    L0053230b:
            mov eax, esi
            pop edi
            pop esi
            pop ebp
            pop ebx
            add esp, 0xc
            ret
    }
#endif
}

// FUNCTION: CARMA2_HW 0x00532320
br_matrix4* C2_HOOK_CDECL BrEulerToMatrix4(br_matrix4* mat, const br_euler* euler) {
    br_matrix34 tmp;

    BrEulerToMatrix34(&tmp, euler);
    BrMatrix4Copy34(mat, &tmp);
    return mat;
}

// FUNCTION: CARMA2_HW 0x00532350
br_euler* C2_HOOK_CDECL BrMatrix4ToEuler(br_euler* dest, const br_matrix4* mat) {
    br_matrix34 tmp;

    BrMatrix34Copy4(&tmp, mat);
    return BrMatrix34ToEuler(dest, &tmp);
}

// FUNCTION: CARMA2_HW 0x00532380
C2_NAKED br_quat* C2_HOOK_CDECL BrEulerToQuat(br_quat* q, const br_euler* euler) {
#ifndef CARPOCALYPSE2_MATCHING
    br_uint_8 o;
    br_angle ti;
    br_angle tj;
    br_angle th;
    br_scalar ci;
    br_scalar cj;
    br_scalar ch;
    br_scalar si;
    br_scalar sj;
    br_scalar sh;
    br_scalar cc;
    br_scalar cs;
    br_scalar sc;
    br_scalar ss;
    int a0;
    int a1;
    int a2;

    o = euler->order;
    if (o & 0x10) {
        ti = euler->c / 2;
        tj = euler->b / 2;
        th = euler->a / 2;
    } else {
        ti = euler->a / 2;
        tj = euler->b / 2;
        th = euler->c / 2;
    }
    if (o & 0x4) {
        tj = -tj;
    }

    ci = BR_COS(ti);
    cj = BR_COS(tj);
    ch = BR_COS(th);

    si = BR_SIN(ti);
    sj = BR_SIN(tj);
    sh = BR_SIN(th);

    cc = ci * ch;
    cs = ci * sh;
    sc = si * ch;
    ss = si * sh;

    a0 = OrderAxes[o].a0;
    a1 = OrderAxes[o].a1;
    a2 = OrderAxes[o].a2;

    if (o & 8) {
        ((br_scalar*)q)[a0] = cj * (cs + sc);
        ((br_scalar*)q)[a1] = sj * (cc + ss);
        ((br_scalar*)q)[a2] = sj * (cs - sc);
        q->w = cj * (cc - ss);
    } else {
        ((br_scalar*)q)[a0] = cj * sc - sj * cs;
        ((br_scalar*)q)[a1] = cj * ss + sj * cc;
        ((br_scalar*)q)[a2] = cj * cs - sj * sc;
        q->w = cj * cc + sj * ss;
    }
    if (o & 4) {
        ((br_scalar*)q)[a1] = -((br_scalar*)q)[a1];
    }
    return q;
#else
    __asm {
            sub esp, 0x10
            mov edx, dword ptr [esp + 0x18]
            push ebx
            push esi
            mov bl, byte ptr [edx + 6]
            test bl, 0x10
            mov byte ptr [esp + 0xc], bl
            je L005323a8
            mov ax, word ptr [edx + 4]
            mov cx, word ptr [edx + 2]
            mov si, word ptr [edx]
            shr ax, 1
            shr cx, 1
            jmp L005323b9
    L005323a8:
            mov ax, word ptr [edx]
            mov cx, word ptr [edx + 2]
            mov si, word ptr [edx + 4]
            shr ax, 1
            shr cx, 1
    L005323b9:
            mov dl, bl
            shr si, 1
            and dl, 4
            mov byte ptr [esp + 0x20], dl
            je L005323c9
            neg ecx
    L005323c9:
            and esi, 0xffff
            and eax, 0xffff
            mov dword ptr [esp + 0x10], esi
            mov dword ptr [esp + 0x14], eax
            fild dword ptr [esp + 0x10]
            fild dword ptr [esp + 0x14]
            and ecx, 0xffff
            mov eax, dword ptr [esp + 0xc]
            mov dword ptr [esp + 0x14], ecx
            and eax, 0xff
            fmul qword ptr [IID_IDirectInputDevice2A+0x500]
            fxch st(1)
            fmul qword ptr [IID_IDirectInputDevice2A+0x500]
            fild dword ptr [esp + 0x14]
            fxch st(2)
            fst dword ptr [esp + 0x14]
            fld st(1)
            fld st(2)
            fld dword ptr [esp + 0x14]
            fxch st(5)
            fmul qword ptr [IID_IDirectInputDevice2A+0x500]
            fxch st(2)
            fcos
            shl eax, 2
            xor ecx, ecx
            mov cl, byte ptr [eax + _CombineTransforms+0x38]
            mov esi, ecx
            xor ecx, ecx
            fxch st(3)
            fsin
            fxch st(1)
            fsin
            fxch st(5)
            fcos
            fxch st(2)
            fst dword ptr [esp + 0x14]
            fld dword ptr [esp + 0x14]
            fxch st(3)
            fst dword ptr [esp + 0x14]
            fstp dword ptr [esp + 0x10]
            fcos
            fxch st(2)
            fsin
            fxch st(3)
            fst dword ptr [esp + 0x14]
            fxch st(1)
            fst dword ptr [esp + 0x14]
            fxch st(5)
            fst dword ptr [esp + 0x14]
            fxch st(1)
            fst dword ptr [esp + 0xc]
            fxch st(5)
            fst dword ptr [esp + 8]
            fxch st(1)
            fst dword ptr [esp + 0x14]
            fld dword ptr [esp + 0x14]
            fxch st(3)
            fst dword ptr [esp + 0x14]
            fstp st(5)
            fxch st(5)
            fmul dword ptr [esp + 0x10]
            fxch st(3)
            fstp dword ptr [esp + 0x14]
            fxch st(4)
            fmul dword ptr [esp + 0x10]
            fld dword ptr [esp + 0x14]
            fxch st(5)
            fmul dword ptr [esp + 0xc]
            fxch st(2)
            fmul dword ptr [esp + 8]
            fxch st(3)
            fstp dword ptr [esp + 0x10]
            mov cl, byte ptr [eax + _CombineTransforms+0x39]
            xor edx, edx
            fstp dword ptr [esp + 8]
            mov dl, byte ptr [eax + _CombineTransforms+0x3a]
            fstp dword ptr [esp + 0xc]
            fstp dword ptr [esp + 0x14]
            test bl, 8
            fxch st(1)
            je L0053251a
            fld dword ptr [esp + 0x14]
            fld dword ptr [esp + 0xc]
            fadd dword ptr [esp + 8]
            fld dword ptr [esp + 8]
            fxch st(2)
            fadd dword ptr [esp + 0x10]
            fld dword ptr [esp + 0x10]
            fxch st(3)
            fsub dword ptr [esp + 0xc]
            fxch st(3)
            fsub dword ptr [esp + 0x14]
            fxch st(2)
            fmul st(0), st(5)
            fxch st(1)
            fmul st(0), st(4)
            fxch st(3)
            fmul st(0), st(4)
            fxch st(2)
            fmul st(0), st(5)
            fxch st(1)
            mov eax, dword ptr [esp + 0x1c]
            fstp dword ptr [eax + esi*4]
            fxch st(2)
            fstp dword ptr [eax + ecx*4]
            fstp dword ptr [eax + edx*4]
            jmp L00532575
    L0053251a:
            fld dword ptr [esp + 0xc]
            fld dword ptr [esp + 8]
            fld dword ptr [esp + 0x10]
            fld dword ptr [esp + 0x14]
            fxch st(3)
            fmul st(0), st(5)
            fld dword ptr [esp + 8]
            fxch st(3)
            fmul st(0), st(5)
            fld dword ptr [esp + 0xc]
            fxch st(1)
            fsubp st(2), st(0)
            fxch st(2)
            fmul st(0), st(5)
            fxch st(1)
            mov eax, dword ptr [esp + 0x1c]
            fstp dword ptr [eax + esi*4]
            fxch st(3)
            fmul st(0), st(5)
            fxch st(2)
            fmul st(0), st(5)
            fxch st(2)
            faddp st(3), st(0)
            fmul st(0), st(3)
            fxch st(2)
            fstp dword ptr [eax + ecx*4]
            fld dword ptr [esp + 0x10]
            fmul st(0), st(4)
            fxch st(2)
            fsubp st(1), st(0)
            fld dword ptr [esp + 0x14]
            fmul st(0), st(3)
            fxch st(1)
            fstp dword ptr [eax + edx*4]
            faddp st(1), st(0)
    L00532575:
            fstp dword ptr [eax + 0xc]
            mov dl, byte ptr [esp + 0x20]
            fstp st(0)
            test dl, dl
            fstp st(0)
            je L0053258c
            fld dword ptr [eax + ecx*4]
            fchs
            fstp dword ptr [eax + ecx*4]
    L0053258c:
            pop esi
            pop ebx
            add esp, 0x10
            ret
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

// FUNCTION: CARMA2_HW 0x005325a0
br_euler* C2_HOOK_CDECL BrQuatToEuler(br_euler* euler, const br_quat* q) {
    br_matrix34 mat;

    BrQuatToMatrix34(&mat, q);
    return BrMatrix34ToEuler(euler, &mat);
}
