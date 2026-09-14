#ifndef FUNCSFROMMAIN_H_INCLUDED
#define FUNCSFROMMAIN_H_INCLUDED

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
#include "DecodingInput.h"
#include "Conversions.h"


namespace caching{

int checkTransmission(string original_file, string decoded_file);

void makeCache(unsigned int id_utente, int **Ind, vector<string> files, unsigned int b_chuncks, unsigned int m_files, unsigned int *n_package_remains, unsigned int id_requested_file, string pathFolder);

void colorRienumeration(int n_col, int **coloring, int n);

void deallocationAllVariables(cf_data *outputForColoring, data_matrix data, int *coloring);

data_matrix generateData(unsigned int m_files, unsigned int b_chuncks, unsigned int request_number);

unsigned int readCacheInfo(unsigned int id_utente, unsigned int id_demand);

string my_to_string(int a);

int index_find(vector<unsigned int>, unsigned int);

}//end namespace caching

#endif // FUNCSFROMMAIN_H_INCLUDED
