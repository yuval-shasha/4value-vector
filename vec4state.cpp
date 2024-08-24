#include "vec4state.h"

#define BITS_IN_VPI 32
#define CELLS_IN_INDEX_VECTOR 2
#define MASK_32 0xFFFFFFFF
#define MASK_BIT_33 0x100000000 

using namespace std;

/**
 * @brief Helper function to calculate the size of the vector.
 * 
 * The function calculates the number of VPI elements required in order to hold numBits bits by dividing numBits by the number of bits in a VPI element (32 bits).
 * 
 * @param numBits Number of bits in the vector.
 * @return long long The size of the vector.
 */
long long calcVectorSize(long long numBits) {
    return (numBits + BITS_IN_VPI - 1) / BITS_IN_VPI;
}

/**
 * @brief Sets unknown field for vec4state.
 * 
 * Scans the vector and sets the unknown flag to true if encountered unknown values in the vector, and to false otherwise.
 */
void vec4state::setUnknown() {
    for (int i = 0; i < vectorSize; i++) {
        if (vector[i].getBval() != 0) {
            unknown = true;
            return;
        }
    }
    unknown = false;
}

/**
 * @brief Increment number of bits for vec4state.
 * 
 * Increments the number of bits in the vector to newNumBits by assigning a bigger VPI array to this vector, copying the values from the original array to the new one, and zero-extending the new VPIs in the new array. Throws vec4stateExceptionInvalidSize if newNumBits is less than the current number of bits or non-positive. If newNumBits is the same as the current number of bits, the vector remains unchanged.
 * 
 * @param newNumBits The new number of bits in the vector.
 */
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
    // If no need to add more VPI elements, leave the array as is.
    if (vectorSize == oldVectorSize) {
        return;
    }
    shared_ptr<VPI[]> newVector(new VPI[vectorSize], default_delete<VPI[]>());
    for (long long i = 0; i < vectorSize; i++) {
        // If the current VPI is still in range of the old array, copy the values from the old array.
        if (i < oldVectorSize) {
            newVector[i] = vector[i];
        }
        // If the current VPI is out of range of the old array, zero-extend the new VPIs.
        else {
            newVector[i].setAval(0);
            newVector[i].setBval(0);
        }
    }
    vector = newVector;
}

/**
 * @brief Truncate number of bits for vec4state.
 * 
 * Decreases the number of bits in the vector to newNumBits by assigning a smaller VPI array to this vector, copying the values that are in the new range from the original array to the new one, and truncating the VPI that are not in the new range. Throws vec4stateExceptionInvalidSize if newNumBits is greater than the current number of bits or negative. If newNumBits is the same as the current number of bits, the vector remains unchanged. If newNumBits is 0, the vector is truncated to a single x bit.
 * 
 * @param newNumBits The new number of bits in the vector.
 */
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
    long long offset = newNumBits % BITS_IN_VPI;
    long long mask = (long long)(pow(2, offset) - 1);
    numBits = newNumBits;
    // If no need to delete VPI elements, remove the unnecessary bits from the last VPI.
    if (vectorSize == indexLastCell + 1) {
        if (offset != 0) {
            vector[indexLastCell].setAval(vector[indexLastCell].getAval() & mask);
            vector[indexLastCell].setBval(vector[indexLastCell].getBval() & mask);
        }
        // If the new last cell still holds unknown bits, the vector holds unknown bits. Otherwise, if the vector was unknown before, check the rest of the vector.
        if (vector[indexLastCell].getBval() != 0) {
            unknown = true;
        } else if (unknown) {
            setUnknown();
        }
    }
    // If need to remove VPI elements, create a new array with the new size and copy the values that are still in range from the old array.
    else {
        vectorSize = indexLastCell + 1;
        shared_ptr<VPI[]> newVector(new VPI[vectorSize], default_delete<VPI[]>());
        for (long long i = 0; i <= indexLastCell; i++) {
            VPI currVPI = vector[i];
            // If the current cell is still in range, copy the values from the old array.
            if (i < indexLastCell) {
                newVector[i] = currVPI;
                // If the current cell holds unknown bits, the new vector holds unknown bits.
                if (currVPI.getBval() != 0) {
                    unknown = true;
                }
            }
            // If the last cell needs to be truncated in the middle, extract the relevant bits.
            else if (offset != 0) {
                newVector[i].setAval(currVPI.getAval() & mask);
                newVector[i].setBval(currVPI.getBval() & mask);
                // If the new last cell holds unknown bits, the vector holds unknown bits.
                if (vector[indexLastCell].getBval() != 0) {
                    unknown = true;
                }
            }
        }
        vector = newVector;
    }
}

