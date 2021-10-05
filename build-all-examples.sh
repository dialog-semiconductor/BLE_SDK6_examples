#!/bin/bash

BUILD_DIR="build"


GCC_PATH=`which arm-none-eabi-gcc`
if [ -L $GCC_PATH ]; then
    GCC_TOOLCHAIN_PATH=`readlink $GCC_PATH | xargs dirname`
else
    GCC_TOOLCHAIN_PATH=$GCC_PATH
fi

EXAMPLE_ROOT=$PWD

if [ -z $SDKROOT ]; then
    SDKROOT="${HOME}/dev/repo/SDK6_release"
fi

if [ "$#" -ne 1 ]; then
    BUILD_LIST=`find . -name "CMakeLists.txt"`
else
    BUILD_LIST="$1/project_evioronment"
fi


for d in $BUILD_LIST; do


    pushd `dirname $d`

    # echo "push to folder: $PWD"
    APP_NAME=`basename $PWD`

    echo "Building $APP_NAME"

    # Cleanup build directory
    if [ -d "$BUILD_DIR" ]; then
        echo "Cleaning build dir"
        rm -rf $BUILD_DIR
        if [ ! $? -eq 0 ]
        then
            echo "rm failed"
            exit 1
        fi
        mkdir $BUILD_DIR
        if [ ! $? -eq 0 ]
        then
            echo "mkdir failed"
            exit 1
        fi
    fi

    cmake -DDEVICE_NAME=$APP_NAME \
        -DCMAKE_BUILD_TYPE=DEBUG \
        -DCMAKE_TOOLCHAIN_FILE=$EXAMPLE_ROOT/build_utils/gcc/arm-none-eabi.cmake \
        -DGCC_TOOLCHAIN_PATH="$GCC_TOOLCHAIN_PATH" \
        -DDIALOG_SDK_PATH=$SDKROOT \
        -DDIALOG_EXAMPLE_PATH=$EXAMPLE_ROOT \
        -S . \
        -B $BUILD_DIR

    pushd $BUILD_DIR
    make -j 7
    popd
    popd
    done
