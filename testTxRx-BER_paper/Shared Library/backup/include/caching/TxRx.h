#ifndef TxRx_H_INCLUDED
#define TxRx_H_INCLUDED

#include <iostream>
#include <stdio.h>
#include <string.h>
#include <string>
#include <vector>
#include <math.h>
#include <fstream>
#include <algorithm>

#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

#include <vector>	

#include "utilityForTesting.h"
#include "DataDefinition.h"
#include "Conversions.h"
#include "FuncsFromMain.h"
#include "CodingDecodingData.h"
#include "PC.h"

#include <complex.h>

typedef std::complex<float> gr_complex;

namespace caching{

void TX_PC_Pack(vector<header_polar> &, vector< vector<char> > , unsigned int , vector<vector<char> > &, int, vector<int> &);

void TX_simul(vector<header_polar> &, vector< vector<char> > , vector<vector<char> > &, vector<int> &);

void RX_simul(vector<gr_complex> , unsigned int &, vector<int> , int , PC , PC ,int , int , int , int , double , bool ,int , vector<int> , double &);

int decode_header(vector<gr_complex> , header_polar &);

void Process_Data(vector<gr_complex> , int , unsigned int &, int , int , bool , int , double , PC, PC, header_polar,int, int, vector<int> , double &);

vector<vector<gr_complex> > BitsToQPSKSymb(vector<vector<int> >);

void decodeDataStrong(int N,double d_SNR, gr_complex* coded_symb,string ,PC ,PC, bool ,unsigned int & ,header_polar , int, int , vector<int> , double &);

vector<char> cache_weak(header_polar , int , int , int , string);

void toHeaderWeak(header_polar , header_transmission &header);

void Transmit(header_transmission* header_data, vector< vector<char> > , unsigned int , int , unsigned int , vector<char> &trasmissione, vector<unsigned int> &small_pack_size);

void Receive(header_transmission &header, vector<char> &coded_data, unsigned int small_packet_len, FILE *RX_file);

void write_byte(vector<char> trasmissione);

}//end namespace caching

#endif // TxRx_H_INCLUDED
