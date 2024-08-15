#include <gtest/gtest.h>
#include "vec4state.h"

class vec4stateTest : public testing::Test {
protected:
    vec4state defaultVector;
    vec4state intVector = vec4state(0x12345678);
    vec4state longLongVector = vec4state(0x1234567890ABCDEF);
    vec4state stringVector = vec4state("01xz11");
    vec4state bigVector = vec4state("0110011xzx0111zzzx0110011xzx0111zzzx0110011xzx0111zzzx0110011xzx0111zzzx0110011xzx0111zzzx0110011xzx0111zzzx");
    vec4state onesVector = vec4state("11");
    vec4state xThenOnesVector = vec4state("x11");
    vec4state xVector = vec4state("xx");
    vec4state zVector = vec4state("zz");
    vec4state xzVector = vec4state("xz");
    vec4state oneAndXVector = vec4state("1x");
    vec4state oneAndZVector = vec4state("1z");
    vec4state zeroAndXVector = vec4state("0x");
    vec4state zeroAndZVector = vec4state("0z");
    vec4state zeroesVector = vec4state("00");
    vec4state zThenZeroesVector = vec4state("z00");
    vec4state negativeVector = vec4state(-1);
};

// Compares the string representation of the vector with the given string.
testing::AssertionResult compareVectorToString(vec4state& vector, string& str) {
    if (vector.toString().compare(str) == 0) {
        return testing::AssertionSuccess();
    } else {
        return testing::AssertionFailure() << "Expected: " << str << "\nActual: " << vector.toString();
    }
}

// Compares the size of the vector with the given size.
testing::AssertionResult checkVectorSize(vec4state& vector, long long size) {
    if (vector.getNumBits() == size) {
        return testing::AssertionSuccess();
    } else {
        return testing::AssertionFailure() << "Expected: " << size << "\nActual: " << vector.getNumBits();
    }
}

// Checks that the default constructor creates a 1-bit vector that stores the value x.
TEST_F(vec4stateTest, TestDefaultConstructor) {
    vec4state testVector;
    EXPECT_TRUE(compareVectorToString(testVector, string("x")));
    EXPECT_TRUE(checkVectorSize(testVector, 1));
}

// Checks that the constructor creates a 32-bit vector that stores the value of the integer passed to it.
TEST_F(vec4stateTest, TestIntegerConstructor) {
    vec4state testVector = vec4state(0x12345678);
    EXPECT_TRUE(compareVectorToString(testVector, string("00010010001101000101011001111000")));
    EXPECT_TRUE(checkVectorSize(testVector, 32));
}

// Checks that the constructor creates a 64-bit vector that stores the value of the long long number passed to it.
TEST_F(vec4stateTest, TestLongLongConstructor) {
    vec4state testVector = vec4state(0x1234567890ABCDEF);
    EXPECT_TRUE(compareVectorToString(testVector, string("0001001000110100010101100111100010010000101010111100110111101111")));
    EXPECT_TRUE(checkVectorSize(testVector, 64));
}

// Checks that the constructor creates a vector that has the same value as the string passed to it and has the same number of bits as the string's length. If the string is empty, the vector should hold 1 bit that stores the value x.
TEST_F(vec4stateTest, TestStringConstructor) {
    vec4state sixBitVector = vec4state("01xz11");
    EXPECT_TRUE(compareVectorToString(sixBitVector, string("01xz11")));
    EXPECT_TRUE(checkVectorSize(sixBitVector, 6));

    vec4state emptyVector = vec4state("");
    EXPECT_TRUE(compareVectorToString(emptyVector, string("x")));
    EXPECT_TRUE(checkVectorSize(emptyVector, 1));

    vec4state hundredAndEightBitVector = vec4state("0110011xzx0111zzzx0110011xzx0111zzzx0110011xzx0111zzzx0110011xzx0111zzzx0110011xzx0111zzzx0110011xzx0111zzzx");
    EXPECT_TRUE(compareVectorToString(hundredAndEightBitVector, string("0110011xzx0111zzzx0110011xzx0111zzzx0110011xzx0111zzzx0110011xzx0111zzzx0110011xzx0111zzzx0110011xzx0111zzzx")));
    EXPECT_TRUE(checkVectorSize(hundredAndEightBitVector, 108));
}

// Checks that the copy constructor creates a vector that has the same value and number of bits as the vector passed to it.
TEST_F(vec4stateTest, TestCopyConstructor) {
    vec4state copyVector = stringVector;
    EXPECT_TRUE(compareVectorToString(copyVector, string("01xz11")));
    EXPECT_TRUE(checkVectorSize(copyVector, 6));
}

// Checks that the move constructor transfers the ownership of the original vector's resources to the destination vector.
TEST_F(vec4stateTest, TestMoveConstructor) {
    EXPECT_TRUE(compareVectorToString(stringVector, string("01xz11")));
    vec4state moveVector = move(stringVector);
    EXPECT_EQ(stringVector.getVector(), nullptr);
    EXPECT_TRUE(compareVectorToString(moveVector, string("01xz11")));
    EXPECT_TRUE(checkVectorSize(moveVector, 6));
}

// Checks that the assignment operator from another vector changes the destination vector to hold the same values as the vector assigned to it.
TEST_F(vec4stateTest, TestAssignmentFromVector) {
    vec4state copy;
    EXPECT_TRUE(compareVectorToString(copy, string("x")));
    EXPECT_TRUE(checkVectorSize(copy, 1));
    copy = stringVector;
    EXPECT_TRUE(compareVectorToString(copy, string("01xz11")));
    EXPECT_TRUE(checkVectorSize(copy, 6));
}

// Checks that the assignment operator from an integer creates a 32-bit vector that stores the value of the integer passed to it.
TEST_F(vec4stateTest, TestIntegerAssignment) {
    vec4state copy;
    EXPECT_TRUE(compareVectorToString(copy, string("x")));
    EXPECT_TRUE(checkVectorSize(copy, 1));
    copy = 0x12345678;
    EXPECT_TRUE(compareVectorToString(copy, string("00010010001101000101011001111000")));
    EXPECT_TRUE(checkVectorSize(copy, 32));
}

// Checks that the assignment operator from a long long number creates a 64-bit vector that stores the value of the long long number passed to it.
TEST_F(vec4stateTest, TestLongLongAssignment) {
    vec4state copy;
    EXPECT_TRUE(compareVectorToString(copy, string("x")));
    EXPECT_TRUE(checkVectorSize(copy, 1));
    copy = 0x1234567890ABCDEF;
    EXPECT_TRUE(compareVectorToString(copy, string("0001001000110100010101100111100010010000101010111100110111101111")));
    EXPECT_TRUE(checkVectorSize(copy, 64));
}

// Checks that the assignment operator from a string creates a vector that has the same value as the string passed to it and it's number of bits is equal to the length of the string.
TEST_F(vec4stateTest, TestStringAssignment) {
    vec4state copy;
    EXPECT_TRUE(compareVectorToString(copy, string("x")));
    EXPECT_TRUE(checkVectorSize(copy, 1));
    copy = "01xz11";
    EXPECT_TRUE(compareVectorToString(copy, string("01xz11")));
    EXPECT_TRUE(checkVectorSize(copy, 6));
}

// Checks that the bitwise AND operator between a vector that holds an integer with itself creates the same vector.
TEST_F(vec4stateTest, TestBitwiseAndIntVectorWithItself) {
    vec4state andVector = intVector & intVector;
    EXPECT_TRUE(compareVectorToString(andVector, string("00010010001101000101011001111000")));
    EXPECT_TRUE(checkVectorSize(andVector, 32));
}

// Checks that the bitwise AND operator between a vector that holds a long long number with itself creates the same vector.
TEST_F(vec4stateTest, TestBitwiseAndLongLongVectorWithItself) {
    vec4state andVector = longLongVector & longLongVector;
    EXPECT_TRUE(compareVectorToString(andVector, string("0001001000110100010101100111100010010000101010111100110111101111")));
    EXPECT_TRUE(checkVectorSize(andVector, 64));
}

// Checks that the bitwise AND operator between a vector that holds an integer with a vector that holds a long long number creates a vector that holds the result of the bitwise AND operation between the two numbers. The result should have the same number of bits as the longer vector.
TEST_F(vec4stateTest, TestBitwiseAndIntVectorWithLongLongVector) {
    vec4state andVector = intVector & longLongVector;
    EXPECT_TRUE(compareVectorToString(andVector, string("0000000000000000000000000000000000010000001000000100010001101000")));
    EXPECT_TRUE(checkVectorSize(andVector, 64));
}

// Checks that the bitwise AND operator between a vector that holds an integer with a long long number creates a vector that holds the result of the bitwise AND operation between the two numbers. The result should have the same number of bits as the longer number.
TEST_F(vec4stateTest, TestBitwiseAndIntVectorWithLongLong) {
    vec4state andVector = intVector & 0x1234567890ABCDEF;
    EXPECT_TRUE(compareVectorToString(andVector, string("0000000000000000000000000000000000010000001000000100010001101000")));
    EXPECT_TRUE(checkVectorSize(andVector, 64));
}

