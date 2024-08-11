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
    if (vector.getSize() == size) {
        return testing::AssertionSuccess();
    } else {
        return testing::AssertionFailure() << "Expected: " << size << "\nActual: " << vector.getSize();
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
    vec4state copyVector = vec4state(stringVector);
    EXPECT_TRUE(compareVectorToString(copyVector, string("01xz11")));
    EXPECT_TRUE(checkVectorSize(copyVector, 6));
}

// Checks that the assignment operator from another vector creates a vector that has the same value and number of bits as the vector passed to it.
TEST_F(vec4stateTest, TestCopyAssignment) {
    vec4state copy = defaultVector;
    EXPECT_TRUE(compareVectorToString(copy, string("x")));
    EXPECT_TRUE(checkVectorSize(copy, 1));
}

// Checks that the assignment operator from an integer creates a 32-bit vector that stores the value of the integer passed to it.
TEST_F(vec4stateTest, TestIntegerAssignment) {
    vec4state copy = 0x12345678;
    EXPECT_TRUE(compareVectorToString(copy, string("00010010001101000101011001111000")));
    EXPECT_TRUE(checkVectorSize(copy, 32));
}

// Checks that the assignment operator from a long long number creates a 64-bit vector that stores the value of the long long number passed to it.
TEST_F(vec4stateTest, TestLongLongAssignment) {
    vec4state copy = 0x1234567890ABCDEF;
    EXPECT_TRUE(compareVectorToString(copy, string("0001001000110100010101100111100010010000101010111100110111101111")));
    EXPECT_TRUE(checkVectorSize(copy, 64));
}

