/**
 * @file vec4state.h
 * @brief Declaration of the vec4state class.
 * 
 * This file contains the declaration of the vec4state class, which is a versatile C++ library that enables efficient representation and arithmetic operations on 4-value bit vectors, emulating the functionalities commonly found in hardware description languages like SystemVerilog.
 * 
 * @author Mia Ekheizer, Yuval Shasha
 * @date 2024-08-24
 */

#ifndef VEC4STATE_H
#define VEC4STATE_H

#include <string>
#include <stdint.h>
#include "vpi.h"
#include <type_traits>
#include <memory>
#include "vec4stateException.h"
#include "math.h"
#include <iostream>

/**
 * @brief A mask for all the bits in a VPI.
 */
#define MASK_32 0xFFFFFFFF

/**
 * @brief The number of bits in a VPI.
 */
#define BITS_IN_VPI 32

/**
 * @brief The number of bits in a byte.
 */
#define BITS_IN_BYTE 8

using namespace std;

/**
 * @brief BitValue represents the four possible values of a bit in a 4-state vector.
 * 
 * The four possible values are: 0, 1, x and z.
 * 
 */
enum BitValue {
    ZERO = '0',
    ONE = '1',
    X = 'x',
    Z = 'z'
};

/**
 * @brief Type trait to check if a type is a valid type for a vec4state constructor.
 * 
 * This type trait checks if a type is either an integral type, a string or a const char*. If the type is valid, the relevant methods of vec4state can be used with this type.
 * 
 * @tparam The type to check.
 */
template <typename T>
struct is_valid_type_for_vec4state : integral_constant<bool, is_integral<T>::value || is_same<T, std::string>::value || is_same<T, const char*>::value || is_same<T, BitValue>::value> {};

/**
 * @class vec4state
 * @brief This class represents a vector of 4-state values.
 * 
 * This class supports four primary values for each bit in the vector: 0, 1, x and z. The vector can hold up to 2^64 - 1 bits, and it's core functionality is bitwise and arithmetic operations, which are essential for digital logic and circuit design. Those operations include: bitwise, equality, shift, slice, logical, relational, arithmetic and casting 4-state to 2-state. The values are stored as unsigned 32-bit integers, where the 32 least significant bits are stored in the first element of the vector.
 */
class vec4state {
public:
    /**
     * @brief Default constructor for vec4state.
     * 
     * Initializes a 1-bit vector, initialized to x.
     */
    vec4state();

    /**
     * @brief Integral constructor for vec4state.
     * 
     * Initializes a vector that holds num with number of bits = the size of T.
     * 
     * @tparam The type of num, must be an integral type.
     * @param num The value to initialize the vector with.
     */
    template<typename T, typename enable_if<is_integral<T>::value, bool>::type = true>
    vec4state(T num) {
        numBits = sizeof(T) * BITS_IN_BYTE;
        vectorSize = (numBits + BITS_IN_VPI - 1) / BITS_IN_VPI;
        unknown = false;
        vector = shared_ptr<VPI[]>(new VPI[vectorSize]);
        int mask = MASK_32;
        for (long long i = 0; i < vectorSize; i++) {
            vector[i].setAval(uint32_t(num & mask));
            vector[i].setBval(0);
            num >>= BITS_IN_VPI;
        }
    }
    
    /**
     * @brief String constructor for vec4state.
     * 
     * Initializes a vector of size str.length() with the values represented by str. The constructor iterates over the string's characters, translates the BitValues to aval and bval of a VPI, and fills the VPI of vector from last VPI to first VPI with these BitValues. If str contains a character that is not a BitValue, the vector is initialized to x and vec4stateExceptionInvalidInput is thrown.
     * 
     * @param str The value to initialize the vector with, must be a string that holds only BitValues.
     */
    vec4state(string str);

    /**
     * @brief Bit constructor for vec4state.
     * 
     * Initializes a vector that holds bit of size 1. If bit is a character that is not a BitValue, the vector is initialized to x and vec4stateExceptionInvalidInput is thrown.
     * 
     * @param bit The bit to initialize the vector with, must be a BitValue.
     */
    vec4state(char bit);

    /**
     * @brief Copy constructor for vec4state.
     * 
     * Initializes a new vector and copies the values from the other vector to the new vector.
     * 
     * @param other The vector to copy from.
     */
    vec4state(const vec4state& other);

    /**
     * @brief Move constructor for vec4state.
     * 
     * Initializes a new vector with the same values as other, and transfers the ownership of the vector to the new object. The other vector is left in a valid but unspecified state.
     * 
     * @param other The vector to move from.
     */
    vec4state(vec4state&& other) noexcept;

    /**
     * @brief Assignment operator for vec4state.
     * 
     * Assigns the values of other to this vector. If the other vector is shorter than this vector, zeroes down the bits that are out of range.
     * 
     * @param other The vector to assign from.
     * @return A reference to this vector.
     */
    vec4state& operator=(const vec4state& other);

    /**
     * @brief Assignment operator for vec4state.
     * 
     * Creates a vec4state that holds num and assigns it to this vector.
     * 
     * @tparam The type of num, must be an integral type or a string.
     * @param num The value to assign from.
     * @return A reference to this vector.
     */
    template<typename T, typename enable_if<is_valid_type_for_vec4state<T>::value, bool>::type = true>
    vec4state& operator=(T num) {
        *this = vec4state(num);
        return *this;
    }

    /**
     * @brief Bool conversion operator for vec4state.
     * 
     * Checks if the vector holds at least one 1 bit. This is done by iterating over the vector's VPIs, and checking if at least one of the VPIs has a bit set to 1.
     * 
     * @return true if the vector holds at least one 1 bit.
     * @return false if the vector holds only 0 or unknown bits.
     */
    explicit operator bool() const;

