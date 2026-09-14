#include "ConflictGraph.h"

namespace caching{

/* Variables declaration */
int n_utenti;
vector<int> chunks_Node; //Nb chunks per packet/node per user
int m_files;
int b_chunks;
vector<int> nodi_user; //Nb nodes per user
int m_archi;
unsigned int sizenodes;

int ***Ind = NULL;
int *Q = NULL;
int **Q_chuncks = NULL;


vector<nodo> nodes; //The vector (structure) of nodes
int **Matrix_Adj = NULL;
nodo *node_list = NULL;

/*************************************************************************************************************/
void computeNumberOfNodes(){
    int i, k, id_file;

    cout << "--- Compute Nodes ---" << endl;
    /*For each user ...*/
    for (i=0; i<n_utenti; i++){
        //nodi_user[i] will contain at the end the number of chunks requested by (not in the cache of) user i 
        nodi_user.push_back(0);
        /*The file that the user 'i' request*/
        id_file = Q[i];
        /*For each chunk ...*/
        for (k=0; k<b_chunks; k++){
            
            /*Check if user 'i' have in its cache the chunk 'k' related to files 'id_file'*/
            if (Ind[i][id_file][k] == 1){
                Q_chuncks[i][k] = 0;
            }else{
                Q_chuncks[i][k] = 1;
                nodi_user[i]++;
            }
            //nodi_user[i] = ceil(nodi_user[i]/chunks_Node[i]);
        }
        cout << nodi_user[i] << ", ";
    }
    cout << endl;
}

/*This is a function that builds the nodes of the conflict information graph*/
void makeNodes(){
    int i, j, k, l, id, nb_pck;
    id = 0; nb_pck = 0;
    nodo n1;

    cout << "--- Make Nodes ---" << endl;

    /*For each user and for each chunk related to the requested file 
    that the user do not have in cache will be created a node*/
    for (i=0; i<n_utenti; i++){
        //nb_pck will contain the number of packets required given the code rate (chunks_Node)
        nb_pck += ceil((double)nodi_user[i]/chunks_Node[i]);
        //cout << nb_pck << ", ";
        vector<unsigned int> chunks_f;
        j = Q[i]; l  = 0; k=0;
        //cout << chunks_Node[i] << endl;
        if(chunks_Node[i]==3){           
            //Add the remaining packets to the vector of packets
            for (int k = 0; k < b_chunks; k++){
                if (Q_chuncks[i][k] == 1){ //index == -1 && 
                    if(l==0){
                        n1.id = id;
                        n1.degree = 0;
                        n1.id_utente = i;
                        n1.id_file = j;
                        n1.id_chunck.push_back(k);
                        chunks_f.push_back(k);
                        l++;
                    }else if(l==1){
                       n1.id_chunck.push_back(k);
                       l++;
                    } else if (l==2){
                        n1.id_chunck.push_back(k);
                        nodes.push_back(n1);
                        n1.id_chunck = vector<int> ();
                        l=0;
                        id++; 
                    }   
                } 

                if((l==1 || l==2) && (k == (b_chunks-1))) {
                    nodes.push_back(n1);
                    n1.id_chunck = vector<int> ();
                    id++;
                }
            }
            /*for(int cc=0; cc< nodes.size(); cc++)
                for(int ccc=0; ccc<nodes[cc].id_chunck.size(); ccc++)
                    cout << nodes[cc].id_chunck[ccc] << ", ";*/

        } else if (chunks_Node[i] < 3){
            while(k < b_chunks){
                if(Q_chuncks[i][k] == 1){
                    if(l==0 || chunks_Node[i] == 1){
                        n1.id = id;
                        n1.degree = 0;
                        n1.id_utente = i;
                        n1.id_file = j;
                        n1.id_chunck.push_back(k);
                        l++;
                        if(l == chunks_Node[i] || k == (b_chunks-1)){//here l==chunks_Node[i] means chunks_Node[i] =1
                            nodes.push_back(n1);
                            n1.id_chunck = vector<int> ();
                            l=0;
                            id++;
                            //cout << k << ", " << id << endl;
                        }
                    }else if (l < chunks_Node[i]){
                        n1.id_chunck.push_back(k);
                        l++;
                        if(l == chunks_Node[i] || k == (b_chunks-1)){
                            nodes.push_back(n1);
                            n1.id_chunck = vector<int> ();
                            l=0;
                            id++;
                        }
                    }
                }      
                k++;   
            }
        } else {
            cout << "Case NOT Suported!" << endl;
            exit(0);
        }
        cout << "\nNb of nodes: " << nodes.size() << endl;
        
        if (id != nb_pck){
            cout << "\nError: Create Nodes Number Dismatch With The Aspected Nodes Number.\n";
            cout << id << "!=" << nb_pck << endl;
            exit(0);
        }
    }
    
    sizenodes = nodes.size();
    cout << "Number of node: " << sizenodes << endl;
    cout << "--- Make Nodes Completed ---" << endl;
    
}

/*This is a function that provide to make a edges of the conflict information graph*/
void makeEdges(){
    int i_1, j_1, i_2, j_2, id1, id2;
    unsigned int i, j, cc;
    cc=0;
    
    vector<int> c1;
    vector<int> c2;
    bool edge;
    m_archi = 0;

    cout << "--- Make Edges ---" << endl;

    Matrix_Adj = (int **) malloc (sizenodes * sizeof(int *));
    check_memory_allocation_2D(Matrix_Adj, "Allocation 2D Adj Matrix.");

    for (j=0; j<sizenodes; j++){
        Matrix_Adj[j] = (int *) malloc (sizenodes * sizeof(int));
        check_memory_allocation_1D(Matrix_Adj[j], "Allocation 1D Adj Matrix.");
    }

    for (j=0; j<sizenodes; j++){
        Matrix_Adj[j][j] = 0;
    }

    for (id1 = 0; id1 < sizenodes-1; ++id1){
        for (id2 = (id1+1); id2 < sizenodes; ++id2){
            i_1 = nodes[id1].id_utente;
            j_1 = nodes[id1].id_file;
            c1 = nodes[id1].id_chunck;

            i_2 = nodes[id2].id_utente;
            j_2 = nodes[id2].id_file;
            c2 = nodes[id2].id_chunck;

            //Check if all chunks associated to node id1 are in the cache of the user associated with node id2
            edge = false;
            for(i=0; i<c1.size(); i++){
                if(Ind[i_2][j_1][c1[i]] == 0 )
                    edge = true;
            }
            for(i=0; i<c2.size(); i++){
                if(Ind[i_1][j_2][c2[i]] == 0 )
                    edge = true;
            }
            if(edge){
                Matrix_Adj[id1][id2] = 1;
                Matrix_Adj[id2][id1] = 1;
                nodes[id1].degree++;
                nodes[id2].degree++;
                m_archi += 2;
            } else{
                Matrix_Adj[id1][id2] = 0;
                Matrix_Adj[id2][id1] = 0;
                cc++;
            }
            
        }
    }
    //cout << cc << endl;
}

/****************************************************************************************************************/
void _dealloc(){
    int i;

    free(Q);
    Q = NULL;

    for (i=0; i<n_utenti; i++){
        free(Q_chuncks[i]);
        Q_chuncks[i] = NULL;
    }

    free(Q_chuncks);
    Q_chuncks = NULL;

    nodes = vector<nodo> ();

}

/*****************************************************************************************************************/

/*This is a main function*/
cf_data conflictGraphGenerator(data_matrix data, vector<int> coderate){
    m_files = data.m_files;
    b_chunks = data.b_chunks;
    n_utenti = data.n_utenti;
    Ind = data.Ind;
    Q = data.Q;
    Q_chuncks = data.Q_chuncks;
    cf_data output;
    unsigned int i;

    chunks_Node = coderate;//6 because we assume the chunk size is 1/6 of the packet size
    cout << "--- Conflict Graph ---" << endl;
    if (Ind != NULL && Q != NULL && Q_chuncks != NULL){
        /*Bulding a conflict information graph*/
        computeNumberOfNodes();

        if (nodi_user[0] > 0){
            makeNodes();
            makeEdges();
        }else{
            m_archi = 0;
        }

        output.Matrix_Adj = Matrix_Adj;
        output.n_nodi = sizenodes;
        //Copy the nodes into the array node_list for compatibility with the functions in other files
        node_list = (nodo *) malloc (sizenodes * sizeof(nodo));
        if (!node_list){
            printf("\nError: Allocation Nodes.\n");
            exit(0);
        }
        cout << "Nb chunks per node: \n";
        for (i = 0; i < sizenodes; ++i){
            cout << nodes.at(i).id_chunck.size() << ",";
            node_list[i].id = nodes.at(i).id;
            node_list[i].degree = nodes.at(i).degree;
            node_list[i].id_utente = nodes.at(i).id_utente;
            node_list[i].id_file = nodes.at(i).id_file;
            node_list[i].id_chunck = nodes.at(i).id_chunck;
        }
        cout << endl;
        output.nodes = node_list;
        output.Ind = Ind;

        //_dealloc();
    }else{
        output.n_nodi = 0;
    }

    return output;
}

}//end namespace caching