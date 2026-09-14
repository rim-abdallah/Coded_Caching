/* -*- c++ -*- */


#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "CheckFunction.h"
#include "EnvironmentSetup.h"
#include "DecodingInput.h"
#include "ConflictGraph.h"
#include "randomHandler.h"
#include "DataDefinition.h"
#include "grasp.h"
#include "CodingDecodingData.h"
#include "FuncsFromMain.h"
#include "TxRx.h"
//#include "TestTxRx.h"
#include <iomanip>
#include <iostream>
#include <math.h>
#include <cmath>
#include <algorithm>
#include <random>

#include <fstream>
#include <chrono> // Required for timing

// Added for the new transmission metrics
#include <set>
#include <utility>
#include <cstdlib>
#include <string>

#include "../config.h"

#include "aes.h"

using namespace caching;
using namespace std;
using namespace CryptoPP;


/*
 * Read T_no_cache calculated by Environment.cpp.
 *
 * The file contains:
 *
 * T_no_cache =
 * number of distinct requested files * number of chunks per file
 */
unsigned int readTNoCache(const string& filename)
{
    ifstream file(filename.c_str());

    if (!file.is_open())
    {
        cerr << "Error: Cannot open T_no_cache file: "
             << filename << endl;
        exit(EXIT_FAILURE);
    }

    unsigned int T_no_cache = 0;

    if (!(file >> T_no_cache))
    {
        cerr << "Error: Invalid T_no_cache value in: "
             << filename << endl;
        file.close();
        exit(EXIT_FAILURE);
    }

    file.close();
    return T_no_cache;
}


/*
 * Calculate T_uncoded after local caching.
 *
 * T_uncoded is the number of distinct missing packets:
 *
 *     (requested file ID, missing chunk ID)
 *
 * If several users request the same file and need the same
 * chunk, that pair is counted once because one uncoded
 * multicast transmission can serve all these users.
 */
unsigned int calculateTUncoded(const data_matrix& data)
{
    if (data.Q == NULL || data.Q_chuncks == NULL)
    {
        cerr << "Error: Q or Q_chuncks is NULL." << endl;
        exit(EXIT_FAILURE);
    }

    set<pair<int, int> > distinct_missing_chunks;

    for (int user = 0; user < data.n_utenti; user++)
    {
        int requested_file = data.Q[user];

        if (requested_file < 0 || requested_file >= data.m_files)
        {
            cerr << "Error: Invalid requested file ID "
                 << requested_file
                 << " for user "
                 << user
                 << endl;
            exit(EXIT_FAILURE);
        }

        for (int chunk = 0; chunk < data.b_chunks; chunk++)
        {
            /*
             * Q_chuncks[user][chunk] == 1 means that this
             * chunk of the requested file is missing from
             * the user's cache.
             */
            if (data.Q_chuncks[user][chunk] == 1)
            {
                distinct_missing_chunks.insert(
                    make_pair(requested_file, chunk)
                );
            }
        }
    }

    return static_cast<unsigned int>(
        distinct_missing_chunks.size()
    );
}


