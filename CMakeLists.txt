cmake_minimum_required(VERSION 3.25...4.2)

cmake_policy(SET CMP0091 NEW)

project(carpocalypse2 C)

include(CMakeDependentOption)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${PROJECT_BINARY_DIR}" CACHE PATH "Runtime directory")

message(STATUS "Compiler: ${CMAKE_C_COMPILER_ID} ${CMAKE_C_COMPILER_VERSION}")
# Carmageddon 2 was built with Visual Studio 5 (MSVC 11.0)
if(WIN32 AND CMAKE_C_COMPILER_ID STREQUAL "MSVC" AND CMAKE_C_COMPILER_VERSION VERSION_EQUAL "11.0")
    option(CARPOCALYPSE2_MATCHING "Matching build" TRUE)
else()
    set(CARPOCALYPSE2_MATCHING FALSE CACHE BOOL "Matching build")
endif()

set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")
if(CARPOCALYPSE2_MATCHING)
    message(STATUS "Matching build")
    add_compile_definitions(-DCARPOCALYPSE2_MATCHING)
    set(CMAKE_MSVC_RUNTIME_LIBRARY "")
    # Embed debug information (/Z7) for Debug and RelWithDebInfo configuration type
    set(CMAKE_MSVC_DEBUG_INFORMATION_FORMAT "$<$<CONFIG:Debug,RelWithDebInfo>:Embedded>")
#    set(CMAKE_MSVC_DEBUG_INFORMATION_FORMAT "$<$<CONFIG:Debug,RelWithDebInfo>:ProgramDatabase>")

    set(CMAKE_C_FLAGS "/W3 /GX /DWIN32 /D_WINDOWS")
    set(CMAKE_C_FLAGS_DEBUG "/Gm /Od /D_DEBUG /MLd")
    set(CMAKE_C_FLAGS_RELEASE "/O2 /Ob2 /G5 /DNDEBUG /ML")
    set(CMAKE_C_FLAGS_RELWITHDEBINFO "/O2 /Ob2 /G5 /DNDEBUG /ML")
    set(CMAKE_C_FLAGS_MINSIZEREL "/Os /DNDEBUG /ML")

    set(CMAKE_CXX_FLAGS "${CMAKE_C_FLAGS}")
    set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG}")
    set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE}")
    set(CMAKE_CXX_FLAGS_RELWITHDEBINFO "${CMAKE_CXX_FLAGS_RELWITHDEBINFO}")
    set(CMAKE_CXX_FLAGS_MINSIZEREL "${CMAKE_CXX_FLAGS_MINSIZEREL}")

    set(CMAKE_EXE_LINKER_FLAGS "/machine:I386")
    set(CMAKE_EXE_LINKER_FLAGS_DEBUG "/incremental:yes /debug")
    set(CMAKE_EXE_LINKER_FLAGS_RELEASE "/incremental:no")
    set(CMAKE_EXE_LINKER_FLAGS_RELWITHDEBINFO "/incremental:no /debug")# /OPT:REF")
    set(CMAKE_EXE_LINKER_FLAGS_MINSIZEREL "/incremental:no")

    set(CMAKE_STATIC_LINKER_FLAGS "/machine:I386")

    set(CMAKE_SHARED_LINKER_FLAGS "/machine:I386")
    set(CMAKE_SHARED_LINKER_FLAGS_DEBUG "/incremental:yes /debug")
    set(CMAKE_SHARED_LINKER_FLAGS_RELEASE "/incremental:no")
    set(CMAKE_SHARED_LINKER_FLAGS_RELWITHDEBINFO "/incremental:no /debug")
    set(CMAKE_SHARED_LINKER_FLAGS_MINSIZEREL "/incremental:no")
endif()

if(CARPOCALYPSE2_MATCHING)
    include_directories("${PROJECT_SOURCE_DIR}/src/msvc500/include")
    add_compile_definitions(-DCARPOCALYPSE2_MATCHING)
    add_compile_options(-Gr) # __fastcall calling convention
endif()

set(CARPOCALYPSE2_PLATFORM_CHOICES)
if(WIN32)
    list(APPEND CARPOCALYPSE2_PLATFORM_CHOICES "WIN32")
endif()
if(NOT CARPOCALYPSE2_MATCHING)
    list(APPEND CARPOCALYPSE2_PLATFORM_CHOICES "SDL3")
endif()
if(WIN32 AND CMAKE_SIZEOF_VOID_P EQUAL 4)
    set(CARPOCALYPSE2_PLATFORM_DEFAULT "WIN32")
else()
    set(CARPOCALYPSE2_PLATFORM_DEFAULT "SDL3")
endif()
set(CARPOCALYPSE2_PLATFORM "${CARPOCALYPSE2_PLATFORM_DEFAULT}" CACHE STRING "carpocalypse2 platform")
set_property(CACHE CARPOCALYPSE2_PLATFORM PROPERTY STRINGS "${CARPOCALYPSE2_PLATFORM_CHOICES}")
if(NOT CARPOCALYPSE2_PLATFORM IN_LIST CARPOCALYPSE2_PLATFORM_CHOICES)
    message(FATAL_ERROR "Invalid CARPOCALYPSE2_PLATFORM=${CARPOCALYPSE2_PLATFORM} (choices=${CARPOCALYPSE2_PLATFORM_CHOICES})")
