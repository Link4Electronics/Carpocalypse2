#include "datafile.h"
#include "bswap.h"
#include "diag.h"
#include "file.h"
#include "fwsetup.h"
#include "resource.h"
#include "scratch.h"

#include "core/math/fixed.h"
#include "core/std/brstdlib.h"

#include "c2_stdlib.h"

// GLOBAL: CARMA2_HW 0x006673c8
br_file_primitives _BrFilePrimsNull = {
    "NULL",
    (int (C2_HOOK_STDCALL*)(br_datafile*, br_uint_32)) & BrNullOther,
    (int (C2_HOOK_STDCALL*)(br_datafile*, br_uint_32, br_uint_32)) & BrNullOther,
    (int (C2_HOOK_STDCALL*)(br_datafile*, br_uint_32*)) & BrNullOther,
    (void (C2_HOOK_STDCALL*)(br_datafile*, br_uint_32)) & BrNullOther,
    (br_uint_32(C2_HOOK_STDCALL*)(br_datafile*)) & BrNullOther,
    (int (C2_HOOK_STDCALL*)(br_datafile*)) & BrNullOther,
    (br_uint_32(C2_HOOK_STDCALL*)(br_datafile*, br_file_struct*, void*)) & BrNullOther,
    (br_uint_32(C2_HOOK_STDCALL*)(br_datafile*, br_file_struct*, void*)) & BrNullOther,
    (int (C2_HOOK_STDCALL*)(br_datafile*, br_file_struct*, void*)) & BrNullOther,
    (int (C2_HOOK_STDCALL*)(br_datafile*, void*, int, int, int, int)) & BrNullOther,
    (void* (C2_HOOK_STDCALL*)(br_datafile*, void*, int*, int, int)) & BrNullOther,
    (int (C2_HOOK_STDCALL*)(br_datafile*, void*, int, int, int, int)) & BrNullOther,
    (int (C2_HOOK_STDCALL*)(br_datafile*, const char*)) & BrNullOther,
    (char* (C2_HOOK_STDCALL*)(br_datafile*, char*)) & BrNullOther,
    (int (C2_HOOK_STDCALL*)(br_datafile*, char*)) & BrNullOther,
};

// GLOBAL: CARMA2_HW 0x00667408
br_file_primitives _BrFilePrimsReadBinary = {
    "Read Binary",
    &DfSkipBinary,
    (int (C2_HOOK_STDCALL*)(br_datafile*, br_uint_32, br_uint_32)) & BrNullOther,
    &DfChunkReadBinary,
    (void (C2_HOOK_STDCALL*)(br_datafile*, br_uint_32)) & BrNullOther,
    &DfCountReadBinary,
    &DfCountSizeBinary,
    (br_uint_32(C2_HOOK_STDCALL*)(br_datafile*, br_file_struct*, void*)) & BrNullOther,
    &DfStructReadBinary,
    &DfStructSizeBinary,
    (int (C2_HOOK_STDCALL*)(br_datafile*, void*, int, int, int, int)) & BrNullOther,
    &DfBlockReadBinary,
    &DfBlockSizeBinary,
    (int (C2_HOOK_STDCALL*)(br_datafile*, const char*)) & BrNullOther,
    &DfNameReadBinary,
    &DfNameSizeBinary,
};

// GLOBAL: CARMA2_HW 0x00667448
br_file_primitives _BrFilePrimsWriteBinary = {
    "Write Binary",
    &DfSkipBinary,
    &DfChunkWriteBinary,
    (int (C2_HOOK_STDCALL*)(br_datafile*, br_uint_32*)) & BrNullOther,
    &DfCountWriteBinary,
    (br_uint_32(C2_HOOK_STDCALL*)(br_datafile*)) & BrNullOther,
    &DfCountSizeBinary,
    &DfStructWriteBinary,
    (br_uint_32(C2_HOOK_STDCALL*)(br_datafile*, br_file_struct*, void*)) & BrNullOther,
    &DfStructSizeBinary,
    &DfBlockWriteBinary,
    (void* (C2_HOOK_STDCALL*)(br_datafile*, void*, int*, int, int)) & BrNullOther,
    &DfBlockSizeBinary,
    &DfNameWriteBinary,
    (char* (C2_HOOK_STDCALL*)(br_datafile*, char*)) & BrNullOther,
    &DfNameSizeBinary,
};

// GLOBAL: CARMA2_HW 0x00667488
br_file_primitives _BrFilePrimsReadText = {
    "Read Text",
    &DfSkipText,
    (int (C2_HOOK_STDCALL*)(br_datafile*, br_uint_32, br_uint_32)) & BrNullOther,
    &DfChunkReadText,
    (void (C2_HOOK_STDCALL*)(br_datafile*, br_uint_32)) & BrNullOther,
    &DfCountReadText,
    &DfCountSizeText,
    (br_uint_32(C2_HOOK_STDCALL*)(br_datafile*, br_file_struct*, void*)) & BrNullOther,
    &DfStructReadText,
    &DfStructSizeText,
    (int (C2_HOOK_STDCALL*)(br_datafile*, void*, int, int, int, int)) & BrNullOther,
    &DfBlockReadText,
    &DfBlockSizeText,
    (int (C2_HOOK_STDCALL*)(br_datafile*, const char*)) & BrNullOther,
    &DfNameReadText,
    &DfNameSizeText,
};

// GLOBAL: CARMA2_HW 0x006674c8
br_file_primitives _BrFilePrimsWriteText = {
    "Write Text",
    &DfSkipText,
    &DfChunkWriteText,
    (int (C2_HOOK_STDCALL*)(br_datafile*, br_uint_32*)) & BrNullOther,
    &DfCountWriteText,
    (br_uint_32(C2_HOOK_STDCALL*)(br_datafile*)) & BrNullOther,
    &DfCountSizeText,
    &DfStructWriteText,
    (br_uint_32(C2_HOOK_STDCALL*)(br_datafile*, br_file_struct*, void*)) & BrNullOther,
    &DfStructSizeText,
    &DfBlockWriteText,
    (void* (C2_HOOK_STDCALL*)(br_datafile*, void*, int*, int, int)) & BrNullOther,
    &DfBlockSizeText,
    &DfNameWriteText,
    (char* (C2_HOOK_STDCALL*)(br_datafile*, char*)) & BrNullOther,
    &DfNameSizeText,
};

// GLOBAL: CARMA2_HW 0x00667250
const char* member_type_names[32] = {
    "int_8",
    "uint_8",
    "int_16",
    "uint_16",
    "int_32",
    "uint_32",
    "fixed",
    "angle",
    "float",
    "double",
    "scalar",
    "fraction",
    "ufraction",
    "enum_8",
    "enum_16",
    "enum_32",
    "struct",
    "asciz",
    "colour",
    "vector2",
    "vector3",
    "vector4",
    "fixed_fraction",
    "fixed_ufraction",
    "float_fraction",
    "float_ufraction",
    "fixed_vector2",
    "fixed_vector3",
    "fixed_vector4",
    "float_vector2",
    "float_vector3",
    "float_vector4",
};


// GLOBAL: CARMA2_HW 0x006ad9b0
datafile_stack_t DatafileStack[1024];

#if defined(BRENDER_FIX_BUGS)
#define CHUNKNAMES_NB 62
#else
#define CHUNKNAMES_NB 61
#endif

#ifdef BRENDER_FIX_BUGS
#define PIXELMAP_OLD_STR "PIXELMAP_OLD"
#define MATERIAL_NEW_STR "MATERIAL"
#define PIXELMAP_NEW_STR "PIXELMAP",
#else
#define PIXELMAP_OLD_STR "PIXELMAP"
#define MATERIAL_NEW_STR "MATERIAL_OLD"
#define PIXELMAP_NEW_STR
#endif

// GLOBAL: CARMA2_HW 0x006672d0
const char* ChunkNames[CHUNKNAMES_NB] = {
    "END",
    "IMAGE_PLANE",
    "RLE_IMAGE_PLANE",
    PIXELMAP_OLD_STR,
    "MATERIAL_OLD",
    "ADD_MATERIAL",
    "OLD_ACTOR",
    "OLD_ADD_SIBLING",
    "OLD_ADD_CHILD",
    "OLD_MATERIAL_INDEX",
    "OLD_VERTICES",
    "OLD_VERTICES_UV",
    "OLD_FACES",
    "OLD_MODEL",
    "ADD_MODEL",
    "ANIM",
    "ANIM_TRANSFORM",
    "ANIM_RATE",
    "FILE_INFO",
    "OLD_LIGHT",
    "OLD_CAMERA",
    "PIVOT",
    "MATERIAL_INDEX",
    "VERTICES",
    "VERTEX_UV",
    "OLD_FACES_1",
    "FACE_MATERIAL",
    "OLD_MODEL_1",
    "COLOUR_MAP_REF",
    "OPACITY_MAP_REF",
    "INDEX_BLEND_REF",
    "INDEX_SHADE_REF",
    "SCREENDOOR_REF",
    "PIXELS",
    "ADD_MAP",
    "ACTOR",
    "ACTOR_MODEL",
    "ACTOR_TRANSFORM",
    "ACTOR_MATERIAL",
    "ACTOR_LIGHT",
    "ACTOR_CAMERA",
    "ACTOR_BOUNDS",
    "ACTOR_ADD_CHILD",
    "TRANSFORM_MATRIX34",
    "TRANSFORM_MATRIX34_LP",
    "TRANSFORM_QUAT",
    "TRANSFORM_EULER",
    "TRANSFORM_LOOK_UP",
    "TRANSFORM_TRANSLATION",
    "TRANSFORM_IDENTITY",
    "BOUNDS",
    "LIGHT",
    "CAMERA",
    "FACES",
    "MODEL",
    "ACTOR_CLIP_PLANE",
    "PLANE",
    "SATURN_FACES",
    "SATURN_MODEL",
    "INDEX_FOG_REF",
    MATERIAL_NEW_STR,
    PIXELMAP_NEW_STR
};


// GLOBAL: CARMA2_HW 0x006b09b0
int DatafileStackTop;

