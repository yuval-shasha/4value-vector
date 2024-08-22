#include "vec4state.h"
#include "math.h"
#include <iostream>

#define BITS_IN_CELL 32
#define CELLS_IN_INDEX_VECTOR 2
#define MASK_32 0xFFFFFFFF
#define MASK_BIT_33 0x100000000 

using namespace std;

long long calcVectorSize(long long numBits) {
    return (numBits + BITS_IN_CELL - 1) / BITS_IN_CELL;
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
    if (newNumBits == numBits) {
        return;
    }
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
    if (newNumBits == numBits) {
        return;
    }
    if (newNumBits == 0) {
        *this = vec4state(X, 1);
        return;
    }
    long long indexLastCell = calcVectorSize(newNumBits) - 1;
    long long offset = newNumBits % BITS_IN_CELL;
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
    if (unknown) {
        setUnknown();
    }
}

long long vec4state::extractNumberFromVector() const {
    // TODO: is this correct?
    // If the vector is 0's after the 64th bit?
    if (vectorSize > CELLS_IN_INDEX_VECTOR) {
        throw string("Cannot convert a vector that stores more than 64 bits to long long number");
    }
    if (unknown) {
        throw string("Cannot convert unknown vector to long long number");
    }
    long long result = 0;
    for (long long i = min(vectorSize, long long(CELLS_IN_INDEX_VECTOR)) - 1; i >= 0; i--) {
        result = result << BITS_IN_CELL;
        result += vector[i].getAval();
    }
    return result;
}

vec4state vec4state::bitwiseAndAvalBval(const vec4state& other) {
    long long maxSize = max(numBits, other.numBits);
    vec4state result = vec4state(ZERO, maxSize);
    for (long long i = 0; i < result.vectorSize; i++) {
        // If the current VPI can be calculated with both vectors.
        if (i < this->vectorSize && i < other.vectorSize) {
            result.vector[i].setAval(this->vector[i].getAval() & other.vector[i].getAval());
            result.vector[i].setBval(this->vector[i].getBval() & other.vector[i].getBval());
        }
        // If reached end of one of the vectors, result is zero from then on.
        else {
            result.vector[i].setAval(0);
            result.vector[i].setBval(0);
        }
        // If the new bval is positive, result is unknown.
        if (result.vector[i].getBval() != 0) {
            result.unknown = true;
        }
    }
    return move(result);
}

vec4state vec4state::AdditionAvalBval(const vec4state& other) const {
    long long maxSize = max(numBits, other.numBits);
    vec4state result = vec4state(ZERO, maxSize);
    for (long long i = 0; i < result.vectorSize; i++) {
        // If the current VPI can be calculated with both vectors.
        if (i < this->vectorSize && i < other.vectorSize) {
            result.vector[i].setAval(this->vector[i].getAval() + other.vector[i].getAval());
            result.vector[i].setBval(this->vector[i].getBval() + other.vector[i].getBval());
        }
        // If reached end of this, copy only other's cells.
        else if (i < other.vectorSize) {
            result.vector[i].setAval(other.vector[i].getAval());
            result.vector[i].setBval(other.vector[i].getBval());
        }
        // If reached end of other, copy only this's cells.
        else {
            result.vector[i].setAval(this->vector[i].getAval());
            result.vector[i].setBval(this->vector[i].getBval());
        }
        // If the new bval is positive, result is unknown.
        if (result.vector[i].getBval() != 0) {
            result.unknown = true;
        }
    }
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
    if (end < 0) {
        throw string("end must be non-negative");
    }
    if (end > numBits) {
        throw string("end must be less than the number of bits in the vector");
    }
    vec4state result = vec4state(Z, end);
    for (long long i = 0; i < result.vectorSize; i++) {
        result.vector[i].setAval(vector[i].getAval() & result.vector[i].getAval());
        result.vector[i].setBval(vector[i].getBval() & result.vector[i].getBval());
        if (result.vector[i].getBval() != 0) {
            result.unknown = true;
        }
    }
    return move(result);
}

