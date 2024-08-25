# 4value-vector

## Overview

**4value-vector** is a C++ library that implements logic vectors based on System Verilog's 4-value logic. This library allows for the initialization and manipulation of 4-value vectors, supporting various operations such as arithmetic, logical, shift, relational, assignment, and bitwise operations. It is designed primarily for the automation and verification of hardware logic vectors.

## Features

- **4-State Vector Initialization**: Create and manage 4-value vectors (`vec4state`).
- **Extensive Operator Support**: Perform a wide range of operations including arithmetic, logical, and bitwise.
- **Exception Handling**: Robust exception handling for invalid operations, inputs, and sizes.
- **Hardware Logic Verification**: Useful in simulation and verification environments.

## Installation

1. Clone the Repository:
   `git clone https://github.com/yuval-shasha/4value-vector.git`

2. Dependencies:
   Ensure `CMake` and `Google Test` are installed.

3. Build the Project:
   `cmake -S . -B build`
   `cmake --build build`

## Usage

The `VPI` class represents the 4 possible values (0, 1, x, z) in each element of the 4-state vector.

### VPI Class Methods

1. **`VPI()`**:
   - **Description**: Default constructor initializes the value to `x`.
   - **Example**:
     ```cpp
     VPI vpi_default;
     ```

2. **`VPI(uint32_t a, uint32_t b)`**:
   - **Description**: Initializes the object with specific values for `a_val` (known values) and `b_val` (unknown values).
   - **Example**:
     ```cpp
     VPI vpi(0x0, 0xFFFFFFFF);
     ```

3. **`uint32_t getAval() const`**:
   - **Description**: Returns the `a_val` field representing the known values in the vector.
   - **Example**:
     ```cpp
     uint32_t a = vpi.getAval();
     ```

4. **`uint32_t getBval() const`**:
   - **Description**: Returns the `b_val` field representing the unknown values in the vector.
   - **Example**:
     ```cpp
     uint32_t b = vpi.getBval();
     ```

5. **`void setAval(uint32_t new_val)`**:
   - **Description**: Sets the `a_val` field to a new value.
   - **Example**:
     ```cpp
     vpi.setAval(0x1);
     ```

6. **`void setBval(uint32_t new_val)`**:
   - **Description**: Sets the `b_val` field to a new value.
   - **Example**:
     ```cpp
     vpi.setBval(0x0);
     ```

### Example Usage:

```cpp
#include "vpi.h"
#include <iostream>

int main() {
    // Default constructor initializes the value to 'x'
    VPI vpi_default;

    // Initializing with specific values
    VPI vpi(0x0, 0xFFFFFFFF);

    // Accessing the 'a_val' and 'b_val' fields
    uint32_t a = vpi.getAval();
    uint32_t b = vpi.getBval();

    // Setting new values
    vpi.setAval(0x1);
    vpi.setBval(0x0);

    // Print the old values
    std::cout << "A Value: " << a << std::endl;
    std::cout << "B Value: " << b << std::endl;

    return 0;
}
```

#vec4state
The `vec4state` class represents a vector of 4-state values (0, 1, x, z) and supports various operations essential for digital logic and circuit design.

### CONSTRUCTORS

1. **`vec4state()`**:
   - **Description**: Default constructor initializes a 1-bit vector to `x`.
   - **Example**:
     ```cpp
     vec4state vec;
     ```

2. **`vec4state(T num)`**:
   - **Description**: Initializes a vector from an integral type, setting the vector to the binary equivalent of the provided number.
   - **Example**:
     ```cpp
     vec4state vec(5);  // Initializes with binary equivalent of 5.
     ```

3. **`vec4state(std::string str)`**:
   - **Description**: Initializes a vector from a string of bit values (`0`, `1`, `x`, `z`).
   - **Example**:
     ```cpp
     vec4state vec("01xz");
     ```

4. **`vec4state(BitValue bit, long long numBits)`**:
   - **Description**: Initializes a vector with a single bit (that can be either 0, 1, x, or z) repeated `numBits` times.
   - **Example**:
     ```cpp
     vec4state vec(ZERO, 32);
     ```

5. **`vec4state(char bit)`**:
   - **Description**: Bit constructor initializes a vector that holds a bit of size 1. If `bit` is not a `BitValue`, the vector is initialized to `x` and `vec4stateExceptionInvalidInput` is thrown.
   - **Example**:
     ```cpp
     try {
         vec4state vec('1');  // Valid BitValue
     } catch (const vec4stateExceptionInvalidInput& e) {
         std::cerr << "Error: " << e.what() << std::endl;
     }
     ```

