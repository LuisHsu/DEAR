find_package(PkgConfig)
pkg_check_modules(PKG_LIBXCB REQUIRED xcb)

find_path(XCB_INCLUDE_DIR xcb/xcb.h ${PKG_XCB_INCLUDE_DIRS})

find_library(XCB_LIBRARIES NAMES xcb PATHS ${PKG_XCB_LIBRARY_DIRS})

set(XCB_FOUND TRUE)

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(XCB DEFAULT_MSG XCB_INCLUDE_DIR XCB_LIBRARIES)
mark_as_advanced(XCB_INCLUDE_DIR XCB_LIBRARIES)