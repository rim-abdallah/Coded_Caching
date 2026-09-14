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

#include "aes.h"

#include <complex.h>

typedef std::complex<float> gr_complex;
#undef byte  // Avoid ambiguity with std::byte

namespace caching{

void Transmit(int nb_users, header_transmission* header_data, vector< vector<char> > coded_data, unsigned int id_demand, int n_col, unsigned int small_packet_len, vector<vector<char>> &transmission, vector<unsigned int> &small_pack_size);

void TransmitEnc(int nb_users, header_transmission* header_data, vector< vector<char> > coded_data, unsigned int id_demand, int n_col, unsigned int small_packet_len, vector<vector<char>> &transmission, vector<unsigned int> &small_pack_size, vector<SecByteBlock> groupKeySet, vector<SecByteBlock> usersKeySet);

//void Receive(int nb_users, header_transmission &header, vector<char> &coded_data, unsigned int small_packet_len, vector<char> transmission);
   
//void ReceiveDec(int nb_users, header_transmission &header, vector<char> &coded_data, unsigned int small_packet_len, vector<char> transmission, SecByteBlock aesIV, SecByteBlock aesKey, vector<SecByteBlock> usersIVset, vector<SecByteBlock> usersKeySet);
         

void write_byte(vector<char> trasmissione);

}//end namespace caching

#endif // TxRx_H_INCLUDED
