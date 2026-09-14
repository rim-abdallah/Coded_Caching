#include "TxRx.h"

#define SQRT_TWO 0.707107
typedef std::complex< float > gr_complex;


namespace caching{



void TX_PC_Pack(vector<header_polar> &hX, vector< vector<char> > coded_packets, unsigned int id_demand,
                vector<vector<char> > &tx, int spack_len, vector<int> &spack_size)
{
    unsigned short int id_header = 0;
    unsigned int field_len;
    unsigned int header_len;
    unsigned int payload_len;
    unsigned int id_large_packet;

    char byte;
    char buff_short[2];
    //char buff[4];

    bool DEBUG = false;
    int nbtest = 5;
    int cc;//, ct;

    int n_col = coded_packets.size();
    cout << "The total number of transmitted packets is: " << n_col << endl;

    for(int id_transmission=0; id_transmission < n_col; id_transmission++)
    {
        /***********/
        vector<char> tx_data;
        id_large_packet = id_transmission;
        cc = 0;
        /* ---------------------------------------------------------- */

        //WRITE THE SMALL PACKET WITH THE HEADER

        //write id packet for the header (is always 0)
        conv_short_int_to_char(id_header, buff_short);
        for(int k=0; k<2; k++)
        {
            tx_data.push_back(buff_short[k]);
            cc++;
        }
        if(DEBUG && id_transmission<nbtest)
        {
            unsigned short int y;
            conv_char_to_short_int(buff_short, y);
            cout << endl << " ID for the header (" << id_transmission << ") = " << y << endl;
        }

        //write header_len
        field_len = hX[id_transmission].id_utenti.size();
        conv_int_to_byte(field_len, byte);
        tx_data.push_back(byte);
        cc++;

        // Header length=(1-user,1-files,1-chuncks,4-sizePack)*field_len + 1-Strg,1-Weak,1-requestID,1-hxLength
        // New Header length = (1-user,1-file,1-NbWeakChunksPerPacket)*field_len + sum_{1}^{K}c_k
        //                   + 1-Strg/Weak,1-requestID,1-hxLength; 1-fieldLength
        header_len = (3 * field_len) + 2*hX[id_transmission].id_chunks.size() + 4;
        conv_int_to_byte(header_len, byte);
        tx_data.push_back(byte);
        cc++;
        if(DEBUG && id_transmission < nbtest)
        {
            unsigned int x;
            conv_byte_to_int(byte, x);
            cout  << " header length (" << id_transmission << ") = " << x << endl;
            cout << field_len << ", " << header_len << endl;
        }

        /*if(header_len > spack_len)
        {
            cout << endl << "ERROR: header_len > small_packet_len " << endl;
            exit(0);
        }*/

        //write id of request
        conv_int_to_byte(id_demand, byte);
        tx_data.push_back(byte);
        cc++;
        if(DEBUG && id_transmission<nbtest)
        {
            unsigned int x;
            conv_byte_to_int(byte, x);
            cout << endl << " id_demand for the header (" << id_transmission << ") = " << x;
        }


        //write header.id_utenti
        for(unsigned int j=0; j<field_len; j++)
        {
            conv_int_to_byte(hX[id_transmission].id_utenti[j], byte);
            tx_data.push_back(byte);
            cc++;

            if(DEBUG && id_transmission<nbtest)
            {
                unsigned int x;
                conv_byte_to_int(byte, x);
                cout << endl << "id_utenti for the header (" << id_transmission << ") = " << x;
            }
        }


        //write header.id_files
        for(unsigned int j=0; j<field_len; j++)
        {
            conv_int_to_byte(hX[id_transmission].id_files[j], byte);
            tx_data.push_back(byte);
            cc++;

            if(DEBUG && id_transmission<nbtest)
            {
                unsigned int x;
                conv_byte_to_int(byte, x);
                cout << endl << " id_files for the header (" << id_transmission << ") = " << x;
            }
        }


        //write header.nb_chunks
        for(unsigned int j=0; j<field_len; j++)
        {
            conv_int_to_byte(hX[id_transmission].nb_chunks[j], byte);
            tx_data.push_back(byte);
            cc++;

            if(DEBUG && id_transmission<nbtest)
            {
                unsigned int x;
                conv_byte_to_int(byte, x);
                cout << endl << " nb_chunks for the header (" << id_transmission << ") = " << x;
            }
        }


        //write header id chunks of each packet
        payload_len = hX[id_transmission].size_package[0];
        //cc=0;
        int TotalChunks = hX[id_transmission].id_chunks.size();
        unsigned short int id_chunks;
        for(int l=0; l<TotalChunks; l++){
            id_chunks = hX[id_transmission].id_chunks[l];
            conv_short_int_to_char(id_chunks , buff_short);
            for(int k=0; k<2; k++) {
                tx_data.push_back(buff_short[k]);
                cc++;
            }
            if(DEBUG && id_transmission<nbtest)
            {
                unsigned short int y;
                conv_char_to_short_int(buff_short, y);
                cout << endl << "ChunksID (" << id_transmission << "," << l << ") = " << y;
            }
        }


        //write the two byte that mentions if the packet contains weak and/or strong
        if (hX[id_transmission].strong==true && hX[id_transmission].weak==true){
            byte = 0x00;
            //buff_short[0] = 0x01;
            //buff_short[1] = 0x01;
        } else if (hX[id_transmission].strong==true && hX[id_transmission].weak==false)
        {
            byte = 0x11;
            //buff_short[0] = 0x01;
            //buff_short[1] = 0x00;
        } else if (hX[id_transmission].strong==false && hX[id_transmission].weak==true){
            byte = 0xFF;
            //buff_short[0] = 0x00;
            //buff_short[1] = 0x01;
        }
        /*for(int k=0; k<2; k++)
            tx_data.push_back(buff_short[k]);*/
        tx_data.push_back(byte);
        cc++;


        //store header pack size: +2 is for the header ID
        spack_size.push_back(header_len+2);

        if(DEBUG && id_transmission<nbtest)
        {
            if(byte==0x00) //buff_short[0] == 0x01 && buff_short[1] == 0x01
                cout << "\n Strong and Weak users" << endl;
            else if (byte == 0x11)//buff_short[0] == 0x01 && buff_short[1] == 0x00
                cout << "\n Strong users only" << endl;
            else if (byte == 0xFF)//buff_short[0] == 0x00 && buff_short[1] == 0x01
                cout << "\n Weak users only" << endl;

            cout << "Header len with ID  (" << id_transmission << ") = " << header_len+2 << endl;
            cout << cc << endl;
        }


        /* -------------------------------- Write Payload as small packets -----------------------------*/
        //compute total number of small packet
        payload_len = coded_packets.at(id_transmission).size();

        int nb_spack = payload_len / spack_len;
        //compute last small packet length
        int last_spack_len = payload_len % spack_len;

        //Important Verifications
        // check if number of small packet is minus then 65535
        if (nb_spack > 65535)
        {
            cout << endl << "ERROR: number_small_packet > 65535 " << endl;
            exit(0);
        }else if(nb_spack == 65535 && last_spack_len != 0)
        {
            cout << endl << "ERROR: number_small_packet = 65535  AND  last_small_packet_len != 0" << endl;
            exit(0);
        }

        unsigned int d_pos = 0;
        //cout << "Nb of small packets" << nb_spack << endl;
        /*
        THE SMALL PACKET STRUCTURE IS THE FOLLOWING:
            2 BYTES:         ID SMALL PACKET
            spack_len BYTES: THE PAYLOAD
        THE HEADER STRUCTURE IS THE FOLLOWING:
            2 BYTES: ID HEADER
            1 BYTE:  HEADER LENGTH
            1 BYTE:  REQUEST ID OR NUMBER
            2 BYTE:  INDICATE IF THERE EXISTS: ONE WEAK, ONE WEAK ONE STRONG, TWO STRONG
            NB_COMBINED_CHUNCKS*3 BYTES: THE CONTENT OF THE HEADER
            SUM_{k=1}^NB_COMBINED_CHUNCKS(C_k)*2 BYTES : THE IDs OF ALL CHUNKS
        */
        for(unsigned short int id_spack = 1; id_spack <= nb_spack; id_spack++)
        {
            //store small pack payolad size
            spack_size.push_back(spack_len+2);
            //cout << spack_size.at(spack_size.size()-1) << ", ";

            //write id of small packet on the top
            conv_short_int_to_char(id_spack, buff_short);
            for(int k=0; k<2; k++)
                tx_data.push_back(buff_short[k]);

            if(DEBUG && id_transmission<nbtest)
            {
                unsigned short int y;
                conv_char_to_short_int(buff_short, y);
                cout << endl << " ID Small Packet for the small packet (" << id_spack << ") = " << y;
            }

            /***********Write the Payload in the i-th Small Packet********/
            for(int k=0; k<spack_len; k++)
            {
                tx_data.push_back(coded_packets[id_transmission][d_pos]);
                d_pos++;
            }
        }

        if(last_spack_len > 0)
        {
            //store last small pack payolad size:
            spack_size.push_back(last_spack_len+2);

            //write id of small packet on the top
            unsigned short int id_last_spack = nb_spack + 1;

            conv_short_int_to_char(id_last_spack, buff_short);
            for(int k=0; k<2; k++)
            {
                tx_data.push_back(buff_short[k]);
            }
            if(DEBUG && id_transmission<nbtest)
            {
                unsigned short int y;
                conv_char_to_short_int(buff_short, y);
                cout << endl << " ID Last Small Packet " << y;
            }

            //write the rest of small packet
            for(int k=0; k<last_spack_len; k++)
            {
                tx_data.push_back(coded_packets[id_transmission][d_pos]);
                d_pos++;
            }
        }

        tx.push_back(tx_data);
        /*if(id_transmission==0){
            for(int j=0;j<tx_data.size(); j++)
                cout << (unsigned int) tx_data[j] << " ";
        }*/

    }/* end (for(int id_transmission=0; id_transmission < n_col; id_transmission++)) */

    //cout << endl << " Size of Transmission = " << tx_data.size() << endl << endl;
}

void TX_simul(vector<header_polar> &hX, vector< vector<char> > coded_packets, vector<vector<char> > &tx,
				vector<int> &all_head_len)
{
    unsigned short int id_header = 0, id_demand;
    unsigned int field_len;
    unsigned int header_len;
    unsigned int payload_len;
    unsigned int id_large_packet;

    char byte;
    char buff_short[2];

    bool DEBUG = false;
    int nbtest = 5;
    int cc;

    int n_col = coded_packets.size();
    cout << "The total number of transmitted packets is: " << n_col << endl;

    for(int id_transmission=0; id_transmission < n_col; id_transmission++)
    {
        /***********/
        vector<char> tx_data;
        id_large_packet = id_transmission;
        cc = 0;
        /* ---------------------------------------------------------- */

        //WRITE THE SMALL PACKET WITH THE HEADER
        //write id packet for the header (is always 0)
        conv_short_int_to_char(id_header, buff_short);
        for(int k=0; k<2; k++)
        {
            tx_data.push_back(buff_short[k]);
            cc++;
        }
        if(DEBUG && id_transmission<nbtest)
        {
            unsigned short int y;
            conv_char_to_short_int(buff_short, y);
            cout << endl << " ID for the header (" << id_transmission << ") = " << y << endl;
        }

        //write header_len
        field_len = hX[id_transmission].id_utenti.size();
        conv_int_to_byte(field_len, byte);
        tx_data.push_back(byte);
        cc++;

        // Header length=(1-user,1-files,1-chuncks,4-sizePack)*field_len + 1-Strg,1-Weak,1-requestID,1-hxLength
        // New Header length = (1-user,1-file,1-NbWeakChunksPerPacket)*field_len + sum_{1}^{K}c_k
        //                   + 1-Strg/Weak,1-requestID,1-hxLength; 1-fieldLength
        header_len = (3 * field_len) + 2*hX[id_transmission].id_chunks.size() + 4;
        conv_int_to_byte(header_len, byte);
        tx_data.push_back(byte);
        cc++;
        if(DEBUG && id_transmission < nbtest)
        {
            unsigned int x;
            conv_byte_to_int(byte, x);
            cout  << " header length (" << id_transmission << ") = " << x << endl;
            cout << field_len << ", " << header_len << endl;
        }

        //write id of request
        conv_int_to_byte(id_demand, byte);
        tx_data.push_back(byte);
        cc++;
        if(DEBUG && id_transmission<nbtest)
        {
            unsigned int x;
            conv_byte_to_int(byte, x);
            cout << endl << " id_demand for the header (" << id_transmission << ") = " << x;
        }

        //write header.id_utenti
        for(unsigned int j=0; j<field_len; j++)
        {
            conv_int_to_byte(hX[id_transmission].id_utenti[j], byte);
            tx_data.push_back(byte);
            cc++;

            if(DEBUG && id_transmission<nbtest)
            {
                unsigned int x;
                conv_byte_to_int(byte, x);
                cout << endl << "id_utenti for the header (" << id_transmission << ") = " << x;
            }
        }

        //write header.id_files
        for(unsigned int j=0; j<field_len; j++)
        {
            conv_int_to_byte(hX[id_transmission].id_files[j], byte);
            tx_data.push_back(byte);
            cc++;

            if(DEBUG && id_transmission<nbtest)
            {
                unsigned int x;
                conv_byte_to_int(byte, x);
                cout << endl << " id_files for the header (" << id_transmission << ") = " << x;
            }
        }

        //write header.nb_chunks
        for(unsigned int j=0; j<field_len; j++)
        {
            conv_int_to_byte(hX[id_transmission].nb_chunks[j], byte);
            tx_data.push_back(byte);
            cc++;

            if(DEBUG && id_transmission<nbtest)
            {
                unsigned int x;
                conv_byte_to_int(byte, x);
                cout << endl << " nb_chunks for the header (" << id_transmission << ") = " << x;
            }
        }

        //write header id chunks of each packet
        payload_len = hX[id_transmission].size_package[0];
        //cc=0;
        int TotalChunks = hX[id_transmission].id_chunks.size();
        unsigned short int id_chunks;
        for(int l=0; l<TotalChunks; l++){
            id_chunks = hX[id_transmission].id_chunks[l];
            conv_short_int_to_char(id_chunks , buff_short);
            for(int k=0; k<2; k++) {
                tx_data.push_back(buff_short[k]);
                cc++;
            }
            if(DEBUG && id_transmission<nbtest)
            {
                unsigned short int y;
                conv_char_to_short_int(buff_short, y);
                cout << endl << "ChunksID (" << id_transmission << "," << l << ") = " << y;
            }
        }

        //write the two byte that mentions if the packet contains weak and/or strong
        if (hX[id_transmission].strong==true && hX[id_transmission].weak==true){
            byte = 0x00;
        } else if (hX[id_transmission].strong==true && hX[id_transmission].weak==false)
        {
            byte = 0x11;
        } else if (hX[id_transmission].strong==false && hX[id_transmission].weak==true){
            byte = 0xFF;
        }
        tx_data.push_back(byte);
        cc++;

        if(DEBUG && id_transmission<nbtest)
        {
            if(byte==0x00) //buff_short[0] == 0x01 && buff_short[1] == 0x01
                cout << "\n Strong and Weak users" << endl;
            else if (byte == 0x11)//buff_short[0] == 0x01 && buff_short[1] == 0x00
                cout << "\n Strong users only" << endl;
            else if (byte == 0xFF)//buff_short[0] == 0x00 && buff_short[1] == 0x01
                cout << "\n Weak users only" << endl;

            cout << "Header len with ID  (" << id_transmission << ") = " << header_len+2 << endl;
            cout << cc << endl;
        }

        /* -------------------------------- Write Payload as small packets -----------------------------*/
        //compute total number of small packet
        payload_len = coded_packets.at(id_transmission).size();
        /*
        THE HEADER STRUCTURE IS THE FOLLOWING:
            2 BYTES: ID HEADER
            1 BYTE:  HEADER LENGTH
            1 BYTE:  REQUEST ID OR NUMBER
            2 BYTE:  INDICATE IF THERE EXISTS: ONE WEAK, ONE WEAK ONE STRONG, TWO STRONG
            NB_COMBINED_CHUNCKS*3 BYTES: THE CONTENT OF THE HEADER
            SUM_{k=1}^NB_COMBINED_CHUNCKS(C_k)*2 BYTES : THE IDs OF ALL CHUNKS
        */
 		all_head_len.push_back(cc);
        /***********Write the Payload in the i-th Small Packet********/
        for(int k=0; k<payload_len; k++)
            tx_data.push_back(coded_packets[id_transmission][k]);

        tx.push_back(tx_data);
        /*if(id_transmission==0){
        	unsigned int x;
            for(int j=0;j< payload_len; j++){//tx_data.size()
            	conv_byte_to_int(coded_packets[id_transmission][j], x);
            	cout << x << " ";
            }
        }*/

    }/* end (for(int id_transmission=0; id_transmission < n_col; id_transmission++)) */

    //cout << endl << " Size of Transmission = " << tx_data.size() << endl << endl;
}


void RX_simul(vector<gr_complex> v, unsigned int &packet_remain, vector<int> code_rate, int id_user, PC PC_w, PC PC_s,
      int b_chunks, int m_files, int size_chunk, int N, double SNR, bool isStr,int num_pack, vector<int> data_bits, double &err)
{
    unsigned int id_header, begin_pack;
    gr_complex buff_qpsk[8];
    vector<gr_complex> coded_packet;
    header_polar d_header;
    bool DEBUG=false;

    if(num_pack==0){
        string pathFolder = "../trasmissioni/User_" + my_to_string(id_user);
        mkdir(pathFolder.c_str(), S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);

        string cacheFolder = "../cache/UserCache/user_" + my_to_string(id_user);
        mkdir(cacheFolder.c_str(), S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);
    }

    for(int j=0; j< 8; j++){
        buff_qpsk[j] = v[j];
        //cout << buff_qpsk[j] << ", ";
    }
    conv_8QPSKsymb_to_int(buff_qpsk, id_header);

    if(id_header == 0){ //The header is received
        //cout << "-------Decoding the header -----" << endl;
        begin_pack = decode_header(v, d_header);
    } else{
        cout << "Heaader should be received first." << endl;
        exit(0);
    }

    for(unsigned int k = begin_pack; k < v.size(); k++)
        coded_packet.push_back(v[k]);

    //Proceed to decoding of the whole packet
    Process_Data(coded_packet,id_user,packet_remain,m_files,b_chunks,isStr,N,SNR,PC_w,PC_s,d_header,size_chunk,code_rate[id_user],data_bits,err);
    //Restore the header
    d_header.id_chunks.clear();
    d_header.id_files.clear();
    d_header.id_utenti.clear();
    d_header.size_package.clear();
    coded_packet.clear();

}

vector<vector<gr_complex> > BitsToQPSKSymb(vector<vector<int> > data_bits){

	//QPSK constellation map
    gr_complex constellation[4];
    constellation[0] = gr_complex(-SQRT_TWO, -SQRT_TWO);
    constellation[1] = gr_complex(SQRT_TWO, -SQRT_TWO);
    constellation[2] = gr_complex(-SQRT_TWO, SQRT_TWO);
    constellation[3] = gr_complex(SQRT_TWO, SQRT_TWO);

    int nbPackets = data_bits.size();
    vector<vector<gr_complex> > data_qpskSymb;
    for(int l=0; l<nbPackets; l++){
        int txBitsSize  = data_bits[l].size();
        int txSymbSize  = txBitsSize/2;
        vector<int> pack_data_bits(txSymbSize, 0);
        data_qpskSymb.push_back(vector<gr_complex> (txSymbSize, (0,0)));

        
        //Pack each two bits into one symb in [0, 1, 2, 3]
        int j=0;
        for(int i=0; i<txSymbSize; i++){
            for (int k = 0; k < 2; k++){
                pack_data_bits[i] += data_bits[l][j++]*pow(2,k);
            }
            //Transform packed bits to symbol
            switch(pack_data_bits[i]){
                case 0:
                    data_qpskSymb[l][i] = constellation[0];
                    break;
                case 1:
                    data_qpskSymb[l][i] = constellation[1];
                    break;
                case 2:
                    data_qpskSymb[l][i] = constellation[2];
                    break;
                case 3:
                    data_qpskSymb[l][i] = constellation[3];
                    break;
            }

        }
        pack_data_bits.clear();
    }

    return data_qpskSymb;

}

int decode_header(vector<gr_complex> in, header_polar &d_header){
        unsigned int i = 8; // 8 BECAUSE QPSK MODULATED- first 2 bytes for header id -
        unsigned int id_demand;
        unsigned int header_len = 0, field_len=0, payload_len;
        char byte;
        char buff[4];
        gr_complex buff_8qpsk[8], buff_4qpsk[4];
        int nb_chunks = 0;
        bool DEBUG = false;


        //read field length
        for(int k=0; k<4; k++)
            buff_4qpsk[k] = in[i++];
        conv_4QPSKsymb_to_int(buff_4qpsk, field_len);

        if(DEBUG)
            cout << "\nField Length = " << field_len << endl;

        //read header length
        for(int k=0; k<4; k++)
            buff_4qpsk[k] = in[i++];
        conv_4QPSKsymb_to_int(buff_4qpsk, header_len);

        if(DEBUG)
            cout << "Header Length = " << header_len << endl;

        /*****STUPID TEST ONLY****/
        /*for(int k=0; k < (header_len+2)*4; k++)
          cout << in[k] << ", ";
        cout << endl;*/

        //read id_demands
        for(int k=0; k<4; k++)
            buff_4qpsk[k] = in[i++];
        conv_4QPSKsymb_to_int(buff_4qpsk, id_demand);

        if(DEBUG)
            cout << "ID Demand = " << id_demand << endl;

        //read header.id_utenti
        for(unsigned int j=0; j<field_len; j++)
        {
            d_header.id_utenti.push_back(0);
            /*byte=in[i++;];
            conv_byte_to_int(byte, d_header.id_utenti[j]);*/
            for(int k=0; k<4; k++)
                buff_4qpsk[k] = in[i++];
            conv_4QPSKsymb_to_int(buff_4qpsk, d_header.id_utenti[j]);

            if(DEBUG)
                cout << "ID Utenti = " << d_header.id_utenti[j] << endl;
        }

        //read header.id_files
        for(unsigned int j=0; j<field_len; j++)
        {
            d_header.id_files.push_back(0);
            /*byte=in[i++];
            conv_byte_to_int(byte, d_header.id_files[j]);*/
            for(int k=0; k<4; k++)
                buff_4qpsk[k] = in[i++];
            conv_4QPSKsymb_to_int(buff_4qpsk, d_header.id_files[j]);

            if(DEBUG)
                cout << "ID Files = " << d_header.id_files[j] << endl;
        }


        //read header.nb_chunks
        for(unsigned int j=0; j<field_len; j++)
        {
            d_header.nb_chunks.push_back(0);
            for(int k=0; k<4; k++)
                buff_4qpsk[k] = in[i++];
            conv_4QPSKsymb_to_int(buff_4qpsk, d_header.nb_chunks[j]);
            nb_chunks += d_header.nb_chunks[j];

            if(DEBUG)
                cout  << "Nb Chunks = " << d_header.nb_chunks[j] << endl;
        }
        if(DEBUG)
            cout  << "Total Nb Chunks = " << nb_chunks << endl;


        //read header.id chunks
        for(unsigned int j=0; j<nb_chunks; j++)
        {
            d_header.id_chunks.push_back(0);

            for(int k=0; k< 8; k++)
                buff_8qpsk[k] = in[i++];
            conv_8QPSKsymb_to_int(buff_8qpsk, d_header.id_chunks[j]);

            if(DEBUG)
                cout << "ID Chunks[" << j << "] = " << d_header.id_chunks[j] << endl;
        }

        //Read if there strong and weak, only weak, or two strog packets
        for(int k=0; k<4; k++){
            buff_4qpsk[k] = in[i++];
        }
        unsigned int a=0;
        conv_4QPSKsymb_to_char(buff_4qpsk, byte);

        if(DEBUG){
            conv_byte_to_int(byte, a);
            cout << "\nStrong user? " << a << endl;
        }

        if(byte == 0x00){
            d_header.strong = true;
            d_header.weak = true;
        } else if (byte == 0x11){
            d_header.strong = true;
            d_header.weak = false;
        } else{
            d_header.strong = false;
            d_header.weak = true;
        }

        //compute total number of small packet
        //payload_len = N/8;

        /******************************************************************/
        return i;
}

void Process_Data(vector<gr_complex> in, int id_user, unsigned int &packet_remain, int Nbfiles, int NbChunks,
    bool isStr, int N, double d_SNR, PC PC_w, PC PC_s, header_polar d_header, int size_chunk, int c_rate, vector<int> data_bits, double &err){

    unsigned int field_len = 0;
    unsigned int SymbPayload_len;//BeginData, payload_len,
    int id_strFile, id_strChunk,size_strPck;
    //Size of the chunk is 1/6 the size of the codeword
	//int size_chunk = N/(8*6);
	int K_w = c_rate*size_chunk*8; //REMOVED FROM THE PARAMETERS
	int K_s = 4*size_chunk*8;
	//header_polar d_header;
    header_transmission d_header_w;
    char buff[4];
    //gr_complex buff_4qpsk[4];
    vector<char> coded_data;
    bool DEBUG = false;
    //bool quit = false;



    /********************************************************/
    SymbPayload_len = in.size();
    gr_complex coded_symb[SymbPayload_len];

    for (unsigned int j = 0; j < SymbPayload_len; ++j)
    {
        coded_symb[j] = in[j];

    }

    //if(DEBUG)
    {
        //Convert the data in the received packet as symbols to bits
        int s [N/2], bits[N];
        int sum=0;
        //cout << "bits stream: ";
        for(int j=0; j< (N/2); j++){
            s[j] = 2*(imag(coded_symb[j])>0) + (real(coded_symb[j])>0);
            //if(j%4==0)
            	//cout << "  ";
            switch(s[j]){
                case 0:
                    bits[2*j] = 0;
                    bits[2*j+1] = 0;
                    break;
                case 1:
                    bits[2*j] = 1;
                    bits[2*j+1] = 0;
                    break;
                case 2:
                    bits[2*j] = 0;
                    bits[2*j+1] = 1;
                    break;
                case 3:
                    bits[2*j] = 1;
                    bits[2*j+1] = 1;
                    break;
            }
            //cout << bits[2*j] << bits[2*j+1];

        }
        //cout << endl;
        for(int j=0; j<N; j++){
            sum = sum+bits[j];
        }
        cout << "[" << sum << ", " ;
        for(int j=0;j<d_header.id_utenti.size(); j++)
        	cout << d_header.id_utenti[j] << ",";
        cout << "]";//"\nsum bits coded: " << endl << endl;
    }

    //Read the header to firstly find out the included packets, strong and weak
    //and take the three cases into account: weak - strong, weak, strong - strong
    double llr_w[N];
    gr_complex recSymbol_w[N];
    int recMessage_w[N], recCodeword_w[N];
    //Set Polar Decoding environment


    //Check if the user is concerned by this packet, if not do nothing
    int index = index_find(d_header.id_utenti, id_user);
    if(DEBUG)
        cout << endl << "user index in header = " << index << endl;

    //cout << ", Strong and/or Weak? " << d_header.strong << ", " << d_header.weak << endl;
    //index = -1;
    if(index != -1 ) {

        int id_file = d_header.id_files.at(index);
        int id_demand = 0;

        if(DEBUG){
            if(d_header.id_utenti.size()>1){
                for (unsigned int ll = 0; ll < d_header.id_utenti.size(); ll++)
                    cout << "[" << ll << "," << d_header.id_utenti.at(ll) << "," << d_header.id_files.at(ll) << "]";
                cout << endl;
                for(int kk=0; kk < d_header.id_chunks.size(); kk++)
                    cout << d_header.id_chunks[kk] << ", ";
                cout << endl;
            }
        }

        //This is the directory of the cached files
        string pathFolder = "../cache/UserCache/user_" + to_string(id_user); //"/CachingFile/cache/UserCache/user_"

        if(d_header.strong == true && d_header.weak == true)
        {   //If this user is not the strong user:
            if(!isStr){
                if(DEBUG)
                    cout << "START ---- User is weak in case SW" << endl;
                //1. Extract the strong packet from the cache
                //2. Store in Frozen Bits
                //3. Proceed to polar decoding

                //1.a Read the id of file and chunk of strong packet; last index in Rx header
                id_strFile  = d_header.id_files.at(d_header.id_files.size()-1);
                id_strChunk = d_header.id_chunks.at(d_header.id_chunks.size()-1);
                //size_strPck = d_header.size_package.at(field_len-1);

                //FOR TEST ONLY
                if(DEBUG){
                    cout << id_strFile << ", " << id_strChunk;
                    cout << ", " << size_chunk << endl;
                }

                //The file in the cache
                string pathFileCache = pathFolder + "/" + to_string(id_strFile) + "_" + to_string(id_strChunk) + ".cache";

                char *cache_str = new char[size_chunk];
                //Read the packet from the cache and store in vector "cache"
                ifstream inFilePackage (pathFileCache, ifstream::binary);
                if (inFilePackage){
                    inFilePackage.read(cache_str, size_chunk);
                    inFilePackage.close();
                }else{
                    cout << endl << "Error reading file cache: " << pathFileCache << endl;
                    // exit(0);
                }

                /**********FOR TEST ONLY - PRINT DATA IN CACHE****************/
                /*cout << "Data in weak user's cache are: "<< endl;
                for(int j=0; j<20; j++)
                    cout << cache_str[j] << ", ";
                cout << endl;
                /**********************************************/

				//1.b If the code rate is less than 3, read the xored chunks with higher code rate and add to the frozen_bits
				//Reconstruct the chunks matrix [users  x chunk ID]
				vector<char> cache_w;
                cache_w = cache_weak(d_header, c_rate, size_chunk, id_user, pathFolder);

                //2. Convert the data in cache to bits and store in Frozen bits
                int lenZeros = N-K_s;
				int len_cache = (4-c_rate)*size_chunk;
                int len_cache_w = (3-c_rate)*size_chunk;
				int frozen_bits [len_cache*8 + lenZeros];//8*size_chunk+lenZeros
				vector<unsigned int> bb (8,0); //bits vector of each converted char
                for(int j=0; j<len_cache_w; j++){
                    bb = conv_char_to_bitsInt(cache_w[j]);
                    for(int k=0; k<8; k++)
                        frozen_bits[j*8+k] = bb[k];
                }
				for(int j=0; j<size_chunk; j++){
                    bb = conv_char_to_bitsInt(cache_str[j]);
                    for(int k=0; k<8; k++)
                        frozen_bits[(len_cache_w + j)*8+k] = bb[k];
                }
                //Add the zeros to the frozen bits
                for(int j=0; j<lenZeros; j++){
                    frozen_bits[len_cache*8+j] = 0;
                }

                //3. Proceed to Polar Decoding
                double variance = pow(10,-(d_SNR/10));
                //double sqrtVariance = sqrt(variance);
                PC_w.computeLLR_qpsk(llr_w,coded_symb,variance);
                PC_w.SC(recMessage_w, recCodeword_w, llr_w, frozen_bits);
                recMessage_w[0] = (recMessage_w[0])?0:1;
                //if(DEBUG)
                {
                    int sum=0, ct_err = 0;
                    for (int i = 0; i < N; i++){
                        //cout << recMessage_w[i];
                        sum+=recMessage_w[i];
                        if(data_bits[i] != recMessage_w[i]){
                        	ct_err ++;
                        }
                    }
                    //cout << sum << ", ";
                    cout << ct_err << ", ";
                    err = (double) ct_err/N;
                }

                //Convert Bits to CHAR
                int abits[8];
                int packetSizeByte = K_w/8;//N/8

                /*------------- COMPARE THE BITS AT THIS LEVEL BETWEEN 0 AND K_w ------------*/

                vector<char> coded_packet;
                vector<char> conv(packetSizeByte,0);
                for (int j = 0; j < (packetSizeByte); j++){
                    for (int k = 0; k < 8; k++){
                        abits[k] = recMessage_w[j*8+k];
                        conv[j] += abits[k]*pow(2,k);
                    }
                    //conv_err = conv_err + (conv[j]-((int) PC_data[i][j]));
                    coded_packet.push_back(conv[j]);
                }

                if(DEBUG && packet_remain > 100){
                    cout << "Weak in Weak and Strong case: ";
                    for (int j = 0; j < coded_packet.size(); ++j)
                        cout << (int)coded_packet[j] << ", ";
                    cout << endl;
                }
                //Now we know that the first K_w bits are for the weak
                //and the next K_s are for the strong and the last N-K_s are 0
                //Since this case is the weak user case, then we do classic cache decoding

                //Transform the header to a weak header
                toHeaderWeak(d_header, d_header_w);
                //Weak decoding
                decodingData(d_header_w,coded_packet,Nbfiles,NbChunks,id_user,id_demand,id_file,c_rate,&packet_remain);
                //The number of requested packets not in the local cache
                if(DEBUG)
                    cout << "Nb Remaining packets: " << packet_remain << endl;

            }else{
                if(DEBUG)
                    cout << "START ---- User is strong in case SW" << endl;
                //Decode the strong user
                //Proceed to Polar Decoding
                //string pathFileDelivery = pathFolder + "/" + to_string(id_file) + "_" + to_string(id_chunck) + ".cache";
				//decodeDataStrong(N,K_w,K_s,d_SNR,coded_symb,pathFileDelivery,PC_w,PC_s,false,packet_remain,d_header);
				decodeDataStrong(N,d_SNR,coded_symb,pathFolder,PC_w,PC_s,false,packet_remain,d_header,id_user,size_chunk,data_bits,err);

            }
        }
        else if(d_header.strong == false && d_header.weak == true){

            if(DEBUG)
                cout << "START ---- User is weak in case W" << endl;

            //1. Get the cached chunks of the other weaks - this is the case when the
            //code rate is different at different mutiplexed users in one xored packet
            vector<char> cache_w;
            cache_w = cache_weak(d_header, c_rate, size_chunk, id_user, pathFolder);

            int lenZeros = N-(size_chunk*8*3);
            int len_cache_w = (3-c_rate)*size_chunk;
            int frozen_bits [len_cache_w*8 + lenZeros];//8*size_chunk+lenZeros
            vector<unsigned int> bb (8,0); //bits vector of each converted char
            for(int j=0; j<len_cache_w; j++){
                bb = conv_char_to_bitsInt(cache_w[j]);
                for(int k=0; k<8; k++)
                    frozen_bits[j*8+k] = bb[k];
            }
            //Add the zeros to the frozen bits
            for(int j=0; j<lenZeros; j++){
                frozen_bits[len_cache_w*8+j] = 0;
            }

            if(DEBUG){
            	int sum_frozen =0;
            	for (int j = 0; j < len_cache_w*8; ++j)
            		sum_frozen += frozen_bits[j];
            	cout << sum_frozen << ", ";
            }
            //Proceed to Polar Decoding
            double variance = pow(10,-(d_SNR/10));
            double sqrtVariance = sqrt(variance);

            if(DEBUG)
                cout << "START THE POLAR DECODING" << endl;
            
            //PC_w.computeLLR_qpsk(llr_w,coded_symb,variance);
            //PC_w.noise_c(coded_symb, recSymbol_w, 0*sqrtVariance);
            /*for (int i = 0; i < 100; ++i)
            	cout << coded_symb[i] << "-" << recSymbol_w[i] << endl;*/

            PC_w.computeLLR_qpsk(llr_w,coded_symb,variance);
            //PC_w.noise(RX_test, recSymbol_w, sqrtVariance);
            //PC_w.computeLLR(llr_w,recSymbol_w,variance);
            PC_w.SC(recMessage_w, recCodeword_w, llr_w, frozen_bits);
            recMessage_w[0] = (recMessage_w[0])?0:1;

            //if(DEBUG)
            {
                int sum=0, sum_bit=0, ct_err = 0;
                for (int i = 0; i < N; i++){
                    //cout << recMessage_w[i];
                    sum+=recMessage_w[i];
                    sum_bit+=data_bits[i];
                    if(data_bits[i] != recMessage_w[i]){
                        ct_err ++;
                    }
                }
                if(DEBUG){
                	cout << "[" << sum << ", " << sum_bit << "], ";
                	cout << ct_err << "_w, " << endl;
                }
                err = (double) ct_err/N;
            }

            if(DEBUG)
                cout << "POLAR DECODING SUCCESSFULLY COMPLETE" << endl;
            //Now we know that the first K_w bits are for the weak
            //and the next K_s are for the strong and the last N-K_s are 0
            //Since this case is the weak user case, then we do classic cache decoding
            //Convert bits to char
            int abits[8];
            //int conv_err=0;

            vector<char> coded_packet;
            int packetSizeByte = K_w/8;
            vector<char> conv(packetSizeByte,0);
            for (int j = 0; j < (packetSizeByte); j++){
                for (int k = 0; k < 8; k++){
                    abits[k] = recMessage_w[j*8+k];
                    conv[j] += abits[k]*pow(2,k);
                }
                //conv_err = conv_err + (conv[j]-((int) PC_data[i][j]));
                coded_packet.push_back(conv[j]);
            }

            if(DEBUG && packet_remain > 160){
                cout << "Weak only: " ;
                for (int j = 0; j < coded_packet.size(); ++j)
                    cout << (int)coded_packet[j] << ", ";
                cout << endl;
            }

            //Transform to a weak header
            toHeaderWeak(d_header, d_header_w);

            //Weak decoding
            decodingData(d_header_w,coded_packet,Nbfiles,NbChunks,id_user,id_demand,id_file,c_rate, &packet_remain);
            //The number of requested packets not in the local cache
            if(DEBUG)
                cout << "Nb Remaining packets: " << packet_remain << endl;

       }
        else if(d_header.strong == true && d_header.weak == false){

            if(DEBUG)
                cout << "START ---- User is strong in case S" << endl;

            //Here is the case where two strong packets are in one packet

            //In this if clause, the requested packet is the second
            //which means we can directly apply the polar decoding
            /*gr_complex coded_symb_m[SymbPayload_len];

            int K=K_s/2;
            for(int j=0; j<K; j++)
                coded_symb_m[K + j] = coded_symb[j];

            for(int j=0; j<K; j++)
                coded_symb_m[j] = coded_symb[K + j];*/

            /*if(d_header.id_utenti.size() > 1) // ==2
            {
                if(d_header.id_utenti[1] == id_user){
                    id_chunck = d_header.id_chunks.at(1);
                    string pathFileDelivery = pathFolder + "/" + to_string(id_file) + "_" + to_string(id_chunck) + ".cache";
                    decodeDataStrong(N,K_w,K_s,d_SNR,coded_symb,pathFileDelivery,PC_w,PC_s,false,packet_remain);

                }
                //In this else clause, the requested packet is the first
                //which meanse the polar decoding cannot applied directly
                //we should first swap the two strong packets and then
                //apply the polar decoding function that is developed taking
                //into account that the first packet iswea, the second is strong
                //and then frozen bits zeros
                //Note that K_s =2*K_w
                if(d_header.id_utenti[0] == id_user){
                    id_chunck = d_header.id_chunks.at(0);
                    string pathFileDelivery = pathFolder + "/" + to_string(id_file) + "_" + to_string(id_chunck) + ".cache";
                    decodeDataStrong(N,K_w,K_s,d_SNR,coded_symb,pathFileDelivery,PC_w,PC_s,true,packet_remain);
                }
            }else{
                string pathFileDelivery = pathFolder + "/" + to_string(id_file) + "_" + to_string(id_chunck) + ".cache";
                decodeDataStrong(N,K_w,K_s,d_SNR,coded_symb,pathFileDelivery,PC_w,PC_s,false,packet_remain,d_header);
            }*/
			decodeDataStrong(N,d_SNR,coded_symb,pathFolder,PC_w,PC_s,true,packet_remain,d_header, id_user, size_chunk,data_bits,err);
            //For testing pourpose
            if (packet_remain < 5 ){
                string name_file;
                name_file = "../trasmissioni/User_" + to_string(id_user) + "/decoded_file_" + to_string(id_demand) + ".xml"; //"/CachingFile/trasmissioni/User_"
                ofstream outFile (name_file, ios::out | ios::binary);

                for (int k = 0; k < NbChunks; k++){
                    /********************************** READ ALL PACKAGE ********************************/
                    // Open file cache for read package
                    string pathFilePackage = pathFolder + "/" + to_string(id_file) + "_" + to_string(k) + ".cache";
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
                            // exit(0);
                        }
                        /**********************************************************************************/

                        outFile.write(buffer, size_file);

                        delete[] buffer;
                    }
                }

                outFile.close();
            }

       }
   }

}

