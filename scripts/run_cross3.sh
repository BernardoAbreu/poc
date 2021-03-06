#!/bin/bash

PA=$HOME'/poc'

INPUT_DIR="loo_data"
OUTPUT_DIR="loo_output"

BASE=$PA/$INPUT_DIR
POTENTIAL="C"
DATA_TYPE="TP"
KS=(10 8 6 4 3 2)
KS=(2)


# K=(1 2);
# K=(3 4);
# K=(5 6 7);
# K=(8 9 10);
# K=(11 12 13);
# K=(14 15 16);
K=(17)


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

COLS="$(cat ${PA}/data/${DATA_TYPE}/Y_${DATA_TYPE}.txt | wc -l)";
HALF_COLS="$(( ($COLS + 1) / 2 + 1))";

# for DIR in $BASE/*/; do
for N in "${KS[@]}"; do
    BASEDIR="T_${POTENTIAL}_X_${DATA_TYPE}_t${N}";
    DIR=$BASE/$BASEDIR;
    echo "Directory ${DIR}";
    # BASEDIR=$(basename ${DIR});
    for OUTMOL in $(seq 1 $COLS); do
        FILE="${BASEDIR}_Xtrain_${OUTMOL}";
        echo "Processing ${FILE}"
        COL=$(awk '{print NF}' "${DIR}/${FILE}" | head -n1);
        COL="$(( ($COL - 1) / 2))";
        # for k in $(seq 1 $COL); do
        for k in "${K[@]}"; do
                mkdir -p $PA/$OUTPUT_DIR/$BASEDIR/$k
                BASEFILE=$(basename ${FILE%.*});
                echo "k=${k} col=${COL} ${BASEFILE}"
                "${PA}/source/poc" -f "${DIR}/${FILE}" -k "${k}" -o "${PA}/${OUTPUT_DIR}/${BASEDIR}/${k}/${BASEFILE}"
        done
    done
done
