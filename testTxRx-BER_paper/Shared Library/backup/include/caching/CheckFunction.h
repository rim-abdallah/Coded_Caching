#ifndef CHECKFUNCTION_H
#define CHECKFUNCTION_H

#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include "gr_complex.h"


namespace caching{

void check_memory_allocation_1D(int *pt, std::string msg);
void check_memory_allocation_2D(int **pt, std::string msg);
void check_memory_allocation_3D(int ***pt, std::string msg);
void check_memory_double_allocation_1D(double *pt, std::string msg);
void check_memory_double_allocation_2D(double **pt, std::string msg);
void check_memory_complex_allocation_1D(gr_complex *pt, std::string msg);
void check_memory_complex_allocation_2D(gr_complex **pt, std::string msg);

}//end namespace caching

#endif // CHECKFUNCTION_H
