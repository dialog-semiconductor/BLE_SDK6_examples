#!/bin/bash

EX_LIST=`find . -name "CMakeLists.txt"`

PASSED=()
FAILED=()

for d in $EX_LIST ;do
    EX_FOLDER=`dirname $d`
    EX_NAME=`basename $EX_FOLDER`
    if [ ! -f "$EX_FOLDER/build/$EX_NAME.elf" ]; then
        FAILED+=( "$EX_FOLDER" )
    else
        PASSED+=( "$EX_FOLDER" )
    fi
done

echo -e "Success build:\n"
for d in "${PASSED[@]}" ;do
    echo " - $d"
done

echo -e "\nFailed build:\n"
for d in "${FAILED[@]}" ;do
    echo " - $d"
done

echo -e "\n\n"
echo -e "---------------"
echo -e "| PASSED | ${#PASSED[@]} |"
echo -e "|-------------|"
echo -e "| FAILED | ${#FAILED[@]} |"
echo -e "---------------"