vector<char> cache_weak(header_polar d_header, int c_rate, int size_chunk, int id_user, string pathFolder){

    vector<char> cache ((3-c_rate)*size_chunk, 0);
    vector<vector<int> > id_chunks;
    int TotalChunks = 0;

    for (unsigned int i = 0; i < d_header.id_utenti.size(); i++){
        /*All info for requested package*/
        unsigned int nb_chunks = d_header.nb_chunks.at(i);
        vector<int> vd;
        for(unsigned int j=TotalChunks; j< nb_chunks + TotalChunks; j++)
            vd.push_back(d_header.id_chunks.at(j));
        id_chunks.push_back(vd);
        TotalChunks += nb_chunks;
    }

    //cout << d_header.id_utenti.size() << ",";
    for(unsigned int i = 0; i < d_header.id_utenti.size(); i++){
        if((id_user != d_header.id_utenti.at(i)) && (id_chunks.at(i).size() > c_rate)){
            int cc=0;
            int size_cache = (id_chunks.at(i).size()-c_rate)*size_chunk;
            vector<char> cache_buff (size_cache, 0);
            for(unsigned int j = c_rate; j < id_chunks.at(i).size(); j++){
                //The file in the cache
                string pathFileCache = pathFolder + "/" + to_string(d_header.id_files.at(i)) + "_" + to_string(id_chunks[i][j]) + ".cache";

                char *cache_chunk = new char[size_chunk];
                //Read the packet from the cache and store in vector "cache"
                ifstream inFilePackage (pathFileCache, ifstream::binary);
                if (inFilePackage){
                    inFilePackage.read(cache_chunk, size_chunk);
                    inFilePackage.close();
                }else{
                    cout << endl << "Error reading file cache: " << pathFileCache << endl;
                    // exit(0);
                }
                for(int k=0; k<size_chunk; k++){
                    cache_buff[cc] = cache_chunk[k];
                    cc ++;
                }
            }
            for (int k = 0; k < size_cache; k++){
                cache[k] = cache[k] ^ cache_buff[k];
            }
        }
    }
    return cache;
}

