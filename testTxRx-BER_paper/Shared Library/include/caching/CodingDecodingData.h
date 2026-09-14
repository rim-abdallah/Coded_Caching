#ifndef CODINGDECODINGDATA_H_INCLUDED
#define CODINGDECODINGDATA_H_INCLUDED

#include <iostream>
#include <string>
#include <vector>
#include <math.h>
#include <fstream>
#include <algorithm>
#include "utilityForTesting.h"
#include "DataDefinition.h"
#include "Conversions.h"
#include "EnvironmentSetup.h"

namespace caching{

using namespace std;

vector< vector<char> > codingData(int *coloring, int n_col, data_matrix data, cf_data outputForColoring, header_transmission **header_data);

void decodingData(header_transmission header, vector<char> &coded_data, unsigned int m_files, unsigned int b_chuncks, unsigned int id_utente, unsigned int id_demand, unsigned int id_requested_file, unsigned int *n_package_remains);

}//end namespace caching

#endif // CODINGDECODINGDATA_H_INCLUDED
