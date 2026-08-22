# Shared CMake scaffolding for the glrend/sdl3gpurend drivers.
#
# Both drivers compile the same drivers/commonrend sources under different
# driver defines (BREND_DRIVER_GL vs BREND_DRIVER_SDL3GPUREND). This file keeps
# those shared source lists and the target-level boilerplate in one place.

# Sets BR_COMMON_STATE_FILES, BR_COMMON_RENDERER_FILES and BR_COMMON_OBJ_FILES
# (the commonrend sources compiled by both drivers) plus the "STATE" source
# group. A macro, so the lists land in the caller's scope where each driver
# appends its own per-driver files.
macro(brender_common_sources REND_COMMON_DIR)
    set(BR_COMMON_STATE_FILES
        ${REND_COMMON_DIR}/cache.c
        ${REND_COMMON_DIR}/modeldata.c
        ${REND_COMMON_DIR}/modelrender.c
        ${REND_COMMON_DIR}/gstored.c
        ${REND_COMMON_DIR}/state.c
        ${REND_COMMON_DIR}/state.h
        ${REND_COMMON_DIR}/state_clip.c
        ${REND_COMMON_DIR}/state_cull.c
        ${REND_COMMON_DIR}/state_hidden.c
        ${REND_COMMON_DIR}/state_light.c
        ${REND_COMMON_DIR}/state_matrix.c
        ${REND_COMMON_DIR}/state_output.c
        ${REND_COMMON_DIR}/state_primitive.c
        ${REND_COMMON_DIR}/state_surface.c
        )
    source_group("STATE" FILES ${BR_COMMON_STATE_FILES})

    set(BR_COMMON_RENDERER_FILES
        ${REND_COMMON_DIR}/gv1model.c
        ${REND_COMMON_DIR}/gv1model.h
        ${REND_COMMON_DIR}/onscreen.c
        ${REND_COMMON_DIR}/renderer.c
        ${REND_COMMON_DIR}/renderer.h
        ${REND_COMMON_DIR}/sbuffer_common.c
        ${REND_COMMON_DIR}/pixconv.c
        ${REND_COMMON_DIR}/pixconv.h
        ${REND_COMMON_DIR}/sstate.c
        ${REND_COMMON_DIR}/sstate.h
        )

    set(BR_COMMON_OBJ_FILES
        ${REND_COMMON_DIR}/driver.c
        ${REND_COMMON_DIR}/outfcty.c
        ${REND_COMMON_DIR}/outfcty.h
        ${REND_COMMON_DIR}/ext_procs.c
        ${REND_COMMON_DIR}/rendfcty.c
        ${REND_COMMON_DIR}/rendfcty.h
        ${REND_COMMON_DIR}/device.c
        ${REND_COMMON_DIR}/device.h
        ${REND_COMMON_DIR}/devpixmp.c
        ${REND_COMMON_DIR}/devpixmp_base.h
        ${REND_COMMON_DIR}/devclut.c
        ${REND_COMMON_DIR}/devclut.h
        ${REND_COMMON_DIR}/template.h
        ${REND_COMMON_DIR}/gstored_base.h
        )
endmacro()

# Applies the shared target boilerplate: entry-point define for shared builds,
# the driver suffix defines, private include dirs, and the BRender::DDI link.
# REND_COMMON_DIR must be set in the caller's scope first.
function(brender_common_target_setup target driver_define driver_suffix_upper driver_suffix_lower driver_name)
    get_target_property(target_type ${target} TYPE)
    if(target_type STREQUAL SHARED_LIBRARY)
        target_compile_definitions(${target} PRIVATE -DDEFINE_BR_ENTRY_POINT)
    endif()

    target_compile_definitions(${target} PRIVATE
            ${driver_define}
            BREND_DRIVER_SUFFIX_UPPER=${driver_suffix_upper}
            BREND_DRIVER_SUFFIX_LOWER=${driver_suffix_lower}
            BREND_DRIVER_NAME="${driver_name}"
            )

    target_include_directories(${target} PRIVATE
            ${CMAKE_CURRENT_BINARY_DIR}
            ${CMAKE_CURRENT_SOURCE_DIR}
            ${REND_COMMON_DIR}
            )

    target_link_libraries(${target} PRIVATE BRender::DDI)
endfunction()
