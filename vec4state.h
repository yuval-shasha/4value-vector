#ifndef VEC4STATE_H
#define VEC4STATE_H

#include <string>
#include <stdint.h>
#include "vpi.h"
#include <type_traits>

#define MASK_32 0xFFFFFFFF
#define BITS_IN_CELL 32
#define BITS_IN_BYTE 8

using namespace std;

// Custom type trait to check if T is either an integral type, string or const char*.
template <typename T>
struct is_valid_type_for_vec4state : integral_constant<bool, is_integral<T>::value || is_same<T, std::string>::value || is_same<T, const char*>::value> {};

enum BitValue {
    ZERO = '0',
    ONE = '1',
    X = 'x',
    Z = 'z'
};

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

        // Initializes a vector with numBits bits (greater than 0) that contains bit repeated numBits times.
        vec4state(BitValue bit, long long numBits);
        // 0-extending the vector to newNumBits.
        void incNumBits(long long newNumBits);
        // Truncates the vector to newNumBits.
        void decNumBits(long long newNumBits);
        // Returns the numerical value that the vector holds. This method can be used only if the vector has up to 2 VPI elements and has no unknown bits.
        long long extractNumberFromVector() const;
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
        // Initializes a 1-bit vector, initialized to x.
        vec4state();
        // Initializes a vector that holds num with number of bits = the size of T.
        template<typename T, typename enable_if<is_integral<T>::value, bool>::type = true>
        vec4state(T num) {
            numBits = sizeof(T) * BITS_IN_BYTE;
            vectorSize = (numBits + BITS_IN_CELL - 1) / BITS_IN_CELL;
            unknown = false;
            vector = new VPI[vectorSize];
            int mask = MASK_32;
            for (long long i = 0; i < vectorSize; i++) {
                vector[i].setAval(uint32_t(num & mask));
                vector[i].setBval(0);
                num >>= BITS_IN_CELL;
            }
        }
        // Initializes a vector of size str.length() with the value of str. str must contain only 0, 1, x, or z.
        vec4state(string str);
        // Copy c'tor
        vec4state(const vec4state& other);
        // Move c'tor
        vec4state(vec4state&& other) noexcept;

        // D'tor
        ~vec4state();

        // Assignment operators
        vec4state& operator=(const vec4state& other);

        template<typename T, typename enable_if<is_valid_type_for_vec4state<T>::value, bool>::type = true>
        vec4state& operator=(T num) {
            *this = vec4state(num);
            return *this;
        }

        // Returns true if the vector is 1, false if the vector is 0 or unknown.
        explicit operator bool() const;

        // Bitwise operators
        vec4state operator&(const vec4state& other) const;

        template<typename T, typename enable_if<is_valid_type_for_vec4state<T>::value, bool>::type = true>
        vec4state operator&(T num) const {
            return *this & vec4state(num);
        }

        vec4state operator|(const vec4state& other) const;

        template<typename T, typename enable_if<is_valid_type_for_vec4state<T>::value, bool>::type = true>
        vec4state operator|(T num) const {
            return *this | vec4state(num);
        }

        vec4state operator^(const vec4state& other) const;

        template<typename T, typename enable_if<is_valid_type_for_vec4state<T>::value, bool>::type = true>
        vec4state operator^(T num) const {
            return *this ^ vec4state(num);
        }

        vec4state operator~() const;

        // Equality operators
        vec4state operator==(const vec4state& other) const;

        template<typename T, typename enable_if<is_valid_type_for_vec4state<T>::value, bool>::type = true>
        vec4state operator==(T num) const {
            return *this == vec4state(num);
        }

        vec4state operator!=(const vec4state& other) const;

        template<typename T, typename enable_if<is_valid_type_for_vec4state<T>::value, bool>::type = true>
        vec4state operator!=(T num) const {
            return *this != vec4state(num);
        }

        // Checks if the vector is equal to other, including x's and z's. Returns a vector of 1's if true and 0's otherwise.
        vec4state caseEquality(const vec4state& other) const;

        template<typename T, typename enable_if<is_valid_type_for_vec4state<T>::value, bool>::type = true>
        vec4state caseEquality(T num) const {
            return caseEquality(vec4state(num));
        }

        // Checks if the vector is not equal to other, including x's and z's. Returns a vector of 1's if true and 0's otherwise.
        vec4state caseInequality(const vec4state& other) const;

        template<typename T, typename enable_if<is_valid_type_for_vec4state<T>::value, bool>::type = true>
        vec4state caseInequality(T num) const {
            return caseInequality(vec4state(num));
        }

        // Shift operators
        // If other contains x or z, returns a vector that is all x's.
        vec4state operator<<(const vec4state& other);
        vec4state operator<<(const long long num);

        // If other contains x or z, returns a vector that is all x's.
        vec4state operator>>(const vec4state& other);
        vec4state operator>>(const long long num);

        // Slice operators
        vec4state getBitSelect(const vec4state& index) const;

        template<typename T, typename enable_if<is_valid_type_for_vec4state<T>::value, bool>::type = true>
        vec4state getBitSelect(T index) const {
            return getBitSelect(vec4state(index));
        }

        void setBitSelect(const vec4state& index, const vec4state& newValue);

        template<typename T, typename enable_if<is_valid_type_for_vec4state<T>::value, bool>::type = true>
        void setBitSelect(T index, const vec4state& newValue) {
            setBitSelect(vec4state(index), newValue);
        }

        template<typename T, typename enable_if<is_valid_type_for_vec4state<T>::value, bool>::type = true>
        void setBitSelect(const vec4state& index, T newValue) {
            setBitSelect(index, vec4state(newValue));
        }

        template<typename T1, typename enable_if<is_valid_type_for_vec4state<T1>::value, bool>::type = true, typename T2, typename enable_if<is_valid_type_for_vec4state<T2>::value, bool>::type = true>
        void setBitSelect(T1 index, T2 newValue) {
            setBitSelect(vec4state(index), vec4state(newValue));
        }

        // Returns a vector that is a slice of the original vector from start to end.
        vec4state getPartSelect(long long end, long long start) const;

        // Sets the slice of the original vector from start to end to the value of other.
        void setPartSelect(long long end, long long start, const vec4state& other);

        template<typename T, typename enable_if<is_valid_type_for_vec4state<T>::value, bool>::type = true>
        void setPartSelect(long long end, long long start, T num) {
            setPartSelect(end, start, vec4state(num));
        }

        // Logical operators
        vec4state operator&&(const vec4state& other) const;

        template<typename T, typename enable_if<is_valid_type_for_vec4state<T>::value, bool>::type = true>
        vec4state operator&&(T num) const {
            return *this && vec4state(num);
        }

        vec4state operator||(const vec4state& other) const;

        template<typename T, typename enable_if<is_valid_type_for_vec4state<T>::value, bool>::type = true>
        vec4state operator||(T num) const {
            return *this || vec4state(num);
        }

        vec4state operator!() const;

        // Relational operators
        vec4state operator<(const vec4state& other) const;

        template<typename T, typename enable_if<is_valid_type_for_vec4state<T>::value, bool>::type = true>
        vec4state operator<(T num) const {
            return *this < vec4state(num);
        }

        vec4state operator>(const vec4state& other) const;

        template<typename T, typename enable_if<is_valid_type_for_vec4state<T>::value, bool>::type = true>
        vec4state operator>(T num) const {
            return *this > vec4state(num);
        }

        vec4state operator<=(const vec4state& other) const;

        template<typename T, typename enable_if<is_valid_type_for_vec4state<T>::value, bool>::type = true>
        vec4state operator<=(T num) const {
            return *this <= vec4state(num);
        }

        vec4state operator>=(const vec4state& other) const;

        template<typename T, typename enable_if<is_valid_type_for_vec4state<T>::value, bool>::type = true>
        vec4state operator>=(T num) const {
            return *this >= vec4state(num);
        }

        // Arithmetic operators
        vec4state operator+(const vec4state& other) const;

        template<typename T, typename enable_if<is_valid_type_for_vec4state<T>::value, bool>::type = true>
        vec4state operator+(T num) const {
            return *this + vec4state(num);
        }

        vec4state operator-(const vec4state& other) const;

        template<typename T, typename enable_if<is_valid_type_for_vec4state<T>::value, bool>::type = true>
        vec4state operator-(T num) const {
            return *this - vec4state(num);
        }

        vec4state operator*(const vec4state& other) const;

        template<typename T, typename enable_if<is_valid_type_for_vec4state<T>::value, bool>::type = true>
        vec4state operator*(T num) const {
            return *this * vec4state(num);
        }

        vec4state operator/(const vec4state& other) const;

        template<typename T, typename enable_if<is_valid_type_for_vec4state<T>::value, bool>::type = true>
        vec4state operator/(T num) const {
            return *this / vec4state(num);
        }

        vec4state operator%(const vec4state& other) const;

        template<typename T, typename enable_if<is_valid_type_for_vec4state<T>::value, bool>::type = true>
        vec4state operator%(T num) const {
            return *this % vec4state(num);
        }

        vec4state operator-() const;

        // Calculates the value of the vector to the power of other.
        vec4state power(const vec4state& other) const;

        // Calculates the value of the vector to the power of num.
        template<typename T, typename enable_if<is_valid_type_for_vec4state<T>::value, bool>::type = true>
        vec4state power(T num) const {
            return power(vec4state(num));
        }

        // Casting operator
        // Replaces all the unknown values of the vector with 0's.
        void convertTo2State();
        
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