#ifdef CARPOCALYPSE2_MATCHING
extern C2_NORETURN void C2_HOOK_CDECL BrFailure();
extern void C2_HOOK_CDECL BrFileClose();
extern int C2_HOOK_CDECL BrFileEof();
extern int C2_HOOK_CDECL BrFileGetLine();
extern int C2_HOOK_CDECL BrFilePrintf();
extern void C2_HOOK_CDECL BrFilePutChar();
extern int C2_HOOK_CDECL BrFileRead();
extern int C2_HOOK_CDECL BrFileWrite();
extern void* C2_HOOK_CDECL BrMemCpy();
extern void* C2_HOOK_CDECL BrResAllocate();
extern void C2_HOOK_CDECL BrResFree();
extern void* C2_HOOK_CDECL BrScratchAllocate();
extern void C2_HOOK_CDECL BrScratchFree();
extern char* C2_HOOK_CDECL BrScratchString();
extern int C2_HOOK_CDECL BrStrCmp();
extern char* C2_HOOK_CDECL BrStrNCpy();
extern long C2_HOOK_CDECL BrStrToL();
extern unsigned long C2_HOOK_CDECL BrStrToUL();
extern br_uint_32 C2_HOOK_CDECL BrSwap32();
extern void* C2_HOOK_CDECL BrSwapBlock();
extern char _diag_scratch[];
extern br_framework_state fw;
#endif
// FUNCTION: CARMA2_HW 0x00529ff0
void C2_HOOK_STDCALL DfPush(int type, void* value, unsigned int count) {

    if (DatafileStackTop >= (int)BR_ASIZE(DatafileStack)) {
        BrFailure("DatafileStack Overflow");
    }
    DatafileStack[DatafileStackTop].type = type;
    DatafileStack[DatafileStackTop].value = value;
    DatafileStack[DatafileStackTop].count = count;
    DatafileStackTop++;
}

// FUNCTION: CARMA2_HW 0x0052a040
void* C2_HOOK_STDCALL DfPop(int type, int* countp) {

    if (DatafileStackTop <= 0) {
        BrFailure("DatafileStack Underflow");
    }
    DatafileStackTop--;
    if (type != DatafileStack[DatafileStackTop].type) {
        BrFailure("DatafileStack type mismatch, wanted %d, got %d", type, DatafileStack[DatafileStackTop].type);
    }
    if (countp != NULL) {
        *countp = DatafileStack[DatafileStackTop].count;
    }
    return DatafileStack[DatafileStackTop].value;
}

// FUNCTION: CARMA2_HW 0x0052a0b0
void* C2_HOOK_STDCALL DfTop(int type, int* countp) {
    if (DatafileStackTop <= 0)
        BrFailure("DatafileStack Underflow");
    if (type != DatafileStack[DatafileStackTop - 1].type)
        BrFailure("DatafileStack type mismatch, wanted %d, got %d", type, DatafileStack[DatafileStackTop].type);
    if (countp != NULL)
        *countp = DatafileStack[DatafileStackTop - 1].count;
    return DatafileStack[DatafileStackTop - 1].value;
}

// FUNCTION: CARMA2_HW 0x0052a120
int C2_HOOK_STDCALL DfTopType(void) {
    if (DatafileStackTop <= 0) {
        return 0;
    }
    return DatafileStack[DatafileStackTop - 1].type;
}

// FUNCTION: CARMA2_HW 0x0052b630
int C2_NAKED C2_HOOK_STDCALL TextReadLine(br_datafile* df, char** ident, char** data) {
#ifndef CARPOCALYPSE2_MATCHING
    char* cp;

    while (1) {
        cp = BrScratchString();
        BrFileGetLine(cp, 256, df->h);
        if (BrFileEof(df->h) != 0) {
            return 0;
        }
        for (; (*cp == ' ') || (*cp == '\t'); cp++) {
        }
        if (*cp != '\0')
            break;
    }
    *ident = cp;
    while ((*cp != ' ') && (*cp != '\t') && (*cp != '\0')) {
        cp++;
    }
    *cp = '\0';
    cp++;
    while ((*cp == ' ') || (*cp == '\t')) {
        cp++;
    }
    *data = cp;
    if (*cp == '\"') {
        cp++;
        while ((*cp != '\0') && (*cp != '\"')) {
            cp++;
        }
    } else {
        while ((*cp != ' ') && (*cp != '\t') && (*cp != '\0')) {
            cp++;
        }
    }
    *cp = '\0';
    return 1;
#else
    __asm {
            push esi
            mov esi, dword ptr [esp + 8]
            mov eax, dword ptr [esi]
            push eax
            push 0x100
            call BrScratchString
            push eax
            call BrFileGetLine
            mov ecx, dword ptr [esi]
            add esp, 0xc
            push ecx
            call BrFileEof
            add esp, 4
            test eax, eax
            jne L0052b698
    L0052b65a:
            call BrScratchString
    L0052b65f:
            mov cl, byte ptr [eax]
            cmp cl, 0x20
            je L0052b66b
            cmp cl, 9
            jne L0052b66e
    L0052b66b:
            inc eax
            jmp L0052b65f
    L0052b66e:
            cmp byte ptr [eax], 0
            jne L0052b69e
            mov edx, dword ptr [esi]
            push edx
            push 0x100
            call BrScratchString
            push eax
            call BrFileGetLine
            mov eax, dword ptr [esi]
            add esp, 0xc
            push eax
            call BrFileEof
            add esp, 4
            test eax, eax
            je L0052b65a
    L0052b698:
            xor eax, eax
            pop esi
            ret 0xc
    L0052b69e:
            mov ecx, dword ptr [esp + 0xc]
            mov dword ptr [ecx], eax
            mov cl, byte ptr [eax]
            cmp cl, 0x20
            je L0052b6bd
    L0052b6ab:
            cmp cl, 9
            je L0052b6bd
            test cl, cl
            je L0052b6bd
            mov cl, byte ptr [eax + 1]
            inc eax
            cmp cl, 0x20
            jne L0052b6ab
    L0052b6bd:
            mov byte ptr [eax], 0
    L0052b6c0:
            mov cl, byte ptr [eax + 1]
            inc eax
            cmp cl, 0x20
            je L0052b6c0
            cmp cl, 9
            je L0052b6c0
            mov edx, dword ptr [esp + 0x10]
            mov dword ptr [edx], eax
            mov cl, byte ptr [eax]
            cmp cl, 0x22
            jne L0052b6fd
            mov cl, byte ptr [eax + 1]
            inc eax
            cmp cl, 0x22
            je L0052b714
    L0052b6e4:
            test cl, cl
            je L0052b714
            mov cl, byte ptr [eax + 1]
            inc eax
            cmp cl, 0x22
            jne L0052b6e4
            mov byte ptr [eax], 0
            mov eax, 1
            pop esi
            ret 0xc
    L0052b6fd:
            cmp cl, 0x20
            je L0052b714
    L0052b702:
            cmp cl, 9
            je L0052b714
            test cl, cl
            je L0052b714
            mov cl, byte ptr [eax + 1]
            inc eax
            cmp cl, 0x20
            jne L0052b702
    L0052b714:
            mov byte ptr [eax], 0
            mov eax, 1
            pop esi
            ret 0xc
    }
#endif
}

br_uint_16 C2_HOOK_STDCALL scalarTypeConvert(br_datafile* df, br_uint_16 t) {

    if (df->scalar_type == BRT_FIXED) {
        switch (t) {
        case DF_TYPE_BR_SCALAR:
            return DF_TYPE_BR_FIXED;
        case DF_TYPE_BR_FRACTION:
            return DF_TYPE_BR_FRACTION_X;
        case DF_TYPE_BR_UFRACTION:
            return DF_TYPE_BR_UFRACTION_X;
        case DF_TYPE_BR_VECTOR2:
            return DF_TYPE_BR_VECTOR2_X;
        case DF_TYPE_BR_VECTOR3:
            return DF_TYPE_BR_VECTOR3_X;
        case DF_TYPE_BR_VECTOR4:
            return DF_TYPE_BR_VECTOR4_X;
        }
    } else if (df->scalar_type == BRT_FLOAT) {
        switch (t) {
        case DF_TYPE_BR_SCALAR:
            return DF_TYPE_FLOAT;
        case DF_TYPE_BR_UFRACTION:
            return DF_TYPE_BR_FRACTION_F;
        case DF_TYPE_BR_FRACTION:
            return DF_TYPE_BR_UFRACTION_F;
        case DF_TYPE_BR_VECTOR2:
            return DF_TYPE_BR_VECTOR2_F;
        case DF_TYPE_BR_VECTOR3:
            return DF_TYPE_BR_VECTOR3_F;
        case DF_TYPE_BR_VECTOR4:
            return DF_TYPE_BR_VECTOR4_F;
        }
    } else if ((t == DF_TYPE_BR_SCALAR) || ((DF_TYPE_BR_VECTOR2 <= t) && (t <= DF_TYPE_BR_VECTOR4))) {
        BrFailure("Incorrect scalar type");
    }
    return t;
}

