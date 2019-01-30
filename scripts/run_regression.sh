#!/bin/bash

BASE=$HOME'/poc'

INPUT_DIR="loo_data"
OUTPUT_DIR="loo_output"

DATASET=$1

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


# for DIR in "${BASE}/${INPUT_DIR}/${DATASET}/"; do
#     echo $DIR;
DIR="${BASE}/${INPUT_DIR}/${DATASET}/";
echo $DIR;
ls $DIR | while read -r FILE ; do
    echo "Processing ${FILE}"
    BASEFILE=$(basename ${FILE%.*})
    $BASE/python_scripts/patmin_regression.py $FILE "${BASE}/Y_data/Y_${DATASET}.txt" "${BASE}/${OUTPUT_DIR}/" &> "${DATASET}_${BASEFILE}.log"
done
# done
