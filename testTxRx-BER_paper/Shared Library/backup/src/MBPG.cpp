/*
 * PC.cpp
 *
 *  Created on: 6 févr. 2018
 *      Author: KAMEL
 */

#include <iostream>
#include <string>
#include <fstream>
#include <ctime>
#include <cstdlib>
#include <cmath>
#include <random>

#include "PC.h"

using namespace std;



////////////////////////////////////////////////////////////////////////////
////////////////////////	 Set functions 			////////////////////////
////////////////////////////////////////////////////////////////////////////

void PC::setPC(int N, int K, float SNRdb)
{
	length = N;
	dimension = K;
	designSNRdb = SNRdb;
	loglength = log2(N);

}/* setPC */

void PC::setGenMatrix(int ** G)
{
	for (int i=0; i<length; i++)
		for (int j=0; j<length; j++)
			genMatrix[i][j] = G[i][j];

}/* setGenMatrix */

void PC::setRn(int * vec)
{
	for (int i=0; i<length; i++)
			Rn[i] = vec[i];

}/* setGenMatrix */

void PC::setArrangedBits(int * bits)
{
	for (int i=0 ; i<dimension ; i++)
	{
		infoBits[i] = bits[i];
		arragedBits[i] = bits[i];
	}
	for (int i=dimension ; i<length ; i++)
	{
		frozenBits[i-dimension] = bits[i];
		arragedBits[i] = bits[i];
		frozenBitsPos[arragedBits[i]] = true;
	}


}/* setArrangedBits */



////////////////////////////////////////////////////////////////////////////
////////////////////////	 Initialize 			////////////////////////
////////////////////////////////////////////////////////////////////////////

void PC::initPC(int N, int K, float SNRdb)
{
	int i,j;

	// Initialize PC initial parameters
	setPC(N, K, SNRdb);

	// Initialize PC generation
	arragedBits = new int [length]; // arranged from best to worst channel
	for ( i = 0 ; i < length ; i++ )
		arragedBits[i] = i;

	Rn = new int[length];
	for ( i = 0 ; i < length ; i++ )
		Rn[i] = 0;

	infoBits = new int [dimension];
	for ( i = 0 ; i < dimension ; i++ )
		infoBits[i] = 0;

	frozenBits = new int [length-dimension];
	for ( i = 0 ; i < length-dimension ; i++ )
		frozenBits[i] = 0;

	frozenBitsPos = new bool[length]; // true if bit is frozen
	for ( i = 0 ; i < length ; i++ )
			frozenBitsPos[i] = false;

	genMatrix = new int * [length];
	for ( i = 0 ; i < length ; i++ )
	{
		genMatrix[i] = new int[length] ;
		for ( j = 0 ; j < length ; j++ )
			genMatrix[i][j] = 0;
	}

	// Initialize variables needed for decoding
	llr = new double * [loglength];
	pos = new int * [loglength];
	msg = new int * [loglength];
	cw = new int * [loglength];
	for ( i = 0 ; i < loglength ; i++ )
	{
		j = pow(2,i+1);
		llr[i] = new double[j] ;
		pos[i] = new int [j];
		msg[i] = new int [j];
		cw[i] = new int [j];
	}

	for ( i = 0 ; i < loglength ; i++ )
	{
		for ( j = 0 ; j < pow(2,i+1) ; j++ )
		{
			llr[i][j] = 0;
			pos[i][j] = 0;
			msg[i][j] = 0;
			cw[i][j] = 0;
		}
	}
	sentFrozenArranged = new int [length];
	for ( i = 0 ; i < length ; i++ )
		sentFrozenArranged[i] = 0;

}/* initPC */


////////////////////////////////////////////////////////////////////////////
////////////////////////	 Get functions			////////////////////////
////////////////////////////////////////////////////////////////////////////

void PC::getParam(int &N, int &K, float &SNRdb)
{
	N = length;
	K = dimension;
	SNRdb = designSNRdb;

}/* getParam */

void PC::getGenMatrix(int ** G)
{
	for (int i=0; i<length; i++)
		for (int j=0; j<length; j++)
			G[i][j] = genMatrix[i][j];

}/* getGenMatrix */

void PC::getArrangedBits(int * bits)
{
	for (int i=0; i<length; i++)
		bits[i] = arragedBits[i];

}/* getArrangedBits */



////////////////////////////////////////////////////////////////////////////
////////////////////////	 Print functions		////////////////////////
////////////////////////////////////////////////////////////////////////////


