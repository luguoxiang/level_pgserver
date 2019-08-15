include(FindPackageHandleStandardArgs)

find_path(gflags_INCLUDE_DIR gflags/gflags.h PATHS ${CMAKE_SOURCE_DIR}/gflags/include)
mark_as_advanced(gflags_INCLUDE_DIR)

set(gflags_LIBRARY ${CMAKE_SOURCE_DIR}/lib/libgflags_nothreads.a)
mark_as_advanced(gflags_LIBRARY)

find_package_handle_standard_args(gflags DEFAULT_MSG
    gflags_INCLUDE_DIR
    gflags_LIBRARY
)
if(gflags_FOUND)
    set(GFLAGS_INCLUDE_DIRS ${gflags_INCLUDE_DIR})
    set(GFLAGS_LIBRARIES ${gflags_LIBRARY})
    message(STATUS "Found gflags  (include: ${gflags_INCLUDE_DIR}, library: ${gflags_LIBRARY})")
    mark_as_advanced(GFLAGS_LIBRARY_DEBUG GFLAGS_LIBRARY_RELEASE
                     GFLAGS_LIBRARY GFLAGS_INCLUDE_DIR GFLAGS_ROOT_DIR)
endif()
