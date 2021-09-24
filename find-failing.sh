#!/bin/bash

EX_LIST=`find . -name "CMakeLists.txt"`

PASSED_531=()
FAILED_531=()
PASSED_585=()
FAILED_585=()
PASSED_586=()
FAILED_586=()

for d in $EX_LIST ;do
    EX_FOLDER=`dirname $d`
    EX_NAME=`basename $EX_FOLDER`
    if [ ! -f "$EX_FOLDER/build/${EX_NAME}_531.elf" ]; then
        FAILED_531+=( "$EX_FOLDER" )
    else
        PASSED_531+=( "$EX_FOLDER" )
    fi
    if [ ! -f "$EX_FOLDER/build/${EX_NAME}_585.elf" ]; then
        FAILED_585+=( "$EX_FOLDER" )
    else
        PASSED_585+=( "$EX_FOLDER" )
    fi
    if [ ! -f "$EX_FOLDER/build/${EX_NAME}_586.elf" ]; then
        FAILED_586+=( "$EX_FOLDER" )
    else
        PASSED_586+=( "$EX_FOLDER" )
    fi
done

echo -e "Success build 531:\n"
for d in "${PASSED_531[@]}" ;do
    echo " - $d"
done

echo -e "\nFailed build 531:\n"
for d in "${FAILED_531[@]}" ;do
    echo " - $d"
done

echo -e "\nSuccess build 585:\n"
for d in "${PASSED_585[@]}" ;do
    echo " - $d"
done

echo -e "\nFailed build 585:\n"
for d in "${FAILED_585[@]}" ;do
    echo " - $d"
done

echo -e "\nSuccess build 586:\n"
for d in "${PASSED_586[@]}" ;do
    echo " - $d"
done

echo -e "\nFailed build 586:\n"
for d in "${FAILED_586[@]}" ;do
    echo " - $d"
done