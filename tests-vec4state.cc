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

TEST_F(vec4stateTest, TestConstructors) {
    // Default constructor
    EXPECT_TRUE(compareVectorToString(defaultVector, string("x")));
    EXPECT_TRUE(checkVectorSize(defaultVector, 1));
    
    // Integer constructor
    EXPECT_TRUE(compareVectorToString(intVector, string("00010010001101000101011001111000")));
    EXPECT_TRUE(checkVectorSize(intVector, 32));

    // Long long constructor
    EXPECT_TRUE(compareVectorToString(longLongVector, string("0001001000110100010101100111100010010000101010111100110111101111")));
    EXPECT_TRUE(checkVectorSize(longLongVector, 64));

    // String constructor
    EXPECT_TRUE(compareVectorToString(stringVector, string("01xz11")));
    EXPECT_TRUE(checkVectorSize(stringVector, 6));
    EXPECT_TRUE(compareVectorToString(emptyStringVector, string("x")));
    EXPECT_TRUE(checkVectorSize(emptyStringVector, 1));

    // Copy constructor
    EXPECT_TRUE(compareVectorToString(copyVector, string("01xz11")));
    EXPECT_TRUE(checkVectorSize(copyVector, 6));
}

TEST_F(vec4stateTest, TestAssignmentOperators) {
    // Copy assignment
    vec4state copy = defaultVector;
    EXPECT_TRUE(compareVectorToString(copy, string("x")));
    EXPECT_TRUE(checkVectorSize(copy, 1));

    // Integer assignment
    copy = 0x12345678;
    EXPECT_TRUE(compareVectorToString(copy, string("00010010001101000101011001111000")));
    EXPECT_TRUE(checkVectorSize(copy, 32));

    // Long long assignment
    copy = 0x1234567890ABCDEF;
    EXPECT_TRUE(compareVectorToString(copy, string("0001001000110100010101100111100010010000101010111100110111101111")));
    EXPECT_TRUE(checkVectorSize(copy, 64));

    // String assignment
    copy = "01xz11";
    EXPECT_TRUE(compareVectorToString(copy, string("01xz11")));
    EXPECT_TRUE(checkVectorSize(copy, 6));
}