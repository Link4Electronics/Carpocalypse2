/*
 * Stored buffer methods
 */
#include <stddef.h>

#include "brassert.h"
#include "drv.h"

/*
 * Set up a static device object
 */
void BufferStoredGLInitFields(struct br_buffer_stored* self) {
    self->gl_tex = 0;
}

static br_boolean is_compatible(br_buffer_stored* self, br_pixelmap* pm, GLenum internal_format) {
    if (self->source == NULL)
        return BR_FALSE;

    if (self->gl_tex == 0 || self->gl_internal_format == 0 || self->gl_format == 0 || self->gl_type == 0)
        return BR_FALSE;

    if (self->source->width != pm->width || self->source->height != pm->height)
        return BR_FALSE;

    if (self->gl_internal_format != internal_format)
        return BR_FALSE;

    return BR_TRUE;
}

static const char* gl_strerror(GLenum err) {
    static char errbuf[64];
    const char* s;

    switch (err) {
    case 0:
        s = "GL_NO_ERROR";
        break;
    case GL_INVALID_ENUM:
        s = "GL_INVALID_ENUM";
        break;
    case GL_INVALID_VALUE:
        s = "GL_INVALID_VALUE";
        break;
    case GL_INVALID_OPERATION:
        s = "GL_INVALID_OPERATION";
        break;
    case GL_INVALID_FRAMEBUFFER_OPERATION:
        s = "GL_INVALID_FRAMEBUFFER_OPERATION";
        break;
    case GL_OUT_OF_MEMORY:
        s = "GL_OUT_OF_MEMORY";
        break;
    default:
        s = NULL;
    }

    if (s != NULL)
        BrSprintfN(errbuf, sizeof(errbuf) - 1, "error %d (%s)", err, s);
    else
        BrSprintfN(errbuf, sizeof(errbuf) - 1, "error %d", err);

    errbuf[sizeof(errbuf) - 1] = '\0';
    return errbuf;
}

#include <stdio.h>
static br_error updateMemory(br_buffer_stored* self, br_pixelmap* pm) {
    GLint internal_format;
    GLenum format, type, err;
    GLsizeiptr elem_bytes;
    br_error r;

    /*
     * The pixelmap is a plain BRender memory pixelmap. Make sure that the pixels can be accessed
     */
    if ((pm->flags & BR_PMF_NO_ACCESS) || pm->pixels == NULL)
        return BRE_FAIL;

    r = VIDEOI_BrPixelmapGetTypeDetails(pm->type, &internal_format, &format, &type, &elem_bytes, &self->blended);
    if (r != BRE_OK)
        return r;

    /*
     * If we're compatible, update the existing texture.
     */
    if (is_compatible(self, pm, internal_format) == BR_TRUE) {
        ASSERT(self->gl_tex != 0);

        BR_FATAL0("updateMemory path not implemented\n");

        return BRE_FAIL;
    }

    if (self->gl_tex == 0) {
        glGenTextures(1, &self->gl_tex);
        glBindTexture(GL_TEXTURE_2D, self->gl_tex);
        glTexImage2D(GL_TEXTURE_2D, 0, internal_format, pm->width, pm->height, 0, format, type, NULL);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

        if ((err = glGetError()) != 0) {
            BR_ERROR1("GLREND: glGenTextures() failed with %s", gl_strerror(err));
            return BRE_FAIL;
        }
    }

    //BR_WARNING("updateMemory");

    glBindTexture(GL_TEXTURE_2D, self->gl_tex);
    self->paletted_source_dirty = BR_FALSE;
    if (pm->type == BR_PMT_INDEX_8) {
        // if paletted, then wait until we are displaying the texture to convert it to 32 bit
        self->paletted_source_dirty = BR_TRUE;
        self->palette_pointer = ObjectDevice(self)->clut;
    } else if (pm->type == BR_PMT_RGB_565) {
        glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, pm->width, pm->height, format, type, pm->pixels);
    } else {
        return BRE_FAIL;
    }

    if ((err = glGetError()) != 0) {
        BR_FATAL1("GLREND: glTexImage2D() failed with %s", gl_strerror(err));
        glBindTexture(GL_TEXTURE_2D, 0);
        return BRE_FAIL;
    }

    glGenerateMipmap(GL_TEXTURE_2D);

    self->source = pm;
    self->source_flags = pm->flags;

    glBindTexture(GL_TEXTURE_2D, 0);
    GL_CHECK_ERROR();
    return BRE_OK;
}

br_error BREND_CMETHOD_DECL(BREND_CLASS(br_buffer_stored_), update)(struct br_buffer_stored* self,
    struct br_device_pixelmap* pm, br_token_value* tv) {
    br_device* pm_device;
    (void)tv;

    if (pm->pm_type != BR_PMT_INDEX_8 && pm->pm_type != BR_PMT_RGB_565) {
        return BRE_FAIL;
    }

    /*
     * Find out where the pixelmap comes from
     */
    pm_device = ObjectDevice(pm);
    if (pm_device == NULL) {
        return updateMemory(self, (br_pixelmap*)pm);
    } else if (pm_device == self->device) {
        ASSERT(self->source == NULL || self->source == pm);
        self->gl_tex = 0; /* Unused for us. */
        self->source = (br_pixelmap*)pm;
        self->source_flags = pm->pm_flags;
        return BRE_OK;
    } else {
        /*
         * The pixelmap is from another device, we can't use it
         */
        return BRE_FAIL;
    }
}

void BREND_CMETHOD_DECL(BREND_CLASS(br_buffer_stored_), free)(br_object* _self) {
    br_buffer_stored* self = (br_buffer_stored*)_self;

    glDeleteTextures(1, &self->gl_tex);
    self->gl_tex = 0;

    ObjectContainerRemove(self->renderer, (br_object*)self);
}

GLuint BufferStoredGLGetTexture(br_buffer_stored* self) {
    if (self->source == NULL)
        return self->gl_tex;

    if (ObjectDevice(self->source) == self->device) {
        br_device_pixelmap* pm = (br_device_pixelmap*)self->source;
        switch (pm->use_type) {
        case BRT_NONE:
        default:
            return 0;

        case BRT_OFFSCREEN:
            return pm->asBack.glTex;

        case BRT_DEPTH:
            return pm->asDepth.glDepth;
        }
    }

    return self->gl_tex;
}
