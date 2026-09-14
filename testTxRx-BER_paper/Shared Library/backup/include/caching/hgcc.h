#ifndef HGCC_H
#define HGCC_H

#include "DataDefinition.h"
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>

namespace caching{

void computeSubsetsList();
void runProcess();
void IGCC_ColoringProcess();
int *localSearch(int *fo);

void checkConstraintsHGCC(int *sol);

void createCacheMatrix(cf_data outputForColoring);

void createGraphForHGCC(cf_data outputForColoring);

int *hGCC(int *n_col, data_matrix data, cf_data outputForColoring);

}//end namespace caching

#endif //HGCC_H