// Checks that the bitwise AND operator between a vector that holds an integer with the integer inside the vector creates the vector itself.
TEST_F(vec4stateTest, TestBitwiseAndIntVectorWithInteger) {
    vec4state andVector = intVector & 0x12345678;
    EXPECT_TRUE(compareVectorToString(andVector, string("00010010001101000101011001111000")));
    EXPECT_TRUE(checkVectorSize(andVector, 32));
}

// Checks that the bitwise AND operator between a vector that stores unknown bits with itself creates a vector in which every unknown value becomes x.
TEST_F(vec4stateTest, TestBitwiseAndStringVectorWithItself) {
    vec4state andVector = stringVector & stringVector;
    EXPECT_TRUE(compareVectorToString(andVector, string("01xx11")));
    EXPECT_TRUE(checkVectorSize(andVector, 6));
}

// Checks that the bitwise AND operator between two unknown vectors with different amount of bits creates a vector that holds the result of the bitwise AND operation between the two vectors (unknown values become x). The result should have the same number of bits as the longer vector.
TEST_F(vec4stateTest, TestBitwiseAndSmallVectorWithBigVector) {
    vec4state andVector = stringVector & bigVector;
    EXPECT_TRUE(compareVectorToString(andVector, string("00000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000001xxxx")));
    EXPECT_TRUE(checkVectorSize(andVector, 108));
}

// Checks that the bitwise OR operator between a vector that holds an integer with itself creates the same vector.
TEST_F(vec4stateTest, TestBitwiseOrIntVectorWithItself) {
    vec4state orVector = intVector | intVector;
    EXPECT_TRUE(compareVectorToString(orVector, string("00010010001101000101011001111000")));
    EXPECT_TRUE(checkVectorSize(orVector, 32));
}

// Checks that the bitwise OR operator between a vector that holds a long long number with itself creates the same vector.
TEST_F(vec4stateTest, TestBitwiseOrLongLongVectorWithItself) {
    vec4state orVector = longLongVector | longLongVector;
    EXPECT_TRUE(compareVectorToString(orVector, string("0001001000110100010101100111100010010000101010111100110111101111")));
    EXPECT_TRUE(checkVectorSize(orVector, 64));
}

// Checks that the bitwise OR operator between a vector that holds an integer with a vector that holds a long long number creates a vector that holds the result of the bitwise OR operation between the two numbers. The result should have the same number of bits as the longer vector.
TEST_F(vec4stateTest, TestBitwiseOrIntVectorWithLongLongVector) {
    vec4state orVector = intVector | longLongVector;
    EXPECT_TRUE(compareVectorToString(orVector, string("0001001000110100010101100111100010010010101111111101111111111111")));
    EXPECT_TRUE(checkVectorSize(orVector, 64));
}

// Checks that the bitwise OR operator between a vector that holds an integer with a long long number creates a vector that holds the result of the bitwise OR operation between the two numbers. The result should have the same number of bits as the longer number.
TEST_F(vec4stateTest, TestBitwiseOrIntVectorWithLongLong) {
    vec4state orVector = intVector | 0x1234567890ABCDEF;
    EXPECT_TRUE(compareVectorToString(orVector, string("0001001000110100010101100111100010010010101111111101111111111111")));
    EXPECT_TRUE(checkVectorSize(orVector, 64));
}

// Checks that the bitwise OR operator between a vector that holds an integer with the integer inside the vector creates the vector itself.
TEST_F(vec4stateTest, TestBitwiseOrIntVectorWithInteger) {
    vec4state orVector = intVector | 0x12345678;
    EXPECT_TRUE(compareVectorToString(orVector, string("00010010001101000101011001111000")));
    EXPECT_TRUE(checkVectorSize(orVector, 32));
}

// Checks that the bitwise OR operator between a vector that stores unknown bits with itself creates a vector in which every unknown value becomes x.
TEST_F(vec4stateTest, TestBitwiseOrStringVectorWithItself) {
    vec4state orVector = stringVector | stringVector;
    EXPECT_TRUE(compareVectorToString(orVector, string("01xx11")));
    EXPECT_TRUE(checkVectorSize(orVector, 6));
}

// Checks that the bitwise OR operator between two unknown vectors with different amount of bits creates a vector that holds the result of the bitwise OR operation between the two vectors (unknown values become x). The result should have the same number of bits as the longer vector.
TEST_F(vec4stateTest, TestBitwiseOrSmallVectorWithBigVector) {
    vec4state orVector = stringVector | bigVector;
    EXPECT_TRUE(compareVectorToString(orVector, string("0110011xxx0111xxxx0110011xxx0111xxxx0110011xxx0111xxxx0110011xxx0111xxxx0110011xxx0111xxxx0110011xxx0111xx11")));
    EXPECT_TRUE(checkVectorSize(orVector, 108));
}

// Checks that the bitwise XOR operator between a vector that holds an integer with itself creates a vector of zeroes.
TEST_F(vec4stateTest, TestBitwiseXorIntVectorWithItself) {
    vec4state xorVector = intVector ^ intVector;
    EXPECT_TRUE(compareVectorToString(xorVector, string("00000000000000000000000000000000")));
    EXPECT_TRUE(checkVectorSize(xorVector, 32));
}

// Checks that the bitwise XOR operator between a vector that holds a long long number with itself creates a vector of zeroes.
TEST_F(vec4stateTest, TestBitwiseXorLongLongVectorWithItself) {
    vec4state xorVector = longLongVector ^ longLongVector;
    EXPECT_TRUE(compareVectorToString(xorVector, string("0000000000000000000000000000000000000000000000000000000000000000")));
    EXPECT_TRUE(checkVectorSize(xorVector, 64));
}

// Checks that the bitwise XOR operator between a vector that holds an integer with a vector that holds a long long number creates a vector that holds the result of the bitwise XOR operation between the two numbers. The result should have the same number of bits as the longer vector.
TEST_F(vec4stateTest, TestBitwiseXorIntVectorWithLongLongVector) {
    vec4state xorVector = intVector ^ longLongVector;
    EXPECT_TRUE(compareVectorToString(xorVector, string("0001001000110100010101100111100010000010100111111001101110010111")));
    EXPECT_TRUE(checkVectorSize(xorVector, 64));
}

// Checks that the bitwise XOR operator between a vector that holds an integer with a long long number creates a vector that holds the result of the bitwise XOR operation between the two numbers. The result should have the same number of bits as the longer number.
TEST_F(vec4stateTest, TestBitwiseXorIntVectorWithLongLong) {
    vec4state xorVector = intVector ^ 0x1234567890ABCDEF;
    EXPECT_TRUE(compareVectorToString(xorVector, string("0001001000110100010101100111100010000010100111111001101110010111")));
    EXPECT_TRUE(checkVectorSize(xorVector, 64));
}

// Checks that the bitwise XOR operator between a vector that holds an integer with the integer inside the vector creates a vector of zeroes.
TEST_F(vec4stateTest, TestBitwiseXorIntVectorWithInteger) {
    vec4state xorVector = intVector ^ 0x12345678;
    EXPECT_TRUE(compareVectorToString(xorVector, string("00000000000000000000000000000000")));
    EXPECT_TRUE(checkVectorSize(xorVector, 32));
}

// Checks that the bitwise XOR operator between a vector that stores unknown bits with itself creates a vector in which every unknown value becomes x and every known bit becomes 0.
TEST_F(vec4stateTest, TestBitwiseXorStringVectorWithItself) {
    vec4state xorVector = stringVector ^ stringVector;
    EXPECT_TRUE(compareVectorToString(xorVector, string("00xx00")));
    EXPECT_TRUE(checkVectorSize(xorVector, 6));
}

// Checks that the bitwise XOR operator between two unknown vectors with different amount of bits creates a vector that holds the result of the bitwise XOR operation between the two vectors. The result should have the same number of bits as the longer vector.
TEST_F(vec4stateTest, TestBitwiseXorSmallVectorWithBigVector) {
    vec4state xorVector = stringVector ^ bigVector;
    EXPECT_TRUE(compareVectorToString(xorVector, string("0110011xxx0111xxxx0110011xxx0111xxxx0110011xxx0111xxxx0110011xxx0111xxxx0110011xxx0111xxxx0110011xxx0110xxxx")));
    EXPECT_TRUE(checkVectorSize(xorVector, 108));
}

// Checks that the bitwise NOT operator on a vector that holds a long long number creates a vector that holds the bitwise NOT of the number.
TEST_F(vec4stateTest, TestBitwiseNotLongLongVector) {
    vec4state notVector = ~longLongVector;
    EXPECT_TRUE(compareVectorToString(notVector, string("1110110111001011101010011000011101101111010101000011001000010000")));
    EXPECT_TRUE(checkVectorSize(notVector, 64));
}

