#include "CheckFunction.h"
#include "EnvironmentSetup.h"
#include "DecodingInput.h"
#include "ConflictGraph.h"
#include "randomHandler.h"
#include "DataDefinition.h"
#include "grasp.h"
//#include "hgcc.h"
#include "CodingDecodingData.h"
#include <fstream>
#include <sstream>
#include <string>
#include <cctype>
#include <limits>
#include <cstdlib>
#include <vector>
    #include <utility>
    #include <algorithm>
    #include <cmath>

    #include "../config.h"

    using namespace caching;

    using namespace std;


    void readProbabilitiesCSV(
        const string& filename,
        double **probs,
        unsigned int n_users,
        unsigned int m_files
    ) {
        ifstream file(filename);

        


        if (!file.is_open()) {
            cout << "Error: Cannot open probability CSV file: " << filename << endl;
            exit(0);
        }

        string line;

        // skip header
        getline(file, line);

        unsigned int user = 0;

        while (getline(file, line) && user < n_users) {
            stringstream ss(line);
            string value;

            // skip first column: User_0, User_1, ...
            getline(ss, value, ',');

            double sum = 0.0;

            for (unsigned int f = 0; f < m_files; f++) {
                if (!getline(ss, value, ',')) {
                    cout << "Error: Missing probability at user " << user
                         << ", file " << f << endl;
                    exit(0);
                }

                probs[user][f] = stod(value);
                sum += probs[user][f];
            }

            /*cout << "User " << user << " probability sum = " << sum << endl;

            for (unsigned int f = 0; f < m_files; f++)
            {
                cout << probs[user][f] << " ";
            }

            cout << endl;*/
            user++;
        }

        if (user != n_users) {
            cout << "Error: CSV contains " << user
                 << " users, expected " << n_users << endl;
            exit(0);
        }

        file.close();
    }

void readRequestRowCSV(const string& filename,unsigned int run_id,vector<int>& Q_array,unsigned int n_users,unsigned int m_files) 
{
    ifstream file(filename.c_str());

    if (!file.is_open()) {
        cout << "Error: Cannot open request CSV file: " << filename << endl;
        exit(0);
    }

    string line;
    getline(file, line); // skip header

    unsigned int current_run = 0;

    while (getline(file, line)) {
        stringstream ss(line);
        string value;

        getline(ss, value, ',');
        cout << "DEBUG run field = [" << value << "]" << endl;
        current_run = stoi(value);

        if (current_run == run_id) {
            for (unsigned int u = 0; u < n_users; u++) {
                if (!getline(ss, value, ',')) {
                    cout << "Error: missing request for user " << u << endl;
                    exit(0);
                }
                cout << "DEBUG request field for user "
                     << u << " = [" << value << "]"
                     << endl;


                unsigned int q = stoi(value);

                if (q >= m_files) {
                    cout << "Error: request file " << q
                         << " >= m_files " << m_files << endl;
                    exit(0);
                }

                Q_array.at(u) = q;
            }

            file.close();
            return;
        }
    }

    cout << "Error: run " << run_id << " not found in " << filename << endl;
    exit(0);
}

