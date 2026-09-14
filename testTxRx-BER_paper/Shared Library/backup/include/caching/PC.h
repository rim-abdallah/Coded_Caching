/*
 * PC.h
 *
 *  Created on: 6 févr. 2018
 *  Modified on: 23 jul. 2018
 *      Author: KAMEL
 */

#ifndef PC_H_
#define PC_H_

#include <complex.h>

typedef std::complex<float> gr_complex;
typedef std::complex<double> gr_complexd;

using namespace std;

class PC {

public:

	// Setters
	void setPC(int N, int K, float SNRdb);  // set initial parameters
	void setGenMatrix(int ** gen);			// set generator matrix
	void setArrangedBits(int * bits);		// set bits order
	void setRn(int * vec);					// set permutation vector

	// Initialize PC
	void initPC(int N, int K, float SNRdb);

	// Getters
	void getParam(int &N, int &K, float &SNRdb);
	void getGenMatrix(int ** gen);
	void getArrangedBits(int * bits);

	// Print
	void printParam();
	void printGenMatrix();
	void printArrangedBits();
	void printDecodingVariables();
	void printAll();
	void printMatrixInt(int ** matrix, int dim);
	void printVectorInt(int * vector, int dim);
	void printMatrixDouble(double ** matrix, int dim);
	void printVectorDouble(double * vector, int dim);
	void printVectorFloat(float * vector, int dim);
	void printDecodingMatrixInt(int ** matrix);
	void printDecodingMatrixDouble(double ** matrix);


	// Tools
	void sortBits(double * Z);
	void sort(int * x, int N);
	double diffLog( double a, double b);
	void kroneckerProduct(int N, int ** G);
	int sign(double x);

	// Generation
	void constructGenMatrix();
	void arrangeBits(); // generate arranged bits without Rn
	void constructPC(int N, int K, float SNRdb);

	// encoder + channel + SC decoder
	void encode(int * sentInfo, int * sentFrozen, int * sentMessage, int * sentCodeword, int * sentSymbol);
	void noise(int * sentSymbol, float * receivedSymbol, double variance);
	void noise_c(gr_complex * sentSymbol, gr_complex * receivedSymbol, double variance);
	void noise_gc(gr_complex * sentSymbol, gr_complex * receivedSymbol, float sqrtVariance, int len);
	void noise_hgc(gr_complex * sentSymbol, gr_complex * receivedSymbol, gr_complex * hp,float sqrtVariance, int len);
	void computeLLR(double * llr, float * receivedSymbol, float variance); // BPSK modulation where 0 -> 1 and 1 -> -1
	void computeLLR_qpsk(double * llr, gr_complex * receivedSymbol, float variance); // QPSK modulation where -1-i -> 0, -1+i -> 1, 1-i -> 2, 1+i -> 3
	void SC(int * recMessage, int * recCodeword, double * llr, int * sentFrozen);
	void recursiveSC(int m, int N, int p, int * sentMessage);

	// Constructor - Destructor
	PC();
	virtual ~PC();

	int * arragedBits;		// arranged from best to worst channel // size = length
	int ** genMatrix;		// Generator matrix / includes permutation
	int * Rn;				// PC bits permutation vector

private:
	// PC initial parameters
	int length,dimension,loglength;
	float designSNRdb; 		// SNR used to design PC = for initial Z value
	//float energyPerSymb;  // BPSK symbol energy

	// PC generation
	int * infoBits;    		// size = dimension
	int * frozenBits;		// size = length - dimension
	bool * frozenBitsPos;	// True if bit is frozen // size = length

	// Variables needed for decoding
	int * sentFrozenArranged;	// size = length
								// used to arrange frozen bits to avoid frozen bits search while decoding
	int * recMessageArranged;
	double ** llr;
	int ** pos;
	int ** msg;
	int ** cw;
};


#endif /* PC_H_ */