// Checks that the bitwise NOT operator on a vector that holds unknown values creates a vector that holds the bitwise NOT of the original vector, where the unknown values become x.
TEST_F(vec4stateTest, TestBitwiseNotStringVector) {
    vec4state notVector = ~stringVector;
    EXPECT_TRUE(compareVectorToString(notVector, string("10xx00")));
    EXPECT_TRUE(checkVectorSize(notVector, 6));
}

// Checks that a vector that holds an integer is logically equal to itself.
TEST_F(vec4stateTest, TestIntVectorEqualityWithItself) {
    EXPECT_TRUE(intVector == intVector);
}

// Checks that a vector that holds an integer is logically equal to the integer inside the vector.
TEST_F(vec4stateTest, TestIntVectorEqualityWithInteger) {
    EXPECT_TRUE(intVector == 0x12345678);
}

// Checks that a vector that holds a long long number is logically equal to itself.
TEST_F(vec4stateTest, TestLongLongVectorEqualityWithItself) {
    EXPECT_TRUE(longLongVector == longLongVector);
}

// Checks that a vector that holds a long long number is logically equal to the long long number inside the vector.
TEST_F(vec4stateTest, TestLongLongVectorEqualityWithLongLong) {
    EXPECT_TRUE(longLongVector == 0x1234567890ABCDEF);
}

// Checks that the result of logical equality between a 6-bit vector that holds unknown values and itself returns an unknown value.
TEST_F(vec4stateTest, TestStringVectorEqualityWithItself) {
    vec4state eqVector = stringVector == stringVector;
    EXPECT_TRUE(compareVectorToString(eqVector, string("x")));
    EXPECT_TRUE(checkVectorSize(eqVector, 1));
    EXPECT_ANY_THROW(bool result = stringVector == stringVector);
}

// Checks that the result of logical equality between a 108-bit vector that holds unknown values and itself returns an unknown value.
TEST_F(vec4stateTest, TestBigVectorEqualityWithItself) {
    vec4state eqVector = bigVector == bigVector;
    EXPECT_TRUE(compareVectorToString(eqVector, string("x")));
    EXPECT_TRUE(checkVectorSize(eqVector, 1));
    EXPECT_ANY_THROW(bool result = bigVector == bigVector);
}

// Checks that the result of logical equality between a vector that holds an integer and a vector that holds a long long number (that have common lower 32-bits) is false.
TEST_F(vec4stateTest, TestIntVectorEqualityWithLongLongVector) {
    EXPECT_FALSE(intVector == longLongVector);
}

// Checks that the result of logical equality between a vector that holds an integer and a vector that holds unknown values (that have different values in the bits that are known) returns false.
TEST_F(vec4stateTest, TestStringVectorEqualityWithIntVector) {
    EXPECT_FALSE(stringVector == intVector);
}

// Checks that the result of logical equality between an unknown vector and a vector that holds a long long number (that have different values in the bits that are known) returns false.
TEST_F(vec4stateTest, TestStringVectorEqualityWithLongLongVector) {
    EXPECT_FALSE(stringVector == longLongVector);
}

// Checks that the result of logical equality between two unknown vectors of different sizes returns false.
TEST_F(vec4stateTest, TestSmallVectorEqualityWithBigVector) {
    EXPECT_FALSE(stringVector == bigVector);
    EXPECT_FALSE(bigVector == stringVector);
}

/// Checks that the result of logical equality between a big unknown vector and a vector that holds a long long number (that have different values in the bits that are known) returns false.
TEST_F(vec4stateTest, TestBigVectorEqualityWithIntVector) {
    EXPECT_FALSE(bigVector == intVector);
}

// Checks that the result of logical equality between a big unknown vector and a vector that holds a long long number (that have different values in the bits that are known) returns false.
TEST_F(vec4stateTest, TestBigVectorEqualityWithLongLongVector) {
    EXPECT_FALSE(bigVector == longLongVector);
}

// Checks that the result of logical equality between a vector that holds only ones and a vector that holds only x's is unknown.
TEST_F(vec4stateTest, TestOnesVectorEqualityWithXVector) {
    vec4state eqVector = onesVector == xVector;
    EXPECT_TRUE(compareVectorToString(eqVector, string("x")));
    EXPECT_TRUE(checkVectorSize(eqVector, 1));
    EXPECT_ANY_THROW(bool result = onesVector == xVector);
}

// Checks that the result of logical equality between a vector that holds only ones and a vector that holds only z's is unknown.
TEST_F(vec4stateTest, TestOnesVectorEqualityWithZVector) {
    vec4state eqVector = onesVector == zVector;
    EXPECT_TRUE(compareVectorToString(eqVector, string("x")));
    EXPECT_TRUE(checkVectorSize(eqVector, 1));
    EXPECT_ANY_THROW(bool result = onesVector == zVector);
}

// Checks that the result of logical equality between a vector that holds two 1's and a vector that holds a one and an x is unknown.
TEST_F(vec4stateTest, TestOnesVectorEqualityWithOneAndXVector) {
    vec4state eqVector = onesVector == oneAndXVector;
    EXPECT_TRUE(compareVectorToString(eqVector, string("x")));
    EXPECT_TRUE(checkVectorSize(eqVector, 1));
    EXPECT_ANY_THROW(bool result = onesVector == oneAndXVector);
}

// Checks that the result of logical equality between a vector that holds two 1's and a vector that holds a zero and a z is false.
TEST_F(vec4stateTest, TestOnesVectorEqualityWithZeroAndZVector) {
    EXPECT_FALSE(onesVector == zeroAndZVector);
}

// Checks that the result of logical equality between a vector that holds only x's and a vector that holds only z's is unknown.
TEST_F(vec4stateTest, TestXVectorEqualityWithZVector) {
    vec4state eqVector = xVector == zVector;
    EXPECT_TRUE(compareVectorToString(eqVector, string("x")));
    EXPECT_TRUE(checkVectorSize(eqVector, 1));
    EXPECT_ANY_THROW(bool result = xVector == zVector);
}

// Checks that the result of logical equality between a 1's vector and the same vector with x in it's MSB is unknown.
TEST_F(vec4stateTest, TestOnesVectorEqualityWithXThenOnesVector) {
    vec4state eqVector = onesVector == xThenOnesVector;
    EXPECT_TRUE(compareVectorToString(eqVector, string("x")));
    EXPECT_TRUE(checkVectorSize(eqVector, 1));
    EXPECT_ANY_THROW(bool result = onesVector == xThenOnesVector);
}

// Checks that the result of logical equality between a 0's vector and the same vector with z in it's MSB is unknown.
TEST_F(vec4stateTest, TestZeroesVectorEqualityWithZThenZeroesVector) {
    vec4state eqVector = zeroesVector == zThenZeroesVector;
    EXPECT_TRUE(compareVectorToString(eqVector, string("x")));
    EXPECT_TRUE(checkVectorSize(eqVector, 1));
    EXPECT_ANY_THROW(bool result = zeroesVector == zThenZeroesVector);
}

// Checks that the result of logical inequality between a vector that holds an integer and itself is false.
TEST_F(vec4stateTest, TestIntVectorInequalityWithItself) {
    EXPECT_FALSE(intVector != intVector);
}

// Checks that the result of logical inequality between a vector that holds an integer and the integer inside the vector is false.
TEST_F(vec4stateTest, TestIntVectorInequalityWithInteger) {
    EXPECT_FALSE(intVector != 0x12345678);
}

// Checks that the result of logical inequality between a vector that holds an integer and a long long number is true.
TEST_F(vec4stateTest, TestIntVectorInequalityWithLongLong) {
    EXPECT_TRUE(intVector != 0x1234567890ABCDEF);
}

// Checks that the result of logical inequality between a vector that holds a long long number and an unknown vector (with no common known bits) is false.
TEST_F(vec4stateTest, TestLongLongVectorInequalityWithBigVector) {
    EXPECT_TRUE(longLongVector != bigVector);
}

// Checks that the result of logical inequality between two unknown vectors of different sizes is true.
TEST_F(vec4stateTest, TestSmallVectorInequalityWithBigVector) {
    EXPECT_TRUE(stringVector != bigVector);
}

// Checks that the result of logical inequality between an unknown vector with itself is unknown.
TEST_F(vec4stateTest, TestStringVectorInequalityWithItself) {
    vec4state neqVector = stringVector != stringVector;
    EXPECT_TRUE(compareVectorToString(neqVector, string("x")));
    EXPECT_TRUE(checkVectorSize(neqVector, 1));
    EXPECT_ANY_THROW(bool result = stringVector != stringVector);
}

