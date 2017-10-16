find_package(PkgConfig)
pkg_check_modules(PKG_LIBUDEV REQUIRED libudev)

find_path(LIBUDEV_INCLUDE_DIR libudev.h ${PKG_LIBUDEV_INCLUDE_DIRS})

find_library(LIBUDEV_LIBRARIES NAMES udev PATHS ${PKG_LIBUDEV_LIBRARY_DIRS})

set(LIBUDEV_FOUND TRUE)

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(LIBUDEV DEFAULT_MSG LIBUDEV_INCLUDE_DIR LIBUDEV_LIBRARIES)
mark_as_advanced(LIBUDEV_INCLUDE_DIR LIBUDEV_LIBRARIES)