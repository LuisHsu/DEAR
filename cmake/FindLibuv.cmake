find_package(PkgConfig)
pkg_check_modules(PKG_LIBUV REQUIRED libuv)

find_path(LIBUV_INCLUDE_DIR uv.h ${PKG_LIBUV_INCLUDE_DIRS})

find_library(LIBUV_LIBRARIES NAMES uv PATHS ${PKG_LIBUV_LIBRARY_DIRS})

set(LIBUV_FOUND TRUE)

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(LIBUV DEFAULT_MSG LIBUV_INCLUDE_DIR LIBUV_LIBRARIES)
mark_as_advanced(LIBUV_INCLUDE_DIR LIBUV_LIBRARIES)