// Checks that the result of logical inequality between a vector that holds only ones and a vector that holds only x's is unknown.
TEST_F(vec4stateTest, TestOnesVectorInequalityWithXVector) {
    vec4state neqVector = onesVector != xVector;
    EXPECT_TRUE(compareVectorToString(neqVector, string("x")));
    EXPECT_TRUE(checkVectorSize(neqVector, 1));
    EXPECT_ANY_THROW(bool result = onesVector != xVector);
}

// Checks that the result of logical inequality between a vector that holds only ones and a vector that holds only z's is unknown.
TEST_F(vec4stateTest, TestOnesVectorInequalityWithZVector) {
    vec4state neqVector = onesVector != zVector;
    EXPECT_TRUE(compareVectorToString(neqVector, string("x")));
    EXPECT_TRUE(checkVectorSize(neqVector, 1));
    EXPECT_ANY_THROW(bool result = onesVector != zVector);
}

// Checks that the result of logical inequality between a vector that holds two 1's and a vector that holds a one and an x is unknown.
TEST_F(vec4stateTest, TestOnesVectorInequalityWithOneAndXVector) {
    vec4state neqVector = onesVector != oneAndXVector;
    EXPECT_TRUE(compareVectorToString(neqVector, string("x")));
    EXPECT_TRUE(checkVectorSize(neqVector, 1));
    EXPECT_ANY_THROW(bool result = onesVector != oneAndXVector);
}

// Checks that the result of logical inequality between a vector that holds two 1's and a vector that holds a zero and a z is true.
TEST_F(vec4stateTest, TestOnesVectorInequalityWithZeroesAndXVector) {
    EXPECT_TRUE(onesVector != zeroAndZVector);
}

// Checks that the result of logical inequality between a vector that holds only x's and a vector that holds only z's is unknown.
TEST_F(vec4stateTest, TestXVectorInequalityWithZVector) {
    vec4state neqVector = xVector != zVector;
    EXPECT_TRUE(compareVectorToString(neqVector, string("x")));
    EXPECT_TRUE(checkVectorSize(neqVector, 1));
    EXPECT_ANY_THROW(bool result = xVector != zVector);
}

// Checks that the result of case equality between a vector that holds an integer and itself is true.
TEST_F(vec4stateTest, TestIntVectorCaseEqualityWithItself) {
    EXPECT_TRUE(intVector.caseEquality(intVector));
}

// Checks that the result of case equality between a vector that holds an integer and the integer inside the vector is true.
TEST_F(vec4stateTest, TestIntVectorCaseEqualityWithInteger) {
    EXPECT_TRUE(intVector.caseEquality(0x12345678));
}

// Checks that the result of case equality between an unknown vector and itself is true.
TEST_F(vec4stateTest, TestStringVectorCaseEqualityWithItself) {
    EXPECT_TRUE(stringVector.caseEquality(stringVector));
}

// Checks that the result of case equality between two unknown vectors of different sizes is false.
TEST_F(vec4stateTest, TestSmallVectorCaseEqualityWithBigVector) {
    EXPECT_FALSE(stringVector.caseEquality(bigVector));
}

// Checks that the result of case equality between a vector that holds only ones and a vector that holds only x's is false.
TEST_F(vec4stateTest, TestOnesVectorCaseEqualityWithXVector) {
    EXPECT_FALSE(onesVector.caseEquality(xVector));
}

// Checks that the result of case equality between a vector that holds only x's and a vector that holds only z's is false.
TEST_F(vec4stateTest, TestXVectorCaseEqualityWithZVector) {
    EXPECT_FALSE(xVector.caseEquality(zVector));
}

// Checks that the result of case inequality between a vector that holds an integer and itself is false.
TEST_F(vec4stateTest, TestIntVectorCaseInequalityWithItself) {
    EXPECT_FALSE(intVector.caseInequality(intVector));
}

// Checks that the result of case inequality between a vector that holds an integer and the integer inside the vector is false.
TEST_F(vec4stateTest, TestIntVectorCaseInequalityWithInteger) {
    EXPECT_FALSE(intVector.caseInequality(0x12345678));
}

// Checks that the result of case inequality between an unknown vector and itself is false.
TEST_F(vec4stateTest, TestStringVectorCaseInequalityWithItself) {
    EXPECT_FALSE(stringVector.caseInequality(stringVector));
}

// Checks that the result of case inequality between two unknown vectors of different sizes is true.
TEST_F(vec4stateTest, TestSmallVectorCaseInequalityWithBigVector) {
    EXPECT_TRUE(stringVector.caseInequality(bigVector));
}

// Checks that the result of case inequality between a vector that holds only ones and a vector that holds only x's is true.
TEST_F(vec4stateTest, TestOnesVectorCaseInequalityWithXVector) {
    EXPECT_TRUE(onesVector.caseInequality(xVector));
}

// Checks that the result of case inequality between a vector that holds only x's and a vector that holds only z's is true.
TEST_F(vec4stateTest, TestXVectorCaseInequalityWithZVector) {
    EXPECT_TRUE(xVector.caseInequality(zVector));
}

// Checks that the result of shifting a vector that holds an integer (32 bits) to the left by two bits is the same as the integer shifted to the left by two bits. The result should have the same number of bits as the original vector.
TEST_F(vec4stateTest, TestShiftLeftIntVectorByTwo) {
    vec4state shiftLeftVector = intVector << 2;
    EXPECT_TRUE(compareVectorToString(shiftLeftVector, string("01001000110100010101100111100000")));
    EXPECT_TRUE(checkVectorSize(shiftLeftVector, 32));
}

// Checks that the result of shifting a vector that holds an integer (32 bits) to the left by thirty two bits is a vector that holds only zeroes. The result should have the same number of bits as the original vector.
TEST_F(vec4stateTest, TestShiftLeftIntVectorByThirtyTwo) {
    vec4state shiftLeftVector = intVector << 32;
    EXPECT_TRUE(compareVectorToString(shiftLeftVector, string("00000000000000000000000000000000")));
    EXPECT_TRUE(checkVectorSize(shiftLeftVector, 32));
}

// Checks that the result of shifting a vector that holds a long long number (64 bits) to the left by two bits is the same as the long long number shifted to the left by two bits. The result should have the same number of bits as the original vector.
TEST_F(vec4stateTest, TestShiftLeftLongLongVectorByTwo) {
    vec4state shiftLeftVector = longLongVector << 2;
    EXPECT_TRUE(compareVectorToString(shiftLeftVector, string("0100100011010001010110011110001001000010101011110011011110111100")));
    EXPECT_TRUE(checkVectorSize(shiftLeftVector, 64));
}

// Checks that the result of shifting a vector that holds a long long number (64 bits) to the left by sixty four bits is a vector that holds only zeroes. The result should have the same number of bits as the original vector.
TEST_F(vec4stateTest, TestShiftLeftLongLongVectorBySixtyFour) {
    vec4state shiftLeftVector = longLongVector << 64;
    EXPECT_TRUE(compareVectorToString(shiftLeftVector, string("0000000000000000000000000000000000000000000000000000000000000000")));
    EXPECT_TRUE(checkVectorSize(shiftLeftVector, 64));
}

// Checks that the result of shifting a vector that holds a long long number (64 bits) to the left by thirty five bits is the same as the long long number shifted to the left by thirty five bits. The result should have the same number of bits as the original vector.
TEST_F(vec4stateTest, TestShiftLeftLongLongVectorByThirtyFive) {
    vec4state shiftLeftVector = longLongVector << 35;
    EXPECT_TRUE(compareVectorToString(shiftLeftVector, string("1000010101011110011011110111100000000000000000000000000000000000")));
    EXPECT_TRUE(checkVectorSize(shiftLeftVector, 64));
}

// Checks that the result of shifting an unknown vector that holds 6 bits is the same vector shifted to the right by 2 bits. The result should have the same number of bits as the original vector.
TEST_F(vec4stateTest, TestShiftLeftStringVectorByTwo) {
    vec4state shiftLeftVector = stringVector << 2;
    EXPECT_TRUE(compareVectorToString(shiftLeftVector, string("xz1100")));
    EXPECT_TRUE(checkVectorSize(shiftLeftVector, 6));
}

// Checks that the result of shifting an unknown vector that holds 108 bits to the left by seventy bits is the same vector shifted to the left by seventy bits. The result should have the same number of bits as the original vector.
TEST_F(vec4stateTest, TestShiftLeftBigVectorBySeventy) {
    vec4state shiftLeftVector = bigVector << 70;
    EXPECT_TRUE(compareVectorToString(shiftLeftVector, string("zx0110011xzx0111zzzx0110011xzx0111zzzx0000000000000000000000000000000000000000000000000000000000000000000000")));
    EXPECT_TRUE(checkVectorSize(shiftLeftVector, 108));
}

