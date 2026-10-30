# Filters ##ifdef blocks out of a shared GLSL source for the SDL3GPU backend.
#
# Usage:
#   cmake -DINPUT=in.glsl -DOUTPUT=out.glsl -DKEEP=SDL3GPU -P FilterShader.cmake
#
# The shared shaders in drivers/commonrend/ mark per-backend sections with
# ##ifdef SDL3GPU / ##ifdef GL_ES / ##ifdef GL_CORE / ##else / ##endif markers
# (double-hash so that neither the GLSL compiler nor glslang ever sees them).
# The GL driver strips these in C (glrend/video.c preprocessShader); this
# script does the same for the SDL3GPU driver so glslang only receives a plain
# shader with a literal-first #version line.
#
# KEEP is the block to retain ("SDL3GPU"). Blocks for other backends are
# dropped. Lines with no ##ifdef block (state "none") always pass through, so
# shaders without markers are filtered unchanged.
#
# The line loop uses a string marker + while() rather than CMake lists, because
# list conversion mangles trailing backslashes (GLSL macro continuations) and
# semicolons.

if(NOT DEFINED INPUT OR NOT DEFINED OUTPUT OR NOT DEFINED KEEP)
    message(FATAL_ERROR "Usage: cmake -DINPUT=in.glsl -DOUTPUT=out.glsl -DKEEP=SDL3 -P FilterShader.cmake")
endif()

file(READ "${INPUT}" raw)
string(REPLACE "\r\n" "\n" raw "${raw}")
string(REPLACE "\r" "\n" raw "${raw}")
string(REPLACE "\n" "__VKNL__" work "${raw}")

set(state "none")
set(out "")

while(work)
    string(FIND "${work}" "__VKNL__" idx)
    if(idx EQUAL -1)
        set(line "${work}")
        set(work "")
    else()
        string(SUBSTRING "${work}" 0 ${idx} line)
        math(EXPR next "${idx} + 8")
        string(SUBSTRING "${work}" ${next} -1 work)
    endif()

    if(line STREQUAL "##ifdef SDL3GPU")
        set(state "SDL3GPU")
    elseif(line STREQUAL "##ifdef GL_ES")
        set(state "GL")
    elseif(line STREQUAL "##ifdef GL_CORE")
        set(state "GL")
    elseif(line STREQUAL "##else")
        if(state STREQUAL "SDL3GPU")
            set(state "SDL3GPU_ELSE")
        else()
            set(state "GL")
        endif()
    elseif(line STREQUAL "##endif")
        set(state "none")
    else()
        if(state STREQUAL "none")
            string(APPEND out "${line}\n")
        elseif(KEEP STREQUAL state)
            string(APPEND out "${line}\n")
        endif()
    endif()
endwhile()

file(WRITE "${OUTPUT}" "${out}")
