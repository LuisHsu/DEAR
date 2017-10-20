find_package(PkgConfig)
pkg_check_modules(PKG_LIBSYSTEMD REQUIRED libsystemd)

find_path(LIBSYSTEMD_INCLUDE_DIR systemd/sd-login.h ${PKG_LIBSYSTEMD_INCLUDE_DIRS})

find_library(LIBSYSTEMD_LIBRARIES NAMES systemd PATHS ${PKG_LIBSYSTEMD_LIBRARY_DIRS})

set(LIBSYSTEMD_FOUND TRUE)

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(LIBSYSTEMD DEFAULT_MSG LIBSYSTEMD_INCLUDE_DIR LIBSYSTEMD_LIBRARIES)
mark_as_advanced(LIBSYSTEMD_INCLUDE_DIR LIBSYSTEMD_LIBRARIES)