if(CARPOCALYPSE2_FIX_BUGS)
    add_definitions(-DCARPOCALYPSE2_FIX_BUGS)
endif()

add_subdirectory(hooks)
add_subdirectory(common)
add_subdirectory(s3)
add_subdirectory(platform)
