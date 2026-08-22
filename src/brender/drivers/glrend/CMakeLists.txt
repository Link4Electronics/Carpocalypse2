project(glrend C)

# Embed shared shaders (drivers/commonrend)
set(SHADER_DIR "${CMAKE_CURRENT_SOURCE_DIR}/../commonrend")

add_custom_command(
        OUTPUT "${CMAKE_CURRENT_BINARY_DIR}/default.vert.glsl.h"
        COMMAND "${CMAKE_COMMAND}" "-DSOURCE_DIR=${SHADER_DIR}" "-DFILE=default.vert.glsl" -P "${BRender_SOURCE_DIR}/cmake/EmbedResource.cmake"
        DEPENDS "${SHADER_DIR}/default.vert.glsl"
)
add_custom_command(
        OUTPUT "${CMAKE_CURRENT_BINARY_DIR}/default.frag.glsl.h"
        COMMAND "${CMAKE_COMMAND}" "-DSOURCE_DIR=${SHADER_DIR}" "-DFILE=default.frag.glsl" -P "${BRender_SOURCE_DIR}/cmake/EmbedResource.cmake"
        DEPENDS "${SHADER_DIR}/default.frag.glsl"
)
add_custom_command(
        OUTPUT "${CMAKE_CURRENT_BINARY_DIR}/brender.vert.glsl.h"
        COMMAND "${CMAKE_COMMAND}" "-DSOURCE_DIR=${SHADER_DIR}" "-DFILE=brender.vert.glsl" -P "${BRender_SOURCE_DIR}/cmake/EmbedResource.cmake"
        DEPENDS "${SHADER_DIR}/brender.vert.glsl"
)
add_custom_command(
        OUTPUT "${CMAKE_CURRENT_BINARY_DIR}/brender.frag.glsl.h"
        COMMAND "${CMAKE_COMMAND}" "-DSOURCE_DIR=${SHADER_DIR}" "-DFILE=brender.frag.glsl" -P "${BRender_SOURCE_DIR}/cmake/EmbedResource.cmake"
        DEPENDS "${SHADER_DIR}/brender.frag.glsl"
)
add_custom_command(
        OUTPUT "${CMAKE_CURRENT_BINARY_DIR}/text.frag.glsl.h"
        COMMAND "${CMAKE_COMMAND}" "-DSOURCE_DIR=${SHADER_DIR}" "-DFILE=text.frag.glsl" -P "${BRender_SOURCE_DIR}/cmake/EmbedResource.cmake"
        DEPENDS "${SHADER_DIR}/text.frag.glsl"
)


set(REND_COMMON_DIR "${CMAKE_CURRENT_SOURCE_DIR}/../commonrend")
include(${CMAKE_CURRENT_SOURCE_DIR}/../cmake/commonrend.cmake)
brender_common_sources(${REND_COMMON_DIR})

set(VIDEO_FILES
        checkboard64.c
        shader_brender.c
        shader_default.c
        shader_text.c
        video.c
        video.h
        ${REND_COMMON_DIR}/shader_data.h
        )
source_group("VIDEO" FILES ${VIDEO_FILES})

set(RENDERER_FILES
        ${BR_COMMON_RENDERER_FILES}

        gv1buckt.c
        gv1buckt.h

        gstored.h
        v1model.c

        sbuffer.h
        sbuffer.c
        )
source_group("Renderer" FILES ${RENDERER_FILES})

set(GLREND_FILES
        ${BR_COMMON_OBJ_FILES}

        drv_ip.h
        devpixmp.h
        devpmglf.c
        devpixmp.c

        quad.c
        util.c

        glassert.c
        glassert.h

        glad.c
        )

set(INC_FILES
        include/brglrend.h
        include/brglrend_p.h

        include/glad/glad.h
        include/KHR/khrplatform.h
        )

add_library(glrend-headers INTERFACE)
target_sources(glrend-headers INTERFACE
        FILE_SET include
        TYPE HEADERS
        BASE_DIRS include
        FILES ${INC_FILES}
        )

target_include_directories(glrend-headers INTERFACE
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}/brender/glrend>
        )

set_target_properties(glrend-headers PROPERTIES
        EXPORT_NAME "Drivers::GLRend::Headers"
        FOLDER "BRender/Drivers"
        )

add_library(glrend
        ${GLREND_FILES}
        ${RENDERER_FILES}
        ${VIDEO_FILES}
        ${BR_COMMON_STATE_FILES}
        ${INC_FILES}

        "${CMAKE_CURRENT_BINARY_DIR}/default.vert.glsl.h"
        "${CMAKE_CURRENT_BINARY_DIR}/default.frag.glsl.h"
        "${CMAKE_CURRENT_BINARY_DIR}/brender.vert.glsl.h"
        "${CMAKE_CURRENT_BINARY_DIR}/brender.frag.glsl.h"
        "${CMAKE_CURRENT_BINARY_DIR}/text.frag.glsl.h"
        )

brender_common_target_setup(glrend BREND_DRIVER_GL GL gl "glrend")
target_link_libraries(glrend PUBLIC glrend-headers)

set_target_properties(glrend PROPERTIES
        C_STANDARD 11
        C_STANDARD_REQUIRED TRUE
        EXPORT_NAME "Drivers::GLRend"
        FOLDER "BRender/Drivers"
        )

if(BRENDER_ASAN_ENABLED)
	set(CMAKE_C_FLAGS
		"${CMAKE_C_FLAGS}  -fsanitize=undefined -fsanitize=address"
	)
	target_compile_options(glrend PUBLIC -fsanitize=address)
	target_link_options(glrend
			BEFORE PUBLIC -fsanitize=undefined PUBLIC -fsanitize=address,undefined
	)
endif()

add_library(BRender::Drivers::GLRend::Headers ALIAS glrend-headers)
add_library(BRender::Drivers::GLRend ALIAS glrend)
