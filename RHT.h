#include <iostream>
#include <string>
#include <fstream>
#include <bitset>
#include <cstring>
#include <sstream>
#include <cstdlib>
#include <algorithm>
#include <cstdio>
#include <vector>
#include <stdint.h>
using std::string;

size_t PointerListLen = 11;
size_t WindowListLen = 2048;

uint32_t get_c[] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 
                    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 
                    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 
                    0, 0, 0, 0, 1, 0, 0, 0, 2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 
                    3};

//# just for debug
uint32_t MASK = 0xffffffff;
#define BASE_MASK 0x3
enum
{
    BASE_A = 0x0,
    BASE_C = 0x1,
    BASE_G = 0x2,
    BASE_T = 0x3,
};


uint32_t to_bit(string s)
{
    uint32_t bitnum = 0, shift, bit_len = PointerListLen;
    for (size_t i = 0; i < bit_len; i++) 
    {
        shift = 2*bit_len-2-2*(i%(bit_len));
        switch (s[i])
        {
            case 'A':
                bitnum |= BASE_A << shift;
                break;
            case 'C':
                bitnum |= BASE_C << shift;
                break;
            case 'G':
                bitnum |= BASE_G << shift;
                break;
            case 'T':
                bitnum |= BASE_T << shift;
                break;
            default:
                std::cout << s << " invalid DNA base\n";
                return 0;

        }
    }
    return bitnum;
}
string to_string(uint32_t bb)
{
    uint32_t bitnum = bb, shift, base, mask, bit_len = PointerListLen;
    string s(bit_len, 0);
    for (size_t i = 0; i < bit_len; i++)
    {
        shift = 2*bit_len-2-2*(i%(bit_len));
        mask = BASE_MASK << shift;
        base = (bitnum & mask) >> shift;

        switch (base)
        {
            case BASE_A:
                s[i] = 'A';
                break;
            case BASE_C:
                s[i] = 'C';
                break;
            case BASE_G:
                s[i] = 'G';
                break;
            case BASE_T:
                s[i] = 'T';
                break;
            default:
                std::cout << "invalid bitnum\n";
                return "";
        }
    }
    return s;
}
//#


class RHT
{
    private:
        size_t pw_len, p_len, w_len;
        uint64_t tmp;
    public:
        uint32_t index, w_index;
        uint64_t *PW;
        uint32_t *P, *W;

        RHT(size_t k)
        {
            p_len = (size_t)1 << (PointerListLen << 1) + 1;
            w_len = k << 1;
            pw_len = w_len;

            // P = new uint32_t[p_len];
            // W = new uint32_t[w_len];
            // PW = new uint64_t[pw_len];

            P = (uint32_t *)malloc(p_len * sizeof(uint32_t));
            W = (uint32_t *)malloc(w_len * sizeof(uint32_t));
            PW = (uint64_t *)malloc(pw_len * sizeof(uint64_t));

            index = 0;
            w_index = 0;
        }
        ~RHT()
        {
            // delete [] P;
            // delete [] W;
            // delete [] PW;
            if (P) free(P);
            if (W) free(W);
            if (PW) free(PW);
        }

        void clear()
        {
            // memset(PW, sizeof(PW), 0);
            memset(P, sizeof(P), 0);
            index = 0;
        }

        void clear_PW()
        {
            for (size_t i = 0; i<index; ++i) PW[i] = 0;
            // memset(PW, sizeof(PW), 0);
            index = 0;
        }

        void link_string(uint32_t p_index, uint32_t w_index)
        {
            // P[p_index] = index;
            tmp = p_index;
            tmp = tmp << 32;
            tmp = tmp | w_index;
            // std::cout << std::bitset<64>(tmp) << " " << std::bitset<32>(p_index) << " " << std::bitset<32>(w_index) << std::endl;
            PW[index++] = tmp;
        }

