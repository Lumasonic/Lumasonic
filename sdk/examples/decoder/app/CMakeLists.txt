# Define the executable
add_executable(lsdecoder
    Main.cpp
)

# Additional include directorties to access the API
target_include_directories(lsdecoder
    PRIVATE
        "${CMAKE_CURRENT_SOURCE_DIR}/../../../include"
)

# Link against the library
target_link_libraries(lsdecoder
    PRIVATE
        LumasonicDecoder
)

# Link libdl on Linux as it is sometimes needed
if(UNIX)
     find_library(DL_LIB NAMES dl REQUIRED)
    message(STATUS "libdl found at: ${DL_LIB}")
    target_link_libraries(lsdecoder PRIVATE ${DL_LIB})
endif()

# Set compile options (optional)
target_compile_options(lsdecoder
    PRIVATE
        $<$<CXX_COMPILER_ID:GNU>:-Wall -Wextra>
        $<$<CXX_COMPILER_ID:MSVC>:/W4>
)

# Install target (optional)
install(TARGETS lsdecoder
    RUNTIME DESTINATION bin
)