    /**
     * @brief Bitwise AND operator for vec4state.
     * 
     * Calculates the bitwise AND of a bit in this vector with the corresponding bit in the other vector, resulting in one bit for each bit of the vectors. If at least one of the bits is 0, the result bit is 0. If both bits are 1, the result bit is 1. If at least one of the bits is unknown, the result bit is x. If the vectors are of unequal bit lengths, the smaller vector is zero-extended to the size of the larger vector. The method calculates the bitwise NOT of each vector, then calculates the bitwise OR of the NOT vectors, and finally calculates the bitwise NOT of the result vector.
     * 
     * @param other The vector to perform the bitwise AND operation with.
     * @return A new vector that holds the result of the bitwise AND operation.
     */
    vec4state operator&(const vec4state& other) const;

    /**
     * @brief Bitwise AND operator for vec4state.
     * 
     * Creates a vec4state that holds the value of num, then calculates the bitwise AND of a bit in this vector with the corresponding bit in num, resulting in one bit for each bit of the vectors. If at least one of the bits is 0, the result bit is 0. If both bits are 1, the result bit is 1. If at least one of the bits is unknown, the result bit is x. If the vectors are of unequal bit lengths, the smaller vector is zero-extended to the size of the larger vector. The method calculates the bitwise NOT of each vector, then calculates the bitwise OR of the NOT vectors, and finally calculates the bitwise NOT of the result vector.
     * 
     * @tparam The type of num, must be an integral type or a string.
     * @param num The value to perform the bitwise AND operation with.
     * @return A new vector that holds the result of the bitwise AND operation.
     */
    template<typename T, typename enable_if<is_valid_type_for_vec4state<T>::value, bool>::type = true>
    vec4state operator&(T num) const {
        return *this & vec4state(num);
    }

    /**
     * @brief Bitwise OR operator for vec4state.
     * 
     * Calculates the bitwise OR of a bit in this vector with the corresponding bit in the other vector, resulting in one bit for each bit of the vectors. If at least one of the bits is 1, the result bit is 1. If both bits are 0, the result bit is 0. If at least one of the bits is unknown, the result bit is x. If the vectors are of unequal bit lengths, the smaller vector is zero-extended to the size of the larger vector.
     * 
     * @param other The vector to perform the bitwise OR operation with.
     * @return A new vector that holds the result of the bitwise OR operation.
     */
    vec4state operator|(const vec4state& other) const;

    /**
     * @brief Bitwise OR operator for vec4state.
     * 
     * Creates a vec4state that holds the value of num, then calculates the bitwise OR of a bit in this vector with the corresponding bit in num, resulting in one bit for each bit of the vectors. If at least one of the bits is 1, the result bit is 1. If both bits are 0, the result bit is 0. If at least one of the bits is unknown, the result bit is x. If the vectors are of unequal bit lengths, the smaller vector is zero-extended to the size of the larger vector.
     * 
     * @tparam The type of num, must be an integral type or a string.
     * @param num The value to perform the bitwise OR operation with.
     * @return A new vector that holds the result of the bitwise OR operation.
     */
    template<typename T, typename enable_if<is_valid_type_for_vec4state<T>::value, bool>::type = true>
    vec4state operator|(T num) const {
        return *this | vec4state(num);
    }

    /**
     * @brief Bitwise XOR operator for vec4state.
     * 
     * Calculates the bitwise XOR of a bit in this vector with the corresponding bit in the other vector, resulting in one bit for each bit of the vectors. If at least one of the bits is unknown, the result bit is x. If both bits are equal, the result bit is 0. If the two bits are not equal, the result bit is 1. If the vectors are of unequal bit lengths, the smaller vector is zero-extended to the size of the larger vector. The method calculates the bitwise AND between each vector and the bitwise NOT of the other vector, then calculates the bitwise OR of the two results.
     * 
     * @param other The vector to perform the bitwise XOR operation with.
     * @return A new vector that holds the result of the bitwise XOR operation.
     */
    vec4state operator^(const vec4state& other) const;

    /**
     * @brief Bitwise XOR operator for vec4state.
     * 
     * Creates a vec4state that holds the value of num, then calculates the bitwise XOR of a bit in this vector with the corresponding bit in num, resulting in one bit for each bit of the vectors. If at least one of the bits is unknown, the result bit is x. If both bits are equal, the result bit is 0. If the two bits are not equal, the result bit is 1. If the vectors are of unequal bit lengths, the smaller vector is zero-extended to the size of the larger vector. The method calculates the bitwise AND between each vector and the bitwise NOT of the other vector, then calculates the bitwise OR of the two results.
     * 
     * @tparam The type of num, must be an integral type or a string.
     * @param num The value to perform the bitwise XOR operation with.
     * @return A new vector that holds the result of the bitwise XOR operation.
     */
    template<typename T, typename enable_if<is_valid_type_for_vec4state<T>::value, bool>::type = true>
    vec4state operator^(T num) const {
        return *this ^ vec4state(num);
    }

    /**
     * @brief Bitwise NOT operator for vec4state.
     * 
     * Calculates the bitwise NOT of each bit in the vector, resulting in one bit for each bit of the vector. If the bit is 0, the result bit is 1. If the bit is 1, the result bit is 0. If the bit is unknown, the result bit is x. The method iterates over the vector's VPIs, and sets each bit in the aval to 1 only if the corresponding bit in the original vector is 0 (in any other case, the bit is set to 0, which means it can be either 0 or x). The bval stays the same (which means every known bit stays known and every unknown bit can be only x). Then the method zeroes down the bits that are out of range (because they have been set to 1 in the bitwise NOT).
     * 
     * @return A new vector that holds the result of the bitwise NOT operation.
     */
    vec4state operator~() const;

