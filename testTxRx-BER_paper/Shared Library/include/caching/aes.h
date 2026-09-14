#ifndef AES_H_INCLUDED
#define AES_H_INCLUDED

#include <iostream>
#include <iomanip>
#include <vector>
#include <string>
#include <cryptopp/osrng.h>         // Random generator
#include <cryptopp/hex.h>           // Hex encoding
#include <cryptopp/filters.h>       // Filters
#include <cryptopp/hmac.h>
#include <cryptopp/sha.h>
#include <cryptopp/secblock.h>      // SecByteBlock
#include <cryptopp/cryptlib.h>      // Needed for `byte`
#include <cryptopp/base64.h>

#undef byte  // Avoid ambiguity with std::byte

namespace caching
{
    using namespace std;
    using namespace CryptoPP;

    
    vector<SecByteBlock> keySetGeneration(int nb_key);
    
    vector<char> TIDGeneration(int user_ID, const SecByteBlock& userkey);
    
    vector<CryptoPP::byte> streamCipherXOR(const vector<CryptoPP::byte>& ptxt, const vector<CryptoPP::byte>& key);
    
    vector<byte> conVecCharToByte(vector<char> input);

    vector<char> conVecByteToChar(vector<byte> input);
    
    vector<byte> secByteBlockToByteVec(const SecByteBlock& block);
    
    SecByteBlock byteVecToSecByteBlock(const vector<byte>& vecByte);
    
    void printHex(const vector<byte>& data);
    
    void printChar(const vector<char> data);
}

#endif // AES_H_INCLUDED
