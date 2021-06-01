/*
 * Copyright (c) 2021 Huawei Device Co., Ltd.
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

#ifndef BATTERYD_H
#define BATTERYD_H

#include "batteryd_api.h"
#include "core/hdf_device_desc.h"

struct BatterydService {
    struct IDeviceIoService ioService;
    struct HdfDeviceObject *device;
    struct BatterydSubscriber *subscriber;
    int32_t ueventFd;
};

int32_t BindBatterySubscriber(struct BatterydService *service, struct BatterydSubscriber *subscriber);
int32_t UnbindBatterySubscriber(struct BatterydService *service);

#endif // BATTERYD_H