        uint32_t search(uint32_t p_index)
        {
            //binary search
            uint32_t l=0, r=index-1, m;
            while (l < r)
            {
                m = (l+r)/2;
                if (p_index <= (PW[m]>>32)) r = m;
                else l = m + 1;
            }
            if ((PW[r]>>32) == p_index) return r+1;
            return 0;
        }

        void sort_pw()
        {
            std::sort(PW, PW+index);
            // uint32_t p_tmp;
            // for (size_t i = 0; i < index; ++i)
            // {
            //     p_tmp = PW[i]>>32;
            //     if (!P[p_tmp])
            //     P[p_tmp] = i+1;
            // }
        }
        void print_pw(std::ofstream &outt)
        {
            for (size_t i = 0; i<index; ++i)
            {
                outt << to_string(PW[i] >> 32) << " " << (uint32_t)PW[i] << "\n";
            }
        }

        void create_p_w()
        {
            std::sort(PW, PW+index);

            // memset(P, 0, sizeof(P));
            uint32_t p_tmp, last;
            w_index=0;
            p_tmp = PW[0] >> 32;
            last = p_tmp;
            P[p_tmp] = w_index + 1;
            W[w_index] = PW[0];
            ++w_index;
            // std::cout << std::bitset<64>(PW[0]) << " " << std::bitset<32>(p_tmp) << " " << std::bitset<32>(W[w_index])  << std::endl;
            for (size_t i = 1; i<index; ++i)
            {
                if (PW[i]==PW[i-1]) continue;
                p_tmp = PW[i] >> 32;
                if (p_tmp == last) 
                {
                    W[w_index++] = PW[i];
                    continue;
                }
                
                P[last+1] = w_index+1;
                P[p_tmp] = w_index+1;
                last = p_tmp;
                W[w_index++] = PW[i];
            }
            P[last+1] = w_index+1;
        }

        void write_hash(FILE *out)
        {
            // fprintf(out, "%lu|", (unsigned long)w_index);
            //write size info
            fwrite(&PointerListLen, sizeof(size_t), 1, out);
            fwrite(&WindowListLen, sizeof(size_t), 1, out);
            fwrite(P, sizeof(uint32_t), p_len, out);


            fwrite(&w_index, sizeof(uint32_t), 1, out);
            // std::cout << w_index << std::endl;
            fwrite(W, sizeof(uint32_t), w_index, out);
        }

        int read_hash(FILE *in)
        {
            //read size info
            size_t tmp_p, tmp_w, tmp;
            tmp = fread(&tmp_p, sizeof(size_t), 1, in);
            tmp = fread(&tmp_w, sizeof(size_t), 1, in);
            printf("In hase table: PointerListLen: %lu\t WindowListLen: %lu\n",  (unsigned long)tmp_p, (unsigned long)tmp_w);
            if (tmp_p!= PointerListLen || tmp_w != WindowListLen)
            {
                fprintf(stderr, "window size error, the hash table have p:%lu and w:%lu\n while the input have:\
                    p:%lu w:%lu\n", (unsigned long)tmp_p, (unsigned long)tmp_w, (unsigned long)PointerListLen, (unsigned long)WindowListLen);
                return 1;
            }

            tmp = fread(P, sizeof(uint32_t), p_len, in);
            tmp = fread(&w_index, sizeof(uint32_t), 1, in);
            // std::cout << w_index << std::endl;
            tmp = fread(W, sizeof(uint32_t), w_index, in);
        }

        
        void write_hash_test(std::ofstream &out)
        {
            for (size_t i = 0; i<p_len-1; ++i)
            {
                if (P[i] > 0 && P[i+1] > 0 && P[i+1] > P[i])
                {
                    // out << std::bitset<32>(i);
                    out << to_string((uint32_t)i);

                    for (size_t j = P[i]-1; j < P[i+1]-1; ++j)
                    {
                        out << " " << W[j];
                    }
                    out << "\n";
                }
            }
        }
};