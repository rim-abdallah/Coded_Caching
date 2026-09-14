#include "CodingDecodingData.h"

namespace caching
{

    vector< vector<char> > codingData(int *coloring, int n_col, data_matrix data, cf_data outputForColoring, 
        header_transmission **header_data)
    {

        string pathFiles = "../repository";//"/CachingFile/repository"

        unsigned int b_chuncks = data.b_chunks;

        nodo *nodi = outputForColoring.nodes;
        int n = outputForColoring.n_nodi;

        //open a file -- added new
        ofstream data_out;
        data_out.open("NbCombineUsers.txt",ios::app);

        header_transmission *header = new header_transmission[n_col];
        if (!header){
            cout << endl << "MEMORY EXCEPTION: header memory allocation." << endl << endl;
            exit(0);
        }

        vector<string> files;
        files = getDirectoryFiles(pathFiles);

        if (files.empty()){
            cout << endl << "Repository folder is empty or it not exist." << endl << endl;
            exit(0);
        }

        /*Sort file by name*/
        sort(files.begin(), files.end());

        /*Find max size on each package*/
        double max_size = -INF;
        for (string f : files){
            int size_file = getFileSize(f);

            if (size_file > max_size){
                max_size = size_file;
            }
        }

        int max_size_package = ceil(max_size / (double) b_chuncks);

        vector< vector<char> > coded_data(n_col, vector<char>(max_size_package, 0));

        /*For each transmission*/
        for (int i = 0; i < n_col; i++){
            int color = i + 1;
            int begin_data = 1;

            /*For each node*/
            for (int j = 0; j < n; j++){
                if (coloring[j] == color){
                    nodo *single_node = &(nodi)[j];

                    unsigned int id_utente = single_node->id_utente;
                    unsigned int id_file = single_node->id_file;
                    unsigned int id_chunck = single_node->id_chunck;

                    unsigned int size_pkg;

                    int trovato = 0;
                    unsigned int q = 0;
                    while (q < header[i].id_files.size() && !trovato){
                        if (id_file == header[i].id_files.at(q) && id_chunck == header[i].id_chunks.at(q)){
                            trovato = 1;
                            size_pkg = header[i].size_package.at(q);
                        }
                        q++;
                    }

                    if (!trovato){
                        /*Reading file */
                        string file = files.at(id_file);
                        ifstream is (file, ifstream::binary);

                        if (is) {
                            double size_file = getFileSize(file);
                            int size_package = ceil(size_file / (double) b_chuncks);

                            int begin_package = (id_chunck * size_package);

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

                                is.close();

                                for (int k = 0; k < size_package; k++){
                                    if (begin_data){
                                        coded_data.at(i).at(k) = buffer[k];
                                    }else{
                                        coded_data.at(i).at(k) = (char) coded_data.at(i).at(k) ^ buffer[k];
                                    }
                                }

                                delete[] buffer;

                                begin_data = 0;
                            }

                            header[i].id_utenti.push_back(id_utente);
                            header[i].id_files.push_back(id_file);
                            header[i].id_chunks.push_back(id_chunck);
                            header[i].size_package.push_back(size_package);
                        }else{
                            cout << endl << "Error opening file in storage: " << file << endl;
                            exit(0);
                        }
                    }else{
                        header[i].id_utenti.push_back(id_utente);
                        header[i].id_files.push_back(id_file);
                        header[i].id_chunks.push_back(id_chunck);
                        header[i].size_package.push_back(size_pkg);
                    }
                }
            }
        }

        //print into a file
        for (int i = 0; i < n_col; i++)
            data_out << header[i].id_utenti.size() << endl;
        data_out.close();