    /**
     * @brief Logical equality operator for vec4state.
     * 
     * Compares this vector to other vector bit for bit. This is done by calculating the bitwise XOR of the two vectors, and then checking if the result vector is zero. If the result vector holds at least one 1 bit, the vectors are not equal. If the result vector has unknown bits, the comparison is ambiguous and the method returns x. In any other case, if the result vector is only 0's, the vectors are equal.
     * 
     * @param other The vector to compare to.
     * @return 1'b0 if the comparison fails.
     * @return 1'b1 if the comparison succeeds.
     * @return 1'bx if the comparison is ambiguous (due to unknown bits in one of the vectors).
     */
    vec4state operator==(const vec4state& other) const;

    /**
     * @brief Logical equality operator for vec4state.
     * 
     * Creates a vec4state that holds the value of num, then compares this vector to num bit for bit. This is done by calculating the bitwise XOR of the two vectors, and then checking if the result vector is zero. If the result vector holds at least one 1 bit, the vectors are not equal. If the result vector has unknown bits, the comparison is ambiguous and the method returns x. In any other case, if the result vector is only 0's, the vectors are equal.
     * 
     * @tparam The type of num, must be an integral type or a string.
     * @param num The value to compare to.
     * @return 1'b0 if the comparison fails.
     * @return 1'b1 if the comparison succeeds.
     * @return 1'bx if the comparison is ambiguous (due to unknown bits in one of the vectors).
     */
    template<typename T, typename enable_if<is_valid_type_for_vec4state<T>::value, bool>::type = true>
    vec4state operator==(T num) const {
        return *this == vec4state(num);
    }

    /**
     * @brief Logical inequality operator for vec4state.
     * 
     * Compares this vector to other vector bit for bit. This is done by checking if the vectors are equal using the logical equality operator, and then negating the result.
     * 
     * @param other The vector to compare to.
     * @return 1'b1 if the comparison fails.
     * @return 1'b0 if the comparison succeeds.
     * @return 1'bx if the comparison is ambiguous (due to unknown bits in one of the vectors).
     */
    vec4state operator!=(const vec4state& other) const;

    /**
     * @brief Logical inequality operator for vec4state.
     * 
     * Creates a vec4state that holds the value of num, then compares this vector to num bit for bit. This is done by checking if the vectors are equal using the logical equality operator, and then negating the result.
     * 
     * @tparam The type of num, must be an integral type or a string.
     * @param num The value to compare to.
     * @return 1'b1 if the comparison fails.
     * @return 1'b0 if the comparison succeeds.
     * @return 1'bx if the comparison is ambiguous (due to unknown bits in one of the vectors).
     */
    template<typename T, typename enable_if<is_valid_type_for_vec4state<T>::value, bool>::type = true>
    vec4state operator!=(T num) const {
        return *this != vec4state(num);
    }

    /**
     * @brief Case equality operator for vec4state.
     * 
     * Compares this vector to other vector bit for bit, where the unknown bits are included in the comparison and shall match for the result to be considered equal. If the vectors are of different lengths, the shorter vector is zero-extended to the size of the longer vector. The method iterates over the vectors' VPIs, and checks if the aval and bval of each VPI are equal. If at least one of the VPIs is not equal, the vectors are not equal. If all of the VPIs are equal, the vectors are equal.
     * 
     * @param other The vector to compare to.
     * @return 1'b0 if the comparison fails.
     * @return 1'b1 if the comparison succeeds.
     */
    vec4state caseEquality(const vec4state& other) const;

    /**
     * @brief Case equality operator for vec4state.
     * 
     * Creates a vec4state that holds the value of num, then compares this vector to other vector bit for bit, where the unknown bits are included in the comparison and shall match for the result to be considered equal. If the vectors are of different lengths, the shorter vector is zero-extended to the size of the longer vector. The method iterates over the vectors' VPIs, and checks if the aval and bval of each VPI are equal. If at least one of the VPIs is not equal, the vectors are not equal. If all of the VPIs are equal, the vectors are equal.
     * 
     * @tparam The type of num, must be an integral type or a string.
     * @param num The value to compare to.
     * @return 1'b0 if the comparison fails.
     * @return 1'b1 if the comparison succeeds.
     */
    template<typename T, typename enable_if<is_valid_type_for_vec4state<T>::value, bool>::type = true>
    vec4state caseEquality(T num) const {
        return caseEquality(vec4state(num));
    }

    /**
     * @brief Case inequality operator for vec4state.
     * 
     * Compares this vector to other vector bit for bit, where the unknown bits are included in the comparison and shall not match for the result to be considered unequal. If the vectors are of different lengths, the shorter vector is zero-extended to the size of the longer vector. The method calculates the case equality of the vectors and then negates the result.
     * 
     * @param other The vector to compare to.
     * @return 1'b1 if the comparison fails.
     * @return 1'b0 if the comparison succeeds.
     */
    vec4state caseInequality(const vec4state& other) const;

    /**
     * @brief Case inequality operator for vec4state.
     * 
     * Creates a vec4state that holds the value of num, then compares this vector to other vector bit for bit, where the unknown bits are included in the comparison and shall not match for the result to be considered unequal. If the vectors are of different lengths, the shorter vector is zero-extended to the size of the longer vector. The method calculates the case equality of the vectors and then negates the result.
     * 
     * @tparam The type of num, must be an integral type or a string.
     * @param num The value to compare to.
     * @return 1'b1 if the comparison fails.
     * @return 1'b0 if the comparison succeeds.
     */
    template<typename T, typename enable_if<is_valid_type_for_vec4state<T>::value, bool>::type = true>
    vec4state caseInequality(T num) const {
        return caseInequality(vec4state(num));
    }

    /**
     * @brief Logical shift left operator for vec4state.
     * 
     * Extracts the value stored in other vector, then shifts this vector to the left by the number of bit positions given by other vector. The vacated bit positions are filled with zeros. If other vector holds a value that cannot be an index of a bit position in this vector, the result is a vector of 0's.
     * 
     * @param other The vector that holds the number of bit positions to shift by.
     * @return A new vector that holds the result of the logical shift left operation. If other vector holds unknown bits, then the result is only x's.
     */
    vec4state operator<<(const vec4state& other);

