# Create an IMPORTED library target for the prebuilt library
add_library(LumasonicDecoder STATIC IMPORTED GLOBAL)

#Platform Specific Includes
if (WIN32)
    set_target_properties(LumasonicDecoder PROPERTIES
        IMPORTED_LOCATION "${CMAKE_CURRENT_SOURCE_DIR}/win/${CMAKE_SYSTEM_PROCESSOR}/release/LumasonicDecoder.lib"
        IMPORTED_LOCATION_DEBUG "${CMAKE_CURRENT_SOURCE_DIR}/win/${CMAKE_SYSTEM_PROCESSOR}/debug/LumasonicDecoder.lib"
        IMPORTED_LOCATION_RELEASE "${CMAKE_CURRENT_SOURCE_DIR}/win/${CMAKE_SYSTEM_PROCESSOR}/release/LumasonicDecoder.lib"
        IMPORTED_LOCATION_RELWITHDEBINFO "${CMAKE_CURRENT_SOURCE_DIR}/win/${CMAKE_SYSTEM_PROCESSOR}/release_symbols/LumasonicDecoder.lib"
        IMPORTED_LOCATION_MINSIZEREL "${CMAKE_CURRENT_SOURCE_DIR}/win/${CMAKE_SYSTEM_PROCESSOR}/release/LumasonicDecoder.lib"
        INTERFACE_INCLUDE_DIRECTORIES "${CMAKE_CURRENT_SOURCE_DIR}/../include"
    )
elseif(APPLE)
    set_target_properties(LumasonicDecoder PROPERTIES
        IMPORTED_LOCATION "${CMAKE_CURRENT_SOURCE_DIR}/macos/debug/libLumasonicDecoder.a"
        IMPORTED_LOCATION_DEBUG "${CMAKE_CURRENT_SOURCE_DIR}/macos/debug/libLumasonicDecoder.a"
        IMPORTED_LOCATION_RELEASE "${CMAKE_CURRENT_SOURCE_DIR}/macos/release/libLumasonicDecoder.a"
        IMPORTED_LOCATION_RELWITHDEBINFO "${CMAKE_CURRENT_SOURCE_DIR}/macos/release_symbols/libLumasonicDecoder.a"
        IMPORTED_LOCATION_MINSIZEREL "${CMAKE_CURRENT_SOURCE_DIR}/macos/release/libLumasonicDecoder.a"
        INTERFACE_INCLUDE_DIRECTORIES "${CMAKE_CURRENT_SOURCE_DIR}/../include"
    )
elseif(UNIX)
    set_target_properties(LumasonicDecoder PROPERTIES
        IMPORTED_LOCATION "${CMAKE_CURRENT_SOURCE_DIR}/linux/${CMAKE_SYSTEM_PROCESSOR}/release/libLumasonicDecoder.a"
        IMPORTED_LOCATION_DEBUG "${CMAKE_CURRENT_SOURCE_DIR}/linux/${CMAKE_SYSTEM_PROCESSOR}/debug/libLumasonicDecoder.a"
        IMPORTED_LOCATION_RELEASE "${CMAKE_CURRENT_SOURCE_DIR}/linux/${CMAKE_SYSTEM_PROCESSOR}/release/libLumasonicDecoder.a"
        IMPORTED_LOCATION_RELWITHDEBINFO "${CMAKE_CURRENT_SOURCE_DIR}/linux/${CMAKE_SYSTEM_PROCESSOR}/release_symbols/libLumasonicDecoder.a"
        IMPORTED_LOCATION_MINSIZEREL "${CMAKE_CURRENT_SOURCE_DIR}/linux/${CMAKE_SYSTEM_PROCESSOR}/release/libLumasonicDecoder.a"
        INTERFACE_INCLUDE_DIRECTORIES "${CMAKE_CURRENT_SOURCE_DIR}/../include"
    )

    target_link_options(LumasonicDecoder
        INTERFACE
        -ldl -pthread
    )
endif()

# Create an alias target for use in other directories
add_library(Lumasonic::Decoder ALIAS LumasonicDecoder)