// Checks that the assignment operator from a string creates a vector that has the same value as the string passed to it and it's number of bits is equal to the length of the string.
TEST_F(vec4stateTest, TestStringAssignment) {
    vec4state copy = "01xz11";
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

// Checks that the bitwise AND operator between a vector that stores unknown bits with itself creates a vector that replaces every z value with x value.
TEST_F(vec4stateTest, TestBitwiseAndStringVectorWithItself) {
    vec4state andVector = stringVector & stringVector;
    EXPECT_TRUE(compareVectorToString(andVector, string("01xx11")));
    EXPECT_TRUE(checkVectorSize(andVector, 6));
}

// Checks that the bitwise AND operator between two unknown vectors with different amount of bits creates a vector that holds the result of the bitwise AND operation between the two vectors. The result should have the same number of bits as the longer vector.
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

// Checks that the bitwise OR operator between a vector that stores unknown bits with itself creates a vector that replaces every z value with x value.
TEST_F(vec4stateTest, TestBitwiseOrStringVectorWithItself) {
    vec4state orVector = stringVector | stringVector;
    EXPECT_TRUE(compareVectorToString(orVector, string("01xx11")));
    EXPECT_TRUE(checkVectorSize(orVector, 6));
}

// Checks that the bitwise OR operator between two unknown vectors with different amount of bits creates a vector that holds the result of the bitwise OR operation between the two vectors. The result should have the same number of bits as the longer vector.
TEST_F(vec4stateTest, TestBitwiseOrSmallVectorWithBigVector) {
    vec4state orVector = stringVector | bigVector;
    EXPECT_TRUE(compareVectorToString(orVector, string("0110011xxx0111xxxx0110011xxx0111xxxx0110011xxx0111xxxx0110011xxx0111xxxx0110011xxx0111xxxx0110011xxx0111xx11")));
    EXPECT_TRUE(checkVectorSize(orVector, 108));
}

TEST_F(vec4stateTest, TestBitwiseXorIntVectorWithItself) {
    vec4state xorVector = intVector ^ intVector;
    EXPECT_TRUE(compareVectorToString(xorVector, string("00000000000000000000000000000000")));
    EXPECT_TRUE(checkVectorSize(xorVector, 32));
}

TEST_F(vec4stateTest, TestBitwiseXorLongLongVectorWithItself) {
    vec4state xorVector = longLongVector ^ longLongVector;
    EXPECT_TRUE(compareVectorToString(xorVector, string("0000000000000000000000000000000000000000000000000000000000000000")));
    EXPECT_TRUE(checkVectorSize(xorVector, 64));
}

TEST_F(vec4stateTest, TestBitwiseXorIntVectorWithLongLongVector) {
    vec4state xorVector = intVector ^ longLongVector;
    EXPECT_TRUE(compareVectorToString(xorVector, string("0001001000110100010101100111100010000010100111111001101110010111")));
    EXPECT_TRUE(checkVectorSize(xorVector, 64));
}

TEST_F(vec4stateTest, TestBitwiseXorIntVectorWithLongLong) {
    vec4state xorVector = intVector ^ 0x1234567890ABCDEF;
    EXPECT_TRUE(compareVectorToString(xorVector, string("0001001000110100010101100111100010000010100111111001101110010111")));
    EXPECT_TRUE(checkVectorSize(xorVector, 64));
}

TEST_F(vec4stateTest, TestBitwiseXorIntVectorWithInteger) {
    vec4state xorVector = intVector ^ 0x12345678;
    EXPECT_TRUE(compareVectorToString(xorVector, string("00000000000000000000000000000000")));
    EXPECT_TRUE(checkVectorSize(xorVector, 32));
}

TEST_F(vec4stateTest, TestBitwiseXorStringVectorWithItself) {
    vec4state xorVector = stringVector ^ stringVector;
    EXPECT_TRUE(compareVectorToString(xorVector, string("00xx00")));
    EXPECT_TRUE(checkVectorSize(xorVector, 6));
}

TEST_F(vec4stateTest, TestBitwiseXorSmallVectorWithBigVector) {
    vec4state xorVector = stringVector ^ bigVector;
    EXPECT_TRUE(compareVectorToString(xorVector, string("0110011xxx0111xxxx0110011xxx0111xxxx0110011xxx0111xxxx0110011xxx0111xxxx0110011xxx0111xxxx0110011xxx0110xxxx")));
    EXPECT_TRUE(checkVectorSize(xorVector, 108));
}

TEST_F(vec4stateTest, TestBitwiseNotLongLongVector) {
    vec4state notVector = ~longLongVector;
    EXPECT_TRUE(compareVectorToString(notVector, string("1110110111001011101010011000011101101111010101000011001000010000")));
    EXPECT_TRUE(checkVectorSize(notVector, 64));
}

TEST_F(vec4stateTest, TestBitwiseNotStringVector) {
    vec4state notVector = ~stringVector;
    EXPECT_TRUE(compareVectorToString(notVector, string("10xx00")));
    EXPECT_TRUE(checkVectorSize(notVector, 6));
}

TEST_F(vec4stateTest, TestIntVectorEqualityWithItself) {
    vec4state eqVector = intVector == intVector;
    EXPECT_TRUE(compareVectorToString(eqVector, string("1")));
    EXPECT_TRUE(checkVectorSize(eqVector, 1));
}

TEST_F(vec4stateTest, TestIntVectorEqualityWithInteger) {
    vec4state eqVector = intVector == 0x12345678;
    EXPECT_TRUE(compareVectorToString(eqVector, string("1")));
    EXPECT_TRUE(checkVectorSize(eqVector, 1));
}

TEST_F(vec4stateTest, TestLongLongVectorEqualityWithItself) {
    vec4state eqVector = longLongVector == longLongVector;
    EXPECT_TRUE(compareVectorToString(eqVector, string("1")));
    EXPECT_TRUE(checkVectorSize(eqVector, 1));
}

TEST_F(vec4stateTest, TestLongLongVectorEqualityWithLongLong) {
    vec4state eqVector = longLongVector == 0x1234567890ABCDEF;
    EXPECT_TRUE(compareVectorToString(eqVector, string("1")));
    EXPECT_TRUE(checkVectorSize(eqVector, 1));
}

TEST_F(vec4stateTest, TestStringVectorEqualityWithItself) {
    vec4state eqVector = stringVector == stringVector;
    EXPECT_TRUE(compareVectorToString(eqVector, string("x")));
    EXPECT_TRUE(checkVectorSize(eqVector, 1));
}

TEST_F(vec4stateTest, TestBigVectorEqualityWithItself) {
    vec4state eqVector = bigVector == bigVector;
    EXPECT_TRUE(compareVectorToString(eqVector, string("x")));
    EXPECT_TRUE(checkVectorSize(eqVector, 1));
}

TEST_F(vec4stateTest, TestIntVectorEqualityWithLongLongVector) {
    vec4state eqVector = intVector == longLongVector;
    EXPECT_TRUE(compareVectorToString(eqVector, string("0")));
    EXPECT_TRUE(checkVectorSize(eqVector, 1));
}

TEST_F(vec4stateTest, TestStringVectorEqualityWithIntVector) {
    vec4state eqVector = stringVector == intVector;
    EXPECT_TRUE(compareVectorToString(eqVector, string("0")));
    EXPECT_TRUE(checkVectorSize(eqVector, 1));
}

TEST_F(vec4stateTest, TestStringVectorEqualityWithLongLongVector) {
    vec4state eqVector = stringVector == longLongVector;
    EXPECT_TRUE(compareVectorToString(eqVector, string("0")));
    EXPECT_TRUE(checkVectorSize(eqVector, 1));
}

TEST_F(vec4stateTest, TestSmallVectorEqualityWithBigVector) {
    vec4state eqVector = stringVector == bigVector;
    EXPECT_TRUE(compareVectorToString(eqVector, string("0")));
    EXPECT_TRUE(checkVectorSize(eqVector, 1));

    eqVector = bigVector == stringVector;
    EXPECT_TRUE(compareVectorToString(eqVector, string("0")));
    EXPECT_TRUE(checkVectorSize(eqVector, 1));
}

TEST_F(vec4stateTest, TestBigVectorEqualityWithIntVector) {
    vec4state eqVector = bigVector == intVector;
    EXPECT_TRUE(compareVectorToString(eqVector, string("0")));
    EXPECT_TRUE(checkVectorSize(eqVector, 1));
}

TEST_F(vec4stateTest, TestBigVectorEqualityWithLongLongVector) {
    vec4state eqVector = bigVector == longLongVector;
    EXPECT_TRUE(compareVectorToString(eqVector, string("0")));
    EXPECT_TRUE(checkVectorSize(eqVector, 1));
}

TEST_F(vec4stateTest, TestOnesVectorEqualityWithXVector) {
    vec4state eqVector = onesVector == xVector;
    EXPECT_TRUE(compareVectorToString(eqVector, string("x")));
    EXPECT_TRUE(checkVectorSize(eqVector, 1));
}

TEST_F(vec4stateTest, TestOnesVectorEqualityWithZVector) {
    vec4state eqVector = onesVector == zVector;
    EXPECT_TRUE(compareVectorToString(eqVector, string("x")));
    EXPECT_TRUE(checkVectorSize(eqVector, 1));
}

TEST_F(vec4stateTest, TestOnesVectorEqualityWithOneAndXVector) {
    vec4state eqVector = onesVector == oneAndXVector;
    EXPECT_TRUE(compareVectorToString(eqVector, string("x")));
    EXPECT_TRUE(checkVectorSize(eqVector, 1));
}

TEST_F(vec4stateTest, TestOnesVectorEqualityWithZeroAndZVector) {
    vec4state eqVector = onesVector == zeroAndZVector;
    EXPECT_TRUE(compareVectorToString(eqVector, string("0")));
    EXPECT_TRUE(checkVectorSize(eqVector, 1));
}

TEST_F(vec4stateTest, TestXVectorEqualityWithZVector) {
    vec4state eqVector = xVector == zVector;
    EXPECT_TRUE(compareVectorToString(eqVector, string("x")));
    EXPECT_TRUE(checkVectorSize(eqVector, 1));
}

TEST_F(vec4stateTest, TestOnesVectorEqualityWithXThenOnesVector) {
    vec4state eqVector = onesVector == xThenOnesVector;
    EXPECT_TRUE(compareVectorToString(eqVector, string("x")));
    EXPECT_TRUE(checkVectorSize(eqVector, 1));
}

TEST_F(vec4stateTest, TestZeroesVectorEqualityWithZThenZeroesVector) {
    vec4state eqVector = zeroesVector == zThenZeroesVector;
    EXPECT_TRUE(compareVectorToString(eqVector, string("x")));
    EXPECT_TRUE(checkVectorSize(eqVector, 1));
}

TEST_F(vec4stateTest, TestIntVectorInequalityWithItself) {
    vec4state neqVector = intVector != intVector;
    EXPECT_TRUE(compareVectorToString(neqVector, string("0")));
    EXPECT_TRUE(checkVectorSize(neqVector, 1));
}

TEST_F(vec4stateTest, TestIntVectorInequalityWithInteger) {
    vec4state neqVector = intVector != 0x12345678;
    EXPECT_TRUE(compareVectorToString(neqVector, string("0")));
    EXPECT_TRUE(checkVectorSize(neqVector, 1));
}

TEST_F(vec4stateTest, TestIntVectorInequalityWithLongLong) {
    vec4state neqVector = intVector != 0x1234567890ABCDEF;
    EXPECT_TRUE(compareVectorToString(neqVector, string("1")));
    EXPECT_TRUE(checkVectorSize(neqVector, 1));
}

TEST_F(vec4stateTest, TestLongLongVectorInequalityWithBigVector) {
    vec4state neqVector = longLongVector != bigVector;
    EXPECT_TRUE(compareVectorToString(neqVector, string("1")));
    EXPECT_TRUE(checkVectorSize(neqVector, 1));
}

TEST_F(vec4stateTest, TestSmallVectorInequalityWithBigVector) {
    vec4state neqVector = stringVector != bigVector;
    EXPECT_TRUE(compareVectorToString(neqVector, string("1")));
    EXPECT_TRUE(checkVectorSize(neqVector, 1));
}

TEST_F(vec4stateTest, TestStringVectorInequalityWithItself) {
    vec4state neqVector = stringVector != stringVector;
    EXPECT_TRUE(compareVectorToString(neqVector, string("x")));
    EXPECT_TRUE(checkVectorSize(neqVector, 1));
}

TEST_F(vec4stateTest, TestOnesVectorInequalityWithXVector) {
    vec4state neqVector = onesVector != xVector;
    EXPECT_TRUE(compareVectorToString(neqVector, string("x")));
    EXPECT_TRUE(checkVectorSize(neqVector, 1));
}

TEST_F(vec4stateTest, TestOnesVectorInequalityWithZVector) {
    vec4state neqVector = onesVector != zVector;
    EXPECT_TRUE(compareVectorToString(neqVector, string("x")));
    EXPECT_TRUE(checkVectorSize(neqVector, 1));
}

TEST_F(vec4stateTest, TestOnesVectorInequalityWithOneAndXVector) {
    vec4state neqVector = onesVector != oneAndXVector;
    EXPECT_TRUE(compareVectorToString(neqVector, string("x")));
    EXPECT_TRUE(checkVectorSize(neqVector, 1));
}

TEST_F(vec4stateTest, TestOnesVectorInequalityWithZeroesAndXVector) {
    vec4state neqVector = onesVector != zeroAndZVector;
    EXPECT_TRUE(compareVectorToString(neqVector, string("1")));
    EXPECT_TRUE(checkVectorSize(neqVector, 1));
}

TEST_F(vec4stateTest, TestXVectorInequalityWithZVector) {
    vec4state neqVector = xVector != zVector;
    EXPECT_TRUE(compareVectorToString(neqVector, string("x")));
    EXPECT_TRUE(checkVectorSize(neqVector, 1));
}

TEST_F(vec4stateTest, TestIntVectorCaseEqualityWithItself) {
    vec4state caseEqVector = intVector.caseEquality(intVector);
    EXPECT_TRUE(compareVectorToString(caseEqVector, string("1")));
    EXPECT_TRUE(checkVectorSize(caseEqVector, 1));
}

TEST_F(vec4stateTest, TestIntVectorCaseEqualityWithInteger) {
    vec4state caseEqVector = intVector.caseEquality(0x12345678);
    EXPECT_TRUE(compareVectorToString(caseEqVector, string("1")));
    EXPECT_TRUE(checkVectorSize(caseEqVector, 1));
}

TEST_F(vec4stateTest, TestStringVectorCaseEqualityWithItself) {
    vec4state caseEqVector = stringVector.caseEquality(stringVector);
    EXPECT_TRUE(compareVectorToString(caseEqVector, string("1")));
    EXPECT_TRUE(checkVectorSize(caseEqVector, 1));
}

TEST_F(vec4stateTest, TestSmallVectorCaseEqualityWithBigVector) {
    vec4state caseEqVector = stringVector.caseEquality(bigVector);
    EXPECT_TRUE(compareVectorToString(caseEqVector, string("0")));
    EXPECT_TRUE(checkVectorSize(caseEqVector, 1));
}

TEST_F(vec4stateTest, TestOnesVectorCaseEqualityWithXVector) {
    vec4state caseEqVector = onesVector.caseEquality(xVector);
    EXPECT_TRUE(compareVectorToString(caseEqVector, string("0")));
    EXPECT_TRUE(checkVectorSize(caseEqVector, 1));
}

TEST_F(vec4stateTest, TestXVectorCaseEqualityWithZVector) {
    vec4state caseEqVector = xVector.caseEquality(zVector);
    EXPECT_TRUE(compareVectorToString(caseEqVector, string("0")));
    EXPECT_TRUE(checkVectorSize(caseEqVector, 1));
}

TEST_F(vec4stateTest, TestIntVectorCaseInequalityWithItself) {
    vec4state caseNeqVector = intVector.caseInequality(intVector);
    EXPECT_TRUE(compareVectorToString(caseNeqVector, string("0")));
    EXPECT_TRUE(checkVectorSize(caseNeqVector, 1));
}

TEST_F(vec4stateTest, TestIntVectorCaseInequalityWithInteger) {
    vec4state caseNeqVector = intVector.caseInequality(0x12345678);
    EXPECT_TRUE(compareVectorToString(caseNeqVector, string("0")));
    EXPECT_TRUE(checkVectorSize(caseNeqVector, 1));
}

TEST_F(vec4stateTest, TestStringVectorCaseInequalityWithItself) {
    vec4state caseNeqVector = stringVector.caseInequality(stringVector);
    EXPECT_TRUE(compareVectorToString(caseNeqVector, string("0")));
    EXPECT_TRUE(checkVectorSize(caseNeqVector, 1));
}

TEST_F(vec4stateTest, TestSmallVectorCaseInequalityWithBigVector) {
    vec4state caseNeqVector = stringVector.caseInequality(bigVector);
    EXPECT_TRUE(compareVectorToString(caseNeqVector, string("1")));
    EXPECT_TRUE(checkVectorSize(caseNeqVector, 1));
}

TEST_F(vec4stateTest, TestOnesVectorCaseInequalityWithXVector) {
    vec4state caseNeqVector = onesVector.caseInequality(xVector);
    EXPECT_TRUE(compareVectorToString(caseNeqVector, string("1")));
    EXPECT_TRUE(checkVectorSize(caseNeqVector, 1));
}

TEST_F(vec4stateTest, TestXVectorCaseInequalityWithZVector) {
    vec4state caseNeqVector = xVector.caseInequality(zVector);
    EXPECT_TRUE(compareVectorToString(caseNeqVector, string("1")));
    EXPECT_TRUE(checkVectorSize(caseNeqVector, 1));
}

TEST_F(vec4stateTest, TestShiftLeftIntVectorByTwo) {
    vec4state shiftLeftVector = intVector << 2;
    EXPECT_TRUE(compareVectorToString(shiftLeftVector, string("01001000110100010101100111100000")));
    EXPECT_TRUE(checkVectorSize(shiftLeftVector, 32));
}

TEST_F(vec4stateTest, TestShiftLeftIntVectorByThirtyTwo) {
    vec4state shiftLeftVector = intVector << 32;
    EXPECT_TRUE(compareVectorToString(shiftLeftVector, string("00000000000000000000000000000000")));
    EXPECT_TRUE(checkVectorSize(shiftLeftVector, 32));
}

TEST_F(vec4stateTest, TestShiftLeftLongLongVectorByTwo) {
    vec4state shiftLeftVector = longLongVector << 2;
    EXPECT_TRUE(compareVectorToString(shiftLeftVector, string("0100100011010001010110011110001001000010101011110011011110111100")));
    EXPECT_TRUE(checkVectorSize(shiftLeftVector, 64));
}

TEST_F(vec4stateTest, TestShiftLeftLongLongVectorBySixtyFour) {
    vec4state shiftLeftVector = longLongVector << 64;
    EXPECT_TRUE(compareVectorToString(shiftLeftVector, string("0000000000000000000000000000000000000000000000000000000000000000")));
    EXPECT_TRUE(checkVectorSize(shiftLeftVector, 64));
}

TEST_F(vec4stateTest, TestShiftLeftLongLongVectorByThirtyFive) {
    vec4state shiftLeftVector = longLongVector << 35;
    EXPECT_TRUE(compareVectorToString(shiftLeftVector, string("1000010101011110011011110111100000000000000000000000000000000000")));
    EXPECT_TRUE(checkVectorSize(shiftLeftVector, 64));
}

TEST_F(vec4stateTest, TestShiftLeftStringVectorByTwo) {
    vec4state shiftLeftVector = stringVector << 2;
    EXPECT_TRUE(compareVectorToString(shiftLeftVector, string("xz1100")));
    EXPECT_TRUE(checkVectorSize(shiftLeftVector, 6));
}

TEST_F(vec4stateTest, TestShiftLeftBigVectorBySeventy) {
    vec4state shiftLeftVector = bigVector << 70;
    EXPECT_TRUE(compareVectorToString(shiftLeftVector, string("zx0110011xzx0111zzzx0110011xzx0111zzzx0000000000000000000000000000000000000000000000000000000000000000000000")));
    EXPECT_TRUE(checkVectorSize(shiftLeftVector, 108));
}

TEST_F(vec4stateTest, TestShiftLeftBigVectorBySmallVector) {
    vec4state shiftLeftVector = bigVector << stringVector;
    EXPECT_TRUE(compareVectorToString(shiftLeftVector, string("xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx")));
    EXPECT_TRUE(checkVectorSize(shiftLeftVector, 108));
}

TEST_F(vec4stateTest, TestShiftLeftBigVectorByOnesVector) {
    vec4state shiftLeftVector = bigVector << onesVector;
    EXPECT_TRUE(compareVectorToString(shiftLeftVector, string("0011xzx0111zzzx0110011xzx0111zzzx0110011xzx0111zzzx0110011xzx0111zzzx0110011xzx0111zzzx0110011xzx0111zzzx000")));
    EXPECT_TRUE(checkVectorSize(shiftLeftVector, 108));
}

TEST_F(vec4stateTest, TestShiftLeftBigVectorByIntVector) {
    vec4state shiftLeftVector = bigVector << intVector;
    EXPECT_TRUE(compareVectorToString(shiftLeftVector, string("000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000")));
    EXPECT_TRUE(checkVectorSize(shiftLeftVector, 108));
}

TEST_F(vec4stateTest, TestShiftRightIntVectorByTwo) {
    vec4state shiftRightVector = intVector >> 2;
    EXPECT_TRUE(compareVectorToString(shiftRightVector, string("00000100100011010001010110011110")));
    EXPECT_TRUE(checkVectorSize(shiftRightVector, 32));
}

TEST_F(vec4stateTest, TestShiftRightIntVectorByThirtyTwo) {
    vec4state shiftRightVector = intVector >> 32;
    EXPECT_TRUE(compareVectorToString(shiftRightVector, string("00000000000000000000000000000000")));
    EXPECT_TRUE(checkVectorSize(shiftRightVector, 32));
}

TEST_F(vec4stateTest, TestShiftRightLongLongVectorByTwo) {
    vec4state shiftRightVector = longLongVector >> 2;
    EXPECT_TRUE(compareVectorToString(shiftRightVector, string("0000010010001101000101011001111000100100001010101111001101111011")));
    EXPECT_TRUE(checkVectorSize(shiftRightVector, 64));
}

TEST_F(vec4stateTest, TestShiftRightLongLongVectorBySixtyFour) {
    vec4state shiftRightVector = longLongVector >> 64;
    EXPECT_TRUE(compareVectorToString(shiftRightVector, string("0000000000000000000000000000000000000000000000000000000000000000")));
    EXPECT_TRUE(checkVectorSize(shiftRightVector, 64));
}

TEST_F(vec4stateTest, TestShiftRightLongLongVectorByThirtyFive) {
    vec4state shiftRightVector = longLongVector >> 35;
    EXPECT_TRUE(compareVectorToString(shiftRightVector, string("0000000000000000000000000000000000000010010001101000101011001111")));
    EXPECT_TRUE(checkVectorSize(shiftRightVector, 64));
}

TEST_F(vec4stateTest, TestShiftRightStringVectorByTwo) {
    vec4state shiftRightVector = stringVector >> 2;
    EXPECT_TRUE(compareVectorToString(shiftRightVector, string("0001xz")));
    EXPECT_TRUE(checkVectorSize(shiftRightVector, 6));
}

TEST_F(vec4stateTest, TestShiftRightBigVectorBySeventy) {
    vec4state shiftRightVector = bigVector >> 70;
    EXPECT_TRUE(compareVectorToString(shiftRightVector, string("00000000000000000000000000000000000000000000000000000000000000000000000110011xzx0111zzzx0110011xzx0111zzzx01")));
    EXPECT_TRUE(checkVectorSize(shiftRightVector, 108));
}

TEST_F(vec4stateTest, TestShiftRightBigVectorBySmallVector) {
    vec4state shiftRightVector = bigVector >> stringVector;
    EXPECT_TRUE(compareVectorToString(shiftRightVector, string("xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx")));
    EXPECT_TRUE(checkVectorSize(shiftRightVector, 108));
}

TEST_F(vec4stateTest, TestShiftRightBigVectorByOnesVector) {
    vec4state shiftRightVector = bigVector >> onesVector;
    EXPECT_TRUE(compareVectorToString(shiftRightVector, string("0000110011xzx0111zzzx0110011xzx0111zzzx0110011xzx0111zzzx0110011xzx0111zzzx0110011xzx0111zzzx0110011xzx0111z")));
    EXPECT_TRUE(checkVectorSize(shiftRightVector, 108));
}

TEST_F(vec4stateTest, TestShiftRightBigVectorByIntVector) {
    vec4state shiftRightVector = bigVector >> intVector;
    EXPECT_TRUE(compareVectorToString(shiftRightVector, string("000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000")));
    EXPECT_TRUE(checkVectorSize(shiftRightVector, 108));
}


TEST_F(vec4stateTest, TestGetBitSelectIntVector) {
    vec4state indexVector = intVector.getBitSelect(2);
    EXPECT_TRUE(compareVectorToString(indexVector, string("0")));
    EXPECT_TRUE(checkVectorSize(indexVector, 1));
}

TEST_F(vec4stateTest, TestGetTooHighBitSelectIntVector) {
    vec4state indexVector = intVector.getBitSelect(32);
    EXPECT_TRUE(compareVectorToString(indexVector, string("x")));
    EXPECT_TRUE(checkVectorSize(indexVector, 1));
}

TEST_F(vec4stateTest, TestGetTooLowBitSelectIntVector) {
    vec4state indexVector = intVector.getBitSelect(-1);
    EXPECT_TRUE(compareVectorToString(indexVector, string("x")));
    EXPECT_TRUE(checkVectorSize(indexVector, 1));
}

TEST_F(vec4stateTest, TestGetBitSelectStringVector) {
    vec4state indexVector = stringVector.getBitSelect(2);
    EXPECT_TRUE(compareVectorToString(indexVector, string("z")));
    EXPECT_TRUE(checkVectorSize(indexVector, 1));
}

TEST_F(vec4stateTest, TestGetBitSelectBigVector) {
    vec4state indexVector = bigVector.getBitSelect(70);
    EXPECT_TRUE(compareVectorToString(indexVector, string("1")));
    EXPECT_TRUE(checkVectorSize(indexVector, 1));
}

TEST_F(vec4stateTest, TestGetUnknownBitSelectIntVector) {
    vec4state indexVector = intVector.getBitSelect(stringVector);
    EXPECT_TRUE(compareVectorToString(indexVector, string("x")));
    EXPECT_TRUE(checkVectorSize(indexVector, 1));
}

TEST_F(vec4stateTest, TestSetBitSelectZeroesVector) {
    zeroesVector.setBitSelect(0, 1);
    EXPECT_TRUE(compareVectorToString(zeroesVector, string("01")));
    EXPECT_TRUE(checkVectorSize(zeroesVector, 2));
}

TEST_F(vec4stateTest, TestSetTooHighBitSelectIntVector) {
    intVector.setBitSelect(32, 1);
    EXPECT_TRUE(compareVectorToString(intVector, string("00010010001101000101011001111000")));
    EXPECT_TRUE(checkVectorSize(intVector, 32));
}

TEST_F(vec4stateTest, TestSetTooLowBitSelectIntVector) {
    intVector.setBitSelect(-1, 1);
    EXPECT_TRUE(compareVectorToString(intVector, string("00010010001101000101011001111000")));
    EXPECT_TRUE(checkVectorSize(intVector, 32));
}

TEST_F(vec4stateTest, TestSetBitSelectStringVectorToDefaultVector) {
    stringVector.setBitSelect(1, defaultVector);
    EXPECT_TRUE(compareVectorToString(stringVector, string("01xzx1")));
    EXPECT_TRUE(checkVectorSize(stringVector, 6));
}

TEST_F(vec4stateTest, TestSetUnknownBitSelectIntVectorToInteger) {
    intVector.setBitSelect(stringVector, 1);
    EXPECT_TRUE(compareVectorToString(intVector, string("00010010001101000101011001111000")));
    EXPECT_TRUE(checkVectorSize(intVector, 32));
}

TEST_F(vec4stateTest, TestSetBitSelectStringVectorToZeroAndXVector) {
    stringVector.setBitSelect(1, zeroAndXVector);
    EXPECT_TRUE(compareVectorToString(stringVector, string("01xzx1")));
    EXPECT_TRUE(checkVectorSize(stringVector, 6));
}



TEST_F(vec4stateTest, TestGetPartSelectIntVector) {
    vec4state sliceVector = intVector.getPartSelect(2, 0);
    EXPECT_TRUE(compareVectorToString(sliceVector, string("000")));
    EXPECT_TRUE(checkVectorSize(sliceVector, 3));
}

TEST_F(vec4stateTest, TestGetPartSelectWholeIntVector) {
    vec4state sliceVector = intVector.getPartSelect(31, 0);
    EXPECT_TRUE(compareVectorToString(sliceVector, string("00010010001101000101011001111000")));
    EXPECT_TRUE(checkVectorSize(sliceVector, 32));
}

TEST_F(vec4stateTest, TestGetPartSelectTooHighEndIntVector) {
    vec4state sliceVector = intVector.getPartSelect(33, 0);
    EXPECT_TRUE(compareVectorToString(sliceVector, string("xx00010010001101000101011001111000")));
    EXPECT_TRUE(checkVectorSize(sliceVector, 34));
}

TEST_F(vec4stateTest, TestGetPartSelectTooLowStartIntVector) {
    vec4state sliceVector = intVector.getPartSelect(5, -5);
    EXPECT_TRUE(compareVectorToString(sliceVector, string("111000xxxxx")));
    EXPECT_TRUE(checkVectorSize(sliceVector, 11));
}

TEST_F(vec4stateTest, TestGetPartSelectTooLowStartAndTooHighEndIntVector) {
    vec4state sliceVector = intVector.getPartSelect(32, -5);
    EXPECT_TRUE(compareVectorToString(sliceVector, string("x00010010001101000101011001111000xxxxx")));
    EXPECT_TRUE(checkVectorSize(sliceVector, 38));
}

TEST_F(vec4stateTest, TestGetPartSelectBetweenVPIsBigVector) {
    vec4state sliceVector = bigVector.getPartSelect(68, 62);
    EXPECT_TRUE(compareVectorToString(sliceVector, string("0011xzx")));
    EXPECT_TRUE(checkVectorSize(sliceVector, 7));
}

TEST_F(vec4stateTest, TestSetPartSelectIntVectorToString) {
    intVector.setPartSelect(2, 0, string("111"));
    EXPECT_TRUE(compareVectorToString(intVector, string("00010010001101000101011001111111")));
    EXPECT_TRUE(checkVectorSize(intVector, 32));
}

TEST_F(vec4stateTest, TestSetPartSelectIntVectorToStringVector) {
    intVector.setPartSelect(10, 4, stringVector);
    EXPECT_TRUE(compareVectorToString(intVector, string("000100100011010001010001xz111000")));
    EXPECT_TRUE(checkVectorSize(intVector, 32));
}

TEST_F(vec4stateTest, TestSetPartSelectIntVectorToInteger) {
    intVector.setPartSelect(10, 4, 15);
    EXPECT_TRUE(compareVectorToString(intVector, string("00010010001101000101000011111000")));
    EXPECT_TRUE(checkVectorSize(intVector, 32));
}

TEST_F(vec4stateTest, TestSetPartSelectTooBigValueIntVector) {
    intVector.setPartSelect(1, 0, string("111"));
    EXPECT_TRUE(compareVectorToString(intVector, string("00010010001101000101011001111011")));
    EXPECT_TRUE(checkVectorSize(intVector, 32));
}

TEST_F(vec4stateTest, TestSetPartSelectTooSmallValueIntVector) {
    intVector.setPartSelect(4, 0, string("x"));
    EXPECT_TRUE(compareVectorToString(intVector, string("0001001000110100010101100110000x")));
    EXPECT_TRUE(checkVectorSize(intVector, 32));
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

TEST_F(vec4stateTest, TestLogicalAndIntVectorWithItself) {
    vec4state andVector = intVector && intVector;
    EXPECT_TRUE(compareVectorToString(andVector, string("1")));
    EXPECT_TRUE(checkVectorSize(andVector, 1));
}

TEST_F(vec4stateTest, TestLogicalAndIntVectorWithInteger) {
    vec4state andVector = intVector && 0x12345678;
    EXPECT_TRUE(compareVectorToString(andVector, string("1")));
    EXPECT_TRUE(checkVectorSize(andVector, 1));
}

TEST_F(vec4stateTest, TestLogicalAndIntVectorWithLongLongVector) {
    vec4state andVector = intVector && longLongVector;
    EXPECT_TRUE(compareVectorToString(andVector, string("1")));
    EXPECT_TRUE(checkVectorSize(andVector, 1));
}

TEST_F(vec4stateTest, TestLogicalAndIntVectorWithStringVector) {
    vec4state andVector = intVector && stringVector;
    EXPECT_TRUE(compareVectorToString(andVector, string("1")));
    EXPECT_TRUE(checkVectorSize(andVector, 1));
}

TEST_F(vec4stateTest, TestLogicalAndIntVectorWithXVector) {
    vec4state andVector = intVector && xVector;
    EXPECT_TRUE(compareVectorToString(andVector, string("x")));
    EXPECT_TRUE(checkVectorSize(andVector, 1));
}

TEST_F(vec4stateTest, TestLogicalAndStringVectorWithZVector) {
    vec4state andVector = stringVector && zVector;
    EXPECT_TRUE(compareVectorToString(andVector, string("x")));
    EXPECT_TRUE(checkVectorSize(andVector, 1));
}

TEST_F(vec4stateTest, TestLogicalAndZVectorWithZeroesVector) {
    vec4state andVector = zVector && zeroesVector;
    EXPECT_TRUE(compareVectorToString(andVector, string("0")));
    EXPECT_TRUE(checkVectorSize(andVector, 1));
}

TEST_F(vec4stateTest, TestLogicalAndXVectorWithZVector) {
    vec4state andVector = xVector && zVector;
    EXPECT_TRUE(compareVectorToString(andVector, string("x")));
    EXPECT_TRUE(checkVectorSize(andVector, 1));
}

TEST_F(vec4stateTest, TestLogicalAndZeroesVectorWithOnesVector) {
    vec4state andVector = zeroesVector && onesVector;
    EXPECT_TRUE(compareVectorToString(andVector, string("0")));
    EXPECT_TRUE(checkVectorSize(andVector, 1));
}

TEST_F(vec4stateTest, TestLogicalOrIntVectorWithItself) {
    vec4state orVector = intVector || intVector;
    EXPECT_TRUE(compareVectorToString(orVector, string("1")));
    EXPECT_TRUE(checkVectorSize(orVector, 1));
}

TEST_F(vec4stateTest, TestLogicalOrIntVectorWithInteger) {
    vec4state orVector = intVector || 0x12345678;
    EXPECT_TRUE(compareVectorToString(orVector, string("1")));
    EXPECT_TRUE(checkVectorSize(orVector, 1));
}

TEST_F(vec4stateTest, TestLogicalOrIntVectorWithLongLongVector) {
    vec4state orVector = intVector || longLongVector;
    EXPECT_TRUE(compareVectorToString(orVector, string("1")));
    EXPECT_TRUE(checkVectorSize(orVector, 1));
}

TEST_F(vec4stateTest, TestLogicalOrIntVectorWithStringVector) {
    vec4state orVector = intVector || stringVector;
    EXPECT_TRUE(compareVectorToString(orVector, string("1")));
    EXPECT_TRUE(checkVectorSize(orVector, 1));
}

TEST_F(vec4stateTest, TestLogicalOrIntVectorWithXVector) {
    vec4state orVector = intVector || xVector;
    EXPECT_TRUE(compareVectorToString(orVector, string("1")));
    EXPECT_TRUE(checkVectorSize(orVector, 1));
}

TEST_F(vec4stateTest, TestLogicalOrStringVectorWithZVector) {
    vec4state orVector = stringVector || zVector;
    EXPECT_TRUE(compareVectorToString(orVector, string("1")));
    EXPECT_TRUE(checkVectorSize(orVector, 1));
}

TEST_F(vec4stateTest, TestLogicalOrZVectorWithZero) {
    vec4state orVector = zVector || 0;
    EXPECT_TRUE(compareVectorToString(orVector, string("x")));
    EXPECT_TRUE(checkVectorSize(orVector, 1));
}

TEST_F(vec4stateTest, TestLogicalOrXVectorWithZVector) {
    vec4state orVector = xVector || zVector;
    EXPECT_TRUE(compareVectorToString(orVector, string("x")));
    EXPECT_TRUE(checkVectorSize(orVector, 1));
}

TEST_F(vec4stateTest, TestLogicalOrZeroesVectorWithZero) {
    vec4state orVector = zeroesVector || 0;
    EXPECT_TRUE(compareVectorToString(orVector, string("0")));
    EXPECT_TRUE(checkVectorSize(orVector, 1));
}

TEST_F(vec4stateTest, TestLogicalNotIntVector) {
    vec4state notVector = !intVector;
    EXPECT_TRUE(compareVectorToString(notVector, string("0")));
    EXPECT_TRUE(checkVectorSize(notVector, 1));
}

TEST_F(vec4stateTest, TestLogicalNotStringVector) {
    vec4state notVector = !stringVector;
    EXPECT_TRUE(compareVectorToString(notVector, string("0")));
    EXPECT_TRUE(checkVectorSize(notVector, 1));
}

TEST_F(vec4stateTest, TestLogicalNotXVector) {
    vec4state notVector = !xVector;
    EXPECT_TRUE(compareVectorToString(notVector, string("x")));
    EXPECT_TRUE(checkVectorSize(notVector, 1));
}

TEST_F(vec4stateTest, TestLogicalNotZVector) {
    vec4state notVector = !zVector;
    EXPECT_TRUE(compareVectorToString(notVector, string("x")));
    EXPECT_TRUE(checkVectorSize(notVector, 1));
}

TEST_F(vec4stateTest, TestLogicalNotZeroesVector) {
    vec4state notVector = !zeroesVector;
    EXPECT_TRUE(compareVectorToString(notVector, string("1")));
    EXPECT_TRUE(checkVectorSize(notVector, 1));
}

TEST_F(vec4stateTest, TestRelationalIntVectorLessThanItself) {
    vec4state ltVector = intVector < intVector;
    EXPECT_TRUE(compareVectorToString(ltVector, string("0")));
    EXPECT_TRUE(checkVectorSize(ltVector, 1));
}

TEST_F(vec4stateTest, TestRelationalIntVectorLessThanInteger) {
    vec4state ltVector = intVector < 0x12345678;
    EXPECT_TRUE(compareVectorToString(ltVector, string("0")));
    EXPECT_TRUE(checkVectorSize(ltVector, 1));
}

TEST_F(vec4stateTest, TestRelationalLongLongVectorLessThanIntVector) {
    vec4state ltVector = longLongVector < intVector;
    EXPECT_TRUE(compareVectorToString(ltVector, string("0")));
    EXPECT_TRUE(checkVectorSize(ltVector, 1));
}

TEST_F(vec4stateTest, TestRelationalIntVectorLessThanLongLongVector) {
    vec4state ltVector = intVector < longLongVector;
    EXPECT_TRUE(compareVectorToString(ltVector, string("1")));
    EXPECT_TRUE(checkVectorSize(ltVector, 1));
}

TEST_F(vec4stateTest, TestRelationalIntVectorLessThanStringVector) {
    vec4state ltVector = intVector < stringVector;
    EXPECT_TRUE(compareVectorToString(ltVector, string("x")));
    EXPECT_TRUE(checkVectorSize(ltVector, 1));
}

TEST_F(vec4stateTest, TestRelationalZeroesVectorLessThanOnesVector) {
    vec4state ltVector = zeroesVector < onesVector;
    EXPECT_TRUE(compareVectorToString(ltVector, string("1")));
    EXPECT_TRUE(checkVectorSize(ltVector, 1));
}

TEST_F(vec4stateTest, TestRelationalIntVectorGreaterThanItself) {
    vec4state gtVector = intVector > intVector;
    EXPECT_TRUE(compareVectorToString(gtVector, string("0")));
    EXPECT_TRUE(checkVectorSize(gtVector, 1));
}

TEST_F(vec4stateTest, TestRelationalIntVectorGreaterThanInteger) {
    vec4state gtVector = intVector > 0x12345678;
    EXPECT_TRUE(compareVectorToString(gtVector, string("0")));
    EXPECT_TRUE(checkVectorSize(gtVector, 1));
}

TEST_F(vec4stateTest, TestRelationalIntVectorGreaterThanLongLongVector) {
    vec4state gtVector = intVector > longLongVector;
    EXPECT_TRUE(compareVectorToString(gtVector, string("0")));
    EXPECT_TRUE(checkVectorSize(gtVector, 1));
}

TEST_F(vec4stateTest, TestRelationalLongLongVectorGreaterThanIntVector) {
    vec4state gtVector = longLongVector > intVector;
    EXPECT_TRUE(compareVectorToString(gtVector, string("1")));
    EXPECT_TRUE(checkVectorSize(gtVector, 1));
}

TEST_F(vec4stateTest, TestRelationalIntVectorGreaterThanStringVector) {
    vec4state gtVector = intVector > stringVector;
    EXPECT_TRUE(compareVectorToString(gtVector, string("x")));
    EXPECT_TRUE(checkVectorSize(gtVector, 1));
}

TEST_F(vec4stateTest, TestRelationalZeroesVectorGreaterThanOnesVector) {
    vec4state gtVector = zeroesVector > onesVector;
    EXPECT_TRUE(compareVectorToString(gtVector, string("0")));
    EXPECT_TRUE(checkVectorSize(gtVector, 1));
}

TEST_F(vec4stateTest, TestRelationalIntVectorLessThanEqualToItself) {
    vec4state lteVector = intVector <= intVector;
    EXPECT_TRUE(compareVectorToString(lteVector, string("1")));
    EXPECT_TRUE(checkVectorSize(lteVector, 1));
}

TEST_F(vec4stateTest, TestRelationalIntVectorLessThanEqualToInteger) {
    vec4state lteVector = intVector <= 0x12345678;
    EXPECT_TRUE(compareVectorToString(lteVector, string("1")));
    EXPECT_TRUE(checkVectorSize(lteVector, 1));
}

TEST_F(vec4stateTest, TestRelationalLongLongVectorLessThanEqualToIntVector) {
    vec4state lteVector = longLongVector <= intVector;
    EXPECT_TRUE(compareVectorToString(lteVector, string("0")));
    EXPECT_TRUE(checkVectorSize(lteVector, 1));
}

TEST_F(vec4stateTest, TestRelationalIntVectorLessThanEqualToLongLongVector) {
    vec4state lteVector = intVector <= longLongVector;
    EXPECT_TRUE(compareVectorToString(lteVector, string("1")));
    EXPECT_TRUE(checkVectorSize(lteVector, 1));
}

TEST_F(vec4stateTest, TestRelationalIntVectorLessThanEqualToStringVector) {
    vec4state lteVector = intVector <= stringVector;
    EXPECT_TRUE(compareVectorToString(lteVector, string("x")));
    EXPECT_TRUE(checkVectorSize(lteVector, 1));
}

TEST_F(vec4stateTest, TestRelationalZeroesVectorLessThanEqualToOnesVector) {
    vec4state lteVector = zeroesVector <= onesVector;
    EXPECT_TRUE(compareVectorToString(lteVector, string("1")));
    EXPECT_TRUE(checkVectorSize(lteVector, 1));
}

TEST_F(vec4stateTest, TestRelationalIntVectorGreaterThanEqualToItself) {
    vec4state gteVector = intVector >= intVector;
    EXPECT_TRUE(compareVectorToString(gteVector, string("1")));
    EXPECT_TRUE(checkVectorSize(gteVector, 1));
}

TEST_F(vec4stateTest, TestRelationalIntVectorGreaterThanEqualToInteger) {
    vec4state gteVector = intVector >= 0x12345678;
    EXPECT_TRUE(compareVectorToString(gteVector, string("1")));
    EXPECT_TRUE(checkVectorSize(gteVector, 1));
}

TEST_F(vec4stateTest, TestRelationalLongLongVectorGreaterThanEqualToIntVector) {
    vec4state gteVector = longLongVector >= intVector;
    EXPECT_TRUE(compareVectorToString(gteVector, string("1")));
    EXPECT_TRUE(checkVectorSize(gteVector, 1));
}

TEST_F(vec4stateTest, TestRelationalIntVectorGreaterThanEqualToLongLongVector) {
    vec4state gteVector = intVector >= longLongVector;
    EXPECT_TRUE(compareVectorToString(gteVector, string("0")));
    EXPECT_TRUE(checkVectorSize(gteVector, 1));
}

TEST_F(vec4stateTest, TestRelationalIntVectorGreaterThanEqualToStringVector) {
    vec4state gteVector = intVector >= stringVector;
    EXPECT_TRUE(compareVectorToString(gteVector, string("x")));
    EXPECT_TRUE(checkVectorSize(gteVector, 1));
}

TEST_F(vec4stateTest, TestRelationalZeroesVectorGreaterThanEqualToOnesVector) {
    vec4state gteVector = zeroesVector >= onesVector;
    EXPECT_TRUE(compareVectorToString(gteVector, string("0")));
    EXPECT_TRUE(checkVectorSize(gteVector, 1));
}
