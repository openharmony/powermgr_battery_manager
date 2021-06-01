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

#include "batteryd_dispatcher.h"

#include "batteryd.h"
#include "batteryd_publisher.h"
#include "batteryd_parser.h"
#include "osal/osal_mem.h"
#include "utils/hdf_log.h"

#define HDF_LOG_TAG batteryd_dispatcher
#define MAX_BUFF_SIZE 128

static int32_t DispatchBindBatterySubscriber(struct BatterydService *service, struct HdfSBuf *data);
static int32_t DispatchUnbindBatterySubscriber(struct BatterydService *service);
static int32_t DispatchGetCapacity(struct HdfSBuf *reply);
static int32_t DispatchGetVoltage(struct HdfSBuf *reply);
static int32_t DispatchGetTemperature(struct HdfSBuf *reply);
static int32_t DispatchGetHealthState(struct HdfSBuf *reply);
static int32_t DispatchGetPluggedType(struct HdfSBuf *reply);
static int32_t DispatchGetChargeState(struct HdfSBuf *reply);
static int32_t DispatchGetPresent(struct HdfSBuf *reply);
static int32_t DispatchGetTechnology(struct HdfSBuf *reply);

int32_t BatterydDispatch(struct HdfDeviceIoClient *client, int cmdId, struct HdfSBuf *data, struct HdfSBuf *reply)
{
    HDF_LOGD("%{public}s enter", __func__);
    if (client == NULL || client->device == NULL) {
        HDF_LOGE("%{public}s: client or client->device is NULL", __func__);
        return HDF_ERR_INVALID_PARAM;
    }

    struct BatterydService *service = (struct BatterydService *)client->device->service;
    if (service == NULL) {
        HDF_LOGE("%{public}s: service is NULL", __func__);
        return HDF_FAILURE;
    }

    switch (cmdId) {
        case CMD_BIND_BATTERY_SUBSCRIBER: {
            return DispatchBindBatterySubscriber(service, data);
        }
        case CMD_UNBIND_BATTERY_SUBSCRIBER: {
            return DispatchUnbindBatterySubscriber(service);
        }
        case CMD_GET_CAPACITY: {
            return DispatchGetCapacity(reply);
        }
        case CMD_GET_VOLTAGE: {
            return DispatchGetVoltage(reply);
        }
        case CMD_GET_TEMPERATURE: {
            return DispatchGetTemperature(reply);
        }
        case CMD_GET_HEALTH_STATE: {
            return DispatchGetHealthState(reply);
        }
        case CMD_GET_PLUGGED_TYPE: {
            return DispatchGetPluggedType(reply);
        }
        case CMD_GET_CHARGE_STATE: {
            return DispatchGetChargeState(reply);
        }
        case CMD_GET_PRESENT: {
            return DispatchGetPresent(reply);
        }
        case CMD_GET_TECHNOLOGY: {
            return DispatchGetTechnology(reply);
        }
        default: {
            HDF_LOGE("%{public}s: invalid cmdId %{public}d", __func__, cmdId);
            return HDF_FAILURE;
        }
    }
}

static int32_t DispatchBindBatterySubscriber(struct BatterydService *service, struct HdfSBuf *data)
{
    struct BatterydSubscriber *subscriber = NULL;
    struct HdfRemoteService *remoteService = HdfSBufReadRemoteService(data);
    if (remoteService == NULL) {
        HDF_LOGW("%{public}s: remoteService is NULL", __func__);
        return HDF_ERR_INVALID_PARAM;
    }

    subscriber = (struct BatterydSubscriber *)OsalMemCalloc(sizeof(*subscriber));
    if (subscriber == NULL) {
        HDF_LOGE("%{public}s: calloc subscriber error", __func__);
        return HDF_ERR_MALLOC_FAIL;
    }
    subscriber->remoteService = remoteService;
    return BindBatterySubscriber(service, subscriber);
}

static int32_t DispatchUnbindBatterySubscriber(struct BatterydService *service)
{
    return UnbindBatterySubscriber(service);
}

static int32_t DispatchGetCapacity(struct HdfSBuf *reply)
{
    int32_t ret;
    int32_t capacity = 0;
    ret = ParseCapacity(&capacity);
    if (ret != HDF_SUCCESS) {
        return ret;
    }
    HdfSbufWriteInt32(reply, capacity);
    return HDF_SUCCESS;
}

static int32_t DispatchGetVoltage(struct HdfSBuf *reply)
{
    int32_t ret;
    int32_t voltage = 0;
    ret = ParseVoltage(&voltage);
    if (ret != HDF_SUCCESS) {
        return ret;
    }
    HdfSbufWriteInt32(reply, voltage);
    return HDF_SUCCESS;
}

static int32_t DispatchGetTemperature(struct HdfSBuf *reply)
{
    int32_t ret;
    int32_t temperature = 0;
    ret = ParseTemperature(&temperature);
    if (ret != HDF_SUCCESS) {
        return ret;
    }
    HdfSbufWriteInt32(reply, temperature);
    return HDF_SUCCESS;
}

static int32_t DispatchGetHealthState(struct HdfSBuf *reply)
{
    int32_t ret;
    int32_t healthState = 0;
    ret = ParseHealthState(&healthState);
    if (ret != HDF_SUCCESS) {
        return ret;
    }
    HdfSbufWriteInt32(reply, healthState);
    return HDF_SUCCESS;
}

static int32_t DispatchGetPluggedType(struct HdfSBuf *reply)
{
    int32_t ret;
    int32_t pluggedType = 0;
    ret = ParsePluggedType(&pluggedType);
    if (ret != HDF_SUCCESS) {
        return ret;
    }
    HdfSbufWriteInt32(reply, pluggedType);
    return HDF_SUCCESS;
}

static int32_t DispatchGetChargeState(struct HdfSBuf *reply)
{
    int32_t ret;
    int32_t chargeState = 0;
    ret = ParseChargeState(&chargeState);
    if (ret != HDF_SUCCESS) {
        return ret;
    }
    HdfSbufWriteInt32(reply, chargeState);
    return HDF_SUCCESS;
}

static int32_t DispatchGetPresent(struct HdfSBuf *reply)
{
    int32_t ret;
    int8_t present = 0;
    ret = ParsePresent(&present);
    if (ret != HDF_SUCCESS) {
        return ret;
    }
    HdfSbufWriteInt8(reply, present);
    return HDF_SUCCESS;
}

static int32_t DispatchGetTechnology(struct HdfSBuf *reply)
{
    int32_t ret;
    char technology[MAX_BUFF_SIZE] = {0};
    ret = ParseTechnology(technology, sizeof(technology));
    if (ret != HDF_SUCCESS) {
        return ret;
    }
    HdfSbufWriteString(reply, technology);
    return HDF_SUCCESS;
}
