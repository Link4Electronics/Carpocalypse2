/*
 * Copyright (c) 1992,1993-1995 Argonaut Technologies Limited. All rights reserved.
 *
 * $Id: tokenmap.h 1.1 1997/12/10 16:41:11 jon Exp $
 * $Locker: $
 *
 */
#ifndef _TOKENMAP_H_
#define _TOKENMAP_H_

/*
 * Template structure used to map token/values to a structure
 */
/* br_tv_template_entry / br_tv_template come from our br_types.h */

/*
 * Defined operations for Query/Set conversions
 *
 * In two part names: token value on left, struct value on right
 */
enum {
	BRTV_CONV_NOP,

	BRTV_CONV_DIRECT,

	BRTV_CONV_CUSTOM,

	BRTV_CONV_COPY,

	BRTV_CONV_V2_COPY,
	BRTV_CONV_V3_COPY,
	BRTV_CONV_V4_COPY,
	BRTV_CONV_M23_COPY,
	BRTV_CONV_M34_COPY,
	BRTV_CONV_M4_COPY,

	BRTV_CONV_I32_I8,
	BRTV_CONV_I32_I16,

	BRTV_CONV_I32_U8,
	BRTV_CONV_I32_U16,

	BRTV_CONV_FIXED_FLOAT,
	BRTV_CONV_FLOAT_FIXED,

	BRTV_CONV_V2_FIXED_FLOAT,
	BRTV_CONV_V2_FLOAT_FIXED,

	BRTV_CONV_V3_FIXED_FLOAT,
	BRTV_CONV_V3_FLOAT_FIXED,

	BRTV_CONV_V4_FIXED_FLOAT,
	BRTV_CONV_V4_FLOAT_FIXED,

	BRTV_CONV_M23_FIXED_FLOAT,
	BRTV_CONV_M23_FLOAT_FIXED,

	BRTV_CONV_M34_FIXED_FLOAT,
	BRTV_CONV_M34_FLOAT_FIXED,

	BRTV_CONV_M4_FIXED_FLOAT,
	BRTV_CONV_M4_FLOAT_FIXED,

	BRTV_CONV_STRING,

	BRTV_CONV_LIST,

	BRTV_CONV_BOOL_TOKEN,

	BRTV_CONV_BIT,
	BRTV_CONV_NOT_BIT,

#ifndef CARPOCALYPSE2_MATCHING
	/*
	 * Port-only: BRTV_CONV_COPY stores a 32-bit word, which truncates the
	 * pointers the hardware drivers receive through their templates on 64-bit
	 * hosts (BRT_OPENGL_CALLBACKS_P, BRT_SDL3GPU_CALLBACKS_P, the hidden-state
	 * V1*_P pointers). tokenval.c implements this conversion as a full-width
	 * pointer copy; the original 32-bit binary never needed it.
	 */
	BRTV_CONV_POINTER = 33,
#endif
};

/*
 * Booleans are 32 bit at the moment
 */
#define	BRTV_CONV_I32_B  BRTV_CONV_COPY

/*
 * Typedef for custom conversion function
 */
/* br_tv_custom and its callbacks come from our br_types.h */

/*
 * Define some operations in terms of the current scalar type
 */
#if BASED_FLOAT
#define BRTV_CONV_FIXED_SCALAR		BRTV_CONV_FIXED_FLOAT
#define BRTV_CONV_FLOAT_SCALAR		BRTV_CONV_COPY

#define BRTV_CONV_V2_FIXED_SCALAR	BRTV_CONV_V2_FIXED_FLOAT
#define BRTV_CONV_V2_FLOAT_SCALAR	BRTV_CONV_V2_COPY

#define BRTV_CONV_V3_FIXED_SCALAR	BRTV_CONV_V3_FIXED_FLOAT
#define BRTV_CONV_V3_FLOAT_SCALAR	BRTV_CONV_V3_COPY

#define BRTV_CONV_V4_FIXED_SCALAR	BRTV_CONV_V4_FIXED_FLOAT
#define BRTV_CONV_V4_FLOAT_SCALAR	BRTV_CONV_V4_COPY

#define BRTV_CONV_M23_FIXED_SCALAR	BRTV_CONV_M23_FIXED_FLOAT
#define BRTV_CONV_M23_FLOAT_SCALAR	BRTV_CONV_M23_COPY

#define BRTV_CONV_M34_FIXED_SCALAR	BRTV_CONV_M34_FIXED_FLOAT
#define BRTV_CONV_M34_FLOAT_SCALAR	BRTV_CONV_M34_COPY

#define BRTV_CONV_M4_FIXED_SCALAR	BRTV_CONV_M4_FIXED_FLOAT
#define BRTV_CONV_M4_FLOAT_SCALAR	BRTV_CONV_M4_COPY
#endif

#if BASED_FIXED
#define BRTV_CONV_FIXED_SCALAR		BRTV_CONV_COPY
#define BRTV_CONV_FLOAT_SCALAR		BRTV_CONV_FLOAT_FIXED

#define BRTV_CONV_V2_FIXED_SCALAR	BRTV_CONV_V2_COPY
#define BRTV_CONV_V2_FLOAT_SCALAR	BRTV_CONV_V2_FLOAT_FIXED

#define BRTV_CONV_V3_FIXED_SCALAR	BRTV_CONV_V3_COPY
#define BRTV_CONV_V3_FLOAT_SCALAR	BRTV_CONV_V3_FLOAT_FIXED

#define BRTV_CONV_V4_FIXED_SCALAR	BRTV_CONV_V4_COPY
#define BRTV_CONV_V4_FLOAT_SCALAR	BRTV_CONV_V4_FLOAT_FIXED

#define BRTV_CONV_M23_FIXED_SCALAR	BRTV_CONV_M23_COPY
#define BRTV_CONV_M23_FLOAT_SCALAR	BRTV_CONV_M23_FLOAT_FIXED

#define BRTV_CONV_M34_FIXED_SCALAR	BRTV_CONV_M34_COPY
#define BRTV_CONV_M34_FLOAT_SCALAR	BRTV_CONV_M34_FLOAT_FIXED

#define BRTV_CONV_M4_FIXED_SCALAR	BRTV_CONV_M4_COPY
#define BRTV_CONV_M4_FLOAT_SCALAR	BRTV_CONV_M4_FLOAT_FIXED
#endif

/*
 * Conversion flags
 */
enum br_tv_flag {
	BRTV_QUERY		= 0x0001,	/* This entry is valid for Query operations	*/
	BRTV_SET		= 0x0002,	/* This entry is valid for Set operations	*/
	BRTV_ALL		= 0x0004,	/* This entry is valid for QueryAll operations	*/

	BRTV_ABS		= 0x0008	/* 'offset' member of template contains value	*/
};

#endif
