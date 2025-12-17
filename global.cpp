#include "global.h"

int global::getDlloutFlag() const
{
    return DlloutFlag;
}

void global::setDlloutFlag(int value)
{
    DlloutFlag = value;
}
