#!/bin/bash

PA=$HOME'/poc'
TEST_VERSION="$1"


INPUT_DIR="data"

BASE=$PA/$INPUT_DIR


# # parse the options
# while getopts 'd:v:h' opt ; do
#   case $opt in
#     d) DATA_SETS=("$OPTARG");;
#     v) TEST_VERSION=$OPTARG; USE_ALL=false ;;
#     h) echo "Options: -d data sets (default: ${DATA_SETS[*]})"
#        echo "         -v test version (default: Use all)";
#        exit 0 ;;
#   esac
# done

# # skip over the processed options
# shift $((OPTIND-1)) 


for DIR in "${BASE}/*/"; do
    echo $DIR;

    ls $DIR | grep X | while read -r FILE ; do
        echo "Processing ${FILE}"
        $PA/split.py $DIR/$FILE
    done
done
