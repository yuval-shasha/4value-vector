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
        // Size is the number of bits in the vector
        long long size;

        // Initializes a vector of size "size" (greater than 0) that contains str (of length 1) repeated size times. 
        vec4state(string str, long long size);
        // Returns true if the vector contains x or z, otherwise returns false
        bool isUnknown() const;
        // Returns the number of VPI elements in the vector
        long long getVectorSize() const;
        // Returns the same vector with size newSize. If newSize is greater than the current size, the vector is extended with 0's.
        void incSize(long long newSize);
        // Returns the same vector with size newSize. If newSize is less than the current size, the vector is truncated.
        void decSize(long long newSize);
        // Returns true if the vector is equal to "1", otherwise returns false
        bool isTrue() const;
        // Returns the value of the vector as a long long, use only if the vector has only 2 VPIs (for slicing).
        long long vecToLongLong() const;
        // Returns a vector which it's aval is the result of the bitwise AND operation between the aval of the vector and the aval of other, and it's bval is the result of the bitwise AND operation between the bval of the vector and the bval of other.
        vec4state bitwiseAndAvalBval(const vec4state& other);
        // Returns a vector which it's aval is the result of the addition of the aval of the vector and the aval of other, and it's bval is the result of the addition of the bval of the vector and the bval of other.
        vec4state AdditionAvalBval(const vec4state& other) const;
        // Returns a vector which it's aval and bval are truncated/extended to the size of newSize.
        vec4state resize(long long newSize) const;
        // Returns a vector which holds the value of the vector from end to start.
        vec4state getPartValidRange(long long end, long long start, vec4state& vec) const;

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

        // D'tor
        ~vec4state();

        // Assignment operators
        vec4state& operator=(const vec4state& other);
        vec4state& operator=(int num);
        vec4state& operator=(long long num);
        vec4state& operator=(string str);

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
        vec4state operator==(long long num) const;
        vec4state operator!=(const vec4state& other) const;
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
        vec4state operator<<(const long long num);
        // If other contains x or z, returns a vector that is all x's.
        vec4state operator>>(const vec4state& other);
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
        vec4state operator<(long long num) const;
        vec4state operator>(const vec4state& other) const;
        vec4state operator>(long long num) const;
        vec4state operator<=(const vec4state& other) const;
        vec4state operator<=(long long num) const;
        vec4state operator>=(const vec4state& other) const;
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
        
        // Casting operators
        // TODO: Figure out how to implement these

        /*********** For testing purposes ***********/
        // Returns the size of the vector.
        long long getSize() const;
        // Returns a string representation of the vector.
        string toString() const;
};


#endif // VEC4STATE_H