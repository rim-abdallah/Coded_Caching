#include "DecodingInput.h"

namespace caching{

int ***Ind_cache = NULL;
int *Q_demand = NULL;
int **Q_matrix = NULL;
int n_users = 0;
int f=0;

/*This is a function that provide a compute the Ind cache matrix, this matrix is a binary matrix, Ind[i,j,k] = 0 if the user 'i' not have the chunk 'k' associated to file 'j'*/
void computeIndMatrix(vector<int> input){
    int index = input.size() - 1;
    int id_user = 0;
    double alpha = 0;
    vector<int> Q(n_users, 0);
    gen_rand_request_zipf(f, alpha, Q);
    for (int i=0; i<n_users; i++){
        Q_demand[i] = Q[i];
    }

    while (index >=0){
        int n_o_r = input.at(index);
        index--;

        for (int i = 0; i < n_o_r; i++){
            //Q_demand[id_user] = input.at(index);
            index--;
        }


        int m_user = input.at(index);
        index--;

        for (int i = 0; i < m_user; i++){
            int id_file = input.at(index);
            index --;
            int id_chunk = input.at(index);
            index --;
            Ind_cache[id_user][id_file][id_chunk] = 1;
        }

        id_user++;
    }
}

/*This is a function that provide a compute the Q matrix, this matrix is a binary matrix, Q[i,k] = 0 if the user 'i' have in cache the chunk 'k' related to files that he request*/
void computeQMatrix(unsigned int n_utenti, unsigned int b_chuncks){
    unsigned int i, k;
    int id_file;

    /*For each user ...*/
    for (i=0; i<n_utenti; i++){
        /*For each chunk ...*/
        for (k=0; k<b_chuncks; k++){
            /*The file that the user 'i' request*/
            id_file = Q_demand[i];
            /*Check if user 'i' have in cache the chunk 'k' related to files 'id_file'*/
            if (Ind_cache[i][id_file][k] == 1){
                Q_matrix[i][k] = 0;
            }else{
                Q_matrix[i][k] = 1;
            }
        }
    }
}

void _allocArrays(unsigned int n_utenti, unsigned int b_chuncks, unsigned int m_files){
    unsigned int i, j ,k;
    Ind_cache = (int ***) malloc (n_utenti * sizeof(int **));
    check_memory_allocation_3D(Ind_cache, "Allocation 3D Ind Matrix.");

    for (i = 0; i < n_utenti; i++){
        Ind_cache[i] = (int **) malloc (m_files * sizeof(int *));
        check_memory_allocation_2D(Ind_cache[i], "Allocation 2D Ind Matrix.");
        for (j = 0; j < m_files; j++){
            Ind_cache[i][j] = (int *) malloc (b_chuncks * sizeof(int));
            check_memory_allocation_1D(Ind_cache[i][j], "Allocation 1D Ind Matrix.");
        }
    }

    /*Initialization of binary matrix, Ind[i,j,k] = 0 if the user 'i' not have the chunk 'k' associated to file 'j'*/
    for (i=0; i<n_utenti; i++){
        for (j=0; j<m_files; j++){
            for (k=0; k<b_chuncks; k++){
                Ind_cache[i][j][k] = 0;
            }
        }
    }

    Q_matrix = (int **) malloc (n_utenti * sizeof(int *));
    check_memory_allocation_2D(Q_matrix, "Allocation 2D Q Matrix.");
    for (i = 0; i < n_utenti; i++){
        Q_matrix[i] = (int *) malloc (b_chuncks * sizeof(int));
        check_memory_allocation_1D(Q_matrix[i], "Allocation 1D Q Matrix.");
    }

    Q_demand = (int *) malloc (n_utenti * sizeof(int));
    check_memory_allocation_1D(Q_demand, "Allocation 1D Q Demand.");
}

data_matrix fromArrayToMatrix(unsigned int m_files, unsigned int b_chuncks, vector<int> input){
    data_matrix data;

    unsigned int n_utenti = 0;
    int index = input.size() - 1;

    while (index >=0){
        n_utenti++;
        int n_o_r = input.at(index);
        index -= (n_o_r + 1);
        int m_user = input.at(index);
        index -= ((2 * m_user) + 1);
    }
    n_users = n_utenti;
    f = m_files;
    //printf("Il numero di utenti e' pari a: %d\n\n", n_utenti);

    _allocArrays(n_utenti, b_chuncks, m_files);

    /*Compute the cache binary matrix*/
    computeIndMatrix(input);

    /*Compute the binary matrix Q*/
    computeQMatrix(n_utenti, b_chuncks);

    data.n_utenti = n_utenti;
    data.b_chunks = b_chuncks;
    data.m_files = m_files;
    data.Q = Q_demand;
    data.Q_chuncks = Q_matrix;
    data.Ind = Ind_cache;

    return data;
}

}//end namespace caching