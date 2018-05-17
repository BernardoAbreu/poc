#!/bin/bash

PA=$HOME'/poc'

INPUT_DIR="loo_data"
OUTPUT_DIR="loo_output"

BASE=$PA/$INPUT_DIR
POTENTIAL="C"
DATA_TYPE="TP"

# parse the options
while getopts 'p:d:h' opt ; do
  case $opt in
    p) POTENTIAL=$OPTARG;;
    d) DATA_TYPE=$OPTARG;;
    h) echo "Options: -d data type (default: ${DATA_TYPE})"
       echo "         -p Potential type (default: C (Coulomb))";
       exit 0 ;;
  esac
done

# skip over the processed options
shift $((OPTIND-1)) 


# for DIR in $BASE/*/; do
for N in 3 4 6 8 10; do
    BASEDIR="T_${POTENTIAL}_X_${DATA_TYPE}_t${N}";
    DIR=$BASE/$BASEDIR;
    echo "Directory ${DIR}";
    # BASEDIR=$(basename ${DIR});
    ls $DIR | grep Xtrain | while read -r FILE ; do
        echo "Processing ${FILE}"
        COL=$(awk '{print NF}' "${DIR}/${FILE}" | head -n1)
        COL="$(( ($COL - 1) / 2))"
        for k in $(seq 1 $COL); do
                mkdir -p $PA/$OUTPUT_DIR/$BASEDIR/$k
                BASEFILE=$(basename ${FILE%.*})
                echo "k=${k} col=${COL} ${BASEFILE}"
                $PA/source/poc -f $DIR/$FILE -k $k -o $PA/$OUTPUT_DIR/$BASEDIR/$k/$BASEFILE
        done
    done
done
