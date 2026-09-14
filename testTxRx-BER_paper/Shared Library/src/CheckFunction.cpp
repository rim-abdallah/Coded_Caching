#include "CheckFunction.h"


namespace caching{

using namespace std;

static const char memory_erro[] = "\nError: Memory Allocation :";

void check_memory_double_allocation_1D(double *pt, string msg){
    if (!pt){
        cout << endl << memory_erro << " " << msg << endl << endl;
		exit(0);
    }
}

void check_memory_double_allocation_2D(double **pt, string msg){
    if (!pt){
        cout << endl << memory_erro << " " << msg << endl << endl;
		exit(0);
    }
}

void check_memory_allocation_1D(int *pt, string msg){
    if (!pt){
		cout << endl << memory_erro << " " << msg << endl << endl;
		exit(0);
    }
}

void check_memory_allocation_2D(int **pt, string msg){
    if (!pt){
        cout << endl << memory_erro << " " << msg << endl << endl;
		exit(0);
    }
}

void check_memory_allocation_3D(int ***pt, string msg){
    if (!pt){
        cout << endl << memory_erro << " " << msg << endl << endl;
		exit(0);
    }
}

}//end namespace caching