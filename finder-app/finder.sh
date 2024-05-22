#!/bin/bash
# Date: 5/15/2024
# Author: Talha Abdulaziz

echo "filesdir entered: $1"
echo "searchstr enered: $2"

filesdir=$1
searchstr=$2

if [ -z $filesdir ] || [ -z $searchstr ]
then
    echo "Missing argument. Please enter the following arguments"
    echo "- File directory"
    echo "- Search string"
    exit 1
else
    echo "Starting search for $searchstr in directory $filesdir"
fi

if [ -d $filesdir ]
then
    echo ...
else
    echo "$filesdir is not recognized as a directory in the filesystem"
    exit 1
fi

matching_files=$(find $filesdir -type f | wc -l)
matching_lines=$(grep -r $searchstr $filesdir | wc -l)

echo "The number of files are $matching_files and the number of matching lines are $matching_lines"

# echo $file_count
# echo $line_count