endif()
message(STATUS "CARPOCALYPSE2_PLATFORM=${CARPOCALYPSE2_PLATFORM} (choices=${CARPOCALYPSE2_PLATFORM_CHOICES})")
foreach(choice IN LISTS CARPOCALYPSE2_PLATFORM_CHOICES)
    if(CARPOCALYPSE2_PLATFORM STREQUAL "${choice}")
        set(CARPOCALYPSE2_PLATFORM_${choice} TRUE)
    else()
        set(CARPOCALYPSE2_PLATFORM_${choice} FALSE)
    endif()
endforeach()

option(KEEP_ACTIVE_IN_BACKGROUND "Keep the game active when alt-tabbing to another application" OFF)
cmake_dependent_option(CARPOCALYPSE2_VENDORED "Use vendored dependencies" ON "NOT CARPOCALYPSE2_MATCHING" ON)
option(CARPOCALYPSE2_WERROR "Treat warnings as errors" OFF)
cmake_dependent_option(CARPOCALYPSE2_FIX_BUGS "Fix carpocalypse2 bugs" ON "NOT CARPOCALYPSE2_MATCHING" OFF)

if(NOT MSVC)
    add_compile_options(-Wno-format-overflow)
endif()

if(CARPOCALYPSE2_VENDORED)
    add_subdirectory(external/libtiff EXCLUDE_FROM_ALL)
    add_subdirectory(external/zlib EXCLUDE_FROM_ALL)
else()
    find_package(TIFF REQUIRED)
    find_package(ZLIB REQUIRED)
endif()

add_subdirectory(external/dummyqt)
add_subdirectory(external/iforce2)
add_subdirectory(external/smackw32)

if(MSVC AND CARPOCALYPSE2_VENDORED)
    add_subdirectory(external/dxsdk EXCLUDE_FROM_ALL)
else()
    if(MSVC)
        set(dxsdk_path "${CMAKE_CURRENT_SOURCE_DIR}/external/dxsdk/dxsdk/lib")

        add_library(DirectX::dinput SHARED IMPORTED)
        find_library(DINPUT_LIBRARY NAMES dinput PATHS "${dxsdk_path}")
        set_property(TARGET DirectX::dinput PROPERTY IMPORTED_IMPLIB "${DINPUT_LIBRARY}")

        add_library(DirectX::dxguid SHARED IMPORTED)
        find_library(DXGUID_LIBRARY NAMES dxguid PATHS "${dxsdk_path}")
        set_property(TARGET DirectX::dxguid PROPERTY IMPORTED_IMPLIB "${DXGUID_LIBRARY}")

        add_library(DirectX::d3dim SHARED IMPORTED)
        find_library(D3DIM_LIBRARY NAMES d3dim PATHS "${dxsdk_path}")
        set_property(TARGET DirectX::d3dim PROPERTY IMPORTED_IMPLIB "${D3DIM_LIBRARY}")
    else()
        add_library(DirectX_dinput INTERFACE)
        target_link_libraries(DirectX_dinput INTERFACE "dinput")
        add_library(DirectX::dinput ALIAS DirectX_dinput)

        add_library(DirectX_dxguid INTERFACE)
        target_link_libraries(DirectX_dxguid INTERFACE "dxguid")
        add_library(DirectX::dxguid ALIAS DirectX_dxguid)

        add_library(DirectX_d3dim INTERFACE)
        target_link_libraries(DirectX_d3dim INTERFACE "d3dim")
        add_library(DirectX::d3dim ALIAS DirectX_d3dim)
    endif()
endif()

if(CARPOCALYPSE2_WERROR)
    if(MSVC)
        add_compile_options(/WX)
    else()
        add_compile_options(-Werror)
    endif()
endif()

if(KEEP_ACTIVE_IN_BACKGROUND)
    add_compile_definitions(KEEP_ACTIVE_IN_BACKGROUND)
endif()

if(NOT CARPOCALYPSE2_MATCHING)
    if(MSVC)
        add_compile_options(/GL- /W3)
        add_compile_definitions(_CRT_SECURE_NO_WARNINGS)
    else()
        add_compile_options(-Wall -Wno-format-overflow)
    endif()
endif()

add_subdirectory(src/brender)
add_subdirectory(src/carma2)
# The launcher is plain Win32 code (windows.h, DirectInput); it cannot be built
# natively on Linux. The MinGW toolchain builds it because WIN32 is defined there.
if(WIN32)
    add_subdirectory(src/launcher)
endif()

set(CARM2_LOCATION "" CACHE FILEPATH "Path of Carmageddon II")
if(CARM2_LOCATION)
    add_custom_target(copy_carpocalypse2
        COMMEND "Copying carpocalypse2 to \"${CARM2_LOCATION}\""
        COMMAND "${CMAKE_COMMAND}" -E copy "$<TARGET_FILE_NAME:carpocalypse2>" "${CARM2_LOCATION}/$<TARGET_FILE_NAME:carpocalypse2>"
    )
endif()

if(CARPOCALYPSE2_MATCHING)
    include("${CMAKE_CURRENT_SOURCE_DIR}/cmake/reccmp.cmake")

    reccmp_add_target(carpocalypse2 ID CARMA2_HW)
    reccmp_add_target(brender_driver_d3d ID D3D)
    if(WIN32)
        reccmp_add_target(carma2 ID LAUNCHER)
    endif()

    reccmp_configure()
endif()
