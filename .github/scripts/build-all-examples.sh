#!/bin/bash

BUILD_DIR="build"


GCC_PATH=`which arm-none-eabi-gcc`
if [ -L $GCC_PATH ]; then
    GCC_TOOLCHAIN_PATH=`readlink $GCC_PATH | xargs dirname`
else
    GCC_TOOLCHAIN_PATH=$GCC_PATH
fi

EXAMPLE_ROOT=$(dirname $(dirname $( cd -- "$( dirname -- "${BASH_SOURCE[0]}" )" &> /dev/null && pwd )))

echo "example root: $EXAMPLE_ROOT"

tree -L 2

if [ -z $SDKROOT ]; then
    SDKROOT="${GITHUB_WORKSPACE}"
fi

# compile a list of files that can be built
if [ "$#" -ne 1 ]; then
    BUILD_LIST=`cd $EXAMPLE_ROOT && find . -name "CMakeLists.txt"`
else
    BUILD_LIST="$1/project_evioronment"
fi

# write build list to file
printf "%s\n" "${BUILD_LIST[@]}" > $EXAMPLE_ROOT/.github/config/build-list.txt

# read ignore list file
IGNORE_LIST=$(<$EXAMPLE_ROOT/.github/config/ignore-list.txt)

# subtract ignore list from build list
BUILD_LIST=$(comm -13 <(sort $EXAMPLE_ROOT/.github/config/ignore-list.txt) <(sort $EXAMPLE_ROOT/.github/config/build-list.txt))

# write updated build list to file
printf "%s\n" "${BUILD_LIST[@]}" > $EXAMPLE_ROOT/.github/config/build-list.txt


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