void decodeDataStrong(int N,double d_SNR, gr_complex* coded_symb,string pathFolder,
    PC PC_w, PC PC_s, bool loc, unsigned int &packet_remain, header_polar d_header, int id_user, int size_chunk, vector<int> data_bits, double &err){

    int id_file, id_chunk;
	//int size_chunk = 42; //SHOULD BE SIZE IN CHARACTERS
    int K_w = 3*size_chunk*8; //SHOULD BE LENGTH IN BTIS
    int K_s = 4*size_chunk*8;
    int frozen_s[N-K_s];
    double llr_s[N];
    float recSymbol_s[N];
    int recMessage_s[N], recCodeword_s[N];
    double designSNRdb = 0;
    bool DEBUG = false;
    string pathFileDelivery;

    //cout << "Coding parameters: " << K_w << ", " << K_s << ", " << size_chunk << endl;

    for (int j = 0; j < N-K_s; j++)
        frozen_s[j] = 0;

    double variance = pow(10,-(d_SNR/10));
    //double sqrtVariance = sqrt(variance);


    if(DEBUG)
    {
        //Convert the data in the received packet as symbols to bits
        int s [N/2], bits[N];
        int sum=0;
        cout << "bits stream: ";
        for(int j=0; j< (N/2); j++){
            s[j] = 2*(imag(coded_symb[j])>0) + (real(coded_symb[j])>0);
            switch(s[j]){
                case 0:
                    bits[2*j] = 0;
                    bits[2*j+1] = 0;
                    break;
                case 1:
                    bits[2*j] = 1;
                    bits[2*j+1] = 0;
                    break;
                case 2:
                    bits[2*j] = 0;
                    bits[2*j+1] = 1;
                    break;
                case 3:
                    bits[2*j] = 1;
                    bits[2*j+1] = 1;
                    break;
            }
            //cout << bits[2*j] << bits[2*j+1];

        }
        for(int j=0; j<N; j++){
            sum = sum+bits[j];
        }
        cout <<  sum << ", "; //"\nsum bits coded: " <<
    }
    PC_s.computeLLR_qpsk(llr_s,coded_symb,variance);
    PC_s.SC(recMessage_s, recCodeword_s, llr_s, frozen_s);

    recMessage_s[0] = (recMessage_s[0])?0:1;

    {
        int sum=0, sum_bit=0, ct_err = 0;
        for (int i = 0; i < N; i++){
            sum+=recMessage_s[i];
            sum_bit+=data_bits[i];
            if(data_bits[i] != recMessage_s[i]){
                ct_err ++;
            }
        }
        if(DEBUG){
            cout << "[" << sum << ", " << sum_bit << "], ";
            cout << ct_err << "_w, " << endl;
        }
        err = (double) ct_err/N;
    }


    //Convert bits to char
    int abits[8];
	int packetSizeByte = K_s/8;

    vector<char> coded_packet;
    vector<char> conv(packetSizeByte,0);//
    for (int j = 0; j < packetSizeByte; j++){//
        for (int k = 0; k < 8; k++){
         abits[k] = recMessage_s[j*8+k];
         conv[j] += abits[k]*pow(2,k);
        }
        coded_packet.push_back(conv[j]);
    }

    /*if(DEBUG){
        for (int j = 0; j < 20; ++j)
            cout << (int)coded_packet[j] << ", ";
        cout << endl;
    }*/


    unsigned int lenUncPack2 = (K_s-K_w)/8;//should be equal to size_chunk
    unsigned int lenUncPack1 = K_w/8;
    /*char uncoded_packet[lenUncPack2];
    for(unsigned int j=0; j<lenUncPack2; j++)
            uncoded_packet[j] = coded_packet[lenUncPack1+j];
    */
	char uncoded_packet[size_chunk];
    if(!loc){
        int field_len = d_header.id_files.size();
        id_file  = d_header.id_files.at(field_len-1);
        id_chunk = d_header.id_chunks.at(d_header.id_chunks.size()-1);
        //char uncoded_packet[lenUncPack2];
        pathFileDelivery = pathFolder + "/" + to_string(id_file) + "_" + to_string(id_chunk) + ".cache";
        for(unsigned int j=0; j<lenUncPack2; j++)
            uncoded_packet[j] = coded_packet[lenUncPack1+j];
        ofstream outFileDelivery(pathFileDelivery, ifstream::binary);
        if (outFileDelivery.is_open()){
            outFileDelivery.write(uncoded_packet, lenUncPack2);
            outFileDelivery.close();
            packet_remain--;
            //cout << "[" << packet_remain << ", " << id_chunk << "] ";
        }
        else{
            cout << endl << "Error writing delivery package: " << pathFileDelivery << endl;
            exit(0);
        }
    }else{
		for(int j=0; j<d_header.id_utenti.size(); j++){
			if(d_header.id_utenti[j] == id_user){
				id_file = d_header.id_files[j];
				id_chunk = d_header.id_chunks[j];
				pathFileDelivery = pathFolder + "/" + to_string(id_file) + "_" + to_string(id_chunk) + ".cache";

			}
			for(unsigned int k=0; k<size_chunk; k++)
				uncoded_packet[k] = coded_packet[j*size_chunk+k];
			ofstream outFileDelivery(pathFileDelivery, ifstream::binary);
			if (outFileDelivery.is_open()){
				outFileDelivery.write(uncoded_packet, lenUncPack2);
				outFileDelivery.close();
				packet_remain--;
                //cout << packet_remain << ", ";
			}
		}
        /*char uncoded_packet[lenUncPack1];
        for(unsigned int j=0; j<lenUncPack1; j++)
            uncoded_packet[j] = coded_packet[j];
        ofstream outFileDelivery(pathFileDelivery, ifstream::binary);
        if (outFileDelivery.is_open()){
            outFileDelivery.write(uncoded_packet, lenUncPack1);
            outFileDelivery.close();
            packet_remain--;
        }
        else{
            cout << endl << "Error writing delivery package: " << pathFileDelivery << endl;
            exit(0);
        }*/
    }

    /********************************** DELIVERY PACKAGE *********************************/
    // Open file for write delivery package
    //string pathFileDelivery = pathFolder + "/" + to_string(id_file) + "_" + to_string(id_chunk) + ".cache";

    /*ofstream outFileDelivery(pathFileDelivery, ifstream::binary);
    if (outFileDelivery.is_open()){
        outFileDelivery.write(uncoded_packet, lenUncPack2);
        outFileDelivery.close();
    }
    else{
        cout << endl << "Error writing delivery package: " << pathFileDelivery << endl;
        exit(0);
    }*/

}


