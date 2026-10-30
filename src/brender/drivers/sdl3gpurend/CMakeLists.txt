project(sdl3gpurend C)

# SDL3 GPU driver. Shares drivers/commonrend with glrend; the common sources
# are compiled a second time here with BREND_DRIVER_SDL3GPUREND.

find_package(SDL3 REQUIRED CONFIG)

# --- Shader toolchain ---------------------------------------------------------
#
# The driver renders with SDL3 GPU, whose backends each require a different
# shader format:
#   - Vulkan:  SPIR-V, produced by glslangValidator/glslang/glslc (-V) from the
#              filtered commonrend GLSL.
#   - Metal:   MSL, cross-compiled from the SPIR-V with SDL_shadercross (or
#              spirv-cross --msl --msl-decoration-binding as a fallback), which
#              maps the SPIR-V bindings onto the SDL3-GPU slots and renames the
#              entry point main -> main0.
#   - D3D12:   DXIL, cross-compiled from the SPIR-V with SDL_shadercross (which
#              bundles SPIRV-Cross + DXC), or spirv-cross (--hlsl
#              --shader-model 60) + dxc as a fallback.
# SDL_shadercross is the preferred translator: it is the tool SDL3 itself uses,
# and its MSL/DXIL output matches the SDL3-GPU conventions by construction.
# Every available format is embedded at build time; video.c requests exactly the
# formats this build produced and selects the matching source per device (see
# sdl3gpurend_shader_formats.h and SDL3GPUREND_CreateShader).
#
# MSL is mandatory on Apple: SDL3 GPU picks the Metal backend there, and a
# build that can only produce SPIR-V would not run without MoltenVK. Fail at
# configure time instead of at runtime.
find_program(GLSL_COMPILER NAMES glslangValidator glslang glslc)
if(NOT GLSL_COMPILER)
    message(FATAL_ERROR "sdl3gpurend requires glslangValidator/glslang/glslc to build its SPIR-V shaders")
endif()
get_filename_component(GLSL_COMPILER_NAME ${GLSL_COMPILER} NAME_WE)

find_program(SHADERCROSS_COMPILER NAMES shadercross)
if(SHADERCROSS_COMPILER)
    message(STATUS "Found SDL_shadercross: ${SHADERCROSS_COMPILER} (SPIR-V -> MSL / DXIL)")
endif()

find_program(SPIRV_CROSS_COMPILER NAMES spirv-cross)
if(SPIRV_CROSS_COMPILER)
    message(STATUS "Found spirv-cross: ${SPIRV_CROSS_COMPILER} (SPIR-V -> MSL / HLSL; fallback)")
endif()

find_program(DXC_COMPILER NAMES dxc)
if(DXC_COMPILER)
    message(STATUS "Found dxc: ${DXC_COMPILER} (HLSL -> DXIL for D3D12; fallback)")
endif()

if(APPLE AND NOT (SPIRV_CROSS_COMPILER OR SHADERCROSS_COMPILER))
    message(FATAL_ERROR
        "SDL_shadercross or spirv-cross is required to build the sdl3gpurend "
        "driver on Apple (MSL shaders for the Metal backend). Install SDL_shadercross "
        "(https://github.com/libsdl-org/SDL_shadercross) or spirv-cross.")
endif()

if(WIN32 AND NOT (SHADERCROSS_COMPILER OR (SPIRV_CROSS_COMPILER AND DXC_COMPILER)))
    message(WARNING
        "SDL_shadercross (or spirv-cross and dxc) not found; the sdl3gpurend "
        "driver will fall back to the Vulkan backend on Windows. Install "
        "SDL_shadercross (https://github.com/libsdl-org/SDL_shadercross) to "
        "enable the D3D12 (DXIL) backend.")
endif()

# Which shader formats does this build embed? Drives the device-create mask in
# video.c (via sdl3gpurend_shader_formats.h).
set(SDL3GPUREND_SHADERFORMAT_MSL_AVAILABLE 0)
set(SDL3GPUREND_SHADERFORMAT_DXIL_AVAILABLE 0)
if(SPIRV_CROSS_COMPILER OR SHADERCROSS_COMPILER)
    set(SDL3GPUREND_SHADERFORMAT_MSL_AVAILABLE 1)
    # DXIL is only used by the D3D12 backend (Windows). SDL_shadercross produces
    # it without dxc; the spirv-cross fallback still needs dxc for the final
    # HLSL -> DXIL step.
    if((SHADERCROSS_COMPILER AND WIN32) OR DXC_COMPILER)
        set(SDL3GPUREND_SHADERFORMAT_DXIL_AVAILABLE 1)
    endif()
