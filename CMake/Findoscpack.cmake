if(APPLE)
    set(OSCPACK_DIR /opt/homebrew/Cellar/oscpack/1.1.0/)
    set(OSCPACK_LIBS_DEBUG ${OSCPACK_DIR}/lib/liboscpack.so)
    set(OSCPACK_LIBS_RELEASE ${OSCPACK_DIR}/lib/liboscpack.so)
else()
    set(OSCPACK_DIR /usr/)
    set(OSCPACK_LIBS_DEBUG /usr/lib/arm-linux-gnueabihf/liboscpack.so)
    set(OSCPACK_LIBS_RELEASE usr/lib/arm-linux-gnueabihf/liboscpack.so)
endif()

# include directory is universal
set(OSCPACK_INCLUDE_DIRS ${OSCPACK_DIR}/include/oscpack)

mark_as_advanced(OSCPACK_INCLUDE_DIRS)

# promote package for find
include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(oscpack REQUIRED_VARS OSCPACK_DIR OSCPACK_INCLUDE_DIRS OSCPACK_LIBS_DEBUG OSCPACK_LIBS_RELEASE)

if(WIN32)
    add_library(oscpack STATIC IMPORTED)
else()
    add_library(oscpack SHARED IMPORTED)
endif()
set_target_properties(oscpack PROPERTIES
                      IMPORTED_CONFIGURATIONS "Debug;Release"
                      IMPORTED_LOCATION_RELEASE ${OSCPACK_LIBS_RELEASE}
                      IMPORTED_LOCATION_DEBUG ${OSCPACK_LIBS_DEBUG}
                      )