    /**
     * @brief Logical shift left operator for vec4state.
     * 
     * Shifts this vector to the left by the number of bit positions given by num. The vacated bit positions are filled with zeros.
     * 
     * @param num The number of bit positions to shift by.
     * @return A new vector that holds the result of the logical shift left operation.
     */
    vec4state operator<<(const long long num);

    /**
     * @brief Logical shift right operator for vec4state.
     * 
     * Converts other vector to a long long number, then shifts this vector to the right by the number of bit positions given by other vector. The vacated bit positions are filled with zeros. If other vector holds a value that cannot be an index of a bit position in this vector, the result is a vector of 0's.
     * 
     * @param other The vector that holds the number of bit positions to shift by.
     * @return A new vector that holds the result of the logical shift right operation. If other vector holds unknown bits, then the result is only x's.
     */
    vec4state operator>>(const vec4state& other);

    /**
     * @brief Logical shift left operator for vec4state.
     * 
     * Shifts this vector to the right by the number of bit positions given by num. The vacated bit positions are filled with zeros.
     * 
     * @param num The number of bit positions to shift by.
     * @return A new vector that holds the result of the logical shift right operation.
     */
    vec4state operator>>(const long long num);

    /**
     * @brief Get bit select operator for vec4state.
     * 
     * Extracts the bit stored in this vector at the given index by extracting the value stored in index and by using the getPartSelect method by setting the start and end indices to index.
     * 
     * @param index The index of the bit to extract.
     * @return The value of the bit at index index in this vector. If the index holds unknown values or is out of range, the result is a vector of x's.
     */
    vec4state getBitSelect(const vec4state& index) const;

    /**
     * @brief Get bit select operator for vec4state.
     * 
     * Creates a vec4state that holds the value of index, then extracts the bit stored in this vector at the given index. Uses the getPartSelect method by setting the start and end indices to index.
     * 
     * @tparam The type of index, must be an integral type or a string.
     * @param index The index of the bit to extract.
     * @return The value of the bit at index index in this vector. If the index holds unknown values or is out of range, the result is a vector of x's.
     */
    template<typename T, typename enable_if<is_valid_type_for_vec4state<T>::value, bool>::type = true>
    vec4state getBitSelect(T index) const {
        return getBitSelect(vec4state(index));
    }

    /**
     * @brief Set bit select operator for vec4state.
     * 
     * Sets the bit stored in this vector at the given index to the value stored in newValue by extracting the value stored in index and by using the setPartSelect method by setting the start and end indices to index.
     * 
     * @param index The index of the bit to set.
     * @param newValue The value to set the bit to. If the index holds unknown values or is out of range, this vector remains unchanged.
     */
    void setBitSelect(const vec4state& index, const vec4state& newValue);

    /**
     * @brief Set bit select operator for vec4state.
     * 
     * Creates a vec4state that holds the value of index, then sets the bit stored in this vector at the given index to the value stored in newValue. This is done by extracting the value stored in index and by using the setPartSelect method by setting the start and end indices to index.
     * 
     * @tparam The type of index, must be an integral type or a string.
     * @param index The index of the bit to set.
     * @param newValue The value to set the bit to. If the index holds unknown values or is out of range, this vector remains unchanged.
     */
    template<typename T, typename enable_if<is_valid_type_for_vec4state<T>::value, bool>::type = true>
    void setBitSelect(T index, const vec4state& newValue) {
        setBitSelect(vec4state(index), newValue);
    }

    /**
     * @brief Set bit select operator for vec4state.
     * 
     * Creates a vec4state that holds the value of newValue, then sets the bit stored in this vector at the given index to the value stored in newValue. This is done by extracting the value stored in index and by using the setPartSelect method by setting the start and end indices to index.
     * 
     * @tparam The type of newValue, must be an integral type or a string.
     * @param index The index of the bit to set.
     * @param newValue The value to set the bit to. If the index holds unknown values or is out of range, this vector remains unchanged.
     */
    template<typename T, typename enable_if<is_valid_type_for_vec4state<T>::value, bool>::type = true>
    void setBitSelect(const vec4state& index, T newValue) {
        setBitSelect(index, vec4state(newValue));
    }

    /**
     * @brief Set bit select operator for vec4state.
     * 
     * Creates vec4states that hold the values of index and newValue, then sets the bit stored in this vector at the given index to the value stored in newValue. This is done by extracting the value stored in index and by using the setPartSelect method by setting the start and end indices to index.
     * 
     * @tparam The types of index and newValue, must be an integral type or a string.
     * @param index The index of the bit to set.
     * @param newValue The value to set the bit to. If the index holds unknown values or is out of range, this vector remains unchanged.
     */
    template<typename T1, typename enable_if<is_valid_type_for_vec4state<T1>::value, bool>::type = true, typename T2, typename enable_if<is_valid_type_for_vec4state<T2>::value, bool>::type = true>
    void setBitSelect(T1 index, T2 newValue) {
        setBitSelect(vec4state(index), vec4state(newValue));
    }

    /**
     * @brief Get part select operator for vec4state.
     * 
     * Extracts the part of this vector from index start to index end. If the start index is greater than the end index, vec4stateExceptionInvalidRange is thrown. If the slice is out of range, the out of range bits are set to x. This is done by shifting this vector's bit until reaching the desired bits, then extracting the relevant bits from the vector. If the part is out of range, the out of range bits are set to x.
     * 
     * @param end The end index of the part to extract.
     * @param start The start index of the part to extract.
     * @return The part of this vector from index start to index end.
     */
    vec4state getPartSelect(long long end, long long start) const;