void PC::printParam()
{
	cout << endl;
	cout << " Length of the code = " << length << endl;
	cout << " Dimension of the code = " << dimension << endl;
	cout << " Design SNR (db) = " << designSNRdb << endl;
	cout << endl;

}/* printParam */

void PC::printGenMatrix()
{
	cout << " Generator matrix is:" << endl;
	for ( int i = 0 ; i < length ; i++ )
	{
		for ( int j = 0 ; j < length ; j++ )
			cout << "  " << genMatrix[i][j];
		cout << endl;
	}
	cout << endl;

}/* printGenMatrix */

void PC::printArrangedBits()
{
	int i;
	cout << " Arranged bits pos = " ;
	for (i=0 ; i<length ; i++)
		cout << " " << arragedBits[i] ;
	cout << endl;

	cout << " Info bits pos = " ;
	for (i=0 ; i<dimension ; i++)
		cout << " " << infoBits[i] ;
	cout << endl;

	cout << " Frozen bits pos = " ;
	for (i=0 ; i<(length-dimension) ; i++)
	{
		cout << " " << frozenBits[i] ;
	}
	cout << endl;

	cout << " Frozen bits or info bits (bool) = " ;
	for (i=0 ; i<length ; i++)
		cout << " "  << frozenBitsPos[i];
	cout << endl; cout << endl;

}/* printArrangedBits */

void PC::printDecodingVariables()
{

	cout << " llr = " << endl ;
	for (int i = 0 ; i < loglength ; i++ )
	{
		for (int j = 0 ; j < pow(2,i+1) ; j++ )
			cout << " " << llr[i][j];
		cout << endl;
	}
	cout << endl;

	cout << " pos = " << endl  ;
	for (int i = 0 ; i < loglength ; i++ )
	{
		for (int j = 0 ; j < pow(2,i+1) ; j++ )
			cout << " " << pos[i][j];
		cout << endl;
	}
	cout << endl;

	cout << " msg = " << endl  ;
	for (int i = 0 ; i < loglength ; i++ )
	{
		for (int j = 0 ; j < pow(2,i+1) ; j++ )
			cout << " " << msg[i][j];
		cout << endl;
	}
	cout << endl;

	cout << " cw = " << endl  ;
	for (int i = 0 ; i < loglength ; i++ )
	{
		for (int j = 0 ; j < pow(2,i+1) ; j++ )
			cout << " " << cw[i][j];
		cout << endl;
	}
	cout << endl;

}/* printDecodingVariables */

void PC::printAll()
{
	int i,j;
	printParam();
	printGenMatrix();
	printArrangedBits();

	cout << " Permutation vector Rn = " ;
	for (i=0 ; i<length ; i++)
		cout << " " << Rn[i] ;
	cout << endl << endl;

	cout << " llr = " << endl ;
	for ( i = 0 ; i < loglength ; i++ )
	{
		for ( j = 0 ; j < pow(2,i+1) ; j++ )
			cout << " " << llr[i][j];
		cout << endl;
	}
	cout << endl;

	cout << " pos = " << endl  ;
	for ( i = 0 ; i < loglength ; i++ )
	{
		for ( j = 0 ; j < pow(2,i+1) ; j++ )
			cout << " " << pos[i][j];
		cout << endl;
	}
	cout << endl;

	cout << " msg = " << endl  ;
	for ( i = 0 ; i < loglength ; i++ )
	{
		for ( j = 0 ; j < pow(2,i+1) ; j++ )
			cout << " " << msg[i][j];
		cout << endl;
	}
	cout << endl;

	cout << " cw = " << endl  ;
	for ( i = 0 ; i < loglength ; i++ )
	{
		for ( j = 0 ; j < pow(2,i+1) ; j++ )
			cout << " " << cw[i][j];
		cout << endl;
	}
	cout << endl;

}/* printAll */

void PC::printMatrixInt(int ** matrix, int dim)
{
	for ( int i = 0 ; i < dim ; i++ )
	{
		for ( int j = 0 ; j < dim ; j++ )
			cout << "  " << matrix[i][j];
		cout << endl;
	}
	cout << endl;

}/* printMatrixInt */

void PC::printVectorInt(int * vector, int dim)
{
	for ( int i = 0 ; i < dim ; i++ )
		cout << "  " << vector[i];
	cout << endl;

}/* printVectorInt */

