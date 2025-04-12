/*
 * Copyright (c) 2025 Huawei Device Co., Ltd.
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

#include "battery_charger_test.h"
#include "battery_log.h"

#ifdef GTEST
#define private   public
#define protected public
#endif
#ifdef ENABLE_CHARGER
#include "graphic_dev.h"
#include "power_supply_provider.h"
#include "display_drv.h"
#endif

using namespace testing::ext;

namespace OHOS {
namespace PowerMgr {
namespace {
#ifdef ENABLE_CHARGER
constexpr int32_t MAX_BUFF_SIZE = 128;
#endif
}

/**
 * @tc.name: BatteryCharger001
 * @tc.desc: Test charger
 * @tc.type: FUNC
 * @tc.require: issueIBYDPK
 */
#ifdef ENABLE_CHARGER
HWTEST_F(BatteryChargerTest, BatteryCharger001, TestSize.Level1)
{
    BATTERY_HILOGI(LABEL_TEST, "BatteryCharger001 begin");
    std::unique_ptr<GraphicDev> sfDev_ = std::make_unique<GraphicDev>();
    std::unique_ptr<DisplayDrv> fbdrv_ = nullptr;
    fbdrv_ = sfDev_->MakeDrv(DevType::FB_DEVICE);
    std::unique_ptr<DisplayDrv> drmdrv_ = nullptr;
    drmdrv_ = sfDev_->MakeDrv(DevType::DRM_DEVICE);
    EXPECT_TRUE(fbdrv_ == nullptr || fbdrv_->Init() || sfDev_->GetDevType() != DevType::FB_DEVICE);
    EXPECT_TRUE(drmdrv_ == nullptr || drmdrv_->Init() || sfDev_->GetDevType() != DevType::DRM_DEVICE);
    BATTERY_HILOGI(LABEL_TEST, "BatteryCharger001 end");
}
#endif

/**
 * @tc.name: BatteryCharger002
 * @tc.desc: Test charger
 * @tc.type: FUNC
 * @tc.require: issueIBYDPK
 */
#ifdef ENABLE_CHARGER
HWTEST_F(BatteryChargerTest, BatteryCharger002, TestSize.Level1)
{
    BATTERY_HILOGI(LABEL_TEST, "BatteryCharger002 begin");
    std::unique_ptr<PowerSupplyProvider> provider_ = std::make_unique<PowerSupplyProvider>();
    provider_->InitBatteryPath();
    provider_->InitPowerSupplySysfs();
    int32_t temperature = 0;
    EXPECT_EQ(provider_->ParseTemperature(&temperature), HDF_SUCCESS);
    int32_t capacity_ = 0;
    EXPECT_EQ(provider_->ParseCapacity(&capacity_), HDF_SUCCESS);
    int32_t chargeState_ = 0;
    EXPECT_EQ(provider_->ParseChargeState(&chargeState_), HDF_SUCCESS);
    char buf[MAX_BUFF_SIZE] = {0};
    EXPECT_NE(provider_->ReadBatterySysfsToBuff("", buf, sizeof(buf)), HDF_SUCCESS);
    BATTERY_HILOGI(LABEL_TEST, "BatteryCharger002 end");
}
#endif
} // namespace PowerMgr
} // namespace OHOS