/**
 * @brief Extract number from vector for vec4state.
 * 
 * Calculates the numerical value that the vector holds by iterating over the vector's VPIs, adding and shifting them. This method can be used only if the vector has up to 2 VPI elements (so it's value can be represented by 64 bits) and has no unknown bits. If the vector has more than 2 VPI elements, vec4stateExceptionInvalidSize is throwns. If the vector has unknown bits, vec4stateExceptionUnknownVector is thrown.
 * 
 * @return The numerical value that the vector holds.
 */
long long vec4state::extractNumberFromVector() const {
    // If the vector has 1's after the 64th bit, the value is too large to be represented by 64 bits.
    for (long long i = vectorSize - 1; i >= CELLS_IN_INDEX_VECTOR; i--) {
        if (vector[i].getAval() != 0) {
            throw vec4stateExceptionInvalidSize("Cannot convert a vector that stores more than 64 bits to a number");
        }
    }
    // If the vector has unknown bits, the value cannot be calculated.
    if (unknown) {
        throw vec4stateExceptionUnknownVector("Cannot convert unknown vector to a number");
    }
    // If the vector holds a value that can be represented by 64 bits, iterate over the VPI elements and calculate the value by adding and shifting the values of the VPI elements.
    long long result = 0;
    for (long long i = vectorSize - 1; i >= 0; i--) {
        result += vector[i].getAval();
        if (i > 0) {
            result <<= BITS_IN_VPI;
        }
    }
    return result;
}

/**
 * @brief Sets the number of bits to a new number for vec4state.
 * 
 * Sets the number of bits in the vector to newNumBits by truncating or extending the vector (using decNumBits / incNumBits, respectively). If newNumBits is less than the current number of bits, the vector is truncated. If newNumBits is greater than the current number of bits, the vector is zero-extended. If newNumBits is the same as the current number of bits, the vector remains unchanged. If newNumBits is negative, vec4stateExceptionInvalidSize is thrown.
 * 
 * @param newNumBits The new number of bits in the vector.
 */
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

/**
 * @brief Default constructor for vec4state.
 * 
 * Initializes a 1-bit vector, initialized to x.
 */
vec4state::vec4state() : vector(nullptr) {
    vector = shared_ptr<VPI[]>(new VPI[1], default_delete<VPI[]>());
    numBits = 1;
    vectorSize = 1;
    vector[0].setAval(0);
    vector[0].setBval(1);
    unknown = true;
}

/**
 * @brief Helper function for filling the vector with bits from a string.
 * 
 * The function iterates over the string's characters (starting from index startStrIndex), translates the BitValues to aval and bval of a VPI, and fills the VPI of vector at index VPIIndex with these BitValues. The filling stops when cellSize bits are filled. If str contains invalid characters, vec4stateExceptionInvalidInput is thrown.
 * 
 * @param vector The vector to fill.
 * @param str The string to read the bits from.
 * @param cellSize The number of bits to fill in the VPI.
 * @param VPIIndex The index of the VPI in the vector to fill.
 * @param startStrIndex The index of the first bit in the string to start reading from.
 * @return The index of the next bit in the string.
 */
