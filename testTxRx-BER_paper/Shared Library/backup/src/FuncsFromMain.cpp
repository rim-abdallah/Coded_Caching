#include "FuncsFromMain.h"


namespace caching{

int checkTransmission(string original_file, string decoded_file){
    FILE* f1 = fopen(original_file.c_str(), "r");
    FILE* f2 = fopen(decoded_file.c_str(), "r");

    int N = 10000;
    char buf1[N];
    char buf2[N];

    int ritorno = 1;

    do {
        size_t r1 = fread(buf1, 1, N, f1);
        size_t r2 = fread(buf2, 1, N, f2);

        if (r1 != r2 || memcmp(buf1, buf2, r1)) {
          ritorno = 0;
        }
    } while ((!feof(f1) || !feof(f2)) && ritorno);

    fclose(f1);
    fclose(f2);

    return ritorno;
}

void makeCache(unsigned int id_utente, int **Ind, vector<string> files, unsigned int b_chuncks, 
    unsigned int m_files, unsigned int *n_package_remains, unsigned int id_requested_file, string pathFolder){
    //string pathFolder = "/CachingFile/cache/UserCache/user_" + to_string(id_utente);
    pathFolder = "UserCache/user_" + to_string(id_utente);
    if (mkdir(pathFolder.c_str(), S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH) == -1) {
        /*cout << endl << "Error creating folder: " << pathFolder << endl;
        exit(0);*/
    }

    (*n_package_remains) = 0;

    for (unsigned int i = 0; i < m_files; i++){
        for (unsigned int j = 0; j < b_chuncks; j++){
            if (Ind[i][j] == 1){
                /*Reading files*/
                string file = files.at(i);
                ifstream is (file, ifstream::binary);
                if (is) {
                    double size_file = getFileSize(file);
                    int size_package = ceil(size_file / (double) b_chuncks);

                    int begin_package = (j * size_package);

                    if (begin_package > size_file){
                        begin_package = size_file;
                        size_package = 0;
                    }else{
                        if (begin_package + size_package > size_file){
                            size_package = abs(size_file - begin_package);
                        }
                    }

                    if (size_package > 0){
                        is.seekg(begin_package);

                        char *buffer = new char[size_package];

                        is.read(buffer, size_package);

                        /********************************** WRITE CACHE FILE *********************************/
                        // Open file cache for write package
                        string pathFile = pathFolder + "/" + to_string(i) + "_" + to_string(j) + ".cache";

                        ofstream outFile (pathFile, ifstream::binary);
                        if (outFile.is_open()){
                            outFile.write(buffer, size_package);
                            outFile.close();
                        }
                        else{
                            cout << endl << "Error writing file cache: " << pathFile << endl;
                            exit(0);
                        }
                        /*************************************************************************************/

                        delete[] buffer;
                    }

                    is.close();
                }else{
                    cout << endl << "Error opening file in storage: " << file << endl;
                    exit(0);
                }
            }else{
                if(i == id_requested_file){
                    (*n_package_remains)++;
                }
            }
        }
    }
}

void colorRienumeration(int n_col, int **coloring, int n){
    int new_col = 0;
    for (int i = 0; i < n; i++){
        int col = i + 1;
        int isThere = 0;
        for (int j = 0; j < n; j++){
            if ((*coloring)[j] == col){
                (*coloring)[j] = new_col + 1;
                isThere = 1;
            }
        }

        if (isThere){
            new_col++;
        }
    }

    if (new_col != n_col){
        cout << endl << "Number color error: new: " << new_col << " original: " << n_col << endl;
        exit(0);
    }
}

void deallocationAllVariables(cf_data *outputForColoring, data_matrix data, int *coloring){
    free(data.Q);
    data.Q = NULL;

    for (unsigned int i=0; i < data.n_utenti; i++){
        free(data.Q_chuncks[i]);
        data.Q_chuncks[i] = NULL;
    }

    free(data.Q_chuncks);
    data.Q_chuncks = NULL;

    for (unsigned int i=0; i<data.n_utenti; i++){
        for (unsigned int j=0; j<data.m_files; j++){
            free((*outputForColoring).Ind[i][j]);
            (*outputForColoring).Ind[i][j] = NULL;
        }

        free((*outputForColoring).Ind[i]);
        (*outputForColoring).Ind[i]  = NULL;
    }

    free((*outputForColoring).Ind);
    (*outputForColoring).Ind = NULL;

    for(int i=0; i<(*outputForColoring).n_nodi; i++){
        free((*outputForColoring).Matrix_Adj[i]);
        (*outputForColoring).Matrix_Adj[i] = NULL;
    }

    free((*outputForColoring).Matrix_Adj);
    (*outputForColoring).Matrix_Adj = NULL;

    if (coloring != NULL){
        free(coloring);
        coloring = NULL;
    }

    free((*outputForColoring).nodes);
    (*outputForColoring).nodes = NULL;
}


data_matrix generateData(unsigned int m_files, unsigned int b_chuncks, unsigned int id_demand)
{
    data_matrix data;

    vector<int> input;

    unsigned int input_size = 0;

    int offset = 0;

    //Read environment file
    FILE *env_file;
    env_file = fopen("../environment/environment_file","rb"); //"/CachingFile/environment/environment_file"
    //cout << "Hello" << endl;
    fread(&input_size,sizeof(int),1,env_file);
    //cout << "Hello" << endl;

    offset = id_demand * (input_size + 1) * sizeof(int);
    //cout << "Hello" << endl;

    fseek(env_file, offset, SEEK_SET);
    //cout << "Hello" << endl;

    fread(&input_size,sizeof(int),1,env_file);
    //cout << "Hello" << endl;

    for(unsigned int i=0; i<input_size; i++)
    {
        input.push_back(0);
        fread(&input.at(i),sizeof(int),1,env_file);
    }
    
    fclose(env_file);

    //Generate Data
    if (input.size() > 0)
    {
        //cout << endl << "generating data interface.." << endl << "-------------" << endl << endl;
        data = fromArrayToMatrix(m_files, b_chuncks, input);
    }

    return data;
}


unsigned int readCacheInfo(unsigned int id_utente, unsigned int id_demand)
{
    FILE *cache_info;
    unsigned int n_package_remains = 0;
    int offset = 0;

    string pathFileInfo = "../cache/UserCache/cache_info_" + to_string(id_utente); //"/CachingFile/cache/UserCache/cache_info_"
    const char * filepath = pathFileInfo.c_str();

    offset = id_demand * sizeof(int);

    cache_info = fopen(filepath,"rb");

    fseek(cache_info, offset, SEEK_SET);

    fread(&n_package_remains,sizeof(int),1,cache_info);

    fclose(cache_info);

    return n_package_remains;
}


//ADD TO_STRING TO MY LIBRARY
string my_to_string(int a)
{
    string b = to_string(a);
    
    return b;
}

int index_find(vector<unsigned int> v, unsigned int value) 
{
    for (unsigned int i = 0; i < v.size(); ++i)
    {
        if(v[i] == value)
            return i;
    }
    return -1;
}


}//end namespace caching