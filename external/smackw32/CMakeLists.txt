if(CARPOCALYPSE2_PLATFORM_WIN32)
    add_library(Smackw32::Smackw32 SHARED IMPORTED GLOBAL)
    set_property(TARGET Smackw32::Smackw32 PROPERTY INTERFACE_INCLUDE_DIRECTORIES "${CMAKE_CURRENT_LIST_DIR}/include")
    set_property(TARGET Smackw32::Smackw32 PROPERTY IMPORTED_IMPLIB "${CMAKE_CURRENT_LIST_DIR}/lib/x86/smackw32.lib")
else()
    add_library(smackw32_smackw32_dummy STATIC
        src/dummy/dummy.c
    )
    target_include_directories(smackw32_smackw32_dummy PUBLIC "${CMAKE_CURRENT_LIST_DIR}/include")
    if(CARPOCALYPSE2_MATCHING OR NOT CARPOCALYPSE2_PLATFORM_SDL3)
        add_library(Smackw32::Smackw32 ALIAS smackw32_smackw32_dummy)
    else()
        # Real .SMK playback via libsmacker (LGPL-2.1+, see external/libsmacker).
        add_library(smackw32_smackw32_libsmacker STATIC
            src/libsmacker/libsmacker_adapter.c
            ../libsmacker/smacker.c
            ../libsmacker/smk_bitstream.c
            ../libsmacker/smk_hufftree.c
        )
        target_include_directories(smackw32_smackw32_libsmacker
            PUBLIC "${CMAKE_CURRENT_LIST_DIR}/include"
            PRIVATE "${CMAKE_CURRENT_LIST_DIR}/../libsmacker"
        )
        add_library(Smackw32::Smackw32 ALIAS smackw32_smackw32_libsmacker)
    endif()
endif()
