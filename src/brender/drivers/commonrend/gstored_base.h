/*
 * Shared private geometry-stored structure for the glrend/sdl3gpurend drivers.
 * Compiled once per driver.
 */
#ifndef REND_GSTORED_BASE_H_
#define REND_GSTORED_BASE_H_

/*
 * Common br_geometry_stored header fields, identical across drivers. Each
 * driver extends this with its backend-specific GPU handles and group info.
 */
#define BR_GEOMETRY_STORED_BASE \
    const struct br_geometry_stored_dispatch* dispatch; \
    const char* identifier; \
    struct br_device* device; \
    struct br_geometry_v1_model* gv1model; \
    br_boolean shared; \
    struct v11model* model

#endif /* REND_GSTORED_BASE_H_ */