void PC::printMatrixDouble(double ** matrix, int dim)
{
	for ( int i = 0 ; i < dim ; i++ )
	{
		for ( int j = 0 ; j < dim ; j++ )
			cout << "  " << matrix[i][j];
		cout << endl;
	}
	cout << endl;

}/* printMatrixDouble */

void PC::printVectorDouble(double * vector, int dim)
{
	for ( int i = 0 ; i < dim ; i++ )
		cout << "  " << vector[i];
	cout << endl;

}/* printVectorDouble */

void PC::printVectorFloat(float * vector, int dim)
{
	for ( int i = 0 ; i < dim ; i++ )
		cout << "  " << vector[i];
	cout << endl;

}/* printVectorFloat */

void PC::printDecodingMatrixInt(int ** matrix)
{
	for ( int i = 0 ; i < loglength ; i++ )
	{
		for ( int j = 0 ; j < pow(2,i+1) ; j++ )
			cout << " " << matrix[i][j];
		cout << endl;
	}
	cout << endl;

}/* printDecodingMatrixInt */

void PC::printDecodingMatrixDouble(double ** matrix)
{
	for ( int i = 0 ; i < loglength ; i++ )
	{
		for ( int j = 0 ; j < pow(2,i+1) ; j++ )
			cout << " " << matrix[i][j];
		cout << endl;
	}
	cout << endl;

}/* printDecodingMatrixDouble */


////////////////////////////////////////////////////////////////////////////
////////////////////////		 Tools			////////////////////////////
////////////////////////////////////////////////////////////////////////////

double PC::diffLog( double x, double y)
{
	// x MUST be greater than y
	// used in constructPC or arrangeBits

	double z = x + log(1-exp(y-x));
	return z;

}/* diffLog */

void PC::kroneckerProduct(int N, int ** G)
{
	int i,j;
	for (i=0; i<N/2; i++)
	{
		for ( j=0; j<=i; j++ )
		{
			G[i+N/2][j] = G[i][j];
			G[i+N/2][j+N/2] = G[i][j];
		}
	}
}/* kroneckerProduct */

void PC::sortBits(double * Z)
{
	// Sort PC bits depending on their Bhattacharya parameter values

	int i,j,k;
	double min,max;
	int minPos,maxPos, tempVar;

	for (i=0; i<(length /2) ; i++)
	{
		j = length-1-i;
		min = Z[i];
		max = Z[j];
		minPos = i;
		maxPos = j;

		for (k=i ; k<=j ; k++)
		{
	    		if (Z[k] < min)
			{
				min = Z[k];
				minPos = k;
			}
			else if (Z[k] > max)
			{
				max = Z[k];
				maxPos = k;
			}
		}

		if ((minPos == j) && (maxPos == i))
		{
			Z[i] = min;
			Z[j] = max;

			tempVar = arragedBits[minPos];
			arragedBits[minPos] = arragedBits[i];
			arragedBits[i] = tempVar;

		}
		else if ((minPos == j) && (maxPos != i))
		{
			Z[minPos] = Z[i];
			Z[i] = min;

			tempVar = arragedBits[minPos];
			arragedBits[minPos] = arragedBits[i];
			arragedBits[i] = tempVar;

			Z[maxPos] = Z[j];
			Z[j] = max;

			tempVar = arragedBits[maxPos];
			arragedBits[maxPos] = arragedBits[j];
			arragedBits[j] = tempVar;

		}
		else if ((minPos != j) && (maxPos == i))
		{
			Z[maxPos] = Z[j];
			Z[j] = max;

			tempVar = arragedBits[maxPos];
			arragedBits[maxPos] = arragedBits[j];
			arragedBits[j] = tempVar;

			Z[minPos] = Z[i];
			Z[i] = min;

			tempVar = arragedBits[minPos];
			arragedBits[minPos] = arragedBits[i];
			arragedBits[i] = tempVar;

		}
		else
		{
			if (minPos != i)
			{
				Z[minPos] = Z[i];
				Z[i] = min;

				tempVar = arragedBits[minPos];
				arragedBits[minPos] = arragedBits[i];
				arragedBits[i] = tempVar;
			}
			if (maxPos != j)
			{
				Z[maxPos] = Z[j];
				Z[j] = max;

				tempVar = arragedBits[maxPos];
				arragedBits[maxPos] = arragedBits[j];
				arragedBits[j] = tempVar;
			}
		}

	}
}/* sortBits */