endif()

file(WRITE "${CMAKE_CURRENT_BINARY_DIR}/sdl3gpurend_shader_formats.h"
    "#ifndef SDL3GPUREND_SHADER_FORMATS_H_\n"
    "#define SDL3GPUREND_SHADER_FORMATS_H_\n"
    "\n"
    "/* Auto-generated; describes which shader formats this build embeds. */\n"
    "#define SDL3GPUREND_SHADERFORMAT_SPIRV_AVAILABLE 1\n"
    "#define SDL3GPUREND_SHADERFORMAT_MSL_AVAILABLE ${SDL3GPUREND_SHADERFORMAT_MSL_AVAILABLE}\n"
    "#define SDL3GPUREND_SHADERFORMAT_DXIL_AVAILABLE ${SDL3GPUREND_SHADERFORMAT_DXIL_AVAILABLE}\n"
    "\n"
    "#endif\n")

# Shaders live in drivers/commonrend and use ##ifdef markers so the same GLSL
# source can be compiled for GL and SDL3 GPU. Filter for the SDL3GPU binding
# layout, then compile to SPIR-V.
set(SHADER_DIR "${CMAKE_CURRENT_SOURCE_DIR}/../commonrend")
set(SPV_GENERATED)
set(MSL_GENERATED)
set(DXIL_GENERATED)
foreach(spv_file
    brender.vert.spv
    brender.frag.spv
    overlay.vert.spv
    overlay.frag.spv
    text.vert.spv
    text.frag.spv
)
    string(REPLACE ".spv" "" spv_base ${spv_file})
    string(REPLACE ".spv" ".glsl" glsl_file ${spv_file})
    set(glsl_src "${SHADER_DIR}/${glsl_file}")
    set(glsl_filtered "${CMAKE_CURRENT_BINARY_DIR}/${glsl_file}")
    set(spv_path "${CMAKE_CURRENT_BINARY_DIR}/${spv_file}")
    set(h_path "${CMAKE_CURRENT_BINARY_DIR}/${spv_file}.h")

    if(spv_base MATCHES "vert$")
        set(shader_stage vert)
        set(shadercross_stage vertex)
    else()
        set(shader_stage frag)
        set(shadercross_stage fragment)
    endif()

    add_custom_command(
        OUTPUT ${glsl_filtered}
        COMMAND ${CMAKE_COMMAND} "-DINPUT=${glsl_src}" "-DOUTPUT=${glsl_filtered}" "-DKEEP=SDL3GPU" -P "${CMAKE_SOURCE_DIR}/cmake/FilterShader.cmake"
        DEPENDS ${glsl_src}
        COMMENT "Filtering ${glsl_file} for SPIR-V"
    )
    if(GLSL_COMPILER_NAME STREQUAL "glslangValidator" OR GLSL_COMPILER_NAME STREQUAL "glslang")
        set(compile_cmd ${GLSL_COMPILER} -V -o ${spv_path} ${glsl_filtered})
    else()
        set(compile_cmd ${GLSL_COMPILER} -o ${spv_path} ${glsl_filtered})
    endif()
    add_custom_command(
        OUTPUT ${spv_path}
        COMMAND ${compile_cmd}
        DEPENDS ${glsl_filtered}
        COMMENT "Compiling ${glsl_file} -> ${spv_file}"
    )

    if(SHADERCROSS_COMPILER)
        # Metal: MSL source, compiled by Metal at runtime. SDL_shadercross
        # maps the SPIR-V binding numbers onto the MSL indices (matching
        # spirv-cross --msl-decoration-binding), which for these shaders are
        # exactly the SDL3-GPU uniform/sampler slots (model=0, scene=1,
        # texture/sampler=0), and renames the entry point main -> main0.
        # The Metal backend treats the shader as MSL at all times (even when
        # METALLIB is the format bit), so this source is embedded as bytes.
        set(metal_path "${CMAKE_CURRENT_BINARY_DIR}/${spv_base}.metal")
        add_custom_command(
            OUTPUT ${metal_path}
            COMMAND ${SHADERCROSS_COMPILER} -s SPIRV -d MSL -t ${shadercross_stage} -e main -o ${metal_path} ${spv_path}
            DEPENDS ${spv_path}
            COMMENT "Cross-compiling ${spv_base} -> MSL (SDL_shadercross)"
        )
        set(metal_h "${CMAKE_CURRENT_BINARY_DIR}/${spv_base}.metal.h")
        add_custom_command(
            OUTPUT ${metal_h}
            COMMAND ${CMAKE_COMMAND} "-DSOURCE_DIR=${CMAKE_CURRENT_BINARY_DIR}" "-DFILE=${spv_base}.metal" -P "${CMAKE_SOURCE_DIR}/cmake/EmbedResource.cmake"
            DEPENDS ${metal_path}
            COMMENT "Embedding ${spv_base}.metal"
        )
        list(APPEND MSL_GENERATED ${metal_h})

        # DXIL (D3D12, Windows only): SDL_shadercross transpiles to HLSL
        # (SM6.0, with the register spaces SDL3's D3D12 backend expects) and
        # compiles to DXIL via the bundled DXC in one step.
        if(WIN32)
            set(dxil_path "${CMAKE_CURRENT_BINARY_DIR}/${spv_base}.dxil")
            add_custom_command(
                OUTPUT ${dxil_path}
                COMMAND ${SHADERCROSS_COMPILER} -s SPIRV -d DXIL -t ${shadercross_stage} -e main -o ${dxil_path} ${spv_path}
                DEPENDS ${spv_path}
                COMMENT "Compiling ${spv_base} -> DXIL (SDL_shadercross)"
            )
            set(dxil_h "${CMAKE_CURRENT_BINARY_DIR}/${spv_base}.dxil.h")
            add_custom_command(
                OUTPUT ${dxil_h}
                COMMAND ${CMAKE_COMMAND} "-DSOURCE_DIR=${CMAKE_CURRENT_BINARY_DIR}" "-DFILE=${spv_base}.dxil" -P "${CMAKE_SOURCE_DIR}/cmake/EmbedResource.cmake"
                DEPENDS ${dxil_path}
                COMMENT "Embedding ${spv_base}.dxil"
            )
            list(APPEND DXIL_GENERATED ${dxil_h})
        endif()
    elseif(SPIRV_CROSS_COMPILER)
        # Metal: MSL source, compiled by Metal at runtime.
        # --msl-decoration-binding maps the SPIR-V binding numbers onto the
        # MSL indices, which for these shaders are exactly the SDL3-GPU
        # uniform/sampler slots (model=0, scene=1, texture/sampler=0).
        set(metal_path "${CMAKE_CURRENT_BINARY_DIR}/${spv_base}.metal")
        add_custom_command(
            OUTPUT ${metal_path}
            COMMAND ${SPIRV_CROSS_COMPILER} ${spv_path} --msl --msl-decoration-binding --rename-entry-point main main0 ${shader_stage} --output ${metal_path}
            DEPENDS ${spv_path}
            COMMENT "Cross-compiling ${spv_base} -> MSL"
        )
        set(metal_h "${CMAKE_CURRENT_BINARY_DIR}/${spv_base}.metal.h")
        add_custom_command(
            OUTPUT ${metal_h}
            COMMAND ${CMAKE_COMMAND} "-DSOURCE_DIR=${CMAKE_CURRENT_BINARY_DIR}" "-DFILE=${spv_base}.metal" -P "${CMAKE_SOURCE_DIR}/cmake/EmbedResource.cmake"
            DEPENDS ${metal_path}
            COMMENT "Embedding ${spv_base}.metal"
        )
        list(APPEND MSL_GENERATED ${metal_h})

        # HLSL (SM6.0) then DXIL. spirv-cross already emits the register
        # spaces SDL3's D3D12 backend expects (vertex: b[0..1] space1;
        # fragment: t/s space2, b[0..1] space3).
        set(hlsl_path "${CMAKE_CURRENT_BINARY_DIR}/${spv_base}.hlsl")
        add_custom_command(
            OUTPUT ${hlsl_path}
            COMMAND ${SPIRV_CROSS_COMPILER} ${spv_path} --hlsl --shader-model 60 --output ${hlsl_path}
            DEPENDS ${spv_path}
            COMMENT "Cross-compiling ${spv_base} -> HLSL"
        )
        if(DXC_COMPILER)
            if(shader_stage STREQUAL "vert")
                set(dxil_profile vs_6_0)
            else()
                set(dxil_profile ps_6_0)
            endif()
            set(dxil_path "${CMAKE_CURRENT_BINARY_DIR}/${spv_base}.dxil")
            add_custom_command(
                OUTPUT ${dxil_path}
                COMMAND ${DXC_COMPILER} -T ${dxil_profile} -E main -Fo ${dxil_path} ${hlsl_path}
                DEPENDS ${hlsl_path}
                COMMENT "Compiling ${spv_base} -> DXIL (${dxil_profile})"
            )
            set(dxil_h "${CMAKE_CURRENT_BINARY_DIR}/${spv_base}.dxil.h")
            add_custom_command(
                OUTPUT ${dxil_h}
                COMMAND ${CMAKE_COMMAND} "-DSOURCE_DIR=${CMAKE_CURRENT_BINARY_DIR}" "-DFILE=${spv_base}.dxil" -P "${CMAKE_SOURCE_DIR}/cmake/EmbedResource.cmake"
                DEPENDS ${dxil_path}
                COMMENT "Embedding ${spv_base}.dxil"
            )
            list(APPEND DXIL_GENERATED ${dxil_h})
        endif()
    endif()

    add_custom_command(
        OUTPUT ${h_path}
        COMMAND ${CMAKE_COMMAND} "-DSOURCE_DIR=${CMAKE_CURRENT_BINARY_DIR}" "-DFILE=${spv_file}" -P "${CMAKE_SOURCE_DIR}/cmake/EmbedResource.cmake"
        DEPENDS ${spv_path}
        COMMENT "Embedding ${spv_file}"
    )
    list(APPEND SPV_GENERATED ${h_path})
