#include "vec4state.h"
#include "math.h"
#include <iostream>

using namespace std;
      
bool vec4state::isUnknown() const {
    for (int i = 0; i < getVectorSize(); i++) {
        if (vector[i].getBval() != 0) return true;
    }
    return false;
}

long long vec4state::getVectorSize() const {
    return (size + 31) / 32;
}

void vec4state::incSize(long long newSize) {
    if (newSize <= size) return;
    long long oldSize = getVectorSize();
    size = newSize;
    VPI* newVector = new VPI[getVectorSize()];
    for (long long i = 0; i < getVectorSize(); i++) {
        if (i < oldSize) newVector[i] = vector[i];
        else {
            newVector[i].setAval(0);
            newVector[i].setBval(0);
        }
    }
    delete[] vector;
    vector = newVector;
}

bool vec4state::isTrue() const {
    if (size == 1 && vector[0].getAval() == 1 && vector[0].getBval() == 0) return true;
    else return false;
}

long long vec4state::vecToLongLong() const {
    long long result = 0;
    for (int i = 0; i < 2; i++) {
        result = result << 32;
        result += vector[i].getAval();
    }
    return result;
}

bool vec4state::isNegative() const {
    return vector[getVectorSize() - 1].getAval() < 0;
}

vec4state::vec4state() : vector(nullptr)
{
    vector = new VPI[1];
    size = 1;
}

vec4state::vec4state(int num) : vector(nullptr)
{
    vector = new VPI[1];
    size = 32;
    vector[0].setAval(num);
    vector[0].setBval(0);
}

vec4state::vec4state(long long num) : vector(nullptr)
{
    vector = new VPI[2];
    size = 64;
    long long mask = 0xFFFFFFFF;
    vector[0].setAval(uint32_t(num & mask));
    vector[1].setAval(uint32_t(num >> 32));
    vector[0].setBval(0);
    vector[1].setBval(0);
}

// Helper function for filling the remainder of the string into the MSB chunk in the vector.
// Returns the index of the string where the function stopped filling the vector.
int fillMSBChunk(VPI* vector, const string& str, long long cellSize, long long cellIndex)
{
    int strIndex = 0;
    uint32_t aval = 0;
    uint32_t bval = 0;
    for (long long i = 0; i < cellSize; i++)
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
            aval = aval << 1;
            bval = bval << 1;
        }
        strIndex++;
    }
    vector[cellIndex].setAval(aval);
    vector[cellIndex].setBval(bval);
    return strIndex;
}