        (*header_data) = header;
        return coded_data;
    }

    void decodingData(header_transmission header, vector<char> &coded_data, unsigned int m_files, unsigned int b_chuncks, 
        unsigned int id_utente, unsigned int id_demand, unsigned int id_requested_file, unsigned int *n_package_remains)
    {
        string pathFolder = "../cache/UserCache/user_" + to_string(id_utente); //"/CachingFile/cache/UserCache/user_"

        char *coded_file_buffer = NULL;
        int trovato = 0;
        unsigned int id_requested_chunck;
        unsigned int size_requested_package;

        for (unsigned int i = 0; i < header.id_utenti.size(); i++){
            /*All info for requested package*/
            unsigned int id_user = header.id_utenti.at(i);
            unsigned int id_file = header.id_files.at(i);
            unsigned int id_chunck = header.id_chunks.at(i);
            unsigned int size_package_ = header.size_package.at(i);

            if (id_user == id_utente && id_file == id_requested_file){
                (*n_package_remains)--;

                if (size_package_ > 0){

                    trovato = 1;
                    id_requested_chunck = id_chunck;
                    size_requested_package = size_package_;

                    coded_file_buffer = new char[coded_data.size()];
                    for (unsigned int k = 0; k < coded_data.size(); k++){
                        coded_file_buffer[k] = coded_data.at(k);
                    }

                    for (unsigned int j = 0; j < header.id_files.size(); j++){
                        unsigned int id_chunck_xor = header.id_chunks.at(j);
                        unsigned int id_file_xor = header.id_files.at(j);
                        unsigned int size_package_xor = header.size_package.at(j);

                        /*************************************************/
                        int isAlreadyAnalized = 0;
                        unsigned int r = 0;
                        while (r < j && !isAlreadyAnalized){
                            if (id_file_xor == header.id_files.at(r) && id_chunck_xor == header.id_chunks.at(r)){
                                isAlreadyAnalized = 1;
                            }
                            r++;
                        }
                        /*************************************************/
                        if (i != j && (id_file != id_file_xor || id_chunck != id_chunck_xor) && !isAlreadyAnalized && size_package_xor > 0){
                            /********************************** READ CACHE FILE *********************************/
                            char *cache = new char[size_package_xor];

                            // Open file cache for read package
                            string pathFileCache = pathFolder + "/" + to_string(id_file_xor) + "_" + to_string(id_chunck_xor) + ".cache";

                            ifstream inFilePackage (pathFileCache, ifstream::binary);
                            if (inFilePackage){
                                inFilePackage.read(cache, size_package_xor);
                                inFilePackage.close();
                            }
                            else{
                                cout << endl << "Error reading file cache: " << pathFileCache << endl;
                                exit(0);
                            }
                            /***********************************************************************************/

                            /*Reading packages in cache*/
                            for (unsigned int k = 0; k < size_package_xor; k++){
                                coded_file_buffer[k] = coded_file_buffer[k] ^ cache[k];
                            }
                        }
                    }
                }
            }
        }

        if (trovato){
            /********************************** DELIVERY PACKAGE *********************************/
            // Open file for write delivery package
            string pathFileDelivery = pathFolder + "/" + to_string(id_requested_file) + "_" + to_string(id_requested_chunck) + ".cache";

            ofstream outFileDelivery (pathFileDelivery, ifstream::binary);
            if (outFileDelivery.is_open()){
                outFileDelivery.write(coded_file_buffer, size_requested_package);
                outFileDelivery.close();
            }
            else{
                cout << endl << "Error writing delivery package: " << pathFileDelivery << endl;
                exit(0);
            }
            /************************************************************************************/
        }

        //For testing pourpose
        if ((*n_package_remains) == 0 ){
            string name_file;
            name_file = "../trasmissioni/User_" + to_string(id_utente) + "/decoded_file_" + to_string(id_demand) + ".xml"; //"/CachingFile/trasmissioni/User_"
            ofstream outFile (name_file, ios::out | ios::binary);

            for (unsigned int i = 0; i < b_chuncks; i++){
                /********************************** READ ALL PACKAGE ********************************/
                // Open file cache for read package
                string pathFilePackage = pathFolder + "/" + to_string(id_requested_file) + "_" + to_string(i) + ".cache";
                int size_file = getFileSize(pathFilePackage);
                ifstream outFilePackage (pathFilePackage, ifstream::binary);

                if (size_file > 0){
                    char *buffer = new char[size_file];

                    if (outFilePackage){
                        outFilePackage.read(buffer, size_file);
                        outFilePackage.close();
                    }
                    else{
                        cout << endl << "Error reading package: " << pathFilePackage << endl;
                        exit(0);
                    }
                    /**********************************************************************************/

                    outFile.write(buffer, size_file);

                    delete[] buffer;
                }
            }

            outFile.close();
        }

        delete[] coded_file_buffer;
    }

}//end namespace caching