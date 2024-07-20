#ifndef VPI_H
#define VPI_H

#include <stdint.h>

// This class describes the 4 possible values in each element of the vector, where:
// a_val = 0, b_val = 0 => 0
// a_val = 1, b_val = 0 => 1
// a_val = 0, b_val = 1 => x
// a_val = 1, b_val = 1 => z
class VPI 
{
    private:
        uint32_t a_val;
        uint32_t b_val;
    public:
        VPI() : a_val(0), b_val(0xFFFFFFFF) {};
        VPI(uint32_t a, uint32_t b) : a_val(a), b_val(b) {};
        ~VPI();
        uint32_t getAval() const { return a_val; };
        uint32_t getBval() const { return b_val; };
        void setAval(uint32_t new_val) { a_val = new_val; };
        void setBval(uint32_t new_val) { b_val = new_val; };
};

#endif // VPI_H