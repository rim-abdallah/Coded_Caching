#include "aes.h"

#undef byte  // Avoid ambiguity with std::byte

namespace caching
{
    using namespace std;
    using namespace CryptoPP;



    //generate group keys / users Keys
    vector<SecByteBlock> keySetGeneration(int nb_key)
    {
        vector<SecByteBlock> keySet;
        AutoSeededRandomPool prng;
        for (int i = 0; i < nb_key; ++i) 
        {
            SecByteBlock key128(16); // 128-bit key
            prng.GenerateBlock(key128, key128.size());
            keySet.push_back(key128);
        }
        return keySet;
    }

    //Hash users ID
    vector<char> TIDGeneration(int user_ID, const SecByteBlock& userkey) 
    {
        // Convert integer user ID to string
        string uid_str = to_string(user_ID);
        
        // Create HMAC-SHA256 object
        HMAC<SHA256> hmac(userkey.data(), userkey.size());
        
        // Calculate digest size and create buffer
        vector<char> result(hmac.DigestSize());
        
        // Compute HMAC
        hmac.CalculateDigest(
            reinterpret_cast<byte*>(result.data()), 
            reinterpret_cast<const byte*>(uid_str.data()), 
            uid_str.size()
        );
        
        return result;
    }

    // XOR operation
    vector<CryptoPP::byte> streamCipherXOR(const vector<CryptoPP::byte>& ptxt, const vector<CryptoPP::byte>& key)
    {
        vector<CryptoPP::byte> ctxt(ptxt.size());

        for (size_t i = 0; i < ptxt.size(); ++i) 
        {
            ctxt[i] = ptxt[i] ^ key[i % key.size()];
        }
        return ctxt;
    }


    // convert vector<char> to vector<byte>
    vector<CryptoPP::byte> conVecCharToByte(vector<char> input)
    {
        vector<CryptoPP::byte> output(input.begin(), input.end());
        return output;
    }


    //convert vector<byte> to vector<char>
    vector<char> conVecByteToChar(vector<CryptoPP::byte> input)
    {
        vector<char> output(input.begin(), input.end());
        return output;
    }


    //convert SecByteBlock to vector<byte>
    vector<CryptoPP::byte> secByteBlockToByteVec(const SecByteBlock& block) 
    {
        vector<CryptoPP::byte> vecByte;
        vecByte.reserve(block.size());  // Pre-allocate memory
        for (size_t i = 0; i < block.size(); i++) 
        {
            vecByte.push_back(block[i]);
        }
        return vecByte;
    }


    //convert vector<byte> to SecByteBlock
    SecByteBlock byteVecToSecByteBlock(const vector<CryptoPP::byte>& vecByte) 
    {
        SecByteBlock block(vecByte.size());
        for (size_t i = 0; i < vecByte.size(); i++) 
        {
            block[i] = vecByte[i];
        }
        return block;
    }


    // Helper: print vector as hex
    void printHex(const vector<CryptoPP::byte>& data) 
    {
        for (size_t i = 0; i < data.size(); ++i) 
        {
            cout << hex                 // Set output to hexadecimal
                 << setw(2)             // Always print 2 digits (e.g., "0a" not "a")
                 << setfill('0')        // Pad with '0' if less than 2 digits
                 << static_cast<int>(data[i]); // Convert byte to int so it prints as number
        }
        cout << dec << endl;       // Reset output to normal decimal mode
    }

    void printChar(const vector<char> data)
    {
        for (size_t i = 0; i < data.size(); ++i) 
        {
            cout << hex << setw(2) << setfill('0') 
                      << (static_cast<unsigned int>(data[i]) & 0xFF);
        }
        cout << dec << endl; // Reset to decimal
    }
    

}