// Checks that the result of shifting a vector that holds 108 bits to the left by an unknown value is only x's. The result should have the same number of bits as the original vector.
TEST_F(vec4stateTest, TestShiftLeftBigVectorBySmallVector) {
    vec4state shiftLeftVector = bigVector << stringVector;
    EXPECT_TRUE(compareVectorToString(shiftLeftVector, string("xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx")));
    EXPECT_TRUE(checkVectorSize(shiftLeftVector, 108));
}

// Checks that the result of shifting an unknown vector that holds 108 bits to the left by a vector that holds the number 3 in binary is the same vector shifted to the left by 3. The result should have the same number of bits as the original vector.
TEST_F(vec4stateTest, TestShiftLeftBigVectorByOnesVector) {
    vec4state shiftLeftVector = bigVector << onesVector;
    EXPECT_TRUE(compareVectorToString(shiftLeftVector, string("0011xzx0111zzzx0110011xzx0111zzzx0110011xzx0111zzzx0110011xzx0111zzzx0110011xzx0111zzzx0110011xzx0111zzzx000")));
    EXPECT_TRUE(checkVectorSize(shiftLeftVector, 108));
}

// Checks that the result of shifting an unknown vector that holds 108 bits to the left by a vector that holds an integer gretaer than 108 is only zeroes. The result should have the same number of bits as the original vector.
TEST_F(vec4stateTest, TestShiftLeftBigVectorByIntVector) {
    vec4state shiftLeftVector = bigVector << intVector;
    EXPECT_TRUE(compareVectorToString(shiftLeftVector, string("000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000")));
    EXPECT_TRUE(checkVectorSize(shiftLeftVector, 108));
}

// Checks that the result of shifting a vector that holds an integer (32 bits) to the right by two bits is the same as the integer shifted to the right by two bits. The result should have the same number of bits as the original vector.
TEST_F(vec4stateTest, TestShiftRightIntVectorByTwo) {
    vec4state shiftRightVector = intVector >> 2;
    EXPECT_TRUE(compareVectorToString(shiftRightVector, string("00000100100011010001010110011110")));
    EXPECT_TRUE(checkVectorSize(shiftRightVector, 32));
}

// Checks that the result of shifting a vector that holds an integer (32 bits) to the right by thirty two bits is a vector that holds only zeroes. The result should have the same number of bits as the original vector.
TEST_F(vec4stateTest, TestShiftRightIntVectorByThirtyTwo) {
    vec4state shiftRightVector = intVector >> 32;
    EXPECT_TRUE(compareVectorToString(shiftRightVector, string("00000000000000000000000000000000")));
    EXPECT_TRUE(checkVectorSize(shiftRightVector, 32));
}

// Checks that the result of shifting a vector that holds a long long number (64 bits) to the right by two bits is the same as the long long number shifted to the right by two bits. The result should have the same number of bits as the original vector.
TEST_F(vec4stateTest, TestShiftRightLongLongVectorByTwo) {
    vec4state shiftRightVector = longLongVector >> 2;
    EXPECT_TRUE(compareVectorToString(shiftRightVector, string("0000010010001101000101011001111000100100001010101111001101111011")));
    EXPECT_TRUE(checkVectorSize(shiftRightVector, 64));
}

// Checks that the result of shifting a vector that holds a long long number (64 bits) to the right by sixty four bits is a vector that holds only zeroes. The result should have the same number of bits as the original vector.
TEST_F(vec4stateTest, TestShiftRightLongLongVectorBySixtyFour) {
    vec4state shiftRightVector = longLongVector >> 64;
    EXPECT_TRUE(compareVectorToString(shiftRightVector, string("0000000000000000000000000000000000000000000000000000000000000000")));
    EXPECT_TRUE(checkVectorSize(shiftRightVector, 64));
}

// Checks that the result of shifting a vector that holds a long long number (64 bits) to the right by thirty five bits is the same as the long long number shifted to the right by thirty five bits. The result should have the same number of bits as the original vector.
TEST_F(vec4stateTest, TestShiftRightLongLongVectorByThirtyFive) {
    vec4state shiftRightVector = longLongVector >> 35;
    EXPECT_TRUE(compareVectorToString(shiftRightVector, string("0000000000000000000000000000000000000010010001101000101011001111")));
    EXPECT_TRUE(checkVectorSize(shiftRightVector, 64));
}

// Checks that the result of shifting an unknown vector that holds 6 bits to the right by 2 is the same vector shifted to the right by 2 bits. The result should have the same number of bits as the original vector.
TEST_F(vec4stateTest, TestShiftRightStringVectorByTwo) {
    vec4state shiftRightVector = stringVector >> 2;
    EXPECT_TRUE(compareVectorToString(shiftRightVector, string("0001xz")));
    EXPECT_TRUE(checkVectorSize(shiftRightVector, 6));
}

// Checks that the result of shifting an unknown vector that holds 108 bits to the right by seventy bits is the same vector shifted to the right by seventy bits. The result should have the same number of bits as the original vector.
TEST_F(vec4stateTest, TestShiftRightBigVectorBySeventy) {
    vec4state shiftRightVector = bigVector >> 70;
    EXPECT_TRUE(compareVectorToString(shiftRightVector, string("00000000000000000000000000000000000000000000000000000000000000000000000110011xzx0111zzzx0110011xzx0111zzzx01")));
    EXPECT_TRUE(checkVectorSize(shiftRightVector, 108));
}

// Checks that the result of shifting an unknown vector that holds 108 bits to the right by an unknown vector is a vector that holds only x's. The result should have the same number of bits as the original vector.
TEST_F(vec4stateTest, TestShiftRightBigVectorBySmallVector) {
    vec4state shiftRightVector = bigVector >> stringVector;
    EXPECT_TRUE(compareVectorToString(shiftRightVector, string("xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx")));
    EXPECT_TRUE(checkVectorSize(shiftRightVector, 108));
}

// Checks that the result of shifting an unknown vector that holds 108 bits to the right by a vector that holds the number 3 in binary is the same vector shifted to the right by 3. The result should have the same number of bits as the original vector.
TEST_F(vec4stateTest, TestShiftRightBigVectorByOnesVector) {
    vec4state shiftRightVector = bigVector >> onesVector;
    EXPECT_TRUE(compareVectorToString(shiftRightVector, string("0000110011xzx0111zzzx0110011xzx0111zzzx0110011xzx0111zzzx0110011xzx0111zzzx0110011xzx0111zzzx0110011xzx0111z")));
    EXPECT_TRUE(checkVectorSize(shiftRightVector, 108));
}

// Checks that the result of shifting an unknown vector that holds 108 bits to the right by a vector that holds an integer gretaer than 108 is only zeroes. The result should have the same number of bits as the original vector.
TEST_F(vec4stateTest, TestShiftRightBigVectorByIntVector) {
    vec4state shiftRightVector = bigVector >> intVector;
    EXPECT_TRUE(compareVectorToString(shiftRightVector, string("000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000")));
    EXPECT_TRUE(checkVectorSize(shiftRightVector, 108));
}

// Checks that accessing the bit at index 2 of a vector that holds an integer (32 bits) returns the value of the third bit in the vector.
TEST_F(vec4stateTest, TestGetBitSelectIntVector) {
    vec4state indexVector = intVector.getBitSelect(2);
    EXPECT_TRUE(compareVectorToString(indexVector, string("0")));
    EXPECT_TRUE(checkVectorSize(indexVector, 1));
}

// Checks that accessing the bit at index 32 of a vector that holds an integer (32 bits) returns x (because it is out of bounds).
TEST_F(vec4stateTest, TestGetTooHighBitSelectIntVector) {
    vec4state indexVector = intVector.getBitSelect(32);
    EXPECT_TRUE(compareVectorToString(indexVector, string("x")));
    EXPECT_TRUE(checkVectorSize(indexVector, 1));
}

// Checks that accessing the bit at index -1 of a vector that holds an integer (32 bits) returns x (because it is out of bounds).
TEST_F(vec4stateTest, TestGetTooLowBitSelectIntVector) {
    vec4state indexVector = intVector.getBitSelect(-1);
    EXPECT_TRUE(compareVectorToString(indexVector, string("x")));
    EXPECT_TRUE(checkVectorSize(indexVector, 1));
}

// Checks that accessing the bit at index 2 of an unknown vector that holds 6 bits returns the value of the thirds bit in the vector.
TEST_F(vec4stateTest, TestGetBitSelectStringVector) {
    vec4state indexVector = stringVector.getBitSelect(2);
    EXPECT_TRUE(compareVectorToString(indexVector, string("z")));
    EXPECT_TRUE(checkVectorSize(indexVector, 1));
}

// Checks that accessing the bit at index 70 of an unknown vector that holds 108 bits returns the value of the seventy first bit in the vector.
TEST_F(vec4stateTest, TestGetBitSelectBigVector) {
    vec4state indexVector = bigVector.getBitSelect(70);
    EXPECT_TRUE(compareVectorToString(indexVector, string("1")));
    EXPECT_TRUE(checkVectorSize(indexVector, 1));
}