    /**
     * @brief Set part select operator for vec4state.
     * 
     * Sets the part of the vector starting from the start index and ending at end index, to the value stored in other vector. If the end index is less than the start index, vec4stateExceptionInvalidRange is thrown. This is done by zeroing down the bits between the start and the end indices and inserting the bits of other vector into the slice. If the slice is completely out of range, the vector remains unchanged. If only start or end are out of range, the slice is truncated to the valid range.
     * 
     * @param end The end index of the part to set.
     * @param start The start index of the part to set.
     * @param other The vector to set the part to.
     */
    void setPartSelect(long long end, long long start, vec4state other);

    /**
     * @brief Set part select operator for vec4state.
     * 
     * Creates a vec4state that holds the value of num, then sets the part of the vector starting from the start index and ending at end index, to num. If the end index is less than the start index, vec4stateExceptionInvalidRange is thrown. This is done by zeroing down the bits between the start and the end indices and inserting the bits of num into the slice. If the slice is completely out of range, the vector remains unchanged. If only start or end are out of range, the slice is truncated to the valid range.
     * 
     * @param end The end index of the part to set.
     * @param start The start index of the part to set.
     * @tparam The type of num, must be an integral type or a string.
     * @param num The value to set the part to.
     */
    template<typename T, typename enable_if<is_valid_type_for_vec4state<T>::value, bool>::type = true>
    void setPartSelect(long long end, long long start, T num) {
        setPartSelect(end, start, vec4state(num));
    }

    /**
     * @brief Logical AND operator for vec4state.
     * 
     * Checks if both this vector and other vector are true (hold at least one 1 bit).
     * 
     * @param other The vector to perform the logical AND operation with.
     * @return 1'b1 if both vectors are true.
     * @return 1'b0 if one of the vectors is false.
     * @return 1'bx if one of the vectors is unknown and the other vector is true, or if both vectors are unknown.
     */
    vec4state operator&&(const vec4state& other) const;

    /**
     * @brief Logical AND operator for vec4state.
     * 
     * Creates a vec4state that holds the value of num, then checks if both this vector and num are true (hold at least one 1 bit).
     * 
     * @tparam The type of num, must be an integral type or a string.
     * @param num The value to perform the logical AND operation with.
     * @return 1'b1 if both vectors are true.
     * @return 1'b0 if one of the vectors is false.
     * @return 1'bx if one of the vectors is unknown and the other vector is true, or if both vectors are unknown.
     */
    template<typename T, typename enable_if<is_valid_type_for_vec4state<T>::value, bool>::type = true>
    vec4state operator&&(T num) const {
        return *this && vec4state(num);
    }

    /**
     * @brief Logical OR operator for vec4state.
     * 
     * Checks if at least one of this vector and other vector is true (holds at least one 1 bit).
     * 
     * @param other The vector to perform the logical OR operation with.
     * @return 1'b1 if one of the vectors is true.
     * @return 1'b0 if both vectors are false.
     * @return 1'bx if one of the vectors is unknown and the other vector is false.
     */
    vec4state operator||(const vec4state& other) const;

    /**
     * @brief Logical OR operator for vec4state.
     * 
     * Creates a vec4state that holds the value of num, then checks if at least one of this vector and num is true (holds at least one 1 bit).
     * 
     * @tparam The type of num, must be an integral type or a string.
     * @param num The value to perform the logical OR operation with.
     * @return 1'b1 if one of the vectors is true.
     * @return 1'b0 if both vectors are false.
     * @return 1'bx if one of the vectors is unknown and the other vector is false.
     */
    template<typename T, typename enable_if<is_valid_type_for_vec4state<T>::value, bool>::type = true>
    vec4state operator||(T num) const {
        return *this || vec4state(num);
    }

    /**
     * @brief Logical NOT operator for vec4state.
     * 
     * Checks if this vector is true (holds at least one 1 bit). This is done by iterating over the vector's VPIs, and checking if at least one of the VPIs has a bit set to 1.
     * 
     * @return 1'b0 if the vector is true.
     * @return 1'b1 if the vector is false.
     * @return 1'bx if the vector has an ambiguous truth value.
     */
    vec4state operator!() const;

    /**
     * @brief Less than relational operator for vec4state.
     * 
     * Checks if this vector is less than other vector. If the vectors are of unequal bit lengths, the smaller vector is zero-extended to the size of the larger vector.
     * 
     * @param other The vector to compare to.
     * @return 1'b1 if this vector is less than other vector.
     * @return 1'b0 if this vector is greater than or equal to other vector.
     * @return 1'bx if the comparison is ambiguous (due to unknown bits in one of the vectors).
     */
    vec4state operator<(const vec4state& other) const;

    /**
     * @brief Less than relational operator for vec4state.
     * 
     * Creates a vec4state that holds the value of num, then checks if this vector is less than num. If the vectors are of unequal bit lengths, the smaller vector is zero-extended to the size of the larger vector.
     * 
     * @tparam The type of num, must be an integral type or a string.
     * @param num The value to compare to.
     * @return 1'b1 if this vector is less than num.
     * @return 1'b0 if this vector is greater than or equal to num.
     * @return 1'bx if the comparison is ambiguous (due to unknown bits in one of the vectors).
     */
    template<typename T, typename enable_if<is_valid_type_for_vec4state<T>::value, bool>::type = true>
    vec4state operator<(T num) const {
        return *this < vec4state(num);
    }

    /**
     * @brief Greater than relational operator for vec4state.
     * 
     * Checks if this vector is greater than other vector. If the vectors are of unequal bit lengths, the smaller vector is zero-extended to the size of the larger vector. This is done by checking if the other vector is less than this vector.
     * 
     * @param other The vector to compare to.
     * @return 1'b1 if this vector is greater than other vector.
     * @return 1'b0 if this vector is less than or equal to other vector.
     * @return 1'bx if the comparison is ambiguous (due to unknown bits in one of the vectors).
     */
    vec4state operator>(const vec4state& other) const;

