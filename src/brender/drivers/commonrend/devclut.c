/*
 * Copyright (c) 1993-1995 Argonaut Technologies Limited. All rights reserved.
 *
 * 3Dfx CLUT methods
 */
#include <stddef.h>
#include <string.h>

#include "brassert.h"
#include "drv.h"
#include "commonrend.h"
#include "shortcut.h"

/*
 * Default dispatch table for device_clut (defined at end of file)
 */
static struct br_device_clut_dispatch deviceClutDispatch;

/*
 * Renderer info. template
 */
#define F(f) offsetof(struct br_device_clut, f)

static struct br_tv_template_entry deviceClutTemplateEntries[] = {
    {
        BRT_IDENTIFIER_CSTR,
        0,
        F(identifier),
        BRTV_QUERY | BRTV_ALL,
        BRTV_CONV_COPY,
    },
};
#undef F

/*
 * Create a new device CLUT
 */
br_device_clut* BREND_FN(DeviceClut, Allocate)(br_device* dev, char* identifier) {
    br_device_clut* self;
    int i;

    self = BrResAllocate(dev->res, sizeof(*self), BR_MEMORY_OBJECT);

    self->dispatch = &deviceClutDispatch;
    if (identifier) {
        self->identifier = identifier;
    }
    self->device = dev;

    for (i = 0; i < CLUT_SIZE; i++) {
        self->entries[i] = BR_COLOUR_RGB(i, i, i);
    }

    ObjectContainerAddFront(dev, (br_object*)self);

    return self;
}

static void BREND_CMETHOD_DECL(BREND_CLASS(br_device_clut_), free)(br_device_clut* self) {
    ObjectContainerRemove(ObjectDevice(self), (br_object*)self);

    BrResFreeNoCallback(self);
}

static br_token BREND_CMETHOD_DECL(BREND_CLASS(br_device_clut_), type)(br_device_clut* self) {
    return BRT_DEVICE_CLUT;
}

static br_boolean BREND_CMETHOD_DECL(BREND_CLASS(br_device_clut_), isType)(br_device_clut* self, br_token t) {
    return (t == BRT_DEVICE_CLUT) || (t == BRT_OBJECT);
}

static br_int_32 BREND_CMETHOD_DECL(BREND_CLASS(br_device_clut_), space)(br_device_clut* self) {
    return sizeof(br_device_clut);
}

static struct br_tv_template* BREND_CMETHOD_DECL(BREND_CLASS(br_device_clut_), queryTemplate)(br_device_clut* self) {
    if (self->device->templates.deviceClutTemplate == NULL)
        self->device->templates.deviceClutTemplate = BrTVTemplateAllocate(self->device,
            deviceClutTemplateEntries,
            BR_ASIZE(deviceClutTemplateEntries));

    return self->device->templates.deviceClutTemplate;
}

static br_error BREND_CMETHOD_DECL(BREND_CLASS(br_device_clut_), entrySet)(br_device_clut* self, br_int_32 index, br_colour entry) {
    if (index < 0 || index >= CLUT_SIZE)
        return BRE_OVERFLOW;

    if (self->entries[index] != entry) {
        self->revision++;
        self->entries[index] = entry;
    }

    return BRE_OK;
}

static br_error BREND_CMETHOD_DECL(BREND_CLASS(br_device_clut_), entryQuery)(br_device_clut* self, br_colour* entry, br_int_32 index) {
    if (index < 0 || index >= CLUT_SIZE)
        return BRE_OVERFLOW;

    *entry = self->entries[index];

    return BRE_OK;
}

static br_error BREND_CMETHOD_DECL(BREND_CLASS(br_device_clut_), entrySetMany)(br_device_clut* self, br_int_32 index, br_int_32 count, br_colour* entries) {
    int i;
    int changed = 0;

    if (index < 0 || index >= CLUT_SIZE)
        return BRE_OVERFLOW;

    if (index + count > CLUT_SIZE)
        return BRE_OVERFLOW;

    for (i = 0; i < count; i++) {
        if (self->entries[index + i] != entries[i]) {
            changed = 1;
            self->entries[index + i] = entries[i];
        }
    }

    if (changed) {
        self->revision++;
    }

    return BRE_OK;
}

static br_error BREND_CMETHOD_DECL(BREND_CLASS(br_device_clut_), entryQueryMany)(br_device_clut* self, br_colour* entries, br_int_32 index, br_int_32 count) {
    int i;

    if (index < 0 || index >= CLUT_SIZE)
        return BRE_OVERFLOW;

    if (index + count > CLUT_SIZE)
        return BRE_OVERFLOW;

    for (i = 0; i < count; i++)
        entries[i] = self->entries[index + i];

    return BRE_OK;
}

static const char* BREND_CMETHOD_DECL(BREND_CLASS(br_device_clut_), identifier)(br_object* self) {
    return ((br_device_clut*)self)->identifier;
}

br_device* BREND_CMETHOD_DECL(BREND_CLASS(br_device_clut_), device)(br_object* self) {
    return ((br_device_clut*)self)->device;
}

/*
 * Default dispatch table for device CLUT
 */
static struct br_device_clut_dispatch deviceClutDispatch = {
    NULL,
    NULL,
    NULL,
    NULL,
    BREND_CMETHOD_REF(BREND_CLASS(br_device_clut_), free),
    BREND_CMETHOD_REF(BREND_CLASS(br_device_clut_), identifier),
    BREND_CMETHOD_REF(BREND_CLASS(br_device_clut_), type),
    BREND_CMETHOD_REF(BREND_CLASS(br_device_clut_), isType),
    BREND_CMETHOD_REF(BREND_CLASS(br_device_clut_), device),
    BREND_CMETHOD_REF(BREND_CLASS(br_device_clut_), space),

    BREND_CMETHOD_REF(BREND_CLASS(br_device_clut_), queryTemplate),
    BR_CMETHOD_REF(br_object, query),
    BR_CMETHOD_REF(br_object, queryBuffer),
    BR_CMETHOD_REF(br_object, queryMany),
    BR_CMETHOD_REF(br_object, queryManySize),
    BR_CMETHOD_REF(br_object, queryAll),
    BR_CMETHOD_REF(br_object, queryAllSize),

    BREND_CMETHOD_REF(BREND_CLASS(br_device_clut_), entrySet),
    BREND_CMETHOD_REF(BREND_CLASS(br_device_clut_), entryQuery),
    BREND_CMETHOD_REF(BREND_CLASS(br_device_clut_), entrySetMany),
    BREND_CMETHOD_REF(BREND_CLASS(br_device_clut_), entryQueryMany),
};
