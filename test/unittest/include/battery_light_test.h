/*
 * Copyright (c) 2022 Huawei Device Co., Ltd.
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

#ifndef BATTERY_LIGHT_TEST_H
#define BATTERY_LIGHT_TEST_H

#include <gtest/gtest.h>
#include "accesstoken_kit.h"
#include "nativetoken_kit.h"
#include "token_setproc.h"

namespace OHOS {
namespace PowerMgr {
class BatteryLightTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void TearDown();
private:
    static Security::AccessToken::AccessTokenID tokenID_;
};
} // namespace PowerMgr
} // namespace OHOS
#endif // BATTERY_LIGHT_TEST_H
