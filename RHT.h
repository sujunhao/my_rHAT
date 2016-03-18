#include <iostream>
#include <string>
#include <fstream>
#include <bitset>
#include <cstring>
#include <sstream>
#include <cstdlib>
#include <cstdio>
#include <vector>
#include <stdint.h>
using std::string;

size_t PointerListLen = 11;
size_t WindowListLen = 2048;

#define BASE_MASK 0x3

enum
{
    BASE_A = 0x0,
    BASE_C = 0x1,
    BASE_G = 0x2,
    BASE_T = 0x3,
};


//transformat form string to bitset
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
                std::cout << "invalid DNA base\n";
                return 0;

        }
    }
    return bitnum;
}

//transformat form bitset to string
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


//keep the RHT
class dna_bitset
{
    public:
        dna_bitset (size_t len) 
        {
            bit_len = len;
            m_len = 1 << (2 * bit_len);
            m_data = new std::vector<std::pair<uint32_t, uint32_t> >[m_len];
        }

        ~dna_bitset ()
        {
            delete [] m_data;
        }

        void link_string(string s, uint32_t dna_ref_index)
        {
            uint32_t n = to_bit(s), v = m_data[n].size();
            if (v > 0)
            {
                if (m_data[n][v - 1].first == dna_ref_index)
                    ++m_data[n][v - 1].second;
                else if (v > 1 && m_data[n][v - 2].first == dna_ref_index)
                    ++m_data[n][v - 2].second;
                else if (m_data[n][v-1].first > dna_ref_index)
                    m_data[n].insert(m_data[n].end() - 1, std::make_pair(dna_ref_index, 1));
                else
                    m_data[n].push_back(std::make_pair(dna_ref_index, 1));
            }
            else
                m_data[n].push_back(std::make_pair(dna_ref_index, 1));            
            //std::cout << s << " " << n << " " << v << " " << dna_ref_index << std::endl;
        }


        //write hash table to fstream
        void write_hash_out(std::ofstream &oo)
        {
            string ts;
            for (uint32_t i=0; i < m_len; ++i) 
            {
                ts = to_string(i);
                if (m_data[i].size() > 0)
                {
                    //int string
                    oo << ts;
                    // fprintf(oo, "%s", ts.c_str());

                    //in bitnum style
                    // oo << to_bit(ts);

                    for (size_t j=0; j < m_data[i].size(); ++j)
                    {
                        //write the hash table to file, the pointer string, the windowList index and occure time
                        // if (m_data[i][j].second  > 1)
                        // fprintf(oo, " %d %d", m_data[i][j].first, m_data[i][j].second);
                        // oo << " " << m_data[i][j].first <<  " " << m_data[i][j].second;

                        //the pointer string, the windowList index
                        // fprintf(oo, " %d", m_data[i][j].first);
                        oo << " " << m_data[i][j].first;
                    }
                    // fprintf(oo, "\n");
                    oo << std::endl;  
               }
            }
        }

        void write_hash_out(FILE *oo)
        {
            string ts;
            for (uint32_t i=0; i < m_len; ++i) 
            {
                ts = to_string(i);
                if (m_data[i].size() > 0)
                {
                    //int string
                    // oo << ts;
                    fprintf(oo, "%s", ts.c_str());

                    //in bitnum style
                    // oo << to_bit(ts);

                    for (size_t j=0; j < m_data[i].size(); ++j)
                    {
                        //write the hash table to file, the pointer string, the windowList index and occure time
                        // if (m_data[i][j].second  > 1)
                        // fprintf(oo, " %d %d", m_data[i][j].first, m_data[i][j].second);
                        // oo << " " << m_data[i][j].first <<  " " << m_data[i][j].second;

                        //the pointer string, the windowList index
                        fprintf(oo, " %d", m_data[i][j].first);
                        // oo << " " << m_data[i][j].first;
                    }
                    fprintf(oo, "\n");
                    // oo << std::endl;  
               }
            }
        }


        void read_hash_in(std::ifstream &ii)
        {
            string ss, line;
            uint32_t n, t;
            while (getline(ii, line))
            {
                std::istringstream ll(line);
                ll >> ss;
                n = to_bit(ss);
                while (ll >> t)
                {
                    m_data[n].push_back(std::make_pair(t, 1));
                }
            }
        }
    private:
        std::vector<std::pair<uint32_t, uint32_t> >* m_data;
        //the table list size  
        uint32_t m_len;
        //the table item size 11
        size_t bit_len;
};