// Checks that accessing the bit at an unknown index (index is unknown vector) of a vector that holds an integer (32 bits) returns x.
TEST_F(vec4stateTest, TestGetUnknownBitSelectIntVector) {
    vec4state indexVector = intVector.getBitSelect(stringVector);
    EXPECT_TRUE(compareVectorToString(indexVector, string("x")));
    EXPECT_TRUE(checkVectorSize(indexVector, 1));
}

// Checks that setting the first bit of a vector that holds 00 to 1 changes the vector to 01. It also checks that the new vector is not equal to the original vector (both logical and case inequality).
TEST_F(vec4stateTest, TestSetBitSelectZeroesVector) {
    vec4state beforeSet = zeroesVector;
    zeroesVector.setBitSelect(0, 1);
    EXPECT_TRUE(compareVectorToString(zeroesVector, string("01")));
    EXPECT_TRUE(zeroesVector != beforeSet);
    EXPECT_TRUE(zeroesVector.caseInequality(beforeSet));
    EXPECT_TRUE(checkVectorSize(zeroesVector, 2));
}

// Checks that setting the thirty second bit of a vector that holds an integer (32 bits) to 1 doesn't change the vector (because it is out of bounds). It also checks that the new vector is equal to the original vector (both logical and case inequality).
TEST_F(vec4stateTest, TestSetTooHighBitSelectIntVector) {
    vec4state beforeSet = intVector;
    intVector.setBitSelect(32, 1);
    EXPECT_TRUE(compareVectorToString(intVector, string("00010010001101000101011001111000")));
    EXPECT_TRUE(intVector == beforeSet);
    EXPECT_TRUE(intVector.caseEquality(beforeSet));
    EXPECT_TRUE(checkVectorSize(intVector, 32));
}

// Checks that setting the negative first bit of a vector that holds an integer (32 bits) to 1 doesn't change the vector (because it is out of bounds). It also checks that the new vector is equal to the original vector (both logical and case inequality).
TEST_F(vec4stateTest, TestSetTooLowBitSelectIntVector) {
    vec4state beforeSet = intVector;
    intVector.setBitSelect(-1, 1);
    EXPECT_TRUE(compareVectorToString(intVector, string("00010010001101000101011001111000")));
    EXPECT_TRUE(intVector == beforeSet);
    EXPECT_TRUE(intVector.caseEquality(beforeSet));
    EXPECT_TRUE(checkVectorSize(intVector, 32));
}

// Checks that setting the second bit of an unknown vector that holds 6 bits to a vector to holds only one x changes the vector to the same vector with the second bit set to x. It also checks that the new vector is not equal to the original vector (both logical and case inequality).
TEST_F(vec4stateTest, TestSetBitSelectStringVectorToDefaultVector) {
    vec4state beforeSet = stringVector;
    stringVector.setBitSelect(1, defaultVector);
    EXPECT_TRUE(compareVectorToString(stringVector, string("01xzx1")));
    EXPECT_TRUE(stringVector.caseInequality(beforeSet));
    EXPECT_ANY_THROW(bool result = stringVector != beforeSet);
    EXPECT_TRUE(checkVectorSize(stringVector, 6));
}

// Checks that setting an unknown bit (index is unknown vector) of a vector that holds an integer (32 bits) to 1 doesn't change the vector. It also checks that the new vector is equal to the original vector (both logical and case inequality).
TEST_F(vec4stateTest, TestSetUnknownBitSelectIntVectorToInteger) {
    vec4state beforeSet = intVector;
    intVector.setBitSelect(stringVector, 1);
    EXPECT_TRUE(compareVectorToString(intVector, string("00010010001101000101011001111000")));
    EXPECT_TRUE(intVector == beforeSet);
    EXPECT_TRUE(intVector.caseEquality(beforeSet));
    EXPECT_TRUE(checkVectorSize(intVector, 32));
}

// Checks that setting the second bit of an unknown vector that holds 6 bits to 0x changes the vector to the same vector with the second bit set to x. It also checks that the new vector is not equal to the original vector (both logical and case inequality).
TEST_F(vec4stateTest, TestSetBitSelectStringVectorToZeroAndXVector) {
    vec4state beforeSet = stringVector;
    stringVector.setBitSelect(1, zeroAndXVector);
    EXPECT_TRUE(compareVectorToString(stringVector, string("01xzx1")));
    EXPECT_TRUE(stringVector.caseInequality(beforeSet));
    EXPECT_ANY_THROW(bool result = stringVector != beforeSet);
    EXPECT_TRUE(checkVectorSize(stringVector, 6));
}

// Checks that setting one of the bits of a known vector to x changes the vector to an unknown vector.
TEST_F(vec4stateTest, TestSetBitSelectKnownToUnknown) {
    vec4state beforeSet = onesVector;
    EXPECT_FALSE(onesVector.isUnknown());
    onesVector.setBitSelect(0, xVector);
    EXPECT_TRUE(compareVectorToString(onesVector, string("1x")));
    EXPECT_TRUE(onesVector.caseInequality(beforeSet));
    EXPECT_TRUE(onesVector.isUnknown());
}

// Checks that setting one of the bits of a known vector to z changes the vector to an unknown vector.
TEST_F(vec4stateTest, TestSetBitSelectKnownToUnknown) {
    vec4state beforeSet = onesVector;
    EXPECT_FALSE(onesVector.isUnknown());
    onesVector.setBitSelect(0, zVector);
    EXPECT_TRUE(compareVectorToString(onesVector, string("1z")));
    EXPECT_TRUE(onesVector.caseInequality(beforeSet));
    EXPECT_TRUE(onesVector.isUnknown());
}

// Checks that setting the only unknown bit of a vector to a known value changes the vector to a known vector.
TEST_F(vec4stateTest, TestSetBitSelectUnknownToKnown) {
    vec4state beforeSet = oneAndXVector;
    EXPECT_TRUE(oneAndXVector.isUnknown());
    oneAndXVector.setBitSelect(0, onesVector);
    EXPECT_TRUE(compareVectorToString(oneAndXVector, string("11")));
    EXPECT_TRUE(oneAndXVector.caseInequality(beforeSet));
    EXPECT_FALSE(oneAndXVector.isUnknown());
}

// Checks that getting the part select of a vector that holds an integer (32 bits) from index 2 to 0 returns a vector that holds the third, second and first bits of the original vector.
TEST_F(vec4stateTest, TestGetPartSelectIntVector) {
    vec4state sliceVector = intVector.getPartSelect(2, 0);
    EXPECT_TRUE(compareVectorToString(sliceVector, string("000")));
    EXPECT_TRUE(checkVectorSize(sliceVector, 3));
}

// Checks that getting the part select of a vector that holds an integer (32 bits) from index 31 to 0 returns the same vector.
TEST_F(vec4stateTest, TestGetPartSelectWholeIntVector) {
    vec4state sliceVector = intVector.getPartSelect(31, 0);
    EXPECT_TRUE(compareVectorToString(sliceVector, string("00010010001101000101011001111000")));
    EXPECT_TRUE(sliceVector == intVector);
    EXPECT_TRUE(checkVectorSize(sliceVector, 32));
}

// Checks that getting the part select of a vector that holds an integer (32 bits) from index 33 to 0 returns a vector that holds the original vector and adds two x bits in the end (because they are out of bounds).
TEST_F(vec4stateTest, TestGetPartSelectTooHighEndIntVector) {
    vec4state sliceVector = intVector.getPartSelect(33, 0);
    EXPECT_FALSE(intVector.isUnknown());
    EXPECT_TRUE(compareVectorToString(sliceVector, string("xx00010010001101000101011001111000")));
    EXPECT_TRUE(checkVectorSize(sliceVector, 34));
    EXPECT_TRUE(sliceVector.isUnknown());
}

// Checks that getting the part select of a vector that holds an integer (32 bits) from index 5 to -5 returns a vector that holds the six starting bits of the original vector and adds five x bits in the beginning (because they are out of bounds).
TEST_F(vec4stateTest, TestGetPartSelectTooLowStartIntVector) {
    vec4state sliceVector = intVector.getPartSelect(5, -5);
    EXPECT_FALSE(intVector.isUnknown());
    EXPECT_TRUE(compareVectorToString(sliceVector, string("111000xxxxx")));
    EXPECT_TRUE(checkVectorSize(sliceVector, 11));
    EXPECT_TRUE(sliceVector.isUnknown());
}

// Checks that getting the part select of a vector that holds an integer (32 bits) from index 32 to -5 returns a vector that holds the original vector and adds one x bit in the end and 5 x bits in the beginning (because they are out of bounds).
TEST_F(vec4stateTest, TestGetPartSelectTooLowStartAndTooHighEndIntVector) {
    vec4state sliceVector = intVector.getPartSelect(32, -5);
    EXPECT_FALSE(intVector.isUnknown());
    EXPECT_TRUE(compareVectorToString(sliceVector, string("x00010010001101000101011001111000xxxxx")));
    EXPECT_TRUE(checkVectorSize(sliceVector, 38));
    EXPECT_TRUE(sliceVector.isUnknown());
}