endforeach()

set(REND_COMMON_DIR "${CMAKE_CURRENT_SOURCE_DIR}/../commonrend")
include(${CMAKE_CURRENT_SOURCE_DIR}/../cmake/commonrend.cmake)
brender_common_sources(${REND_COMMON_DIR})

set(VIDEO_FILES
        sdl3_shaders.c
        sdl3_dyn.c
        sdl3_dyn.h
        sdl3_syms.h
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

set(SDL3GPUREND_FILES
        ${BR_COMMON_OBJ_FILES}

        drv_ip.h
        devpixmp.h
        devpmgpu.c
        devpixmp.c

        ../glrend/compat/compat.c
        )

set(INC_FILES
        brsdl3gpurend.h
        )

add_library(sdl3gpurend-headers INTERFACE)
target_sources(sdl3gpurend-headers INTERFACE
        FILE_SET include
        TYPE HEADERS
        BASE_DIRS ${CMAKE_CURRENT_SOURCE_DIR}
        FILES ${INC_FILES}
        )

target_include_directories(sdl3gpurend-headers INTERFACE
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
        $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}/brender/sdl3gpurend>
        )

set_target_properties(sdl3gpurend-headers PROPERTIES
        EXPORT_NAME "Drivers::SDL3GPUREND::Headers"
        FOLDER "BRender/Drivers"
        )

