rm -rf build

cmake -DDEVICE_NAME="active_scanner" \
    -DCMAKE_BUILD_TYPE=DEBUG \
    -DCMAKE_TOOLCHAIN_FILE="/root/SDK6.1182.1/projects/BLE_SDK6_examples/build_utils/gcc/clang.cmake" \
    -DDIALOG_SDK_PATH="/root/SDK6.1182.1" \
    -DDIALOG_EXAMPLE_PATH="/root/SDK6.1182.1/projects/BLE_SDK6_examples" \
    -S "." \
    -B "build"

cd build
make -j 7