#include "vec4state.h"
#include "math.h"
#include <iostream>
#include "vec4stateException.h"

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
        throw vec4stateExceptionInvalidSize("Number of bits must be positive");
    }
    if (newNumBits < numBits) {
        throw vec4stateExceptionInvalidSize("Number of bits must be greater than the current number of bits");
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
        throw vec4stateExceptionInvalidSize("Number of bits must be non-negative");
    }
    if (newNumBits > numBits) {
        throw vec4stateExceptionInvalidSize("Number of bits must be less than the current number of bits");
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
        VPI currVPI = vector[i];
        // If the current cell is still in range.
        if (i < indexLastCell) {
            newVector[i] = currVPI;
        }
        // If the last cell needs to be truncated in the middle
        else if (offset != 0) {
            newVector[i].setAval(currVPI.getAval() & mask);
            newVector[i].setBval(currVPI.getBval() & mask);
        }
    }
    delete[] vector;
    vector = newVector;
    if (unknown) {
        setUnknown();
    }
}

long long vec4state::extractNumberFromVector() const {
    // If the vector has 1's after the 64th bit, the number is too large to be used as an index.
    for (long long i = vectorSize - 1; i >= CELLS_IN_INDEX_VECTOR; i--) {
        if (vector[i].getAval() != 0) {
            throw vec4stateExceptionInvalidSize("Cannot convert a vector that stores more than 64 bits to a number");
        }
    }
    if (unknown) {
        throw vec4stateExceptionUnknownVector("Cannot convert unknown vector to a number");
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
        VPI currThisVPI = this->vector[i];
        VPI currOtherVPI = other.vector[i];
        // If the current VPI can be calculated with both vectors.
        if (i < this->vectorSize && i < other.vectorSize) {
            result.vector[i].setAval(currThisVPI.getAval() & currOtherVPI.getAval());
            result.vector[i].setBval(currThisVPI.getBval() & currOtherVPI.getBval());
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
        VPI currThisVPI = this->vector[i];
        VPI currOtherVPI = other.vector[i];
        // If the current VPI can be calculated with both vectors.
        if (i < this->vectorSize && i < other.vectorSize) {
            result.vector[i].setAval(currThisVPI.getAval() + currOtherVPI.getAval());
            result.vector[i].setBval(currThisVPI.getBval() + currOtherVPI.getBval());
        }
        // If reached end of this, copy only other's cells.
        else if (i < other.vectorSize) {
            result.vector[i].setAval(currOtherVPI.getAval());
            result.vector[i].setBval(currOtherVPI.getBval());
        }
        // If reached end of other, copy only this's cells.
        else {
            result.vector[i].setAval(currThisVPI.getAval());
            result.vector[i].setBval(currThisVPI.getBval());
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
        throw vec4stateExceptionInvalidSize("Number of bits must be non-negative");
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
        throw vec4stateExceptionInvalidIndex("end must be non-negative");
    }
    if (end > numBits) {
        throw vec4stateExceptionInvalidIndex("end must be less than the number of bits in the vector");
    }
    vec4state result = vec4state(Z, end);
    for (long long i = 0; i < result.vectorSize; i++) {
        VPI currThisVPI = vector[i];
        VPI currResultVPI = result.vector[i];
        result.vector[i].setAval(currThisVPI.getAval() & currResultVPI.getAval());
        result.vector[i].setBval(currThisVPI.getBval() & currResultVPI.getBval());
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

/*
Helper function for filling the vector with bits from a string.
The function fills the VPI of vector at index currVPIIndex with bits from str, starting from index currStrIndex.
The filling stops when cellSize bits are filled.
Returns the index of the next bit in the string.
*/
long long fillVPIWithStringBits(VPI* vector, const string& str, long long cellSize, long long currVPIIndex, long long currStrIndex) {
    uint32_t aval = 0;
    uint32_t bval = 0;
    for (long long currBitIndex = 0; currBitIndex < cellSize; currBitIndex++) {
        switch(str[currStrIndex]) {
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
                throw vec4stateExceptionInvalidInput("Invalid bit: " + str[currStrIndex]);
        }
        // If the current bit is not the last bit in the cell, shift the bits to the left.
        if (currBitIndex < cellSize - 1) {
            aval = aval << 1;
            bval = bval << 1;
        }
        currStrIndex++;
    }
    vector[currVPIIndex].setAval(aval);
    vector[currVPIIndex].setBval(bval);
    return currStrIndex;
}

vec4state::vec4state(string str) : vec4state() {
    // If the string is empty, creates a default vector.
    if (str.length() == 0) {
        return;
    }
    numBits = str.length();
    vectorSize = calcVectorSize(numBits);
    vector = new VPI[vectorSize];
    // For each VPI element in the vector, fill it with the bits from the string.
    int numUndividedBits = numBits % BITS_IN_CELL;
    long long currStrIndex = 0;
    for (long long currVPIIndex = vectorSize - 1; currVPIIndex >= 0; currVPIIndex--) {
        // The last element gets a special case because when the c'tor receives a string, it reads the string's characters from the MSB to the LSB, translates them to VPIs, and stores them in the vector. If the string's length doesn't divide by 32, then the last VPI (where the 32 MSBs are stored) needs alignment to 32 bits (by padding it with zeroes).
        if (currVPIIndex == vectorSize - 1 && numUndividedBits != 0) {
            try {
                currStrIndex = fillVPIWithStringBits(vector, str, numUndividedBits, currVPIIndex, 0);
            } catch (vec4stateExceptionInvalidInput& e) {
                delete vector;
                throw e;
            }
        }
        // For the rest of the elements in the vector, fill them with the bits from the string.
        else {
            try {
                currStrIndex = fillVPIWithStringBits(vector, str, BITS_IN_CELL, currVPIIndex, currStrIndex);
            } catch (vec4stateExceptionInvalidInput& e) {
                delete vector;
                throw e;
            }
        }
    }
    setUnknown();
}
    
vec4state::vec4state(BitValue bit, long long numBits) : vec4state(string(numBits, bit)) {
    if (bit != ZERO && bit != ONE && bit != X && bit != Z) {
        throw vec4stateExceptionInvalidInput("Invalid bit");
    }
    if (numBits <= 0) {
        throw vec4stateExceptionInvalidSize("Number of bits must be greater than 0");
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
    vec4state result = vec4state(ZERO, max(numBits, other.numBits));
    // If the vectors have different number of bits, change the size of the smaller vector to the size of the larger vector by padding it with 0's.
    if (numBits < other.numBits) {
        vec4state copyThis = *this;
        copyThis.setNumBits(other.numBits);
        result = ~(~copyThis | ~other);
    } else if (numBits > other.numBits) {
        vec4state copyOther = other;
        copyOther.setNumBits(numBits);
        result = ~(~(*this) | ~copyOther);
    } else {
        result = ~(~(*this) | ~other);
    }
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
        VPI currThisVPI = copyThis.vector[i];
        VPI currOtherVPI = copyOther.vector[i];
        copyThis.vector[i].setAval(currThisVPI.getAval() - (currThisVPI.getAval() & currThisVPI.getBval()));
        copyOther.vector[i].setAval(currOtherVPI.getAval() - (currOtherVPI.getAval() & currOtherVPI.getBval()));
    }
    for (long long i = 0; i < maxVectorSize; i++) {
        VPI currThisVPI = copyThis.vector[i];
        VPI currOtherVPI = copyOther.vector[i];
        copyThis.vector[i].setBval(currThisVPI.getBval() - (currOtherVPI.getAval() & currThisVPI.getBval()));
        copyOther.vector[i].setBval(currOtherVPI.getBval() - (currThisVPI.getAval() & currOtherVPI.getBval()));
    }
    for (long long i = 0; i < maxVectorSize; i++) {
        VPI currThisVPI = copyThis.vector[i];
        VPI currOtherVPI = copyOther.vector[i];
        result.vector[i].setAval(currThisVPI.getAval() | currOtherVPI.getAval());
        result.vector[i].setBval(currThisVPI.getBval() | currOtherVPI.getBval());
        if (result.vector[i].getBval() != 0) {
            result.unknown = true;
        }
    }
    return move(result);
}

vec4state vec4state::operator^(const vec4state& other) const {
    vec4state result = vec4state(ZERO, max(numBits, other.numBits));
    // If the vectors have different number of bits, change the size of the smaller vector to the size of the larger vector by padding it with 0's.
    if (numBits < other.numBits) {
        vec4state copyThis = *this;
        copyThis.setNumBits(other.numBits);
        result = (copyThis & ~other) | (~copyThis & other);
    } else if (numBits > other.numBits) {
        vec4state copyOther = other;
        copyOther.setNumBits(numBits);
        result = (*this & ~copyOther) | (~(*this) & copyOther);
    } else {
        result = (*this & ~other) | (~(*this) & other);
    }
    return move(result);
}

// Helper function for zeroing down the bits that are out of range while the vector stays the same size.
void zeroDownOutOfRangeBits(VPI* vector, long long vectorSize, long long numBits) {
    long long indexLastCell = calcVectorSize(numBits) - 1;
    long long offset = numBits % BITS_IN_CELL;
    long long mask = (long long)(pow(2, offset) - 1);
    for (long long i = indexLastCell; i < vectorSize; i++) {
        if (i == indexLastCell) {
            if (offset) {
                VPI currVPI = vector[i];
                // If the last relevant cell needs to be truncated in the middle.
                vector[i].setAval(currVPI.getAval() & mask);
                vector[i].setBval(currVPI.getBval() & mask);
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
        VPI currVPI = result.vector[i];
        result.vector[i].setAval(~(currVPI.getAval() | currVPI.getBval()));
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
        VPI currThisVPI = vector[i];
        VPI currOtherVPI = other.vector[i];
        if (currThisVPI.getAval() != currOtherVPI.getAval() || currThisVPI.getBval() != currOtherVPI.getBval()) {
            return vec4state(ZERO, 1);
        }
    }
    return vec4state(ONE, 1);
}

vec4state vec4state::caseInequality(const vec4state& other) const {
    return !caseEquality(other);
}

vec4state vec4state::operator<<(const vec4state& other) {
    long long numOfThis;
    try {
        numOfThis = other.extractNumberFromVector();
    } catch (vec4stateExceptionUnknownVector&) {
        return vec4state(X, numBits);
    } catch (vec4stateExceptionInvalidSize&) {
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
            VPI currThisVPI = vector[i];
            (res.vector[i + offset]).setAval(currThisVPI.getAval());
            (res.vector[i + offset]).setBval(currThisVPI.getBval());
            if (res.vector[i + offset].getBval() != 0) {
                res.unknown = true;
            }
            res.vector[i].setAval(0);
            res.vector[i].setBval(0);
        }
    }
    // Shifting the remaining bits
    for (long long i = vectorSize - 1; i >= 0; i--) {
        VPI currVPI = res.vector[i];
        res.vector[i].setAval(currVPI.getAval() << (num % BITS_IN_CELL));
        res.vector[i].setBval(currVPI.getBval() << (num % BITS_IN_CELL));
        currVPI = res.vector[i];
        if (i > 0) {
            VPI prevVPI = res.vector[i - 1];
            res.vector[i].setAval(currVPI.getAval() | (prevVPI.getAval() >> (BITS_IN_CELL - (num % BITS_IN_CELL))));
            res.vector[i].setBval(currVPI.getBval() | (prevVPI.getBval() >> (BITS_IN_CELL - (num % BITS_IN_CELL))));
        }
        currVPI = res.vector[i];
        if (currVPI.getBval() != 0) {
            res.unknown = true;
        }
    }
    return move(res);
}

vec4state vec4state::operator>>(const vec4state& other) {
    long long numOfThis;
    try {
        numOfThis = other.extractNumberFromVector();
    } catch (vec4stateExceptionUnknownVector&) {
        return vec4state(X, numBits);
    } catch (vec4stateExceptionInvalidSize&) {
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
            VPI currThisVPI = vector[i];
            res.vector[i - offset].setAval(currThisVPI.getAval());
            res.vector[i - offset].setBval(currThisVPI.getBval());
            if (res.vector[i - offset].getBval() != 0) {
                res.unknown = true;
            }
            res.vector[i].setAval(0);
            res.vector[i].setBval(0);
        }
    }
    // Shifting the remaining bits
    for (int i = 0; i < vectorSize; i++) {
        VPI currVPI = res.vector[i];
        res.vector[i].setAval(currVPI.getAval() >> (num % BITS_IN_CELL));
        res.vector[i].setBval(currVPI.getBval() >> (num % BITS_IN_CELL));
        currVPI = res.vector[i];
        if (i < vectorSize - 1) {
            VPI nextVPI = res.vector[i + 1];
            res.vector[i].setAval(currVPI.getAval() | (nextVPI.getAval() << (BITS_IN_CELL - (num % BITS_IN_CELL))));
            res.vector[i].setBval(currVPI.getBval() | (nextVPI.getBval() << (BITS_IN_CELL - (num % BITS_IN_CELL))));
        }
        currVPI = res.vector[i];
        if (currVPI.getBval() != 0) {
            res.unknown = true;
        }
    }
    return move(res);
}

vec4state vec4state::getBitSelect(const vec4state& index) const {
    long long bitIndex;
    try {
        bitIndex = index.extractNumberFromVector();
    } catch (vec4stateExceptionUnknownVector&) {
        // If the index is unknown, the result is unknown.
        return vec4state(X, numBits);
    } catch (vec4stateExceptionInvalidSize&) {
        // If the index is out of range, the result is unknown.
        return vec4state(X, numBits);
    }
    if (bitIndex > numBits) {
        // If the index is out of range, the result is unknown.
        return vec4state(X, numBits);
    }
    return getPartSelect(bitIndex, bitIndex);
}

void vec4state::setBitSelect(const vec4state& index, const vec4state& newValue) {
    long long bitIndex;
    try {
        bitIndex = index.extractNumberFromVector();
    } catch (vec4stateExceptionUnknownVector&) {
        // If the index is unknown, the result is unknown.
        return;
    } catch (vec4stateExceptionInvalidSize&) {
        // If the index is out of range, the result is unknown.
        return;
    }
    if (bitIndex > numBits) {
        // If the index is out of range, the result is unknown.
        return;
    }
    setPartSelect(bitIndex, bitIndex, newValue);
}

vec4state vec4state::getPartSelect(long long end, long long start) const {
    // If input is invalid.
    if (end < start) {
        throw vec4stateExceptionInvalidRange(end, start);
    }
    // If the slice is completely out of range.
    if (end < 0 || start >= numBits) {
        return vec4state(X, end - start + 1);
    }

    // Extract the relevant VPIs of the vector.
    long long startVPIIndex = start / BITS_IN_CELL;
    long long lastVPIIndex = end / BITS_IN_CELL;
    vec4state result = vec4state(ZERO, (lastVPIIndex - startVPIIndex + 1) * BITS_IN_CELL);
    for(long long currVPIIndex = startVPIIndex; currVPIIndex <= lastVPIIndex; currVPIIndex++) {
        VPI currVPI = vector[currVPIIndex];
        result.vector[currVPIIndex - startVPIIndex].setAval(currVPI.getAval());
        result.vector[currVPIIndex - startVPIIndex].setBval(currVPI.getBval());
    }
    // Move the bits to the right if the slice starts after index 0.
    if (start >= 0) {
        result = result >> start;
    }
    // If the slice starts before the first index, shift the values put x's where the index is less than 0.
    else {
        // Move the bits to the left.
        result = result << (-start);
        // Put x's where the index is less than 0.
        result.setPartSelect(-start - 1, 0, vec4state(X, -start));
    }
    // If the slice ends after the last index, put x's where the index is greater than the number of bits.
    if (end >= numBits) {
        result.setPartSelect(result.numBits - 1, numBits - start, vec4state(X, result.numBits - 1 - numBits + start)); 
    }
    result.decNumBits(end - start + 1);
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

// TODO: figure out why we fail tests.
void vec4state::setPartSelect(long long end, long long start, vec4state other) {
    // If input is invalid.
    if (end < start) {
        throw vec4stateExceptionInvalidRange(end, start);
    }
    // Resizing other to the size of the slice.
    other.setNumBits(end - start + 1);
    // In case the slice starts before the first index, move the bits to the right.
    if (start < 0) {
        other = other >> (-start);
    }
    start = max(start, (long long)(0));
    end = min(end, numBits - 1);
    // Adjust the size of other to the size of the slice.
    other.setNumBits(end - start + 1);
    // If the slice is not completely out of range, perform the operation.
    if (end >= 0 && start < numBits) {
        long long startVPIIndex = start / BITS_IN_CELL;
        int offsetStart = start % BITS_IN_CELL;
        long long endVPIIndex = end / BITS_IN_CELL;
        int offsetEnd = end % BITS_IN_CELL;
        long long currVPIIndex = startVPIIndex;
        uint32_t avalStartThis = vector[startVPIIndex].getAval();
        uint32_t bvalStartThis = vector[startVPIIndex].getBval();
        uint32_t avalStartOther = other.vector[0].getAval();
        uint32_t bvalStartOther = other.vector[0].getBval(); 
        // If the slice starts in the middle of a cell, copy only the relevant bits.
        if (offsetStart != 0) {
            VPI currOtherVPI = other.vector[0];
            VPI currThisVPI = vector[currVPIIndex];
            // Leave the bits from the original vector that are not in the slice.
            avalStartThis = currThisVPI.getAval() & (MASK_32 >> (BITS_IN_CELL - offsetStart));
            bvalStartThis = currThisVPI.getBval() & (MASK_32 >> (BITS_IN_CELL - offsetStart));
            // Get the bits from the other vector that are in the slice.
            avalStartOther = currOtherVPI.getAval() << offsetStart;
            bvalStartOther = currOtherVPI.getBval() << offsetStart;
            // Combine the relevant bits from the original vector and the other vector only if it won't affect the end of the slice.
            if (startVPIIndex != endVPIIndex) {
                vector[currVPIIndex].setAval(avalStartThis | avalStartOther);
                vector[currVPIIndex].setBval(bvalStartThis | bvalStartOther);
                if (vector[currVPIIndex].getBval() != 0) {
                    unknown = true;
                }
                currVPIIndex++;
            }
        }
        // While haven't reached the VPI of the end of the slice, copy all the bits from the other vector.
        for (; currVPIIndex <= endVPIIndex; currVPIIndex++) {
            VPI currOtherVPI = other.vector[currVPIIndex - startVPIIndex];
            // If reached the VPI of the end of the slice and the end of the slice is in the middle of the VPI, copy only the relevant bits.
            if (currVPIIndex == endVPIIndex && offsetEnd != BITS_IN_CELL - 1) {
                VPI currOtherVPI = other.vector[endVPIIndex - startVPIIndex];
                VPI currThisVPI = vector[endVPIIndex];
                // Leave the bits from the original vector that are not in the slice.
                uint32_t avalEndThis = currThisVPI.getAval() & (MASK_32 << (offsetEnd + 1));
                uint32_t bvalEndThis = currThisVPI.getBval() & (MASK_32 << (offsetEnd + 1));
                // Get the bits from the other vector that are in the slice.
                uint32_t avalEndOther = currOtherVPI.getAval() & (MASK_32 >> (BITS_IN_CELL - offsetEnd - 1));
                uint32_t bvalEndOther = currOtherVPI.getBval() & (MASK_32 >> (BITS_IN_CELL - offsetEnd - 1));
                // Combine the relevant bits from the original vector and the other vector.
                // If the slice starts and ends in the same cell, leave only the bits that are not in the slice and combine the rest.
                if (startVPIIndex == endVPIIndex && offsetStart != 0) {
                    avalEndThis |= avalStartThis;
                    bvalEndThis |= bvalStartThis;
                }
                vector[endVPIIndex].setAval(avalEndThis | avalEndOther);
                vector[endVPIIndex].setBval(bvalEndThis | bvalEndOther);
            }
            // If the end of the slice is at the end of the VPI, copy the whole VPI.
            else {
                vector[currVPIIndex].setAval(currOtherVPI.getAval());
                vector[currVPIIndex].setBval(currOtherVPI.getBval());
            }
        }
        if (vector[endVPIIndex].getBval() != 0) {
            unknown = true;
        }
    }
}

vec4state vec4state::operator&&(const vec4state& other) const {
    // If both vectors have at least one bit set to 1, return 1.
    if (bool(*this) && bool(other)) {
        return vec4state(ONE, 1);
    }
    // If one of the vectors has at least one bit set to 1 and the other is unknown, or both are unknown, return x.
    else if ((bool(*this) && other.unknown) || (bool(other) && this->unknown) || (this->unknown && other.unknown)) {
        return vec4state(X, 1);
    }
    // If at least one of the vectors has all bits set to 0, return 0.
    else {
        return vec4state(ZERO, 1);
    }
}

vec4state vec4state::operator||(const vec4state& other) const {
    // If at least one of the vectors have at least one bit set to 1, return 1.
    if (bool(*this) || bool(other)) {
        return vec4state(ONE, 1);
    }
    // If at least one of the vectors is unknown, return x.
    else if (this->unknown || other.unknown) {
        return vec4state(X, 1);
    }
    // If both vectors have all bits set to 0, return 0.
    else {
        return vec4state(ZERO, 1);
    }
}

vec4state vec4state::operator!() const {
    // If the vector has at least one bit set to 1, return 0.
    for (int i = 0; i < vectorSize; i++) {
        VPI currVPI = vector[i];
        // Extract the 1 bits.
        uint32_t oneBits = currVPI.getAval() - (currVPI.getAval() & currVPI.getBval());
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
        VPI currThisVPI = vector[i];
        VPI currOtherVPI = other.vector[i];
        if (currThisVPI.getAval() < currOtherVPI.getAval()) {
            return vec4state(ONE, 1);
        }
        if (currThisVPI.getAval() > currOtherVPI.getAval()) {
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
    long long carry = 0;
    long long sum = 0;
    for (long long i = 0; i < result.vectorSize; i++) {
        VPI currThisVPI = this->vector[i];
        VPI currOtherVPI = other.vector[i];
        if (i >= this->vectorSize) {
            sum = unsigned long long(currOtherVPI.getAval()) + carry;
        }
        else if (i >= other.vectorSize) {
            sum = unsigned long long(currThisVPI.getAval()) + carry;
        }
        else {
            sum = unsigned long long(currThisVPI.getAval()) + unsigned long long(currOtherVPI.getAval()) + carry;
        }
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
    // Align this to the maximal size.
    vec4state copyThis = *this;
    copyThis.setNumBits(maxNumBits);
    for (long long i = 0; i < result.vectorSize; i++) {
        // If the current VPI is out of range of the other vector, copy it as is.
        if (i >= other.vectorSize) {
            result.vector[i].setAval(copyThis.vector[i].getAval());
        }
        // If the current VPI is not out of range of the other vector, calculate the result.
        else {
            VPI currThisVPI = copyThis.vector[i];
            VPI currOtherVPI = other.vector[i];
            // If the current VPI in this vector is less than the corresponding one in other vector, borrow from the next VPI of this vector.
            if (currThisVPI.getAval() < currOtherVPI.getAval()) {
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
                    result.vector[i].setAval(uint32_t(MASK_BIT_33 + currThisVPI.getAval() - currOtherVPI.getAval()));
                } else {
                    // If there is no VPI to borrow from, the result is negative.
                    for (long long j = 0; j < copyThis.getVectorSize(); j++) {
                        result.vector[j].setAval(copyThis.vector[j].getAval() - other.vector[j].getAval());
                    }
                    return move(result);
                }
            }
            // Calculate the result as usual.
            else {
                result.vector[i].setAval(currThisVPI.getAval() - currOtherVPI.getAval());
            }
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
        throw vec4stateExceptionInvalidOperation("Division by zero is not allowed");
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
        throw vec4stateExceptionInvalidOperation("Division by zero is not allowed");
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
        VPI currVPI = vector[i];
        uint32_t oneBits = currVPI.getAval() - (currVPI.getAval() & currVPI.getBval());
        if (oneBits != 0) {
            return true;
        }
    }
    // If the vector has all bits set to 0 or has unknown bits, return false.
    return false;
}

void vec4state::convertTo2State() {
    for (int i = 0; i < vectorSize; i++) {
        VPI currVPI = vector[i];
        // Replace the z bits with x bits.
        vector[i].setAval(currVPI.getAval() - (currVPI.getAval() & currVPI.getBval()));
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