void PC::sort(int * x, int N)
{
	// Sort array values in increasing order

	int i,j,k;
	double min,max;
	int minPos,maxPos;


		for (i=0; i<(N /2) ; i++)
		{
			j = N-1-i;
			min = x[i];
			max = x[j];
			minPos = i;
			maxPos = j;

			for (k=i ; k<=j ; k++)
			{
				if (x[k] < min)
				{
					min = x[k];
					minPos = k;
				}
				else if (x[k] > max)
				{
					max = x[k];
					maxPos = k;
				}
			} /* end for k */

			if ((minPos == j) && (maxPos == i))
			{
				x[i] = min;
				x[j] = max;
			}
			else if ((minPos == j) && (maxPos != i))
			{
				x[minPos] = x[i];
				x[i] = min;
				x[maxPos] = x[j];
				x[j] = max;
			}
			else if ((minPos != j) && (maxPos == i))
			{
				x[maxPos] = x[j];
				x[j] = max;
				x[minPos] = x[i];
				x[i] = min;
			}
			else
			{
				if (minPos != i)
				{
					x[minPos] = x[i];
					x[i] = min;
				}
				if (maxPos != j)
				{
					x[maxPos] = x[j];
					x[j] = max;
				}
			}

		} /* end for i */

}

int PC::sign(double x)
{
	int s;
	if (x < 0)
		s = -1;
	else
		s = 1;
	return s;
}

////////////////////////////////////////////////////////////////////////////
////////////////////////	PC Construction			////////////////////////
////////////////////////////////////////////////////////////////////////////

void PC::arrangeBits()
{
	// Construction based on the Bhattacharyya parameter
	// Following implementation of Viterbo's student (Matlab)
	// I changed their bits ordering -> obtained bits here are NOT permuted

	double * Z = new double [length];
	int i,j,lev,B;
	double T;

	double designSNR = pow(10, (designSNRdb/10));
	Z[0] = - designSNR;

	//cout << "Z0 = " << Z[0] << endl;

	for (lev=0 ; lev<log2(length) ; lev++)
	{
		B = pow(2,lev);
		for (j=B-1 ; j>=0 ; j--)
		{
			T = Z[j];
			Z[2*j] = diffLog( log(2)+T , 2*T);
			Z[2*j+1] = 2*T;
		}
	}

	/*// Print
	cout << "Z = ";
	for (i=0; i<length; i++)
		cout << Z[i] << " " ;
	cout << endl;
	// end Print*/

	sortBits(Z);

	for (i=0 ; i<dimension ; i++)
		infoBits[i] = arragedBits[i];

	for (i=dimension ; i<length ; i++)
	{
		frozenBits[i-dimension] = arragedBits[i];
		frozenBitsPos[arragedBits[i]] = true;
	}

	//sort(infoBits,dimension);
	//sort(frozenBits,length-dimension);


/*
	// Print
	cout << "Z = " ;
	for (i=0 ; i<length ; i++)
		cout << Z[i] << " " ;
	cout << endl;
	// end Print
*/

}/* arrangeBits */

void PC::constructGenMatrix()
{
	// Generator matrix includes permutation

	int i,n,N;

	int ** matrix = new int * [length];
	for ( int i = 0 ; i < length ; i++ )
		matrix[i] = new int[length] ;

	for ( i = 0 ; i < length ; i++ )
		for ( int j = 0 ; j < length ; j++ )
			matrix[i][j] = 0;

	matrix[0][0] = 1;	matrix[0][1] = 0;
	matrix[1][0] = 1;	matrix[1][1] = 1;

	for (i=2; i<=log2(length); i++)
		kroneckerProduct(pow(2,i),matrix);

	// Permutation vector
	Rn[0] = 0; Rn[length/2] = 1;
	for (n=1; n<log2(length) ; n++)
	{
		N = pow(2,n);
		for (i=0; i<N ; i++)
			Rn[i*length/N + length/(N*2)] = Rn[i*length/N] + N;
	}

/*	// Print
	cout << "Rn = ";
	for (i=0; i<length ; i++)
	{
		cout << Rn[i] << " ";
	}
	cout << endl;	cout << endl;*/

	for (i=0; i<length; i++)
		for (int j=0; j<length; j++)
			genMatrix[Rn[i]][j] = matrix[i][j];

}/* constructGenMatrix */

void PC::constructPC(int N, int K, float SNRdb)
{
	initPC(N, K, designSNRdb);
	constructGenMatrix();
	arrangeBits();
}


