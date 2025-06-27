/*
 * Copyright (c) 2025 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 * limitations under the License.
 */

#include "battery_log.h"
#include "battery_xcollie.h"
#include "xcollie/xcollie.h"

namespace OHOS {
namespace PowerMgr {
BatteryXCollie::BatteryXCollie(const std::string &logTag, bool isRecovery)
{
    logTag_ = logTag;
    isCanceled_.store(false, std::memory_order_release);
    const int DFX_DELAY_S = 60;
    unsigned int flag = HiviewDFX::XCOLLIE_FLAG_LOG;
    if (isRecovery) {
        flag = HiviewDFX::XCOLLIE_FLAG_LOG | HiviewDFX::XCOLLIE_FLAG_RECOVERY;
    }
    id_ = HiviewDFX::XCollie::GetInstance().SetTimer(logTag_, DFX_DELAY_S, nullptr, nullptr, flag);
    if (id_ == HiviewDFX::INVALID_ID) {
        BATTERY_HILOGE(COMP_SVC, "Start BatteryXCollie SetTimer fail, tag:%{public}s, timeout(s):%{public}u",
            logTag_.c_str(), DFX_DELAY_S);
        return;
    }
    BATTERY_HILOGD(COMP_SVC, "Start BatteryXCollie, id:%{public}d, tag:%{public}s, timeout(s):%{public}u", id_,
        logTag_.c_str(), DFX_DELAY_S);
}

BatteryXCollie::~BatteryXCollie()
{
    CancelBatteryXCollie();
}

void BatteryXCollie::CancelBatteryXCollie()
{
    if (isCanceled_.load(std::memory_order_acquire)) {
        return;
    }
    if (id_ != HiviewDFX::INVALID_ID) {
        HiviewDFX::XCollie::GetInstance().CancelTimer(id_);
        id_ = HiviewDFX::INVALID_ID;
    }
    isCanceled_.store(true, std::memory_order_release);
    BATTERY_HILOGD(COMP_SVC, "Cancel BatteryXCollie, id:%{public}d, tag:%{public}s", id_, logTag_.c_str());
}

} // namespace PowerMgr
} // namespace OHOS