add_library(sdl3gpurend
        ${SDL3GPUREND_FILES}
        ${RENDERER_FILES}
        ${VIDEO_FILES}
        ${BR_COMMON_STATE_FILES}
        ${INC_FILES}
        ${SPV_GENERATED}
        ${MSL_GENERATED}
        ${DXIL_GENERATED}
        )

brender_common_target_setup(sdl3gpurend BREND_DRIVER_SDL3GPUREND SDL3GPUREND sdl3gpurend "sdl3gpurend")

target_link_libraries(sdl3gpurend PUBLIC sdl3gpurend-headers PRIVATE SDL3::SDL3 c2_hooks)
target_compile_definitions(sdl3gpurend PRIVATE BASED_FLOAT=1 BASED_FIXED=0)
target_include_directories(sdl3gpurend PRIVATE
        "${CMAKE_CURRENT_SOURCE_DIR}/../glrend/compat"
        "${CMAKE_CURRENT_SOURCE_DIR}/../ddi"
        "${CMAKE_CURRENT_SOURCE_DIR}/../.."
        "${CMAKE_CURRENT_SOURCE_DIR}/../../include/brender"
        )
# See glrend/CMakeLists.txt: the decompiled dispatch tables use br_uint_32*
# instead of the upstream typed pointers, but the layouts match.
target_compile_options(sdl3gpurend PRIVATE -Wno-incompatible-pointer-types -Wno-int-conversion)

set_target_properties(sdl3gpurend PROPERTIES
        C_STANDARD 11
        C_STANDARD_REQUIRED TRUE
        EXPORT_NAME "Drivers::SDL3GPUREND"
        FOLDER "BRender/Drivers"
        )

add_library(BRender::Drivers::SDL3GPUREND::Headers ALIAS sdl3gpurend-headers)
add_library(BRender::Drivers::SDL3GPUREND ALIAS sdl3gpurend)
