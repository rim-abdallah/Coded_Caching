#ifndef CODINGDECODINGDATA_H_INCLUDED
#define CODINGDECODINGDATA_H_INCLUDED

#include <iostream>
#include <string>
#include <vector>
#include <math.h>
#include <fstream>
#include <algorithm>
#include <random>
#include "utilityForTesting.h"
#include "DataDefinition.h"
#include "Conversions.h"
#include "FuncsFromMain.h"
#include "PC.h"
#include "EnvironmentSetup.h"
#include "TxRx.h"

namespace caching{

using namespace std;

const float pi = 3.1415926;

bool bpm(vector<vector<bool> > bpGraph, int u, bool seen[], int matchR[]);

vector<vector<bool> > maxBPM(vector<vector<bool> > bpGraph);

void gen_rand_request_zipf(int , double , vector<int> & );

vector< vector<char> > MaxBipartiteGraph(int *d_coloring, int d_n_col, nodo *nodes, 
                int n_nodi, int nb_strg, data_matrix d_data, header_transmission **d_hdr_sdata, vector<vector<bool> >& );

vector<vector<char> > codingDataPolar(vector<vector<char> > weak_data, vector<vector<char> > strg_data, vector<vector<int> > &,
    vector<vector<bool> > G_edges, header_transmission *hdr_weak, header_transmission *hdr_strg, vector<header_polar> &, const int, int[] , float, float, int, int);

PC initialize_PC(int ,int);

bool myfunction (int i,int j);

vector< vector<char> > codingVarCodeRate(int *coloring, int , data_matrix , cf_data , header_transmission **header_data, vector<int>);
vector< vector<char> > codingVarCodeRate_Ref(int *coloring, int n_, data_matrix , cf_data ,header_transmission **header_data, vector<int>);

void decodingData(header_transmission , vector<char> &coded_data, unsigned int , unsigned int , unsigned int , unsigned int , unsigned int , int, unsigned int *n_package_remains);

void print( const char * prompt, gr_complex A[], int log2N );
void FFT ( gr_complex f[]     , gr_complex ftilde[], int log2N );
void iFFT( gr_complex ftilde[], gr_complex f[]     , int log2N );
gr_complex** generate_ch_param(double pow_del[], int OFDM_Symb, int Nc,int Ls,int fd,int T,int L);
void dyn_chnl(gr_complex out_ch[], gr_complex in_ch[], gr_complex h[], int Nc, int L, int t[]);
gr_complex divide_complex(gr_complex s1, gr_complex s2);

}//end namespace caching

#endif // CODINGDECODINGDATA_H_INCLUDED