// Checks that getting the part select of an unknown vector that holds 108 bits from index 68 to 62 returns a vector that holds the 7 bits in these indexes of the original vector.
TEST_F(vec4stateTest, TestGetPartSelectBetweenVPIsBigVector) {
    vec4state sliceVector = bigVector.getPartSelect(68, 62);
    EXPECT_TRUE(compareVectorToString(sliceVector, string("0011xzx")));
    EXPECT_TRUE(checkVectorSize(sliceVector, 7));
}

// Checks that getting the part select of an unknown vector so the bits in this part are known returns a known vector.
TEST_F(vec4stateTest, TestGetPartSelectFromStringVectorToKnownVector) {
    vec4state sliceVector = stringVector.getPartSelect(1, 0);
    EXPECT_TRUE(stringVector.isUnknown());
    EXPECT_TRUE(compareVectorToString(sliceVector, string("11")));
    EXPECT_TRUE(checkVectorSize(sliceVector, 2));
    EXPECT_FALSE(sliceVector.isUnknown());
}

// Checks that the getPartSelect method throws an exception when the end index is less than the start index.
TEST_F(vec4stateTest, TestGetPartSelectEndLessThanStart) {
    EXPECT_ANY_THROW(bigVector.getPartSelect(62, 68));
}

// Checks that setting the part select of a vector that holds an integer (32 bits) from index 2 to 0 to 111 changes the first three bits of the vector to 111.
TEST_F(vec4stateTest, TestSetPartSelectIntVectorToString) {
    vec4state beforeSet = intVector;
    intVector.setPartSelect(2, 0, string("111"));
    EXPECT_TRUE(compareVectorToString(intVector, string("00010010001101000101011001111111")));
    EXPECT_TRUE(checkVectorSize(intVector, 32));
    EXPECT_TRUE(intVector != beforeSet);
}

// Checks that setting the part select of a vector that holds an integer (32 bits) from index 10 to 4 to an unknown vector that holds 6 bits changes the original vector to the same vector with the fourth to nineth bits set to the unknown vector and the tenth bit set to 0.
TEST_F(vec4stateTest, TestSetPartSelectIntVectorToStringVector) {
    vec4state beforeSet = intVector;
    EXPEVT_FALSE(intVector.isUnknown());
    intVector.setPartSelect(10, 4, stringVector);
    EXPECT_TRUE(compareVectorToString(intVector, string("000100100011010001010001xz111000")));
    EXPECT_TRUE(checkVectorSize(intVector, 32));
    EXPECT_TRUE(intVector.caseInequality(beforeSet));
    EXPECT_ANY_THROW(intVector != beforeSet);
    EXPECT_TRUE(intVector.isUnknown());
}

// Checks that setting the part select of a vector that holds an integer (32 bits) from index ten to four to 15 (1111) changes the original vector to the same vector with the fourth to eighth bits set to 15 and the nineth to tenth bits set to 0.
TEST_F(vec4stateTest, TestSetPartSelectIntVectorToInteger) {
    vec4state beforeSet = intVector;
    intVector.setPartSelect(10, 4, 15);
    EXPECT_TRUE(compareVectorToString(intVector, string("00010010001101000101000011111000")));
    EXPECT_TRUE(checkVectorSize(intVector, 32));
    EXPECT_TRUE(intVector != beforeSet);
}

// Checks that setting the part select of a vector that holds an integer (32 bits) from index 1 to 0 to 111 changes the original vector to the same vector with only the first 2 bits set to 1.
TEST_F(vec4stateTest, TestSetPartSelectTooBigValueIntVector) {
    vec4state beforeSet = intVector;
    intVector.setPartSelect(1, 0, string("111"));
    EXPECT_TRUE(compareVectorToString(intVector, string("00010010001101000101011001111011")));
    EXPECT_TRUE(checkVectorSize(intVector, 32));
    EXPECT_TRUE(intVector != beforeSet);
}

// Checks that setting the part select of a vector that holds an integer (32 bits) from index 4 to 0 to x changes the original vector to the same vector with the first bit set to x and the other 4 bits set to 0.
TEST_F(vec4stateTest, TestSetPartSelectTooSmallValueIntVector) {
    vec4state beforeSet = intVector;
    EXPECT_FALSE(intVector.isUnknown());
    intVector.setPartSelect(4, 0, string("x"));
    EXPECT_TRUE(compareVectorToString(intVector, string("0001001000110100010101100110000x")));
    EXPECT_TRUE(checkVectorSize(intVector, 32));
    EXPECT_TRUE(intVector != beforeSet);
    EXPECT_TRUE(intVector.isUnknown());
}

TEST_F(vec4stateTest, TestSetPartSelectTooHighEndIntVector) {
    intVector.setPartSelect(35, 30, stringVector);
    EXPECT_TRUE(compareVectorToString(intVector, string("11010010001101000101011001111000")));
    EXPECT_TRUE(checkVectorSize(intVector, 32));
}

TEST_F(vec4stateTest, TestSetPartSelectTooLowStartIntVector) {
    intVector.setPartSelect(5, -5, stringVector);
    EXPECT_TRUE(compareVectorToString(intVector, string("00010010001101000101011001000000")));
    EXPECT_TRUE(checkVectorSize(intVector, 32));
}

TEST_F(vec4stateTest, TestSetPartSelectTooLowEndIntVector) {
    intVector.setPartSelect(-4, -5, xVector);
    EXPECT_TRUE(compareVectorToString(intVector, string("00010010001101000101011001111000")));
    EXPECT_TRUE(checkVectorSize(intVector, 32));
}

TEST_F(vec4stateTest, TestSetPartSelectTooHighStartIntVector) {
    intVector.setPartSelect(34, 33, xVector);
    EXPECT_TRUE(compareVectorToString(intVector, string("00010010001101000101011001111000")));
    EXPECT_TRUE(checkVectorSize(intVector, 32));
}

TEST_F(vec4stateTest, TestSetPartSelectBetweenVPIsBigVector) {
    bigVector.setPartSelect(68, 62, string("111"));
    EXPECT_TRUE(compareVectorToString(bigVector, string("0110011xzx0111zzzx0110011xzx0111zzzx01100001110111zzzx0110011xzx0111zzzx0110011xzx0111zzzx0110011xzx0111zzzx")));
    EXPECT_TRUE(checkVectorSize(bigVector, 108));
}

// Checks that the setPartSelect method throws an exception when the end index is less than the start index.
TEST_F(vec4stateTest, TestSetPartSelectEndLessThanStart) {
    EXPECT_ANY_THROW(bigVector.setPartSelect(62, 68, string("111")));
}

// add tests that check the transition from unknown to known and vice versa

TEST_F(vec4stateTest, TestLogicalAndIntVectorWithItself) {
    EXPECT_TRUE(intVector && intVector);
}

TEST_F(vec4stateTest, TestLogicalAndIntVectorWithInteger) {
    EXPECT_TRUE(intVector && 0x12345678);
}

TEST_F(vec4stateTest, TestLogicalAndIntVectorWithLongLongVector) {
    EXPECT_TRUE(intVector && longLongVector);
}

TEST_F(vec4stateTest, TestLogicalAndIntVectorWithStringVector) {
    EXPECT_TRUE(intVector && stringVector);
}

TEST_F(vec4stateTest, TestLogicalAndIntVectorWithXVector) {
    vec4state andVector = intVector && xVector;
    EXPECT_TRUE(compareVectorToString(andVector, string("x")));
    EXPECT_TRUE(checkVectorSize(andVector, 1));
    EXPECT_ANY_THROW(bool result = intVector && xVector);
}

TEST_F(vec4stateTest, TestLogicalAndStringVectorWithZVector) {
    vec4state andVector = stringVector && zVector;
    EXPECT_TRUE(compareVectorToString(andVector, string("x")));
    EXPECT_TRUE(checkVectorSize(andVector, 1));
    EXPECT_ANY_THROW(bool result = stringVector && zVector);
}

TEST_F(vec4stateTest, TestLogicalAndZVectorWithZeroesVector) {
    EXPECT_FALSE(zVector && zeroesVector);
}

TEST_F(vec4stateTest, TestLogicalAndXVectorWithZVector) {
    vec4state andVector = xVector && zVector;
    EXPECT_TRUE(compareVectorToString(andVector, string("x")));
    EXPECT_TRUE(checkVectorSize(andVector, 1));
    EXPECT_ANY_THROW(bool result = xVector && zVector);
}

TEST_F(vec4stateTest, TestLogicalAndZeroesVectorWithOnesVector) {
    EXPECT_FALSE(zeroesVector && onesVector);
}

