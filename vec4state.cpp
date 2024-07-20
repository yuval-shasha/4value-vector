#include "vec4state.h"

vec4state::vec4state()
{
    vector = new VPI[1];
    size = 1;
}

vec4state::vec4state(int num)
{
    vector = new VPI[1];
    size = 1;
    vector[0].setAval(num);
    vector[0].setBval(0);
}