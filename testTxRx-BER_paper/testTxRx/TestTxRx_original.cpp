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
#include <iomanip>
#include <math.h>
#include <cmath>
#include <algorithm>
#include <random>

using namespace caching;
using namespace std;


int main()
{
    data_matrix d_data;
    cf_data d_outputForColoring;
    bool d_GRASP=true;
    int d_n_col=0;
    int *d_coloring;

    header_transmission *d_header_data;
    header_transmission *d_hdr_sdata;

    vector< vector<char> > d_coded_data;
    vector< vector<char> > d_strg_data;
    vector< vector<char> > d_PC_data;
    vector<header_polar> d_hX; //this is the main header to be added directly to the transmitted packets
    int N=2048;
    
    //--------------------  Simulation parameters ------------
    int m_files = 20;
    int b_chunks = 200;
    int id_demand = 0;
    int nb_strg = 1;
    int user_id = 4;  
    std::vector<int> coderate = {1, 2, 2, 3, 4};  
    int coderate_arr[5];
    for (int i = 0; i < coderate.size(); ++i)
        coderate_arr[i] = coderate[i];
    int Gaussian = 0; // 1 means gaussian channel, 0 means Rayleigh channel
    float snrmin = -1, snrmax = 6;
    int scheme = 4;//schema = 1-no-caching, 2-coded caching, 3-generalized coded caching, 4-caching-aided polarcoding
    //---------------------------------------------------------

    cout << endl << "Data generation process" << endl << "-------------" << endl << endl;

    d_data = generateData(m_files, b_chunks, id_demand);
    int nb_users = d_data.n_utenti;

    cout << endl << "Conflict-Graph generator process" << endl << "-------------" << endl << endl;
    d_outputForColoring = conflictGraphGenerator(d_data, coderate);
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
        cout << "The expected gain is: " << (100*(d_outputForColoring.n_nodi-d_n_col)/d_outputForColoring.n_nodi) << "%" << endl;

    }/*end if (d_outputForColoring.n_nodi > 0)*/

    if (d_n_col > 0)
    {
        cout << endl << "Coding data process" << endl << "-------------" << endl << endl;
        //Coding data to be transmitted for weak users - xor coding
        /*if(scheme == 1 || scheme == 2)
            d_coded_data = codingVarCodeRate_Ref(d_coloring, d_n_col, d_data, d_outputForColoring, &d_header_data,coderate);
        else if(scheme == 3 || scheme == 4)
            d_coded_data = codingVarCodeRate(d_coloring, d_n_col, d_data, d_outputForColoring, &d_header_data,coderate);
        */
        d_coded_data = codingData(d_coloring, d_n_col, d_data, d_outputForColoring, &d_header_data);

        //Build the schema (graph) packets for strong and weak users
        /*vector<vector<bool> > G_edges;
        //bool **G_edges;
        d_strg_data = MaxBipartiteGraph(d_coloring, d_n_col, d_outputForColoring.nodes, 
            d_outputForColoring.n_nodi, nb_strg, d_data, &d_hdr_sdata, G_edges);
        
        cout << scheme << endl;
        if(scheme <= 3){
            G_edges = vector<vector<bool> > ();

            for(unsigned int i=0; i<d_coded_data.size(); i++){
                vector<bool> temp(d_strg_data.size(), false);
                G_edges.push_back(temp);
            }
        }
        //Coding polarly the weak and strong packets
        vector<vector<int> >  bits_coded;
        d_PC_data = codingDataPolar(d_coded_data, d_strg_data, bits_coded, G_edges, d_header_data, 
            d_hdr_sdata, d_hX, N, coderate_arr, snrmin,snrmax,user_id,Gaussian);
        cout << "The total number of transmitted packet is: " << d_PC_data.size() << endl;

        */

    }/* end if (d_n_col > 0) */
    
    delete [] d_coloring;
        
    return 0;

}
