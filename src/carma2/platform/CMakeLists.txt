add_library(c2_platform STATIC
    platform.h
    carpocalypse2_logging.c
)

add_library(c2_platform_brender STATIC
    platform.h
)

if(CARPOCALYPSE2_PLATFORM_WIN32)
    target_sources(c2_platform_brender PRIVATE
        win32/win32_br_platform.c
    )
    target_sources(c2_platform PRIVATE
        win32/win32_ssdx.c
        win32/win32_ssdx.h
        win32/win32_sound.c
        win32/win32_sound.h
        win32/win32_dinput.c
        win32/win32_dinput.h
        win32/win32_windows.c
        win32/win32_windows.h
        win32/win32.c
        win32/win32.h
        win32/win32net.c
        win32/win32net.h
        win32/winmain.c
    )
    target_compile_definitions(c2_platform
        PUBLIC
            DIRECTINPUT_VERSION=0x0500
            DIRECTSOUND_VERSION=0x0500
    )
    find_library(DSOUND_LIBRARY NAMES dsound)
    find_library(WINMM_LIBRARY NAMES winmm)
    find_library(WS2_32_LIBRARY NAMES ws2_32)
    option(SOUND_ENABLED "Enable sound with DirectSound" ON)
    target_link_libraries(c2_platform
        PRIVATE
            DirectX::dinput
            DirectX::dxguid
            winmm
            ws2_32
            IForce2::IForce2
    )
    if(SOUND_ENABLED)
        target_link_libraries(c2_platform PRIVATE dsound)
        target_compile_definitions(c2_platform PRIVATE DSOUND_ENABLED)
    endif()
elseif(CARPOCALYPSE2_PLATFORM_SDL3)
    target_sources(c2_platform_brender PRIVATE
        sdl3/sdl3_br_platform.c
        sdl3/sdl3_br_platform.h
    )
    target_link_libraries(c2_platform_brender PRIVATE SDL3::SDL3)
    target_sources(c2_platform PRIVATE
        sdl3/main.c
        sdl3/sdl3.c
        sdl3/sdl3.h
        sdl3/sdl3_input.c
        sdl3/sdl3_input.h
        sdl3/sdl3_net.c
        sdl3/sdl3_net.h
        sdl3/sdl3_plib.c
        sdl3/sdl3_rend.c
        sdl3/sdl3_ssdx.c
        sdl3/sdl3_ssdx.h
    )
    target_link_libraries(c2_platform PRIVATE SDL3::SDL3)
    # sdl3_rend.c uses BRender DDI internals (core headers)
    target_link_libraries(c2_platform PRIVATE BRender::ddi)
    if(UNIX AND NOT APPLE)
        # Case-insensitive file name resolution (see sdl3/sdl3_casefix.c).
        target_sources(c2_platform PRIVATE sdl3/sdl3_casefix.c)
    endif()
else()
    message(FATAL_ERROR "Unsupported platform")
endif()

if(CARPOCALYPSE2_PLATFORM_SDL3)
    find_package(SDL3 REQUIRED CONFIG)
endif()

target_include_directories(c2_platform
    PUBLIC
        "$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>"
)
target_link_libraries(c2_platform
    PUBLIC
        BRender::Headers
        c2_newcommon
        c2_hooks
)
target_include_directories(c2_platform_brender
    PUBLIC
        "$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>"
)
target_link_libraries(c2_platform_brender
    PUBLIC
        BRender::Headers
        c2_hooks
)

add_executable(carpocalypse2 WIN32
    $<TARGET_OBJECTS:c2_newcommon>
    $<TARGET_OBJECTS:c2_platform>
)
target_link_libraries(carpocalypse2 PRIVATE
    BRender::BRender
    c2_newcommon
    c2_platform
)
if(MSVC)
    target_link_options(carpocalypse2 PRIVATE "-MAP:$<TARGET_FILE_DIR:carpocalypse2>/$<TARGET_FILE_BASE_NAME:carpocalypse2>.map")
endif()
if(CARPOCALYPSE2_PLATFORM_SDL3 AND UNIX AND NOT APPLE)
    # Interpose fopen() with a case-insensitive resolver; the game requests its
    # data files in Windows-style casing (see sdl3/sdl3_casefix.c).
    target_link_options(carpocalypse2 PRIVATE "-Wl,--wrap=fopen")
endif()
if(CARPOCALYPSE2_MATCHING)
    add_dependencies(carpocalypse2 brender_driver_d3d)

    set(CMAKE_C_FLAGS "/W3 /GX /DWIN32 /D_WINDOWS")
    set(CMAKE_C_FLAGS_DEBUG "/Gm /Od /D_DEBUG /MLd")
    set(CMAKE_C_FLAGS_RELEASE "/O0 /DNDEBUG /ML")
    set(CMAKE_C_FLAGS_RELWITHDEBINFO "/DNDEBUG /ML")
    set(CMAKE_C_FLAGS_MINSIZEREL "/DNDEBUG /ML")

    # No optimizations for win32/win32_sound.c
    set_property(SOURCE
        "${CMAKE_CURRENT_SOURCE_DIR}/win32/win32_dinput.c"
        "${CMAKE_CURRENT_SOURCE_DIR}/win32/win32_ssdx.c"
        "${CMAKE_CURRENT_SOURCE_DIR}/win32/win32.c"
        "${CMAKE_CURRENT_SOURCE_DIR}/win32/win32net.c"
        "${CMAKE_CURRENT_SOURCE_DIR}/win32/winmain.c"
        APPEND PROPERTY COMPILE_OPTIONS "-O2;-Ob2")
endif()
