/*
 * Copyright (c) 2021-2023 Huawei Device Co., Ltd.
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

#ifndef BATTERY_SRV_BATERY_INFO_H
#define BATTERY_SRV_BATERY_INFO_H

#include <string>

namespace OHOS {
namespace PowerMgr {
static constexpr int32_t INVALID_BATT_INT_VALUE = -1;
static constexpr int32_t INVALID_BATT_TEMP_VALUE = 100;
static constexpr int32_t INVALID_BATT_LEVEL_VALUE = -1;
static constexpr int64_t INVALID_REMAINING_CHARGE_TIME_VALUE = -1;
static constexpr const char* INVALID_STRING_VALUE = "Invalid";
static constexpr bool INVALID_BATT_BOOL_VALUE = true;

/**
 * Type for acquire BatteryChargeState.
 */
enum class BatteryChargeState : uint32_t {
    /**
     * Battery is discharge.
     */
    CHARGE_STATE_NONE,

    /**
     * Battery is charging.
     */
    CHARGE_STATE_ENABLE,

    /**
     * Battery is not charging.
     */
    CHARGE_STATE_DISABLE,

    /**
     * Battery charge full.
     */
    CHARGE_STATE_FULL,

    /**
    * The bottom of the enum.
    */
    CHARGE_STATE_BUTT
};

/**
 * Type for acquire BatteryHealthState.
 */
enum class BatteryHealthState : uint32_t {
    /**
     * Health Status: unknown.
     */
    HEALTH_STATE_UNKNOWN,

    /**
     * Health Status: good.
     */
    HEALTH_STATE_GOOD,

    /**
     * Health Status: over heat.
     */
    HEALTH_STATE_OVERHEAT,

    /**
     * Health Status: over voltage.
     */
    HEALTH_STATE_OVERVOLTAGE,

    /**
     * Health Status: COLD.
     */
    HEALTH_STATE_COLD,

    /**
     * Health Status: Dead.
     */
    HEALTH_STATE_DEAD,

    /**
    * The bottom of the enum.
    */
    HEALTH_STATE_BUTT
};

/**
 * Type for acquire BatteryPluggedType.
 */
enum class BatteryPluggedType : uint32_t {
    /**
     * Power source is unplugged.
     */
    PLUGGED_TYPE_NONE,

    /**
     * Power source is an AC charger.
     */
    PLUGGED_TYPE_AC,

    /**
     * Power source is a USB DC charger.
     */
    PLUGGED_TYPE_USB,

    /**
     * Power source is wireless charger.
     */
    PLUGGED_TYPE_WIRELESS,

    /**
    * The bottom of the enum.
    */
    PLUGGED_TYPE_BUTT
};

/**
 * Battery capacity level of a device
 */
enum class BatteryCapacityLevel : uint32_t {
    /**
     * The battery is in unknown capacity level
     */
    LEVEL_NONE,

    /**
     * The battery is in full capacity level
     */
    LEVEL_FULL,

    /**
     * The battery is in high capacity level
     */
    LEVEL_HIGH,

    /**
     * The battery is in normal capacity level
     */
    LEVEL_NORMAL,

    /**
     * The battery is in low capacity level
     */
    LEVEL_LOW,

    /**
     * The battery is in warning low capacity level
     */
    LEVEL_WARNING,

    /**
     * The battery is in critical low capacity level
     */
    LEVEL_CRITICAL,

    /**
     * The battery is in shutdown low capacity level
     */
    LEVEL_SHUTDOWN,

    /**
    * Reserved
    */
    LEVEL_RESERVED
};

class BatteryInfo {
public:
    enum {
        COMMON_EVENT_CODE_CAPACITY = 0,
        COMMON_EVENT_CODE_VOLTAGE = 1,
        COMMON_EVENT_CODE_TEMPERATURE = 2,
        COMMON_EVENT_CODE_HEALTH_STATE = 3,
        COMMON_EVENT_CODE_PLUGGED_TYPE = 4,
        COMMON_EVENT_CODE_PLUGGED_MAX_CURRENT = 5,
        COMMON_EVENT_CODE_PLUGGED_MAX_VOLTAGE = 6,
        COMMON_EVENT_CODE_CHARGE_STATE = 7,
        COMMON_EVENT_CODE_CHARGE_COUNTER = 8,
        COMMON_EVENT_CODE_PRESENT = 9,
        COMMON_EVENT_CODE_TECHNOLOGY = 10,
        COMMON_EVENT_CODE_CAPACITY_LEVEL = 11,
        COMMON_EVENT_CODE_PLUGGED_NOW_CURRENT = 12,
    };
    BatteryInfo() = default;
    ~BatteryInfo() = default;

    void SetCapacity(const int32_t capacity)
    {
        capacity_ = capacity;
    }

    void SetVoltage(const int32_t voltage)
    {
        voltage_ = voltage;
    }

    void SetTemperature(const int32_t temperature)
    {
        temperature_ = temperature;
    }

    void SetHealthState(const BatteryHealthState healthState)
    {
        healthState_ = healthState;
    }

    void SetPluggedType(const BatteryPluggedType pluggedType)
    {
        pluggedType_ = pluggedType;
    }

    void SetPluggedMaxCurrent(const int32_t maxCurrent)
    {
        pluggedMaxCurrent_ = maxCurrent;
    }

    void SetPluggedMaxVoltage(const int32_t maxVoltage)
    {
        pluggedMaxVoltage_ = maxVoltage;
    }

    void SetChargeState(const BatteryChargeState chargeState)
    {
        chargeState_ = chargeState;
    }

    void SetChargeCounter(const int32_t chargeCounter)
    {
        chargeCounter_ = chargeCounter;
    }

