/**
 * @file vpi.h
 * @brief Declaration and implementation of the VPI class.
 * 
 * This file contains the declaration and implementation of the VPI class, which is used to represent the 4 possible values in each element of the 4-state vector.
 * 
 * @author Mia Ekheizer, Yuval Shasha
 * @date 2024-08-24
 */

#ifndef VPI_H
#define VPI_H

#include <stdint.h>

/**
 * @class VPI
 * @brief This class represents the 4 possible values in each element of the 4-state vector.
 * 
 * For representing the 4 possible values each bit in the 4-state vector can have, there are 2 fields in the VPI class: a_val and b_val. Each field is a 32-bit unsigned integer, and each bit in the integer represents one of the 4 possible values, where b_val indicates wether the bit is known or unknown. The values are represented as follows:
 * - a_val = 0, b_val = 0 => 0
 * - a_val = 1, b_val = 0 => 1
 * - a_val = 0, b_val = 1 => x
 * - a_val = 1, b_val = 1 => z
 */
class VPI {
public:
    /**
     * @brief Default constructor for the VPI class.
     * 
     * Initializes the VPI object with the default values of x.
     */
    VPI() : a_val(0), b_val(0xFFFFFFFF) {};

    /**
     * @brief Constructor for the VPI class.
     * 
     * Initializes the VPI object with the given values.
     * 
     * @param a The a_val field of the VPI.
     * @param b The b_val field of the VPI.
     */
    VPI(uint32_t a, uint32_t b) : a_val(a), b_val(b) {};

    /**
     * @brief Copy constructor for the VPI class.
     * 
     * Inializes the VPI object with the values of other.
     * 
     * @param other The VPI object to copy from.
     */
    VPI(const VPI& other) : a_val(other.a_val), b_val(other.b_val) {};

    /**
     * @brief Get the a_val field of the VPI.
     * 
     * @return The a_val field of the VPI.
     */
    uint32_t getAval() const { 
        return a_val;
    };
    
    /**
     * @brief Get the b_val field of the VPI.
     * 
     * @return The b_val field of the VPI.
     */
    uint32_t getBval() const {
        return b_val;
    };
    
    /**
     * @brief Set the a_val field of the VPI to new_val.
     * 
     * @param new_val The new value to set the a_val field to.
     */
    void setAval(uint32_t new_val) {
        a_val = new_val;
    };
    
    /**
     * @brief Set the b_val field of the VPI to new_val.
     * 
     * @param new_val The new value to set the b_val field to.
     */
    void setBval(uint32_t new_val) {
        b_val = new_val;
    };
    
    /**
     * @brief Gets the known bits of the VPI.
     * 
     * This method extracts only the bits of the VPI that are known by removing the 1 bits in the a_val field that represent z.
     * 
     * @return The number represented by the known bits of the VPI.
     */
    uint32_t getKnownBits() const {
        return a_val - (a_val & b_val);
    };

private:
    /**
     * @brief The a_val field of the VPI, which represents the known values of the 4-state vector - 0 and 1.
     * 
     */
    uint32_t a_val;

    /**
     * @brief The b_val field of the VPI, which represents the unknown values of the 4-state vector - x and z.
     * 
     */
    uint32_t b_val;
};

#endif // VPI_H