void toHeaderWeak(header_polar d_header, header_transmission &d_header_w)
{
    bool DEBUG = false;
    int field_len;

    if (!d_header.strong)
        field_len = d_header.id_utenti.size();
    else
        field_len = d_header.id_utenti.size()-1;

    if(DEBUG)
        cout << "\nField length: " << field_len << endl;


    for(unsigned int j=0; j<field_len; j++)
    {
        d_header_w.id_utenti.push_back(d_header.id_utenti[j]);
        if(DEBUG)
            cout << endl << "ID Utenti = " << d_header_w.id_utenti[j];
    }
    //read header.id_files
    for(unsigned int j=0; j<field_len; j++)
    {
        d_header_w.id_files.push_back(d_header.id_files[j]);
        if(DEBUG)
            cout << endl << "ID Files = " << d_header_w.id_files[j];
    }
    //read header.nb_chunks
    for(unsigned int j=0; j<field_len; j++)
    {
        d_header_w.nb_chunks.push_back( d_header.nb_chunks[j]);
        if(DEBUG)
            cout << endl << "NB Chunks = " << d_header.nb_chunks[j];
    }
    for(unsigned int j=0; j<d_header.id_chunks.size(); j++)
    {
        d_header_w.id_chunks.push_back(d_header.id_chunks[j]);
        if(DEBUG)
            cout << endl << "ID chunks = " << d_header.id_chunks[j];
    }

}