// FUNCTION: CARMA2_HW 0x0052a140
br_uint_32 C2_HOOK_STDCALL DfStructWriteBinary(br_datafile* df, br_file_struct* str, void* base) {
    unsigned int m;
    int i;
    int n;
    unsigned char* mp;
    br_file_struct_member* sm;
    union { // size: 0x8
        unsigned char b[8]; // @0x0
        float f; // @0x0
        double d; // @0x0
    } conv;

    for (m = 0; m < str->nmembers; m++) {
        sm = &str->members[m];
        mp = ((unsigned char*)base) + sm->offset;

        switch (scalarTypeConvert(df, sm->type)) {
        case DF_TYPE_BR_INT_8:
        case DF_TYPE_BR_UINT_8:
        case DF_TYPE_ENUM_8:
            BrFilePutChar(((br_uint_8*)mp)[0], df->h);
            break;
        case DF_TYPE_BR_ANGLE:
        case DF_TYPE_BR_INT_16:
        case DF_TYPE_BR_UINT_16:
        case DF_TYPE_ENUM_16:
            BrFilePutChar(((br_uint_8*)mp)[1], df->h);
            BrFilePutChar(((br_uint_8*)mp)[0], df->h);
            break;
        case DF_TYPE_BR_INT_32:
        case DF_TYPE_BR_UINT_32:
        case DF_TYPE_ENUM_32:
            BrFilePutChar(((br_uint_8*)mp)[3], df->h);
            BrFilePutChar(((br_uint_8*)mp)[2], df->h);
            BrFilePutChar(((br_uint_8*)mp)[1], df->h);
            BrFilePutChar(((br_uint_8*)mp)[0], df->h);
            break;
        case DF_TYPE_FLOAT:
        case DF_TYPE_BR_FRACTION_F:
        case DF_TYPE_BR_UFRACTION_F:
            conv.f = *(float*)mp;
            BrFilePutChar(conv.b[3], df->h);
            BrFilePutChar(conv.b[2], df->h);
            BrFilePutChar(conv.b[1], df->h);
            BrFilePutChar(conv.b[0], df->h);
            break;
        case DF_TYPE_DOUBLE:
            conv.d = *(double*)mp;
            BrFilePutChar(conv.b[7], df->h);
            BrFilePutChar(conv.b[6], df->h);
            BrFilePutChar(conv.b[5], df->h);
            BrFilePutChar(conv.b[4], df->h);
            BrFilePutChar(conv.b[3], df->h);
            BrFilePutChar(conv.b[2], df->h);
            BrFilePutChar(conv.b[1], df->h);
            BrFilePutChar(conv.b[0], df->h);
            break;
        case DF_TYPE_STRUCT:
            DfStructWriteBinary(df, sm->extra, mp);
            break;
        case DF_TYPE_ASCIZ:
            if (*(char**)mp != NULL) {
                BrFileWrite(*(char**)mp, 1, BrStrLen(*(char**)mp), df->h);
            }
            BrFilePutChar('\0', df->h);
            break;
        case DF_TYPE_BR_COLOUR:
            BrFilePutChar(((br_uint_8*)mp)[2], df->h);
            BrFilePutChar(((br_uint_8*)mp)[1], df->h);
            BrFilePutChar(((br_uint_8*)mp)[0], df->h);
            break;
        case DF_TYPE_BR_FIXED:
            conv.f = BrFixedToFloat(*(br_fixed_ls*)mp);
            BrFilePutChar(conv.b[3], df->h);
            BrFilePutChar(conv.b[2], df->h);
            BrFilePutChar(conv.b[1], df->h);
            BrFilePutChar(conv.b[0], df->h);
            break;
        case DF_TYPE_BR_FRACTION_X:
            conv.f = BrFixedFractionToFloat(*(br_fraction_x*)mp);
            BrFilePutChar(conv.b[3], df->h);
            BrFilePutChar(conv.b[2], df->h);
            BrFilePutChar(conv.b[1], df->h);
            BrFilePutChar(conv.b[0], df->h);
            break;
        case DF_TYPE_BR_UFRACTION_X:
            conv.f = BrFixedUFractionToFloat(*(br_fraction_x*)mp);
            BrFilePutChar(conv.b[3], df->h);
            BrFilePutChar(conv.b[2], df->h);
            BrFilePutChar(conv.b[1], df->h);
            BrFilePutChar(conv.b[0], df->h);
            break;
        case DF_TYPE_BR_VECTOR2_X:
            n = 2;
            goto copy_fixed_vector;
        case DF_TYPE_BR_VECTOR3_X:
            n = 3;
            goto copy_fixed_vector;
        case DF_TYPE_BR_VECTOR4_X:
            n = 4;
        copy_fixed_vector:
            for (i = 0; i < n; i++) {
                conv.f = BrFixedToFloat(*(br_fixed_ls*)mp);
                BrFilePutChar(conv.b[3], df->h);
                BrFilePutChar(conv.b[2], df->h);
                BrFilePutChar(conv.b[1], df->h);
                BrFilePutChar(conv.b[0], df->h);
                mp += sizeof(br_fixed_ls);
            }
            break;
        case DF_TYPE_BR_VECTOR2_F:
            n = 2;
            goto copy_float_vector;
        case DF_TYPE_BR_VECTOR3_F:
            n = 3;
            goto copy_float_vector;
        case DF_TYPE_BR_VECTOR4_F:
            n = 4;
        copy_float_vector:
            for (i = 0; i < n; i++) {
                conv.f = *(float*)mp;
                BrFilePutChar(conv.b[3], df->h);
                BrFilePutChar(conv.b[2], df->h);
                BrFilePutChar(conv.b[1], df->h);
                BrFilePutChar(conv.b[0], df->h);
                mp += sizeof(br_fixed_ls);
            }
            break;
        }
    }
    return 1;
}

// FUNCTION: CARMA2_HW 0x0052a610
br_uint_32 C2_HOOK_STDCALL DfStructReadBinary(br_datafile* df, br_file_struct* str, void* base) {
    char tmp_string[256];
    unsigned int m;
    int i;
    int c;
    int n;
    unsigned char* mp;
    br_file_struct_member* sm;
    union {
        unsigned char b[8];
        float f;
        double d;
    } conv;


    for (m = 0; m < str->nmembers; m++) {
        sm = &str->members[m];

        c = scalarTypeConvert(df, sm->type);
        mp = ((unsigned char*)base) + sm->offset;

        switch (c) {
        case DF_TYPE_BR_INT_8:
        case DF_TYPE_BR_UINT_8:
        case DF_TYPE_ENUM_8:
            mp[0] = BrFileGetChar(df->h);
            break;
        case DF_TYPE_BR_INT_16:
        case DF_TYPE_BR_UINT_16:
        case DF_TYPE_BR_ANGLE:
        case DF_TYPE_ENUM_16:
            mp[1] = BrFileGetChar(df->h);
            mp[0] = BrFileGetChar(df->h);
            break;
        case DF_TYPE_BR_INT_32:
        case DF_TYPE_BR_UINT_32:
        case DF_TYPE_FLOAT:
        case DF_TYPE_ENUM_32:
        case DF_TYPE_BR_FRACTION_F:
        case DF_TYPE_BR_UFRACTION_F:
            mp[3] = BrFileGetChar(df->h);
            mp[2] = BrFileGetChar(df->h);
            mp[1] = BrFileGetChar(df->h);
            mp[0] = BrFileGetChar(df->h);
            break;
        case DF_TYPE_BR_FIXED:
            conv.b[3] = BrFileGetChar(df->h);
            conv.b[2] = BrFileGetChar(df->h);
            conv.b[1] = BrFileGetChar(df->h);
            conv.b[0] = BrFileGetChar(df->h);
            *(br_fixed_ls*)mp = BrFloatToFixed(conv.f);
            break;
        case DF_TYPE_DOUBLE:
            conv.b[7] = BrFileGetChar(df->h);
            conv.b[6] = BrFileGetChar(df->h);
            conv.b[5] = BrFileGetChar(df->h);
            conv.b[4] = BrFileGetChar(df->h);
            conv.b[3] = BrFileGetChar(df->h);
            conv.b[2] = BrFileGetChar(df->h);
            conv.b[1] = BrFileGetChar(df->h);
            conv.b[0] = BrFileGetChar(df->h);
            *(double*)mp = conv.d;
            break;
        case DF_TYPE_STRUCT:
            DfStructReadBinary(df, sm->extra, mp);
            break;
        case DF_TYPE_ASCIZ:
            for (n = 0; n < 255; n++) {
                c = BrFileGetChar(df->h);
                if (c == '\0' || c == -1)
                    break;
                tmp_string[n] = c;
            }
            tmp_string[n] = 0;
            *(char**)mp = (char*)BrResStrDup(df->res ? df->res : fw.res, tmp_string);
            break;
        case DF_TYPE_BR_COLOUR:
            mp[2] = BrFileGetChar(df->h);
            mp[1] = BrFileGetChar(df->h);
            mp[0] = BrFileGetChar(df->h);
            break;
        case DF_TYPE_BR_FRACTION_X:
            conv.b[3] = BrFileGetChar(df->h);
            conv.b[2] = BrFileGetChar(df->h);
            conv.b[1] = BrFileGetChar(df->h);
            conv.b[0] = BrFileGetChar(df->h);
            *((br_fraction_x*)mp) = BrFloatToFixedFraction(conv.f);
            break;
        case DF_TYPE_BR_UFRACTION_X:
            conv.b[3] = BrFileGetChar(df->h);
            conv.b[2] = BrFileGetChar(df->h);
            conv.b[1] = BrFileGetChar(df->h);
            conv.b[0] = BrFileGetChar(df->h);
            *(br_ufraction_x*)mp = BrFloatToFixedUFraction(conv.f);
            break;
        case DF_TYPE_BR_VECTOR2_X:
            n = 2;
            goto copy_fixed_vector;
        case DF_TYPE_BR_VECTOR3_X:
            n = 3;
            goto copy_fixed_vector;
        case DF_TYPE_BR_VECTOR4_X:
            n = 4;
        copy_fixed_vector:
            for (i = 0; i < n; i++) {
                conv.b[3] = BrFileGetChar(df->h);
                conv.b[2] = BrFileGetChar(df->h);
                conv.b[1] = BrFileGetChar(df->h);
                conv.b[0] = BrFileGetChar(df->h);
                *(br_fixed_ls*)mp = BrFloatToFixed(conv.f);
                mp += sizeof(br_fixed_ls);
            }
            break;
        case DF_TYPE_BR_VECTOR2_F:
            n = 2;
            goto copy_float_vector;
            break;
        case DF_TYPE_BR_VECTOR3_F:
            n = 3;
            goto copy_float_vector;
            break;
        case DF_TYPE_BR_VECTOR4_F:
            n = 4;
        copy_float_vector:
            for (i = 0; i < n; i++) {
                conv.b[3] = BrFileGetChar(df->h);
                conv.b[2] = BrFileGetChar(df->h);
                conv.b[1] = BrFileGetChar(df->h);
                conv.b[0] = BrFileGetChar(df->h);
                *(float*)mp = conv.f;
                mp += sizeof(float);
            }
            break;
        }
    }

    return 1;
}

// FUNCTION: CARMA2_HW 0x0052ab30
int C2_HOOK_STDCALL DfStructSizeBinary(br_datafile* df, br_file_struct* str, void* base) {
    unsigned char* mp;
    unsigned int m;
    br_file_struct_member* sm;
    int bytes;

    bytes = 0;
    for (m = 0; m < str->nmembers; m++) {
        sm = &str->members[m];

        mp = ((unsigned char*)base) + sm->offset;

        switch (scalarTypeConvert(df, sm->type)) {
        case DF_TYPE_BR_INT_8:
        case DF_TYPE_BR_UINT_8:
        case DF_TYPE_ENUM_8:
            bytes += 1;
            break;
        case DF_TYPE_BR_INT_16:
        case DF_TYPE_BR_UINT_16:
        case DF_TYPE_BR_ANGLE:
        case DF_TYPE_ENUM_16:
            bytes += 2;
            break;
        case DF_TYPE_BR_COLOUR:
            bytes += 4;
            break;
        case DF_TYPE_BR_INT_32:
        case DF_TYPE_BR_UINT_32:
        case DF_TYPE_FLOAT:
        case DF_TYPE_ENUM_32:
        case DF_TYPE_BR_FRACTION_F:
        case DF_TYPE_BR_UFRACTION_F:
        case DF_TYPE_BR_FIXED:
        case DF_TYPE_BR_FRACTION_X:
        case DF_TYPE_BR_UFRACTION_X:
            bytes += 4;
            break;
        case DF_TYPE_DOUBLE:
            bytes += 8;
            break;
        case DF_TYPE_STRUCT:
            bytes += DfStructSizeBinary(df, sm->extra, mp);
            break;
        case DF_TYPE_ASCIZ:
            if (*(char**)mp != NULL) {
                bytes += BrStrLen(*(char**)mp);
            }
            break;
        case DF_TYPE_BR_VECTOR2_X:
        case DF_TYPE_BR_VECTOR2_F:
            bytes += 8;
            break;
        case DF_TYPE_BR_VECTOR3_X:
        case DF_TYPE_BR_VECTOR3_F:
            bytes += 12;
            break;
        case DF_TYPE_BR_VECTOR4_X:
        case DF_TYPE_BR_VECTOR4_F:
            bytes += 16;
            break;
        }
    }

    return bytes;
}

int C2_HOOK_STDCALL EnumFromString(br_file_enum* e, char* str) {
    unsigned int m;

    if (e == NULL) {
        BrFailure("Unknown enum string: %s", str);
    }
    for (m = 0; m < e->nmembers; m++) {
        if (BrStrCmp(str, e->members[m].name) == 0) {
            return e->members[m].value;
        }
    }
    BrFailure("Unknown enum string: %s", str);
    return -1;
}

