find_package(PkgConfig)
pkg_check_modules(PKG_XML REQUIRED libxml-2.0)

find_path(XML_INCLUDE_DIR libxml/parser.h ${PKG_XML_INCLUDE_DIRS})

find_library(XML_LIBRARIES NAMES input PATHS ${PKG_XML_LIBRARY_DIRS})

set(XML_FOUND TRUE)

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(XML DEFAULT_MSG XML_INCLUDE_DIR XML_LIBRARIES)
mark_as_advanced(XML_INCLUDE_DIR XML_LIBRARIES)