vec4state::vec4state(string str) : vector(nullptr)
{
    size = str.length();
    if (size == 0) {
        (*this) = vec4state("x", 1);
        return;
    }
    vector = new VPI[getVectorSize()];
    long long vecSize = getVectorSize();
    bool dividedBy32 = (size % 32 == 0);
    int strIndex = 0;
    for (long long i = vecSize - 1; i >= 0; i--)
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
    
vec4state::vec4state(string str, long long size) : vector(nullptr)
{
    string result_str;
    for (int i = 0; i < size; i++) {
        result_str += str;
    }
    *this = vec4state(result_str);
}

vec4state::vec4state(const vec4state& other) : size(other.size), vector(nullptr) {
    vector = new VPI[other.getVectorSize()];
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

vec4state& vec4state::operator=(long long num) {
    *this = vec4state(num);
    return *this;
}

vec4state& vec4state::operator=(string str) {
    *this = vec4state(str);
    return *this;
}

vec4state vec4state::operator&(const vec4state& other) const {
    vec4state copy_this = *this;
    vec4state copy_other = other;
    copy_this.incSize(max(getSize(), other.getSize()));
    copy_other.incSize(max(getSize(), other.getSize()));
    return ~(~copy_this | ~copy_other);
}

vec4state vec4state::operator&(long long num) const {
    return *this & vec4state(num);
}

vec4state vec4state::operator&(int num) const {
    return *this & vec4state(num);
}

vec4state vec4state::operator|(const vec4state& other) const {
    vec4state copy_this = *this;
    vec4state copy_other = other;
    long long biggerVectorSize = max(getVectorSize(), other.getVectorSize());
    vec4state result = vec4state("0", max(getSize(), other.getSize()));
    copy_this.incSize(max(getSize(), other.getSize()));
    copy_other.incSize(max(getSize(), other.getSize()));

    for (long long i = 0; i < biggerVectorSize; i++) {
        copy_this.vector[i].setAval(copy_this.vector[i].getAval() - (copy_this.vector[i].getAval() & copy_this.vector[i].getBval()));
        copy_other.vector[i].setAval(copy_other.vector[i].getAval() - (copy_other.vector[i].getAval() & copy_other.vector[i].getBval()));
    }
    
    for (long long i = 0; i < biggerVectorSize; i++) {
        copy_this.vector[i].setBval(copy_this.vector[i].getBval() - (copy_other.vector[i].getAval() & copy_this.vector[i].getBval()));
        copy_other.vector[i].setBval(copy_other.vector[i].getBval() - (copy_this.vector[i].getAval() & copy_other.vector[i].getBval()));
    }
    
    for (long long i = 0; i < biggerVectorSize; i++) {
        result.vector[i].setAval(copy_this.vector[i].getAval() | copy_other.vector[i].getAval());
        result.vector[i].setBval(copy_this.vector[i].getBval() | copy_other.vector[i].getBval());
    }
    return result;
}

vec4state vec4state::operator|(long long num) const {
    return *this | vec4state(num);
}

vec4state vec4state::operator|(int num) const {
    return *this | vec4state(num);
}

vec4state vec4state::operator^(const vec4state& other) const {
    vec4state copy_this = *this;
    vec4state copy_other = other;
    copy_this.incSize(max(getSize(), other.getSize()));
    copy_other.incSize(max(getSize(), other.getSize()));
    return (copy_this & ~copy_other) | (~copy_this & copy_other);
}

vec4state vec4state::operator^(long long num) const {
    return *this ^ vec4state(num);
}

vec4state vec4state::operator^(int num) const {
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
    vec4state xorVector = *this ^ other;
    for (long long i = 0; i < xorVector.getVectorSize(); i++) {
        if (xorVector.vector[i].getAval() != 0) return vec4state("0", 1);
        if (xorVector.vector[i].getBval() != 0) return vec4state("x", 1);
    }
    return vec4state("1", 1);
}

vec4state vec4state::operator==(long long num) const {
    return *this == vec4state(num);
}

vec4state vec4state::operator!=(const vec4state& other) const {
    return ~(*this == other);
}

vec4state vec4state::operator!=(long long num) const {
    return *this != vec4state(num);
}

vec4state vec4state::caseEquality(const vec4state& other) const {
    for (int i = 0; i < min(getVectorSize(), other.getVectorSize()); i++) {
        if (vector[i].getAval() != other.vector[i].getAval() || vector[i].getBval() != other.vector[i].getBval())
            return vec4state("0", 1);
    }
    
    // If the vectors are of different sizes, check if the larger vector has any bits set to 1
    if (getVectorSize() > other.getVectorSize()) {
        for (long long i = other.getVectorSize(); i < getVectorSize(); i++) {
            if (vector[i].getAval() != 0 || vector[i].getBval() != 0)
                return vec4state("0", 1);
        }
    } else if (getVectorSize() < other.getVectorSize()) {
        for (long long i = getVectorSize(); i < other.getVectorSize(); i++) {
            if (other.vector[i].getAval() != 0 || other.vector[i].getBval() != 0)
                return vec4state("0", 1);
        }
    }
    return vec4state("1", 1);
}

vec4state vec4state::caseEquality(long long num) const {
    return caseEquality(vec4state(num));
}

vec4state vec4state::caseInequality(const vec4state& other) const {
    return ~caseEquality(other);
}

vec4state vec4state::caseInequality(long long num) const {
    return caseInequality(vec4state(num));
}

vec4state vec4state::operator<<(const vec4state& other) {
    if (other.isUnknown()) return vec4state("x", size);
    for (long long i = other.getVectorSize() - 1; i >= 2; i--) {
        if (other.vector[i].getAval() != 0) return vec4state("0", size);
    }
    long long num = 0;
    for (int i = 1; i >= 0; i--) {
        num = num << 32;
        num += other.vector[i].getAval();
    }
    if (num < 0) return vec4state("0", size);
    return *this << num;
}

vec4state vec4state::operator<<(const long long num) {
    if (num < 0) return vec4state("0", size);
    if (num >= size) return vec4state("0", size);
    vec4state res = *this;
    long long offset = num / 32;

    // Shifting whole cells
    if (offset > 0) {
        for (long long i = getVectorSize() - offset - 1; i >= 0; i--)
        {
            res.vector[i + offset].setAval(vector[i].getAval());
            res.vector[i + offset].setBval(vector[i].getBval());
            res.vector[i].setAval(0);
            res.vector[i].setBval(0);
        }
    }
    // Shifting the remaining bits
    for (long long i = getVectorSize() - 1; i >= 0; i--)
    {
        res.vector[i].setAval(vector[i].getAval() << (num % 32));
        res.vector[i].setBval(vector[i].getBval() << (num % 32));
        if (i > 0)
        {
            res.vector[i].setAval(vector[i].getAval() | (vector[i - 1].getAval() >> (32 - (num % 32))));
            res.vector[i].setBval(vector[i].getBval() | (vector[i - 1].getBval() >> (32 - (num % 32))));
        }
    }
    return res;
}

vec4state vec4state::operator>>(const vec4state& other) {
    if (other.isUnknown()) return vec4state("x", size);
    for (long long i = other.getVectorSize() - 1; i >= 2; i--) {
        if (other.vector[i].getAval() != 0) return vec4state("0", size);
    }
    long long num = 0;
    for (int i = 1; i >= 0; i--) {
        num = num << 32;
        num += other.vector[i].getAval();
    }
    if (num < 0) return vec4state("0", size);
    return *this >> num;
}

vec4state vec4state::operator>>(const long long num) {
    if (num < 0) return vec4state("0", size);
    if (num >= size) return vec4state("0", size);
    vec4state res = *this;
    long long offset = num / 32;
    
    // Shifting whole cells
    for (long long i = offset; i < getVectorSize(); i++)
    {
        res.vector[i - offset].setAval(vector[i].getAval());
        res.vector[i - offset].setBval(vector[i].getBval());
        res.vector[i].setAval(0);
        res.vector[i].setBval(0);
    }

    // Shifting the remaining bits
    for (int i = 0; i < getVectorSize(); i++)
    {
        res.vector[i].setAval(vector[i].getAval() >> (num % 32));
        res.vector[i].setBval(vector[i].getBval() >> (num % 32));
        if (i < getVectorSize() - 1)
        {
            res.vector[i].setAval(vector[i].getAval() | (vector[i + 1].getAval() << (32 - (num % 32))));
            res.vector[i].setBval(vector[i].getBval() | (vector[i + 1].getBval() << (32 - (num % 32))));
        }
    }
    return res;
}

vec4state vec4state::getBitSelect(const vec4state& index) const {
    if (index.isUnknown() || (index > vec4state(size)).isTrue() || (index < 0).isTrue()) return vec4state("x", 1);
    else return getPartSelect(index.vecToLongLong(), index.vecToLongLong());
}

vec4state vec4state::getBitSelect(const long long index) const {
    return getBitSelect(vec4state(index));
}

void vec4state::setBitSelect(const vec4state& index, const vec4state& newValue) {
    if (index.isUnknown() || (index > vec4state(size)).isTrue() || (index < 0).isTrue()) return;
    setPartSelect(index.vecToLongLong(), index.vecToLongLong(), newValue);
}

void vec4state::setBitSelect(const long long index, const vec4state& newValue) {
    setBitSelect(vec4state(index), newValue);
}

void vec4state::setBitSelect(const vec4state& index, const long long newValue) {
    setBitSelect(index, vec4state(newValue));
}

void vec4state::setBitSelect(const long long index, const long long newValue) {
    setBitSelect(vec4state(index), vec4state(newValue));
}

vec4state vec4state::bitwiseAndAvalBval(const vec4state& other) const {
    vec4state copy_this = *this;
    vec4state copy_other = other;
    long long maxSize = max(getSize(), other.getSize());
    copy_this.incSize(maxSize);
    copy_other.incSize(maxSize);
    vec4state result = vec4state("0", maxSize);
    for (int i = 0; i < result.getVectorSize(); i++) {
        result.vector[i].setAval(result.vector[i].getAval() & other.vector[i].getAval());
        result.vector[i].setBval(result.vector[i].getBval() & other.vector[i].getBval());
    }
    return result;
}

vec4state vec4state::getPartSelect(long long end, long long start) const {
    // If input is invalid.
    if (end < start) return vec4state("x", 1);
    vec4state result = vec4state("x", end - start + 1);
    // If the slice is completely out of bounds.
    if (end < 0 || start >= size) return result;

    vec4state tmp = *this;
    if (start >= 0) {
        // Move the bits to the right if the slice starts after index 0.
        tmp = tmp >> start;
    }
    // Mask the bits that are in the slice.
    for (long long i = 0; i < (end - start + 1) / 32; i++) {
        result.vector[i].setAval(0xFFFFFFFF);
        result.vector[i].setBval(0xFFFFFFFF);
    }
    if ((end - start + 1) % 32 != 0) {
        result.vector[((end - start + 1) / 32) + 1].setAval(uint32_t(pow(2, (end % 32) + 1)) - 1);
        result.vector[((end - start + 1) / 32) + 1].setBval(uint32_t(pow(2, (end % 32) + 1)) - 1);
    }
    // Extract the relevant bits from the vector.
    result = result.bitwiseAndAvalBval(tmp);
    if (start < 0) {
        // Move the bits to the left if the slice starts becfore index 0.
        result = result << -start;
        // Put x's where the index is less than 0.
        for (long long i = 0; i < -start / 32; i++) result.vector[i].setBval(0xFFFFFFFF);
        if (-start % 32 != 0) result.vector[-start / 32].setBval(uint32_t(pow(2, -start % 32)) - 1 + result.vector[-start / 32].getBval());
    } 
    return result;
}

vec4state vec4state::AdditionAvalBval(const vec4state& other) const {
    vec4state copy_this = *this;
    vec4state copy_other = other;
    long long maxSize = max(getSize(), other.getSize());
    copy_this.incSize(maxSize);
    copy_other.incSize(maxSize);
    vec4state result = vec4state("0", maxSize);
    for (long long i = 0; i < result.getVectorSize(); i++) {
        result.vector[i].setAval(result.vector[i].getAval() + other.vector[i].getAval());
        result.vector[i].setBval(result.vector[i].getBval() + other.vector[i].getBval());
    }
    return result;
}

void vec4state::decSize(long long newSize) {
    if (newSize >= size) return;
    long long indexLastCell = ((newSize + 31) / 32) - 1;
    long long offset = newSize % 32;
    long long mask = long long(pow(2, offset) - 1);
    // Zero down the bits that are not in the new size.
    for (long long i = indexLastCell; i < getVectorSize(); i++) {
        // If the last cell needs to be truncated in the middle
        if (i == indexLastCell && offset != 0) {
            vector[i].setAval(vector[i].getAval() & mask);
            vector[i].setBval(vector[i].getBval() & mask);
        }
        // Zero down the bits that are not in the new size.
        else if (i > indexLastCell) {
            vector[i].setAval(0);
            vector[i].setBval(0);
        }
    }
    size = newSize;
}

vec4state vec4state::resize(long long newSize) const {
    vec4state result = *this;
    // If need to extend the vector
    if (newSize > size) result.incSize(newSize);
    // If need to truncate the vector
    if (newSize < size) result.decSize(newSize);
    return result;
}

void vec4state::setPartSelect(long long end, long long start, const vec4state& other) {
    if (start <= end && end < size && start >= 0) {
        start = max(start, long long(0));
        end = min(end, size - 1);
        // Adjust the size of other to the size of the slice.
        vec4state other_copy = other;
        other_copy.resize(end - start);
        // Save the bits before the slice.
        vec4state beforeStart = getPartSelect(start, 0);
        // Zero down the bits in the slice.
        *this = *this >> end;
        *this = *this << end - start;
        // Insert the bits of other into the slice.
        *this = AdditionAvalBval(other);
        // Move the starting bits back to their original position.
        *this = *this << start;
        *this = AdditionAvalBval(beforeStart);
    }
}

void vec4state::setPartSelect(long long end, long long start, long long num) {
    setPartSelect(end, start, vec4state(num));
}

void vec4state::setPartSelect(long long end, long long start, string str) {
    setPartSelect(end, start, vec4state(str));
}

vec4state vec4state::operator&&(const vec4state& other) const {
    int first = 0, second = 0;
    if (getVectorSize() > other.getVectorSize()) {
        for (long long i = getVectorSize() - 1; i >= other.getVectorSize(); i--) {
            // Remove the bits of the z's
            uint32_t tmp = vector[i].getAval() - (vector[i].getAval() & vector[i].getBval());
            if (tmp != 0){
                first = 1;
                break;
            }
        }
    }
    else if (getVectorSize() < other.getVectorSize()) {
        for (long long i = other.getVectorSize() - 1; i >= getVectorSize(); i--) {
            // Remove the bits of the z's
            uint32_t tmp = other.vector[i].getAval() - (other.vector[i].getAval() & other.vector[i].getBval());
            if (tmp != 0){
                second = 1;
                break;
            }
        }
    }
    for (long long i = min(getVectorSize(), other.getVectorSize()) - 1; i >= 0; i--) {
        if (first == 1 || second == 1) return vec4state("1", 1);
        // Remove the bits of the z's
        uint32_t tmp1 = vector[i].getAval() - (vector[i].getAval() & vector[i].getBval());
        uint32_t tmp2 = other.vector[i].getAval() - (other.vector[i].getAval() & other.vector[i].getBval());
        if (tmp1 != 0) first = 1;
        if (tmp2 != 0) second = 1;
    }
    // After checking all the bits

    // If both vector hava at least one bit set to 1
    if (first == 1 && second == 1) return vec4state("1", 1);

    // If at least one of the vectors has all bits set to 0
    if (first == 0 && !isUnknown()) return vec4state("0", 1);
    if (second == 0 && !other.isUnknown()) return vec4state("0", 1);

    // If at one of the vectors has at least one bit set to 1 and the other is unknown
    return vec4state("x", 1);
}

vec4state vec4state::operator&&(long long num) const {
    return *this && vec4state(num);
}

vec4state vec4state::operator||(const vec4state& other) const {
    if (getVectorSize() > other.getVectorSize()) {
        for (long long i = getVectorSize() - 1; i >= other.getVectorSize(); i--) {
            // Remove the bits of the z's
            uint32_t tmp = vector[i].getAval() - (vector[i].getAval() & vector[i].getBval());
            if (tmp != 0) return vec4state("1", 1);
        }
    }
    else if (getVectorSize() < other.getVectorSize()) {
        for (long long i = other.getVectorSize() - 1; i >= getVectorSize(); i--) {
            // Remove the bits of the z's
            uint32_t tmp = other.vector[i].getAval() - (other.vector[i].getAval() & other.vector[i].getBval());
            if (tmp != 0) return vec4state("1", 1);
        }
    }
    for (long long i = min(getVectorSize(), other.getVectorSize()) - 1; i >= 0; i--) {
        // Remove the bits of the z's
        uint32_t tmp1 = vector[i].getAval() - (vector[i].getAval() & vector[i].getBval());
        uint32_t tmp2 = other.vector[i].getAval() - (other.vector[i].getAval() & other.vector[i].getBval());
        if (tmp1 != 0) return vec4state("1", 1);
        if (tmp2 != 0) return vec4state("1", 1);
    }
    // After checking all the bits
    // Now we know that both vectors have all bits set to 0 or are unknown

    if (isUnknown() || other.isUnknown()) return vec4state("x", 1);

    // If both vectors have all bits set to 0
    return vec4state("0", 1);
}

vec4state vec4state::operator||(long long num) const {
    return *this || vec4state(num);
}

vec4state vec4state::operator!() const {
    for (int i = 0; i < getVectorSize(); i++) {
        uint32_t tmp = vector[i].getAval() - (vector[i].getAval() & vector[i].getBval());
        if (tmp != 0) return vec4state("0", 1);
    }
    // After checking all the bits
    // Now we know that the vector has all bits set to 0 or is unknown

    // If the vector has at least one bit set to x or z
    if (isUnknown()) return vec4state("x", 1);

    // If the vector has all bits set to 0
    return vec4state("1", 1);
}

vec4state vec4state::operator<(const vec4state& other) const {
    if (isUnknown() || other.isUnknown()) return vec4state("x", 1);
    if (getVectorSize() > other.getVectorSize()) {
        for (long long i = getVectorSize() - 1; i >= other.getVectorSize(); i--) {
            if (vector[i].getAval() != 0) return vec4state("0", 1);
        }
    }
    else if (getVectorSize() < other.getVectorSize()) {
        for (long long i = other.getVectorSize() - 1; i >= getVectorSize(); i--) {
            if (other.vector[i].getAval() != 0) return vec4state("1", 1);
        }
    }
    for (long long i = min(getVectorSize(), other.getVectorSize()) - 1; i >= 0; i--) {
        if (vector[i].getAval() < other.vector[i].getAval()) return vec4state("1", 1);
        if (vector[i].getAval() > other.vector[i].getAval()) return vec4state("0", 1);
    }

    // If the vectors are equal
    return vec4state("0", 1);
}

vec4state vec4state::operator<(long long num) const {
    return *this < vec4state(num);
}

vec4state vec4state::operator>(const vec4state& other) const {
    return other < *this;
}

vec4state vec4state::operator>(long long num) const {
    return *this > vec4state(num);
}

vec4state vec4state::operator<=(const vec4state& other) const {
    return ~(*this > other);
}

vec4state vec4state::operator<=(long long num) const {
    return *this <= vec4state(num);
}

vec4state vec4state::operator>=(const vec4state& other) const {
    return ~(*this < other);
}

vec4state vec4state::operator>=(long long num) const {
    return *this >= vec4state(num);
}

vec4state vec4state::operator+(const vec4state& other) const {
    long long maxSize = max(getSize(), other.getSize());
    if (isUnknown() || other.isUnknown()) return vec4state("x", maxSize);
    vec4state result = vec4state("0", maxSize);
    // Align the vectors to the same size.
    vec4state copy_this = *this;
    vec4state copy_other = other;
    copy_this.incSize(maxSize);
    copy_other.incSize(maxSize);
    long long carry = 0;
    for (long long i = 0; i < maxSize; i++) {
        long long sum = long long(copy_this.vector[i].getAval()) + long long(copy_other.vector[i].getAval()) + carry;
        // Put in the result vector only the lower 32 bits of the sum.
        result.vector[i].setAval(uint32_t(sum & 0xFFFFFFFF));
        // If the sum is bigger than 32 bits, set the carry to 1 for the next iteration.
        if (sum > 0xFFFFFFFF) carry = 1;
        else carry = 0;
    }
    // If there is a carry in the last iteration, increase the size of the result vector by 1.
    if (carry) {
        result.incSize(maxSize + 1);
        result.vector[maxSize].setAval(1);
    }
    return result;
}

vec4state vec4state::operator+(long long num) const {
    return *this + vec4state(num);
}

vec4state vec4state::operator-(const vec4state& other) const {
    long long maxSize = max(getSize(), other.getSize());
    if (isUnknown() || other.isUnknown()) return vec4state("x", maxSize);
    vec4state result = vec4state("0", maxSize);
    // Align the vectors to the same size.
    vec4state copy_this = *this;
    vec4state copy_other = other;
    copy_this.incSize(maxSize);
    copy_other.incSize(maxSize);
    for (long long i = 0; i < maxSize; i++) {
        // If the current VPI in this vector is less than the corresponding one in other vector, borrow from the next VPI of this vector.
        if (copy_this.vector[i].getAval() < copy_other.vector[i].getAval()) {
            // Find the first next VPI in this vector that is not zero.
            long long firstNotZero = i + 1;
            while (copy_this.vector[firstNotZero].getAval() == 0 && firstNotZero < maxSize) firstNotZero++;
            if (firstNotZero != maxSize) {
                // If possible, borrow from the next VPI.
                copy_this.vector[firstNotZero].setAval(copy_this.vector[firstNotZero].getAval() - 1);
                firstNotZero--;
                for (; firstNotZero > i; firstNotZero--) copy_this.vector[firstNotZero].setAval(0xFFFFFFFF);
                // Calculate the fixed value of the current VPI.
                result.vector[i].setAval(0x100000000 + copy_this.vector[i].getAval() - copy_other.vector[i].getAval());
            }
            else {
                // If there is no VPI to borrow from, the result is negative.
                for (long long j = 0; j < maxSize; j++) result.vector[j].setAval(copy_this.vector[j].getAval() - copy_other.vector[j].getAval());
                return result;
            }
        }
        // Calculate the result as usual.
        else result.vector[i].setAval(copy_this.vector[i].getAval() - copy_other.vector[i].getAval());
    }
    return result;
}

vec4state vec4state::operator-(long long num) const {
    return *this - vec4state(num);
}

vec4state vec4state::operator*(const vec4state& other) const {
    long long maxSize = max(getSize(), other.getSize());
    if (isUnknown() || other.isUnknown()) return vec4state("x", maxSize);
    vec4state result = vec4state("0", maxSize);
    // Align the vectors to the same size.
    vec4state copy_this = *this;
    vec4state copy_other = other;
    copy_this.incSize(maxSize);
    copy_other.incSize(maxSize);
    // for each VPI in this vector, multiply it by each VPI in other vector and add the result to the corresponding VPI in the result vector.
    long long carry = 0;
    for (int i_this = 0; i_this < maxSize; i_this++) {
        for (int i_other = 0; i_other < maxSize; i_other++) {
            long long mul = long long(copy_this.vector[i_this].getAval()) * long long(copy_other.vector[i_other].getAval());
            long long new_result = result.vector[max(i_this, i_other)].getAval() + mul + carry;
            if (new_result > 0xFFFFFFFF) {
                carry = new_result >> 32;
                new_result = new_result & 0xFFFFFFFF;
            }
            else carry = 0;
            // Add the lower 32 bits of the multiplication to the result vector.
            result.vector[max(i_this, i_other)].setAval(new_result);
        }
    }
    // If there is a carry in the last iteration, increase the size of the result vector by 1.
    if (carry) {
        result.incSize(maxSize + 1);
        result.vector[maxSize].setAval(carry);
    }
    return result;
}

vec4state vec4state::operator*(long long num) const {
    return *this * vec4state(num);
}

vec4state vec4state::operator/(const vec4state& other) const {
    long long maxSize = max(getSize(), other.getSize());
    if (isUnknown() || other.isUnknown()) return vec4state("x", maxSize);
    vec4state result = vec4state("0", maxSize);
    // Align the vectors to the same size.
    vec4state copy_this = *this;
    vec4state copy_other = other;
    copy_this.incSize(maxSize);
    copy_other.incSize(maxSize);
    // Divide the vectors using bit manipulation.
    while ((copy_this >= copy_other).isTrue()) {
        long long shift = 0;
        while ((copy_this > (copy_other << (shift + 1))).isTrue()) shift++;
        copy_this = copy_this - (copy_other << shift);
        result = result + (vec4state(1) << shift);
    }
    return result;
}

vec4state vec4state::operator/(long long num) const {
    return *this / vec4state(num);
}

vec4state vec4state::operator%(const vec4state& other) const {
    long long maxSize = max(getSize(), other.getSize());
    if (isUnknown() || other.isUnknown()) return vec4state("x", maxSize);
    // Align the vectors to the same size.
    vec4state copy_this = *this;
    vec4state copy_other = other;
    copy_this.incSize(maxSize);
    copy_other.incSize(maxSize);
    // Divide the vectors using bit manipulation and extract the remainder.
    while ((copy_this >= copy_other).isTrue()) {
        long long shift = 0;
        while ((copy_this > (copy_other << (shift + 1))).isTrue()) shift++;
        copy_this = copy_this - (copy_other << shift);
    }
    return copy_this;
}

vec4state vec4state::operator%(long long num) const {
    return *this % vec4state(num);
}

vec4state vec4state::operator-() const {
    if (isUnknown()) return vec4state("x", size);
    return ~*this + vec4state(1);
}

vec4state vec4state::power(const vec4state& other) const {
    vec4state result;
    if (isUnknown() || other.isUnknown() || ((*this == 0).isTrue() && other.isNegative())) result = vec4state("x", size);
    else if (((*this == -1).isTrue() && (other % 2 == 0).isTrue()) || (other == 0).isTrue() || (*this == 1).isTrue()) {
        result = vec4state(1);
        result.incSize(size);
    }
    else if ((*this == -1).isTrue() && (other % 2 == 1).isTrue()) result = vec4state("1", size);
    else if ((other.isNegative() && ((*this < -1).isTrue() || (*this > 1).isTrue())) || ((*this == 0).isTrue() && !other.isNegative())) result = vec4state("0", size);
    else {
        // If the power is positive and this vector is less than -1 or greater than 1:
        result = *this;
        for (vec4state i = 2; (i <= other).isTrue(); i = i + 1) result = result * *this;
    }
    return result;
}

vec4state vec4state::power(long long num) const {
    return power(vec4state(num));
}

/*********** For testing purposes ***********/

long long vec4state::getSize() const {
    return size;
}

string vec4state::toString() const {
    string result;
    for (long long i = getVectorSize() - 1; i >= 0; i--) 
    {
        uint32_t currAval = vector[i].getAval();
        uint32_t currBval = vector[i].getBval();
        for (int j = 31; j >= 0; j--) 
        {
            if (i == getVectorSize() - 1 && size % 32 != 0 && j > (size % 32) - 1) continue;
            uint32_t mask = 1 << j;
            if ((currAval & mask) && (currBval & mask)) 
                result += "z";
            else if (currAval & mask)
                result += "1";
            else if (currBval & mask)
                result += "x";
            else
                result += "0";
        }
    }
    return result;
}

