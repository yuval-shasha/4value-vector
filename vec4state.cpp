#include "vec4state.h"

uint32_t vec4state::getAvalAtIndex(unsigned int index) const {
    return vector[index].getAval();
}

void vec4state::setAvalAtIndex(unsigned int index, uint32_t new_val) {
    vector[index].setAval(new_val);
}
        
uint32_t vec4state::getBvalAtIndex(unsigned int index) const {
    return vector[index].getBval();
}
        
void vec4state::setBvalAtIndex(unsigned int index, uint32_t new_val) {
    vector[index].setBval(new_val);
}
        
bool vec4state::isUnknown() const {
    for (int i = 0; i < size; i++) {
        if (vector[i].getBval() != 0) return true;
    }
    return false;
}

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

// Helper function for filling the remainder of the string into the MSB chunk in the vector.
// Returns the index of the string where the function stopped filling the vector.
int fillMSBChunk(VPI* vector, const string& str, int cellSize, int cellIndex)
{
    int strIndex = 0;
    uint32_t aval = 0;
    uint32_t bval = 0;
    for (int i = 0; i < cellSize; i++)
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
    string result_str;
    for (int i = 0; i < size; i++) {
        result_str += str;
    }
    vec4state(result_str);
}

vec4state::vec4state(const vec4state& other) : size(other.size), vector(new VPI[(other.size + 31) / 32]) {
    for (int i = 0; i < size; i++) {
        vector[i] = other.vector[i];
    }
}

vec4state::~vec4state()
{
    delete[] vector;
}

vec4state& vec4state::operator=(const vec4state& other) {
    if (this == &other) return *this;
    delete[] vector;
    size = other.size;
    vector = new VPI[(size + 31) / 32];
    for (int i = 0; i < (size + 31) / 32; i++) {
        vector[i] = other.vector[i];
    }
    return *this;
}

vec4state& vec4state::operator=(int num) {
    *this = vec4state(num);
    return *this;
}

vec4state& vec4state::operator=(long num) {
    *this = vec4state(num);
    return *this;
}

vec4state& vec4state::operator=(string str) {
    *this = vec4state(str);
    return *this;
}

vec4state vec4state::operator&(const vec4state& other) const {

}

vec4state vec4state::operator&(long num) const {

}

vec4state vec4state::operator|(const vec4state& other) const {

}

vec4state vec4state::operator|(long num) const {

}

vec4state vec4state::operator^(const vec4state& other) const {

}

vec4state vec4state::operator^(long num) const {

}

vec4state vec4state::operator~() const {

}
