#!/bin/bash
set -e


# =====================================================
# SETTINGS
# =====================================================

N_RUNS=50
USER_LIST="70"

CONFIG_FILE="config.h"

TOPK_VALUES="75 100 150 200 300 500 800 1000"

BASE_PATH="/media/sf_Shared/top_k_redo/top_k"


# =====================================================
# Extract value from TestTxRx output
# =====================================================

extract_value()
{
    local OUTPUT_TEXT="$1"
    local LABEL="$2"
    local VALUE

    VALUE=$(
        echo "$OUTPUT_TEXT" |
        awk -F': *' -v label="$LABEL" '
            $1 == label {
                print $2
            }
        ' |
        tail -1
    )

    if [ -z "$VALUE" ]
    then
        VALUE="NA"
    fi

    echo "$VALUE"
}


# =====================================================
# LOOP OVER TOP-K VALUES
# =====================================================

for K in $TOPK_VALUES
do

    echo ""
    echo "#####################################################"
    echo "################ TOP-K = $K #########################"
    echo "#####################################################"
    echo ""


    # =================================================
    # ONLY CACHE PATH CHANGES
    # =================================================

    CACHE_PATH="$BASE_PATH/personalized_top_${K}.csv"

    RESULT_FILE="top_k_${K}.csv"


    # =================================================
    # CHANGE ONLY PROBA_PATH_CACHE
    # =================================================

    sed -i \
        "s|^#define PROBA_PATH_CACHE.*|#define PROBA_PATH_CACHE \"$CACHE_PATH\"|" \
        "$CONFIG_FILE"


    # =================================================
    # CHECK CACHE FILE
    # =================================================

    if [ ! -f "$CACHE_PATH" ]
    then
        echo "ERROR: Cannot find:"
        echo "$CACHE_PATH"
        exit 1
    fi


    echo "Cache probabilities:"
    echo "$CACHE_PATH"

    echo "Output:"
    echo "$RESULT_FILE"


    # =====================================================
    # CREATE CSV HEADER
    # =====================================================

    HEADER="run"

    for USERS in $USER_LIST
    do
        HEADER="$HEADER,gain_$USERS"
        HEADER="$HEADER,colors_$USERS"
        HEADER="$HEADER,nodes_$USERS"

        HEADER="$HEADER,TNoCache_$USERS"
        HEADER="$HEADER,TUncoded_$USERS"
        HEADER="$HEADER,TCoded_$USERS"

        HEADER="$HEADER,OverallGain_$USERS"
        HEADER="$HEADER,LocalCacheDistribution_$USERS"
        HEADER="$HEADER,LocalCacheContribution_$USERS"
        HEADER="$HEADER,MulticastGain_$USERS"
        HEADER="$HEADER,MulticastContribution_$USERS"
    done

    echo "$HEADER" > "$RESULT_FILE"


    # =====================================================
    # RUN EXPERIMENTS
    # =====================================================

    for RUN in $(seq 1 $N_RUNS)
    do

        echo ""
        echo "========== Top-K $K | Run $RUN =========="


        # =================================================
        # CHANGE RUN ID
        # =================================================

        sed -i \
            "s/^#define Run_ID.*/#define Run_ID $RUN/" \
            "$CONFIG_FILE"

        LINE="$RUN"


        for USERS in $USER_LIST
        do

            echo "----- Users = $USERS -----"


            # =============================================
            # CHANGE NUMBER OF USERS
            # =============================================

            sed -i \
                "s/^#define N_USERS.*/#define N_USERS $USERS/" \
                "$CONFIG_FILE"


            # ===============================================
            # 1) BUILD + RUN ENVIRONMENT
            # ===============================================

            cd environment

            make clean >/dev/null 2>&1 || true
            ENV_OUTPUT=$(make 2>&1)

            cd ..


            # ===============================================
            # COPY T_NO_CACHE TO TESTTXRX
            # ===============================================

            if [ ! -f "environment/t_no_cache.txt" ]
            then
                echo "ERROR: environment/t_no_cache.txt was not created."
                echo "$ENV_OUTPUT"
                exit 1
            fi

            cp \
                "environment/t_no_cache.txt" \
                "testTxRx/t_no_cache.txt"


            # ===============================================
            # 2) BUILD + RUN CACHE
            # ===============================================

            cd cache

            make clean >/dev/null 2>&1 || true
            CACHE_OUTPUT=$(make 2>&1)

            cd ..


            # ===============================================
            # 3) BUILD + RUN TESTTXRX
            # ===============================================

            cd testTxRx

            make clean >/dev/null 2>&1 || true
            OUTPUT=$(make 2>&1)

            cd ..


            # ===============================================
            # OLD GAIN
            # ===============================================

            GAIN=$(
                echo "$OUTPUT" |
                awk -F': *' '
                    /The expected gain is/ {
                        value = $2
                        gsub(/%/, "", value)
                        print value
                    }
                ' |
                tail -1
            )

            if [ -z "$GAIN" ]
            then
                GAIN="NA"
            fi


            COLORS=$(extract_value "$OUTPUT" "Colors")
            NODES=$(extract_value "$OUTPUT" "Nodes")


            # ===============================================
            # TRANSMISSION VALUES
            # ===============================================

            T_NO_CACHE=$(extract_value "$OUTPUT" "TNoCache")
            T_UNCODED=$(extract_value "$OUTPUT" "TUncoded")
            T_CODED=$(extract_value "$OUTPUT" "TCoded")


            # ===============================================
            # METRICS
            # ===============================================

            OVERALL_GAIN=$(extract_value "$OUTPUT" "OverallGain")

            LOCAL_CACHE_DISTRIBUTION=$(
                extract_value \
                    "$OUTPUT" \
                    "LocalCacheDistribution"
            )

            LOCAL_CACHE_CONTRIBUTION=$(
                extract_value \
                    "$OUTPUT" \
                    "LocalCacheContribution"
            )

            MULTICAST_GAIN=$(
                extract_value \
                    "$OUTPUT" \
                    "MulticastGain"
            )

            MULTICAST_CONTRIBUTION=$(
                extract_value \
                    "$OUTPUT" \
                    "MulticastContribution"
            )


            # ===============================================
            # DISPLAY RESULT
            # ===============================================

            echo "Old gain = $GAIN"
            echo "Colors = $COLORS"
            echo "Nodes = $NODES"

            echo "T_no_cache = $T_NO_CACHE"
            echo "T_uncoded = $T_UNCODED"
            echo "T_coded = $T_CODED"

            echo "Overall gain = $OVERALL_GAIN"
            echo "Local cache distribution = $LOCAL_CACHE_DISTRIBUTION"
            echo "Local cache contribution = $LOCAL_CACHE_CONTRIBUTION"
            echo "Multicast gain = $MULTICAST_GAIN"
            echo "Multicast contribution = $MULTICAST_CONTRIBUTION"


            # ===============================================
            # ADD VALUES TO CSV ROW
            # ===============================================

            LINE="$LINE,$GAIN"
            LINE="$LINE,$COLORS"
            LINE="$LINE,$NODES"

            LINE="$LINE,$T_NO_CACHE"
            LINE="$LINE,$T_UNCODED"
            LINE="$LINE,$T_CODED"

            LINE="$LINE,$OVERALL_GAIN"
            LINE="$LINE,$LOCAL_CACHE_DISTRIBUTION"
            LINE="$LINE,$LOCAL_CACHE_CONTRIBUTION"
            LINE="$LINE,$MULTICAST_GAIN"
            LINE="$LINE,$MULTICAST_CONTRIBUTION"

        done


        echo "$LINE" >> "$RESULT_FILE"

    done


    echo ""
    echo "====================================================="
    echo "FINISHED:"
    echo "Top-K   = $K"
    echo "Results = $RESULT_FILE"
    echo "====================================================="

done


echo ""
echo "#####################################################"
echo "ALL TOP-K EXPERIMENTS COMPLETED"
echo "#####################################################"