vec4state::vec4state() : vector(nullptr) {
    vector = new VPI[1];
    numBits = 1;
    vectorSize = 1;
    vector[0].setBval(1);
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
            case ZERO:
                break;
            case ONE:
                aval += 1;
                break;
            case X:
                bval += 1;
                break;
            case Z:
                aval += 1;
                bval += 1;
                break;
            default:
                throw string("Invalid bit MSB");
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

vec4state::vec4state(string str) : vec4state() {
    if (str.length() == 0) {
        return;
    }
    numBits = str.length();
    vectorSize = calcVectorSize(numBits);
    vector = new VPI[vectorSize];
    bool dividedBy32 = (numBits % BITS_IN_CELL == 0);
    int strIndex = 0;
    for (long long i = vectorSize - 1; i >= 0; i--) {
        uint32_t aval = 0;
        uint32_t bval = 0;
        // The last element gets a special case because when the c'tor receives a string, it reads the string's characters from the MSB to the LSB, translates them to VPIs, and stores them in the vector. If the string's length doesn't divide by 32, then the last VPI (where the 32 MSBs are stored) needs alignment to 32 bits (by padding it with zeroes).
        if (i == vectorSize - 1 && !dividedBy32) {
            try {
                strIndex = fillMSBChunk(vector, str, numBits % BITS_IN_CELL, i);
            } catch (string& e) {
                delete vector;
                throw e;
            }
            continue;
        }

        for (int j = 0; j < BITS_IN_CELL; j++) {
            switch(str[strIndex]) {
                case ZERO:
                    break;
                case ONE:
                    aval += 1;
                    break;
                case X:
                    bval += 1;
                    break;
                case Z:
                    aval += 1;
                    bval += 1;
                    break;
                default:
                    delete vector;
                    throw string("Invalid bit");
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
    
vec4state::vec4state(BitValue bit, long long numBits) : vec4state(string(numBits, bit)) {
    if (bit != ZERO && bit != ONE && bit != X && bit != Z) {
        throw string("Invalid bit");
    }
    if (numBits <= 0) {
        throw string("Number of bits must be greater than 0");
    }
}

vec4state::vec4state(const vec4state& other) : numBits(other.numBits), vectorSize(other.vectorSize), unknown(other.unknown), vector(nullptr) {
    vector = new VPI[vectorSize];
    for (int i = 0; i < vectorSize; i++) {
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
    if (this == &other) {
        return *this;
    }
    // Change the value of unknown because assigning another value.
    unknown = false;
    // Setting this's bits to the other's bits.
    for (long long i = 0; i < min(this->vectorSize, other.vectorSize); i++) {
        this->vector[i] = other.vector[i];
        // If the other's vector has unknown bits, this's vector has unknown bits.
        if (vector[i].getBval() != 0) {
            unknown = true;
        }
    }
    // In case the other's vector is shorter than this's vector:
    // put 0's in the remaining cells.
    if (other.vectorSize < vectorSize) {
        for (long long i = other.vectorSize; i < vectorSize; i++) {
            vector[i].setAval(0);
            vector[i].setBval(0);
        }
    }
    // Zero down the bits that are out of range.
    if (numBits % BITS_IN_CELL != 0) {
        vector[vectorSize - 1].setAval(vector[vectorSize - 1].getAval() & (MASK_32 >> (BITS_IN_CELL - (numBits % BITS_IN_CELL))));
        vector[vectorSize - 1].setBval(vector[vectorSize - 1].getBval() & (MASK_32 >> (BITS_IN_CELL - (numBits % BITS_IN_CELL))));
    }
    return *this;
}

vec4state vec4state::operator&(const vec4state& other) const {
    vec4state copyThis = *this;
    vec4state copyOther = other;
    copyThis.setNumBits(max(numBits, other.numBits));
    copyOther.setNumBits(max(numBits, other.numBits));
    vec4state tmp = (~copyThis | ~copyOther);
    vec4state result = ~(~copyThis | ~copyOther);
    return move(result);
}

vec4state vec4state::operator|(const vec4state& other) const {
    vec4state copyThis = *this;
    vec4state copyOther = other;
    long long maxVectorSize = max(vectorSize, other.vectorSize);
    long long maxNumBits = max(numBits, other.numBits);
    vec4state result = vec4state(ZERO, maxNumBits);
    copyThis.setNumBits(maxNumBits);
    copyOther.setNumBits(maxNumBits);
    for (long long i = 0; i < maxVectorSize; i++) {
        copyThis.vector[i].setAval(copyThis.vector[i].getAval() - (copyThis.vector[i].getAval() & copyThis.vector[i].getBval()));
        copyOther.vector[i].setAval(copyOther.vector[i].getAval() - (copyOther.vector[i].getAval() & copyOther.vector[i].getBval()));
    }
    for (long long i = 0; i < maxVectorSize; i++) {
        copyThis.vector[i].setBval(copyThis.vector[i].getBval() - (copyOther.vector[i].getAval() & copyThis.vector[i].getBval()));
        copyOther.vector[i].setBval(copyOther.vector[i].getBval() - (copyThis.vector[i].getAval() & copyOther.vector[i].getBval()));
    }
    for (long long i = 0; i < maxVectorSize; i++) {
        result.vector[i].setAval(copyThis.vector[i].getAval() | copyOther.vector[i].getAval());
        result.vector[i].setBval(copyThis.vector[i].getBval() | copyOther.vector[i].getBval());
        if (result.vector[i].getBval() != 0) {
            result.unknown = true;
        }
    }
    return move(result);
}

vec4state vec4state::operator^(const vec4state& other) const {
    vec4state copyThis = *this;
    vec4state copyOther = other;
    copyThis.setNumBits(max(numBits, other.numBits));
    copyOther.setNumBits(max(numBits, other.numBits));
    return ((copyThis & ~copyOther) | (~copyThis & copyOther));
}

// Helper function for zeroing down the bits that are out of range while the vector stays the same size.
void zeroDownOutOfRangeBits(VPI* vector, long long vectorSize, long long numBits) {
    long long indexLastCell = calcVectorSize(numBits) - 1;
    long long offset = numBits % BITS_IN_CELL;
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
    zeroDownOutOfRangeBits(result.vector, result.vectorSize, result.numBits);
    return move(result);
}

vec4state vec4state::operator==(const vec4state& other) const {
    vec4state xorVector = *this ^ other;
    for (long long i = 0; i < xorVector.vectorSize; i++) {
        if (xorVector.vector[i].getAval() != 0) {
            return vec4state(ZERO, 1);
        }
    }
    for (long long i = 0; i < xorVector.vectorSize; i++) {
        if (xorVector.vector[i].getBval() != 0) {
            return vec4state(X, 1);
        }
    }
    return vec4state(ONE, 1);
}

vec4state vec4state::operator!=(const vec4state& other) const {
    return !(*this == other);
}

// TODO: vectors are not equal if they have different number of bits, change tests accordingly.
vec4state vec4state::caseEquality(const vec4state& other) const {
    if (numBits != other.numBits) {
        return vec4state(ZERO, 1);
    }
    for (int i = 0; i < vectorSize; i++) {
        if (vector[i].getAval() != other.vector[i].getAval() || vector[i].getBval() != other.vector[i].getBval()) {
            return vec4state(ZERO, 1);
        }
    }
    return vec4state(ONE, 1);
}

vec4state vec4state::caseInequality(const vec4state& other) const {
    return !caseEquality(other);
}

vec4state vec4state::operator<<(const vec4state& other) {
    if (other.unknown) {
        return vec4state(X, numBits);
    }
    // If the value of other is greater than the possible number of bits in a vector, the returned value is only 0's.
    for (long long i = other.vectorSize - 1; i >= CELLS_IN_INDEX_VECTOR; i--) {
        if (other.vector[i].getAval() != 0) {
            return vec4state(ZERO, numBits);
        }
    }
    long long numOfThis = other.extractNumberFromVector();
    if (numOfThis < 0) {
        return vec4state(ZERO, numBits);
    }
    vec4state result = *this << numOfThis;
    return move(result);
}

vec4state vec4state::operator<<(const long long num) {
    if (num == 0) {
        return *this;
    }
    if (num < 0) {
        return vec4state(ZERO, numBits);
    }
    if (num >= numBits) {
        return vec4state(ZERO, numBits);
    }
    vec4state res = *this;
    long long offset = num / BITS_IN_CELL;
    // Shifting whole cells
    if (offset > 0) {
        for (long long i = vectorSize - offset - 1; i >= 0; i--) {
            (res.vector[i + offset]).setAval(vector[i].getAval());
            (res.vector[i + offset]).setBval(vector[i].getBval());
            if (res.vector[i + offset].getBval() != 0) {
                res.unknown = true;
            }
            res.vector[i].setAval(0);
            res.vector[i].setBval(0);
        }
    }
    // Shifting the remaining bits
    for (long long i = vectorSize - 1; i >= 0; i--) {
        res.vector[i].setAval(res.vector[i].getAval() << (num % BITS_IN_CELL));
        res.vector[i].setBval(res.vector[i].getBval() << (num % BITS_IN_CELL));
        if (i > 0) {
            res.vector[i].setAval(res.vector[i].getAval() | (res.vector[i - 1].getAval() >> (BITS_IN_CELL - (num % BITS_IN_CELL))));
            res.vector[i].setBval(res.vector[i].getBval() | (res.vector[i - 1].getBval() >> (BITS_IN_CELL - (num % BITS_IN_CELL))));
        }
        if (res.vector[i].getBval() != 0) {
            res.unknown = true;
        }
    }
    return move(res);
}

vec4state vec4state::operator>>(const vec4state& other) {
    if (other.unknown) {
        return vec4state(X, numBits);
    }
    // If the value of other is greater than the possible number of bits in a vector, the returned value is only 0's.
    for (long long i = other.vectorSize - 1; i >= CELLS_IN_INDEX_VECTOR; i--) {
        if (other.vector[i].getAval() != 0) {
            return vec4state(ZERO, numBits);
        }
    }
    long long numOfThis = other.extractNumberFromVector();
    if (numOfThis < 0) {
        return vec4state(ZERO, numBits);
    }
    vec4state result = *this >> numOfThis;
    return move(result);
}

vec4state vec4state::operator>>(const long long num) {
    if (num == 0) {
        return *this;
    }
    if (num < 0) {
        return vec4state(ZERO, numBits);
    }
    if (num >= numBits) {
        return vec4state(ZERO, numBits);
    }
    vec4state res = *this;
    long long offset = num / BITS_IN_CELL;
    // Shifting whole cells
    if (offset > 0) {
        for (long long i = offset; i < vectorSize; i++) {
            res.vector[i - offset].setAval(vector[i].getAval());
            res.vector[i - offset].setBval(vector[i].getBval());
            if (res.vector[i - offset].getBval() != 0) {
                res.unknown = true;
            }
            res.vector[i].setAval(0);
            res.vector[i].setBval(0);
        }
    }
    // Shifting the remaining bits
    for (int i = 0; i < vectorSize; i++) {
        res.vector[i].setAval(res.vector[i].getAval() >> (num % BITS_IN_CELL));
        res.vector[i].setBval(res.vector[i].getBval() >> (num % BITS_IN_CELL));
        if (i < vectorSize - 1) {
            res.vector[i].setAval(res.vector[i].getAval() | (res.vector[i + 1].getAval() << (BITS_IN_CELL - (num % BITS_IN_CELL))));
            res.vector[i].setBval(res.vector[i].getBval() | (res.vector[i + 1].getBval() << (BITS_IN_CELL - (num % BITS_IN_CELL))));
        }
        if (res.vector[i].getBval() != 0) {
            res.unknown = true;
        }
    }
    return move(res);
}

vec4state vec4state::getBitSelect(const vec4state& index) const {
    if (index.unknown || index > vec4state(numBits) || index < vec4state(0)) {
        return vec4state(X, 1);
    } else {
        long long bitIndex = index.extractNumberFromVector();
        return getPartSelect(bitIndex, bitIndex);
    }
}

void vec4state::setBitSelect(const vec4state& index, const vec4state& newValue) {
    if (index.unknown || index > vec4state(numBits) || index < vec4state(0)) {
        return;
    }
    long long bitIndex = index.extractNumberFromVector();
    setPartSelect(bitIndex, bitIndex, newValue);
}

vec4state vec4state::getPartSelect(long long end, long long start) const {
    // If input is invalid.
    if (end < start) {
        throw string("end must be greater than or equal to start");
    }
    // If the slice is completely out of range.
    if (end < 0 || start >= numBits) {
        return vec4state(X, end - start + 1);
    }

    vec4state result = vec4state(Z, end - start + 1);
    vec4state wanted = *this;
    // Move the bits to the right if the slice starts after index 0.
    if (start >= 0) {
        wanted = wanted >> start;
    }
    wanted = wanted.getPartValidRange(min(end + 1, numBits));
    
    // Extract the relevant bits from the vector.
    result = result.bitwiseAndAvalBval(wanted);
    result.decNumBits(end - start + 1);
    
    // If the slice ends after the last index, put x's where the index is greater than the number of bits.
    if (end >= numBits) {
        result.setPartSelect(result.numBits - 1, numBits, vec4state(X, result.numBits - numBits)); 
    }
    // If the slice starts before the first index, put x's where the index is less than 0.
    if (start < 0) {
        // Move the bits to the left.
        result = result << (-start);
        // Put x's where the index is less than 0.
        result.setPartSelect(-start - 1, 0, vec4state(X, -start));
    }
    // If needed to pad the result with x's.
    if (start < 0 || end >= numBits) {
        result.unknown = true;
    }
    // If the original vector holds only known bits and the slice is not out of bounds, the slice holds only known bits.
    else if (!unknown) {
        result.unknown = false;
    }
    // In all other cases, check for unknown bits.
    else {
        result.setUnknown();
    }
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
        vec4state otherCopy = other;
        
        // Resizing other to the size of the slice.
        otherCopy.setNumBits(end - start + 1);
        // In case the slice starts before the first index, move the bits to the right.
        if (start < 0) {
            otherCopy = otherCopy >> (-start);
        }
        start = max(start, (long long)(0));
        end = min(end, numBits - 1);
        // Adjust the size of other to the size of the slice.
        otherCopy.setNumBits(end - start + 1);
        // Save the bits before the slice.
        vec4state beforeStart = vec4state(ZERO, max(start, (long long)(1)));
        
        if (start > 0) {
            beforeStart = getPartValidRange(start);
        } 

        // Zero down the bits in the slice.
        *this = *this >> (end + 1);
        *this = *this << (end - start + 1);
        // Insert the bits of other into the slice.
        *this = AdditionAvalBval(otherCopy);
        // Move the starting bits back to their original position.
        *this = *this << start;
        *this = AdditionAvalBval(beforeStart);
        setNumBits(oldSize);
    }
    // If the original vector could have changed from unknown to known or vice versa.
    if ((other.unknown && !this->unknown) || (!other.unknown && this->unknown)) {
        setUnknown();
    }
}

vec4state vec4state::operator&&(const vec4state& other) const {
    int first = 0, second = 0;
    if (vectorSize > other.vectorSize) {
        for (long long i = vectorSize - 1; i >= other.vectorSize; i--) {
            // Remove the z bits
            uint32_t tmp = vector[i].getAval() - (vector[i].getAval() & vector[i].getBval());
            if (tmp != 0) {
                first = 1;
                break;
            }
        }
    } else if (vectorSize < other.vectorSize) {
        for (long long i = other.vectorSize - 1; i >= vectorSize; i--) {
            // Remove the z bits
            uint32_t tmp = other.vector[i].getAval() - (other.vector[i].getAval() & other.vector[i].getBval());
            if (tmp != 0) {
                second = 1;
                break;
            }
        }
    }
    for (long long i = min(vectorSize, other.vectorSize) - 1; i >= 0; i--) {
        if (first == 1 || second == 1) {
            return vec4state(ONE, 1);
        }
        // Remove the z bits
        uint32_t tmp1 = vector[i].getAval() - (vector[i].getAval() & vector[i].getBval());
        uint32_t tmp2 = other.vector[i].getAval() - (other.vector[i].getAval() & other.vector[i].getBval());
        if (tmp1 != 0) {
            first = 1;
        }
        if (tmp2 != 0) {
            second = 1;
        }
    }
    // After checking all the bits

    // If both vector have at least one bit set to 1
    if (first == 1 && second == 1) {
        return vec4state(ONE, 1);
    }

    // If at least one of the vectors has all bits set to 0
    if (first == 0 && !unknown) {
        return vec4state(ZERO, 1);
    }
    if (second == 0 && !other.unknown) {
        return vec4state(ZERO, 1);
    }

    // If at one of the vectors has at least one bit set to 1 and the other is unknown
    return vec4state(X, 1);
}

vec4state vec4state::operator||(const vec4state& other) const {
    if (vectorSize > other.vectorSize) {
        for (long long i = vectorSize - 1; i >= other.vectorSize; i--) {
            // Remove the bits of the z's
            uint32_t tmp = vector[i].getAval() - (vector[i].getAval() & vector[i].getBval());
            if (tmp != 0) {
                return vec4state(ONE, 1);
            }
        }
    } else if (vectorSize < other.vectorSize) {
        for (long long i = other.vectorSize - 1; i >= vectorSize; i--) {
            // Remove the bits of the z's
            uint32_t tmp = other.vector[i].getAval() - (other.vector[i].getAval() & other.vector[i].getBval());
            if (tmp != 0) {
                return vec4state(ONE, 1);
            }
        }
    }
    for (long long i = min(vectorSize, other.vectorSize) - 1; i >= 0; i--) {
        // Remove the bits of the z's
        uint32_t tmp1 = vector[i].getAval() - (vector[i].getAval() & vector[i].getBval());
        uint32_t tmp2 = other.vector[i].getAval() - (other.vector[i].getAval() & other.vector[i].getBval());
        if (tmp1 != 0 || tmp2 != 0) {
            return vec4state(ONE, 1);
        }
    }
    // After checking all the bits
    // Now we know that both vectors have all bits set to 0 or are unknown
    if (unknown || other.unknown) {
        return vec4state(X, 1);
    }

    // If both vectors have all bits set to 0
    return vec4state(ZERO, 1);
}

vec4state vec4state::operator!() const {
    // If the vector has at least one bit set to 1, return 0.
    for (int i = 0; i < vectorSize; i++) {
        // Extract the 1 bits.
        uint32_t oneBits = vector[i].getAval() - (vector[i].getAval() & vector[i].getBval());
        if (oneBits != 0) {
            return vec4state(ZERO, 1);
        }
    }
    // If the vector has at least one bit set to x or z, return x.
    if (unknown) {
        return vec4state(X, 1);
    }
    // If the vector has only 0 bits, return 1.
    return vec4state(ONE, 1);
}

vec4state vec4state::operator<(const vec4state& other) const {
    if (unknown || other.unknown) {
        return vec4state(X, 1);
    }
    if (vectorSize > other.vectorSize) {
        for (long long i = vectorSize - 1; i >= other.vectorSize; i--) {
            if (vector[i].getAval() != 0) {
                return vec4state(ZERO, 1);
            }
        }
    } else if (vectorSize < other.vectorSize) {
        for (long long i = other.vectorSize - 1; i >= vectorSize; i--) {
            if (other.vector[i].getAval() != 0) {
                return vec4state(ONE, 1);
            }
        }
    }
    for (long long i = min(vectorSize, other.vectorSize) - 1; i >= 0; i--) {
        if (vector[i].getAval() < other.vector[i].getAval()) {
            return vec4state(ONE, 1);
        }
        if (vector[i].getAval() > other.vector[i].getAval()) {
            return vec4state(ZERO, 1);
        }
    }

    // If the vectors are equal
    return vec4state(ZERO, 1);
}

vec4state vec4state::operator>(const vec4state& other) const {
    return other < *this;
}

vec4state vec4state::operator<=(const vec4state& other) const {
    return !(*this > other);
}

vec4state vec4state::operator>=(const vec4state& other) const {
    return !(*this < other);
}

vec4state vec4state::operator+(const vec4state& other) const {
    long long maxNumBits = max(numBits, other.numBits);
    if (unknown || other.unknown) {
        return vec4state(X, maxNumBits);
    }
    vec4state result = vec4state(ZERO, maxNumBits);
    // Align the vectors to the same size.
    vec4state copyThis = *this;
    vec4state copyOther = other;
    copyThis.setNumBits(maxNumBits);
    copyOther.setNumBits(maxNumBits);
    long long carry = 0;
    for (long long i = 0; i < result.vectorSize; i++) {
        long long sum = unsigned long long(copyThis.vector[i].getAval()) + unsigned long long(copyOther.vector[i].getAval()) + carry;
        // Put in the result vector only the lower 32 bits of the sum.
        result.vector[i].setAval(uint32_t(sum & MASK_32));
        // If the sum is bigger than 32 bits, set the carry to 1 for the next iteration.
        if (sum > MASK_32) {
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
        return vec4state(X, maxNumBits);
    }
    vec4state result = vec4state(ZERO, maxNumBits);
    // Align the vectors to the same size.
    vec4state copyThis = *this;
    vec4state copyOther = other;
    copyThis.setNumBits(maxNumBits);
    copyOther.setNumBits(maxNumBits);
    for (long long i = 0; i < copyThis.getVectorSize(); i++) {
        // If the current VPI in this vector is less than the corresponding one in other vector, borrow from the next VPI of this vector.
        if (copyThis.vector[i].getAval() < copyOther.vector[i].getAval()) {
            // Find the first next VPI in this vector that is not zero.
            long long firstNotZero = i + 1;
            while (copyThis.vector[firstNotZero].getAval() == 0 && firstNotZero < copyThis.getVectorSize()) {
                firstNotZero++;
            }
            if (firstNotZero != copyThis.getVectorSize()) {
                // If possible, borrow from the next VPI.
                copyThis.vector[firstNotZero].setAval(copyThis.vector[firstNotZero].getAval() - 1);
                firstNotZero--;
                for (; firstNotZero > i; firstNotZero--) {
                    copyThis.vector[firstNotZero].setAval(MASK_32);
                }
                // Calculate the fixed value of the current VPI.
                result.vector[i].setAval(uint32_t(MASK_BIT_33 + copyThis.vector[i].getAval() - copyOther.vector[i].getAval()));
            } else {
                // If there is no VPI to borrow from, the result is negative.
                for (long long j = 0; j < copyThis.getVectorSize(); j++) {
                    result.vector[j].setAval(copyThis.vector[j].getAval() - copyOther.vector[j].getAval());
                }
                return move(result);
            }
        }
        // Calculate the result as usual.
        else {
            result.vector[i].setAval(copyThis.vector[i].getAval() - copyOther.vector[i].getAval());
        }
    }
    return move(result);
}

vec4state vec4state::operator*(const vec4state& other) const {
    long long maxNumBits = max(numBits, other.numBits);
    if (unknown || other.unknown) {
        return vec4state(X, maxNumBits);
    }
    vec4state result = vec4state(ZERO, maxNumBits);
    // Align the vectors to the same size.
    vec4state copyThis = *this;
    vec4state copyOther = other;
    copyThis.setNumBits(maxNumBits);
    copyOther.setNumBits(maxNumBits);
    // for each VPI in this vector, multiply it by each VPI in other vector and add the result to the corresponding VPI in the result vector.
    long long carry = 0;
    for (int idxThis = 0; idxThis < maxNumBits; idxThis++) {
        for (int idxOther = 0; idxOther < maxNumBits; idxOther++) {
            long long mul = long long(copyThis.vector[idxThis].getAval()) * long long(copyOther.vector[idxOther].getAval());
            long long newResult = result.vector[max(idxThis, idxOther)].getAval() + mul + carry;
            if (newResult > MASK_32) {
                carry = newResult >> BITS_IN_CELL;
                newResult = newResult & MASK_32;
            } else {
                carry = 0;
            }
            // Add the lower 32 bits of the multiplication to the result vector.
            result.vector[max(idxThis, idxOther)].setAval(uint32_t(newResult));
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
        return vec4state(X, maxNumBits);
    }
    vec4state result = vec4state(ZERO, maxNumBits);
    // Align the vectors to the same size.
    vec4state copyThis = *this;
    vec4state copyOther = other;
    copyThis.setNumBits(maxNumBits);
    copyOther.setNumBits(maxNumBits);
    // Divide the vectors using bit manipulation.
    while (copyThis >= copyOther) {
        long long shift = 0;
        while (copyThis > (copyOther << (shift + 1))) {
            shift++;
        }
        copyThis = copyThis - (copyOther << shift);
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
        return vec4state(X, maxNumBits);
    }
    // Align the vectors to the same size.
    vec4state copyThis = *this;
    vec4state copyOther = other;
    copyThis.setNumBits(maxNumBits);
    copyOther.setNumBits(maxNumBits);
    // Divide the vectors using bit manipulation and extract the remainder.
    while (copyThis >= copyOther) {
        long long shift = 0;
        while (copyThis > (copyOther << (shift + 1))) {
            shift++;
        }
        copyThis = copyThis - (copyOther << shift);
    }
    return move(copyThis);
}

vec4state vec4state::operator-() const {
    if (unknown) {
        return vec4state(X, numBits);
    }
    return ~*this + vec4state(1);
}

vec4state vec4state::power(const vec4state& other) const {
    vec4state result;
    // If the base or the power has an unknown value, the result is unknown.
    if (unknown || other.unknown) {
        result = vec4state(X, numBits);
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
            if (i == vectorSize - 1 && numBits % BITS_IN_CELL != 0 && j > (numBits % BITS_IN_CELL) - 1) {
                continue;
            }
            uint32_t mask = 1 << j;
            if ((currAval & mask) && (currBval & mask)) {
                result += Z;
            } else if (currAval & mask) {
                result += ONE;
            } else if (currBval & mask) {
                result += X;
            } else {
                result += ZERO;
            }
        }
    }
    return result;
}

vec4state::operator bool() const {
    // If the vector has at least one bit set to 1, return true.
    for (long long i = 0; i < vectorSize; i++) {
        uint32_t oneBits = vector[i].getAval() - (vector[i].getAval() & vector[i].getBval());
        if (oneBits != 0) {
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