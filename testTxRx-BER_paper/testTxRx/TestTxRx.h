#ifndef TESTTXRX_H_INCLUDED
#define TESTTXRX_H_INCLUDED

#include "DataDefinition.h"
#include "utilityForTesting.h"
#include "gr_complex.h"

using namespace std;
namespace caching{

      void gen_attenu(Complex* , double pow_del[],int ,int ,int ,int ,int );
      void print( const char * , vector<gr_complex> , int  );
      void FFT ( vector<gr_complex> & , vector<gr_complex> & , int &);
      void iFFT( vector<gr_complex> & , vector<gr_complex> & , int );
}//end caching namespace
#endif
