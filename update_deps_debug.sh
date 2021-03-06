#!/bin/bash
DEAR_PATH=`pwd`
THIRD_PARTY_PATH=`pwd`/third_party
rm -rf ${THIRD_PARTY_PATH}/*

# VulkanTool
git clone git@github.com:LunarG/VulkanTools.git "${THIRD_PARTY_PATH}/VulkanTool-tmp"
cd "${THIRD_PARTY_PATH}/VulkanTool-tmp"
git checkout sdk-1.0.61
sed -i 's/create_spirv-tools/create_spirv_tools/g' update_external_sources.sh
sed -i 's/update_spirv-tools/update_spirv_tools/g' update_external_sources.sh
sed -i 's/build_spirv-tools/build_spirv_tools/g' update_external_sources.sh
sh update_external_sources.sh
mkdir build
cd build
cmake -DCMAKE_INSTALL_PREFIX="${THIRD_PARTY_PATH}/VulkanTool" -DCMAKE_BUILD_TYPE='Debug' ..
make install -j4
mv ${THIRD_PARTY_PATH}/VulkanTool-tmp/external/glslang/build/install/bin/glslangValidator ${DEAR_PATH}/tools
rm -rf ${THIRD_PARTY_PATH}/VulkanTool-tmp ${THIRD_PARTY_PATH}/VulkanTool/bin 

# Skia
cd "${THIRD_PARTY_PATH}"
git clone https://skia.googlesource.com/skia.git "${THIRD_PARTY_PATH}/Skia"
cd "${THIRD_PARTY_PATH}/Skia"
python tools/git-sync-deps
git clone 'https://chromium.googlesource.com/chromium/tools/depot_tools.git'
export PATH="${THIRD_PARTY_PATH}/Skia/depot_tools:${PATH}"
bin/gn gen out/Static --args='is_official_build=true'
echo skia_vulkan_sdk = "\"${THIRD_PARTY_PATH}/VulkanTool\"" >> out/Static/args.gn
echo skia_use_libwebp = false >> out/Static/args.gn
echo skia_use_libjpeg_turbo = false >> out/Static/args.gn
ninja -C out/Static