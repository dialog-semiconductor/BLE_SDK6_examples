rm -rf build

cmake -DDEVICE_NAME="advertising_example" \
    -DCMAKE_BUILD_TYPE=DEBUG \
    -DCMAKE_TOOLCHAIN_FILE="/root/SDK6.1182.1/projects/BLE_SDK6_examples/build_utils/gcc/arm-none-eabi.cmake" \
    -DGCC_TOOLCHAIN_PATH="/usr/bin/arm-none-eabi-gcc" \
    -DDIALOG_SDK_PATH="/root/SDK6.1182.1" \
    -DDIALOG_EXAMPLE_PATH="/root/SDK6.1182.1/projects/BLE_SDK6_examples" \
    -S "." \
    -B "build"


cd build
make -j 7