6. **`vec4state(const vec4state& other)`**:
   - **Description**: Copy constructor initializes a new vector with the values of another vector.
   - **Example**:
     ```cpp
     vec4state vec2 = vec1;
     ```

7. **`vec4state(vec4state&& other)`**:
   - **Description**: Move constructor transfers the ownership of another vector to a new object.
   - **Example**:
     ```cpp
     vec4state vec2 = std::move(vec1);
     ```

### BITWISE OPERATORS

8. **`vec4state operator&(const vec4state& other) const`**:
   - **Description**: Performs a bitwise AND operation between two vectors.
   - **Example**:
     ```cpp
     vec4state result = vec1 & vec2;
     ```

9. **`vec4state operator|(const vec4state& other) const`**:
   - **Description**: Performs a bitwise OR operation between two vectors.
   - **Example**:
     ```cpp
     vec4state result = vec1 | vec2;
     ```

10. **`vec4state operator^(const vec4state& other) const`**:
   - **Description**: Performs a bitwise XOR operation between two vectors.
   - **Example**:
     ```cpp
     vec4state result = vec1 ^ vec2;
     ```

11. **`vec4state operator~() const`**:
    - **Description**: Performs a bitwise NOT operation on a vector.
    - **Example**:
      ```cpp
      vec4state result = ~vec1;
      ```

### ARITHMETIC OPERATORS

12. **`vec4state operator+(const vec4state& other) const`**:
    - **Description**: Adds two vectors.
    - **Example**:
      ```cpp
      vec4state result = vec1 + vec2;
      ```

13. **`vec4state operator-(const vec4state& other) const`**:
    - **Description**: Subtracts one vector from another.
    - **Example**:
      ```cpp
      vec4state result = vec1 - vec2;
      ```

14. **`vec4state operator*(const vec4state& other) const`**:
    - **Description**: Multiplies two vectors.
    - **Example**:
      ```cpp
      vec4state result = vec1 * vec2;
      ```

15. **`vec4state operator/(const vec4state& other) const`**:
    - **Description**: Divides one vector by another.
    - **Example**:
      ```cpp
      vec4state result = vec1 / vec2;
      ```

16. **`vec4state operator%(const vec4state& other) const`**:
    - **Description**: Computes the modulus of one vector with another.
    - **Example**:
      ```cpp
      vec4state result = vec1 % vec2;
      ```

### RELATIONAL AND LOGICAL OPERATORS

17. **`bool operator==(const vec4state& other) const`**:
    - **Description**: Checks if two vectors are equal.
    - **Example**:
      ```cpp
      bool isEqual = (vec1 == vec2);
      ```

18. **`bool operator!=(const vec4state& other) const`**:
    - **Description**: Checks if two vectors are not equal.
    - **Example**:
      ```cpp
      bool notEqual = (vec1 != vec2);
      ```

19. **`bool operator<(const vec4state& other) const`**:
    - **Description**: Checks if this vector is less than another vector.
    - **Example**:
      ```cpp
      bool isLess = (vec1 < vec2);
      ```

20. **`bool operator<=(const vec4state& other) const`**:
    - **Description**: Checks if this vector is less than or equal to another vector.
    - **Example**:
      ```cpp
      bool isLessOrEqual = (vec1 <= vec2);
      ```

21. **`bool operator>(const vec4state& other) const`**:
    - **Description**: Checks if this vector is greater than another vector.
    - **Example**:
      ```cpp
      bool isGreater = (vec1 > vec2);
      ```

22. **`bool operator>=(const vec4state& other) const`**:
    - **Description**: Checks if this vector is greater than or equal to another vector.
    - **Example**:
      ```cpp
      bool isGreaterOrEqual = (vec1 >= vec2);
      ```

23. **`bool operator&&(const vec4state& other) const`**:
    - **Description**: Performs a logical AND operation between two vectors.
    - **Example**:
      ```cpp
      bool result = (vec1 && vec2);
      ```

24. **`bool operator||(const vec4state& other) const`**:
    - **Description**: Performs a logical OR operation between two vectors.
    - **Example**:
      ```cpp
      bool result = (vec1 || vec2);
      ```

25. **`bool operator!() const`**:
    - **Description**: Performs a logical NOT operation on a vector.
    - **Example**:
      ```cpp
      bool result = !vec1;
      ```

26. **`bool caseEquality(const vec4state& other) const`**:
    - **Description**: Checks if two vectors are equal considering case sensitivity.
    - **Example**:
      ```cpp
      bool isCaseEqual = vec1.caseEquality(vec2);
      ```