    /**
     * @brief Greater than relational operator for vec4state.
     * 
     * Creates a vec4state that holds the value of num, then checks if this vector is greater than num. If the vectors are of unequal bit lengths, the smaller vector is zero-extended to the size of the larger vector. This is done by checking if num is less than this vector.
     * 
     * @tparam The type of num, must be an integral type or a string.
     * @param num The value to compare to.
     * @return 1'b1 if this vector is greater than num.
     * @return 1'b0 if this vector is less than or equal to num.
     * @return 1'bx if the comparison is ambiguous (due to unknown bits in one of the vectors).
     */
    template<typename T, typename enable_if<is_valid_type_for_vec4state<T>::value, bool>::type = true>
    vec4state operator>(T num) const {
        return *this > vec4state(num);
    }

    /**
     * @brief Less than or equal to relational operator for vec4state.
     * 
     * Checks if this vector is less than or equal to other vector. If the vectors are of unequal bit lengths, the smaller vector is zero-extended to the size of the larger vector. This is done by checking if this vector is greater than other vector, and then negating the result.
     * 
     * @param other The vector to compare to.
     * @return 1'b1 if this vector is less than or equal to other vector.
     * @return 1'b0 if this vector is greater than other vector.
     * @return 1'bx if the comparison is ambiguous (due to unknown bits in one of the vectors).
     */
    vec4state operator<=(const vec4state& other) const;

    /**
     * @brief Less than or equal to relational operator for vec4state.
     * 
     * Creates a vec4state that holds the value of num, then checks if this vector is less than or equal to num. If the vectors are of unequal bit lengths, the smaller vector is zero-extended to the size of the larger vector. This is done by checking if this vector is greater than num, and then negating the result.
     * 
     * @tparam The type of num, must be an integral type or a string.
     * @param num The value to compare to.
     * @return 1'b1 if this vector is less than or equal to num.
     * @return 1'b0 if this vector is greater than num.
     * @return 1'bx if the comparison is ambiguous (due to unknown bits in one of the vectors).
     */
    template<typename T, typename enable_if<is_valid_type_for_vec4state<T>::value, bool>::type = true>
    vec4state operator<=(T num) const {
        return *this <= vec4state(num);
    }

    /**
     * @brief Greater than or equal to relational operator for vec4state.
     * 
     * Checks if this vector is greater than or equal to other vector. If the vectors are of unequal bit lengths, the smaller vector is zero-extended to the size of the larger vector. This is done by checking if this vector is less than other vector, and then negating the result.
     * 
     * @param other The vector to compare to.
     * @return 1'b1 if this vector is greater than or equal to other vector.
     * @return 1'b0 if this vector is less than other vector.
     * @return 1'bx if the comparison is ambiguous (due to unknown bits in one of the vectors).
     */
    vec4state operator>=(const vec4state& other) const;

    /**
     * @brief Greater than or equal to relational operator for vec4state.
     * 
     * Creates a vec4state that holds the value of num, then checks if this vector is greater than or equal to num. If the vectors are of unequal bit lengths, the smaller vector is zero-extended to the size of the larger vector. This is done by checking if this vector is less than num, and then negating the result.
     * 
     * @tparam The type of num, must be an integral type or a string.
     * @param num The value to compare to.
     * @return 1'b1 if this vector is greater than or equal to num.
     * @return 1'b0 if this vector is less than num.
     * @return 1'bx if the comparison is ambiguous (due to unknown bits in one of the vectors).
     */
    template<typename T, typename enable_if<is_valid_type_for_vec4state<T>::value, bool>::type = true>
    vec4state operator>=(T num) const {
        return *this >= vec4state(num);
    }

    /**
     * @brief Addition operator for vec4state.
     * 
     * Calculates the sum of this vector and other vector. The method iterates over the vectors' VPIs, and calculates the sum of the corresponding VPIs. If the sum is greater than 32 bits, the carry is set to 1 for the next iteration. If there is a carry in the last iteration, the size of the result vector is increased by 1.
     * 
     * @param other The vector to add.
     * @return A new vector that holds the result of the addition operation. If one of the vectors holds unknown bits, then the result is only x's.
     */
    vec4state operator+(const vec4state& other) const;

    /**
     * @brief Addition operator for vec4state.
     * 
     * Creates a vec4state that holds the value of num, then calculates the sum of this vector and num. The method iterates over the vectors' VPIs, and calculates the sum of the corresponding VPIs. If the sum is greater than 32 bits, the carry is set to 1 for the next iteration. If there is a carry in the last iteration, the size of the result vector is increased by 1.
     * 
     * @tparam The type of num, must be an integral type or a string.
     * @param num The value to add.
     * @return A new vector that holds the result of the addition operation. If one of the vectors holds unknown bits, then the result is only x's.
     */
    template<typename T, typename enable_if<is_valid_type_for_vec4state<T>::value, bool>::type = true>
    vec4state operator+(T num) {
        return *this + vec4state(num);
    }

    /**
     * @brief Subtraction operator for vec4state.
     * 
     * Calculates the difference between this vector and other vector. The method iterates over the vectors' VPIs, and calculates the difference of the corresponding VPIs. If the current VPI in this vector is less than the corresponding one in other vector, the method borrows from the next positive VPI of this vector. If there is no VPI to borrow from, the result is negative.
     * 
     * @param other The vector to subtract.
     * @return A new vector that holds the result of the subtraction operation. If one of the vectors holds unknown bits, then the result is only x's.
     */
    vec4state operator-(const vec4state& other) const;

    /**
     * @brief Subtraction operator for vec4state.
     * 
     * Creates a vec4state that holds the value of num, then calculates the difference between this vector and num.  The method iterates over the vectors' VPIs, and calculates the difference of the corresponding VPIs. If the current VPI in this vector is less than the corresponding one in other vector, the method borrows from the next positive VPI of this vector. If there is no VPI to borrow from, the result is negative.
     * 
     * @tparam The type of num, must be an integral type or a string.
     * @param num The value to subtract.
     * @return A new vector that holds the result of the subtraction operation. If one of the vectors holds unknown bits, then the result is only x's.
     */
    template<typename T, typename enable_if<is_valid_type_for_vec4state<T>::value, bool>::type = true>
    vec4state operator-(T num) const {
        return *this - vec4state(num);
    }

