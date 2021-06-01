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

#include "battery_srv_stub.h"
#include "message_parcel.h"
#include "power_common.h"
#include "battery_srv_proxy.h"

namespace OHOS {
namespace PowerMgr {
int BatterySrvStub::OnRemoteRequest(uint32_t code, MessageParcel &data, MessageParcel &reply, MessageOption &option)
{
    POWER_HILOGD(MODULE_BATT_SERVICE, "BatterySrvStub::OnRemoteRequest, cmd = %d, flags = %d", code, option.GetFlags());
    std::u16string descriptor = BatterySrvStub::GetDescriptor();
    std::u16string remoteDescriptor = data.ReadInterfaceToken();
    if (descriptor != remoteDescriptor) {
        POWER_HILOGE(MODULE_SERVICE, "BatterySrvStub::OnRemoteRequest failed, descriptor is not matched!");
        return E_GET_POWER_SERVICE_FAILED;
    }

    switch (code) {
        case static_cast<int>(IBatterySrv::BATT_GET_CAPACITY): {
            return GetCapacityStub(reply);
        }
        case static_cast<int>(IBatterySrv::BATT_GET_CHARGING_STATUS): {
            return GetChargingStatusStub(reply);
        }
        case static_cast<int>(IBatterySrv::BATT_GET_HEALTH_STATUS): {
            return GetHealthStatusStub(reply);
        }
        case static_cast<int>(IBatterySrv::BATT_GET_PLUG_TYPE): {
            return GetPluggedTypeStub(reply);
        }
        case static_cast<int>(IBatterySrv::BATT_GET_VOLTAGE): {
            return GetVoltageStub(reply);
        }
        case static_cast<int>(IBatterySrv::BATT_GET_PRESENT): {
            return GetPresentStub(reply);
        }
        case static_cast<int>(IBatterySrv::BATT_GET_TEMPERATURE): {
            return GetBatteryTemperatureStub(reply);
        }
        case static_cast<int>(IBatterySrv::BATT_GET_TECHNOLOGY): {
            return GetTechnologyStub(reply);
        }
        default: {
            return IPCObjectStub::OnRemoteRequest(code, data, reply, option);
        }
    }
}

int32_t BatterySrvStub::GetCapacityStub(MessageParcel &reply)
{
    int32_t ret = GetCapacity();
    WRITE_PARCEL_WITH_RET(reply, Int32, ret, E_WRITE_PARCEL_ERROR);
    return ERR_OK;
}

int32_t BatterySrvStub::GetChargingStatusStub(MessageParcel &reply)
{
    BatteryChargeState ret = GetChargingStatus();
    WRITE_PARCEL_WITH_RET(reply, Uint32, static_cast<uint32_t>(ret), E_WRITE_PARCEL_ERROR);
    return ERR_OK;
}

int32_t BatterySrvStub::GetHealthStatusStub(MessageParcel &reply)
{
    BatteryHealthState ret = GetHealthStatus();
    WRITE_PARCEL_WITH_RET(reply, Uint32, static_cast<uint32_t>(ret), E_WRITE_PARCEL_ERROR);
    return ERR_OK;
}

int32_t BatterySrvStub::GetPluggedTypeStub(MessageParcel &reply)
{
    BatteryPluggedType ret = GetPluggedType();
    WRITE_PARCEL_WITH_RET(reply, Uint32, static_cast<uint32_t>(ret), E_WRITE_PARCEL_ERROR);
    return ERR_OK;
}

int32_t BatterySrvStub::GetVoltageStub(MessageParcel &reply)
{
    int32_t ret = GetVoltage();
    WRITE_PARCEL_WITH_RET(reply, Int32, ret, E_WRITE_PARCEL_ERROR);
    return ERR_OK;
}

int32_t BatterySrvStub::GetPresentStub(MessageParcel &reply)
{
    bool ret = GetPresent();
    WRITE_PARCEL_WITH_RET(reply, Bool, ret, E_WRITE_PARCEL_ERROR);
    return ERR_OK;
}

int32_t BatterySrvStub::GetTechnologyStub(MessageParcel &reply)
{
    std::string ret = GetTechnology();
    WRITE_PARCEL_WITH_RET(reply, String, ret, E_WRITE_PARCEL_ERROR);
    return ERR_OK;
}

int32_t BatterySrvStub::GetBatteryTemperatureStub(MessageParcel &reply)
{
    int32_t ret = GetBatteryTemperature();
    WRITE_PARCEL_WITH_RET(reply, Int32, ret, E_WRITE_PARCEL_ERROR);
    return ERR_OK;
}
} // namespace PowerMgr
} // namespace OHOS
