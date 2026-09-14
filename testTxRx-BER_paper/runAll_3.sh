#!/bin/bash
set -e

N_RUNS=50
USER_LIST="100"

CONFIG_FILE="config.h"
RESULT_FILE="zipf.csv"


# =====================================================
# Extract a value from the simple TestTxRx output lines
#
# Example:
# OverallGain: 0.752000
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
    echo "========== Run $RUN =========="

    sed -i \
        "s/^#define Run_ID.*/#define Run_ID $RUN/" \
        "$CONFIG_FILE"

    LINE="$RUN"

    for USERS in $USER_LIST
    do
        echo "----- Users = $USERS -----"

        sed -i \
            "s/^#define N_USERS.*/#define N_USERS $USERS/" \
            "$CONFIG_FILE"


        # ===============================
        # 1) BUILD + RUN ENVIRONMENT
        #
        # Creates environment_file and
        # calculates t_no_cache.txt
        # ===============================
        cd environment

        make clean || true
        ENV_OUTPUT=$(make 2>&1)

        cd ..


        # ===============================
        # COPY T_NO_CACHE TO TESTTXRX
        # ===============================
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


        # ===============================
        # OLD OUTPUTS
        # ===============================
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


        # ===============================
        # TRANSMISSION VALUES
        # ===============================
        T_NO_CACHE=$(extract_value "$OUTPUT" "TNoCache")
        T_UNCODED=$(extract_value "$OUTPUT" "TUncoded")
        T_CODED=$(extract_value "$OUTPUT" "TCoded")


        # ===============================
        # NEW METRICS
        # ===============================
        OVERALL_GAIN=$(
            extract_value \
                "$OUTPUT" \
                "OverallGain"
        )

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

        # ===============================
        # DISPLAY CURRENT RESULT
        # ===============================
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


        # ===============================
        # ADD VALUES TO CSV ROW
        # ===============================
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


echo "Execution completed."
echo "Results saved in $RESULT_FILE"