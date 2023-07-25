#include <iostream>
#include <bitset>
#include <string>
#include <cstdio>
#define INT_BITS 32

using namespace std;

uint32_t rightrotate(uint32_t n, int d)
{
    return (n >> d)|(n << (INT_BITS - d));
}

string sha256(string msg)
{
    uint32_t h0 = 0x6a09e667;
    uint32_t h1 = 0xbb67ae85;
    uint32_t h2 = 0x3c6ef372;
    uint32_t h3 = 0xa54ff53a;
    uint32_t h4 = 0x510e527f;
    uint32_t h5 = 0x9b05688c;
    uint32_t h6 = 0x1f83d9ab;
    uint32_t h7 = 0x5be0cd19;

    //Initialize array of round constants:
    //(first 32 bits of the fractional parts of the cube roots of the first 64 primes 2..311):
    uint32_t k[] =
       {0x428a2f98, 0x71374491, 0xb5c0fbcf, 0xe9b5dba5, 0x3956c25b, 0x59f111f1, 0x923f82a4, 0xab1c5ed5,
       0xd807aa98, 0x12835b01, 0x243185be, 0x550c7dc3, 0x72be5d74, 0x80deb1fe, 0x9bdc06a7, 0xc19bf174,
       0xe49b69c1, 0xefbe4786, 0x0fc19dc6, 0x240ca1cc, 0x2de92c6f, 0x4a7484aa, 0x5cb0a9dc, 0x76f988da,
       0x983e5152, 0xa831c66d, 0xb00327c8, 0xbf597fc7, 0xc6e00bf3, 0xd5a79147, 0x06ca6351, 0x14292967,
       0x27b70a85, 0x2e1b2138, 0x4d2c6dfc, 0x53380d13, 0x650a7354, 0x766a0abb, 0x81c2c92e, 0x92722c85,
       0xa2bfe8a1, 0xa81a664b, 0xc24b8b70, 0xc76c51a3, 0xd192e819, 0xd6990624, 0xf40e3585, 0x106aa070,
       0x19a4c116, 0x1e376c08, 0x2748774c, 0x34b0bcb5, 0x391c0cb3, 0x4ed8aa4a, 0x5b9cca4f, 0x682e6ff3,
       0x748f82ee, 0x78a5636f, 0x84c87814, 0x8cc70208, 0x90befffa, 0xa4506ceb, 0xbef9a3f7, 0xc67178f2};

    //Pre-processing (Padding):
    //begin with the original message of length L bits
    //append a single '1' bit
    int size = msg.size();
    string binMsg = "";
    for (int i = 0; i < size; i++)
    {
        uint8_t ch = msg[i];
        string binary = bitset<8>(ch).to_string();
        binMsg += binary;
    }
    uint64_t L = binMsg.size();
    //cout << L << endl;
    binMsg += "1";
    //append K '0' bits, where K is the minimum number >= 0 such that (L + 1 + K + 64) is a multiple of 512
    uint64_t K = 0;
    while (K == 0 || (L + 1 + K + 64) % 512 != 0)
    {
        binMsg += "0";
        K++;
    }
    //append L as a 64-bit big-endian integer, making the total post-processed length a multiple of 512 bits such that the bits in the message are: <original message of length L> 1 <K zeros> <L as 64 bit integer> , (the number of bits will be a multiple of 512)
    string bin64 = bitset<64>(L).to_string();
    binMsg += bin64;

    //Process the message in successive 512-bit chunks:
    //break message into 512-bit chunks
    string temp = "";
    const int CHUNKS = binMsg.size() / 512;
    string chunks[CHUNKS];
    int p = 0;
    for (int i = 0; i < binMsg.size(); i++)
    {
        temp += binMsg[i];
        if (temp.size() == 512)
        {
            chunks[p] = temp;
            p++;
            temp = "";
        }
    }
    //for each chunk
    for (int chunk = 0; chunk < CHUNKS; chunk++)
    {
        //create a 64-entry message schedule array w[0..63] of 32-bit words
        //(The initial values in w[0..63] don't matter, so many implementations zero them here)
        //copy chunk into first 16 words w[0..15] of the message schedule array
        string w[64];
        string temp = "";
        int p = 0;
        cout<<endl;
        for (int i = 0; i < chunks[chunk].size(); i++)
        {
            temp += chunks[chunk][i];
            if (temp.size() == 32)
            {   
                w[p] = temp;
                p++; 
                temp = "";
            }
        }

        //Extend the first 16 words into the remaining 48 words w[16..63] of the message schedule array:
        for (int i = 16; i < 64; i++)
        {
            uint32_t s0 = (rightrotate((uint32_t)(stoul(w[i-15], 0, 2)), 7)) ^ (rightrotate((uint32_t)(stoul(w[i-15], 0, 2)), 18)) ^ (stoul(w[i-15], 0, 2) >> 3);
            uint32_t s1 = (rightrotate((uint32_t)(stoul(w[i-2], 0, 2)), 17)) ^ (rightrotate((uint32_t)(stoul(w[i-2], 0, 2)), 19)) ^ (stoul(w[i-2], 0, 2) >> 10);
            uint32_t s2 = stoul(w[i-16], 0, 2);
            uint32_t s3 = stoul(w[i-7], 0, 2);
            w[i] = bitset<32>(s2 + s0 + s3 + s1).to_string();
        }

        //Initialize working variables to current hash value:
        uint32_t a = h0;
        uint32_t b = h1;
        uint32_t c = h2;
        uint32_t d = h3;
        uint32_t e = h4;
        uint32_t f = h5;
        uint32_t g = h6;
        uint32_t h = h7;

        //Compression function main loop:
        for (int i = 0; i < 64; i++) 
        {
            uint32_t S1 = rightrotate(e, 6) ^ rightrotate(e, 11) ^ rightrotate(e, 25);
            uint32_t ch = (e & f) ^ ((~e) & g);
            uint32_t temp1 = h + S1 + ch + k[i] + (stoul(w[i], 0, 2));
            uint32_t S0 = rightrotate(a, 2) ^ rightrotate(a, 13) ^ rightrotate(a, 22);
            uint32_t maj = (a & b) ^ (a & c) ^ (b & c);
            uint32_t temp2 = S0 + maj;
    
            h = g;
            g = f;
            f = e;
            e = d + temp1;
            d = c;
            c = b;
            b = a;
            a = temp1 + temp2;
        }

        //Add the compressed chunk to the current hash value:
        h0 = h0 + a;
        h1 = h1 + b;
        h2 = h2 + c;
        h3 = h3 + d;
        h4 = h4 + e;
        h5 = h5 + f;
        h6 = h6 + g;
        h7 = h7 + h;
    }
    
    //Produce the final hash value (big-endian):
    char strh0[256],strh1[256],strh2[256],strh3[256],strh4[256],strh5[256],strh6[256],strh7[256];
    snprintf(strh0,256,"%X",h0);
    snprintf(strh1,256,"%X",h1);
    snprintf(strh2,256,"%X",h2);
    snprintf(strh3,256,"%X",h3);
    snprintf(strh4,256,"%X",h4);
    snprintf(strh5,256,"%X",h5);
    snprintf(strh6,256,"%X",h6);
    snprintf(strh7,256,"%X",h7);

    string h0FINAL (strh0);
    string h1FINAL (strh1);
    string h2FINAL (strh2);
    string h3FINAL (strh3);
    string h4FINAL (strh4);
    string h5FINAL (strh5);
    string h6FINAL (strh6);
    string h7FINAL (strh7);

    string hash = h0FINAL + h1FINAL + h2FINAL + h3FINAL + h4FINAL + h5FINAL + h6FINAL + h7FINAL;
    return hash;
}

int main()
{
    string str;
    cout << "Enter string to be hashed: ";
    cin >> str;
    string hashedStr = sha256(str);
    cout << "Hashed string: " << hashedStr << endl;
}
