#include <gtest/gtest.h>
#include "vec4state.h"

class vec4stateTest : public testing::Test {
protected:
    vec4state defaultVector;
    vec4state intVector = vec4state(0x12345678);
    vec4state longLongVector = vec4state(0x1234567890ABCDEF);
    vec4state stringVector = vec4state("01xz11");
    vec4state emptyStringVector = vec4state("");
    vec4state copyVector = vec4state(stringVector);
    vec4state bigVector = vec4state("0110011xzx0111zzzx0110011xzx0111zzzx0110011xzx0111zzzx0110011xzx0111zzzx0110011xzx0111zzzx0110011xzx0111zzzx");
    vec4state onesVector = vec4state("11");
    vec4state xVector = vec4state("xx");
    vec4state zVector = vec4state("zz");
    vec4state xzVector = vec4state("xz");
    vec4state oneAndXVector = vec4state("1x");
    vec4state oneAndZVector = vec4state("1z");
    vec4state zeroAndXVector = vec4state("0x");
    vec4state zeroAndZVector = vec4state("0z");
    vec4state zeroesVector = vec4state("00");
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

TEST_F(vec4stateTest, TestDefaultConstructor) {
    EXPECT_TRUE(compareVectorToString(defaultVector, string("x")));
    EXPECT_TRUE(checkVectorSize(defaultVector, 1));
}

TEST_F(vec4stateTest, TestIntegerConstructor) {
    EXPECT_TRUE(compareVectorToString(intVector, string("00010010001101000101011001111000")));
    EXPECT_TRUE(checkVectorSize(intVector, 32));
}

TEST_F(vec4stateTest, TestLongLongConstructor) {
    EXPECT_TRUE(compareVectorToString(longLongVector, string("0001001000110100010101100111100010010000101010111100110111101111")));
    EXPECT_TRUE(checkVectorSize(longLongVector, 64));
}

TEST_F(vec4stateTest, TestStringConstructor) {
    EXPECT_TRUE(compareVectorToString(stringVector, string("01xz11")));
    EXPECT_TRUE(checkVectorSize(stringVector, 6));

    EXPECT_TRUE(compareVectorToString(emptyStringVector, string("x")));
    EXPECT_TRUE(checkVectorSize(emptyStringVector, 1));

    EXPECT_TRUE(compareVectorToString(bigVector, string("0110011xzx0111zzzx0110011xzx0111zzzx0110011xzx0111zzzx0110011xzx0111zzzx0110011xzx0111zzzx0110011xzx0111zzzx")));
    EXPECT_TRUE(checkVectorSize(bigVector, 108));
}

TEST_F(vec4stateTest, TestCopyConstructor) {
    EXPECT_TRUE(compareVectorToString(copyVector, string("01xz11")));
    EXPECT_TRUE(checkVectorSize(copyVector, 6));
}

TEST_F(vec4stateTest, TestCopyAssignment) {
    vec4state copy = defaultVector;
    EXPECT_TRUE(compareVectorToString(copy, string("x")));
    EXPECT_TRUE(checkVectorSize(copy, 1));
}

TEST_F(vec4stateTest, TestIntegerAssignment) {
    vec4state copy = 0x12345678;
    EXPECT_TRUE(compareVectorToString(copy, string("00010010001101000101011001111000")));
    EXPECT_TRUE(checkVectorSize(copy, 32));
}

TEST_F(vec4stateTest, TestLongLongAssignment) {
    vec4state copy = 0x1234567890ABCDEF;
    EXPECT_TRUE(compareVectorToString(copy, string("0001001000110100010101100111100010010000101010111100110111101111")));
    EXPECT_TRUE(checkVectorSize(copy, 64));
}

TEST_F(vec4stateTest, TestStringAssignment) {
    vec4state copy = "01xz11";
    EXPECT_TRUE(compareVectorToString(copy, string("01xz11")));
    EXPECT_TRUE(checkVectorSize(copy, 6));
}

TEST_F(vec4stateTest, TestBitwiseAndIntVectorWithItself) {
    vec4state andVector = intVector & intVector;
    EXPECT_TRUE(compareVectorToString(andVector, string("00010010001101000101011001111000")));
    EXPECT_TRUE(checkVectorSize(andVector, 32));
}

TEST_F(vec4stateTest, TestBitwiseAndLongLongVectorWithItself) {
    vec4state andVector = longLongVector & longLongVector;
    EXPECT_TRUE(compareVectorToString(andVector, string("0001001000110100010101100111100010010000101010111100110111101111")));
    EXPECT_TRUE(checkVectorSize(andVector, 64));
}

TEST_F(vec4stateTest, TestBitwiseAndIntVectorWithLongLongVector) {
    vec4state andVector = intVector & longLongVector;
    EXPECT_TRUE(compareVectorToString(andVector, string("0000000000000000000000000000000000010000001000000100010001101000")));
    EXPECT_TRUE(checkVectorSize(andVector, 64));
}

TEST_F(vec4stateTest, TestBitwiseAndIntVectorWithLongLong) {
    vec4state andVector = intVector & 0x1234567890ABCDEF;
    EXPECT_TRUE(compareVectorToString(andVector, string("0001001000110100010101100111100010010000101010111100110111101111")));
    EXPECT_TRUE(checkVectorSize(andVector, 64));
}

TEST_F(vec4stateTest, TestBitwiseAndIntVectorWithInteger) {
    vec4state andVector = intVector & 0x12345678;
    EXPECT_TRUE(compareVectorToString(andVector, string("00010010001101000101011001111000")));
    EXPECT_TRUE(checkVectorSize(andVector, 32));
}

TEST_F(vec4stateTest, TestBitwiseAndStringVectorWithItself) {
    vec4state andVector = stringVector & stringVector;
    EXPECT_TRUE(compareVectorToString(andVector, string("01xx11")));
    EXPECT_TRUE(checkVectorSize(andVector, 6));
}

TEST_F(vec4stateTest, TestBitwiseAndSmallVectorWithBigVector) {
    vec4state andVector = stringVector & bigVector;
    EXPECT_TRUE(compareVectorToString(andVector, string("00000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000001xxxx")));
    EXPECT_TRUE(checkVectorSize(andVector, 108));
}

TEST_F(vec4stateTest, TestBitwiseOrIntVectorWithItself) {
    vec4state orVector = intVector | intVector;
    EXPECT_TRUE(compareVectorToString(orVector, string("00010010001101000101011001111000")));
    EXPECT_TRUE(checkVectorSize(orVector, 32));
}

TEST_F(vec4stateTest, TestBitwiseOrLongLongVectorWithItself) {
    vec4state orVector = longLongVector | longLongVector;
    EXPECT_TRUE(compareVectorToString(orVector, string("0001001000110100010101100111100010010000101010111100110111101111")));
    EXPECT_TRUE(checkVectorSize(orVector, 64));
}

TEST_F(vec4stateTest, TestBitwiseOrIntVectorWithLongLongVector) {
    vec4state orVector = intVector | longLongVector;
    EXPECT_TRUE(compareVectorToString(orVector, string("0001001000110100010101100111100010010010101111111101111111111111")));
    EXPECT_TRUE(checkVectorSize(orVector, 64));
}

TEST_F(vec4stateTest, TestBitwiseOrIntVectorWithLongLong) {
    vec4state orVector = intVector | 0x1234567890ABCDEF;
    EXPECT_TRUE(compareVectorToString(orVector, string("0001001000110100010101100111100010010010101111111101111111111111")));
    EXPECT_TRUE(checkVectorSize(orVector, 64));
}

TEST_F(vec4stateTest, TestBitwiseOrIntVectorWithInteger) {
    vec4state orVector = intVector | 0x12345678;
    EXPECT_TRUE(compareVectorToString(orVector, string("00010010001101000101011001111000")));
    EXPECT_TRUE(checkVectorSize(orVector, 32));
}

TEST_F(vec4stateTest, TestBitwiseOrStringVectorWithItself) {
    vec4state orVector = stringVector | stringVector;
    EXPECT_TRUE(compareVectorToString(orVector, string("01xx11")));
    EXPECT_TRUE(checkVectorSize(orVector, 6));
}

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

TEST_F(vec4stateTest, TestBitwiseXorIntVectorWithLongVector) {
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

// TODO: split the following tests into multiple tests!
TEST_F(vec4stateTest, TestEqualityOperators) {
    // ==
    vec4state eqVector = intVector == intVector;
    EXPECT_TRUE(compareVectorToString(eqVector, string("1")));
    EXPECT_TRUE(checkVectorSize(eqVector, 1));

    eqVector = intVector == 0x12345678;
    EXPECT_TRUE(compareVectorToString(eqVector, string("1")));
    EXPECT_TRUE(checkVectorSize(eqVector, 1));

    eqVector = longLongVector == longLongVector;
    EXPECT_TRUE(compareVectorToString(eqVector, string("1")));
    EXPECT_TRUE(checkVectorSize(eqVector, 1));

    eqVector = longLongVector == 0x1234567890ABCDEF;
    EXPECT_TRUE(compareVectorToString(eqVector, string("1")));
    EXPECT_TRUE(checkVectorSize(eqVector, 1));

    eqVector = stringVector == stringVector;
    EXPECT_TRUE(compareVectorToString(eqVector, string("x")));
    EXPECT_TRUE(checkVectorSize(eqVector, 1));

    eqVector = bigVector == bigVector;
    EXPECT_TRUE(compareVectorToString(eqVector, string("x")));
    EXPECT_TRUE(checkVectorSize(eqVector, 1));

    eqVector = intVector == longLongVector;
    EXPECT_TRUE(compareVectorToString(eqVector, string("0")));
    EXPECT_TRUE(checkVectorSize(eqVector, 1));

    eqVector = stringVector == intVector;
    EXPECT_TRUE(compareVectorToString(eqVector, string("0")));
    EXPECT_TRUE(checkVectorSize(eqVector, 1));

    eqVector = stringVector == longLongVector;
    EXPECT_TRUE(compareVectorToString(eqVector, string("0")));
    EXPECT_TRUE(checkVectorSize(eqVector, 1));

    eqVector = stringVector == bigVector;
    EXPECT_TRUE(compareVectorToString(eqVector, string("0")));
    EXPECT_TRUE(checkVectorSize(eqVector, 1));

    eqVector = bigVector == stringVector;
    EXPECT_TRUE(compareVectorToString(eqVector, string("0")));
    EXPECT_TRUE(checkVectorSize(eqVector, 1));

    eqVector = bigVector == intVector;
    EXPECT_TRUE(compareVectorToString(eqVector, string("0")));
    EXPECT_TRUE(checkVectorSize(eqVector, 1));

    eqVector = bigVector == longLongVector;
    EXPECT_TRUE(compareVectorToString(eqVector, string("0")));
    EXPECT_TRUE(checkVectorSize(eqVector, 1));

    eqVector = onesVector == xVector;
    EXPECT_TRUE(compareVectorToString(eqVector, string("x")));
    EXPECT_TRUE(checkVectorSize(eqVector, 1));

    eqVector = onesVector == zVector;
    EXPECT_TRUE(compareVectorToString(eqVector, string("x")));
    EXPECT_TRUE(checkVectorSize(eqVector, 1));

    eqVector = onesVector == oneAndXVector;
    EXPECT_TRUE(compareVectorToString(eqVector, string("x")));
    EXPECT_TRUE(checkVectorSize(eqVector, 1));

    eqVector = onesVector == zeroAndZVector;
    EXPECT_TRUE(compareVectorToString(eqVector, string("0")));
    EXPECT_TRUE(checkVectorSize(eqVector, 1));

    eqVector = xVector == zVector;
    EXPECT_TRUE(compareVectorToString(eqVector, string("x")));
    EXPECT_TRUE(checkVectorSize(eqVector, 1));

    // !=
    vec4state neqVector = intVector != intVector;
    EXPECT_TRUE(compareVectorToString(neqVector, string("0")));
    EXPECT_TRUE(checkVectorSize(neqVector, 1));

    neqVector = intVector != 0x12345678;
    EXPECT_TRUE(compareVectorToString(neqVector, string("0")));
    EXPECT_TRUE(checkVectorSize(neqVector, 1));

    neqVector = intVector != 0x1234567890ABCDEF;
    EXPECT_TRUE(compareVectorToString(neqVector, string("0")));
    EXPECT_TRUE(checkVectorSize(neqVector, 1));

    neqVector = longLongVector != bigVector;
    EXPECT_TRUE(compareVectorToString(neqVector, string("1")));
    EXPECT_TRUE(checkVectorSize(neqVector, 1));

    neqVector = stringVector != bigVector;
    EXPECT_TRUE(compareVectorToString(neqVector, string("1")));
    EXPECT_TRUE(checkVectorSize(neqVector, 1));

    neqVector = stringVector != stringVector;
    EXPECT_TRUE(compareVectorToString(neqVector, string("x")));
    EXPECT_TRUE(checkVectorSize(neqVector, 1));

    neqVector = onesVector != xVector;
    EXPECT_TRUE(compareVectorToString(neqVector, string("x")));
    EXPECT_TRUE(checkVectorSize(neqVector, 1));

    neqVector = onesVector != zVector;
    EXPECT_TRUE(compareVectorToString(neqVector, string("x")));
    EXPECT_TRUE(checkVectorSize(neqVector, 1));

    neqVector = onesVector != oneAndXVector;
    EXPECT_TRUE(compareVectorToString(neqVector, string("x")));
    EXPECT_TRUE(checkVectorSize(neqVector, 1));

    neqVector = onesVector != zeroAndZVector;
    EXPECT_TRUE(compareVectorToString(neqVector, string("1")));
    EXPECT_TRUE(checkVectorSize(neqVector, 1));

    neqVector = xVector != zVector;
    EXPECT_TRUE(compareVectorToString(neqVector, string("x")));
    EXPECT_TRUE(checkVectorSize(neqVector, 1));

    // caseEquality
    vec4state caseEqVector = intVector.caseEquality(intVector);
    EXPECT_TRUE(compareVectorToString(caseEqVector, string("1")));
    EXPECT_TRUE(checkVectorSize(caseEqVector, 1));

    caseEqVector = intVector.caseEquality(0x12345678);
    EXPECT_TRUE(compareVectorToString(caseEqVector, string("1")));
    EXPECT_TRUE(checkVectorSize(caseEqVector, 1));

    caseEqVector = stringVector.caseEquality(stringVector);
    EXPECT_TRUE(compareVectorToString(caseEqVector, string("1")));

    caseEqVector = stringVector.caseEquality(bigVector);
    EXPECT_TRUE(compareVectorToString(caseEqVector, string("0")));
    EXPECT_TRUE(checkVectorSize(caseEqVector, 1));

    caseEqVector = onesVector.caseEquality(xVector);
    EXPECT_TRUE(compareVectorToString(caseEqVector, string("0")));
    EXPECT_TRUE(checkVectorSize(caseEqVector, 1));

    caseEqVector = xVector.caseEquality(zVector);
    EXPECT_TRUE(compareVectorToString(caseEqVector, string("0")));
    EXPECT_TRUE(checkVectorSize(caseEqVector, 1));

    // caseInequality
    vec4state caseNeqVector = intVector.caseInequality(intVector);
    EXPECT_TRUE(compareVectorToString(caseNeqVector, string("0")));
    EXPECT_TRUE(checkVectorSize(caseNeqVector, 1));

    caseNeqVector = intVector.caseInequality(0x12345678);
    EXPECT_TRUE(compareVectorToString(caseNeqVector, string("0")));
    EXPECT_TRUE(checkVectorSize(caseNeqVector, 1));

    caseNeqVector = stringVector.caseInequality(stringVector);
    EXPECT_TRUE(compareVectorToString(caseNeqVector, string("0")));
    EXPECT_TRUE(checkVectorSize(caseNeqVector, 1));

    caseNeqVector = stringVector.caseInequality(bigVector);
    EXPECT_TRUE(compareVectorToString(caseNeqVector, string("1")));
    EXPECT_TRUE(checkVectorSize(caseNeqVector, 1));

    caseNeqVector = onesVector.caseInequality(xVector);
    EXPECT_TRUE(compareVectorToString(caseNeqVector, string("1")));
    EXPECT_TRUE(checkVectorSize(caseNeqVector, 1));

    caseNeqVector = xVector.caseInequality(zVector);
    EXPECT_TRUE(compareVectorToString(caseNeqVector, string("1")));
    EXPECT_TRUE(checkVectorSize(caseNeqVector, 1));
}

TEST_F(vec4stateTest, TestShiftOperators) {
    // <<
    vec4state shiftLeftVector = intVector << 2;
    EXPECT_TRUE(compareVectorToString(shiftLeftVector, string("01001000110100010101100111100000")));
    EXPECT_TRUE(checkVectorSize(shiftLeftVector, 32));

    shiftLeftVector = intVector << 32;
    EXPECT_TRUE(compareVectorToString(shiftLeftVector, string("00000000000000000000000000000000")));
    EXPECT_TRUE(checkVectorSize(shiftLeftVector, 32));

    shiftLeftVector = longLongVector << 2;
    EXPECT_TRUE(compareVectorToString(shiftLeftVector, string("0100100011010001010110011110001001000010101011110011011110111100")));
    EXPECT_TRUE(checkVectorSize(shiftLeftVector, 64));

    shiftLeftVector = longLongVector << 64;
    EXPECT_TRUE(compareVectorToString(shiftLeftVector, string("0000000000000000000000000000000000000000000000000000000000000000")));
    EXPECT_TRUE(checkVectorSize(shiftLeftVector, 64));

    shiftLeftVector = longLongVector << 35;
    EXPECT_TRUE(compareVectorToString(shiftLeftVector, string("1000010101011110011011110111100000000000000000000000000000000000")));
    EXPECT_TRUE(checkVectorSize(shiftLeftVector, 64));

    shiftLeftVector = stringVector << 2;
    EXPECT_TRUE(compareVectorToString(shiftLeftVector, string("xz1100")));
    EXPECT_TRUE(checkVectorSize(shiftLeftVector, 6));

    shiftLeftVector = bigVector << 70;
    EXPECT_TRUE(compareVectorToString(shiftLeftVector, string("zx0110011xzx0111zzzx0110011xzx0111zzzx0000000000000000000000000000000000000000000000000000000000000000000000")));
    EXPECT_TRUE(checkVectorSize(shiftLeftVector, 108));

    shiftLeftVector = bigVector << stringVector;
    EXPECT_TRUE(compareVectorToString(shiftLeftVector, string("xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx")));
    EXPECT_TRUE(checkVectorSize(shiftLeftVector, 108));

    shiftLeftVector = bigVector << onesVector;
    EXPECT_TRUE(compareVectorToString(shiftLeftVector, string("0011xzx0111zzzx0110011xzx0111zzzx0110011xzx0111zzzx0110011xzx0111zzzx0110011xzx0111zzzx0110011xzx0111zzzx000")));
    EXPECT_TRUE(checkVectorSize(shiftLeftVector, 108));

    shiftLeftVector = bigVector << intVector;
    EXPECT_TRUE(compareVectorToString(shiftLeftVector, string("000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000")));
    EXPECT_TRUE(checkVectorSize(shiftLeftVector, 108));

    // >>
    vec4state shiftRightVector = intVector >> 2;
    EXPECT_TRUE(compareVectorToString(shiftRightVector, string("00000100100011010001010110011110")));
    EXPECT_TRUE(checkVectorSize(shiftRightVector, 32));

    shiftRightVector = intVector >> 32;
    EXPECT_TRUE(compareVectorToString(shiftRightVector, string("00000000000000000000000000000000")));
    EXPECT_TRUE(checkVectorSize(shiftRightVector, 32));

    shiftRightVector = longLongVector >> 2;
    EXPECT_TRUE(compareVectorToString(shiftRightVector, string("0000010010001101000101011001111000100100001010101111001101111011")));
    EXPECT_TRUE(checkVectorSize(shiftRightVector, 64));

    shiftRightVector = longLongVector >> 64;
    EXPECT_TRUE(compareVectorToString(shiftRightVector, string("0000000000000000000000000000000000000000000000000000000000000000")));
    EXPECT_TRUE(checkVectorSize(shiftRightVector, 64));

    shiftRightVector = longLongVector >> 35;
    EXPECT_TRUE(compareVectorToString(shiftRightVector, string("0000000000000000000000000000000000000010010001101000101011001111")));
    EXPECT_TRUE(checkVectorSize(shiftRightVector, 64));

    shiftRightVector = stringVector >> 2;
    EXPECT_TRUE(compareVectorToString(shiftRightVector, string("0001xz")));
    EXPECT_TRUE(checkVectorSize(shiftRightVector, 6));

    shiftRightVector = bigVector >> 70;
    EXPECT_TRUE(compareVectorToString(shiftRightVector, string("00000000000000000000000000000000000000000000000000000000000000000000000110011xzx0111zzzx0110011xzx0111zzzx01")));
    EXPECT_TRUE(checkVectorSize(shiftRightVector, 108));

    shiftRightVector = bigVector >> stringVector;
    EXPECT_TRUE(compareVectorToString(shiftRightVector, string("xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx")));
    EXPECT_TRUE(checkVectorSize(shiftRightVector, 108));

    shiftRightVector = bigVector >> onesVector;
    EXPECT_TRUE(compareVectorToString(shiftRightVector, string("0000110011xzx0111zzzx0110011xzx0111zzzx0110011xzx0111zzzx0110011xzx0111zzzx0110011xzx0111zzzx0110011xzx0111z")));
    EXPECT_TRUE(checkVectorSize(shiftRightVector, 108));

    shiftRightVector = bigVector >> intVector;
    EXPECT_TRUE(compareVectorToString(shiftRightVector, string("000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000")));
    EXPECT_TRUE(checkVectorSize(shiftRightVector, 108));
}
/*
TEST_F(vec4stateTest, TestSliceOperators) {
    // []
    vec4state indexVector = intVector[2];
    EXPECT_TRUE(compareVectorToString(indexVector, string("0")));
    EXPECT_TRUE(checkVectorSize(indexVector, 1));

    indexVector = intVector[32];
    EXPECT_TRUE(compareVectorToString(indexVector, string("x")));
    EXPECT_TRUE(checkVectorSize(indexVector, 1));

    indexVector = longLongVector[50];
    EXPECT_TRUE(compareVectorToString(indexVector, string("1")));
    EXPECT_TRUE(checkVectorSize(indexVector, 1));

    indexVector = longLongVector[70];
    EXPECT_TRUE(compareVectorToString(indexVector, string("x")));
    EXPECT_TRUE(checkVectorSize(indexVector, 1));

    indexVector = stringVector[2];
    EXPECT_TRUE(compareVectorToString(indexVector, string("z")));
    EXPECT_TRUE(checkVectorSize(indexVector, 1));

    indexVector = stringVector[3];
    EXPECT_TRUE(compareVectorToString(indexVector, string("x")));
    EXPECT_TRUE(checkVectorSize(indexVector, 1));

    indexVector = bigVector[70];
    EXPECT_TRUE(compareVectorToString(indexVector, string("1")));
    EXPECT_TRUE(checkVectorSize(indexVector, 1));
    
    indexVector = intVector[-1];
    EXPECT_TRUE(compareVectorToString(indexVector, string("x")));
    EXPECT_TRUE(checkVectorSize(indexVector, 1));

    // getSlice
    vec4state sliceVector = intVector.getSlice(2, 0);
    EXPECT_TRUE(compareVectorToString(sliceVector, string("000")));
    EXPECT_TRUE(checkVectorSize(sliceVector, 3));

    sliceVector = intVector.getSlice(31, 0);
    EXPECT_TRUE(compareVectorToString(sliceVector, string("00010010001101000101011001111000")));
    EXPECT_TRUE(checkVectorSize(sliceVector, 31));

    sliceVector = intVector.getSlice(33, 0);
    EXPECT_TRUE(compareVectorToString(sliceVector, string("xx00010010001101000101011001111000")));
    EXPECT_TRUE(checkVectorSize(sliceVector, 33));

    sliceVector = intVector.getSlice(32, -5);
    EXPECT_TRUE(compareVectorToString(sliceVector, string("x00010010001101000101011001111000xxxxx")));
    EXPECT_TRUE(checkVectorSize(sliceVector, 38));

    sliceVector = intVector.getSlice(35, 30);
    EXPECT_TRUE(compareVectorToString(sliceVector, string("xxxx00")));
    EXPECT_TRUE(checkVectorSize(sliceVector, 6));

    sliceVector = intVector.getSlice(5, -5);
    EXPECT_TRUE(compareVectorToString(sliceVector, string("111000xxxxx")));
    EXPECT_TRUE(checkVectorSize(sliceVector, 11));

    sliceVector = bigVector.getSlice(68, 62);
    EXPECT_TRUE(compareVectorToString(sliceVector, string("0011xzx")));
    EXPECT_TRUE(checkVectorSize(sliceVector, 7));

    // setSlice
    intVector.setSlice(2, 0, string("111"));
    EXPECT_TRUE(compareVectorToString(intVector, string("00010010001101000101011001111111")));
    EXPECT_TRUE(checkVectorSize(intVector, 32));

    intVector = 0x12345678;

    intVector.setSlice(10, 4, stringVector);
    EXPECT_TRUE(compareVectorToString(intVector, string("000100100011010001010001xz111000")));
    EXPECT_TRUE(checkVectorSize(intVector, 32));

    intVector = 0x12345678;

    intVector.setSlice(10, 4, 15);
    EXPECT_TRUE(compareVectorToString(intVector, string("00010010001101000101000011111000")));
    EXPECT_TRUE(checkVectorSize(intVector, 32));

    intVector = 0x12345678;

    intVector.setSlice(1, 0, string("111"));
    EXPECT_TRUE(compareVectorToString(intVector, string("00010010001101000101011001111011")));
    EXPECT_TRUE(checkVectorSize(intVector, 32));

    intVector = 0x12345678;

    intVector.setSlice(4, 0, string("x"));
    EXPECT_TRUE(compareVectorToString(intVector, string("0001001000110100010101100110000x")));
    EXPECT_TRUE(checkVectorSize(intVector, 32));

    intVector = 0x12345678;

    intVector.setSlice(35, 30, stringVector);
    EXPECT_TRUE(compareVectorToString(intVector, string("11010010001101000101011001111000")));
    EXPECT_TRUE(checkVectorSize(intVector, 32));

    intVector = 0x12345678;

    intVector.setSlice(5, -5, stringVector);
    EXPECT_TRUE(compareVectorToString(intVector, string("00010010001101000101011001000000")));
    EXPECT_TRUE(checkVectorSize(intVector, 32));

    intVector = 0x12345678;

    intVector.setSlice(-4, -5, xVector);
    EXPECT_TRUE(compareVectorToString(intVector, string("00010010001101000101011001111000")));
    EXPECT_TRUE(checkVectorSize(intVector, 32));

    intVector = 0x12345678;

    intVector.setSlice(34, 33, xVector);
    EXPECT_TRUE(compareVectorToString(intVector, string("00010010001101000101011001111000")));
    EXPECT_TRUE(checkVectorSize(intVector, 32));
}
*/
TEST_F(vec4stateTest, TestLogicalOperators) {
    // &&
    vec4state andVector = intVector && intVector;
    EXPECT_TRUE(compareVectorToString(andVector, string("1")));
    EXPECT_TRUE(checkVectorSize(andVector, 1));

    andVector = intVector && 0x12345678;
    EXPECT_TRUE(compareVectorToString(andVector, string("1")));
    EXPECT_TRUE(checkVectorSize(andVector, 1));

    andVector = intVector && longLongVector;
    EXPECT_TRUE(compareVectorToString(andVector, string("1")));
    EXPECT_TRUE(checkVectorSize(andVector, 1));

    andVector = intVector && stringVector;
    EXPECT_TRUE(compareVectorToString(andVector, string("1")));
    EXPECT_TRUE(checkVectorSize(andVector, 1));

    andVector = intVector && xVector;
    EXPECT_TRUE(compareVectorToString(andVector, string("x")));
    EXPECT_TRUE(checkVectorSize(andVector, 1));

    andVector = stringVector && zVector;
    EXPECT_TRUE(compareVectorToString(andVector, string("x")));
    EXPECT_TRUE(checkVectorSize(andVector, 1));

    andVector = zVector && zeroesVector;
    EXPECT_TRUE(compareVectorToString(andVector, string("0")));
    EXPECT_TRUE(checkVectorSize(andVector, 1));

    andVector = xVector && zVector;
    EXPECT_TRUE(compareVectorToString(andVector, string("x")));
    EXPECT_TRUE(checkVectorSize(andVector, 1));

    // ||
    vec4state orVector = intVector || intVector;
    EXPECT_TRUE(compareVectorToString(orVector, string("1")));
    EXPECT_TRUE(checkVectorSize(orVector, 1));

    orVector = intVector || 0x12345678;
    EXPECT_TRUE(compareVectorToString(orVector, string("1")));
    EXPECT_TRUE(checkVectorSize(orVector, 1));

    orVector = intVector || longLongVector;
    EXPECT_TRUE(compareVectorToString(orVector, string("1")));
    EXPECT_TRUE(checkVectorSize(orVector, 1));

    orVector = intVector || stringVector;
    EXPECT_TRUE(compareVectorToString(orVector, string("1")));
    EXPECT_TRUE(checkVectorSize(orVector, 1));

    orVector = intVector || xVector;
    EXPECT_TRUE(compareVectorToString(orVector, string("1")));
    EXPECT_TRUE(checkVectorSize(orVector, 1));

    orVector = stringVector || zVector;
    EXPECT_TRUE(compareVectorToString(orVector, string("1")));
    EXPECT_TRUE(checkVectorSize(orVector, 1));

    orVector = zVector || 0;
    EXPECT_TRUE(compareVectorToString(orVector, string("x")));
    EXPECT_TRUE(checkVectorSize(orVector, 1));

    orVector = xVector || zVector;
    EXPECT_TRUE(compareVectorToString(orVector, string("x")));
    EXPECT_TRUE(checkVectorSize(orVector, 1));

    orVector = zeroesVector || 0;
    EXPECT_TRUE(compareVectorToString(orVector, string("0")));
    EXPECT_TRUE(checkVectorSize(orVector, 1));

    // !
    vec4state notVector = !intVector;
    EXPECT_TRUE(compareVectorToString(notVector, string("0")));
    EXPECT_TRUE(checkVectorSize(notVector, 1));

    notVector = !stringVector;
    EXPECT_TRUE(compareVectorToString(notVector, string("0")));
    EXPECT_TRUE(checkVectorSize(notVector, 1));

    notVector = !xVector;
    EXPECT_TRUE(compareVectorToString(notVector, string("x")));
    EXPECT_TRUE(checkVectorSize(notVector, 1));

    notVector = !zVector;
    EXPECT_TRUE(compareVectorToString(notVector, string("x")));
    EXPECT_TRUE(checkVectorSize(notVector, 1));

    notVector = !zeroesVector;
    EXPECT_TRUE(compareVectorToString(notVector, string("1")));
    EXPECT_TRUE(checkVectorSize(notVector, 1));
}

TEST_F(vec4stateTest, TestRelationalOperators) {
    // <
    vec4state ltVector = intVector < intVector;
    EXPECT_TRUE(compareVectorToString(ltVector, string("0")));
    EXPECT_TRUE(checkVectorSize(ltVector, 1));

    ltVector = intVector < 0x12345678;
    EXPECT_TRUE(compareVectorToString(ltVector, string("0")));
    EXPECT_TRUE(checkVectorSize(ltVector, 1));

    ltVector = longLongVector < intVector;
    EXPECT_TRUE(compareVectorToString(ltVector, string("0")));
    EXPECT_TRUE(checkVectorSize(ltVector, 1));

    ltVector = intVector < longLongVector;
    EXPECT_TRUE(compareVectorToString(ltVector, string("1")));
    EXPECT_TRUE(checkVectorSize(ltVector, 1));

    ltVector = intVector < stringVector;
    EXPECT_TRUE(compareVectorToString(ltVector, string("x")));
    EXPECT_TRUE(checkVectorSize(ltVector, 1));

    ltVector = zeroesVector < onesVector;
    EXPECT_TRUE(compareVectorToString(ltVector, string("1")));
    EXPECT_TRUE(checkVectorSize(ltVector, 1));

    // >
    vec4state gtVector = intVector > intVector;
    EXPECT_TRUE(compareVectorToString(gtVector, string("0")));
    EXPECT_TRUE(checkVectorSize(gtVector, 1));

    gtVector = intVector > 0x12345678;
    EXPECT_TRUE(compareVectorToString(gtVector, string("0")));
    EXPECT_TRUE(checkVectorSize(gtVector, 1));

    gtVector = intVector > longLongVector;
    EXPECT_TRUE(compareVectorToString(gtVector, string("0")));
    EXPECT_TRUE(checkVectorSize(gtVector, 1));

    gtVector = longLongVector > intVector;
    EXPECT_TRUE(compareVectorToString(gtVector, string("1")));
    EXPECT_TRUE(checkVectorSize(gtVector, 1));

    gtVector = intVector > stringVector;
    EXPECT_TRUE(compareVectorToString(gtVector, string("x")));
    EXPECT_TRUE(checkVectorSize(gtVector, 1));

    gtVector = zeroesVector > onesVector;
    EXPECT_TRUE(compareVectorToString(gtVector, string("0")));
    EXPECT_TRUE(checkVectorSize(gtVector, 1));

    // <=
    vec4state lteVector = intVector <= intVector;
    EXPECT_TRUE(compareVectorToString(lteVector, string("1")));
    EXPECT_TRUE(checkVectorSize(lteVector, 1));

    lteVector = intVector <= 0x12345678;
    EXPECT_TRUE(compareVectorToString(lteVector, string("1")));
    EXPECT_TRUE(checkVectorSize(lteVector, 1));

    lteVector = longLongVector <= intVector;
    EXPECT_TRUE(compareVectorToString(lteVector, string("0")));
    EXPECT_TRUE(checkVectorSize(lteVector, 1));

    lteVector = intVector <= longLongVector;
    EXPECT_TRUE(compareVectorToString(lteVector, string("1")));
    EXPECT_TRUE(checkVectorSize(lteVector, 1));

    lteVector = intVector <= stringVector;
    EXPECT_TRUE(compareVectorToString(lteVector, string("x")));
    EXPECT_TRUE(checkVectorSize(lteVector, 1));

    lteVector = zeroesVector <= onesVector;
    EXPECT_TRUE(compareVectorToString(lteVector, string("1")));
    EXPECT_TRUE(checkVectorSize(lteVector, 1));

    // >=
    vec4state gteVector = intVector >= intVector;
    EXPECT_TRUE(compareVectorToString(gteVector, string("1")));
    EXPECT_TRUE(checkVectorSize(gteVector, 1));

    gteVector = intVector >= 0x12345678;
    EXPECT_TRUE(compareVectorToString(gteVector, string("1")));
    EXPECT_TRUE(checkVectorSize(gteVector, 1));

    gteVector = longLongVector >= intVector;
    EXPECT_TRUE(compareVectorToString(gteVector, string("1")));
    EXPECT_TRUE(checkVectorSize(gteVector, 1));

    gteVector = intVector >= longLongVector;
    EXPECT_TRUE(compareVectorToString(gteVector, string("0")));
    EXPECT_TRUE(checkVectorSize(gteVector, 1));

    gteVector = intVector >= stringVector;
    EXPECT_TRUE(compareVectorToString(gteVector, string("x")));
    EXPECT_TRUE(checkVectorSize(gteVector, 1));

    gteVector = zeroesVector >= onesVector;
    EXPECT_TRUE(compareVectorToString(gteVector, string("0")));
    EXPECT_TRUE(checkVectorSize(gteVector, 1));
}