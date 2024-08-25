#include "vec4state.h"
#include <iostream>
#include <string>

using namespace std;

int main()
{
    vec4state defaultVector;
    vec4state vec1("01xz1100");
    vec4state vec2(15); // 15 = 0b1111
    vec4state vec3("xzxx00");
    // printing the vectors
    cout << "Default vector: " << defaultVector.toString() << endl;
    cout << "Vector 1: " << vec1.toString() << endl;
    cout << "Vector 2: " << vec2.toString() << endl;
    cout << "Vector 3: " << vec3.toString() << endl;
    // shift
    cout << "Vector 1 shifted left by 2: " << (vec1<<2).toString() << endl;
    cout << "Vector 3 shifted right by 4: " << (vec3>>4).toString() << endl;
    // bitwise operations
    cout << "Vector 1 AND Vector 2: " << (vec1 & vec2).toString() << endl;
    cout << "Vector 1 OR Vector 3: " << (vec1 | vec3).toString() << endl;
    cout << "not Vector 2: " << (~vec2).toString() << endl;
    cout << "not Vector 3: " << (~vec3).toString() << endl;
}