char* C2_HOOK_STDCALL EnumToString(br_file_enum* e, int num) {
    unsigned int m;

    if (e == NULL) {
        goto fail;
    }
    for (m = 0; m < e->nmembers; m++) {
        if (e->members[m].value == num) {
            return e->members[m].name;
        }
    }
fail:
    BrFailure("Unknown enum %d", num);
    return NULL;
}

// FUNCTION: CARMA2_HW 0x0052ada0
br_uint_32 C2_HOOK_STDCALL DfStructWriteText(br_datafile* df, br_file_struct* str, void* base) {

    BrFilePrintf(df->h, "  struct    %s\n", str->name);
    StructWriteTextSub(df, str, base, 4);
    return 1;
}

// FUNCTION: CARMA2_HW 0x0052ade0
br_uint_32 C2_HOOK_STDCALL StructWriteTextSub(br_datafile* df, br_file_struct* str, void* base, int indent) {
    unsigned int m;
    int i;
    int w;
    int add_comment;
    void* mp;
    br_file_struct_member* sm;

    for (m = 0; m < str->nmembers; m++) {
        sm = &str->members[m];
        mp = ((unsigned char*)base) + sm->offset;
        w = 0;
        add_comment = 1;

        for (i = 0; i < indent; i++) {
            BrFilePutChar(' ', df->h);
        }

        // Modified from "%-10s", to ensure space after long type names
        BrFilePrintf(df->h, "%-9s ", member_type_names[sm->type]);

        switch (scalarTypeConvert(df, sm->type)) {
        case DF_TYPE_BR_INT_8:
            w = BrFilePrintf(df->h, "%d", *(br_int_8*)mp);
            break;
        case DF_TYPE_BR_UINT_8:
            w = BrFilePrintf(df->h, "%u", *(br_uint_8*)mp);
            break;
        case DF_TYPE_BR_INT_16:
            w = BrFilePrintf(df->h, "%d", *(br_int_16*)mp);
            break;
        case DF_TYPE_BR_UINT_16:
            w = BrFilePrintf(df->h, "%u", *(br_uint_16*)mp);
            break;
        case DF_TYPE_BR_INT_32:
            w = BrFilePrintf(df->h, "%d", *(br_int_32*)mp);
            break;
        case DF_TYPE_BR_UINT_32:
            w = BrFilePrintf(df->h, "%u", *(br_uint_32*)mp);
            break;
        case DF_TYPE_BR_FIXED:
            w = BrFilePrintf(df->h, "%g", BrFixedToFloat(*(br_fixed_ls*)mp));
            break;
        case DF_TYPE_BR_ANGLE:
            w = BrFilePrintf(df->h, "%g", BrFixedToFloat(BrFixedMul(*(br_angle*)mp, BrIntToFixed(360))));
            break;
        case DF_TYPE_FLOAT:
        case DF_TYPE_BR_FRACTION_F:
        case DF_TYPE_BR_UFRACTION_F:
            w = BrFilePrintf(df->h, "%g", (double)*(float*)mp);
            break;
        case DF_TYPE_DOUBLE:
            w = BrFilePrintf(df->h, "%g", *(double*)mp);
            break;
        case DF_TYPE_ENUM_8:
            w = BrFilePrintf(df->h, "%s", EnumToString(sm->extra, *(br_uint_8*)mp));
            break;
        case DF_TYPE_ENUM_16:
            w = BrFilePrintf(df->h, "%s", EnumToString(sm->extra, *(br_uint_16*)mp));
            break;
        case DF_TYPE_ENUM_32:
            w = BrFilePrintf(df->h, "%s", EnumToString(sm->extra, *(br_uint_32*)mp));
            break;
        case DF_TYPE_STRUCT:
            add_comment = 0;
            w = BrFilePrintf(df->h, "%s", ((br_file_struct*)sm->extra)->name);
            if (sm->name != NULL) {
                if (w < 40) {
                    for (i = 40 - w; i != 0; i--) {
                        BrFilePutChar(' ', df->h);
                    }
                }
                BrFilePrintf(df->h, " # %s", sm->name);
            }
            BrFilePutChar('\n', df->h);
            StructWriteTextSub(df, sm->extra, mp, indent + 2);
            break;
        case DF_TYPE_ASCIZ:
            if (*(char**)mp == NULL) {
                w = BrFilePrintf(df->h, "NULL");
            } else {
                w = BrFilePrintf(df->h, "\"%s\"", *(char**)mp);
            }
            break;
        case DF_TYPE_BR_COLOUR:
            w = BrFilePrintf(df->h, "%d,%d,%d", (br_uint_8)((*(br_uint_32*)mp) >> 16), (br_uint_8)((*(br_uint_32*)mp) >> 8), (br_uint_8)((*(br_uint_32*)mp)));
            break;
        case DF_TYPE_BR_FRACTION_X:
            w = BrFilePrintf(df->h, "%g", (double)BrFixedFractionToFloat(*(br_fraction_x*)mp));
            break;
        case DF_TYPE_BR_UFRACTION_X:
            w = BrFilePrintf(df->h, "%g", (double)BrFixedUFractionToFloat(*(br_fraction_x*)mp));
            break;
        case DF_TYPE_BR_VECTOR2_X:
            w = BrFilePrintf(df->h, "%g,%g", BrFixedToFloat(((br_fixed_ls*)mp)[0]), BrFixedToFloat(((br_fixed_ls*)mp)[1]));
            break;
        case DF_TYPE_BR_VECTOR3_X:
            w = BrFilePrintf(df->h, "%g,%g,%g", BrFixedToFloat(((br_fixed_ls*)mp)[0]), BrFixedToFloat(((br_fixed_ls*)mp)[1]), BrFixedToFloat(((br_fixed_ls*)mp)[2]));
            break;
        case DF_TYPE_BR_VECTOR4_X:
            w = BrFilePrintf(df->h, "%g,%g,%g,%g", BrFixedToFloat(((br_fixed_ls*)mp)[0]), BrFixedToFloat(((br_fixed_ls*)mp)[1]), BrFixedToFloat(((br_fixed_ls*)mp)[2]), BrFixedToFloat(((br_fixed_ls*)mp)[3]));
            break;
        case DF_TYPE_BR_VECTOR2_F:
            w = BrFilePrintf(df->h, "%g,%g", ((float*)mp)[0], ((float*)mp)[1]);
            break;
        case DF_TYPE_BR_VECTOR3_F:
            w = BrFilePrintf(df->h, "%g,%g,%g", ((float*)mp)[0], ((float*)mp)[1], ((float*)mp)[2]);
            break;
        case DF_TYPE_BR_VECTOR4_F:
            w = BrFilePrintf(df->h, "%g,%g,%g,%g", ((float*)mp)[0], ((float*)mp)[1], ((float*)mp)[2], ((float*)mp)[3]);
            break;
        }
        if ((add_comment != 0) && (sm->name != NULL)) {
            if (w < 40) {
                for (i = 40 - w; i != 0; i--) {
                    BrFilePutChar(' ', df->h);
                }
            }
            BrFilePrintf(df->h, " # %s\n", sm->name);
        }
    }
    return 1;
}

// FUNCTION: CARMA2_HW 0x0052b5b0
br_uint_32 C2_HOOK_STDCALL DfStructReadText(br_datafile* df, br_file_struct* str, void* base) {
    char* id;
    char* data;

    TextReadLine(df, &id, &data);
    if (BrStrCmp(id, "struct") != 0) {
        BrFailure("Unknown text identifer \"%s\"", id);
    }
    if (BrStrCmp(data, str->name) != 0) {
        BrFailure("Incorrect structure name \"%s\"", data);
    }
    StructReadTextSub(df, str, base);
    return 1;
}

// FUNCTION: CARMA2_HW 0x0052b720
br_uint_32 C2_HOOK_STDCALL StructReadTextSub(br_datafile* df, br_file_struct* str, void* base) {
    unsigned int m;
    unsigned int r;
    unsigned int g;
    unsigned int b;
    int i;
    int n;
    void* mp;
    br_file_struct_member* sm;
    char* id;
    char* data;
    char* ep;

    for (m = 0; m < str->nmembers; m++) {
        sm = &str->members[m];
        mp = ((unsigned char*)base) + sm->offset;

        if (TextReadLine(df, &id, &data) == 0) {
            BrFailure("Unexpected EOF in strructure");
        }
        if (BrStrCmp(id, member_type_names[sm->type]) != 0) {
            BrFailure("Unknown member identifer \"%s\"", id);
        }

        switch (scalarTypeConvert(df, sm->type)) {
        case DF_TYPE_BR_INT_8:
            *(br_int_8*)mp = (br_int_8)BrStrToL(data, NULL, 0);
            break;
        case DF_TYPE_BR_UINT_8:
            *(br_uint_8*)mp = (br_uint_8)BrStrToL(data, NULL, 0);
            break;
        case DF_TYPE_BR_INT_16:
            *(br_int_16*)mp = (br_int_16)BrStrToL(data, NULL, 0);
            break;
        case DF_TYPE_BR_UINT_16:
            *(br_uint_16*)mp = (br_uint_16)BrStrToL(data, NULL, 0);
            break;
        case DF_TYPE_BR_INT_32:
            *(br_int_32*)mp = (br_int_32)BrStrToL(data, NULL, 0);
            break;
        case DF_TYPE_BR_UINT_32:
            *(br_uint_32*)mp = (br_uint_32)BrStrToUL(data, NULL, 0);
            break;
        case DF_TYPE_BR_FIXED:
            *(br_fixed_ls*)mp = BrFloatToFixed(BrStrToD(data, NULL));
            break;
        case DF_TYPE_BR_ANGLE:
            *(br_angle*)mp = BrFixedMulDiv(BrFloatToFixed(BrStrToF(data, NULL)), BrIntToFixed(1), BrIntToFixed(360));
            break;
        case DF_TYPE_FLOAT:
        case DF_TYPE_BR_FRACTION_F:
        case DF_TYPE_BR_UFRACTION_F:
            *(float*)mp = BrStrToF(data, NULL);
            break;
        case DF_TYPE_DOUBLE:
            *(double*)mp = BrStrToD(data, NULL);
            break;
        case DF_TYPE_ENUM_8:
            *(br_uint_8*)mp = EnumFromString(sm->extra, data);
            break;
        case DF_TYPE_ENUM_16:
            *(br_uint_16*)mp = EnumFromString(sm->extra, data);
            break;
        case DF_TYPE_ENUM_32:
            *(br_uint_32*)mp = EnumFromString(sm->extra, data);
            break;
        case DF_TYPE_STRUCT:
            if (BrStrCmp(data, ((br_file_struct*)sm->extra)->name) != 0) {
                BrFailure("Incorrect structure name \"%s\"", data);
            }
            StructReadTextSub(df, sm->extra, mp);
            break;
        case DF_TYPE_ASCIZ:
            if (BrStrCmp(data, "NULL") == 0) {
                *(char**)mp = NULL;
            } else {
                *(char**)mp = BrResStrDup(df->res == NULL ? fw.res : df->res, data + 1);
            }
            break;
        case DF_TYPE_BR_COLOUR:
            r = BrStrToUL(data, &ep, 0);
            if (*ep != ',') {
                BrFailure("Incorrect colour");
            }
            g = BrStrToUL(ep + 1, &ep, 0);
            if (*ep != ',') {
                BrFailure("Incorrect colour");
            }
            b = BrStrToUL(ep + 1, &ep, 0);
            *(br_colour*)mp = BR_COLOUR_RGB(r, g, b);
            break;
        case DF_TYPE_BR_FRACTION_X:
            *(br_fixed_ss*)mp = BrFloatToFixedFraction(BrStrToD(data, NULL));
            break;
        case DF_TYPE_BR_UFRACTION_X:
            *(br_fixed_su*)mp = BrFloatToFixedUFraction(BrStrToD(data, NULL));
            break;
        case DF_TYPE_BR_VECTOR2_X:
            n = 2;
            goto copy_fixed_vector;
        case DF_TYPE_BR_VECTOR3_X:
            n = 3;
            goto copy_fixed_vector;
        case DF_TYPE_BR_VECTOR4_X:
            n = 4;
        copy_fixed_vector:
            for (i = 0; i < n; i++) {
                while (1) {
                    if ((*data != ',') && (*data != ' ') && (*data != '\t')) {
                        break;
                    }
                    data++;
                }
                if (*data == '\0') {
                    BrFailure("Incorrect vector");
                }
                *(br_fixed_ls*)mp = BrFloatToFixed(BrStrToF(data, NULL));
                while ((*data != ' ' && *data != ',' && *data != ' ' && *data != '\t')) {
                    data++;
                }
                mp = (char*)mp + sizeof(br_fixed_ls);
            }
            break;
        case DF_TYPE_BR_VECTOR2_F:
            n = 2;
            goto copy_float_vector;
        case DF_TYPE_BR_VECTOR3_F:
            n = 3;
            goto copy_float_vector;
        case DF_TYPE_BR_VECTOR4_F:
            n = 4;
        copy_float_vector:
            for (i = 0; i < n; i++) {
                while (1) {
                    if ((*data != ',') && (*data != ' ') && (*data != '\t')) {
                        break;
                    }
                    data++;
                }
                if (*data == '\0') {
                    BrFailure("Incorrect vector");
                }
                *(float*)mp = BrStrToF(data, NULL);
                while ((*data != ' ' && *data != ',' && *data != ' ' && *data != '\t')) {
                    data++;
                }
                mp = (char*)mp + sizeof(float);
            }
            break;
        }
    }

    return 1;
}

