#include "vec4state.h"
#include "math.h"
#include <iostream>

using namespace std;

long long calcVectorSize(long long numBits) {
    return (numBits + 31) / 32;
}

void vec4state::setUnknown() {
    for (int i = 0; i < vectorSize; i++) {
        if (vector[i].getBval() != 0) {
            unknown = true;
            return;
        }
    }
    unknown = false;
}

void vec4state::incNumBits(long long newNumBits) {
    if (newNumBits <= 0) {
        throw string("Number of bits must be positive");
    }
    if (newNumBits < numBits) {
        throw string("Number of bits must be greater than the current number of bits");
    }
    if (newNumBits == numBits) return;
    long long oldVectorSize = vectorSize;
    numBits = newNumBits;
    vectorSize = calcVectorSize(numBits);
    VPI* newVector = new VPI[vectorSize];
    for (long long i = 0; i < vectorSize; i++) {
        if (i < oldVectorSize) {
            newVector[i] = vector[i];
        } else {
            newVector[i].setAval(0);
            newVector[i].setBval(0);
        }
    }
    delete[] vector;
    vector = newVector;
}

void vec4state::decNumBits(long long newNumBits) {
    if (newNumBits < 0) {
        throw string("Number of bits must be non-negative");
    }
    if (newNumBits > numBits) {
        throw string("Number of bits must be less than the current number of bits");
    }
    if (newNumBits == numBits) return;
    if (newNumBits == 0) {
        *this = vec4state("x", 1);
        return;
    }
    long long indexLastCell = ((newNumBits + 31) / 32) - 1;
    long long offset = newNumBits % 32;
    long long mask = (long long)(pow(2, offset) - 1);
    numBits = newNumBits;
    vectorSize = calcVectorSize(numBits);
    VPI* newVector = new VPI[vectorSize];
    for (long long i = 0; i <= indexLastCell; i++) {
        // If the current cell is still in range.
        if (i < indexLastCell) {
            newVector[i] = vector[i];
        }
        // If the last cell needs to be truncated in the middle
        else if (offset != 0) {
            newVector[i].setAval(vector[i].getAval() & mask);
            newVector[i].setBval(vector[i].getBval() & mask);
        }
    }
    delete[] vector;
    vector = newVector;
    if (unknown) setUnknown();
}

long long vec4state::convertVectorToLongLong() const {
    if (vectorSize > 2) {
        throw string("Cannot convert a vector that stores more than 64 bits to long long number");
    }
    if (unknown) {
        throw string("Cannot convert unknown vector to long long number");
    }
    long long result = 0;
    for (long long i = min(vectorSize, long long(2)); i >= 0; i--) {
        result = result << 32;
        result += vector[i].getAval();
    }
    return result;
}

vec4state vec4state::bitwiseAndAvalBval(const vec4state& other) {
    vec4state copy_this = *this;
    vec4state copy_other = other;
    long long maxSize = max(numBits, other.numBits);
    copy_this.setNumBits(maxSize);
    copy_other.setNumBits(maxSize);
    vec4state result = vec4state("0", maxSize);
    for (int i = 0; i < result.vectorSize; i++) {
        result.vector[i].setAval(copy_this.vector[i].getAval() & copy_other.vector[i].getAval());
        result.vector[i].setBval(copy_this.vector[i].getBval() & copy_other.vector[i].getBval());
    }
    result.setUnknown();
    return move(result);
}

vec4state vec4state::AdditionAvalBval(const vec4state& other) const {
    vec4state copy_this = *this;
    vec4state copy_other = other;
    long long maxSize = max(numBits, other.numBits);
    copy_this.incNumBits(maxSize);
    copy_other.incNumBits(maxSize);
    vec4state result = vec4state("0", maxSize);
    for (long long i = 0; i < result.vectorSize; i++) {
        result.vector[i].setAval(copy_this.vector[i].getAval() + copy_other.vector[i].getAval());
        result.vector[i].setBval(copy_this.vector[i].getBval() + copy_other.vector[i].getBval());
    }
    result.setUnknown();
    return move(result);
}

void vec4state::setNumBits(long long newNumBits) {
    if (newNumBits < 0) {
        throw string("Number of bits must be non-negative");
    }
    // If need to extend the vector
    if (newNumBits > numBits) {
        incNumBits(newNumBits);
    }
    // If need to truncate the vector
    if (newNumBits < numBits) {
        decNumBits(newNumBits);
    }
}

