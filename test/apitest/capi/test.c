#include <stdio.h>
#include "ohbattery_info.h"
int main()
{
#ifndef __cplusplus
    printf("not a c++ programm \n");
#endif
    printf("Hello World! \n");
    int32_t capacity = OH_BatteryInfo_GetCapacity();
    BatteryInfo_BatteryPluggedType ptype = OH_BatteryInfo_GetPluggedType();
    printf("capacity %d; plugType %d\n", capacity, ptype);
}