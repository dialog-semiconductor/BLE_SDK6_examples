cmake -DDEVICE_NAME="active_scanner" \
    -DCMAKE_BUILD_TYPE=DEBUG -DCMAKE_TOOLCHAIN_FILE="/root/lpc_cs_sdk6/projects/BLE_SDK6_examples/build_utils/gcc/clang.cmake" \
    -DDIALOG_SDK_PATH="/root/lpc_cs_sdk6" \
    -DDIALOG_EXAMPLE_PATH="/root/lpc_cs_sdk6/projects/BLE_SDK6_examples" \
    -S "/root/lpc_cs_sdk6/projects/BLE_SDK6_examples/connectivity/active_scanner" \
    -B "/root/lpc_cs_sdk6/projects/BLE_SDK6_examples/connectivity/active_scanner/build"

make -f /root/lpc_cs_sdk6/projects/BLE_SDK6_examples/connectivity/active_scanner/CMakeLists.txt -j 7