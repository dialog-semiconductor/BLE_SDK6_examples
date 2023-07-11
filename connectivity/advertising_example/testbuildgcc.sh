rm -rf ./build-gcc

cmake -DDEVICE_NAME="advertising_example" \
    -DCMAKE_BUILD_TYPE=DEBUG \
    -DCMAKE_TOOLCHAIN_FILE="/root/SDK6.1182.1/projects/BLE_SDK6_examples/build_utils/gcc/arm-none-eabi.cmake" \
    -DGCC_TOOLCHAIN_PATH="/usr/bin/arm-none-eabi-gcc" \
    -DDIALOG_SDK_PATH="/root/SDK6.1182.1" \
    -DDIALOG_EXAMPLE_PATH="/root/SDK6.1182.1/projects/BLE_SDK6_examples" \
    -DCOMPILER="gcc" \
    -S "." \
    -B "build-gcc"


cd ./build-gcc
make -j 7

objdump -ht advertising_example_531.elf > objdump-gcc.txt
ls -lah advertising_example_531.* > ls-gcc.txt