long long fillVPIWithStringBits(shared_ptr<VPI[]> vector, const string& str, long long cellSize, long long VPIIndex, long long startStrIndex) {
    uint32_t aval = 0;
    uint32_t bval = 0;
    // Iterate over the string's characters and fill the VPI with the bits until reaching cellSize.
    long long currStrIndex = startStrIndex;
    for (long long currBitIndex = 0; currBitIndex < cellSize; currBitIndex++) {
        switch(str[currStrIndex]) {
            // Translate the BitValues to aval and bval of VPI.
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
        // If the current bit is not the last bit in the cell, shift the current values to the left to keep on adding.
        if (currBitIndex < cellSize - 1) {
            aval = aval << 1;
            bval = bval << 1;
        }
        currStrIndex++;
    }
    vector[VPIIndex].setAval(aval);
    vector[VPIIndex].setBval(bval);
    return currStrIndex;
}

/**
 * @brief String constructor for vec4state.
 * 
 * Initializes a vector of size str.length() with the values represented by str. The constructor iterates over the string's characters, translates the BitValues to aval and bval of a VPI, and fills the VPI of vector from last VPI to first VPI with these BitValues. If str contains a character that is not a BitValue, the vector is initialized to x and vec4stateExceptionInvalidInput is thrown.
 * 
 * @param str The value to initialize the vector with, must be a string that holds only BitValues.
 */
vec4state::vec4state(string str) : vec4state() {
    // If the string is empty, creates a default vector.
    if (str.length() == 0) {
        return;
    }
    numBits = str.length();
    vectorSize = calcVectorSize(numBits);
    vector = shared_ptr<VPI[]>(new VPI[vectorSize], default_delete<VPI[]>());
    // For each VPI element in the vector, fill it with the bits from the string.
    int numUndividedBits = numBits % BITS_IN_VPI;
    long long currStrIndex = 0;
    for (long long currVPIIndex = vectorSize - 1; currVPIIndex >= 0; currVPIIndex--) {
        // The constructor reads the string's characters from the MSB to the LSB, therefore if the string's length doesn't divide by 32, then the last VPI (where the 32 MSBs are stored) needs alignment to 32 bits (by zero-extension).
        if (currVPIIndex == vectorSize - 1 && numUndividedBits != 0) {
            try {
                currStrIndex = fillVPIWithStringBits(vector, str, numUndividedBits, currVPIIndex, 0);
            } catch (vec4stateExceptionInvalidInput& e) {
                throw e;
            }
        }
        // For the rest of the elements in the vector, fill them with the bits from the string as usual.
        else {
            try {
                currStrIndex = fillVPIWithStringBits(vector, str, BITS_IN_VPI, currVPIIndex, currStrIndex);
            } catch (vec4stateExceptionInvalidInput& e) {
                throw e;
            }
        }
    }
    setUnknown();
}

/**
 * @brief Single repeated BitValue constructor for vec4state.
 * 
 * Initializes a vector with a single bit (that can be either 0, 1, x, or z) repeated numBits times. If numBits is non-positive or bit is not a BitValue, the vector is default initialized and vec4stateExceptionInvalidSize or vec4stateExceptionInvalidInput is thrown, respectively. 
 * 
 * @param bit The bit to repeat.
 * @param numBits The number of bits in the vector.
 */
vec4state::vec4state(BitValue bit, long long numBits) : vec4state(string(numBits, bit)) {
    if (bit != ZERO && bit != ONE && bit != X && bit != Z) {
        throw vec4stateExceptionInvalidInput("Invalid bit");
    }
    if (numBits <= 0) {
        throw vec4stateExceptionInvalidSize("Number of bits must be greater than 0");
    }
}

/**
 * @brief Copy constructor for vec4state.
 * 
 * Initializes a new vector and copies the values from the other vector to the new vector.
 * 
 * @param other The vector to copy from.
 */
vec4state::vec4state(const vec4state& other) : numBits(other.numBits), vectorSize(other.vectorSize), unknown(other.unknown), vector(nullptr) {
    vector = shared_ptr<VPI[]>(new VPI[vectorSize], default_delete<VPI[]>());
    for (int i = 0; i < vectorSize; i++) {
        vector[i] = other.vector[i];
    }
}

/**
 * @brief Move constructor for vec4state.
 * 
 * Initializes a new vector with the same values as other, and transfers the ownership of the vector to the new object. The other vector is left in a valid but unspecified state.
 * 
 * @param other The vector to move from.
 */
vec4state::vec4state(vec4state&& other) noexcept : numBits(other.numBits), vectorSize(other.vectorSize), unknown(other.unknown), vector(other.vector) {
    other.vector.reset();
}

/**
 * @brief Assignment operator for vec4state.
 * 
 * Assigns the values of other to this vector. If the other vector is shorter than this vector, zeroes down the bits that are out of range.
 * 
 * @param other The vector to assign from.
 * @return A reference to this vector.
 */
vec4state& vec4state::operator=(const vec4state& other) {
    if (this == &other) {
        return *this;
    }
    // Change the value of unknown because assigning another value.
    unknown = false;
    // Setting this vector's bits to the other vector's bits.
    for (long long i = 0; i < min(this->vectorSize, other.vectorSize); i++) {
        this->vector[i] = other.vector[i];
        // If the other's vector has unknown bits, this vector has unknown bits.
        if (vector[i].getBval() != 0) {
            unknown = true;
        }
    }
    // In case the other vector is shorter than this vector, put 0's in the remaining cells.
    if (other.vectorSize < vectorSize) {
        for (long long i = other.vectorSize; i < vectorSize; i++) {
            vector[i].setAval(0);
            vector[i].setBval(0);
        }
    }
    // Zero down the bits that are out of range.
    if (numBits % BITS_IN_VPI != 0) {
        vector[vectorSize - 1].setAval(vector[vectorSize - 1].getAval() & (MASK_32 >> (BITS_IN_VPI - (numBits % BITS_IN_VPI))));
        vector[vectorSize - 1].setBval(vector[vectorSize - 1].getBval() & (MASK_32 >> (BITS_IN_VPI - (numBits % BITS_IN_VPI))));
    }
    return *this;
}

// TODO: continue documenting from here
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
void zeroDownOutOfRangeBits(shared_ptr<VPI[]> vector, long long vectorSize, long long numBits) {
    long long indexLastCell = calcVectorSize(numBits) - 1;
    long long offset = numBits % BITS_IN_VPI;
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

vec4state vec4state::caseEquality(const vec4state& other) const {
    if (numBits < other.numBits) {
        vec4state copyThis = *this;
        copyThis.setNumBits(other.numBits);
        return copyThis.caseEquality(other);
    } else if (numBits > other.numBits) {
        vec4state copyOther = other;
        copyOther.setNumBits(numBits);
        return this->caseEquality(copyOther);
    } else {
        for (int i = 0; i < vectorSize; i++) {
        VPI currThisVPI = vector[i];
        VPI currOtherVPI = other.vector[i];
        if (currThisVPI.getAval() != currOtherVPI.getAval() || currThisVPI.getBval() != currOtherVPI.getBval()) {
            return vec4state(ZERO, 1);
        }
    }
    return vec4state(ONE, 1);
    }
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
    long long offset = num / BITS_IN_VPI;
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
        res.vector[i].setAval(currVPI.getAval() << (num % BITS_IN_VPI));
        res.vector[i].setBval(currVPI.getBval() << (num % BITS_IN_VPI));
        currVPI = res.vector[i];
        if (i > 0) {
            VPI prevVPI = res.vector[i - 1];
            res.vector[i].setAval(currVPI.getAval() | (prevVPI.getAval() >> (BITS_IN_VPI - (num % BITS_IN_VPI))));
            res.vector[i].setBval(currVPI.getBval() | (prevVPI.getBval() >> (BITS_IN_VPI - (num % BITS_IN_VPI))));
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
    long long offset = num / BITS_IN_VPI;
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
        res.vector[i].setAval(currVPI.getAval() >> (num % BITS_IN_VPI));
        res.vector[i].setBval(currVPI.getBval() >> (num % BITS_IN_VPI));
        currVPI = res.vector[i];
        if (i < vectorSize - 1) {
            VPI nextVPI = res.vector[i + 1];
            res.vector[i].setAval(currVPI.getAval() | (nextVPI.getAval() << (BITS_IN_VPI - (num % BITS_IN_VPI))));
            res.vector[i].setBval(currVPI.getBval() | (nextVPI.getBval() << (BITS_IN_VPI - (num % BITS_IN_VPI))));
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
    cout << "this: " << this->toString() << endl;
    // If input is invalid.
    if (end < start) {
        throw vec4stateExceptionInvalidRange(end, start);
    }
    // If the slice is completely out of range.
    if (end < 0 || start >= numBits) {
        return vec4state(X, end - start + 1);
    }

    // Extract the relevant VPIs of the vector.
    long long startVPIIndex = max(start / BITS_IN_VPI, long long (0));
    long long lastVPIIndex = end / BITS_IN_VPI;
    cout << "startVPIIndex: " << startVPIIndex << endl;
    cout << "lastVPIIndex: " << lastVPIIndex << endl;
    vec4state result = vec4state(ZERO, end - start + 1);
    cout << "result size: " << result.numBits << endl;
    // Copy the relevant VPIs to the result vector.
    for(long long currVPIIndex = startVPIIndex; currVPIIndex <= lastVPIIndex; currVPIIndex++) {
        VPI currVPI;
        if (currVPIIndex >= vectorSize) {
            currVPI.setAval(0);
            currVPI.setBval(0);
        } else {
            currVPI = vector[currVPIIndex];
        }
        result.vector[currVPIIndex - startVPIIndex].setAval(currVPI.getAval());
        result.vector[currVPIIndex - startVPIIndex].setBval(currVPI.getBval());
    }
    cout << "result: " << result.toString() << endl;
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
        long long startVPIIndex = start / BITS_IN_VPI;
        int offsetStart = start % BITS_IN_VPI;
        long long endVPIIndex = end / BITS_IN_VPI;
        int offsetEnd = end % BITS_IN_VPI;
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
            avalStartThis = currThisVPI.getAval() & (MASK_32 >> (BITS_IN_VPI - offsetStart));
            bvalStartThis = currThisVPI.getBval() & (MASK_32 >> (BITS_IN_VPI - offsetStart));
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
            if (currVPIIndex == endVPIIndex && offsetEnd != BITS_IN_VPI - 1) {
                VPI currOtherVPI = other.vector[endVPIIndex - startVPIIndex];
                VPI currThisVPI = vector[endVPIIndex];
                // Leave the bits from the original vector that are not in the slice.
                uint32_t avalEndThis = currThisVPI.getAval() & (MASK_32 << (offsetEnd + 1));
                uint32_t bvalEndThis = currThisVPI.getBval() & (MASK_32 << (offsetEnd + 1));
                // Get the bits from the other vector that are in the slice.
                uint32_t avalEndOther = currOtherVPI.getAval() & (MASK_32 >> (BITS_IN_VPI - offsetEnd - 1));
                uint32_t bvalEndOther = currOtherVPI.getBval() & (MASK_32 >> (BITS_IN_VPI - offsetEnd - 1));
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
        uint32_t oneBits = currVPI.getOneBits();
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
                carry = newResult >> BITS_IN_VPI;
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
    if (!other) {
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
    if (!other) {
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
            if (i == vectorSize - 1 && numBits % BITS_IN_VPI != 0 && j > (numBits % BITS_IN_VPI) - 1) {
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
        uint32_t oneBits = vector[i].getOneBits();
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
        vector[i].setAval(vector[i].getOneBits());
        // Zero down the unknown bits.
        vector[i].setBval(0);
    }
    unknown = false;
}

long long vec4state::getNumBits() const {
    return numBits;
}

shared_ptr<VPI[]> vec4state::getVector() const {
    return vector;
}

long long vec4state::getVectorSize() const {
    return vectorSize;
}

bool vec4state::isUnknown() const {
    return unknown;
}