/*
 * Copyright (c) 2024 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
#include <dlfcn.h>
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
    printf("conopeningnecting ndk so \n");
    void* soHandle = dlopen("libohbattery_info.so", RTLD_NOW);
    if (!soHandle) {
        printf("opening ndk so failed\n");
        return 0;
    }
    void* getCapacityPtr = dlsym(soHandle, "OH_BatteryInfo_GetCapacity");
    if (!getCapacityPtr) {
        printf("find symbol failed\n");
        return 0;
    }
    void* pluggedTypePtr = dlsym(soHandle, "OH_BatteryInfo_GetPluggedType");
       if (!pluggedTypePtr) {
        printf("find symbol2 failed\n");
        return 0;
    }
    printf("calling OH_BatteryInfo_GetCapacity from ndk so \n");
    capacity = ((int32_t(*)(void))getCapacityPtr)();
    printf("calling OH_BatteryInfo_GetPluggedType from ndk so \n");
    ptype = ((BatteryInfo_BatteryPluggedType(*)(void))pluggedTypePtr)();

    printf("capacity %d; plugType %d\n", capacity, ptype);
}