    /**
     * @brief Multiplication operator for vec4state.
     * 
     * Calculates the product of this vector and other vector. The method aligns the vectors to the same size, and then multiplies each VPI in this vector by each VPI in other vector and adds the result to the corresponding VPI in the result vector.
     * 
     * @param other The vector to multiply.
     * @return A new vector that holds the result of the multiplication operation. If one of the vectors holds unknown bits, then the result is only x's.
     */
    vec4state operator*(const vec4state& other) const;

    /**
     * @brief Multiplication operator for vec4state.
     * 
     * Creates a vec4state that holds the value of num, then calculates the product of this vector and num. The method aligns the vectors to the same size, and then multiplies each VPI in this vector by each VPI in other vector and adds the result to the corresponding VPI in the 
     * 
     * @tparam The type of num, must be an integral type or a string.
     * @param num The value to multiply.
     * @return A new vector that holds the result of the multiplication operation. If one of the vectors holds unknown bits, then the result is only x's.
     */
    template<typename T, typename enable_if<is_valid_type_for_vec4state<T>::value, bool>::type = true>
    vec4state operator*(T num) const {
        return *this * vec4state(num);
    }

    /**
     * @brief Division operator for vec4state.
     * 
     * Calculates the division of this vector by other vector. If other is 0, vec4stateExceptionInvalidOperation is thrown.
     * 
     * @param other The vector to divide by.
     * @return A new vector that holds the result of the division operation. If one of the vectors holds unknown bits, then the result is only x's.
     */
    vec4state operator/(const vec4state& other) const;

    /**
     * @brief Division operator for vec4state.
     * 
     * Creates a vec4state that holds the value of num, then calculates the division of this vector by num. If num is 0, vec4stateExceptionInvalidOperation is thrown.
     * 
     * @tparam The type of num, must be an integral type or a string.
     * @param num The value to divide by.
     * @return A new vector that holds the result of the division operation. If one of the vectors holds unknown bits, then the result is only x's.
     */
    template<typename T, typename enable_if<is_valid_type_for_vec4state<T>::value, bool>::type = true>
    vec4state operator/(T num) const {
        return *this / vec4state(num);
    }

    /**
     * @brief Modulus operator for vec4state.
     * 
     * Calculates the modulus of this vector by other vector. If other is 0, vec4stateExceptionInvalidOperation is thrown.
     * 
     * @param other The vector to calculate the modulus by.
     * @return A new vector that holds the result of the modulus operation. If one of the vectors holds unknown bits, then the result is only x's.
     */
    vec4state operator%(const vec4state& other) const;

    /**
     * @brief Modulus operator for vec4state.
     * 
     * Creates a vec4state that holds the value of num, then calculates the modulus of this vector by num. If num is 0, vec4stateExceptionInvalidOperation is thrown.
     * 
     * @tparam The type of num, must be an integral type or a string.
     * @param num The value to calculate the modulus by.
     * @return A new vector that holds the result of the modulus operation. If one of the vectors holds unknown bits, then the result is only x's.
     */
    template<typename T, typename enable_if<is_valid_type_for_vec4state<T>::value, bool>::type = true>
    vec4state operator%(T num) const {
        return *this % vec4state(num);
    }

    /**
     * @brief Minus operator for vec4state.
     * 
     * Calculates the negative value of this vector. The method calculates the two's complement of this vector by inverting all the bits and adding 1.
     * 
     * @return A new vector that holds the result of the minus operation. If this vector holds unknown bits, then the result is only x's.
     */
    vec4state operator-() const;

    /**
     * @brief Power operator for vec4state.
     * 
     * Calculates the value of the vector to the power of other vector. The method calculates the power of the vector by multiplying the vector by itself other times.
     * 
     * @param other The vector to raise the value to the power of.
     * @return A new vector that holds the result of the power operation. If one of the vectors holds unknown bits, then the result is only x's.
     */
    vec4state power(const vec4state& other) const;

    /**
     * @brief Power operator for vec4state.
     * 
     * Creates a vec4state that holds the value of num, then calculates the value of the vector to the power of num. The method calculates the power of the vector by multiplying the vector by itself other times.
     * 
     * @tparam The type of num, must be an integral type or a string.
     * @param num The value to raise the value to the power of.
     * @return A new vector that holds the result of the power operation. If one of the vectors holds unknown bits, then the result is only x's.
     */
    template<typename T, typename enable_if<is_valid_type_for_vec4state<T>::value, bool>::type = true>
    vec4state power(T num) const {
        return power(vec4state(num));
    }

    /**
     * @brief Conversion operator to vec2state for vec4state.
     * 
     * Converts this vector to a 2-state vector by replacing all the unknown values with 0's. This is done by iterating over the vector's VPIs, setting the z bits to x bits (aval = 0, bval = 1), and then setting the x bits to 0 bits.
     */
    void convertTo2State();
    
    /**
     * @brief Gets the number of bits in this vector.
     * 
     * @return The number of bits in this vector.
     */
    long long getNumBits() const;

    /**
     * @brief Gets the vector of the VPI elements in this vector.
     * 
     * @return The vector of the VPI elements in this vector.
     */
    shared_ptr<VPI[]> getVector() const;

    /**
     * @brief Gets the number of VPI elements in this vector.
     * 
     * @return The number of VPI elements in this vector.
     */
    long long getVectorSize() const;

    /**
     * @brief Checks if the vector contains any unknown values.
     * 
     * @return true if the vector contains any unknown values.
     * @return false if the vector does not contain any unknown values.
     */
    bool isUnknown() const;

