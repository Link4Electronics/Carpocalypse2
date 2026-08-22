#ifndef REND_COMMON_H_
#define REND_COMMON_H_

/*
 * Shared scaffolding for the glrend/sdl3gpurend renderer drivers.
 *
 * This file is compiled twice — once per GL driver, once per SDL3-GPU driver:
 *   -DBREND_DRIVER_GL            (glrend)  or -DBREND_DRIVER_SDL3GPUREND (sdl3gpurend)
 *   -DBREND_DRIVER_SUFFIX_UPPER=GL  or =SDL3
 *   -DBREND_DRIVER_SUFFIX_LOWER=gl  or =sdl3
 *
 * The suffix is threaded through every symbol so that both drivers keep
 * unique names when they are linked into the same binary.
 */

#define BREND_CAT_(a, b) a##b
#define BREND_CAT(a, b) BREND_CAT_(a, b)

/* BR_CMETHOD class names: br_device_clut_gl / br_device_clut_sdl3 */
#define BREND_CLASS(prefix) BREND_CAT(prefix, BREND_DRIVER_SUFFIX_LOWER)

/* Plain function names: OutputFacility + GL + Init */
#define BREND_FN(prefix, rest) BREND_CAT(BREND_CAT(prefix, BREND_DRIVER_SUFFIX_UPPER), rest)

/* Functions that begin with the suffix: GL + OnScreenCheck */
#define BREND_FNPREFIX(rest) BREND_CAT(BREND_DRIVER_SUFFIX_UPPER, rest)

/*
 * BR_CMETHOD_DECL/REF paste their arguments with ##, which suppresses macro
 * expansion. Go through one extra level so a BREND_CLASS() argument is fully
 * expanded before the pasting happens.
 */
#define BREND_CMETHOD_DECL(cls, name) BREND_CMETHOD_DECL_1(cls, name)
#define BREND_CMETHOD_DECL_1(cls, name) BR_CMETHOD_DECL(cls, name)
#define BREND_CMETHOD_REF(cls, name) BREND_CMETHOD_REF_1(cls, name)
#define BREND_CMETHOD_REF_1(cls, name) BR_CMETHOD_REF(cls, name)

#endif /* REND_COMMON_H_ */