void Transmit(header_transmission* header_data, vector< vector<char> > coded_data, unsigned int id_demand,
    int n_col, unsigned int small_packet_len, vector<char> &trasmissione, vector<unsigned int> &small_pack_size)
{
    unsigned short int id_header = 0;
    unsigned int field_len;
    unsigned int header_len;
    unsigned short int id_large_packet;
    unsigned int payload_len;

    unsigned int number_small_packet;
    unsigned int last_small_packet_len;

    char byte;
    char buff_short[2];
    char buff[4];

    bool DEBUG = false;


    for(int id_transmission=0; id_transmission < n_col; id_transmission++)
    {

        /* ---------------------------------------------------------------------------------- */

        //WRITE THE SMALL PACKET WITH THE HEADER

        //write id_small_packet for the header (is always 0)
        conv_short_int_to_char(id_header, buff_short);
        for(int k=0; k<2; k++)
        {
            trasmissione.push_back(buff_short[k]);
        }
        if(DEBUG)
        {
            unsigned short int y;
            conv_char_to_short_int(buff_short, y);
            cout << endl << " ID Small Packet for the header (" << id_transmission << ") = " << y;
        }

        //write header_len
        field_len = header_data[id_transmission].id_utenti.size();
        header_len = (7 * field_len) + 2;
        conv_int_to_byte(header_len, byte);
        trasmissione.push_back(byte);
        if(DEBUG)
        {
            unsigned int x;
            conv_byte_to_int(byte, x);
            cout << endl << " header_len for the header (" << id_transmission << ") = " << x;
        }

        if(header_len > small_packet_len)
        {
            cout << endl << "ERROR: header_len > small_packet_len " << endl;
            exit(0);
        }


        //write id of request
        id_large_packet = id_transmission;
        conv_int_to_byte(id_demand, byte);
        trasmissione.push_back(byte);
        if(DEBUG)
        {
            unsigned int x;
            conv_byte_to_int(byte, x);
            cout << endl << " id_demand for the header (" << id_transmission << ") = " << x;
        }


        //write header.id_utenti
        for(unsigned int j=0; j<field_len; j++)
        {
            conv_int_to_byte(header_data[id_transmission].id_utenti[j], byte);
            trasmissione.push_back(byte);

            if(DEBUG)
            {
                unsigned int x;
                conv_byte_to_int(byte, x);
                cout << endl << " id_utenti for the header (" << id_transmission << ") = " << x;
            }
        }


        //write header.id_files
        for(unsigned int j=0; j<field_len; j++)
        {
            conv_int_to_byte(header_data[id_transmission].id_files[j], byte);
            trasmissione.push_back(byte);

            if(DEBUG)
            {
                unsigned int x;
                conv_byte_to_int(byte, x);
                cout << endl << " id_files for the header (" << id_transmission << ") = " << x;
            }
        }


        //write header.id_chunks
        for(unsigned int j=0; j<field_len; j++)
        {
            conv_int_to_byte(header_data[id_transmission].id_chunks[j], byte);
            trasmissione.push_back(byte);

            if(DEBUG)
            {
                unsigned int x;
                conv_byte_to_int(byte, x);
                cout << endl << " id_chunks for the header (" << id_transmission << ") = " << x;
            }
        }


        //write header.size_package and fine max payload_len
        payload_len = header_data[id_transmission].size_package[0];

        for(unsigned int j=0; j<field_len; j++)
        {
            conv_int_to_char(header_data[id_transmission].size_package[j], buff);
            for(int k=0; k<4; k++)
            {
                trasmissione.push_back(buff[k]);
            }
            if(DEBUG)
            {
                unsigned int x;
                conv_char_to_int(buff, x);
                cout << endl << " size_package for the header (" << id_transmission << ") = " << x;
            }

            //find max_size_pack in the header
            if(header_data[id_transmission].size_package[j] > payload_len)
            {
                payload_len = header_data[id_transmission].size_package[j];
            }

        }


        //store header pack size:
        small_pack_size.push_back(header_len+2);
        if(DEBUG)
        {
            cout << endl << " header small packet len  (" << id_transmission << ") = " << header_len+2;
        }



        /* ---------------------------------------------------------------------------------- */

        //WRITE SMALL PACKET WITH PAYLOAD
        if(DEBUG)
        {
            cout << endl << endl << " payload_len for the large packet (" << id_transmission << ") = " << payload_len;
        }

        //compute total number of small packet
        number_small_packet = payload_len / small_packet_len;
        //compute last small packet length
        last_small_packet_len = payload_len % small_packet_len;


        if(DEBUG)
        {
            cout << endl << " number_small_packet for the large packet (" << id_transmission << ") = " << number_small_packet;
            cout << endl << " last_small_packet_len for the large packet (" << id_transmission << ") = " << last_small_packet_len;
        }


        // check if number of small packet is minus then 65535
        if (number_small_packet > 65535)
        {
            cout << endl << "ERROR: number_small_packet > 65535 " << endl;
            exit(0);
        }
        else if(number_small_packet == 65535 && last_small_packet_len != 0)
        {
            cout << endl << "ERROR: number_small_packet = 65535  AND  last_small_packet_len != 0" << endl;
            exit(0);
        }



        unsigned int d_pos = 0;

        //write small packets
        for(unsigned short int id_small_packet = 1; id_small_packet <= number_small_packet; id_small_packet++)
        {
            //store small pack payolad size:
            small_pack_size.push_back(small_packet_len+2);
            if(DEBUG)
            {
                cout << endl << " small_packet_len for the small packet (" << id_small_packet << ") = " << small_packet_len;
            }


            //write id of small packet on the top
            conv_short_int_to_char(id_small_packet, buff_short);
            for(int k=0; k<2; k++)
            {
                trasmissione.push_back(buff_short[k]);
            }
            if(DEBUG)
            {
                unsigned short int y;
                conv_char_to_short_int(buff_short, y);
                cout << endl << " ID Small Packet for the small packet (" << id_small_packet << ") = " << y;
            }

            //write the small packet with payload
            for(unsigned int k=0; k<small_packet_len; k++)
            {
                trasmissione.push_back(coded_data.at(id_transmission).at(d_pos));
                d_pos++;
            }

        }

        if(last_small_packet_len > 0)
        {
            //store last small pack payolad size:
            small_pack_size.push_back(last_small_packet_len+2);
            if(DEBUG)
            {
                cout << endl << " last_small_packet_len for the small packet (" << number_small_packet + 1 << ") = " << last_small_packet_len;
            }

            //write id of small packet on the top
            unsigned short int id_last_small_packet = number_small_packet + 1;

            conv_short_int_to_char(id_last_small_packet, buff_short);
            for(int k=0; k<2; k++)
            {
                trasmissione.push_back(buff_short[k]);
            }
            if(DEBUG)
            {
                unsigned short int y;
                conv_char_to_short_int(buff_short, y);
                cout << endl << " ID Last Small Packet " << y;
            }

            //write the rest of small packet
            for(unsigned int k=0; k<last_small_packet_len; k++)
            {
                trasmissione.push_back(coded_data.at(id_transmission).at(d_pos));
                d_pos++;
            }
        }

    }/* end (for(int id_transmission=0; id_transmission < n_col; id_transmission++)) */
}


