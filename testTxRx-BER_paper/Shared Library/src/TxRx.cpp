#include "TxRx.h"

#include <thread>    // For std::this_thread::sleep_for
#include <chrono>    // For std::chrono::seconds

#define SQRT_TWO 0.707107
typedef std::complex< float > gr_complex;

using namespace std;

namespace caching
{


    void Transmit(int nb_users, header_transmission* header_data, vector< vector<char> > coded_data, unsigned int id_demand, int n_col, unsigned int small_packet_len, vector<vector<char>> &transmission, vector<unsigned int> &small_pack_size)
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

        
        int gain = 0;

        //open file
        //ofstream PacketSize_out, CachingGain_out;
        //PacketSize_out.open("/home/Batoul/Desktop/testTxRx-BER_paper/Results/Packets/PacketSize_" + to_string(nb_users) + ".txt",ios::app);
        //CachingGain_out.open("/home/Batoul/Desktop/testTxRx-BER_paper/CachingGain/CachingGain_" + to_string(nb_users) + ".txt",ios::app);
        

        cout << "start Transmit fct" << endl;

        for(int id_transmission=0; id_transmission < n_col; id_transmission++)
        {
            
            /* ---------------------------------------------------------------------------------- */

            //WRITE THE SMALL PACKET WITH THE HEADER 


            //write id_small_packet for the header (is always 0)
            //cout << "Headre ID : " << id_header << endl;

            conv_short_int_to_char(id_header, buff_short);
            for(int k=0; k<2; k++)
            {   
                transmission.at(id_transmission).push_back(buff_short[k]);
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

            //cout << "Header length : " << header_len << endl;

            conv_int_to_byte(header_len, byte);
            transmission.at(id_transmission).push_back(byte);

            if(DEBUG)
            {
                unsigned int x;
                conv_byte_to_int(byte, x);
                cout << endl << " header_len for the header (" << id_transmission << ") = " << x;
            }
 
            //write id of request
            id_large_packet = id_transmission;

            //cout << "ID demand : " << id_demand << endl;

            conv_int_to_byte(id_demand, byte);
            transmission.at(id_transmission).push_back(byte);
            
            if(DEBUG)
            {
                unsigned int x;
                conv_byte_to_int(byte, x);
                cout << endl << " id_demand for the header (" << id_transmission << ") = " << x;
            }


            //write header.id_utenti
            for(unsigned int j=0; j<field_len; j++)
            {
                //cout << " User " << j << " : " <<  header_data[id_transmission].id_utenti[j] << endl;

                conv_int_to_byte(header_data[id_transmission].id_utenti[j], byte);
                transmission.at(id_transmission).push_back(byte);
                
            
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
                //cout << "File " << j << " : " << header_data[id_transmission].id_files[j] << endl;

                conv_int_to_byte(header_data[id_transmission].id_files[j], byte);
                transmission.at(id_transmission).push_back(byte);

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
                //cout << "Chunk " << j << " : " << header_data[id_transmission].id_chunks[j] << endl;

                conv_int_to_byte(header_data[id_transmission].id_chunks[j], byte);
                transmission.at(id_transmission).push_back(byte);
                

                if(DEBUG)
                {
                    unsigned int x;
                    conv_byte_to_int(byte, x);
                    cout << endl << " id_chunks for the header (" << id_transmission << ") = " << x;
                }
            }

            //write header.size_package and find max payload_len
            payload_len = header_data[id_transmission].size_package[0];

            for(unsigned int j=0; j<field_len; j++)
            {
                //cout << "Chunk size " << j << " : " << header_data[id_transmission].size_package[j] << endl;

                conv_int_to_char(header_data[id_transmission].size_package[j], buff);
                for(int k=0; k<4; k++)
                {   
                    transmission.at(id_transmission).push_back(buff[k]);
                    
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
                    transmission.at(id_transmission).push_back(buff_short[k]);
                    
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
                    transmission.at(id_transmission).push_back(coded_data.at(id_transmission).at(d_pos));
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
                    transmission.at(id_transmission).push_back(buff_short[k]);
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
                    transmission.at(id_transmission).push_back(coded_data.at(id_transmission).at(d_pos));
                    d_pos++;
                }

            }

        //PacketSize_out << transmission.at(id_transmission).size() << endl;
        gain += transmission.at(id_transmission).size();

        }/* end (for(int id_transmission=0; id_transmission < n_col; id_transmission++)) */


        //CachingGain_out << gain << endl;

        //PacketSize_out.close();
        //CachingGain_out.close();

        cout << "end Transmit fct" << endl;
    }