27. **`bool caseInequality(const vec4state& other) const`**:
    - **Description**: Checks if two vectors are not equal considering case sensitivity.
    - **Example**:
      ```cpp
      bool isCaseNotEqual = vec1.caseInequality(vec2);
      ```

### SLICE OPERATORS

28. **`vec4state getPartSelect(long long end, long long start) const`**:
    - **Description**: Extracts a slice of the vector from `start` to `end`.
    - **Example**:
      ```cpp
      vec4state slice = vec1.getPartSelect(3, 1);
      ```

29. **`void setPartSelect(long long end, long long start, const vec4state& newValue)`**:
    - **Description**: Sets the slice of the vector from `start` to `end`.
    - **Example**:
      ```cpp
      vec1.setPartSelect(3, 1, newValue);
      ```

30. **`vec4state getBitSelect(const vec4state& index) const`**:
    - **Description**: Gets the bit at a specific index.
    - **Example**:
      ```cpp
      vec4state bit = vec1.getBitSelect(2);
      ```

31. **`void setBitSelect(const vec4state& index, const vec4state& newValue)`**:
    - **Description**: Sets a specific bit in the vector at a given index.
    - **Example**:
      ```cpp
      vec1.setBitSelect(vecIndex, newValue);
      ```

### MISCELLANEOUS OPERATORS

32. **`vec4state toString() const`**:
    - **Description**: Converts the vector to a string representation.
    - **Example**:
      ```cpp
      std::string str = vec1.toString();
      ```

### Example Usage:

```cpp
#include "vec4state.h"
#include <iostream>

int main() {
    // Initialize a vector from a string of bit values
    vec4state vec1("zx01");
    vec4state vec2("1100");

    // Perform a bitwise AND operation
    vec4state result = vec1 & vec2;

    // Print the result
    std::cout << "Result: " << result.toString() << std::endl;

    return 0;
}
```

### Exception Handling

The `vec4state` class includes robust exception handling to manage various error scenarios during vector operations. Below are the custom exceptions used in the class:

1. **`vec4stateException`**:
   - **Description**: Base class for all exceptions thrown by the `vec4state` class.

2. **`vec4stateExceptionInvalidIndex`**:
   - **Description**: Thrown when an invalid index is provided during bit or part selection.
   - **Example**:
     ```cpp
     try {
         vec4state vec("ZX01");
         vec.getBitSelect(10);  // Invalid index
     } catch (const vec4stateExceptionInvalidIndex& e) {
         std::cerr << "Error: " << e.what() << std::endl;
     }
     ```

3. **`vec4stateExceptionInvalidRange`**:
   - **Description**: Thrown when an invalid range is provided for part selection.
   - **Example**:
     ```cpp
     try {
         vec4state vec("1100");
         vec.getPartSelect(5, 3);  // Invalid range
     } catch (const vec4stateExceptionInvalidRange& e) {
         std::cerr << "Error: " << e.what() << std::endl;
     }
     ```

4. **`vec4stateExceptionInvalidSize`**:
   - **Description**: Thrown when an invalid vector size is specified during initialization.
   - **Example**:
     ```cpp
     try {
         vec4state vec(ZERO, -1);  // Invalid size
     } catch (const vec4stateExceptionInvalidSize& e) {
         std::cerr << "Error: " << e.what() << std::endl;
     }
     ```

5. **`vec4stateExceptionInvalidInput`**:
   - **Description**: Thrown when invalid input is provided to the vector during initialization.
   - **Example**:
     ```cpp
     try {
         vec4state vec("ABCD");  // Invalid input
     } catch (const vec4stateExceptionInvalidInput& e) {
         std::cerr << "Error: " << e.what() << std::endl;
     }
     ```

6. **`vec4stateExceptionInvalidOperation`**:
   - **Description**: Thrown when an invalid operation is performed on the vector, such as incompatible vector sizes during arithmetic or bitwise operations.
   - **Example**:
     ```cpp
     try {
         vec4state vec1("1100");
         vec4state vec2("101");  // Different size
         vec4state result = vec1 + vec2;  // Invalid operation
     } catch (const vec4stateExceptionInvalidOperation& e) {
         std::cerr << "Error: " << e.what() << std::endl;
     }
     ```

_For a complete list of methods and detailed descriptions, refer to the source code and the provided examples._

## Running Tests

1. **Install Google Test**:
   ```bash
   vcpkg install gtest
   ```
2. **Run the Tests**:
   ```bash
   cmake -S . -B build (in case it does not work append: -G "MinGW Makefiles")
   cmake --build build
   cd build
   ctest
   ```