    void SetTotalEnergy(const int32_t totalEnergy)
    {
        totalEnergy_ = totalEnergy;
    }

    void SetCurAverage(const int32_t curAverage)
    {
        curAverage_ = curAverage;
    }

    void SetNowCurrent(const int32_t nowCurr)
    {
        nowCurr_ = nowCurr;
    }

    void SetRemainEnergy(const int32_t remainEnergy)
    {
        remainEnergy_ = remainEnergy;
    }

    void SetPresent(const bool present)
    {
        present_ = present;
    }

    void SetTechnology(const std::string& technology)
    {
        technology_ = technology;
    }

    const int32_t& GetCapacity() const
    {
        return capacity_;
    }

    const int32_t& GetVoltage() const
    {
        return voltage_;
    }

    const int32_t& GetTemperature() const
    {
        return temperature_;
    }

    BatteryHealthState GetHealthState() const
    {
        return healthState_;
    }

    BatteryPluggedType GetPluggedType() const
    {
        return pluggedType_;
    }

    const int32_t& GetPluggedMaxCurrent() const
    {
        return pluggedMaxCurrent_;
    }

    const int32_t& GetPluggedMaxVoltage() const
    {
        return pluggedMaxVoltage_;
    }

    BatteryChargeState GetChargeState() const
    {
        return chargeState_;
    }

    const int32_t& GetTotalEnergy() const
    {
        return totalEnergy_;
    }

    const int32_t& GetCurAverage() const
    {
        return curAverage_;
    }

    const int32_t& GetNowCurrent() const
    {
        return nowCurr_;
    }

    const int32_t& GetRemainEnergy() const
    {
        return remainEnergy_;
    }

    const int32_t& GetChargeCounter() const
    {
        return chargeCounter_;
    }

    bool IsPresent() const
    {
        return present_;
    }

    const std::string& GetTechnology() const
    {
        return technology_;
    }

    bool operator==(const BatteryInfo& info)
    {
        return (present_ == info.IsPresent()) &&
            (capacity_ == info.GetCapacity()) &&
            (voltage_ == info.GetVoltage()) &&
            (temperature_ == info.GetTemperature()) &&
            (totalEnergy_ == info.GetTotalEnergy()) &&
            (curAverage_ == info.GetCurAverage()) &&
            (nowCurr_ == info.GetNowCurrent()) &&
            (pluggedMaxCurrent_ == info.GetPluggedMaxCurrent()) &&
            (pluggedMaxVoltage_ == info.GetPluggedMaxVoltage()) &&
            (chargeCounter_ == info.GetChargeCounter()) &&
            (healthState_ == info.GetHealthState()) &&
            (pluggedType_ == info.GetPluggedType()) &&
            (remainEnergy_ == info.GetRemainEnergy()) &&
            (chargeState_ == info.GetChargeState()) &&
            (technology_ == info.GetTechnology());
    }

    bool operator!=(const BatteryInfo& info)
    {
        return !(*this == info);
    }

    // Used by both napi and native
    static constexpr const char* COMMON_EVENT_KEY_CAPACITY = "soc";
    static constexpr const char* COMMON_EVENT_KEY_CHARGE_STATE = "chargeState";
    static constexpr const char* COMMON_EVENT_KEY_HEALTH_STATE = "healthState";
    static constexpr const char* COMMON_EVENT_KEY_PLUGGED_TYPE = "pluggedType";
    static constexpr const char* COMMON_EVENT_KEY_VOLTAGE = "voltage";
    static constexpr const char* COMMON_EVENT_KEY_TECHNOLOGY = "technology";
    static constexpr const char* COMMON_EVENT_KEY_TEMPERATURE = "temperature";
    static constexpr const char* COMMON_EVENT_KEY_PRESENT = "present";
    static constexpr const char* COMMON_EVENT_KEY_CAPACITY_LEVEL = "capacityLevel";

    // Used by native only
    static constexpr const char* COMMON_EVENT_KEY_PLUGGED_MAX_CURRENT = "maxCurrent";
    static constexpr const char* COMMON_EVENT_KEY_PLUGGED_MAX_VOLTAGE = "maxVoltage";
    static constexpr const char* COMMON_EVENT_KEY_PLUGGED_NOW_CURRENT = "nowCurrent";
    static constexpr const char* COMMON_EVENT_KEY_CHARGE_COUNTER = "chargeCounter";

private:
    bool present_ = INVALID_BATT_BOOL_VALUE;
    int32_t capacity_ = INVALID_BATT_INT_VALUE;
    int32_t voltage_ = INVALID_BATT_INT_VALUE;
    int32_t temperature_ = INVALID_BATT_TEMP_VALUE;
    int32_t totalEnergy_ = INVALID_BATT_INT_VALUE;
    int32_t curAverage_ = INVALID_BATT_INT_VALUE;
    int32_t nowCurr_ = INVALID_BATT_INT_VALUE;
    int32_t pluggedMaxCurrent_ = INVALID_BATT_INT_VALUE;
    int32_t pluggedMaxVoltage_ = INVALID_BATT_INT_VALUE;
    int32_t chargeCounter_ = INVALID_BATT_INT_VALUE;
    int32_t remainEnergy_ = INVALID_BATT_INT_VALUE;
    BatteryHealthState healthState_ = BatteryHealthState::HEALTH_STATE_BUTT;
    BatteryPluggedType pluggedType_ = BatteryPluggedType::PLUGGED_TYPE_BUTT;
    BatteryChargeState chargeState_ = BatteryChargeState::CHARGE_STATE_BUTT;
    std::string technology_ = INVALID_STRING_VALUE;
};
} // namespace PowerMgr
} // namespace OHOS

#endif // BATTERY_SRV_BATERY_INFO_H