    /**
     * @brief String representation of the vector.
     * 
     * Creates a string representation of the vecor, where each bit is represented by it's corresponding BitValue (0, 1, x, or z). This is done by iterating over the vector's VPIs from MSB to LSB, and for each VPI, iterating over the bits in it and adding the corresponding BitValue to the result string.
     * 
     * @return A string representation of the vector.
     */
    string toString() const;

private:
    /**
     * @brief Array of VPI elements.
     * 
     * The 4-value vector consists of an array of VPI elements, where each element is 32 bits long and each bit can be either 0, 1, x, or z. The first element holds the least significant 32 bits.
     */
    shared_ptr<VPI[]> vector;

    /**
     * @brief Number of bits in the vector.
     * 
     * The number of bits the vector can hold can be up to 2^63 - 1 bits.
     */
    long long numBits;

    /**
     * @brief Number of VPI elements in the vector field.
     * 
     * The number of VPI elements in the vector is equal to the number of bits divided by 32, rounded up.
     */
    long long vectorSize;

    /**
     * @brief Flag that indicates if the vector contains any unknown values.
     * 
     * true if the vector contains any unknown values.
     * false if the vector does not contain any unknown values.
     */
    bool unknown;

    /**
     * @brief Single repeated BitValue constructor for vec4state.
     * 
     * Initializes a vector with a single bit (that can be either 0, 1, x, or z) repeated numBits times. If numBits is non-positive or bit is not a BitValue, the vector is default initialized and vec4stateExceptionInvalidSize or vec4stateExceptionInvalidInput is thrown, respectively.
     * 
     * @param bit The bit to repeat.
     * @param numBits The number of bits in the vector.
     */
    vec4state(BitValue bit, long long numBits);

    /**
     * @brief Increment number of bits for vec4state.
     * 
     * Increments the number of bits in the vector to newNumBits by assigning a bigger VPI array to this vector, copying the values from the original array to the new one, and zero-extending the new VPIs in the new array. Throws vec4stateExceptionInvalidSize if newNumBits is less than the current number of bits or non-positive. If newNumBits is the same as the current number of bits, the vector remains unchanged.
     * 
     * @param newNumBits The new number of bits in the vector.
     */
    void incNumBits(long long newNumBits);

    /**
     * @brief Truncate number of bits for vec4state.
     * 
     * Decreases the number of bits in the vector to newNumBits by assigning a smaller VPI array to this vector, copying the values that are in the new range from the original array to the new one, and truncating the VPI that are not in the new range. Throws vec4stateExceptionInvalidSize if newNumBits is greater than the current number of bits or negative. If newNumBits is the same as the current number of bits, the vector remains unchanged. If newNumBits is 0, the vector is truncated to a single x bit.
     * 
     * @param newNumBits The new number of bits in the vector.
     */
    void decNumBits(long long newNumBits);

    /**
     * @brief Extract number from vector for vec4state.
     * 
     * Calculates the numerical value that the vector holds by iterating over the vector's VPIs, adding and shifting them. This method can be used only if the vector has up to 2 VPI elements (so it's value can be represented by 64 bits) and has no unknown bits. If the vector has more than 2 VPI elements, vec4stateExceptionInvalidSize is throwns. If the vector has unknown bits, vec4stateExceptionUnknownVector is thrown.
     * 
     * @return The numerical value that the vector holds.
     */
    long long extractNumberFromVector() const;

    /**
     * @brief Get part of the vector that is in range.
     * 
     * Extracts the part of the vector that is in range, starting from the first bit and ending at the given end index. If the end index is out of range, vec4stateExceptionInvalidInput is thrown. This is done by initializing the result vector to end z bits, which means that it holds end 1 bits in it's aval and bval. Then, the method iterates over the vector's VPIs and copying the relevant bits to the result vector by performing a bitwise AND operation between the vector's VPIs and the result vector's VPIs.
     * 
     * @param end The end index of the part to extract.
     * @return The part of the vector from the first bit to the end index.
     */
    vec4state getPartValidRange(long long end) const;

    /**
     * @brief Addition of the aval and bval of the vectors.
     * 
     * Adds the aval and bval of the vectors, resulting in a new vector. If the vectors are of unequal bit lengths, the smaller vector is zero-extended to the size of the larger vector. The method iterates over the vectors' VPIs and calculates the sum of the aval and bval of each VPI.
     * 
     * @param other The vector to add to this vector.
     * @return A new vector that holds the result of the addition.
     */
    vec4state additionAvalBval(const vec4state& other) const;

    /**
     * @brief Bitwise AND of the aval and bval of the vectors.
     * 
     * Calculates the bitwise AND of the aval and bval of the vectors. The method iterates over the vectors' VPIs and calculates the bitwise AND of the aval and bval of each VPI. If the vectors are of unequal bit lengths, the smaller vector is zero-extended to the size of the larger vector.
     * 
     * @param other The vector to perform the bitwise AND operation with.
     * @return A new vector that holds the result of the bitwise AND operation.
     */
    vec4state bitwiseAndAvalBval(const vec4state& other) const;

    /**
     * @brief Sets the number of bits to a new number for vec4state.
     * 
     * Sets the number of bits in the vector to newNumBits by truncating or extending the vector (using decNumBits / incNumBits, respectively). If newNumBits is less than the current number of bits, the vector is truncated. If newNumBits is greater than the current number of bits, the vector is zero-extended. If newNumBits is the same as the current number of bits, the vector remains unchanged. If newNumBits is negative, vec4stateExceptionInvalidSize is thrown.
     * 
     * @param newNumBits The new number of bits in the vector.
     */
    void setNumBits(long long newNumBits);

    /**
     * @brief Sets unknown field for vec4state.
     * 
     * Scans the vector and sets the unknown flag to true if encountered unknown values in the vector, and to false otherwise.
     */
    void setUnknown();


};

#endif // VEC4STATE_H