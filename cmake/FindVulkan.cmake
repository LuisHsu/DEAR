
find_path(VULKAN_INCLUDE_DIR NAMES vulkan/vulkan.h HINTS
"/usr/local/include")
find_library(VULKAN_LIBRARIES NAMES vulkan HINTS
"/usr/local/lib")

set(VULKAN_FOUND TRUE)

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(VULKAN DEFAULT_MSG VULKAN_INCLUDE_DIR VULKAN_LIBRARIES)
mark_as_advanced(VULKAN_INCLUDE_DIR VULKAN_LIBRARIES)