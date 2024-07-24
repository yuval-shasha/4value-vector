#include "vec4state.h"

vec4state::vec4state()
{
    vector = new VPI[1];
    size = 1;
}

vec4state::vec4state(int num)
{
    vector = new VPI[1];
    size = 32;
    vector[0].setAval(num);
    vector[0].setBval(0);
}

vec4state::vec4state(long num)
{
    vector = new VPI[2];
    size = 64;
    long mask = 0xFFFFFFFF;
    vector[0].setAval(num & mask);
    vector[1].setAval(num >> 32);
    vector[0].setBval(0);
    vector[1].setBval(0);
}

int fillMSBChunk(VPI* vector, const string& str, int cellSize, int cellIndex)
{
    int strIndex = 0;
    uint32_t aval = 0;
    uint32_t bval = 0;
    for (int i=0; i<cellSize; i++)
    {
        switch(str[strIndex])
        {
            case '0':
                break;
            case '1':
                aval += 1;
                break;
            case 'x':
                bval += 1;
                break;
            case 'z':
                aval += 1;
                bval += 1;
                break;
        }
        if (i < cellSize - 1)
        {
            aval <<= 1;
            bval <<= 1;
        }
        strIndex++;
    }
    vector[cellIndex].setAval(aval);
    vector[cellIndex].setBval(bval);
    return strIndex;
}

vec4state::vec4state(string str)
{
    vector = new VPI[(str.length() + 31) / 32];
    size = str.length();
    int vecSize = (str.length() + 31) / 32;
    bool dividedBy32 = (size % 32 == 0);
    int strIndex = 0;
    for (int i = vecSize - 1; i >= 0; i--)
    {
        uint32_t aval = 0;
        uint32_t bval = 0;
        if (i == vecSize - 1 && !dividedBy32)
        {
            strIndex = fillMSBChunk(vector, str, size % 32, i);
            continue;
        }

        for (int j = 0; j < 32; j++)
        {
            switch(str[strIndex])
            {
                case '0':
                    break;
                case '1':
                    aval += 1;
                    break;
                case 'x':
                    bval += 1;
                    break;
                case 'z':
                    aval += 1;
                    bval += 1;
                    break;
            }
            if (j < 31)
            {
                aval <<= 1;
                bval <<= 1;
            }
            strIndex++;
        }
        vector[i].setAval(aval);
        vector[i].setBval(bval);
    }
}
    
vec4state::vec4state(string str, size_t size)
{
    
}