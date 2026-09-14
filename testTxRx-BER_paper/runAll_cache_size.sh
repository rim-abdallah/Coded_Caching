#!/bin/bash
set -e

N_RUNS=100
USER_LIST="100"
CACHE_LIST="5"

CONFIG_FILE="config.h"
RESULT_FILE="results_top_200_requestANDcache_global.csv"

# =====================================================
# FUNCTION: EXTRACT A VALUE FROM TESTTXRX OUTPUT
#
# Example:
# extract_value "$OUTPUT" "OverallGain"
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

    # Prevent an empty CSV cell when a metric is undefined
    # or the expected output line was not found.
    if [ -z "$VALUE" ]
    then
        VALUE="NA"
    fi

    echo "$VALUE"
}


# =====================================================
# CREATE CSV HEADER
# =====================================================

HEADER="Run"

for CACHE in $CACHE_LIST
do
    for USERS in $USER_LIST
    do
        HEADER="$HEADER,Color_${CACHE}%_${USERS}U"
        HEADER="$HEADER,Nodes_${CACHE}%_${USERS}U"

        HEADER="$HEADER,TNoCache_${CACHE}%_${USERS}U"
        HEADER="$HEADER,TUncoded_${CACHE}%_${USERS}U"
        HEADER="$HEADER,TCoded_${CACHE}%_${USERS}U"

        HEADER="$HEADER,OverallGain_${CACHE}%_${USERS}U"
        HEADER="$HEADER,LocalCacheDistribution_${CACHE}%_${USERS}U"
        HEADER="$HEADER,LocalCacheContribution_${CACHE}%_${USERS}U"

        # Keep the old expected gain for comparison
        HEADER="$HEADER,OldExpectedGain_${CACHE}%_${USERS}U"
    done
done

echo "$HEADER" > "$RESULT_FILE"


# =====================================================
# RUN EXPERIMENTS
# =====================================================

for RUN in $(seq 1 $N_RUNS)
do
    echo "========== Run $RUN =========="

    sed -i \
        "s/^#define Run_ID.*/#define Run_ID $RUN/" \
        "$CONFIG_FILE"

    LINE="$RUN"

    for CACHE in $CACHE_LIST
    do
        echo "===== Cache = $CACHE ====="

        sed -i \
            "s/^#define CACHE_SIZE.*/#define CACHE_SIZE $CACHE/" \
            "$CONFIG_FILE"

        for USERS in $USER_LIST
        do
            echo "----- Users = $USERS -----"

            sed -i \
                "s/^#define N_USERS.*/#define N_USERS $USERS/" \
                "$CONFIG_FILE"


            # ===============================
            # 1) BUILD + RUN ENVIRONMENT
            #
            # Creates or reads the requests,
            # generates environment_file,
            # and calculates T_no_cache.
            # ===============================

            cd environment

            make clean || true
            ENV_OUTPUT=$(make 2>&1)

            cd ..


            # =====================================================
            # COPY T_NO_CACHE FILE
            #
            # Environment.cpp writes:
            #     environment/t_no_cache.txt
            #
            # TestTxRx.cpp currently reads:
            #     testTxRx/t_no_cache.txt
            # =====================================================

            if [ ! -f "environment/t_no_cache.txt" ]
            then
                echo "Error: environment/t_no_cache.txt was not created."
                echo "$ENV_OUTPUT"
                exit 1
            fi

            cp \
                "environment/t_no_cache.txt" \
                "testTxRx/t_no_cache.txt"


            # ===============================
            # 2) BUILD + RUN CACHE
            # ===============================

            cd cache

            make clean || true
            CACHE_OUTPUT=$(make 2>&1)

            cd ..


            # ===============================
            # 3) BUILD + RUN TESTTXRX
            # ===============================

            cd testTxRx

            make clean || true
            OUTPUT=$(make 2>&1)

            cd ..


            # =====================================================
            # READ THE SIMPLE OUTPUT LINES
            #
            # TestTxRx prints:
            #
            # Nodes:
            # Colors:
            # TNoCache:
            # TUncoded:
            # TCoded:
            # OverallGain:
            # LocalCacheDistribution:
            # LocalCacheContribution:
            # =====================================================

            COLOR=$(extract_value "$OUTPUT" "Colors")
            NODES=$(extract_value "$OUTPUT" "Nodes")

            T_NO_CACHE=$(extract_value "$OUTPUT" "TNoCache")
            T_UNCODED=$(extract_value "$OUTPUT" "TUncoded")
            T_CODED=$(extract_value "$OUTPUT" "TCoded")

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


            # =====================================================
            # READ THE OLD EXPECTED GAIN
            #
            # This output contains a percentage symbol, so remove it.
            # =====================================================

            OLD_GAIN=$(
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

            if [ -z "$OLD_GAIN" ]
            then
                OLD_GAIN="NA"
            fi


            # =====================================================
            # PRINT CURRENT RESULT
            # =====================================================

            echo "Color = $COLOR"
            echo "Nodes = $NODES"

            echo "T_no_cache = $T_NO_CACHE"
            echo "T_uncoded = $T_UNCODED"
            echo "T_coded = $T_CODED"

            echo "Overall gain = $OVERALL_GAIN"s
            echo "Local cache distribution = $LOCAL_CACHE_DISTRIBUTION"
            echo "Local cache contribution = $LOCAL_CACHE_CONTRIBUTION"
            echo "Old expected gain = $OLD_GAIN"


            # =====================================================
            # ADD VALUES TO THE CURRENT CSV ROW
            # =====================================================

            LINE="$LINE,$COLOR"
            LINE="$LINE,$NODES"

            LINE="$LINE,$T_NO_CACHE"
            LINE="$LINE,$T_UNCODED"
            LINE="$LINE,$T_CODED"

            LINE="$LINE,$OVERALL_GAIN"
            LINE="$LINE,$LOCAL_CACHE_DISTRIBUTION"
            LINE="$LINE,$LOCAL_CACHE_CONTRIBUTION"

            LINE="$LINE,$OLD_GAIN"
        done
    done

    echo "$LINE" >> "$RESULT_FILE"
done


echo "Execution completed."
echo "Results saved in $RESULT_FILE"