int main()
{
    int m_files = N_FILES;
    int b_chunks = 100;
    int id_demand = 0;
    int id_user = 0;
    //int size_chunk =42;
    data_matrix d_data;
    cf_data d_outputForColoring;
    bool d_GRASP=true;
    int d_n_col=0;
    int *d_coloring = NULL;
    //bool DEBUG = true;
    unsigned int packet_remain;
    int packet_len = 256;

    header_transmission *d_header_data;

    vector<int> d_spack_size;
    vector< vector<char> > d_coded_data;
    vector<header_polar> d_hX; //this is the main header to be added directly to the transmitted packets

    vector<vector<char> > d_transmission, d_transmission_Enc;
    vector<int> header_len;
    vector<unsigned int> d_small_packet_size, d_small_packet_size_Enc;

    //my variables
    vector< vector<char> > d_coded_data_enc;
    vector<CryptoPP::byte> packet_enc, tempVec, tempKey;

    //srand((unsigned) time(NULL));

    cout << endl << "Data generation process" << endl << "-------------" << endl << endl;

    d_data = generateData(m_files, b_chunks);
    int nb_users = d_data.n_utenti;

    /*
     * T_no_cache was calculated in Environment.cpp after
     * Q_array was filled, whether requests came from the CSV
     * file or from random request generation.
     */
    unsigned int T_no_cache =
        readTNoCache("./t_no_cache.txt");

    /*
     * T_uncoded counts distinct missing (file, chunk) pairs.
     * It is not necessarily equal to the graph-node count,
     * because graph nodes are user-specific.
     */
    unsigned int T_uncoded =
        calculateTUncoded(d_data);

    /*for(int i=0; i<nb_users; i++){
        for(int j=0; j<d_b_chunks; j++)
            cout << d_data.Q_chuncks[i][j]  << ", ";
        cout << endl << endl;
    } */


    cout << endl << "Conflict-Graph generator process" << endl << "-------------" << endl << endl;
    d_outputForColoring = conflictGraphGenerator(d_data);
    cout << endl << "Numero nodi del grafo: " << d_outputForColoring.n_nodi << endl << endl;

    if (d_outputForColoring.n_nodi > 0)
    {
        cout << endl << "Graph Coloring process" << endl << "-------------" << endl << endl;
        // Coloring
        if (d_GRASP)
        {
            int d_maxIter = 20;
            d_coloring = graspGraphColoring(d_maxIter, d_outputForColoring, &d_n_col);
            colorRienumeration(d_n_col, &d_coloring, d_outputForColoring.n_nodi);
        }

        cout << endl << "La colorazione e' stata effettuata con successo!" << endl;
        cout << endl << "Numero di colori utilizzati: " << d_n_col << endl;

        /*
         * OLD CODING-GAIN METRIC:
         *
         * This line is kept because it existed in the old code.
         * It measures the reduction from graph nodes to colors.
         *
         * The calculation was changed to double division to avoid
         * integer-division truncation.
         */
        double old_expected_gain =
            100.0 *
            (
                static_cast<double>(d_outputForColoring.n_nodi) -
                static_cast<double>(d_n_col)
            )
            /
            static_cast<double>(d_outputForColoring.n_nodi);

        cout << "The expected gain is: "
             << old_expected_gain
             << "%"
             << endl; //output needed

    }/*end if (d_outputForColoring.n_nodi > 0)*/
    else
    {
        /*
         * If the graph has no nodes, every requested chunk
         * is already available in the users' local caches.
         */
        d_n_col = 0;

        cout << "No graph coloring is needed because all "
             << "requested chunks are locally cached."
             << endl;
    }


    // =====================================================
    // NEW TRANSMISSION METRICS
    // =====================================================

    /*
     * One graph color corresponds to one coded transmission.
     */
    unsigned int T_coded =
        static_cast<unsigned int>(d_n_col);

    /*
     * Convert to double before subtraction and division.
     * This prevents integer division and unsigned underflow.
     */
    double T_no_cache_double =
        static_cast<double>(T_no_cache);

    double T_uncoded_double =
        static_cast<double>(T_uncoded);

    double T_coded_double =
        static_cast<double>(T_coded);


    // =====================================================
    // 1. OVERALL GAIN
    //
    //                 T_no_cache - T_coded
    // Overall gain = ----------------------
    //                       T_no_cache
    // =====================================================

    bool overall_gain_defined = false;
    double overall_gain = 0.0;

    if (T_no_cache_double > 0.0)
    {
        overall_gain =
            (
                T_no_cache_double -
                T_coded_double
            )
            /
            T_no_cache_double;

        overall_gain_defined = true;
    }


    // =====================================================
    // 2. LOCAL CACHE DISTRIBUTION
    //
    //                            T_no_cache - T_uncoded
    // Local cache distribution = ------------------------
    //                                  T_uncoded
    // =====================================================

    bool local_cache_distribution_defined = false;
    double local_cache_distribution = 0.0;

    if (T_uncoded_double > 0.0)
    {
        local_cache_distribution =
            (
                T_no_cache_double -
                T_uncoded_double
            )
            /
            T_no_cache_double;

        local_cache_distribution_defined = true;
    }


    // =====================================================
    // 3. LOCAL CACHE CONTRIBUTION
    //
    //                          T_no_cache - T_uncoded
    // Local cache contribution = -------------------------
    //                            T_no_cache - T_coded
    // =====================================================

    double total_reduction =
        T_no_cache_double -
        T_coded_double;

    bool local_cache_contribution_defined = false;
    double local_cache_contribution = 0.0;

    if (fabs(total_reduction) > 1e-12)
    {
        local_cache_contribution =
            (
                T_no_cache_double -
                T_uncoded_double
            )
            /
            total_reduction;

        local_cache_contribution_defined = true;
    }
    // =====================================================
    // 4. MULTICAST GAIN
    //
    //                  T_uncoded - T_coded
    // Multicast gain = ---------------------
    //                       T_no_cache
    // =====================================================

    bool multicast_gain_defined = false;
    double multicast_gain = 0.0;

    if (T_no_cache_double > 0.0)
    {
        multicast_gain =
            (
                T_uncoded_double -
                T_coded_double
            )
            /
            T_no_cache_double;

        multicast_gain_defined = true;
    }


    // =====================================================
    // 5. MULTICAST CONTRIBUTION
    //
    //                            Multicast gain
    // Multicast contribution = ----------------
    //                             Overall gain
    // =====================================================

    bool multicast_contribution_defined = false;
    double multicast_contribution = 0.0;

    if (
        multicast_gain_defined &&
        overall_gain_defined &&
        fabs(overall_gain) > 1e-12
    )
    {
        multicast_contribution =
            multicast_gain /
            overall_gain;

        multicast_contribution_defined = true;
    }


    
    // =====================================================
    // PRINT THE NEW METRICS
    // =====================================================

    cout << fixed << setprecision(6);

    cout << endl;
    cout << "==========================================" << endl;
    cout << "Transmission Values" << endl;
    cout << "==========================================" << endl;

    cout << "T_no_cache = "
         << T_no_cache
         << endl;

    cout << "T_uncoded = "
         << T_uncoded
         << endl;

    cout << "T_coded = "
         << T_coded
         << endl;

    cout << "Graph nodes = "
         << d_outputForColoring.n_nodi
         << endl;

    cout << endl;
    cout << "==========================================" << endl;
    cout << "New Gain Metrics" << endl;
    cout << "==========================================" << endl;

    if (overall_gain_defined)
    {
        cout << "Overall Gain = "
             << overall_gain
             << endl;

        cout << "Overall Gain Percentage = "
             << overall_gain * 100.0
             << "%"
             << endl;
    }
    else
    {
        cout << "Overall Gain = N/A because T_no_cache = 0."
             << endl;
    }

    if (local_cache_distribution_defined)
    {
        cout << "Local Cache Distribution = "
             << local_cache_distribution
             << endl;

        cout << "Local Cache Distribution Percentage = "
             << local_cache_distribution * 100.0
             << "%"
             << endl;
    }
    else
    {
        cout << "Local Cache Distribution = N/A because "
             << "T_uncoded = 0."
             << endl;

        cout << "All requested chunks are available in "
             << "the local caches."
             << endl;
    }

    if (local_cache_contribution_defined)
    {
        cout << "Local Cache Contribution = "
             << local_cache_contribution
             << endl;

        cout << "Local Cache Contribution Percentage = "
             << local_cache_contribution * 100.0
             << "%"
             << endl;
    }
    else
    {
        cout << "Local Cache Contribution = N/A because "
             << "T_no_cache - T_coded = 0."
             << endl;
    }

    cout << "==========================================" << endl;

    if (multicast_gain_defined)
    {
        cout << "Multicast Gain = "
             << multicast_gain
             << endl;

        cout << "Multicast Gain Percentage = "
             << multicast_gain * 100.0
             << "%"
             << endl;
    }
    else
    {
        cout << "Multicast Gain = N/A because T_no_cache = 0."
             << endl;
    }

    if (multicast_contribution_defined)
    {
        cout << "Multicast Contribution = "
             << multicast_contribution
             << endl;

        cout << "Multicast Contribution Percentage = "
             << multicast_contribution * 100.0
             << "%"
             << endl;
    }
    else
    {
        cout << "Multicast Contribution = N/A because "
             << "Overall Gain = 0."
             << endl;
    }


    // =====================================================
    // SIMPLE OUTPUT LINES FOR runAll / grep
    // =====================================================

    cout << "Nodes: "
         << d_outputForColoring.n_nodi
         << endl;

    cout << "Colors: "
         << T_coded
         << endl;

    cout << "TNoCache: "
         << T_no_cache
         << endl;

    cout << "TUncoded: "
         << T_uncoded
         << endl;

    cout << "TCoded: "
         << T_coded
         << endl;

    if (overall_gain_defined)
    {
        cout << "OverallGain: "
             << overall_gain
             << endl;
    }
    else
    {
        cout << "OverallGain: NA"
             << endl;
    }

    if (local_cache_distribution_defined)
    {
        cout << "LocalCacheDistribution: "
             << local_cache_distribution
             << endl;
    }
    else
    {
        cout << "LocalCacheDistribution: NA"
             << endl;
    }

    if (local_cache_contribution_defined)
    {
        cout << "LocalCacheContribution: "
             << local_cache_contribution
             << endl;
    }
    else
    {
        cout << "LocalCacheContribution: NA"
             << endl;
    }

    if (multicast_gain_defined)
    {
        cout << "MulticastGain: "
             << multicast_gain
             << endl;
    }
    else
    {
        cout << "MulticastGain: NA"
             << endl;
    }

    if (multicast_contribution_defined)
    {
        cout << "MulticastContribution: "
             << multicast_contribution
             << endl;
    }
    else
    {
        cout << "MulticastContribution: NA"
             << endl;
    }
 //remove
    //open file
    //ofstream transmit_time, enc_time;
    //transmit_time.open("/home/Batoul/Desktop/testTxRx-BER_new_sec_v2/Results/TransmitTime/TransmitTime_" + to_string(nb_users) + ".txt",ios::app);
    //enc_time.open("/home/Batoul/Desktop/testTxRx-BER_new_sec_v2/Results/EncryptionTime/EncTime_" + to_string(nb_users) + ".txt",ios::app);


    /*
     * ORIGINAL TRANSMISSION / ENCRYPTION SECTION
     *
     * This complete section from the old code is intentionally
     * preserved below. It remains disabled, as it was in the
     * version provided by the user.
     *
     * It is written using line comments to avoid nested C-style
     * block comments, which are not valid in C++.
     */

    // if (d_n_col > 0)
    // {
    //     cout << endl << "Coding data process" << endl << "-------------" << endl << endl;
    //     d_coded_data = codingData(d_coloring, d_n_col, d_data, d_outputForColoring, &d_header_data);

    //     cout << " Data : " << endl;
    //     tempVec = conVecCharToByte(d_coded_data[0]);
    //     printHex(tempVec);
    //     cout << endl;

    //     //Pack the data and attach the header to be ready for transmission


    //     //transmission without encryption
    //     //d_transmission.resize(d_n_col);
    //     //auto startTx = chrono::high_resolution_clock::now();

    //     //Transmit(nb_users, d_header_data, d_coded_data, id_demand, d_n_col, packet_len, d_transmission, d_small_packet_size);

    //     //auto endTx = chrono::high_resolution_clock::now();

    //     //chrono::duration<double, std::milli> durationTx = endTx - startTx;
    //     //transmit_time << durationTx.count() << endl;

    //     //Reception without encryption
    //     //vector<vector<char>> received_data(d_n_col);
    //     //vector<header_transmission> header_data(d_n_col);

    //     // cout << "start receive " << endl;


    //     // for(int i=0; i<1 ; i++)
    //     // {

    //     //     Receive(nb_users, header_data.at(i), received_data.at(i), packet_len, d_transmission.at(i));

    //     // }


    //     // cout << "end receive" << endl;


    //     //cout << "Encrypt coded data" << endl;


    //     // generation of key for each user
    //     //vector <SecByteBlock> usersKeySet = keySetGeneration(nb_users);


    //     //generation of aes keys
    //     //vector<SecByteBlock> groupKeySet = keySetGeneration(d_n_col);


    //     //auto startEnc = chrono::high_resolution_clock::now();



    //     //encrypt coded data
    //     // for(int i=0; i<d_n_col; i++)
    //     // {
    //     //     tempVec = conVecCharToByte(d_coded_data[i]);
    //     //     tempKey = secByteBlockToByteVec(groupKeySet[i]);
    //     //     packet_enc = streamCipherXOR(tempVec, tempKey);
    //     //     d_coded_data_enc.push_back(conVecByteToChar(packet_enc));
    //     // }

    //     //auto endEnc = chrono::high_resolution_clock::now();

    //     //chrono::duration<double, std::milli> durationEnc = endEnc - startEnc;
    //     //enc_time << durationEnc.count() << endl;

    //     // cout << " Encrypted data :" << endl;
    //     // for(int i=0; d_coded_data_enc[0].size(); i++)
    //     // {
    //     //     cout << d_coded_data_enc[0][i];
    //     // }
    //     // cout << endl;


    //     //transmission with encryption
    //     //d_transmission_Enc.resize(d_n_col);
    //     //TransmitEnc(nb_users, d_header_data, d_coded_data_enc, id_demand, d_n_col, packet_len, d_transmission_Enc, d_small_packet_size_Enc, groupKeySet, usersKeySet);


    //     //Reception with encryption
    //     //vector<vector<char>> received_data_dec(d_n_col);
    //     //vector<header_transmission> header_data_dec(d_n_col);


    //     //cout << "start ReceiveDec " << endl;


    //     //for(int i=0; i<1 ; i++)
    //     //{
    //     //    ReceiveDec(nb_users, header_data_dec.at(i), received_data_dec.at(i), packet_len, 
    //     //        d_transmission_Enc.at(i),aesIVset[i], aesKeySet[i], usersIVset, usersKeySet);  
    //     //}


    //     //cout << "end ReceiveDec fct" << endl;



    // } // end if (d_n_col > 0) 

    //transmit_time.close();

    //enc_time.close();

    if (d_coloring != NULL)
    {
        delete [] d_coloring;
        d_coloring = NULL;
    }


    return 0;
}