void Receive(header_transmission &header, vector<char> &coded_data, unsigned int small_packet_len, FILE *RX_file)
{
    unsigned short int id_small_packet;
    unsigned short int id_large_packet;

    unsigned int header_len;
    unsigned int field_len;
    unsigned int id_demand;
    unsigned int payload_len;

    unsigned int number_small_packet;
    unsigned int last_small_packet_len;

    char byte;
    char buff_short[2];
    char buff[4];

    bool DEBUG = 0;

    if(DEBUG)
    {
        cout << endl << "HEADER: ";
    }

    //read id small packet
    for(int k=0; k<2; k++)
    {
        fread(&buff_short[k],sizeof(char),1,RX_file);
    }
    conv_char_to_short_int(buff_short, id_small_packet);

    if(DEBUG)
    {
        cout << endl << "ID small packet = " << id_small_packet;
    }


    //read header length
    fread(&byte,sizeof(char),1,RX_file);
    conv_byte_to_int(byte, header_len);
    field_len = (header_len - 2) / 7;
    if(DEBUG)
    {
        cout << endl << "Header Length = " << header_len;
    }

    //read id_demand
    fread(&byte,sizeof(char),1,RX_file);
    conv_byte_to_int(byte, id_demand);
    if(DEBUG)
    {
        cout << endl << "ID Demand = " << id_demand;
    }


    //read header.id_utenti
    for(unsigned int j=0; j<field_len; j++)
    {
        header.id_utenti.push_back(0);
        fread(&byte,sizeof(char),1,RX_file);
        conv_byte_to_int(byte, header.id_utenti[j]);

        if(DEBUG)
        {
            cout << endl << "ID Utenti = " << header.id_utenti[j];
        }
    }


    //read header.id_files
    for(unsigned int j=0; j<field_len; j++)
    {
        header.id_files.push_back(0);
        fread(&byte,sizeof(char),1,RX_file);
        conv_byte_to_int(byte, header.id_files[j]);
        if(DEBUG)
        {
            cout << endl << "ID Files = " << header.id_files[j];
        }
    }


    //read header.id_chunks
    for(unsigned int j=0; j<field_len; j++)
    {
        header.id_chunks.push_back(0);
        fread(&byte,sizeof(char),1,RX_file);
        conv_byte_to_int(byte, header.id_chunks[j]);
        if(DEBUG)
        {
            cout << endl << "ID Chunks = " << header.id_chunks[j];
        }
    }


    //read header.size_package and compute payload_len
    payload_len = 0;

    for(unsigned int j=0; j<field_len; j++)
    {
        header.size_package.push_back(0);

        for(int k=0; k<4; k++)
        {
            fread(&buff[k],sizeof(char),1,RX_file);
        }
        conv_char_to_int(buff, header.size_package[j]);

        if(DEBUG)
        {
            cout << endl << "Size Package = " << header.size_package[j];
        }

        //find max_payload_len in the header
        if(field_len>1)
        {
            if(header.size_package[j] > payload_len)
            {
                payload_len = header.size_package[j];
            }
        }else
        {
            payload_len = header.size_package[j];
        }
    }


    /* ---------------------------------------------------------------------------------- */


    if(DEBUG)
    {
        cout << endl << endl << "Payload Length = " << payload_len;
    }


    //compute total number of small packet
    number_small_packet = payload_len / small_packet_len;
    //compute last small packet length
    last_small_packet_len = payload_len % small_packet_len;

    if(DEBUG)
    {
        cout << endl << " number_small_packet readed = " << number_small_packet;
        cout << endl << " last_small_packet_len readed = " << last_small_packet_len;
    }

    // check if number of small packet is minus then 65535
    if (number_small_packet > 65535)
    {
        cout << endl << "ERROR: number_small_packet > 65535 " << endl;
        exit(0);
    }
    else if(number_small_packet == 65535 && last_small_packet_len != 0)
    {
        cout << endl << "ERROR: number_small_packet = 65535  AND  last_small_packet_len != 0" << endl;
        exit(0);
    }


    /* ---------------------------------------------------------------------------------- */


    if(DEBUG)
    {
        cout << endl << endl << "LARGE PACKET: ";
    }

    unsigned int d_pos = 0;

    for (unsigned int i = 0; i < number_small_packet; i++)
    {
        //read id small packet
        for(int k=0; k<2; k++)
        {
            fread(&buff_short[k],sizeof(char),1,RX_file);
        }

        conv_char_to_short_int(buff_short, id_small_packet);

        if(DEBUG)
        {
            cout << endl << "ID small packet = " << id_small_packet;
        }

        for(unsigned int k = 0; k < small_packet_len; k++)
        {
            coded_data.push_back(0);
            fread(&coded_data.at(d_pos),sizeof(char),1,RX_file);
            d_pos++;
        }
    }

    if (last_small_packet_len > 0)
    {
        //read id small packet
        for(int k=0; k<2; k++)
        {
            fread(&buff_short[k],sizeof(char),1,RX_file);
        }

        conv_char_to_short_int(buff_short, id_small_packet);

        if(DEBUG)
        {
            cout << endl << "ID small packet = " << id_small_packet;
        }

        for(unsigned int k = 0; k < last_small_packet_len; k++)
        {
            coded_data.push_back(0);
            fread(&coded_data.at(d_pos),sizeof(char),1,RX_file);
            d_pos++;
        }
    }

    if(DEBUG)
    {
        cout << endl << "Coded Data total size = " << d_pos;
        cout << endl << endl << endl;
    }

}


//write trasmission
void write_byte(vector<char> trasmissione)
{
    FILE *TX_file;

    TX_file = fopen("../tx_file","wb"); //"/CachingFile/tx_file"

    for(unsigned int i=0; i<trasmissione.size(); i++)
    {
        fwrite(&trasmissione.at(i),sizeof(char),1,TX_file);
    }

    fclose(TX_file);
}


}//end namespace caching
