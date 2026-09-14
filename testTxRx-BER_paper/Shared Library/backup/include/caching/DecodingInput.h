#ifndef DECODINGINPUT_H
#define DECODINGINPUT_H

#include <vector>
#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "CheckFunction.h"
#include "DataDefinition.h"
#include "CodingDecodingData.h"

namespace caching{

using namespace std;

void _allocArrays(unsigned int, unsigned int, unsigned int);
data_matrix fromArrayToMatrix(unsigned int, unsigned int, vector<int>);
void computeQMatrix(unsigned int, unsigned int);
void computeIndMatrix(vector<int>);

}//end namespace caching

#endif
