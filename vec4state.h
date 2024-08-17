#ifndef VEC4STATE_H
#define VEC4STATE_H

#include <string>
#include <stdint.h>
#include "vpi.h"
#include <type_traits>

using namespace std;

// Custom type trait to check if T is either an integral type, string or const char*.
template <typename T>
struct is_valid_type_for_vec4state : integral_constant<bool, is_integral<T>::value || is_same<T, std::string>::value || is_same<T, const char*>::value> {};

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
        vec4state(unsigned int num);
        vec4state(int num);
        // Initializes a 64-bit vector to the value of num
        vec4state(unsigned long long num);
        vec4state(long long num);
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

        template <typename T>
        vec4state& operator=(T num) {
            static_assert(is_valid_type_for_vec4state<T>::value, "num must be an integral type or string");
            *this = vec4state(num);
            return *this;
        }

        // Returns true if the vector is 1, false if the vector is 0 or unknown.
        explicit operator bool() const;

        // Bitwise operators
        vec4state operator&(const vec4state& other) const;

        template <typename T>
        vec4state operator&(T num) const {
            static_assert(is_valid_type_for_vec4state<T>::value, "num must be an integral type or string");
            return *this & vec4state(num);
        }

        vec4state operator|(const vec4state& other) const;

        template <typename T>
        vec4state operator|(T num) const {
            static_assert(is_valid_type_for_vec4state<T>::value, "num must be an integral type or string");
            return *this | vec4state(num);
        }

        vec4state operator^(const vec4state& other) const;

        template <typename T>
        vec4state operator^(T num) const {
            static_assert(is_valid_type_for_vec4state<T>::value, "num must be an integral type or string");
            return *this ^ vec4state(num);
        }

        vec4state operator~() const;

        // Equality operators
        vec4state operator==(const vec4state& other) const;

        template <typename T>
        vec4state operator==(T num) const {
            static_assert(is_valid_type_for_vec4state<T>::value, "num must be an integral type or string");
            return *this == vec4state(num);
        }

        vec4state operator!=(const vec4state& other) const;

        template <typename T>
        vec4state operator!=(T num) const {
            static_assert(is_valid_type_for_vec4state<T>::value, "num must be an integral type or string");
            return *this != vec4state(num);
        }

        // Checks if the vector is equal to other, including x's and z's. Returns a vector of 1's if true and 0's otherwise.
        vec4state caseEquality(const vec4state& other) const;

        template <typename T>
        vec4state caseEquality(T num) const {
            static_assert(is_valid_type_for_vec4state<T>::value, "num must be an integral type or string");
            return caseEquality(vec4state(num));
        }

        // Checks if the vector is not equal to other, including x's and z's. Returns a vector of 1's if true and 0's otherwise.
        vec4state caseInequality(const vec4state& other) const;

        template <typename T>
        vec4state caseInequality(T num) const {
            static_assert(is_valid_type_for_vec4state<T>::value, "num must be an integral type or string");
            return caseInequality(vec4state(num));
        }

        // Shift operators
        // If other contains x or z, returns a vector that is all x's.
        vec4state operator<<(const vec4state& other);

        vec4state operator<<(const long long num);

        template <typename T>
        vec4state operator<<(T num) {
            static_assert(is_integral<T>::value, "num must be an integral type");
            return *this << long long(num);
        }

        // If other contains x or z, returns a vector that is all x's.
        vec4state operator>>(const vec4state& other);

        vec4state operator>>(const long long num);

        template <typename T>
        vec4state operator>>(T num) {
            static_assert(is_integral<T>::value, "num must be an integral type");
            return *this >> long long(num);
        }

        // Slice operators
        vec4state getBitSelect(const vec4state& index) const;

        template <typename T>
        vec4state getBitSelect(T index) const {
            static_assert(is_valid_type_for_vec4state<T>::value, "index must be an integral type or string");
            return getBitSelect(vec4state(index));
        }

        void setBitSelect(const vec4state& index, const vec4state& newValue);

        template <typename T>
        void setBitSelect(T index, const vec4state& newValue) {
            static_assert(is_valid_type_for_vec4state<T>::value, "index must be an integral type or string");
            setBitSelect(vec4state(index), newValue);
        }

        template <typename T>
        void setBitSelect(const vec4state& index, T newValue) {
            static_assert(is_valid_type_for_vec4state<T>::value, "newValue must be an integral type or string");
            setBitSelect(index, vec4state(newValue));
        }

        template <typename T1, typename T2>
        void setBitSelect(T1 index, T2 newValue) {
            static_assert(is_valid_type_for_vec4state<T1>::value, "index must be an integral type or string");
            static_assert(is_valid_type_for_vec4state<T2>::value, "newValue must be an integral type or string");
            setBitSelect(vec4state(index), vec4state(newValue));
        }

        // Returns a vector that is a slice of the original vector from start to end.
        vec4state getPartSelect(long long end, long long start) const;

        template <typename T1, typename T2>
        vec4state getPartSelect(T1 end, T2 start) const {
            static_assert(is_integral<T1>::value, "end must be an integral type");
            static_assert(is_integral<T2>::value, "start must be an integral type");
            return getPartSelect(long long(end), long long(start));
        }

        // Sets the slice of the original vector from start to end to the value of other.
        void setPartSelect(long long end, long long start, const vec4state& other);

        template <typename T1, typename T2>
        void setPartSelect(T1 end, T2 start, const vec4state& other) {
            static_assert(is_integral<T1>::value, "end must be an integral type");
            static_assert(is_integral<T2>::value, "start must be an integral type");
            setPartSelect(long long(end), long long(start), other);
        }

        template <typename T1, typename T2, typename T3>
        void setPartSelect(T1 end, T2 start, T3 num) {
            static_assert(is_integral<T1>::value, "end must be an integral type");
            static_assert(is_integral<T2>::value, "start must be an integral type");
            static_assert(is_valid_type_for_vec4state<T3>::value, "num must be an integral type or string");
            setPartSelect(long long(end), long long(start), vec4state(num));
        }

        // Logical operators
        vec4state operator&&(const vec4state& other) const;

        template <typename T>
        vec4state operator&&(T num) const {
            static_assert(is_valid_type_for_vec4state<T>::value, "num must be an integral type or string");
            return *this && vec4state(num);
        }

        vec4state operator||(const vec4state& other) const;

        template <typename T>
        vec4state operator||(T num) const {
            static_assert(is_valid_type_for_vec4state<T>::value, "num must be an integral type or string");
            return *this || vec4state(num);
        }

        vec4state operator!() const;

        // Relational operators
        vec4state operator<(const vec4state& other) const;

        template <typename T>
        vec4state operator<(T num) const {
            static_assert(is_valid_type_for_vec4state<T>::value, "num must be an integral type or string");
            return *this < vec4state(num);
        }

        vec4state operator>(const vec4state& other) const;

        template <typename T>
        vec4state operator>(T num) const {
            static_assert(is_valid_type_for_vec4state<T>::value, "num must be an integral type or string");
            return *this > vec4state(num);
        }

        vec4state operator<=(const vec4state& other) const;

        template <typename T>
        vec4state operator<=(T num) const {
            static_assert(is_valid_type_for_vec4state<T>::value, "num must be an integral type or string");
            return *this <= vec4state(num);
        }

        vec4state operator>=(const vec4state& other) const;

        template <typename T>
        vec4state operator>=(T num) const {
            static_assert(is_valid_type_for_vec4state<T>::value, "num must be an integral type or string");
            return *this >= vec4state(num);
        }

        // Arithmetic operators
        vec4state operator+(const vec4state& other) const;

        template <typename T>
        vec4state operator+(T num) const {
            static_assert(is_valid_type_for_vec4state<T>::value, "num must be an integral type or string");
            return *this + vec4state(num);
        }

        vec4state operator-(const vec4state& other) const;

        template <typename T>
        vec4state operator-(T num) const {
            static_assert(is_valid_type_for_vec4state<T>::value, "num must be an integral type or string");
            return *this - vec4state(num);
        }

        vec4state operator*(const vec4state& other) const;

        template <typename T>
        vec4state operator*(T num) const {
            static_assert(is_valid_type_for_vec4state<T>::value, "num must be an integral type or string");
            return *this * vec4state(num);
        }

        vec4state operator/(const vec4state& other) const;

        template <typename T>
        vec4state operator/(T num) const {
            static_assert(is_valid_type_for_vec4state<T>::value, "num must be an integral type or string");
            return *this / vec4state(num);
        }

        vec4state operator%(const vec4state& other) const;

        template <typename T>
        vec4state operator%(T num) const {
            static_assert(is_valid_type_for_vec4state<T>::value, "num must be an integral type or string");
            return *this % vec4state(num);
        }

        vec4state operator-() const;

        // Calculates the value of the vector to the power of other.
        vec4state power(const vec4state& other) const;

        // Calculates the value of the vector to the power of num.
        template <typename T>
        vec4state power(T num) const {
            static_assert(is_valid_type_for_vec4state<T>::value, "num must be an integral type or string");
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