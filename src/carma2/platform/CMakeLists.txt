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
        win32/win32_pdinput.c
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
        sdl3/sdl3_audio.c
        sdl3/sdl3_net.c
        sdl3/sdl3_net.h
        sdl3/sdl3_plib.c
        sdl3/sdl3_rend.c
        sdl3/sdl3_ssdx.c
        sdl3/sdl3_ssdx.h
    )
    target_link_libraries(c2_platform PRIVATE SDL3::SDL3)
    if(WIN32)
        target_link_libraries(c2_platform PRIVATE ws2_32)
    endif()
    # sdl3_rend.c uses BRender DDI internals (core headers)
    target_link_libraries(c2_platform PRIVATE BRender::ddi)
    if(UNIX)
        # Case-insensitive file name resolution (see sdl3/sdl3_casefix.c).
        # macOS gets the resolver too: its case-sensitive volumes need it even
        # though Apple's linker cannot interpose fopen()/open().
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
        c2_common
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

if(ANDROID)
    # Android has no exec() entry point for this binary: an activity is
    # launched from Java by SDL's SDLActivity, which dlopens a shared library
    # and calls the SDL_main that SDL_main.h exports here. An executable would
    # not link (no main) and could not be loaded even if it did, so the game
    # is the library in this configuration.
    #
    # scripts/packaging/bundle-android.sh renames the result to libmain.so --
    # SDLActivity.getLibraries() loads {"SDL3", "main"} -- and assembles the
    # APK from packaging/android/AndroidManifest.xml plus SDL's Java sources.
    # The output directory matches where CMAKE_RUNTIME_OUTPUT_DIRECTORY puts
    # the executables (the build root), so the bundler finds
    # build/libcarpocalypse2.so without searching the tree for it.
    add_library(carpocalypse2 SHARED
        $<TARGET_OBJECTS:c2_common>
        $<TARGET_OBJECTS:c2_platform>
    )
    set_target_properties(carpocalypse2 PROPERTIES
        LIBRARY_OUTPUT_DIRECTORY "${PROJECT_BINARY_DIR}"
    )
else()
    add_executable(carpocalypse2 WIN32
        $<TARGET_OBJECTS:c2_common>
        $<TARGET_OBJECTS:c2_platform>
    )
endif()
target_link_libraries(carpocalypse2 PRIVATE
    BRender::BRender
    c2_common
    c2_platform
)
if(MSVC)
    target_link_options(carpocalypse2 PRIVATE "-MAP:$<TARGET_FILE_DIR:carpocalypse2>/$<TARGET_FILE_BASE_NAME:carpocalypse2>.map")
endif()
if(CARPOCALYPSE2_PLATFORM_SDL3 AND UNIX AND NOT APPLE)
    # Interpose fopen() with a case-insensitive resolver; the game requests its
    # data files in Windows-style casing (see sdl3/sdl3_casefix.c). That file
    # compiles its __wrap_* half only where CARPOCALYPSE2_CASEFIX_WRAP is set,
    # since Apple's linker has no --wrap (macOS gets the resolver only).
    target_compile_definitions(c2_platform PRIVATE CARPOCALYPSE2_CASEFIX_WRAP)
    target_link_options(carpocalypse2 PRIVATE "-Wl,--wrap=fopen")
    # The S3 sound layer opens data files with open()/open64() rather than
    # fopen(), so route those through the same case-insensitive resolver too.
    target_link_options(carpocalypse2 PRIVATE "-Wl,--wrap=open" "-Wl,--wrap=open64")
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
        "${CMAKE_CURRENT_SOURCE_DIR}/win32/win32_pdinput.c"
        "${CMAKE_CURRENT_SOURCE_DIR}/win32/win32_ssdx.c"
        "${CMAKE_CURRENT_SOURCE_DIR}/win32/win32.c"
        "${CMAKE_CURRENT_SOURCE_DIR}/win32/win32net.c"
        "${CMAKE_CURRENT_SOURCE_DIR}/win32/winmain.c"
        APPEND PROPERTY COMPILE_OPTIONS "-O2;-Ob2")
endif()