// FUNCTION: CARMA2_HW 0x0052be60
int C2_NAKED C2_HOOK_STDCALL DfStructSizeText(br_datafile* df, br_file_struct* str, void* base) {
#ifndef CARPOCALYPSE2_MATCHING
    unsigned int m;
    br_file_struct_member* sm;
    int lines;

    lines = 1;
    for (m = 0; m < str->nmembers; m++) {
        sm = &str->members[m];
        if (sm->type == DF_TYPE_STRUCT) {
            lines += DfStructSizeText(df, sm->extra, (br_uint_8*)base + sm->offset);
        } else {
            lines++;
        }
    }
    return lines;
#else
    __asm {
    L0052be60:
            push ebx
            push ebp
            mov ebp, dword ptr [esp + 0x10]
            push esi
            push edi
            xor edi, edi
            mov eax, dword ptr [ebp + 4]
            mov esi, dword ptr [ebp + 8]
            test eax, eax
            mov ebx, 1
            jbe L0052bea7
    L0052be79:
            cmp word ptr [esi], 0x10
            jne L0052be9b
            mov eax, dword ptr [esi + 4]
            mov edx, dword ptr [esp + 0x1c]
            mov ecx, dword ptr [esi + 0xc]
            add eax, edx
            mov edx, dword ptr [esp + 0x14]
            push eax
            push ecx
            push edx
            call L0052be60
            add ebx, eax
            jmp L0052be9c
    L0052be9b:
            inc ebx
    L0052be9c:
            mov eax, dword ptr [ebp + 4]
            inc edi
            add esi, 0x10
            cmp edi, eax
            jb L0052be79
    L0052bea7:
            pop edi
            pop esi
            mov eax, ebx
            pop ebp
            pop ebx
            ret 0xc
    }
#endif
}

// FUNCTION: CARMA2_HW 0x0052beb0
br_uint_32 C2_HOOK_STDCALL DfStructWriteArray(br_datafile* df, br_file_struct* str, void* base, int n) {
    char* cp;
    int i;

    cp = (char*)base;
    for (i = 0; i < n; i++) {
        df->prims->struct_write(df, str, cp);
        cp += str->mem_size;
    }
    return i;
}

// FUNCTION: CARMA2_HW 0x0052bef0
br_uint_32 C2_HOOK_STDCALL DfStructReadArray(br_datafile* df, br_file_struct* str, void* base, int n) {
    char* cp;
    int i;

    cp = (char*)base;
    for (i = 0; i < n; i++) {
        if (BrFileEof(df->h) != 0) {
            break;
        }
        df->prims->struct_read(df, str, cp);
        cp += str->mem_size;
    }
    return i;
}

// FUNCTION: CARMA2_HW 0x0052bf40
int C2_HOOK_STDCALL DfChunkWriteText(br_datafile* df, br_uint_32 id, br_uint_32 length) {

    if (id < BR_ASIZE(ChunkNames)) {
        BrFilePrintf(df->h, "*%-16s %d\n", ChunkNames[id], length);
    } else {
        BrFilePrintf(df->h, "*0x%08x %d\n", id, length);
    }
    return 0;
}

// FUNCTION: CARMA2_HW 0x0052bf90
int C2_HOOK_STDCALL DfChunkReadText(br_datafile* df, br_uint_32* plength) {
    int i;
    char* id;
    char* data;

    if (TextReadLine(df, &id, &data) == 0) {
        return -1;
    }
    if (*id != '*') {
        BrFailure("Chunk ID not found");
    }
    id++;
    if (*id == '0') {
        i = BrStrToUL(id, NULL, 0);
    } else {
        for (i = 0; i < BR_ASIZE(ChunkNames); i++) {
            if (BrStrCmp(ChunkNames[i], id) == 0) {
                break;
            }
        }
        if (i >= BR_ASIZE(ChunkNames)) {
            BrFailure("Chunk ID not known: %s", id);
        }
    }
    if (plength != NULL) {
        *plength = BrStrToUL(data, NULL, 0);
    }
    return i;
}

// FUNCTION: CARMA2_HW 0x0052c050
int C2_HOOK_STDCALL DfChunkWriteBinary(br_datafile* df, br_uint_32 id, br_uint_32 length) {
    br_uint_32 l;

    l = BrSwap32(id);
    BrFileWrite(&l, sizeof(br_uint_32), 1, df->h);
    l = BrSwap32(length);
    BrFileWrite(&l, sizeof(br_uint_32), 1, df->h);
    return 0;
}

// FUNCTION: CARMA2_HW 0x0052c0b0
int C2_HOOK_STDCALL DfChunkReadBinary(br_datafile* df, br_uint_32* plength) {
    br_uint_32 id;
    br_uint_32 l;

    if (BrFileEof(df->h) != 0) {
        return -1;
    }
    BrFileRead(&id, sizeof(br_uint_32), 1, df->h);
    if (BrFileEof(df->h) != 0) {
        return -1;
    }
    id = BrSwap32(id);
    BrFileRead(&l, sizeof(br_uint_32), 1, df->h);
    if (BrFileEof(df->h) != 0) {
        return -1;
    }
    if (plength != NULL) {
        *plength = BrSwap32(l);
    }
    return id;
}

// FUNCTION: CARMA2_HW 0x0052c160
void C2_HOOK_STDCALL DfCountWriteText(br_datafile* df, br_uint_32 count) {

    BrFilePrintf(df->h, " count %d\n", count);
}

// FUNCTION: CARMA2_HW 0x0052c180
br_uint_32 C2_HOOK_STDCALL DfCountReadText(br_datafile* df) {
    char* id;
    char* data;

    TextReadLine(df, &id, &data);
    if (BrStrCmp(id, "count") != 0) {
        BrFailure("no element count for chunk");
    }
    return BrStrToUL(data, NULL, 0);
}

// FUNCTION: CARMA2_HW 0x0052c1d0
void C2_HOOK_STDCALL DfCountWriteBinary(br_datafile* df, br_uint_32 count) {
    br_uint_32 l;

    l = BrSwap32(count);
    BrFileWrite(&l, sizeof(l), 1, df->h);
}

// FUNCTION: CARMA2_HW 0x0052c200
br_uint_32 C2_HOOK_STDCALL DfCountReadBinary(br_datafile* df) {
    br_uint_32 l;

    BrFileRead(&l, sizeof(br_uint_32), 1, df->h);
    return BrSwap32(l);
}

// FUNCTION: CARMA2_HW 0x0052c230
int C2_HOOK_STDCALL DfCountSizeText(br_datafile* df) {

    return 1;
}

// FUNCTION: CARMA2_HW 0x0052c240
int C2_HOOK_STDCALL DfCountSizeBinary(br_datafile* df) {

    return sizeof(br_uint_32);
}

br_uint_8* C2_HOOK_STDCALL BlockWriteSetup(void* base, int block_size, int block_stride, int block_count, int size) {
    abort();
    return NULL;
}

