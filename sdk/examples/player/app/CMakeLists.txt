# Define the executable
add_executable(lsfplayer
    Main.cpp
)

# Additional include directorties to access the API
target_include_directories(lsfplayer
    PRIVATE
        "${CMAKE_CURRENT_SOURCE_DIR}/../../../include"
)

# Link against the library
target_link_libraries(lsfplayer
    PRIVATE
        LumasonicDecoder
)

# Link libdl on Linux as it is sometimes needed
if(UNIX)
    find_library(DL_LIB NAMES dl REQUIRED)
    message(STATUS "libdl found at: ${DL_LIB}")
    target_link_libraries(lsfplayer PRIVATE ${DL_LIB})

    find_library(ASOUND_LIB NAMES asound REQUIRED)
    message(STATUS "libasound found at: ${ASOUND_LIB}")
    target_link_libraries(lsfplayer PRIVATE ${ASOUND_LIB})
endif()

# Set compile options (optional)
target_compile_options(lsfplayer
    PRIVATE
        $<$<CXX_COMPILER_ID:GNU>:-Wall -Wextra>
        $<$<CXX_COMPILER_ID:MSVC>:/W4>
)

# Install target (optional)
install(TARGETS lsfplayer
    RUNTIME DESTINATION bin
)