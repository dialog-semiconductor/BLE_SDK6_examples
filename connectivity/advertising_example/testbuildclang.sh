rm -rf ./build-clang

cmake -DDEVICE_NAME="advertising_example" \
    -DCMAKE_BUILD_TYPE=DEBUG \
    -DCMAKE_TOOLCHAIN_FILE="/root/SDK6.1182.1/projects/BLE_SDK6_examples/build_utils/clang/arm-none-eabi.cmake" \
    -DDIALOG_SDK_PATH="/root/SDK6.1182.1" \
    -DDIALOG_EXAMPLE_PATH="/root/SDK6.1182.1/projects/BLE_SDK6_examples" \
    -DCOMPILER="clang" \
    -S "." \
    -B "build-clang"

cd ./build-clang
make -j 7

llvm-objdump-15 -ht advertising_example_531.elf > objdump.txt
ls -lah advertising_example_531.elf > ls.txt