    void TransmitEnc(int nb_users, header_transmission* header_data, vector< vector<char> > coded_data, unsigned int id_demand, int n_col, unsigned int small_packet_len, 
                vector<vector<char>> &transmission, vector<unsigned int> &small_pack_size, vector<SecByteBlock> groupKeySet, vector<SecByteBlock> usersKeySet)
    {
        unsigned short int id_header = 0;
        unsigned int field_len;
        unsigned short int header_len;
        unsigned short int id_large_packet;
        unsigned int payload_len;

        unsigned int number_small_packet;
        unsigned int last_small_packet_len;

        char byte;
        char buff_short[2];
        char buff[4];

        vector<CryptoPP::byte> tempByteVec;
        vector<CryptoPP::byte> encVec;
        int userId;

        bool DEBUG = false;

        
        int gain = 0;

        //open file
        //ofstream encPacketSize_out, encCachingGain_out;
        //encPacketSize_out.open("/home/Batoul/Desktop/testTxRx-BER_paper/Results/Packets/encPacketSize_" + to_string(nb_users) + ".txt",ios::app);
        //encCachingGain_out.open("/home/Batoul/Desktop/testTxRx-BER_paper/CachingGain/encCachingGain_" + to_string(nb_users) + ".txt",ios::app);
        

        
        cout << "start TransmitEnc fct" << endl;

        for(int id_transmission=0; id_transmission < n_col; id_transmission++)
        {
            
            /* ---------------------------------------------------------------------------------- */


            //WRITE THE SMALL PACKET WITH THE HEADER 


            //write id_small_packet for the header (is always 0)

            //cout << " Header Id : " << id_header << endl;

            conv_short_int_to_char(id_header, buff_short);
            for(int k=0; k<2; k++)
            {   
                transmission.at(id_transmission).push_back(buff_short[k]);
                
            }
            if(DEBUG)
            {
                unsigned short int y;
                conv_char_to_short_int(buff_short, y);
                cout << endl << " ID Small Packet for the header (" << id_transmission << ") = " << y;
            }

            
            //write header_len

            //nb of users per transmission
            field_len = header_data[id_transmission].id_utenti.size();

            //cout << " users nb : " << field_len << endl;

            
            header_len = 5 + 38 * field_len + 16 * field_len + 4;

            //cout << " header length : " << header_len << endl;

            conv_short_int_to_char(header_len, buff_short);
            for(int k=0; k<2; k++)
            {   
                transmission.at(id_transmission).push_back(buff_short[k]);
                
            }
            if(DEBUG)
            {
                unsigned short int y;
                conv_char_to_short_int(buff_short, y);
                cout << endl << " Header length (" << id_transmission << ") = " << y;
            }
            

            //write id of request
            id_large_packet = id_transmission;

            //cout << "Id demand : " << id_demand << endl;

            conv_int_to_byte(id_demand, byte);
            transmission.at(id_transmission).push_back(byte);

            if(DEBUG)
            {
                unsigned int x;
                conv_byte_to_int(byte, x);
                cout << endl << " id_demand for the header (" << id_transmission << ") = " << x;
            }


            vector<char> UID;

            //write header.id_utenti
            for(unsigned int j=0; j<field_len; j++)
            {
                //cout << "user " << j << " : " <<  header_data[id_transmission].id_utenti[j] << endl;

                UID = TIDGeneration(header_data[id_transmission].id_utenti[j], usersKeySet.at(j));
                for(int k=0; k<UID.size(); k++)
                {
                    transmission.at(id_transmission).push_back(UID[k]);
                }

                //cout << "Enc user " << j << " : ";
                //printChar(UID);
            
                if(DEBUG)
                {
                    cout << endl << " id_utenti for the header (" << id_transmission << ") = " << header_data[id_transmission].id_utenti[j];
                }
            }


            //write group key
            for(unsigned int j=0; j<field_len; j++)
            {
                userId = header_data[id_transmission].id_utenti[j];
                tempByteVec = secByteBlockToByteVec(groupKeySet[id_transmission]);
                
                //cout << "Group key " << j << " before : " ;
                //printHex(tempByteVec);

                encVec = streamCipherXOR(tempByteVec, secByteBlockToByteVec(usersKeySet.at(userId)));

                //cout << "Enc Group key " << j << " : " ;
                //printHex(encVec);

                for(int k=0; k<encVec.size(); k++)
                {
                    transmission.at(id_transmission).push_back(encVec[k]);
                }

                if(DEBUG)
                {
                    cout << endl << " key for the header (" << id_transmission << ") = ";
                    printHex(tempByteVec);
                }
            }
            
            vector<CryptoPP::byte> metadata;

            //write header.id_files
            for(unsigned int j=0; j<field_len; j++)
            {
                //cout << "file Id " << j << " : " <<  header_data[id_transmission].id_files[j] << endl;

                conv_int_to_byte(header_data[id_transmission].id_files[j], byte);
                metadata.push_back(byte);

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
                //cout << "chunk Id " << j << " : " <<  header_data[id_transmission].id_chunks[j] << endl;

                conv_int_to_byte(header_data[id_transmission].id_chunks[j], byte);
                metadata.push_back(byte);

                if(DEBUG)
                {
                    unsigned int x;
                    conv_byte_to_int(byte, x);
                    cout << endl << " id_chunks for the header (" << id_transmission << ") = " << x;
                }
            }
            

            //write header.size_package (chunk size)

            for(unsigned int j=0; j<field_len; j++)
            {
                //cout << "Chunk size " << j << " : " <<  header_data[id_transmission].size_package[j] << endl;

                conv_int_to_char(header_data[id_transmission].size_package[j], buff);  //chunk size
                for(int k=0; k<4; k++)
                {   
                    metadata.push_back(buff[k]);
                }
                if(DEBUG)
                {
                    unsigned int x;
                    conv_char_to_int(buff, x);
                    cout << endl << " size_package for the header (" << id_transmission << ") = " << x;
                }

            }

            //encrypt metadata
            tempByteVec = secByteBlockToByteVec(groupKeySet[id_transmission]);
            encVec = streamCipherXOR(metadata, tempByteVec);
            
            //cout << "MetaData : "; 
            //printHex(encVec);
            
            for(int k=0; k<encVec.size(); k++)
            {
                transmission.at(id_transmission).push_back(encVec[k]);
                
            }

            //store header pack size:
            small_pack_size.push_back(header_len+2);
            if(DEBUG)
            {
                cout << endl << " header small packet len  (" << id_transmission << ") = " << header_len+2;
            }


            /* ---------------------------------------------------------------------------------- */
            

            //WRITE SMALL PACKET WITH PAYLOAD

            //find max_size_pack in the header
            payload_len = coded_data.at(id_transmission).size();

            //cout << "Payload length : " << payload_len << endl;

            conv_int_to_char(payload_len, buff);
            for(int k=0; k<4; k++)
            {   
                transmission.at(id_transmission).push_back(buff[k]);
                    
            }
            if(DEBUG)
            {
                unsigned int x;
                conv_char_to_int(buff, x);
                cout << endl << " Payload length (" << id_transmission << ") = " << x;
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
                    transmission.at(id_transmission).push_back(buff_short[k]);
                    
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
                    transmission.at(id_transmission).push_back(coded_data.at(id_transmission).at(d_pos));
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
                    transmission.at(id_transmission).push_back(buff_short[k]);
                    
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
                    transmission.at(id_transmission).push_back(coded_data.at(id_transmission).at(d_pos));
                    d_pos++;
                }
            }

            //cout << "data : " << endl;
            //printHex(conVecCharToByte(coded_data.at(id_transmission)));

            //encPacketSize_out << transmission.at(id_transmission).size() << endl;
            gain += transmission.at(id_transmission).size();

        }/* end (for(int id_transmission=0; id_transmission < n_col; id_transmission++)) */


