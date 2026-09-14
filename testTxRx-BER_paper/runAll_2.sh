#!/bin/bash
set -e

N_RUNS=50
USER_LIST="100"

CONFIG_FILE="config.h"

# Communities still to run
COMMUNITIES="10"

# Alpha values
ALPHAS="0"

BASE_PATH="/media/sf_Shared/fixed com"


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
# LOOP OVER NUMBER OF COMMUNITIES
# =====================================================
for COM in $COMMUNITIES
do

    echo ""
    echo "#####################################################"
    echo "############ COMMUNITY = $COM #######################"
    echo "#####################################################"
    echo ""

    # =================================================
    # Fixed paths for this community experiment
    # =================================================
    PROBA_REQUEST_PATH="$BASE_PATH/${COM}com/1st/user_preferences.csv"
    REQUEST_PATH="$BASE_PATH/${COM}com/1st/combined_requests.csv"
    COMMUNITY_ID_PATH="$BASE_PATH/${COM}com/1st/selected_users_mapping.csv"

    # =================================================
    # Change fixed paths in config.h
    # =================================================
    sed -i \
        "s|^#define PROBA_PATH_REQUEST.*|#define PROBA_PATH_REQUEST \"$PROBA_REQUEST_PATH\"|" \
        "$CONFIG_FILE"

    sed -i \
        "s|^#define REQUEST_PATH.*|#define REQUEST_PATH \"$REQUEST_PATH\"|" \
        "$CONFIG_FILE"
    sed -i \
        "s|^#define COMMUNITY_ID_PATH.*|#define COMMUNITY_ID_PATH \"$COMMUNITY_ID_PATH\"|" \
        "$CONFIG_FILE"

    # =================================================
    # LOOP OVER ALPHA
    # =================================================
    for ALPHA in $ALPHAS
    do

        echo ""
        echo "====================================================="
        echo " COMMUNITY = $COM"
        echo " ALPHA     = $ALPHA"
        echo " STRATEGY  = GLOBAL"
        echo "====================================================="
        echo ""

        # =============================================
        # Cache path changes with alpha                global_popularity_all_communities
        # =============================================
        CACHE_PATH="$BASE_PATH/${COM}com/1st/alpha/com/cache_alpha_0.5.csv"


        # =============================================
        # Result CSV also changes with alpha
        # =============================================
        RESULT_FILE="com_${COM}_.5_com.csv"


        # =============================================
        # Update cache path in config.h
        # =============================================
        sed -i \
            "s|^#define PROBA_PATH_CACHE.*|#define PROBA_PATH_CACHE \"$CACHE_PATH\"|" \
            "$CONFIG_FILE"


        # =============================================
        # Check input files before running
        # =============================================
        if [ ! -f "$PROBA_REQUEST_PATH" ]
        then
            echo "ERROR: Cannot find:"
            echo "$PROBA_REQUEST_PATH"
            exit 1
        fi

        if [ ! -f "$REQUEST_PATH" ]
        then
            echo "ERROR: Cannot find:"
            echo "$REQUEST_PATH"
            exit 1
        fi

        if [ ! -f "$CACHE_PATH" ]
        then
            echo "ERROR: Cannot find:"
            echo "$CACHE_PATH"
            exit 1
        fi


        echo "Request probabilities:"
        echo "$PROBA_REQUEST_PATH"

        echo "Cache probabilities:"
        echo "$CACHE_PATH"

        echo "Requests:"
        echo "$REQUEST_PATH"

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
            HEADER="$HEADER,BuildAndRunTimeSeconds_$USERS"
        done

        echo "$HEADER" > "$RESULT_FILE"


        # =====================================================
        # RUN EXPERIMENTS
        # =====================================================
        for RUN in $(seq 1 $N_RUNS)
        do
            echo ""
            echo "========== Community $COM | Alpha $ALPHA | Run $RUN =========="

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


                # ===============================================
                # 1) BUILD + RUN ENVIRONMENT
                # ===============================================
                 START_TIME=$(date +%s.%N)
                cd environment

                make clean || true
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

                make clean || true
                CACHE_OUTPUT=$(make 2>&1)

                cd ..


                # ===============================================
                # 3) BUILD + RUN TESTTXRX
                # ===============================================
                cd testTxRx

                make clean || true
                OUTPUT=$(make 2>&1)

                cd ..
                END_TIME=$(date +%s.%N)

                ELAPSED_SECONDS=$(
                    LC_ALL=C awk \
                        -v start="$START_TIME" \
                        -v end="$END_TIME" \
                        'BEGIN { printf "%.3f", end - start }'
                )


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
                echo "Build and run time = $ELAPSED_SECONDS seconds"

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
                LINE="$LINE,$ELAPSED_SECONDS" 

            done

            echo "$LINE" >> "$RESULT_FILE"

        done


        echo ""
        echo "====================================================="
        echo "FINISHED:"
        echo "Community = $COM"
        echo "Alpha     = $ALPHA"
        echo "Results   = $RESULT_FILE"
        echo "====================================================="
        echo ""

    done
done


echo ""
echo "#####################################################"
echo "ALL EXPERIMENTS COMPLETED"
echo "#####################################################"