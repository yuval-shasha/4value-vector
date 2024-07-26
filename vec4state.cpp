#include "vec4state.h"
#include "math.h"
        
bool vec4state::isUnknown() const {
    for (int i = 0; i < size; i++) {
        if (vector[i].getBval() != 0) return true;
    }
    return false;
}

int vec4state::getVectorSize() const {
    return (size + 31) / 32;
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
    size = str.length();
    vector = new VPI[getVectorSize()];
    int vecSize = getVectorSize();
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

vec4state::vec4state(const vec4state& other) : size(other.size), vector(new VPI[other.getVectorSize()]) {
    for (int i = 0; i < other.getVectorSize(); i++) {
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
    vector = new VPI[getVectorSize()];
    for (int i = 0; i < getVectorSize(); i++) {
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
    return ~(~*this | ~other);
}

vec4state vec4state::operator&(long num) const {
    return *this & vec4state(num);
}

vec4state vec4state::operator|(const vec4state& other) const {
    vec4state copy_this = *this;
    vec4state copy_other = other;
    vec4state result;
    for (int i = 0; i < getVectorSize(); i++) {
        copy_this.vector[i].setAval(copy_this.vector[i].getAval() - (copy_this.vector[i].getAval() & copy_this.vector[i].getBval()));
    }
    for (int i = 0; i < other.getVectorSize(); i++) {
        copy_other.vector[i].setAval(copy_other.vector[i].getAval() - (copy_other.vector[i].getAval() & copy_other.vector[i].getBval()));
    }
    for (int i = 0; i < min(getVectorSize(), other.getVectorSize()); i++) {
        copy_this.vector[i].setBval(copy_this.vector[i].getBval() - (copy_other.vector[i].getAval() & copy_this.vector[i].getBval()));
        copy_other.vector[i].setBval(copy_other.vector[i].getBval() - (copy_this.vector[i].getAval() & copy_other.vector[i].getBval()));
    }
    if (getVectorSize() > other.getVectorSize()) result = copy_this;
    else result = copy_other;
    for (int i = 0; i < min(getVectorSize(), other.getVectorSize()); i++) {
        result.vector[i].setAval(copy_this.vector[i].getAval() | copy_other.vector[i].getAval());
        result.vector[i].setBval(copy_this.vector[i].getBval() | copy_other.vector[i].getBval());
    }
    return result;
}

vec4state vec4state::operator|(long num) const {
    return *this | vec4state(num);
}

vec4state vec4state::operator^(const vec4state& other) const {
    return (*this & ~other) | (~*this & other);
}

vec4state vec4state::operator^(long num) const {
    return *this ^ vec4state(num);
}

vec4state vec4state::operator~() const {
    vec4state result = *this;
    for (int i = 0; i < getVectorSize(); i++) {
        result.vector[i].setAval(~(result.vector[i].getAval() | result.vector[i].getBval()));
    }
    return result;
}

vec4state vec4state::operator==(const vec4state& other) const {
    if (isUnknown() || other.isUnknown()) return vec4state("x", 1);
    if (getVectorSize() != other.getVectorSize()) return vec4state("0", 1);
    else {
        for (int i = 0; i < getVectorSize(); i++) {
            if (vector[i].getAval() != other.vector[i].getAval()) return vec4state("0", 1);
        }
        return vec4state("1", 1);
    }
}

vec4state vec4state::operator==(long num) const {
    return *this == vec4state(num);
}

vec4state vec4state::operator!=(const vec4state& other) const {
    if (isUnknown() || other.isUnknown()) return vec4state("x", 1);
    else return ~(*this == other);
}

vec4state vec4state::operator!=(long num) const {
    return *this != vec4state(num);
}

vec4state vec4state::caseEquality(const vec4state& other) const {
    if (getVectorSize() != other.getVectorSize()) return vec4state("0", 1);
    else {
        for (int i = 0; i < getVectorSize(); i++) {
            if (vector[i].getAval() != other.vector[i].getAval() || vector[i].getBval() != other.vector[i].getBval())   return vec4state("0", 1);
        }
        return vec4state("1", 1);
    }
}

vec4state vec4state::caseInequality(const vec4state& other) const {
    return ~caseEquality(other);
}

size_t vec4state::getSize() const {
    return size;
}

string vec4state::toString() const {
    
}