// FUNCTION: CARMA2_HW 0x0052c250
int C2_NAKED C2_HOOK_STDCALL DfBlockWriteText(br_datafile* df, void* base, int block_size, int block_stride, int block_count, int size) {
#ifndef CARPOCALYPSE2_MATCHING
    int i;
    br_uint_8* block;
    int count;

    if (block_stride == block_size) {
        block_size = block_count * block_size;
        block_count = 1;
    }
    block = base;
    if ((size != 1) || (block_count != 1)) {
        block = BrScratchAllocate(block_count * block_size * size);
        for (count = 0; count < block_count; count++) {
            BrMemCpy(block + count * block_size * size, (br_uint_8*)base + count * block_stride, block_size * size);
        }
        BrSwapBlock(block, block_count * block_size, size);
    }
    BrFilePrintf(df->h, "  block %d\n", block_count * block_size);
    BrFilePrintf(df->h, "  size %d\n", size);
    for (i = 0; i < block_count * block_size * size; i++) {
        if ((i & 0x1f) == 0) {
            BrFilePrintf(df->h, "    %08x: %02x", i, block[i]);
        } else {
            BrFilePrintf(df->h, "%02x", block[i]);
        }
        if ((i & 0x1f) == 0x1f) {
            BrFilePutChar('\n', df->h);
        }
    }
    if ((i & 0x1f) != 0x0) {
        BrFilePutChar('\n', df->h);
    }
    if (block != base) {
        BrScratchFree(block);
    }
    return 0;
#else
    __asm {
            sub esp, 8
            mov ecx, dword ptr [esp + 0x14]
            push ebx
            push ebp
            mov ebp, dword ptr [esp + 0x24]
            mov eax, ebp
            mov edx, dword ptr [esp + 0x20]
            imul eax, ecx
            push esi
            cmp edx, ecx
            push edi
            mov dword ptr [esp + 0x10], eax
            mov dword ptr [esp + 0x2c], ecx
            jne L0052c27d
            mov dword ptr [esp + 0x2c], eax
            mov ebp, 1
    L0052c27d:
            mov ebx, dword ptr [esp + 0x30]
            cmp ebx, 1
            jne L0052c290
            cmp ebp, ebx
            jne L0052c290
            mov ebp, dword ptr [esp + 0x20]
            jmp L0052c2f7
    L0052c290:
            mov esi, ebp
            imul esi, dword ptr [esp + 0x2c]
            imul ebx, esi
            push ebx
            mov dword ptr [esp + 0x18], esi
            call BrScratchAllocate
            mov edi, dword ptr [esp + 0x24]
            add esp, 4
            test ebp, ebp
            mov dword ptr [esp + 0x24], eax
            mov ebx, eax
            jle L0052c2dc
            mov esi, dword ptr [esp + 0x2c]
            imul esi, dword ptr [esp + 0x30]
    L0052c2be:
            push esi
            push edi
            push ebx
            call BrMemCpy
            mov edx, dword ptr [esp + 0x34]
            add esp, 0xc
            add edi, edx
            add ebx, esi
            dec ebp
            jne L0052c2be
            mov eax, dword ptr [esp + 0x24]
            mov esi, dword ptr [esp + 0x14]
    L0052c2dc:
            mov ecx, dword ptr [esp + 0x30]
            push ecx
            push esi
            push eax
            call BrSwapBlock
            mov ebp, dword ptr [esp + 0x30]
            mov eax, dword ptr [esp + 0x1c]
            mov ebx, dword ptr [esp + 0x3c]
            add esp, 0xc
    L0052c2f7:
            mov edi, dword ptr [esp + 0x1c]
            push eax
            push offset _BrFilePrimsWriteText+0x814
            mov edx, dword ptr [edi]
            push edx
            call BrFilePrintf
            mov eax, dword ptr [edi]
            add esp, 0xc
            push ebx
            push offset _BrFilePrimsWriteText+0x820
            push eax
            call BrFilePrintf
            mov eax, dword ptr [esp + 0x1c]
            add esp, 0xc
            imul eax, dword ptr [esp + 0x30]
            xor esi, esi
            mov dword ptr [esp + 0x30], eax
            test eax, eax
            jle L0052c38e
    L0052c330:
            mov eax, esi
            cdq
            xor eax, edx
            sub eax, edx
            and eax, 0x1f
            xor eax, edx
            mov ebx, eax
            sub ebx, edx
            jne L0052c35c
            mov edx, dword ptr [edi]
            xor ecx, ecx
            mov cl, byte ptr [ebp + esi]
            push ecx
            push esi
            push offset _BrFilePrimsWriteText+0x82c
            push edx
            call BrFilePrintf
            add esp, 0x10
            jmp L0052c373
    L0052c35c:
            mov ecx, dword ptr [edi]
            xor eax, eax
            mov al, byte ptr [ebp + esi]
            push eax
            push offset _BrFilePrimsWriteText+0x83c
            push ecx
            call BrFilePrintf
            add esp, 0xc
    L0052c373:
            cmp ebx, 0x1f
            jne L0052c385
            mov edx, dword ptr [edi]
            push edx
            push 0xa
            call BrFilePutChar
            add esp, 8
    L0052c385:
            mov eax, dword ptr [esp + 0x30]
            inc esi
            cmp esi, eax
            jl L0052c330
    L0052c38e:
            mov eax, esi
            cdq
            xor eax, edx
            sub eax, edx
            and eax, 0x1f
            xor eax, edx
            sub eax, edx
            je L0052c3ab
            mov eax, dword ptr [edi]
            push eax
            push 0xa
            call BrFilePutChar
            add esp, 8
    L0052c3ab:
            cmp ebp, dword ptr [esp + 0x20]
            je L0052c3ba
            push ebp
            call BrScratchFree
            add esp, 4
    L0052c3ba:
            pop edi
            pop esi
            pop ebp
            xor eax, eax
            pop ebx
            add esp, 8
            ret 0x18
    }
#endif
}

// FUNCTION: CARMA2_HW 0x0052c3d0
C2_NAKED void* C2_HOOK_STDCALL DfBlockReadText(br_datafile* df, void* base, int* count, int size, int mtype) {
#ifndef CARPOCALYPSE2_MATCHING
    char* id;
    char* data;
    int l;
    int s;
    int a;
    char b[3];

    TextReadLine(df, &id, &data);
    if (BrStrCmp(id, "block") != 0) {
        BrFailure("no block");
    }
    l = BrStrToUL(data, NULL, 0);
    TextReadLine(df, &id, &data);
    if (BrStrCmp(id, "size") != 0) {
        BrFailure("no size");
    }
    s = BrStrToUL(data, NULL, 0);
    if (s != size) {
        BrFailure("block size mismatch");
    }
    if (base == NULL) {
        base = BrResAllocate(df->res ? df->res : fw.res, l * size, mtype);
    } else {
        if (*count < l) {
            BrFailure("DfBlockReadText: block too long: %d", l);
        }
    }
    *count = l;
    for (a = 0; a < l * size;) {
        TextReadLine(df, &id, &data);
        if (a != BrStrToL(id, NULL, 16)) {
            BrFailure("block address mismatch");
        }
        while ((data[0] != '\0') && (data[1] != '\0')) {
            b[0] = data[0];
            b[1] = data[1];
            b[2] = '\0';
            ((br_uint_8*)base)[a] = (br_uint_8)BrStrToUL(b, NULL, 16);
            data += 2;
            a++;
        }
    }
    BrSwapBlock(base, l, size);
    return base;
#else
    __asm {
            sub esp, 0xc
            lea eax, [esp]
            lea ecx, [esp + 4]
            push ebx
            push ebp
            mov ebp, dword ptr [esp + 0x18]
            push esi
            push edi
            push eax
            push ecx
            push ebp
            call TextReadLine
            push offset _BrFilePrimsWriteText+0x844
            mov edx, dword ptr [esp + 0x18]
            push edx
            call BrStrCmp
            add esp, 8
            test eax, eax
            je L0052c40e
            push offset _BrFilePrimsWriteText+0x84c
            call BrFailure
            add esp, 4
    L0052c40e:
            mov eax, dword ptr [esp + 0x10]
            push 0
            push 0
            push eax
            call BrStrToUL
            add esp, 0xc
            lea ecx, [esp + 0x10]
            lea edx, [esp + 0x14]
            mov ebx, eax
            push ecx
            push edx
            push ebp
            mov dword ptr [esp + 0x24], ebx
            call TextReadLine
            push offset _BrFilePrimsWriteText+0x858
            mov eax, dword ptr [esp + 0x18]
            push eax
            call BrStrCmp
            add esp, 8
            test eax, eax
            je L0052c458
            push offset _BrFilePrimsWriteText+0x860
            call BrFailure
            add esp, 4
    L0052c458:
            mov ecx, dword ptr [esp + 0x10]
            push 0
            push 0
            push ecx
            call BrStrToUL
            mov ecx, dword ptr [esp + 0x38]
            add esp, 0xc
            cmp eax, ecx
            je L0052c47e
            push offset _BrFilePrimsWriteText+0x868
            call BrFailure
            add esp, 4
    L0052c47e:
            mov edi, dword ptr [esp + 0x24]
            mov esi, dword ptr [esp + 0x28]
            test edi, edi
            jne L0052c4b0
            mov eax, dword ptr [ebp + 0xc]
            test eax, eax
            jne L0052c496
            mov eax, dword ptr [fw+0x438]
    L0052c496:
            mov ecx, ebx
            mov edx, dword ptr [esp + 0x30]
            imul ecx, dword ptr [esp + 0x2c]
            push edx
            push ecx
            push eax
            call BrResAllocate
            add esp, 0xc
            mov edi, eax
            jmp L0052c4c2
    L0052c4b0:
            cmp dword ptr [esi], ebx
            jge L0052c4c2
            push ebx
            push offset _BrFilePrimsWriteText+0x87c
            call BrFailure
            add esp, 8
    L0052c4c2:
            mov dword ptr [esi], ebx
            imul ebx, dword ptr [esp + 0x2c]
            xor esi, esi
            mov byte ptr [esp + 0x22], 0
            test ebx, ebx
            jle L0052c54c
    L0052c4d4:
            lea edx, [esp + 0x10]
            lea eax, [esp + 0x14]
            push edx
            push eax
            push ebp
            call TextReadLine
            push 0x10
            push 0
            mov ecx, dword ptr [esp + 0x1c]
            push ecx
            call BrStrToL
            add esp, 0xc
            cmp esi, eax
            je L0052c506
            push offset _BrFilePrimsWriteText+0x8a0
            call BrFailure
            add esp, 4
    L0052c506:
            mov eax, dword ptr [esp + 0x10]
            mov cl, byte ptr [eax]
            test cl, cl
            je L0052c548
    L0052c510:
            mov dl, byte ptr [eax + 1]
            test dl, dl
            je L0052c548
            mov byte ptr [esp + 0x20], cl
            mov dl, byte ptr [eax + 1]
            push 0x10
            lea eax, [esp + 0x24]
            push 0
            push eax
            mov byte ptr [esp + 0x2d], dl
            call BrStrToUL
            mov byte ptr [esi + edi], al
            mov eax, dword ptr [esp + 0x1c]
            add eax, 2
            add esp, 0xc
            mov dword ptr [esp + 0x10], eax
            inc esi
            mov cl, byte ptr [eax]
            test cl, cl
            jne L0052c510
    L0052c548:
            cmp esi, ebx
            jl L0052c4d4
    L0052c54c:
            mov ecx, dword ptr [esp + 0x2c]
            mov edx, dword ptr [esp + 0x18]
            push ecx
            push edx
            push edi
            call BrSwapBlock
            add esp, 0xc
            mov eax, edi
            pop edi
            pop esi
            pop ebp
            pop ebx
            add esp, 0xc
            ret 0x14
            nop
            nop
            nop
            nop
            nop
    }
#endif
}