TEST_F(vec4stateTest, TestLogicalOrIntVectorWithItself) {
    EXPECT_TRUE(intVector || intVector);
}

TEST_F(vec4stateTest, TestLogicalOrIntVectorWithInteger) {
    EXPECT_TRUE(intVector || 0x12345678);
}

TEST_F(vec4stateTest, TestLogicalOrIntVectorWithLongLongVector) {
    EXPECT_TRUE(intVector || longLongVector);
}

TEST_F(vec4stateTest, TestLogicalOrIntVectorWithStringVector) {
    EXPECT_TRUE(intVector || stringVector);
}

TEST_F(vec4stateTest, TestLogicalOrIntVectorWithXVector) {
    EXPECT_TRUE(intVector || xVector);
}

TEST_F(vec4stateTest, TestLogicalOrStringVectorWithZVector) {
    EXPECT_TRUE(stringVector || zVector);
}

TEST_F(vec4stateTest, TestLogicalOrZVectorWithZero) {
    vec4state orVector = zVector || 0;
    EXPECT_TRUE(compareVectorToString(orVector, string("x")));
    EXPECT_TRUE(checkVectorSize(orVector, 1));
    EXPECT_ANY_THROW(bool result = zVector || 0);
}

TEST_F(vec4stateTest, TestLogicalOrXVectorWithZVector) {
    vec4state orVector = xVector || zVector;
    EXPECT_TRUE(compareVectorToString(orVector, string("x")));
    EXPECT_TRUE(checkVectorSize(orVector, 1));
    EXPECT_ANY_THROW(bool result = xVector || zVector);
}

TEST_F(vec4stateTest, TestLogicalOrZeroesVectorWithZero) {
    EXPECT_FALSE(zeroesVector || 0);
}

TEST_F(vec4stateTest, TestLogicalNotIntVector) {
    EXPECT_FALSE(!intVector);
}

TEST_F(vec4stateTest, TestLogicalNotStringVector) {
    EXPECT_FALSE(!stringVector);
}

TEST_F(vec4stateTest, TestLogicalNotXVector) {
    vec4state notVector = !xVector;
    EXPECT_TRUE(compareVectorToString(notVector, string("x")));
    EXPECT_TRUE(checkVectorSize(notVector, 1));
    EXPECT_ANY_THROW(bool result = !xVector);
}

TEST_F(vec4stateTest, TestLogicalNotZVector) {
    vec4state notVector = !zVector;
    EXPECT_TRUE(compareVectorToString(notVector, string("x")));
    EXPECT_TRUE(checkVectorSize(notVector, 1));
    EXPECT_ANY_THROW(bool result = !zVector);
}

TEST_F(vec4stateTest, TestLogicalNotZeroesVector) {
    EXPECT_TRUE(!zeroesVector);
}

TEST_F(vec4stateTest, TestRelationalIntVectorLessThanItself) {
    EXPECT_FALSE(intVector < intVector);
}

TEST_F(vec4stateTest, TestRelationalIntVectorLessThanInteger) {
    EXPECT_FALSE(intVector < 0x12345678);
}

TEST_F(vec4stateTest, TestRelationalLongLongVectorLessThanIntVector) {
    EXPECT_FALSE(longLongVector < intVector);
}

TEST_F(vec4stateTest, TestRelationalIntVectorLessThanLongLongVector) {
    EXPECT_TRUE(intVector < longLongVector);
}

TEST_F(vec4stateTest, TestRelationalIntVectorLessThanStringVector) {
    vec4state ltVector = intVector < stringVector;
    EXPECT_TRUE(compareVectorToString(ltVector, string("x")));
    EXPECT_TRUE(checkVectorSize(ltVector, 1));
    EXPECT_ANY_THROW(bool result = intVector < stringVector);
}

TEST_F(vec4stateTest, TestRelationalZeroesVectorLessThanOnesVector) {
    EXPECT_TRUE(zeroesVector < onesVector);
}

TEST_F(vec4stateTest, TestRelationalIntVectorGreaterThanItself) {
    EXPECT_FALSE(intVector > intVector);
}

TEST_F(vec4stateTest, TestRelationalIntVectorGreaterThanInteger) {
    EXPECT_FALSE(intVector > 0x12345678);
}

TEST_F(vec4stateTest, TestRelationalIntVectorGreaterThanLongLongVector) {
    EXPECT_FALSE(intVector > longLongVector);
}

TEST_F(vec4stateTest, TestRelationalLongLongVectorGreaterThanIntVector) {
    EXPECT_TRUE(longLongVector > intVector);
}

TEST_F(vec4stateTest, TestRelationalIntVectorGreaterThanStringVector) {
    vec4state gtVector = intVector > stringVector;
    EXPECT_TRUE(compareVectorToString(gtVector, string("x")));
    EXPECT_TRUE(checkVectorSize(gtVector, 1));
    EXPECT_ANY_THROW(bool result = intVector > stringVector);
}

TEST_F(vec4stateTest, TestRelationalZeroesVectorGreaterThanOnesVector) {
    EXPECT_FALSE(zeroesVector > onesVector);
}

TEST_F(vec4stateTest, TestRelationalIntVectorLessThanEqualToItself) {
    EXPECT_TRUE(intVector <= intVector);
}

TEST_F(vec4stateTest, TestRelationalIntVectorLessThanEqualToInteger) {
    EXPECT_TRUE(intVector <= 0x12345678);
}

TEST_F(vec4stateTest, TestRelationalLongLongVectorLessThanEqualToIntVector) {
    EXPECT_FALSE(longLongVector <= intVector);
}

TEST_F(vec4stateTest, TestRelationalIntVectorLessThanEqualToLongLongVector) {
    EXPECT_TRUE(intVector <= longLongVector);
}

TEST_F(vec4stateTest, TestRelationalIntVectorLessThanEqualToStringVector) {
    vec4state lteVector = intVector <= stringVector;
    EXPECT_TRUE(compareVectorToString(lteVector, string("x")));
    EXPECT_TRUE(checkVectorSize(lteVector, 1));
    EXPECT_ANY_THROW(bool result = intVector <= stringVector);
}

TEST_F(vec4stateTest, TestRelationalZeroesVectorLessThanEqualToOnesVector) {
    EXPECT_TRUE(zeroesVector <= onesVector);
}

TEST_F(vec4stateTest, TestRelationalIntVectorGreaterThanEqualToItself) {
    EXPECT_TRUE(intVector >= intVector);
}

TEST_F(vec4stateTest, TestRelationalIntVectorGreaterThanEqualToInteger) {
    EXPECT_TRUE(intVector >= 0x12345678);
}

TEST_F(vec4stateTest, TestRelationalLongLongVectorGreaterThanEqualToIntVector) {
    EXPECT_TRUE(longLongVector >= intVector);
}

TEST_F(vec4stateTest, TestRelationalIntVectorGreaterThanEqualToLongLongVector) {
    EXPECT_FALSE(intVector >= longLongVector);
}

TEST_F(vec4stateTest, TestRelationalIntVectorGreaterThanEqualToStringVector) {
    vec4state gteVector = intVector >= stringVector;
    EXPECT_TRUE(compareVectorToString(gteVector, string("x")));
    EXPECT_TRUE(checkVectorSize(gteVector, 1));
    EXPECT_ANY_THROW(bool result = intVector >= stringVector);
}

TEST_F(vec4stateTest, TestRelationalZeroesVectorGreaterThanEqualToOnesVector) {
    EXPECT_FALSE(zeroesVector >= onesVector);
}

// Tests the addition of known vector with itself.
TEST_F(vec4stateTest, TestArithmeticAddIntVectorWithItself) {
    vec4state addVector = intVector + intVector;
    EXPECT_TRUE(compareVectorToString(addVector, string("00100100011010001010110011110000")));
    EXPECT_TRUE(checkVectorSize(addVector, 32));
}

// Tests the addition of vector with unknown vector.
TEST_F(vec4stateTest, TestArithmeticAddIntVectorWithStringVector) {
    EXPECT_TRUE(compareVectorToString(intVector + stringVector, string("xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx")));
    EXPECT_TRUE(checkVectorSize(intVector + stringVector, 32));
}

// Tests the addition of vector with an int number.
TEST_F(vec4stateTest, TestArithmeticAddIntVectorWithNumber) {
    vec4state addVector = intVector + 0x12345678;
    EXPECT_TRUE(compareVectorToString(addVector, string("00100100011010001010110011110000")));
    EXPECT_TRUE(checkVectorSize(addVector, 32));
}

// Tests the addition of vector with an int number that causes carry.
TEST_F(vec4stateTest, TestArithmeticAddIntVectorWithNumberCarry) {
    vec4state addVector = intVector + 0xFF123456;
    EXPECT_TRUE(compareVectorToString(addVector, string("100010001010001101000101011001110")));
    EXPECT_TRUE(checkVectorSize(addVector, 33));
}
