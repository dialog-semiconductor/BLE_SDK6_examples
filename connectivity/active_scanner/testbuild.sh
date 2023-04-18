rm -r build

cmake -DDEVICE_NAME="active_scanner" \
    -DCMAKE_BUILD_TYPE=DEBUG \
    -DCMAKE_TOOLCHAIN_FILE="/root/lpc_cs_sdk6/projects/BLE_SDK6_examples/build_utils/gcc/clang.cmake" \
    -DDIALOG_SDK_PATH="/root/lpc_cs_sdk6" \
    -DDIALOG_EXAMPLE_PATH="/root/lpc_cs_sdk6/projects/BLE_SDK6_examples" \
    -S "." \
    -B "build"

cd build
make -j 7