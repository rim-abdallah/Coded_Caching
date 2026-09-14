#include "EnvironmentSetup.h"
#include <vector>

namespace caching{

int n_utenti_env;
int m_files_env;
int b_chuncks_env;

int *Q_array = NULL;

/******************************************************************************************************************************************/

/*This is a function that provide to generate, for each user, a random request*/
void randomQVector(double **probs){
    int i, idx_rand;

    /******************************************************************************************/
    unsigned long int randSeed = 0;
    gsl_rng *r;
    gsl_ran_discrete_t *rand_disc;

    srand(time(NULL));                    /* initialization for rand() */
    randSeed = rand();                    /* returns a non-negative integer */

    gsl_rng_env_setup();
    gsl_rng_default_seed = randSeed;

    r = gsl_rng_alloc(gsl_rng_default);

    /******************************************************************************************/
    /*For each user generate a random request*/
    for (i=0; i<n_utenti_env; i++){
		rand_disc = gsl_ran_discrete_preproc (m_files_env, probs[i]);
        idx_rand =  gsl_ran_discrete (r, rand_disc);

		if (idx_rand >= m_files_env || idx_rand < 0){
			printf("\nError: Random Index.\n");
			exit(0);
		}

        Q_array[i] = idx_rand;
    }
}

/*This is a function that generates a binary random cache matrix*/
void randomIndMatrix(float memory_discretizzata, int **M, vector<int> &input, int number_of_request, vector<int> memory_per_user){
    int i, j, k, n_list, idx_rand, x, idx_sel;
    //random_list *l = NULL, *temp = NULL, *h = NULL;
    int cacheVecLen;
    cacheVecLen = floor(b_chuncks_env/6);

    int index_input = input.size() - 1;
    
    for (i=0; i<n_utenti_env; i++){
        input.at(index_input) = number_of_request;
        index_input--;
        input.at(index_input) = Q_array[i];
        index_input--;
        input.at(index_input) = memory_per_user.at(i);
        index_input--;

        for (j=0; j<m_files_env; j++){
 
            /*The matrix M indicates the number of chunks related to files 'j' that user 'i' can memorize*/
            
            /*if(b_chuncks_env%4==0){
                cacheVecLen = (b_chuncks_env/4);
                //cout << "Vector length of possible cache = " << cacheVecLen << endl;
            }else{
                cout << "Chunks per file should be multiple of 4";
                exit(0);
            }*/
            //cout << M[i][j] << endl;
            /*if(M[i][j]%4 != 0){
                cout << "Chunks to store should be multiple of 4";
                exit(0);
            }*/

            //Vector that contains the chunks indices for possible caching as package of 4
            //0...5, 6...11, 12...17, ...
            //Should be redifined because it is modified for every file
            vector<int> chunks_vec;
            for (int i = 0; i < cacheVecLen; ++i)
                chunks_vec.push_back(6*i);

            //This variable a defines length of the caching vector, 4 chunks per row, the last row might include less 
            int a =  M[i][j]/6;
            int f = M[i][j]%6;
            if(f != 0)
                a += 1;
            int lim = 6;

            for (k=0; k< a; k++){
                /*Random index to extract from the id chunks list the id chunk, related to files 'j', that the user 'i' must memorize*/
                idx_rand = round(randomNumber(0, chunks_vec.size()-1));
                //cout << idx_rand << " : ";
                
                if(k == a-1)
                    lim = (f == 0) ? 6: f;
                    
                for (int i = 0; i < lim; ++i){
                    input.at(index_input) = j;
                    index_input--;
                    input.at(index_input) = chunks_vec[idx_rand]+i;
                    index_input--;
                    //cout << chunks_vec[idx_rand]+i << " : ";
                }
                chunks_vec.erase(chunks_vec.begin()+idx_rand);

            }
        }
    }

    if (index_input >=0){
        printf("Error input vector!! \n\n");
        exit(0);
    }

    /*Check if all free memory slot have been filled */
    //check_Ind_Matrix();
}

/*void check_Ind_Matrix(){
    int i, j, k, sum;

    sum = 0;
    for (i=0; i<n_utenti_env; i++){
        for (j=0; j<m_files_env; j++){
            for (k=0; k<b_chuncks_env; k++){
                if (Ind[i][j][k] == 1){
                    sum++;
                }
            }
        }
    }

    if (sum != memory_discretizzata ){
        printf("\nError: randomIndMatrix Function.\n");
		exit(0);
    }
}*/

/******************************************************************************************************************************************/
void _allocVariables(){
    Q_array = (int *) malloc (n_utenti_env * sizeof(int));
    check_memory_allocation_1D(Q_array, "Q Vector.");
}

void _deallocVariables(){
    free(Q_array);
    Q_array = NULL;
}
/******************************************************************************************************************************************/


vector <int>setEnvironment(unsigned int n_utenti_in, unsigned int m_files_in, unsigned int b_chuncks_in, double **probs, int **M, vector <int> &memory_per_user){
    vector <int> input;
    //vector <int> memory_per_user(n_utenti_in, 0);
    int input_size;
    int number_of_request = 1;

    n_utenti_env = n_utenti_in;
    m_files_env = m_files_in;
    b_chuncks_env = b_chuncks_in;

    _allocVariables();

    float memory_discretizzata = 0;
    for (int k=0; k<n_utenti_env; k++){
        for (int j=0; j<m_files_env; j++){
            memory_discretizzata += M[k][j];
            memory_per_user.at(k) += M[k][j];
        }
    }

    input_size = memory_discretizzata * 2 + (n_utenti_env * (2 + number_of_request));
    input.resize(input_size);

    /*Generate the random vector of the user's requests*/
    randomQVector(probs);

    /*Generate the random binary matrix for the user's cache*/
    randomIndMatrix(memory_discretizzata, M, input, number_of_request, memory_per_user);

    //_deallocVariables();

    /* Input variables deallocation */
    /*for (int i = 0; i < n_utenti_env; i++){
        free(probs[i]);
    }

    for (int i = 0; i < n_utenti_env; i++){
        free(M[i]);
    }

    free(M);
    free(probs);*/

    return input;
}

}//end namespace caching