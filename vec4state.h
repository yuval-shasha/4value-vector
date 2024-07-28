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
        size_t size;

        // Returns true if the vector contains x or z, otherwise returns false
        bool isUnknown() const;
        // Returns the number of VPI elements in the vector
        size_t getVectorSize() const;

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
        // Initializes a vector of size "size" that contains str (of length 1) repeated size times. 
        vec4state(string str, size_t size);
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
        vec4state operator|(const vec4state& other) const;
        vec4state operator|(long long num) const;
        vec4state operator^(const vec4state& other) const;
        vec4state operator^(long long num) const;
        vec4state operator~() const;

        // Equality operators
        vec4state operator==(const vec4state& other) const;
        vec4state operator==(long long num) const;
        vec4state operator!=(const vec4state& other) const;
        vec4state operator!=(long long num) const;
        // Checks if the vector is equal to other, including x's and z's. Returns a vector of 1's if true and 0's otherwise.
        vec4state caseEquality(const vec4state& other) const;
        // Checks if the vector is not equal to other, including x's and z's. Returns a vector of 1's if true and 0's otherwise.
        vec4state caseInequality(const vec4state& other) const;

        // Shift operators
        // If other contains x or z, returns a vector that is all x's.
        vec4state operator<<(const vec4state& other);
        vec4state operator<<(const long long num);
        // If other contains x or z, returns a vector that is all x's.
        vec4state operator>>(const vec4state& other);
        vec4state operator>>(const long long num);

        // Slice methods
        vec4state operator[](const vec4state& index);
        vec4state operator[](const long long index);
        // Returns a vector that is a slice of the original vector from start to end.
        vec4state getSlice(long long end, long long start);
        // Sets the slice of the original vector from start to end to the value of other.
        void setSlice(long long end, long long start, const vec4state& other);

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
        size_t getSize() const;
        // Returns a string representation of the vector.
        string toString() const;
};


#endif // VEC4STATE_H