vec4state vec4state::getPartValidRange(long long end) const {
    if (end < 0) throw string("end must be non-negative");
    if (end > numBits) {
        throw string("end must be less than the number of bits in the vector");
    }
    vec4state result = vec4state("z", end);
    for (long long i = 0; i < result.vectorSize; i++) {
        result.vector[i].setAval(vector[i].getAval() & result.vector[i].getAval());
        result.vector[i].setBval(vector[i].getBval() & result.vector[i].getBval());
    }
    result.setUnknown();
    return move(result);
}

vec4state::vec4state() : vector(nullptr) {
    vector = new VPI[1];
    numBits = 1;
    vectorSize = 1;
    vector[0].setBval(1);
    unknown = false;
}

vec4state::vec4state(unsigned int num) : vector(nullptr) {
    vector = new VPI[1];
    numBits = 32;
    vectorSize = 1;
    vector[0].setAval(num);
    vector[0].setBval(0);
    unknown = false;
}

vec4state::vec4state(int num) : vector(nullptr) {
    vector = new VPI[1];
    numBits = 32;
    vectorSize = 1;
    vector[0].setAval(num);
    vector[0].setBval(0);
    unknown = false;
}

vec4state::vec4state(unsigned long long num) : vector(nullptr) {
    vector = new VPI[2];
    numBits = 64;
    vectorSize = 2;
    vector[0].setAval(uint32_t(num));
    vector[1].setAval(uint32_t(num >> 32));
    vector[0].setBval(0);
    vector[1].setBval(0);
    unknown = false;
}

vec4state::vec4state(long long num) : vector(nullptr) {
    vector = new VPI[2];
    numBits = 64;
    vectorSize = 2;
    long long mask = 0xFFFFFFFF;
    vector[0].setAval(uint32_t(num & mask));
    vector[1].setAval(uint32_t(num >> 32));
    vector[0].setBval(0);
    vector[1].setBval(0);
    unknown = false;
}