unsigned int calculateTNoCache(
    const vector<int>& Q_array,
    unsigned int m_files,
    unsigned int b_chunks
)
{
    // requested[file_id] is true when at least one user
    // requested that file.
    vector<bool> requested(m_files, false);

    unsigned int number_distinct_files = 0;

    for (unsigned int i = 0; i < Q_array.size(); i++)
    {
        int file_id = Q_array.at(i);

        if (file_id < 0 ||
            file_id >= static_cast<int>(m_files))
        {
            cout << "Error: invalid request file ID "
                 << file_id
                 << " for user " << i
                 << endl;

            exit(0);
        }

        // Count the file only the first time it appears.
        if (!requested.at(file_id))
        {
            requested.at(file_id) = true;
            number_distinct_files++;
        }
    }

    unsigned int T_no_cache =
        number_distinct_files * b_chunks;

    cout << "Number of distinct requested files = "
         << number_distinct_files << endl;

    cout << "T_no_cache = "
         << T_no_cache << endl;

    return T_no_cache;
}

    int main()
    {

        double alpha = 1.0;                                             //ALPHA PARAMETER for Zipf distribution //dont need it anymore 

        unsigned n_utenti = N_USERS;
        unsigned m_files = N_FILES;
        unsigned int RUN_ID = Run_ID;                                       //request line 

        //unsigned int n_utenti = 10;                                       //number of user
        unsigned int n_user_s = 0;                                       //WHAT is this ?  number of strong user ?   <----
        //unsigned int m_files = 6298;                                     //number of file
        unsigned int b_chunks = 100;                                     //number of chunks

        unsigned int L_request = 1;                                      //number of demands for each user
        //unsigned int top_k_files =6298;                                       //top k file 

        unsigned int M_cache = (CACHE_SIZE  * m_files / 100);                     //dimensione della cache 10% del catalogo
        unsigned int M_max = (70 *b_chunks / 100);                              // number of files to cache relative archive (2)
        unsigned int B_cache = M_cache * b_chunks;                       //numero totale di pacchetti int cache
                                                                        // size of cache chunks as an integer (not in bytes) (200) 

        int index_input;

        double *probs_vec;  // proba vector
        double sum=0;
        double sum_check;

        int *M_vec;                                                     //1D vector that show how many chuncks from each file will be cache for the user i
        int chunk_check;

        double **probs;                                                 //Matrice n_utenti X m_files inizializzata sotto
                                                                        // proba of M[user][file] intialized below
                                                                        // To be replaced, read from a text file lightgcn output
        int **M;                                                        //Matrice n_utenti X m_files inizializzata sotto
                                                                        //Matrix n_users X m_files initialized below
                                                                        //Matrix that show how many chuncks from each file will be cache for the user i (all user )



        double **probs_request;                                         //full initial probabilities

        double **probs_cache;                                           //top-k prbabilities

        vector<int> input;                                              //vettore prodotto da environment setup
                                                                        // vector created by env setup -?
        vector<int> memory_per_user(n_utenti, 0);                       //how much memory/chunks each user stores.  

        FILE *env_file;

        printf("\n\nNumero Utenti: %d Numero Files: %d Numero Chunks: %d\n", n_utenti, m_files, b_chunks);


        //probs_vec = (double *) malloc (m_files * sizeof(double));
        //check_memory_double_allocation_1D(probs_vec, "Allocation 1D Probs.");


    //   TO EDIT
        // should be commented and changed to take lightgcn proba --> user proba  --> distribution per user
        //Probs_vec[i] = probabilità dell'i-esimo file di essere richiesto (global distribution) 
        // creation of proba zipf --> global proba, 
        


        //probs = (double **) malloc (n_utenti * sizeof(double *));
        //check_memory_double_allocation_2D(probs, "Allocation 2D Probs.");

        probs_cache = (double **) malloc (n_utenti * sizeof(double *));
        check_memory_double_allocation_2D(probs_cache, "Allocation 2D Probs.");

        probs_request = (double **) malloc (n_utenti * sizeof(double *));
        check_memory_double_allocation_2D(probs_request, "Allocation 2D Probs_cache.");

        //for (unsigned int i=0; i<n_utenti; i++){
        //    probs[i] = (double *) malloc (m_files * sizeof(double));
        //    check_memory_double_allocation_1D(probs[i], "Allocation 1D Probs_request.");
        //}
        /*
        cout << "Check probs" << endl;
        for(unsigned int i=0; i<n_utenti; i++)
        {
          for (unsigned int j=0; j<20; j++){
            cout << endl << "Probs at (" << i << ") = " << probs[i][j] << endl;
          }    
        }*/

        //probs[i][j] = probabilità dell'utente i-esimo di richiedere il file j-esimo
        // proba of i-th user requesting the j-th file 

        for (unsigned int i = 0; i < n_utenti; i++)
        {
            probs_cache[i] = (double *) malloc(m_files * sizeof(double));
            check_memory_double_allocation_1D(probs_cache[i], "Allocation 1D Probs_cache.");

            probs_request[i] = (double *) malloc(m_files * sizeof(double));
            check_memory_double_allocation_1D(probs_request[i], "Allocation 1D Probs_request.");
        }


        readProbabilitiesCSV(PROBA_PATH_CACHE,probs_cache,n_utenti,m_files);
        readProbabilitiesCSV(PROBA_PATH_REQUEST,probs_request,n_utenti,m_files);
        
        // for(unsigned int i=0; i<n_utenti; i++)
        // {
          
        //      cout << "Check probs_cache for user :" << i<<endl;
        
        //     for (unsigned int j=0; j<m_files; j++)
        //     {
        //         cout << endl << "Probs_cache for file  (" << j << ") = " << probs_cache[i][j] << endl;
        //     }
        // }
          

    //    TO EDIT






        //FUNZIONI PER ARROTONDARE (rounding function)
        //round() round to nearest integer
        //ceil() round to next integer
        //floor() round to previous integer


        M_vec = (int *) malloc (m_files * sizeof(int));
        check_memory_allocation_1D(M_vec, "Allocation 1D M_vec.");


        M = (int **) malloc (n_utenti * sizeof(int *));
        check_memory_allocation_2D(M, "Allocation 2D M.");

        for (unsigned int i=0; i<n_utenti; i++){
            M[i] = (int *) malloc (m_files * sizeof(int));
            check_memory_allocation_1D(M[i], "Allocation 1D M.");
        }


        //M[i][j] = numero di pacchetti del j-esimo file che l'i-esimo utente ha in memoria
        // number of packages that the j-th file that the i-th user has in memory




       for (unsigned int i = 0; i < n_utenti; i++)
        {
            chunk_check = 0;

            vector<pair<double, unsigned int>> remainders;

            for (unsigned int j = 0; j < m_files; j++)
            {
                double raw = B_cache * probs_cache[i][j];

                if (probs_cache[i][j] <= 0.0)
                {
                    M[i][j] = 0;
                    continue;
                }

                M[i][j] = floor(raw);

                if (M[i][j] >M_max)
                    M[i][j] = M_max;

                chunk_check += M[i][j];

                double decimal_part = raw - floor(raw);
                remainders.push_back(make_pair(decimal_part, j));
            }

            sort(remainders.begin(), remainders.end(), greater<pair<double, unsigned int>>());

            int remaining = B_cache - chunk_check;

            while (remaining > 0)
            {
                bool added = false;

                for (unsigned int r = 0; r < remainders.size() && remaining > 0; r++)
                {
                    unsigned int file_id = remainders[r].second;

                    if (probs_cache[i][file_id] > 0.0 && M[i][file_id] < M_max)
                    {
                        M[i][file_id]++;
                        remaining--;
                        added = true;
                    }
                }

                if (!added)
                {
                    cout << "Warning: cannot fill full cache for user "
                         << i << ". Remaining chunks = "
                         << remaining << endl;
                    break;
                }
            }

            chunk_check = 0;

            for (unsigned int j = 0; j < m_files; j++)
            {
                if (probs_cache[i][j] <= 0.0 && M[i][j] != 0)
                {
                    cout << "ERROR: user " << i
                         << " cached file " << j
                         << " with zero cache probability!" << endl;
                    exit(0);
                }

                chunk_check += M[i][j];
            }

            cout << "User " << i
                 << " total cached chunks = "
                 << chunk_check << endl;
        }



        //strong users have cache memory equal to 0                 
        //they dont need cache can directly receive so they store nothing 
        /*for(unsigned int i=(n_utenti-n_user_s); i<n_utenti; i++)
            for(int k=0; k <m_files; k++)
                M[i][k] = 0;*/
        /*cout << "The memory is filled as follows: " << endl;
        for(int i=0; i<n_utenti; i++){
            cout << "USER " << i << ": " ;
            for(int k=0; k <m_files; k++)
                cout << M[i][k] << " ";
            cout << endl;
        }
        cout << endl;*/

        

        //Create the "input" vector -?  input vector is a vector that have pair of file chunks id then memory per user then requested file then nb of request 
        cout << "Creating input vec" << endl;
        //cout<<"before set enviremnt "<<endl;
        input = setEnvironment(n_utenti, m_files, b_chunks, probs_request, M, memory_per_user);
        for (int i = 0; i < n_utenti; ++i)
        { 
            cout <<  "USER : " << i  << " : " << memory_per_user.at(i) << ", ";
        }
        //cout <<endl<<"after" <<endl;

        //write the "input" vector in a file
        unsigned int input_dim = input.size();
        cout << endl;
        cout << "Input size : " << input_dim;
        cout << endl;
        

        env_file = fopen("./environment_file","wb");  

        /*fwrite(&input_dim,sizeof(int),1,env_file);

        for(unsigned int i=0; i<input_dim; i++)
        {
            fwrite(&input.at(i),sizeof(int),1,env_file);
        }*/


        /******************************************************************************************/
        /******************************************************************************************/
        /******************************************************************************************/
        vector <int> Q_array(n_utenti, 0);
        srand(time(NULL));
        // for(unsigned int k=0; k<L_request; k++)
        // {

        //     //FUNZIONE randomQvector
        //     //randomQvector function
        //     int idx_rand;

        //     /******************************************************************************************/
        //     unsigned long int randSeed = 0;
        //     gsl_rng *r;

        //     gsl_ran_discrete_t *rand_disc;

        //     //srand(time(NULL));                    /* initialization for rand() */
        //     randSeed = rand();                    /* returns a non-negative integer */

        //     gsl_rng_env_setup();
        //     gsl_rng_default_seed = randSeed;

        //     r = gsl_rng_alloc(gsl_rng_default);

        //     /******************************************************************************************/
        //     /*For each user generate a random request*/
        //     for (unsigned int i=0; i<n_utenti; i++)
        //     {
        //         rand_disc = gsl_ran_discrete_preproc (m_files, probs_request[i]); // TODO: proba will be based on User Preference 
        //         idx_rand =  gsl_ran_discrete (r, rand_disc);


        //         /*cout << "User " << i
        //              << " requested file " << idx_rand
        //              << " | request_proba = " << probs_request[i][idx_rand]
        //              << " | cache_proba = " << probs_cache[i][idx_rand]
        //              << endl;*/

        //         if (idx_rand >= m_files || idx_rand < 0)
        //         {
        //             printf("\nError: Random Index.\n");
        //             exit(0);
        //         }

        //         Q_array.at(i) = idx_rand;
        //         //cout << "Idx_rand " << idx_rand << endl;
        //     }

            

            readRequestRowCSV(REQUEST_PATH,RUN_ID,Q_array,n_utenti,m_files);

            //calcule de Tno_cache
            unsigned int T_no_cache = calculateTNoCache(Q_array,m_files,b_chunks);
            ofstream t_no_cache_file("./t_no_cache.txt");

            if (!t_no_cache_file.is_open())
            {
                cout << "Error: cannot create t_no_cache.txt"
                     << endl;

                exit(0);
            }

            t_no_cache_file << T_no_cache << endl;
            t_no_cache_file.close();

            for(unsigned int i=0; i<n_utenti; i++)
            {
                cout << "user "<<i<<"Request file "  << " : " << Q_array.at(i) << endl;
            }
            cout << endl;


            index_input = input.size() - 1;

            //Funzione modifica vettore di input
            //Function modify input vector
            for(unsigned int i=0; i<n_utenti; i++)
            {
                index_input --;
                input.at(index_input) = Q_array.at(i);
                index_input --;
                index_input = (index_input - ( memory_per_user.at(i) * 2 ) ) - 1;
            }


            input_dim = input.size();
            // cout << "Input size : " << input_dim;
            //cout << "The input size is: " << input_dim << endl;
            //cout << "The input vector is: " << endl;

            //for strong users with no cache, print: -the nb request -the id file -cache size
            for(int i=0; i<n_user_s; i++)
                for(int j=0; j<3; j++)
                    cout << input.at(3*i+j) << " ";
            cout << endl << endl;

            //for weak users, print -the nb request -the id requested file
            //-the memory size in chunks, and -the chunks to cache (file id, chunck id)
            //for (int i = 0; i < (n_utenti-n_user_s); i++){
            //   for (int j = 0; j < (memory_per_user.at(i)*2+3); j++){
            //        cout << input.at(3*n_user_s+(memory_per_user.at(i)*2+3)*i+j) << " ";
            //   }
            //   cout << endl << endl;
            //}

            for(int i=0; i<input_dim; i++)
               cout << input.at(i) << " ";  

            fwrite(&input_dim,sizeof(int),1,env_file);

            for(unsigned int i=0; i<input_dim; i++)
            {
                fwrite(&input.at(i),sizeof(int),1,env_file);
            }

        
        /******************************************************************************************/
        /******************************************************************************************/
        /******************************************************************************************/

        fclose(env_file);

        cout<<endl<<"Environment scritto correttamente!!"<<endl<<endl;


        return 0;
    }

