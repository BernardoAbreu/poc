#!/bin/bash

PA=$HOME'/poc'

INPUT_DIR="test_data"
OUTPUT_DIR="loo_data"

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


for DIR in "${BASE}/"; do
    echo $DIR;

    ls $DIR | grep X | while read -r FILE ; do
        echo "Processing ${FILE}"
        COL=$(awk '{print NF}' "${DIR}/${FILE}" | head -n1)
        # COL="$(( ($COL - 1) / 2))"
        BASEFILE=$(basename ${FILE%.*})
        mkdir -p "${PA}/${OUTPUT_DIR}/${BASEFILE}"
        for k in $(seq 1 $COL); do
                echo "k=${k} col=${COL} ${BASEFILE}"
                cut -f $k --complement -d' ' $DIR/$FILE > "${PA}/${OUTPUT_DIR}/${BASEFILE}/${BASEFILE}_Xtrain_${k}"
                cut -f $k -d' ' $DIR/$FILE > "${PA}/${OUTPUT_DIR}/${BASEFILE}/${BASEFILE}_Xtest_${k}"
                # sed "${k}d" $DIR/$FILE > "${PA}/${OUTPUT_DIR}/${BASEFILE}__${k}"
                # $PA/poc -f $DIR/$FILE -k $k -o $PA/$OUTPUT_DIR/$BASEFILE
        done
    done
done
