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

#ifndef BATTERYD_PARSER_H
#define BATTERYD_PARSER_H

#include "batteryd_api.h"

int32_t InitBatterydSysfs(void);
int32_t ParseCapacity(int32_t *capacity);
int32_t ParseVoltage(int32_t *voltage);
int32_t ParseTemperature(int32_t *temperature);
int32_t ParseHealthState(int32_t *healthState);
int32_t ParsePluggedType(int32_t *pluggedType);
int32_t ParseChargeState(int32_t *chargeState);
int32_t ParsePresent(int8_t *present);
int32_t ParseTechnology(char *technology, size_t size);
void ParseUeventToBatterydInfo(const char *msg, struct BatterydInfo *info);

#endif // BATTERYD_PARSER_H