// FUNCTION: CARMA2_HW 0x0052c570
int C2_NAKED C2_HOOK_STDCALL DfBlockWriteBinary(br_datafile* df, void* base, int block_size, int block_stride, int block_count, int size) {
#ifndef CARPOCALYPSE2_MATCHING
    int count;
    br_uint_32 l;
    br_uint_32 s;
    void* block;

    l = BrSwap32(block_count * block_size);
    s = BrSwap32(size);
    if (block_stride == block_size) {
        block_size = block_count * block_size;
        block_count = 1;
    }
    block = base;
    if ((size != 1) || (block_count != 1)) {
        block = BrScratchAllocate(block_count * block_size * size);
        for (count = 0; count < block_count; count++) {
            BrMemCpy((br_uint_8*)block + count * block_size * size, (br_uint_8*)base + count * block_stride, block_size * size);
        }
        BrSwapBlock(block, block_count * block_size, size);
    }
    BrFileWrite(&l, sizeof(l), 1, df->h);
    BrFileWrite(&s, sizeof(s), 1, df->h);
    BrFileWrite(block, block_count * block_size, size, df->h);
    if (block != base) {
        BrScratchFree(block);
    }
    return 0;
#else
    __asm {
            sub esp, 0x10
            push ebx
            push ebp
            mov ebp, dword ptr [esp + 0x2c]
            push esi
            push edi
            mov edi, dword ptr [esp + 0x2c]
            mov esi, ebp
            imul esi, edi
            push esi
            mov dword ptr [esp + 0x18], esi
            call BrSwap32
            mov ebx, dword ptr [esp + 0x3c]
            add esp, 4
            mov dword ptr [esp + 0x18], eax
            push ebx
            call BrSwap32
            mov dword ptr [esp + 0x20], eax
            mov eax, dword ptr [esp + 0x34]
            add esp, 4
            cmp eax, edi
            mov dword ptr [esp + 0x34], edi
            jne L0052c5bb
            mov dword ptr [esp + 0x34], esi
            mov ebp, 1
    L0052c5bb:
            cmp ebx, 1
            jne L0052c5ca
            cmp ebp, ebx
            jne L0052c5ca
            mov ebp, dword ptr [esp + 0x28]
            jmp L0052c631
    L0052c5ca:
            mov esi, ebp
            imul esi, dword ptr [esp + 0x34]
            imul ebx, esi
            push ebx
            mov dword ptr [esp + 0x14], esi
            call BrScratchAllocate
            mov edi, dword ptr [esp + 0x2c]
            add esp, 4
            test ebp, ebp
            mov dword ptr [esp + 0x2c], eax
            mov ebx, eax
            jle L0052c616
            mov esi, dword ptr [esp + 0x34]
            imul esi, dword ptr [esp + 0x38]
    L0052c5f8:
            push esi
            push edi
            push ebx
            call BrMemCpy
            mov edx, dword ptr [esp + 0x3c]
            add esp, 0xc
            add edi, edx
            add ebx, esi
            dec ebp
            jne L0052c5f8
            mov eax, dword ptr [esp + 0x2c]
            mov esi, dword ptr [esp + 0x10]
    L0052c616:
            mov ecx, dword ptr [esp + 0x38]
            push ecx
            push esi
            push eax
            call BrSwapBlock
            mov ebp, dword ptr [esp + 0x38]
            mov esi, dword ptr [esp + 0x20]
            mov ebx, dword ptr [esp + 0x44]
            add esp, 0xc
    L0052c631:
            mov edi, dword ptr [esp + 0x24]
            lea eax, [esp + 0x18]
            mov edx, dword ptr [edi]
            push edx
            push 1
            push 4
            push eax
            call BrFileWrite
            mov ecx, dword ptr [edi]
            add esp, 0x10
            lea edx, [esp + 0x1c]
            push ecx
            push 1
            push 4
            push edx
            call BrFileWrite
            mov eax, dword ptr [edi]
            add esp, 0x10
            push eax
            push ebx
            push esi
            push ebp
            call BrFileWrite
            mov eax, dword ptr [esp + 0x38]
            add esp, 0x10
            cmp ebp, eax
            je L0052c67c
            push ebp
            call BrScratchFree
            add esp, 4
    L0052c67c:
            pop edi
            pop esi
            pop ebp
            xor eax, eax
            pop ebx
            add esp, 0x10
            ret 0x18
    }
#endif
}

// FUNCTION: CARMA2_HW 0x0052c690
C2_NAKED void* C2_HOOK_STDCALL DfBlockReadBinary(br_datafile* df, void* base, int* count, int size, int mtype) {
#ifndef CARPOCALYPSE2_MATCHING
    int l;
    int s;

    BrFileRead(&l, 4, 1, df->h);
    l = BrSwap32(l);
    BrFileRead(&s, 4, 1, df->h);
    s = BrSwap32(s);
    if (s != size) {
        BrFailure("block size mismatch");
    }
    if (base != NULL) {
        if (l > *count) {
            BrFailure("DfBlockReadBinary: block too long: %d", l);
        }
    } else {
        base = BrResAllocate(df->res != NULL ? df->res : fw.res, size * l, mtype);
    }
    *count = l;
    BrFileRead(base, l, size, df->h);
    BrSwapBlock(base, l, size);
    return base;
#else
    __asm {
            push ecx
            push ebx
            mov ebx, dword ptr [esp + 0xc]
            push ebp
            push esi
            mov eax, dword ptr [ebx]
            push edi
            push eax
            push 1
            lea ecx, [esp + 0x20]
            push 4
            push ecx
            call BrFileRead
            mov edx, dword ptr [esp + 0x28]
            add esp, 0x10
            push edx
            call BrSwap32
            add esp, 4
            mov dword ptr [esp + 0x18], eax
            mov eax, dword ptr [ebx]
            lea ecx, [esp + 0x10]
            push eax
            push 1
            push 4
            push ecx
            call BrFileRead
            mov edx, dword ptr [esp + 0x20]
            add esp, 0x10
            push edx
            call BrSwap32
            mov edi, dword ptr [esp + 0x28]
            add esp, 4
            cmp eax, edi
            mov dword ptr [esp + 0x10], eax
            je L0052c6f8
            push offset _BrFilePrimsWriteText+0x8b8
            call BrFailure
            add esp, 4
    L0052c6f8:
            mov esi, dword ptr [esp + 0x1c]
            mov ebp, dword ptr [esp + 0x20]
            test esi, esi
            jne L0052c72a
            mov eax, dword ptr [ebx + 0xc]
            test eax, eax
            jne L0052c710
            mov eax, dword ptr [fw+0x438]
    L0052c710:
            mov edx, dword ptr [esp + 0x18]
            mov ecx, dword ptr [esp + 0x28]
            imul edx, edi
            push ecx
            push edx
            push eax
            call BrResAllocate
            add esp, 0xc
            mov esi, eax
            jmp L0052c743
    L0052c72a:
            mov eax, dword ptr [esp + 0x18]
            mov ecx, dword ptr [ebp]
            cmp ecx, eax
            jge L0052c747
            push eax
            push offset _BrFilePrimsWriteText+0x8cc
            call BrFailure
            add esp, 8
    L0052c743:
            mov eax, dword ptr [esp + 0x18]
    L0052c747:
            mov dword ptr [ebp], eax
            mov ecx, dword ptr [ebx]
            push ecx
            push edi
            push eax
            push esi
            call BrFileRead
            mov edx, dword ptr [esp + 0x28]
            add esp, 0x10
            push edi
            push edx
            push esi
            call BrSwapBlock
            add esp, 0xc
            mov eax, esi
            pop edi
            pop esi
            pop ebp
            pop ebx
            pop ecx
            ret 0x14
    }
#endif
}

// FUNCTION: CARMA2_HW 0x0052c780
int C2_NAKED C2_HOOK_STDCALL DfBlockSizeText(br_datafile* df, void* base, int block_size, int block_stride, int block_count, int size) {
#ifndef CARPOCALYPSE2_MATCHING

    return ((size * block_count * block_size + (32 - 1)) >> 5) + 2;
#else
    __asm {
            mov eax, dword ptr [esp + 0x18]
            imul eax, dword ptr [esp + 0x14]
            imul eax, dword ptr [esp + 0xc]
            add eax, 0x1f
            cdq
            and edx, 0x1f
            add eax, edx
            sar eax, 5
            add eax, 2
            ret 0x18
    }
#endif
}

// FUNCTION: CARMA2_HW 0x0052c7a0
int C2_HOOK_STDCALL DfBlockSizeBinary(br_datafile* df, void* base, int block_size, int block_stride, int block_count, int size) {

    return size * block_count * block_size + 8;
}

