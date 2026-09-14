#ifndef CONVERSIONS_H_INCLUDED
#define CONVERSIONS_H_INCLUDED

#include <iostream>
#include <stdio.h>
#include <string.h>
#include <string>
#include <vector>
#include <math.h>
#include <fstream>
#include <algorithm>
#include "gr_complex.h"

#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

#include <vector>	

#include "utilityForTesting.h"
#include "DataDefinition.h"


namespace caching{

void conv_8BPSKsymb_to_int(gr_complex *buff_bpsk, unsigned int &x);

void conv_8BPSKsymb_to_char(gr_complex *buff_bpsk, char &x);

void conv_16BPSKsymb_to_int(gr_complex *buff_bpsk, unsigned int &x);

void conv_4QPSKsymb_to_char(gr_complex *buff_qpsk, char &x);

void conv_4QPSKsymb_to_int(gr_complex *buff_qpsk, unsigned int &x);

void conv_8QPSKsymb_to_int(gr_complex *buff_qpsk, unsigned int &x);

std::vector<unsigned int> conv_char_to_bitsInt(unsigned char a);

void conv_int_to_char(unsigned int n, char* bytes);

void conv_char_to_int(char* bytes, unsigned int &x);

void conv_short_int_to_char(unsigned short int n, char* bytes);

void conv_char_to_short_int(char* bytes, unsigned short int &x);

std::vector<unsigned int> conv_stream_char_to_bits(const char *in, int packetSize);

void conv_int_to_byte(unsigned int integer, char &b);

void conv_byte_to_int(char b, unsigned int &integer);

}//end namespace caching

#endif // CONVERSIONS_H_INCLUDED
