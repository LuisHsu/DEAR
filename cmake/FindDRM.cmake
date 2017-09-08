find_package(PkgConfig)
pkg_check_modules(PC_DRM REQUIRED libdrm)
find_library(DRM_LIBRARIES NAMES drm HINTS ${PC_DRM_LIBRARY_DIRS})
find_path(DRM_INCLUDE_DIRS NAMES drm.h PATH_SUFFIXES libdrm drm HINTS ${PC_DRM_INCLUDE_DIRS})

set(DRM_DEFINITIONS ${PC_DRM_CFLAGS_OTHER})

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(DRM DEFAULT_MSG DRM_INCLUDE_DIRS DRM_LIBRARIES)
mark_as_advanced(DRM_INCLUDE_DIRS DRM_LIBRARIES DRM_DEFINITIONS)