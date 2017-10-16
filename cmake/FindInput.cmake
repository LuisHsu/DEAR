find_package(PkgConfig)
pkg_check_modules(PKG_LIBINPUT REQUIRED libinput)

find_path(LIBINPUT_INCLUDE_DIR libinput.h ${PKG_LIBINPUT_INCLUDE_DIRS})

find_library(LIBINPUT_LIBRARIES NAMES input PATHS ${PKG_LIBINPUT_LIBRARY_DIRS})

set(LIBINPUT_FOUND TRUE)

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(LIBINPUT DEFAULT_MSG LIBINPUT_INCLUDE_DIR LIBINPUT_LIBRARIES)
mark_as_advanced(LIBINPUT_INCLUDE_DIR LIBINPUT_LIBRARIES)