        //encCachingGain_out << gain << endl;

        //encPacketSize_out.close();
        //encCachingGain_out.close();

        cout << "end TransmitEnc fct" << endl;
    }


    void Receive(int nb_users, header_transmission &header, vector<char> &coded_data, unsigned int small_packet_len, vector<char> transmission)
    {

        //ofstream receive_time; 
        //receive_time.open("/home/Batoul/Desktop/testTxRx-BER_new_sec_v2/Results/ReceiveTime/ReceiveTime_" + to_string(nb_users) + ".txt",ios::app);
        
        //auto startRx = std::chrono::high_resolution_clock::now();
        
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

        int tran_pos = 0;
            
        if(DEBUG)
        {
            cout << endl << "HEADER: ";
        }

        //read id small packet
        for(int k=0; k<2; k++)
        {   
            buff_short[k] = transmission.at(tran_pos);
            tran_pos++;
        }
        conv_char_to_short_int(buff_short, id_small_packet);

        if(DEBUG)
        {
            cout << endl << "ID small packet = " << id_small_packet;
        }


        //read header length
        byte = transmission.at(tran_pos);
        tran_pos++;
        conv_byte_to_int(byte, header_len);
        field_len = (header_len - 2) / 7;
        if(DEBUG)
        {
            cout << endl << "Header Length = " << header_len;
        }

        //read id_demand
        byte = transmission.at(tran_pos);
        tran_pos++;
        conv_byte_to_int(byte, id_demand);
        if(DEBUG)
        {
            cout << endl << "ID Demand = " << id_demand;
        }


        //read header.id_utenti
        for(unsigned int j=0; j<field_len; j++)
        {
            header.id_utenti.push_back(0);
            byte = transmission.at(tran_pos);
            tran_pos++;
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
            byte = transmission.at(tran_pos);
            tran_pos++;
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
            byte = transmission.at(tran_pos);
            tran_pos++;
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
                buff[k] = transmission.at(tran_pos);
                tran_pos++;
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


        for (unsigned int i = 0; i < number_small_packet; i++)
        {
            //read id small packet
            for(int k=0; k<2; k++)
            {   
                buff_short[k] = transmission.at(tran_pos);
                tran_pos++;
            }
            
            conv_char_to_short_int(buff_short, id_small_packet);

            if(DEBUG)
            {
                cout << endl << "ID small packet = " << id_small_packet;
            }

            for(unsigned int k = 0; k < small_packet_len; k++)
            {
                coded_data.push_back(0);
                coded_data.push_back(transmission.at(tran_pos));
                tran_pos++;
            }
        }

        if (last_small_packet_len > 0)
        {
            //read id small packet
            for(int k=0; k<2; k++)
            {   
                buff_short[k] = transmission.at(tran_pos);
                tran_pos++;
            }
            
            conv_char_to_short_int(buff_short, id_small_packet);

            if(DEBUG)
            {
                cout << endl << "ID small packet = " << id_small_packet;
            }

            for(unsigned int k = 0; k < last_small_packet_len; k++)
            {
                coded_data.push_back(0);
                coded_data.push_back(transmission.at(tran_pos));
                tran_pos++;
            }
        }

        //auto endRx = chrono::high_resolution_clock::now();
        //chrono::duration<double, std::milli> durationRx = endRx - startRx;
        //receive_time << durationRx.count() << endl;

        //receive_time.close();

    }


    /*
    void ReceiveDec(int nb_users, header_transmission &header, vector<char> &coded_data, unsigned int small_packet_len, vector<char> transmission, 
         SecByteBlock aesIV, SecByteBlock aesKey, vector<SecByteBlock> usersIVset, vector<SecByteBlock> usersKeySet)
    {
        unsigned short int id_small_packet;
        unsigned short int id_large_packet;

        unsigned short int header_len;
        unsigned int id_demand;
        unsigned int payload_len;

        unsigned int number_small_packet;
        unsigned int last_small_packet_len;

        char byte;
        char buff_short[2];
        char buff[4];
        
        bool DEBUG = 0;

        int tran_pos = 0;

        //std::chrono::duration<double, std::milli> totalTime{0};  // Accumulator

        //open file
        //ofstream dec_time;
        //dec_time.open("/home/Batoul/Desktop/testTxRx-BER_new_sec_v2/Results/DecryptionTime/decTime_" + to_string(nb_users) + ".txt",ios::app);

            
        if(DEBUG)
        {
            cout << endl << "HEADER: ";
        }

        //read id small packet
        for(int k=0; k<2; k++)
        {   
            buff_short[k] = transmission.at(tran_pos);
            tran_pos++;
        }
        conv_char_to_short_int(buff_short, id_small_packet);
        
        //cout << " Header Id : " << id_small_packet << endl;

        if(DEBUG)
        {
            cout << endl << "ID small packet = " << id_small_packet;
        }


        //read header length
        for(int k=0; k<2; k++)
        {   
            buff_short[k] = transmission.at(tran_pos);
            tran_pos++;
        }
        conv_char_to_short_int(buff_short, header_len);
                
        //cout << " header length : " << header_len << endl;

        if (header_len == 0) 
         {
            cout << "header = Exiting function." << endl;
            this_thread::sleep_for(chrono::seconds(1));
            return;  // Exit the function early
        }

        if(DEBUG)
        {
            cout << endl << "Header Length = " << header_len;
        }


        int field_len = ( header_len - 9 ) / 39;
        //cout << " users nb : " << field_len << endl;

        //read id_demand
        byte = transmission.at(tran_pos);
        tran_pos++;
        conv_byte_to_int(byte, id_demand);
        //cout << "Id demand : " << id_demand << endl;

        if(DEBUG)
        {
            cout << endl << "ID Demand = " << id_demand;
        }

        
        //read header.id_utenti
        for(unsigned int j=0; j<field_len; j++)
        {
            header.id_utenti.push_back(0);
            byte = transmission.at(tran_pos);
            tran_pos++;
            conv_byte_to_int(byte, header.id_utenti[j]);
            
            //cout << "user Id" << j << " : " <<  header.id_utenti[j] << endl;

            if(DEBUG)
            {
                cout << endl << "ID Utenti = " << header.id_utenti[j];
            }
        }


        vector<char> tempCharVec;
        vector<CryptoPP::byte> tempByteVec, tempKey, decVec;
        vector<SecByteBlock> groupKeySet;
        SecByteBlock groupKey, sessionKey;
        int userId;


        //read group key 
        for(unsigned int j=0; j<field_len; j++)
        {
            userId = header.id_utenti[j];
            tempCharVec.clear();
            tempByteVec.clear();
            for(int k=0; k<32; k++)
            {   
                tempCharVec.push_back(transmission.at(tran_pos));
                tran_pos++;
            }
            tempByteVec = conVecCharToByte(tempCharVec);

            //cout << "Group key " << j << " : " ;
            //printHex(tempByteVec);

            /*
            if(j == 0)
            {
                auto start1 = std::chrono::high_resolution_clock::now();
                decVec = aesDecryption(tempByteVec, usersIVset[userId], usersKeySet[userId]);
                auto end1 = std::chrono::high_resolution_clock::now();
                totalTime += end1 - start1;
            }
            else
            {/
                decVec = aesDecryption(tempByteVec, usersIVset[userId], usersKeySet[userId]);
            //}
            
            //cout << "Group key " << j << " after : " ;
            //printHex(decVec);

            groupKeySet.push_back(byteVecToSecByteBlock(decVec));

            if(DEBUG)
            {
                cout << endl << "Group key = ";
                printHex(decVec);
            }
        }

        

        // check the group key
        groupKey = groupKeySet[0];

        for(unsigned int j=1; j<field_len; j++)
        {
            if(groupKey != groupKeySet[j])
            {
                cout << "different group keys" << endl;
                exit(0);
            }
        }


        //generate session key

        //auto start2 = std::chrono::high_resolution_clock::now();

        sessionKey = sessionKeyGeneration(groupKey, aesIV, aesKey);

        //auto end2 = std::chrono::high_resolution_clock::now();
        //totalTime += end2 - start2;

        tempKey = secByteBlockToByteVec(sessionKey);


        //read metadata
        vector<CryptoPP::byte> metadata;
        int metadata_enc_len = 6 * field_len;

        
        tempCharVec.clear();
        tempByteVec.clear();
        for(int k=0; k<metadata_enc_len; k++)
        {   
            tempCharVec.push_back(transmission.at(tran_pos));
            tran_pos++;
        }
        tempByteVec = conVecCharToByte(tempCharVec);

        //cout << "MetaData : "; 
        //printHex(tempByteVec);
        
        
        metadata = streamCipherXOR(tempByteVec, tempKey);


        int meta_pos = 0;
    
        //read header.id_files
        for(unsigned int j=0; j<field_len; j++)
        {
            header.id_files.push_back(0);
            byte = metadata[meta_pos];
            conv_byte_to_int(byte, header.id_files[j]);
            
            //cout << "file Id" << j << " : " <<  header.id_files[j] << endl;
            
            if(DEBUG)
            {
                cout << endl << "ID Files = " << header.id_files[j];
            }
            meta_pos++;
        }


        //read header.id_chunks
        for(unsigned int j=0; j<field_len; j++)
        {
            header.id_chunks.push_back(0);
            byte = metadata[meta_pos];
            conv_byte_to_int(byte, header.id_chunks[j]);
            
            //cout << "chunk Id" << j << " : " <<  header.id_chunks[j] << endl;
            
            if(DEBUG)
            {
                cout << endl << "ID Chunks = " << header.id_chunks[j];
            }
            meta_pos++;
        }


        //read header.size_package
        for(unsigned int j=0; j<field_len; j++)
        {
            header.size_package.push_back(0);

            for(int k=0; k<4; k++)
            {   
                buff[k] = metadata[meta_pos];
                meta_pos++;
            }
            conv_char_to_int(buff, header.size_package[j]);

            //cout << "Chunk size" << j << " : " <<  header.size_package[j] << endl;


            if(DEBUG)
            {
                cout << endl << "Size Package = " << header.size_package[j];
            }
        }


        /* ---------------------------------------------------------------------------------- /


        //read payload length
        for(int k=0; k<4; k++)
        {   
            buff[k] = transmission.at(tran_pos);
            tran_pos++;
        }
        conv_char_to_int(buff, payload_len);
        
        //cout << "Payload length : " << payload_len << endl;

        if(DEBUG)
        {
            cout << endl << "payload length = " << payload_len;
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


        /* ---------------------------------------------------------------------------------- /


        if(DEBUG)
        {
            cout << endl << endl << "LARGE PACKET: ";
        }

        vector<char> coded_data_enc;


        for (unsigned int i = 0; i < number_small_packet; i++)
        {
            //read id small packet
            for(int k=0; k<2; k++)
            {   
                buff_short[k] = transmission.at(tran_pos);
                tran_pos++;
            }
            
            conv_char_to_short_int(buff_short, id_small_packet);

            if(DEBUG)
            {
                cout << endl << "ID small packet = " << id_small_packet;
            }

            for(unsigned int k = 0; k < small_packet_len; k++)
            {
                coded_data_enc.push_back(transmission.at(tran_pos));
                tran_pos++;
            }

        }

        if (last_small_packet_len > 0)
        {
            //read id small packet
            for(int k=0; k<2; k++)
            {   
                buff_short[k] = transmission.at(tran_pos);
                tran_pos++;
            }
            
            conv_char_to_short_int(buff_short, id_small_packet);

            if(DEBUG)
            {
                cout << endl << "ID small packet = " << id_small_packet;
            }

            for(unsigned int k = 0; k < last_small_packet_len; k++)
            {
                coded_data_enc.push_back(transmission.at(tran_pos));
                tran_pos++;
            }

        }

        tempByteVec = conVecCharToByte(coded_data_enc);

        //auto start3 = std::chrono::high_resolution_clock::now();

        decVec = streamCipherXOR(tempByteVec, tempKey);

        //auto end3 = std::chrono::high_resolution_clock::now();


        //cout << "Decrypted data : " << endl;
        //printHex(decVec);

        coded_data = conVecByteToChar(decVec);

        
        //totalTime += end3 - start3;

        //dec_time << totalTime.count() << endl;

        //dec_time.close();

    }
    */


    //write trasmission
    void write_byte(vector<char> transmission)
    {
        FILE *TX_file;

        TX_file = fopen("../tx_file","wb"); //"/CachingFile/tx_file"

        for(unsigned int i=0; i<transmission.size(); i++)
        {
            fwrite(&transmission.at(i),sizeof(char),1,TX_file);
        }

        fclose(TX_file);
    }


}//end namespace caching