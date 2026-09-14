#ifndef DATADEFINITION_H
#define DATADEFINITION_H

#include <limits>
#include <vector>

namespace caching{

using namespace std;

const int INF = numeric_limits<int>::max();

typedef struct NODE_LIST_STRUCT{
    int id;
    int degree;
    int id_utente;
    int id_file;
    int id_chunck;
    struct NODE_LIST_STRUCT *next;
}NODE_LIST;

typedef struct LIST_STRUCT{
    NODE_LIST *nodes_list;
}LIST;

typedef struct NODE_SUBSET_STRUCT{
    int id;
    int cardinality;
	int *label;
	int size_label;
    struct NODE_SUBSET_STRUCT *next;
}NODE_SUBSET;

typedef struct SUBSET_STRUCT{
    NODE_SUBSET *pt_end;
    NODE_SUBSET *nodes_list;
}SUBSET;

typedef struct NODO{
    int id;
    int degree;   //number of edges connection
    int id_utente;
    int id_file;
    int id_chunck;
}nodo;

typedef struct ARC_STRUCT{
    int adj;
}ARC;

typedef struct NODE_STRUCT{
    int id;
    int degree;
    struct ARC_STRUCT *arcList;
}NODE;

typedef struct RCL_LIST_STRUCT{
    int i;
}RCL_LIST;

typedef struct RANDOM_LIST{
    int idx;
    struct RANDOM_LIST *next;
}random_list;

typedef struct DATA_MATRIX{
    unsigned int n_utenti;
    unsigned int m_files;
    unsigned int b_chunks;
    int ***Ind;
    int *Q;
    int **Q_chuncks;
}data_matrix;

typedef struct CF_DATA{
    int **Matrix_Adj;
    nodo *nodes;
    int n_nodi;
    int ***Ind;
}cf_data;

typedef struct HEADER_TRANSMISSION{
    vector<unsigned int> id_utenti;
    vector<unsigned int> id_files;
    vector<unsigned int> id_chunks;
    vector<unsigned int> size_package;
}header_transmission;

typedef struct HEADER_POLAR{
    vector<unsigned int> id_utenti;
    vector<unsigned int> id_files;
    vector<unsigned int> id_chunks;
    vector<unsigned int> size_package;
    bool strong;
    bool weak;
}header_polar;

}//end namespace caching

#endif // DATADEFINITION_H
