#include <gtest/gtest.h>
#include "../vec4state.h"

class vec4stateTest : public testing::Test {
protected:
    vec4stateTest() {
        // You can do set-up work for each test here.
    }

    virtual ~vec4stateTest() {
        // You can do clean-up work that doesn't throw exceptions here.
    }

    virtual void SetUp() {
        // Code here will be called immediately after the constructor (right before each test).
    }

    virtual void TearDown() {
        // Code here will be called immediately after each test (right before the destructor).
    }

    vec4state defaultVector;
    vec4state intVector(0x12345678);
    vec4state longVector(0x1234567890ABCDEF);
    vec4state stringVector("01xz11");
    vec4state stringVectorWithSize("x", 6);
    vec4state copyVector(stringVector);
};

TEST_F(vec4stateTest, TestConstructors) {
    EXPECT_EQ();
    EXPECT_EQ();
    EXPECT_EQ();
}