////////////////////////////////////////////////////////////////////////////
/////////////	    Encoding + decoding functions 			////////////////
////////////////////////////////////////////////////////////////////////////

void PC::encode(int * sentInfo, int * sentFrozen, int * sentMessage, int * sentCodeword, int * sentSymbol)
{

	int i,j;
	for (i = 0; i<dimension; i++)
		sentMessage[infoBits[i]] = sentInfo[i];
	for (i = 0; i<(length-dimension); i++)
		sentMessage[frozenBits[i]] = sentFrozen[i];

	for (j=0; j<length; j++)
	{
		sentCodeword[j] = sentMessage[0]*genMatrix[0][j];
		for(i=1; i<length; i++)
			sentCodeword[j] = sentCodeword[j] + sentMessage[i]*genMatrix[i][j];
		sentCodeword[j] = sentCodeword[j]%2;

		if (sentCodeword[j] == 0)
			sentSymbol[j] = 1;
		else
			sentSymbol[j] = -1;
	}

}

void PC::noise(int * sentSymbol, float * receivedSymbol, float sqrtVariance)
{

	 //default_random_engine generator;
	// random_device rd;
	 default_random_engine generator( rand() );
	 normal_distribution<double> distribution(0,sqrtVariance);

	 for (int i=0; i<length; i++)
		 receivedSymbol[i] = sentSymbol[i] + distribution(generator);

}

void PC::computeLLR(double * llr, float * receivedSymbol, float variance)
{
	float p0,p1;
	for (int i=0; i<length; i++)
	{
		 p0 = exp( -pow((receivedSymbol[i]-1),2)/(2*variance) );
		 p1 = exp( -pow((receivedSymbol[i]+1),2)/(2*variance) );
		 llr[i]= log( p0/ p1);
	}
}


void PC::SC(int * recMessage, int * recCodeword, double * recLLR, int * sentFrozen)
{

	// Arrange Frozen bits to avoid searching for their position at each frozen bit decoding step
	for (int i=0; i<(length-dimension); i++)
		sentFrozenArranged[frozenBits[i]] = sentFrozen[i];

	// Based on my Matlab internship program u|u+v (but space optimized)


	// Divide LLRs of u+v|u of length N into LLRs of u and LLRs of v each of length N/2
	for (int i=0; i<length/2; i++)
	{
		pos[loglength-1][i] = Rn[2*i];
		pos[loglength-1][length/2+i] = Rn[2*i+1];
	}

	// Decoding of u
	for (int i=0; i<length/2; i++)
		llr[loglength-1][i] =  2 * atanh ( tanh(recLLR[2*i]/2) * tanh(recLLR[2*i+1]/2));
		//llr_u[i] = sign(llr[2*i])*sign(llr[2*i+1])*min(llr[2*i],llr[2*i+1]);

	recursiveSC(loglength-2,length/2,0,sentFrozenArranged);

	for (int i=0; i<length/2; i++)
	{
		recMessage[i] = msg[loglength-2][i];
		recCodeword[2*i] = cw[loglength-2][i];
	}

	// Decoding of v
	for (int i=0; i<length/2; i++)
		llr[loglength-1][length/2+i] = pow(-1,recCodeword[2*i])*recLLR[2*i] + recLLR[2*i+1];

	recursiveSC(loglength-2,length/2,length/2,sentFrozenArranged);


	// Reconstruction of u+v|v
	for (int i=0; i<length/2; i++)
	{
		recMessage[length/2+i] = msg[loglength-2][i];
		recCodeword[2*i] = (recCodeword[2*i]+cw[loglength-2][i])%2;
		recCodeword[2*i+1] = cw[loglength-2][i];
	}
}/* SC */


