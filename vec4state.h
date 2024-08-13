#ifndef VEC4STATE_H
#define VEC4STATE_H

#include <string>
#include <stdint.h>
#include "vpi.h"

using namespace std;

class vec4state
{
    private:
        // The 4 value vector consists of an array of VPI elements, where each element is 32 bits long that can be either 0, 1, x, or z. The first element is the least significant 32 bits.
        VPI* vector;
        // numBits is the number of bits in the vector.
        long long numBits;
        // vectorSize is the number of VPI elements in the vector.
        long long vectorSize;
        // isUnknown is a flag that indicates if the vector contains any x's or z's (true if it does, false otherwise).
        bool unknown;

        // Initializes a vector with numBits bits (greater than 0) that contains str (of length 1) repeated numBits times.
        vec4state(string str, long long numBits);
        // 0-extending the vector to newNumBits.
        void incNumBits(long long newNumBits);
        // Truncates the vector to newNumBits.
        void decNumBits(long long newNumBits);
        // Returns the numerical value (of type long long) that the vector holds. This method can be used only if the vector has up to 2 VPI elements and has no unknown bits.
        long long convertVectorToLongLong() const;
        // Returns a vector which it's aval is the result of the bitwise AND operation between the aval of the vector and the aval of other, and it's bval is the result of the bitwise AND operation between the bval of the vector and the bval of other.
        vec4state bitwiseAndAvalBval(const vec4state& other);
        // Returns a vector which it's aval is the result of the addition of the aval of the vector and the aval of other, and it's bval is the result of the addition of the bval of the vector and the bval of other.
        vec4state AdditionAvalBval(const vec4state& other) const;
        // Truncates/extends the number of bits of the vector to newNumBits.
        void setNumBits(long long newNumBits);
        // Returns a vector which holds the values of this vector from the first bit to end bit, where end is in the range of the number of bits of this vector.
        vec4state getPartValidRange(long long end) const;
        // Sets isUnkwon to true if the vector contains any x's or z's, false otherwise.
        void setUnknown();

    public:
        // C'tors
        // Initializes a 32-bit vector, all bits initialized to x
        vec4state();
        // Initializes a 32-bit vector to the value of num
        vec4state(int num);
        // Initializes a 64-bit vector to the value of num
        vec4state(long long num);
        // Initializes a vector of size str.length with the value of str
        vec4state(string str);
        // Copy c'tor
        vec4state(const vec4state& other);
        // Move c'tor
        vec4state(vec4state&& other);

        // D'tor
        ~vec4state();

        // Assignment operators
        vec4state& operator=(const vec4state& other);
        vec4state& operator=(int num);
        vec4state& operator=(long long num);
        vec4state& operator=(string str);

        // Returns true if the vector is 1, false if the vector is 0.
        operator bool() const;

        // Bitwise operators
        vec4state operator&(const vec4state& other) const;
        vec4state operator&(long long num) const;
        vec4state operator&(int num) const;
        vec4state operator|(const vec4state& other) const;
        vec4state operator|(long long num) const;
        vec4state operator|(int num) const;
        vec4state operator^(const vec4state& other) const;
        vec4state operator^(long long num) const;
        vec4state operator^(int num) const;
        vec4state operator~() const;

        // Equality operators
        vec4state operator==(const vec4state& other) const;
        vec4state operator==(int num) const;
        vec4state operator==(long long num) const;
        vec4state operator!=(const vec4state& other) const;
        vec4state operator!=(int num) const;
        vec4state operator!=(long long num) const;
        // Checks if the vector is equal to other, including x's and z's. Returns a vector of 1's if true and 0's otherwise.
        vec4state caseEquality(const vec4state& other) const;
        vec4state caseEquality(long long num) const;
        // Checks if the vector is not equal to other, including x's and z's. Returns a vector of 1's if true and 0's otherwise.
        vec4state caseInequality(const vec4state& other) const;
        vec4state caseInequality(long long num) const;

        // Shift operators
        // If other contains x or z, returns a vector that is all x's.
        vec4state operator<<(const vec4state& other);
        vec4state operator<<(const int num);
        vec4state operator<<(const long long num);
        // If other contains x or z, returns a vector that is all x's.
        vec4state operator>>(const vec4state& other);
        vec4state operator>>(const int num);
        vec4state operator>>(const long long num);

        // Slice operators
        vec4state getBitSelect(const vec4state& index) const;
        vec4state getBitSelect(const long long index) const;
        void setBitSelect(const vec4state& index, const vec4state& newValue);
        void setBitSelect(const long long index, const vec4state& newValue);
        void setBitSelect(const vec4state& index, const long long newValue);
        void setBitSelect(const long long index, const long long newValue);
        // Returns a vector that is a slice of the original vector from start to end.
        vec4state getPartSelect(long long end, long long start) const;
        // Sets the slice of the original vector from start to end to the value of other.
        void setPartSelect(long long end, long long start, const vec4state& other);
        void setPartSelect(long long end, long long start, long long num);
        void setPartSelect(long long end, long long start, string str);

        // Logical operators
        vec4state operator&&(const vec4state& other) const;
        vec4state operator&&(long long num) const;
        vec4state operator||(const vec4state& other) const;
        vec4state operator||(long long num) const;
        vec4state operator!() const;

        // Relational operators
        vec4state operator<(const vec4state& other) const;
        vec4state operator<(int num) const;
        vec4state operator<(long long num) const;
        vec4state operator>(const vec4state& other) const;
        vec4state operator>(int num) const;
        vec4state operator>(long long num) const;
        vec4state operator<=(const vec4state& other) const;
        vec4state operator<=(int num) const;
        vec4state operator<=(long long num) const;
        vec4state operator>=(const vec4state& other) const;
        vec4state operator>=(int num) const;
        vec4state operator>=(long long num) const;

        // Arithmetic operators
        vec4state operator+(const vec4state& other) const;
        vec4state operator+(long long num) const;
        vec4state operator-(const vec4state& other) const;
        vec4state operator-(long long num) const;
        vec4state operator*(const vec4state& other) const;
        vec4state operator*(long long num) const;
        vec4state operator/(const vec4state& other) const;
        vec4state operator/(long long num) const;
        vec4state operator%(const vec4state& other) const;
        vec4state operator%(long long num) const;
        vec4state operator-() const;
        // Calculates the value of the vector to the power of other.
        vec4state power(const vec4state& other) const;
        // Calculates the value of the vector to the power of num.
        vec4state power(long long num) const;

        // Casting operator
        // Replaces all the unknown values of the vector with 0's.
        void convertToTwoState();
        
        // Getters and setters
        // Returns the size of the vector.
        long long getNumBits() const;
        // Returns the vector of the VPI elements.
        VPI* getVector() const;
        // Returns the number of the VPI elements in the vector.
        long long getVectorSize() const;
        // Returns true if the vector contains any x's or z's, false otherwise.
        bool isUnknown() const;
        // Returns a string representation of the vector.
        string toString() const;
};


#endif // VEC4STATE_H