// FUNCTION: CARMA2_HW 0x0052c7c0
C2_NAKED char* C2_HOOK_STDCALL DfNameReadText(br_datafile* df, char* name) {
#ifndef CARPOCALYPSE2_MATCHING
    char* id;
    char* data;

    TextReadLine(df, &id, &data);
    if (BrStrCmp(id, "name") != 0) {
        BrFailure("no name");
    }
    if ((data != NULL) && (*data == '\"')) {
        BrStrNCpy(name, data + 1, 0xff);
        name[0xff] = '\0';
        return data;
    }
    BrFailure("no name string");
    return NULL;
#else
    __asm {
            push ecx
            mov edx, dword ptr [esp + 8]
            lea eax, [esp + 8]
            push esi
            lea ecx, [esp + 4]
            push eax
            push ecx
            push edx
            call TextReadLine
            push offset _BrFilePrimsWriteText+0x8f4
            mov eax, dword ptr [esp + 8]
            push eax
            call BrStrCmp
            add esp, 8
            test eax, eax
            je L0052c7f9
            push offset _BrFilePrimsWriteText+0x8fc
            call BrFailure
            add esp, 4
    L0052c7f9:
            mov eax, dword ptr [esp + 0xc]
            test eax, eax
            je L0052c806
            cmp byte ptr [eax], 0x22
            je L0052c817
    L0052c806:
            push offset _BrFilePrimsWriteText+0x904
            call BrFailure
            mov eax, dword ptr [esp + 0x10]
            add esp, 4
    L0052c817:
            mov esi, dword ptr [esp + 0x10]
            inc eax
            push 0xff
            push eax
            push esi
            call BrStrNCpy
            add esp, 0xc
            mov byte ptr [esi + 0xff], 0
            mov eax, esi
            pop esi
            pop ecx
            ret 8
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

// FUNCTION: CARMA2_HW 0x0052c840
int C2_HOOK_STDCALL DfNameWriteText(br_datafile* df, const char* name) {

    if (name == NULL) {
        name = "NULL";
    }
    BrFilePrintf(df->h, "  name \"%s\"\n", name);
    return 0;
}

// FUNCTION: CARMA2_HW 0x0052c870
int C2_HOOK_STDCALL DfNameSizeText(br_datafile* df, char* name) {

    return 1;
}

// FUNCTION: CARMA2_HW 0x0052c880
char* C2_HOOK_STDCALL DfNameReadBinary(br_datafile* df, char* name) {
    int c;
    int i;

    for (i = 0; i < 255; i++) {
        c = BrFileGetChar(df->h);
        if ((c == 0) || (c == -1)) {
            break;
        }
        name[i] = c;
    }
    name[i] = '\0';
    return name;
}

// FUNCTION: CARMA2_HW 0x0052c8c0
int C2_HOOK_STDCALL DfNameWriteBinary(br_datafile* df, const char* name) {

    if (name != NULL) {
        BrFileWrite(name, 1, BrStrLen(name), df->h);
    }
    BrFilePutChar('\0', df->h);
    return 0;
}

// FUNCTION: CARMA2_HW 0x0052c900
int C2_HOOK_STDCALL DfNameSizeBinary(br_datafile* df, char* name) {

    if (name != NULL) {
        return BrStrLen(name) + 1;
    }
    return 1;
}

// FUNCTION: CARMA2_HW 0x0052c920
int C2_NAKED C2_HOOK_STDCALL DfSkipText(br_datafile* df, br_uint_32 length) {
#ifndef CARPOCALYPSE2_MATCHING
    char* id;
    char* data;

    while ((BrFileEof(df->h) == 0) && (length != 0)) {
        TextReadLine(df, &data, &id);
        length--;
    }
    return 0;
#else
    __asm {
            push ecx
            push esi
            mov esi, dword ptr [esp + 0xc]
            push edi
            mov eax, dword ptr [esi]
            push eax
            call BrFileEof
            add esp, 4
            test eax, eax
            jne L0052c960
            mov edi, dword ptr [esp + 0x14]
    L0052c93a:
            mov ecx, edi
            dec edi
            test ecx, ecx
            je L0052c960
            lea edx, [esp + 0x10]
            lea eax, [esp + 8]
            push edx
            push eax
            push esi
            call TextReadLine
            mov ecx, dword ptr [esi]
            push ecx
            call BrFileEof
            add esp, 4
            test eax, eax
            je L0052c93a
    L0052c960:
            pop edi
            xor eax, eax
            pop esi
            pop ecx
            ret 8
    }
#endif
}

// FUNCTION: CARMA2_HW 0x0052c970
int C2_HOOK_STDCALL DfSkipBinary(br_datafile* df, br_uint_32 length) {
    BrFileAdvance(length, df->h);
    return 0;
}

// FUNCTION: CARMA2_HW 0x0052c990
int C2_NAKED C2_HOOK_STDCALL DfChunksInterpret(br_datafile* df, br_chunks_table* table) {
#ifndef CARPOCALYPSE2_MATCHING
    br_uint_32 length;
    br_uint_32 count;
    br_uint_32 id;
    int r;
    int i;

    while (1) {
        id = df->prims->chunk_read(df, &length);
        // LOG_DEBUG("chunk id=%d, len=%d", id, length);
        if (id == (br_uint_32)-1) {
            break;
        }
        for (i = 0; i < table->nentries; i++) {
            if (table->entries[i].id == id) {
                break;
            }
        }
        if (i >= table->nentries) {
            df->prims->skip(df, length);
        } else {
            if (table->entries[i].has_count != 0) {
                count = df->prims->count_read(df);
            } else {
                count = 0;
            }
            r = table->entries[i].handler(df, id, length, count);
            if (r != 0) {
                return r;
            }
        }
    }
    return 0;
#else
    __asm {
            push ebx
            push ebp
            push esi
            push edi
            mov edi, dword ptr [esp + 0x14]
            lea ecx, [esp + 0x14]
            push ecx
            push edi
            mov eax, dword ptr [edi + 4]
            call dword ptr [eax + 0xc]
            mov ebx, eax
            cmp ebx, -1
            je L0052ca1b
            mov ebp, dword ptr [esp + 0x18]
    L0052c9af:
            mov ecx, dword ptr [ebp]
            xor eax, eax
            test ecx, ecx
            jle L0052c9c7
            mov edx, dword ptr [ebp + 4]
    L0052c9bb:
            cmp ebx, dword ptr [edx]
            je L0052c9c7
            inc eax
            add edx, 0xc
            cmp eax, ecx
            jl L0052c9bb
    L0052c9c7:
            cmp eax, ecx
            jge L0052c9fc
            mov edx, dword ptr [ebp + 4]
            lea esi, [eax + eax*2]
            shl esi, 2
            mov al, byte ptr [edx + esi + 4]
            test al, al
            je L0052c9e5
            mov eax, dword ptr [edi + 4]
            push edi
            call dword ptr [eax + 0x14]
            jmp L0052c9e7
    L0052c9e5:
            xor eax, eax
    L0052c9e7:
            mov edx, dword ptr [esp + 0x14]
            mov ecx, dword ptr [ebp + 4]
            push eax
            push edx
            push ebx
            push edi
            call dword ptr [ecx + esi + 8]
            test eax, eax
            jne L0052ca1d
            jmp L0052ca08
    L0052c9fc:
            mov ecx, dword ptr [esp + 0x14]
            mov eax, dword ptr [edi + 4]
            push ecx
            push edi
            call dword ptr [eax + 4]
    L0052ca08:
            mov edx, dword ptr [edi + 4]
            lea eax, [esp + 0x14]
            push eax
            push edi
            call dword ptr [edx + 0xc]
            mov ebx, eax
            cmp ebx, -1
            jne L0052c9af
    L0052ca1b:
            xor eax, eax
    L0052ca1d:
            pop edi
            pop esi
            pop ebp
            pop ebx
            ret 8
    }
#endif
}

// FUNCTION: CARMA2_HW 0x0052ca30
void C2_HOOK_STDCALL BrNullOther(void) {

    // Yes, "invalid" is misspelled.
    BrFatal("datafile.c", 1825, "Invald file primitive call");
}

// FUNCTION: CARMA2_HW 0x0052cb40
int C2_HOOK_CDECL DfFileIdentify(const br_uint_8* magics, br_size_t n_magics) {
    static char text_magics[8] = { '*', 'F', 'I', 'L', 'E', '_', 'I', 'N' };
    static char binary_magics[8] = { '\0', '\0', '\0', '\x12', '\0', '\0', '\0', '\b' };

    if (BrMemCmp(magics, text_magics, sizeof(magics)) == 0) {
        return BR_FS_MODE_TEXT;
    }
    if (BrMemCmp(magics, binary_magics, sizeof(magics)) == 0) {
        return BR_FS_MODE_BINARY;
    }
    // file does not match magics
    return BR_FS_MODE_UNKNOWN;
}

// FUNCTION: CARMA2_HW 0x0052ca50
br_datafile* C2_HOOK_STDCALL DfOpen(const char* name, int write, br_token scalar_type) {
    int mode;
    void* h;
    br_datafile* df;
    br_file_primitives* prims;

    mode = fw.open_mode;
    if (write != 0) {
        h = BrFileOpenWrite(name, fw.open_mode);
    } else {
        h = BrFileOpenRead(name, 8u, DfFileIdentify, &mode);
    }
    if (h == NULL) {
        return NULL;
    }
    df = BrResAllocate(fw.res, sizeof(br_datafile), BR_MEMORY_DATAFILE);
    df->h = h;
    df->prims = &_BrFilePrimsNull;
    df->scalar_type = scalar_type;

    switch (mode) {
    case BR_FS_MODE_BINARY:
        if (write) {
            prims = &_BrFilePrimsWriteBinary;
        } else {
            prims = &_BrFilePrimsReadBinary;
        }
        df->prims = prims;
        break;
    case BR_FS_MODE_TEXT:
        if (write) {
            prims = &_BrFilePrimsWriteText;
        } else {
            prims = &_BrFilePrimsReadText;
        }
        df->prims = prims;
        break;
    }
    if (DatafileStackTop >= (int)BR_ASIZE(DatafileStack)) {
        BrFailure("DatafileStack Overflow");
    }
    DatafileStack[DatafileStackTop].type = BR_MEMORY_FILE;
    DatafileStack[DatafileStackTop].value = df;
    DatafileStack[DatafileStackTop].count = 1;
    DatafileStackTop++;
    return df;
}

// FUNCTION: CARMA2_HW 0x0052cb80
void C2_NAKED C2_HOOK_STDCALL DfClose(br_datafile* df) {
#ifndef CARPOCALYPSE2_MATCHING
    br_datafile* dfp;

    while (1) {
        if (DatafileStackTop > 0) {
            if (DatafileStack[DatafileStackTop - 1].type == BR_MEMORY_FILE) {
                break;
            }
            DfPop(DatafileStack[DatafileStackTop - 1].type, NULL);
        }
    }
    dfp = DfPop(BR_MEMORY_FILE, NULL);
    BrFileClose(dfp->h);
    BrResFree(dfp);
#else
    __asm {
            push esi
    L0052cb81:
            mov eax, dword ptr [DatafileStackTop]
    L0052cb86:
            test eax, eax
            jg L0052cb8e
            xor ecx, ecx
            jmp L0052cb98
    L0052cb8e:
            lea ecx, [eax + eax*2]
            mov ecx, dword ptr [ecx*4 + _diag_scratch+0x7c]
    L0052cb98:
            cmp ecx, 0x15
            je L0052cbea
            test eax, eax
            jg L0052cba5
            xor esi, esi
            jmp L0052cbaf
    L0052cba5:
            lea edx, [eax + eax*2]
            mov esi, dword ptr [edx*4 + _diag_scratch+0x7c]
    L0052cbaf:
            test eax, eax
            jg L0052cbc5
            push offset _BrFilePrimsWriteText+0x68
            call BrFailure
            mov eax, dword ptr [DatafileStackTop]
            add esp, 4
    L0052cbc5:
            dec eax
            mov dword ptr [DatafileStackTop], eax
            lea ecx, [eax + eax*2]
            mov ecx, dword ptr [ecx*4 + DatafileStack]
            cmp esi, ecx
            je L0052cb86
            push ecx
            push esi
            push offset _BrFilePrimsWriteText+0x80
            call BrFailure
            add esp, 0xc
            jmp L0052cb81
    L0052cbea:
            test eax, eax
            jg L0052cc00
            push offset _BrFilePrimsWriteText+0x68
            call BrFailure
            mov eax, dword ptr [DatafileStackTop]
            add esp, 4
    L0052cc00:
            dec eax
            mov dword ptr [DatafileStackTop], eax
            lea edx, [eax + eax*2]
            mov eax, dword ptr [edx*4 + DatafileStack]
            cmp eax, 0x15
            je L0052cc25
            push eax
            push 0x15
            push offset _BrFilePrimsWriteText+0x80
            call BrFailure
            add esp, 0xc
    L0052cc25:
            mov esi, dword ptr [esp + 8]
            mov eax, dword ptr [esi]
            push eax
            call BrFileClose
            add esp, 4
            push esi
            call BrResFree
            add esp, 4
            pop esi
            ret 4
    }
#endif
}

// FUNCTION: CARMA2_HW 0x0052cc50
int C2_HOOK_CDECL BrWriteModeSet(int mode) {
    int old;

    old = fw.open_mode;
    fw.open_mode = mode;
    return old;
}
