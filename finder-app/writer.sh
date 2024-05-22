#!/bin/bash
# Date: 5/21/2024
# Author: Talha Abdulaziz

echo "writefile entered: $1"
echo "writestr enered: $2"

writefile=$1
writestr=$2

if [ -z $writefile ] || [ -z $writestr ]
then
    echo "Missing argument. Please enter the following arguments"
    echo "- File path and name"
    echo "- String to write"
    exit 1
else
    echo "Creating file $writefile in and inserting $writestr"
fi

true_path=${writefile%/*}

if [ -d $true_path ]
then
    echo "Located existing path for $true_path"
    echo
else
    echo "$writefile does not exist inside a directory, attempting to create path"
    echo
    mkdir -p $true_path
fi

echo $writestr > $writefile

echo "Successfully inserted $writestr into $writefile, attempting to read back file"

cat $writefile