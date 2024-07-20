#ifndef VEC4STATE_H
#define VEC4STATE_H

#include <string>
#include <stdint.h>
#include "vpi.h"

using namespace std;

class vec4state
{
    private:
        VPI* vector;
        size_t size;
        // Getters and setters for internal use
        // Returns the a_val of the VPI at index "index" in the field vector
        uint32_t getAvalAtIndex(unsigned int index) const;
        // Sets the a_val of the VPI at index "index" in the field vector to new_val
        void setAvalAtIndex(unsigned int index, uint32_t new_val);
        // Returns the b_val of the VPI at index "index" in the field vector
        uint32_t getBvalAtIndex(unsigned int index) const;
        // Sets the b_val of the VPI at index "index" in the field vector to new_val
        void setBvalAtIndex(unsigned int index, uint32_t new_val);
        // Returns true if the vector contains x or z, otherwise returns false
        bool isUnknown() const;

    public:
        // C'tors
        // Initializes a 32-bit vector, all bits initialized to x
        vec4state();
        // Initializes a 32-bit vector to the value of num
        vec4state(int num);
        // Initializes a 64-bit vector to the value of num
        vec4state(long num);
        // Initializes a vector of size str.length with the value of str
        vec4state(string str);
        // Initializes a vector of size "size" with the value of str
        vec4state(string str, size_t size);

        // D'tor
        ~vec4state();

        // Assignment operators
        vec4state& operator=(int num);
        vec4state& operator=(long num);
        vec4state& operator=(string str);
        // If other contains only x's, the assigned vector is all x's.
        vec4state& operator=(const vec4state& other);

        // Bitwise operators
        vec4state operator&(const vec4state& other) const;
        vec4state operator&(long num) const;
        vec4state operator|(const vec4state& other) const;
        vec4state operator|(long num) const;
        vec4state operator^(const vec4state& other) const;
        vec4state operator^(long num) const;
        vec4state operator~() const;

        // Equality operators
        vec4state operator==(const vec4state& other) const;
        vec4state operator!=(const vec4state& other) const;
        vec4state operator==(long num) const;
        vec4state operator!=(long num) const;
        //vec4state operator===(const vec4state& other) const;
        // vec4state operator!==(const vec4state& other) const;

        // Shift operators
        // If other contains x or z, returns a vector that is all x's.
        vec4state operator<<(const vec4state& other);
        // If other contains x or z, returns a vector that is all x's.
        vec4state operator>>(const vec4state& other);
        vec4state operator<<(const long num);
        vec4state operator>>(const long num);

        // Slice operators
        // TODO: How to write the declaration of slice and set []???
        vec4state operator[](const vec4state& index);

        // Logical operators
        vec4state operator&&(const vec4state& other) const;
        vec4state operator&&(long num) const;
        vec4state operator||(const vec4state& other) const;
        vec4state operator||(long num) const;
        vec4state operator!() const;

        // Relational operators
        vec4state operator<(const vec4state& other) const;
        vec4state operator<(long num) const;
        vec4state operator>(const vec4state& other) const;
        vec4state operator>(long num) const;
        vec4state operator<=(const vec4state& other) const;
        vec4state operator<=(long num) const;
        vec4state operator>=(const vec4state& other) const;
        vec4state operator>=(long num) const;

        // Arithmetic operators
        vec4state operator+(const vec4state& other) const;
        vec4state operator+(long num) const;
        vec4state operator-(const vec4state& other) const;
        vec4state operator-(long num) const;
        vec4state operator*(const vec4state& other) const;
        vec4state operator*(long num) const;
        vec4state operator/(const vec4state& other) const;
        vec4state operator/(long num) const;
        vec4state operator%(const vec4state& other) const;
        vec4state operator%(long num) const;
        vec4state operator-() const;
        // vec4state operator**(const vec4state& other) const;
        // vec4state operator**(long num) const;
        
        // Casting operators
        // TODO: How to write the declaration of casting???
};


#endif // VEC4STATE_H