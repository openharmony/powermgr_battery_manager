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

#ifndef BATTERY_THREAD_H
#define BATTERY_THREAD_H

#include <thread>
#include <vector>
#include <memory>
#include <map>

#include "batteryd.h"
#include "power_supply_provider.h"

namespace OHOS {
namespace HDI {
namespace Battery {
namespace V1_0 {
enum EventType {
    EVENT_UEVENT_FD,
    EVENT_TIMER_FD,
};

class BatteryThread {
public:
    virtual ~BatteryThread() {}

    void StartThread(void *service);
protected:
    int LoopingThreadEntry(void *arg);
    virtual void Run(void *service);
    virtual void UpdateBatteryInfo(void *service, char *msg);
    virtual void UpdateBatteryInfo(void *service);
    virtual void HandleStates() {}
    virtual int UpdateWaitInterval();
    void UpdateEpollInterval(const int32_t chargestate);
    virtual void CycleMatters() {}
private:
    int32_t OpenUeventSocket(void) const;
    bool IsPowerSupplyEvent(const char *msg) const;
    int32_t Init(void *service);
    int32_t InitUevent();
    int32_t InitTimer();
    int32_t InitBacklightTimer();
    void TimerCallback(void *service);
    void UeventCallback(void *service);
    void SetTimerInterval(int interval);
    int RegisterCallback(const int fd, const EventType et);
    int timerInterval_ = -1;
    int32_t ueventFd_ = -1;
    int32_t timerFd_ = -1;
    int32_t epFd_ = -1;
    int epollInterval_ = -1;
    using Callback = std::function<void(BatteryThread*, void*)>;
    std::map<int32_t, Callback> callbacks_;
    std::unique_ptr<PowerSupplyProvider> provider_ = nullptr;
};
}  // namespace V1_0
}  // namespace Battery
}  // namespace HDI
}  // namespace OHOS

#endif // BATTERY_THREAD_H