// Helper function for filling the remainder of the string into the MSB chunk in the vector.
// Returns the index of the string where the function stopped filling the vector.
int fillMSBChunk(VPI* vector, const string& str, long long cellSize, long long cellIndex) {
    int strIndex = 0;
    uint32_t aval = 0;
    uint32_t bval = 0;
    for (long long i = 0; i < cellSize; i++) {
        switch(str[strIndex]) {
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
        if (i < cellSize - 1) {
            aval = aval << 1;
            bval = bval << 1;
        }
        strIndex++;
    }
    vector[cellIndex].setAval(aval);
    vector[cellIndex].setBval(bval);
    return strIndex;
}

vec4state::vec4state(string str) : vector(nullptr) {
    numBits = str.length();
    vectorSize = calcVectorSize(numBits);
    if (numBits == 0) {
        (*this) = vec4state("x", 1);
        return;
    }
    vector = new VPI[vectorSize];
    bool dividedBy32 = (numBits % 32 == 0);
    int strIndex = 0;
    for (long long i = vectorSize - 1; i >= 0; i--) {
        uint32_t aval = 0;
        uint32_t bval = 0;
        if (i == vectorSize - 1 && !dividedBy32) {
            strIndex = fillMSBChunk(vector, str, numBits % 32, i);
            continue;
        }

        for (int j = 0; j < 32; j++) {
            switch(str[strIndex]) {
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
                default:
                    delete vector;
                    throw string("Invalid character in the string");
            }
            if (j < 31) {
                aval <<= 1;
                bval <<= 1;
            }
            strIndex++;
        }
        vector[i].setAval(aval);
        vector[i].setBval(bval);
    }
    setUnknown();
}
    
vec4state::vec4state(string str, long long numBits) : vector(nullptr) {
    if (str.length() == 0 || str.length() > 1) {
        throw string("String must contain exactly one character");
    }
    if (numBits <= 0) {
        throw string("Number of bits must be greater than 0");
    }
    string result_str;
    for (int i = 0; i < numBits; i++) {
        result_str += str;
    }
    *this = vec4state(result_str);
}

vec4state::vec4state(const vec4state& other) : numBits(other.numBits), vectorSize(other.vectorSize), unknown(other.unknown), vector(nullptr) {
    vector = new VPI[other.vectorSize];
    for (int i = 0; i < other.vectorSize; i++) {
        vector[i] = other.vector[i];
    }
}

vec4state::vec4state(vec4state&& other) noexcept : numBits(other.numBits), vectorSize(other.vectorSize), unknown(other.unknown), vector(other.vector) {
    other.vector = nullptr;
}

vec4state::~vec4state() {
    delete[] vector;
}

vec4state& vec4state::operator=(const vec4state& other) {
    if (this == &other) return *this;
    delete[] vector;
    numBits = other.numBits;
    vectorSize = other.vectorSize;
    unknown = other.unknown;
    vector = new VPI[vectorSize];
    for (int i = 0; i < vectorSize; i++) {
        vector[i] = other.vector[i];
    }
    return *this;
}

vec4state vec4state::operator&(const vec4state& other) const {
    vec4state copy_this = *this;
    vec4state copy_other = other;
    copy_this.setNumBits(max(numBits, other.numBits));
    copy_other.setNumBits(max(numBits, other.numBits));
    vec4state result = ~(~copy_this | ~copy_other);
    result.setUnknown();
    return move(result);
}

vec4state vec4state::operator|(const vec4state& other) const {
    vec4state copy_this = *this;
    vec4state copy_other = other;
    long long maxVectorSize = max(vectorSize, other.vectorSize);
    long long maxNumBits = max(numBits, other.numBits);
    vec4state result = vec4state("0", maxNumBits);
    copy_this.setNumBits(maxNumBits);
    copy_other.setNumBits(maxNumBits);
    for (long long i = 0; i < maxVectorSize; i++) {
        copy_this.vector[i].setAval(copy_this.vector[i].getAval() - (copy_this.vector[i].getAval() & copy_this.vector[i].getBval()));
        copy_other.vector[i].setAval(copy_other.vector[i].getAval() - (copy_other.vector[i].getAval() & copy_other.vector[i].getBval()));
    }
    for (long long i = 0; i < maxVectorSize; i++) {
        copy_this.vector[i].setBval(copy_this.vector[i].getBval() - (copy_other.vector[i].getAval() & copy_this.vector[i].getBval()));
        copy_other.vector[i].setBval(copy_other.vector[i].getBval() - (copy_this.vector[i].getAval() & copy_other.vector[i].getBval()));
    }
    for (long long i = 0; i < maxVectorSize; i++) {
        result.vector[i].setAval(copy_this.vector[i].getAval() | copy_other.vector[i].getAval());
        result.vector[i].setBval(copy_this.vector[i].getBval() | copy_other.vector[i].getBval());
    }
    result.setUnknown();
    return move(result);
}

vec4state vec4state::operator^(const vec4state& other) const {
    vec4state copy_this = *this;
    vec4state copy_other = other;
    copy_this.setNumBits(max(numBits, other.numBits));
    copy_other.setNumBits(max(numBits, other.numBits));
    return ((copy_this & ~copy_other) | (~copy_this & copy_other));
}

// Helper function for zeroing down the bits that are out of range while the vector stays the same size.
void zeroDownOutOfRangeBits(VPI* vector, long long vectorSize, long long numBits) {
    long long indexLastCell = ((numBits + 31) / 32) - 1;
    long long offset = numBits % 32;
    long long mask = (long long)(pow(2, offset) - 1);
    for (long long i = indexLastCell; i < vectorSize; i++) {
        if (i == indexLastCell) {
            if (offset) {
                // If the last relevant cell needs to be truncated in the middle.
                vector[i].setAval(vector[i].getAval() & mask);
                vector[i].setBval(vector[i].getBval() & mask);
            }
        } else {
            vector[i].setAval(0);
            vector[i].setBval(0);
        }
    }
}

vec4state vec4state::operator~() const {
    vec4state result = *this;
    for (int i = 0; i < vectorSize; i++) {
        result.vector[i].setAval(~(result.vector[i].getAval() | result.vector[i].getBval()));
    }
    result.setUnknown();
    zeroDownOutOfRangeBits(result.vector, result.vectorSize, result.numBits);
    return move(result);
}

vec4state vec4state::operator==(const vec4state& other) const {
    vec4state xorVector = *this ^ other;
    for (long long i = 0; i < xorVector.vectorSize; i++) {
        if (xorVector.vector[i].getAval() != 0) {
            return vec4state("0", 1);
        }
    }
    for (long long i = 0; i < xorVector.vectorSize; i++) {
        if (xorVector.vector[i].getBval() != 0) {
            return vec4state("x", 1);
        }
    }
    return vec4state("1", 1);
}

vec4state vec4state::operator!=(const vec4state& other) const {
    return !(*this == other);
}

vec4state vec4state::caseEquality(const vec4state& other) const {
    for (int i = 0; i < min(vectorSize, other.vectorSize); i++) {
        if (vector[i].getAval() != other.vector[i].getAval() || vector[i].getBval() != other.vector[i].getBval()) {
            return vec4state("0", 1);
        }
    }
    // If the vectors are of different sizes, check if the larger vector has any bits set to 1
    if (vectorSize > other.vectorSize) {
        for (long long i = other.vectorSize; i < vectorSize; i++) {
            if (vector[i].getAval() != 0 || vector[i].getBval() != 0) {
                return vec4state("0", 1);
            }
        }
    } else if (vectorSize < other.vectorSize) {
        for (long long i = vectorSize; i < other.vectorSize; i++) {
            if (other.vector[i].getAval() != 0 || other.vector[i].getBval() != 0) {
                return vec4state("0", 1);
            }
        }
    }
    return vec4state("1", 1);
}

vec4state vec4state::caseInequality(const vec4state& other) const {
    return !caseEquality(other);
}

vec4state vec4state::operator<<(const vec4state& other) {
    if (other.unknown) {
        return vec4state("x", numBits);
    }
    for (long long i = other.vectorSize - 1; i >= 2; i--) {
        if (other.vector[i].getAval() != 0) {
            return vec4state("0", numBits);
        }
    }
    long long num = 0;
    for (long long i = min(other.vectorSize - 1, (long long)(1)); i >= 0; i--) {
        num = num << 32;
        num += other.vector[i].getAval();
    }
    if (num < 0) {
        return vec4state("0", numBits);
    }
    vec4state result = *this << num;
    result.setUnknown();
    return move(result);
}

vec4state vec4state::operator<<(const long long num) {
    if (num == 0) return *this;
    if (num < 0) {
        return vec4state("0", numBits);
    }
    if (num >= numBits) {
        return vec4state("0", numBits);
    }
    vec4state res = *this;
    long long offset = num / 32;
    // Shifting whole cells
    if (offset > 0) {
        for (long long i = vectorSize - offset - 1; i >= 0; i--) {
            (res.vector[i + offset]).setAval(vector[i].getAval());
            (res.vector[i + offset]).setBval(vector[i].getBval());
            res.vector[i].setAval(0);
            res.vector[i].setBval(0);
        }
    }
    // Shifting the remaining bits
    for (long long i = vectorSize - 1; i >= 0; i--) {
        res.vector[i].setAval(res.vector[i].getAval() << (num % 32));
        res.vector[i].setBval(res.vector[i].getBval() << (num % 32));
        if (i > 0) {
            res.vector[i].setAval(res.vector[i].getAval() | (res.vector[i - 1].getAval() >> (32 - (num % 32))));
            res.vector[i].setBval(res.vector[i].getBval() | (res.vector[i - 1].getBval() >> (32 - (num % 32))));
        }
    }
    res.setUnknown();
    return move(res);
}

vec4state vec4state::operator>>(const vec4state& other) {
    if (other.unknown) return vec4state("x", numBits);
    for (long long i = other.vectorSize - 1; i >= 2; i--) {
        if (other.vector[i].getAval() != 0) {
            return vec4state("0", numBits);
        }
    }
    long long num = 0;
    for (long long i = min(other.vectorSize - 1, (long long)(1)); i >= 0; i--) {
        num = num << 32;
        num += other.vector[i].getAval();
    }
    if (num < 0) {
        return vec4state("0", numBits);
    }
    vec4state result = *this >> num;
    result.setUnknown();
    return move(result);
}

vec4state vec4state::operator>>(const long long num) {
    if (num == 0) return *this;
    if (num < 0) {
        return vec4state("0", numBits);
    }
    if (num >= numBits) {
        return vec4state("0", numBits);
    }
    vec4state res = *this;
    long long offset = num / 32;
    // Shifting whole cells
    if (offset > 0) {
        for (long long i = offset; i < vectorSize; i++) {
            res.vector[i - offset].setAval(vector[i].getAval());
            res.vector[i - offset].setBval(vector[i].getBval());
            res.vector[i].setAval(0);
            res.vector[i].setBval(0);
        }
    }
    // Shifting the remaining bits
    for (int i = 0; i < vectorSize; i++) {
        res.vector[i].setAval(res.vector[i].getAval() >> (num % 32));
        res.vector[i].setBval(res.vector[i].getBval() >> (num % 32));
        if (i < vectorSize - 1) {
            res.vector[i].setAval(res.vector[i].getAval() | (res.vector[i + 1].getAval() << (32 - (num % 32))));
            res.vector[i].setBval(res.vector[i].getBval() | (res.vector[i + 1].getBval() << (32 - (num % 32))));
        }
    }
    res.setUnknown();
    return move(res);
}

vec4state vec4state::getBitSelect(const vec4state& index) const {
    if (index.unknown || index > vec4state(numBits) || index < vec4state(0)) {
        return vec4state("x", 1);
    } else {
        return getPartSelect(index.convertVectorToLongLong(), index.convertVectorToLongLong());
    }
}

void vec4state::setBitSelect(const vec4state& index, const vec4state& newValue) {
    if (index.unknown || index > vec4state(numBits) || index < vec4state(0)) {
        return;
    }
    setPartSelect(index.convertVectorToLongLong(), index.convertVectorToLongLong(), newValue);
}

vec4state vec4state::getPartSelect(long long end, long long start) const {
    // If input is invalid.
    if (end < start) {
        throw string("end must be greater than or equal to start");
    }
    // If the slice is completely out of range.
    if (end < 0 || start >= numBits) {
        return vec4state("x", end - start + 1);
    }

    vec4state result = vec4state("z", end - start + 1);
    vec4state wanted = *this;
    // Move the bits to the right if the slice starts after index 0.
    if (start >= 0) {
        wanted = wanted >> start;
    }
    wanted.getPartValidRange(min(end + 1, numBits));
    
    // Extract the relevant bits from the vector.
    result = result.bitwiseAndAvalBval(wanted);
    result.decNumBits(end - start + 1);

    // If the slice ends after the last index, put x's where the index is greater than the number of bits.
    if (end >= numBits) {
        result.setPartSelect(result.numBits - 1, numBits, vec4state("x", result.numBits - numBits)); 
    }

    // If the slice starts before the first index, put x's where the index is less than 0.
    if (start < 0) {
        // Move the bits to the left.
        result = result << (-start);
        // Put x's where the index is less than 0.
        result.setPartSelect(-start - 1, 0, vec4state("x", -start));
    }
    result.setUnknown();
    return move(result);
}

void vec4state::setPartSelect(long long end, long long start, const vec4state& other) {
    // If input is invalid.
    if (end < start) {
        throw string("end must be greater than or equal to start");
    }
    // If the slice is not completely out of range, perform the operation.
    if (end >= 0 && start < numBits) {
        long long oldSize = numBits;
        vec4state other_copy = other;
        
        // Resizing other to the size of the slice.
        other_copy.setNumBits(end - start + 1);
        // In case the slice starts before the first index, move the bits to the right.
        if (start < 0) {
            other_copy = other_copy >> (-start);
        }
        start = max(start, (long long)(0));
        end = min(end, numBits - 1);

        // Adjust the size of other to the size of the slice.
        other_copy.setNumBits(end - start + 1);

        // Save the bits before the slice.
        vec4state beforeStart = vec4state("0", 1);
        if (start > 0) {
            beforeStart = getPartValidRange(start);
        } 
        // Zero down the bits in the slice.
        *this = *this >> (end + 1);
        *this = *this << (end - start + 1);

        // Insert the bits of other into the slice.
        *this = AdditionAvalBval(other_copy);
        // Move the starting bits back to their original position.
        *this = *this << start;
        *this = AdditionAvalBval(beforeStart);
        setNumBits(oldSize);
    }
    if (other.unknown) setUnknown();
}

vec4state vec4state::operator&&(const vec4state& other) const {
    int first = 0, second = 0;
    if (vectorSize > other.vectorSize) {
        for (long long i = vectorSize - 1; i >= other.vectorSize; i--) {
            // Remove the bits of the z's
            uint32_t tmp = vector[i].getAval() - (vector[i].getAval() & vector[i].getBval());
            if (tmp != 0) {
                first = 1;
                break;
            }
        }
    } else if (vectorSize < other.vectorSize) {
        for (long long i = other.vectorSize - 1; i >= vectorSize; i--) {
            // Remove the bits of the z's
            uint32_t tmp = other.vector[i].getAval() - (other.vector[i].getAval() & other.vector[i].getBval());
            if (tmp != 0) {
                second = 1;
                break;
            }
        }
    }
    for (long long i = min(vectorSize, other.vectorSize) - 1; i >= 0; i--) {
        if (first == 1 || second == 1) {
            return vec4state("1", 1);
        }
        // Remove the bits of the z's
        uint32_t tmp1 = vector[i].getAval() - (vector[i].getAval() & vector[i].getBval());
        uint32_t tmp2 = other.vector[i].getAval() - (other.vector[i].getAval() & other.vector[i].getBval());
        if (tmp1 != 0) first = 1;
        if (tmp2 != 0) second = 1;
    }
    // After checking all the bits

    // If both vector hava at least one bit set to 1
    if (first == 1 && second == 1) {
        return vec4state("1", 1);
    }

    // If at least one of the vectors has all bits set to 0
    if (first == 0 && !unknown) {
        return vec4state("0", 1);
    }
    if (second == 0 && !other.unknown) {
        return vec4state("0", 1);
    }

    // If at one of the vectors has at least one bit set to 1 and the other is unknown
    return vec4state("x", 1);
}

vec4state vec4state::operator||(const vec4state& other) const {
    if (vectorSize > other.vectorSize) {
        for (long long i = vectorSize - 1; i >= other.vectorSize; i--) {
            // Remove the bits of the z's
            uint32_t tmp = vector[i].getAval() - (vector[i].getAval() & vector[i].getBval());
            if (tmp != 0) {
                return vec4state("1", 1);
            }
        }
    } else if (vectorSize < other.vectorSize) {
        for (long long i = other.vectorSize - 1; i >= vectorSize; i--) {
            // Remove the bits of the z's
            uint32_t tmp = other.vector[i].getAval() - (other.vector[i].getAval() & other.vector[i].getBval());
            if (tmp != 0) {
                return vec4state("1", 1);
            }
        }
    }
    for (long long i = min(vectorSize, other.vectorSize) - 1; i >= 0; i--) {
        // Remove the bits of the z's
        uint32_t tmp1 = vector[i].getAval() - (vector[i].getAval() & vector[i].getBval());
        uint32_t tmp2 = other.vector[i].getAval() - (other.vector[i].getAval() & other.vector[i].getBval());
        if (tmp1 != 0 || tmp2 != 0) {
            return vec4state("1", 1);
        }
    }
    // After checking all the bits
    // Now we know that both vectors have all bits set to 0 or are unknown
    if (unknown || other.unknown) {
        return vec4state("x", 1);
    }

    // If both vectors have all bits set to 0
    return vec4state("0", 1);
}

vec4state vec4state::operator!() const {
    // If the vector has at least one bit set to 1, return 0.
    for (int i = 0; i < vectorSize; i++) {
        // Extract the bits that are 1.
        uint32_t tmp = vector[i].getAval() - (vector[i].getAval() & vector[i].getBval());
        if (tmp != 0) {
            return vec4state("0", 1);
        }
    }
    // If the vector has at least one bit set to x or z, return x.
    if (unknown) return vec4state("x", 1);
    // If the vector has only 0 bits, return 1.
    return vec4state("1", 1);
}

vec4state vec4state::operator<(const vec4state& other) const {
    if (unknown || other.unknown) {
        return vec4state("x", 1);
    }
    if (vectorSize > other.vectorSize) {
        for (long long i = vectorSize - 1; i >= other.vectorSize; i--) {
            if (vector[i].getAval() != 0) {
                return vec4state("0", 1);
            }
        }
    } else if (vectorSize < other.vectorSize) {
        for (long long i = other.vectorSize - 1; i >= vectorSize; i--) {
            if (other.vector[i].getAval() != 0) {
                return vec4state("1", 1);
            }
        }
    }
    for (long long i = min(vectorSize, other.vectorSize) - 1; i >= 0; i--) {
        if (vector[i].getAval() < other.vector[i].getAval()) {
            return vec4state("1", 1);
        }
        if (vector[i].getAval() > other.vector[i].getAval()) {
            return vec4state("0", 1);
        }
    }

    // If the vectors are equal
    return vec4state("0", 1);
}

vec4state vec4state::operator>(const vec4state& other) const {
    return other < *this;
}

vec4state vec4state::operator<=(const vec4state& other) const {
    return ~(*this > other);
}

vec4state vec4state::operator>=(const vec4state& other) const {
    return ~(*this < other);
}

vec4state vec4state::operator+(const vec4state& other) const {
    long long maxNumBits = max(numBits, other.numBits);
    if (unknown || other.unknown) {
        return vec4state("x", maxNumBits);
    }
    vec4state result = vec4state("0", maxNumBits);
    // Align the vectors to the same size.
    vec4state copy_this = *this;
    vec4state copy_other = other;
    copy_this.setNumBits(maxNumBits);
    copy_other.setNumBits(maxNumBits);
    long long carry = 0;
    for (long long i = 0; i < result.vectorSize; i++) {
        long long sum = unsigned long long(copy_this.vector[i].getAval()) + unsigned long long(copy_other.vector[i].getAval()) + carry;
        // Put in the result vector only the lower 32 bits of the sum.
        result.vector[i].setAval(uint32_t(sum & 0xFFFFFFFF));
        // If the sum is bigger than 32 bits, set the carry to 1 for the next iteration.
        if (sum > 0xFFFFFFFF) {
            carry = 1;
        } else {
            carry = 0;
        }
    }
    // If there is a carry in the last iteration, increase the size of the result vector by 1.
    if (carry) {
        result.incNumBits(maxNumBits + 1);
        result.vector[result.vectorSize - 1].setAval(1);
    }
    return move(result);
}

vec4state vec4state::operator-(const vec4state& other) const {
    long long maxNumBits = max(numBits, other.numBits);
    if (unknown || other.unknown) {
        return vec4state("x", maxNumBits);
    }
    vec4state result = vec4state("0", maxNumBits);
    // Align the vectors to the same size.
    vec4state copy_this = *this;
    vec4state copy_other = other;
    copy_this.setNumBits(maxNumBits);
    copy_other.setNumBits(maxNumBits);
    for (long long i = 0; i < copy_this.getVectorSize(); i++) {
        // If the current VPI in this vector is less than the corresponding one in other vector, borrow from the next VPI of this vector.
        if (copy_this.vector[i].getAval() < copy_other.vector[i].getAval()) {
            // Find the first next VPI in this vector that is not zero.
            long long firstNotZero = i + 1;
            while (copy_this.vector[firstNotZero].getAval() == 0 && firstNotZero < maxNumBits) {
                firstNotZero++;
            }
            if (firstNotZero != maxNumBits) {
                // If possible, borrow from the next VPI.
                copy_this.vector[firstNotZero].setAval(copy_this.vector[firstNotZero].getAval() - 1);
                firstNotZero--;
                for (; firstNotZero > i; firstNotZero--) {
                    copy_this.vector[firstNotZero].setAval(0xFFFFFFFF);
                }
                // Calculate the fixed value of the current VPI.
                result.vector[i].setAval(uint32_t(0x100000000 + copy_this.vector[i].getAval() - copy_other.vector[i].getAval()));
            } else {
                // If there is no VPI to borrow from, the result is negative.
                for (long long j = 0; j < maxNumBits; j++) {
                    result.vector[j].setAval(copy_this.vector[j].getAval() - copy_other.vector[j].getAval());
                }
                return move(result);
            }
        }
        // Calculate the result as usual.
        else {
            result.vector[i].setAval(copy_this.vector[i].getAval() - copy_other.vector[i].getAval());
        }
    }
    return move(result);
}

vec4state vec4state::operator*(const vec4state& other) const {
    long long maxNumBits = max(numBits, other.numBits);
    if (unknown || other.unknown) {
        return vec4state("x", maxNumBits);
    }
    vec4state result = vec4state("0", maxNumBits);
    // Align the vectors to the same size.
    vec4state copy_this = *this;
    vec4state copy_other = other;
    copy_this.setNumBits(maxNumBits);
    copy_other.setNumBits(maxNumBits);
    // for each VPI in this vector, multiply it by each VPI in other vector and add the result to the corresponding VPI in the result vector.
    long long carry = 0;
    for (int i_this = 0; i_this < maxNumBits; i_this++) {
        for (int i_other = 0; i_other < maxNumBits; i_other++) {
            long long mul = long long(copy_this.vector[i_this].getAval()) * long long(copy_other.vector[i_other].getAval());
            long long new_result = result.vector[max(i_this, i_other)].getAval() + mul + carry;
            if (new_result > 0xFFFFFFFF) {
                carry = new_result >> 32;
                new_result = new_result & 0xFFFFFFFF;
            } else {
                carry = 0;
            }
            // Add the lower 32 bits of the multiplication to the result vector.
            result.vector[max(i_this, i_other)].setAval(uint32_t(new_result));
        }
    }
    // If there is a carry in the last iteration, increase the size of the result vector by 1.
    if (carry) {
        result.incNumBits(maxNumBits + 1);
        result.vector[maxNumBits].setAval(uint32_t(carry));
    }
    return move(result);
}

vec4state vec4state::operator/(const vec4state& other) const {
    if (other == vec4state(0)) {
        throw string("Division by zero");
    }
    long long maxNumBits = max(numBits, other.numBits);
    if (unknown || other.unknown) {
        return vec4state("x", maxNumBits);
    }
    vec4state result = vec4state("0", maxNumBits);
    // Align the vectors to the same size.
    vec4state copy_this = *this;
    vec4state copy_other = other;
    copy_this.setNumBits(maxNumBits);
    copy_other.setNumBits(maxNumBits);
    // Divide the vectors using bit manipulation.
    while (copy_this >= copy_other) {
        long long shift = 0;
        while (copy_this > (copy_other << (shift + 1))) {
            shift++;
        }
        copy_this = copy_this - (copy_other << shift);
        result = result + (vec4state(1) << shift);
    }
    return move(result);
}

vec4state vec4state::operator%(const vec4state& other) const {
    if (other == vec4state(0)) {
        throw string("Division by zero");
    }
    long long maxNumBits = max(numBits, other.numBits);
    if (unknown || other.unknown) {
        return vec4state("x", maxNumBits);
    }
    // Align the vectors to the same size.
    vec4state copy_this = *this;
    vec4state copy_other = other;
    copy_this.setNumBits(maxNumBits);
    copy_other.setNumBits(maxNumBits);
    // Divide the vectors using bit manipulation and extract the remainder.
    while (copy_this >= copy_other) {
        long long shift = 0;
        while (copy_this > (copy_other << (shift + 1))) {
            shift++;
        }
        copy_this = copy_this - (copy_other << shift);
    }
    return move(copy_this);
}

vec4state vec4state::operator-() const {
    if (unknown) {
        return vec4state("x", numBits);
    }
    return ~*this + vec4state(1);
}

vec4state vec4state::power(const vec4state& other) const {
    vec4state result;
    // If the base or the power has an unknown value, the result is unknown.
    if (unknown || other.unknown) {
        result = vec4state("x", numBits);
    }
    // If the power is 0 or the base is 1, the result is 1.
    else if (other == vec4state(0) || *this == vec4state(1)) {
        result = vec4state(1);
        result.setNumBits(numBits);
    } else {
        result = *this;
        for (vec4state i = 2; i <= other; i = i + vec4state(1)) {
            result = result * *this;
        }
    }
    return move(result);
}

string vec4state::toString() const {
    string result;
    for (long long i = vectorSize - 1; i >= 0; i--) {
        uint32_t currAval = vector[i].getAval();
        uint32_t currBval = vector[i].getBval();
        for (int j = 31; j >= 0; j--) {
            if (i == vectorSize - 1 && numBits % 32 != 0 && j > (numBits % 32) - 1) {
                continue;
            }
            uint32_t mask = 1 << j;
            if ((currAval & mask) && (currBval & mask)) {
                result += "z";
            } else if (currAval & mask) {
                result += "1";
            } else if (currBval & mask) {
                result += "x";
            } else {
                result += "0";
            }
        }
    }
    return result;
}

vec4state::operator bool() const {
    // If the vector has at least one bit set to 1, return true.
    for (long long i = 0; i < vectorSize; i++) {
        uint32_t tmp = vector[i].getAval() - (vector[i].getAval() & vector[i].getBval());
        if (tmp != 0) {
            return true;
        }
    }
    // If the vector has all bits set to 0 or has unknown bits, return false.
    return false;
}

void vec4state::convertTo2State() {
    for (int i = 0; i < vectorSize; i++) {
        // Replace the z bits with x bits.
        vector[i].setAval(vector[i].getAval() - (vector[i].getAval() & vector[i].getBval()));
        // Zero down the unknown bits.
        vector[i].setBval(0);
    }
    unknown = false;
}

long long vec4state::getNumBits() const {
    return numBits;
}

VPI* vec4state::getVector() const {
    return vector;
}

long long vec4state::getVectorSize() const {
    return vectorSize;
}

bool vec4state::isUnknown() const {
    return unknown;
}