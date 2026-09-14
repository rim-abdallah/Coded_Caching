#include "CheckFunction.h"
#include "EnvironmentSetup.h"
#include "DecodingInput.h"
#include "ConflictGraph.h"
#include "randomHandler.h"
#include "DataDefinition.h"
#include "grasp.h"
//#include "hgcc.h"
#include "CodingDecodingData.h"
#include "FuncsFromMain.h"
#include <algorithm>
#include <string>

#include "../config.h"

using namespace caching;

using namespace std;


void generateCache (unsigned int id_utente, unsigned int n_utenti, unsigned int m_files, unsigned int b_chuncks, data_matrix &data, FILE *cache_file, string pathFolder)
{


    unsigned int id_requested_file = data.Q[id_utente];               // Requested file for current user

    //cout << endl << "User id: " << id_utente;
    //cout << endl << "Request File id: " << id_requested_file << endl;

    /*-------------------------MODIFICATION FOR CORTEXLAB---------------------*/
    string pathFiles = "../repository"; //"/CachingFile/repository"
    vector<string> files;
    files = getDirectoryFiles(pathFiles);                              // Load all repository files

    if (files.empty())
    {
        cout << endl << "Repository folder is empty or it not exist." << endl << endl;
        exit(0);
    }

    /*Sort file by name*/
    //sort(files.begin(), files.end());


    sort(files.begin(), files.end(), [](const string& a, const string& b) 
    {
        auto getNumber = [](const string& s) 
        {
            size_t start = s.find("file_");
            size_t end = s.find(".xml");

            if (start == string::npos || end == string::npos)
                return 0;

            start += 5; // length of "file_"
            return stoi(s.substr(start, end - start));
        };

        return getNumber(a) < getNumber(b);
    });



    //cout << endl << "Request File Name: " << files.at(id_requested_file) << endl << endl;




    //cout << endl << " ----------------- Making cache ----------------- " <<endl;


    unsigned int n_package_remains = 0;
    
    cout << "\n==========================" << endl;
    cout << "USER " << id_utente << endl;
    cout << "Requested file = " << id_requested_file << endl;
    cout << "Cached chunks:" << endl;

    for (unsigned int f = 0; f < m_files; f++)
    {
        cout << "File " << f << " : ";

        bool has_cache = false;

        for (unsigned int c = 0; c < b_chuncks; c++)
        {
            if (data.Ind[id_utente][f][c] == 1)
            {
                cout << c << " ";
                has_cache = true;
            }
        }

        if (!has_cache)
            cout << "none";

        cout << endl;
    }

    cout << "==========================\n" << endl;

    makeCache(id_utente, data.Ind[id_utente], files, b_chuncks, m_files, &n_package_remains, id_requested_file, pathFolder);    //Create cache chunks/files for the user


    //cout << endl << " ----------------- Cache Done ! ----------------- " <<endl;


    //Write cache in binary file
    //cout << endl << " ----------------- Writing Cache info ----------------- " <<endl;



    //string pathFileInfo = "/CachingFile/cache/UserCache/cache_info_" + to_string(id_utente);
    //const char * filepath = pathFileInfo.c_str();

    //FILE *cache_file;
    //cache_file = fopen(filepath,"wb");                                           //file aperto in scrittura binaria
                                                                                   //file opened for binary writing

    fwrite(&n_package_remains,sizeof(int),1,cache_file);                         //scrivo il numero di pacchetti rimanenti
                                                                                //write the number of packets remaining(not cached by user )
    

    //fclose(cache_file);


    //cout << endl << " ----------------- Cache is ready ----------------- " <<endl<<endl;

}


int main()
{

    unsigned int n_utenti, m_files, b_chuncks, id_utente, L_request;

    n_utenti = N_USERS;
    m_files = N_FILES;
    b_chuncks = 100;
    L_request = 1;

    FILE *cache_file;

    printf("\n\nNumero Utenti: %d Numero Files: %d Numero Chunks: %d\n", n_utenti, m_files, b_chuncks);

    for(unsigned int id_utente=0; id_utente<n_utenti; id_utente++)
    {

         // Create a dedicated cache folder for each user
        string pathFolder = "../cache/UserCache/user_" + to_string(id_utente); // "/CachingFile/cache/UserCache/user_"
        if (mkdir(pathFolder.c_str(), S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH) == -1)
        {
            cout << endl << "Error creating folder: " << pathFolder << endl;
            exit(0);
        }


        //env_file = fopen("/CachingFile/environment/environment_file","rb");

        // Create binary file to store cache information
        // such as remaining packets/chunks
        string pathFileInfo = "../cache/UserCache/cache_info_" + to_string(id_utente); //"/CachingFile/cache/UserCache/cache_info_"
        const char * filepath = pathFileInfo.c_str();

        cache_file = fopen(filepath,"wb");

        //cout << endl << "User id : " << id_utente;

        for(unsigned int i=0; i<L_request; i++)
        {
            data_matrix data;

            data = generateData(m_files, b_chuncks );      //generate the simulation data


            //cout << endl << "Request file " << id_utente << " = " << data.Q[id_utente];

            generateCache(id_utente, n_utenti, m_files, b_chuncks, data, cache_file, pathFolder);  // generate cache content for the user and store shunks inside user cache folder
        }

        fclose(cache_file);
    }

    return 0;
}