void PC::recursiveSC(int m, int N, int p, int * sentFrozenArranged)
{
	if (N > 2)
	{
		// Divide LLRs of u+v|u of length N into LLRs of u and LLRs of v each of length N/2
		for (int i=0; i<N/2; i++)
		{
			pos[m][i] = pos[m+1][p+2*i];
			pos[m][N/2+i] = pos[m+1][p+2*i+1];
		}

		// Decoding of u
		for (int i=0; i<N/2; i++)
			llr[m][i] =  2 * atanh ( tanh(llr[m+1][p+2*i]/2) * tanh(llr[m+1][p+2*i+1]/2));
			//llr_u[i] = sign(llr[2*i])*sign(llr[2*i+1])*min(llr[2*i],llr[2*i+1]);

		recursiveSC(m-1, N/2,0,sentFrozenArranged);

		for (int i=0; i<N/2; i++)
		{
			msg[m][i] = msg[m-1][i];
			cw[m][2*i] = cw[m-1][i];
		}

		// Decoding of v
		for (int i=0; i<N/2; i++)
			llr[m][N/2+i] = pow(-1,cw[m][2*i])*llr[m+1][p+2*i] + llr[m+1][p+2*i+1];

		recursiveSC(m-1, N/2,N/2,sentFrozenArranged);

		for (int i=0; i<N/2; i++)
		{
			msg[m][N/2+i] = msg[m-1][i];
			cw[m][2*i] = (cw[m][2*i]+cw[m-1][i])%2;
			cw[m][2*i+1] = cw[m-1][i];
		}
	}
	else
	{
		pos[0][0] = pos[1][p];
		pos[0][1] = pos[1][p+1];


		if (!frozenBitsPos[pos[0][0]])
		{
			llr[0][0] = 2 * atanh ( tanh(llr[1][p+0]/2) * tanh(llr[1][p+1]/2));
					//sign(llr[0])*sign(llr[1])*min(llr[0],llr[1]);
			if ( llr[0][0] >= 0)
				msg[0][0] = 0;
			else
				msg[0][0] = 1;
		}
		else
		{
			msg[0][0] = sentFrozenArranged[pos[0][0]];
			/*for (int i=0; i<length-dimension; i++)
			{
				if (frozenBits[i]==pos[0][0])
				{
					msg[0][0] = sentFrozen[i];
					break;
				}
			}*/
		}

		if (!frozenBitsPos[pos[0][1]])
		{
			llr[0][1]= pow(-1,msg[0][0])*llr[1][p+0] + llr[1][p+1];
			if ( llr[0][1] >= 0)
				msg[0][1] = 0;
			else
				msg[0][1] = 1;
		}
		else
		{
			msg[0][1] = sentFrozenArranged[pos[0][1]];
			/*for (int i=0; i<length-dimension; i++)
			{
				if (frozenBits[i]==pos[0][1])
				{
					msg[0][1] = sentFrozen[i];
					break;
				}
			}*/
		}

		cw[0][0] = (msg[0][0]+msg[0][1])%2;
		cw[0][1] = msg[0][1];

	}

}/* recursiveSC */

////////////////////////////////////////////////////////////////////////////
///////////////////   	   Constructor + Destructor		    ////////////////
////////////////////////////////////////////////////////////////////////////


PC::PC()
{
	// Initialize PC initial parameters
	length = 0;
	loglength = 0;
	dimension = 0;
	designSNRdb = 0;


	// Initialize PC generation
	Rn = new int[length];
	for ( int i = 0 ; i < length ; i++ )
		Rn[i] = 0;

	arragedBits = new int [length]; // arranged from best to worst channel
	for ( int i = 0 ; i < length ; i++ )
		arragedBits[i] = i;

	infoBits = new int [dimension];
	for ( int i = 0 ; i < dimension ; i++ )
		infoBits[i] = 0;

	frozenBits = new int [length-dimension];
	for ( int i = 0 ; i < length-dimension ; i++ )
		frozenBits[i] = 0;

	frozenBitsPos = new bool [length];
	for ( int i = 0 ; i < length ; i++ )
		frozenBitsPos[i] = false;

	genMatrix = new int * [length];
	for ( int i = 0 ; i < length ; i++ )
		genMatrix[i] = new int[length];

	for ( int i = 0 ; i < length ; i++ )
		for ( int j = 0 ; j < length ; j++ )
			genMatrix[i][j] = 0;

	// Initialize variables needed for decoding
	llr = new double * [loglength];
	pos = new int * [loglength];
	msg = new int * [loglength];
	cw = new int * [loglength];
	int j;
	for ( int i = 0 ; i < loglength ; i++ )
	{
		j = pow(2,i+1);
		llr[i] = new double[j] ;
		pos[i] = new int [j];
		msg[i] = new int [j];
		cw[i] = new int [j];

		for ( j = 0 ; j < pow(2,i+1) ; j++ )
		{
			llr[i][j] = 0;
			pos[i][j] = 0;
			msg[i][j] = 0;
			cw[i][j] = 0;
		}
	}
	sentFrozenArranged = new int [length];
	for ( int i = 0 ; i < length ; i++ )
		sentFrozenArranged[i] = 0;

}/* PC */

PC::~PC() {
	// TODO Auto-generated destructor stub
}

