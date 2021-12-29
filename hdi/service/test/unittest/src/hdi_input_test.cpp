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

#include "hdi_input_test.h"

#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <string>
#include <unistd.h>
#include <fcntl.h>
#include <gtest/gtest.h>
#include <securec.h>
#include "osal_time.h"
#include "hdf_log.h"
#include "input_manager.h"

using namespace testing::ext;

namespace HdiInputTest {
IInputInterface* g_inputInterface;
InputEventCb g_callback;
InputHostCb g_hotplugCb;

const int INIT_DEFAULT_VALUE = 255;
const int KEEP_ALIVE_TIME_MS = 10000;
const int TOUCH_INDEX = 1;
const int INVALID_INDEX = 5;
const int NAME_MAX_LEN = 10;
const int TEST_RESULT_LEN = 32;
const int MAX_DEVICES = 32;

void HdiInputTest::SetUpTestCase(void)
{
    int32_t ret = GetInputInterface(&g_inputInterface);
    if (ret != INPUT_SUCCESS) {
        HDF_LOGE("%s: get input hdi failed, ret %d", __func__, ret);
    }
}

void HdiInputTest::TearDownTestCase(void)
{
    if (g_inputInterface != NULL) {
        free(g_inputInterface);
        g_inputInterface = NULL;
    }
}

void HdiInputTest::SetUp(void)
{
}

void HdiInputTest::TearDown(void)
{
}

template<typename T1, typename T2>
inline void InputCheckNullPointer(const T1& pointer, const T2& ret)
{
    do {
        if ((pointer) == NULL) {
            HDF_LOGE("%s: null pointer", __func__);
            ASSERT_EQ ((ret), INPUT_SUCCESS);
        }
    }while (0);
}

static void ReportEventPkgCallback(const EventPackage** pkgs, uint32_t count, uint32_t devIndex)
{
    if (pkgs == NULL) {
        return;
    }
    for (uint32_t i = 0; i < count; i++) {
        HDF_LOGI("%s: pkgs[%d] = 0x%x, 0x%x, %d", __func__, i, pkgs[i]->type, pkgs[i]->code, pkgs[i]->value);
    }
    if (devIndex == 0) {
        HDF_LOGI("%u:", devIndex);
    }
}

void ReportHotPlugEventPkgCallback(const HotPlugEvent* msg)
{
    if (msg == NULL) {
        return;
    }
    HDF_LOGI("%s: status =%d devId=%d type =%d", __func__, msg->status, msg->devIndex, msg->devType);
}

HWTEST_F (HdiInputTest, ScanInputDevice, TestSize.Level1)
{
    DevDesc sta[MAX_DEVICES] = {{0}};

    HDF_LOGI("%s: [Input] RegisterCallbackAndReportData001 enter", __func__);
    int32_t ret;

    InputCheckNullPointer(g_inputInterface, INPUT_NULL_PTR);
    InputCheckNullPointer(g_inputInterface->iInputManager, INPUT_NULL_PTR);

    ret  = g_inputInterface->iInputManager->ScanInputDevice(sta, sizeof(sta)/sizeof(DevDesc));
    if (!ret) {
        HDF_LOGI("%s:%d, %d, %d, %d", __func__, sta[0].devType, sta[0].devIndex, sta[1].devType, sta[1].devIndex);
    }

    EXPECT_EQ(ret, INPUT_SUCCESS);
}

/**
  * @tc.name: OpenInputDev001
  * @tc.desc: open input device test
  * @tc.type: FUNC
  */
HWTEST_F (HdiInputTest, OpenInputDev001, TestSize.Level1)
{
    HDF_LOGI("%s: [Input] OpenInputDev001 enter", __func__);
    InputCheckNullPointer(g_inputInterface, INPUT_NULL_PTR);
    InputCheckNullPointer(g_inputInterface->iInputManager, INPUT_NULL_PTR);
    int32_t ret = g_inputInterface->iInputManager->OpenInputDevice(TOUCH_INDEX);
    if (ret) {
        HDF_LOGE("%s: open device1 failed, ret %d", __func__, ret);
    }
    EXPECT_EQ(ret, INPUT_SUCCESS);
}

/**
  * @tc.name: OpenInputDevice002
  * @tc.desc: open input device test
  * @tc.type: FUNC
  */
HWTEST_F (HdiInputTest, OpenInputDevice002, TestSize.Level1)
{
    HDF_LOGI("%s: [Input] OpenInputDev002 enter", __func__);
    InputCheckNullPointer(g_inputInterface, INPUT_NULL_PTR);
    InputCheckNullPointer(g_inputInterface->iInputManager, INPUT_NULL_PTR);
    /* Device "5" is used for testing nonexistent device node */
    int32_t ret = g_inputInterface->iInputManager->OpenInputDevice(INVALID_INDEX);
    if (ret) {
        HDF_LOGE("%s: device5 dose not exist, can't open it, ret %d", __func__, ret);
    }
    EXPECT_NE(ret, INPUT_SUCCESS);
}

/**
  * @tc.name: CloseInputDevice001
  * @tc.desc: close input device test
  * @tc.type: FUNC
  */
HWTEST_F (HdiInputTest, CloseInputDevice001, TestSize.Level1)
{
    HDF_LOGI("%s: [Input] CloseInputDev001 enter", __func__);
    InputCheckNullPointer(g_inputInterface, INPUT_NULL_PTR);
    InputCheckNullPointer(g_inputInterface->iInputManager, INPUT_NULL_PTR);
    int32_t ret = g_inputInterface->iInputManager->CloseInputDevice(TOUCH_INDEX);
    if (ret) {
        HDF_LOGE("%s: close device1 failed, ret %d", __func__, ret);
    }
    EXPECT_EQ(ret, INPUT_SUCCESS);
}

/**
  * @tc.name: CloseInputDevice002
  * @tc.desc: close input device test
  * @tc.type: FUNC
  */
HWTEST_F (HdiInputTest, CloseInputDevice002, TestSize.Level1)
{
    HDF_LOGI("%s: [Input] CloseInputDev002 enter", __func__);
    InputCheckNullPointer(g_inputInterface, INPUT_NULL_PTR);
    InputCheckNullPointer(g_inputInterface->iInputManager, INPUT_NULL_PTR);
    /* Device "5" is used for testing nonexistent device node */
    int32_t ret = g_inputInterface->iInputManager->CloseInputDevice(INVALID_INDEX);
    if (ret) {
        HDF_LOGE("%s: device5 doesn't exist, can't close it, ret %d", __func__, ret);
    }
    EXPECT_NE(ret, INPUT_SUCCESS);
}

/**
  * @tc.name: GetInputDevice001
  * @tc.desc: get input device info test
  * @tc.type: FUNC
  */
HWTEST_F (HdiInputTest, GetInputDevice001, TestSize.Level1)
{
    HDF_LOGI("%s: [Input] GetInputDevice001 enter", __func__);
    DeviceInfo* dev = NULL;
    InputCheckNullPointer(g_inputInterface, INPUT_NULL_PTR);
    InputCheckNullPointer(g_inputInterface->iInputManager, INPUT_NULL_PTR);

    int32_t ret = g_inputInterface->iInputManager->OpenInputDevice(TOUCH_INDEX);
    if (ret) {
        HDF_LOGE("%s: open device1 failed, ret %d", __func__, ret);
    }
    ASSERT_EQ(ret, INPUT_SUCCESS);

    ret = g_inputInterface->iInputManager->GetInputDevice(TOUCH_INDEX, &dev);
    if (ret) {
        HDF_LOGE("%s: get device1 failed, ret %d", __func__, ret);
    }

    HDF_LOGI("%s: devindex = %u, devType = %u", __func__, dev->devIndex,
            dev->devType);
    HDF_LOGI("%s: chipInfo = %s, vendorName = %s, chipName = %s",
        __func__, dev->chipInfo, dev->vendorName, dev->chipName);
    EXPECT_EQ(ret, INPUT_SUCCESS);
}

/**
  * @tc.name: GetInputDeviceList001
  * @tc.desc: get input device list info test
  * @tc.type: FUNC
  */
HWTEST_F (HdiInputTest, GetInputDeviceList001, TestSize.Level1)
{
    HDF_LOGI("%s: [Input] GetInputDeviceList001 enter", __func__);
    int32_t ret;
    uint32_t num = 0;
    DeviceInfo* dev[MAX_INPUT_DEV_NUM] = {0};

    InputCheckNullPointer(g_inputInterface, INPUT_NULL_PTR);
    InputCheckNullPointer(g_inputInterface->iInputManager, INPUT_NULL_PTR);
    ret = g_inputInterface->iInputManager->GetInputDeviceList(&num, dev, MAX_INPUT_DEV_NUM);
    if (ret) {
        HDF_LOGE("%s: get device list failed, ret %d", __func__, ret);
    }
    ASSERT_LE(int32_t(num), MAX_INPUT_DEV_NUM);  /* num <= MAX_INPUT_DEV_NUM return true */

    for (uint32_t i = 0; i < num; i++) {
        HDF_LOGI("%s: num = %u, device[%d]'s info is:", __func__, num, i);
        HDF_LOGI("%s: index = %u, devType = %u", __func__, dev[i]->devIndex,
                dev[i]->devType);
        HDF_LOGI("%s: chipInfo = %s, vendorName = %s, chipName = %s",
            __func__, dev[i]->chipInfo, dev[i]->vendorName, dev[i]->chipName);
    }
    EXPECT_EQ(ret, INPUT_SUCCESS);
}

/**
  * @tc.name: GetDeviceType001
  * @tc.desc: get input device type test
  * @tc.type: FUNC
  */
HWTEST_F (HdiInputTest, GetDeviceType001, TestSize.Level1)
{
    HDF_LOGI("%s: [Input] GetDeviceType001 enter", __func__);
    int32_t ret;
    uint32_t devType = INIT_DEFAULT_VALUE;

    InputCheckNullPointer(g_inputInterface, INPUT_NULL_PTR);
    InputCheckNullPointer(g_inputInterface->iInputController, INPUT_NULL_PTR);
    ret = g_inputInterface->iInputController->GetDeviceType(TOUCH_INDEX, &devType);
    if (ret) {
        HDF_LOGE("%s: get device1's type failed, ret %d", __func__, ret);
    }
    HDF_LOGI("%s: device1's type is %u", __func__, devType);
    EXPECT_EQ(ret, INPUT_SUCCESS);
}

/**
  * @tc.name: GetChipInfo001
  * @tc.desc: get input device chip info test
  * @tc.type: FUNC
  */
HWTEST_F (HdiInputTest, GetChipInfo001, TestSize.Level1)
{
    HDF_LOGI("%s: [Input] GetChipInfo001 enter", __func__);
    int32_t ret;
    char chipInfo[CHIP_INFO_LEN] = {0};

    InputCheckNullPointer(g_inputInterface, INPUT_NULL_PTR);
    InputCheckNullPointer(g_inputInterface->iInputController, INPUT_NULL_PTR);
    ret = g_inputInterface->iInputController->GetChipInfo(TOUCH_INDEX, chipInfo, CHIP_INFO_LEN);
    if (ret) {
        HDF_LOGE("%s: get device1's chip info failed, ret %d", __func__, ret);
    }
    HDF_LOGI("%s: device1's chip info is %s", __func__, chipInfo);
    EXPECT_EQ(ret, INPUT_SUCCESS);
}

/**
  * @tc.name: GetInputDevice002
  * @tc.desc: get input device chip info test
  * @tc.type: FUNC
  */
HWTEST_F (HdiInputTest, GetInputDevice002, TestSize.Level1)
{
    HDF_LOGI("%s: [Input] GetInputDevice002 enter", __func__);
    int32_t ret;
    DeviceInfo* dev = NULL;

    InputCheckNullPointer(g_inputInterface, INPUT_NULL_PTR);
    InputCheckNullPointer(g_inputInterface->iInputManager, INPUT_NULL_PTR);
    ret = g_inputInterface->iInputManager->GetInputDevice(TOUCH_INDEX, &dev);
    if (ret) {
        HDF_LOGE("%s: get device1 failed, ret %d", __func__, ret);
    }

    HDF_LOGI("%s: After fill the info, new device0's info is:", __func__);
    HDF_LOGI("%s: new devindex = %u, devType = %u", __func__, dev->devIndex,
            dev->devType);
    HDF_LOGI("%s: new chipInfo = %s, vendorName = %s, chipName = %s",
        __func__, dev->chipInfo, dev->vendorName, dev->chipName);
    EXPECT_EQ(ret, INPUT_SUCCESS);
}

/**
  * @tc.name: RegisterCallback001
  * @tc.desc: get input device chip info test
  * @tc.type: FUNC
  */
HWTEST_F (HdiInputTest, RegisterCallback001, TestSize.Level1)
{
    HDF_LOGI("%s: [Input] RegisterCallbac001 enter", __func__);
    int32_t ret;
    g_callback.EventPkgCallback = ReportEventPkgCallback;

    InputCheckNullPointer(g_inputInterface, INPUT_NULL_PTR);
    InputCheckNullPointer(g_inputInterface->iInputReporter, INPUT_NULL_PTR);
    /* Device "5" is used for testing nonexistent device node */
    ret  = g_inputInterface->iInputReporter->RegisterReportCallback(INVALID_INDEX, &g_callback);
    if (ret) {
        HDF_LOGE("%s: device2 dose not exist, can't register callback to it, ret %d", __func__, ret);
    }
    EXPECT_NE(ret, INPUT_SUCCESS);
}

/**
  * @tc.name: SetPowerStatus001
  * @tc.desc: set device power status test
  * @tc.type: FUNC
  */
HWTEST_F (HdiInputTest, SetPowerStatus001, TestSize.Level1)
{
    HDF_LOGI("%s: [Input] SetPowerStatus001 enter", __func__);
    int32_t ret;
    uint32_t setStatus = INPUT_LOW_POWER;

    InputCheckNullPointer(g_inputInterface, INPUT_NULL_PTR);
    InputCheckNullPointer(g_inputInterface->iInputController, INPUT_NULL_PTR);
    ret = g_inputInterface->iInputController->SetPowerStatus(TOUCH_INDEX, setStatus);
    if (ret) {
        HDF_LOGE("%s: set device1's power status failed, ret %d", __func__, ret);
    }
    EXPECT_EQ(ret, INPUT_SUCCESS);
}

/**
  * @tc.name: SetPowerStatus002
  * @tc.desc: set device power status test
  * @tc.type: FUNC
  */
HWTEST_F (HdiInputTest, SetPowerStatus002, TestSize.Level1)
{
    HDF_LOGI("%s: [Input] SetPowerStatus002 enter", __func__);
    int32_t ret;
    uint32_t setStatus = INPUT_LOW_POWER;

    InputCheckNullPointer(g_inputInterface, INPUT_NULL_PTR);
    InputCheckNullPointer(g_inputInterface->iInputController, INPUT_NULL_PTR);
    /* Device "5" is used for testing nonexistent device node */
    ret = g_inputInterface->iInputController->SetPowerStatus(INVALID_INDEX, setStatus);
    if (ret) {
        HDF_LOGE("%s: set device5's power status failed, ret %d", __func__, ret);
    }
    EXPECT_NE(ret, INPUT_SUCCESS);
}

/**
  * @tc.name: GetPowerStatus001
  * @tc.desc: get device power status test
  * @tc.type: FUNC
  */
HWTEST_F (HdiInputTest, GetPowerStatus001, TestSize.Level1)
{
    HDF_LOGI("%s: [Input] GetPowerStatus001 enter", __func__);
    int32_t ret;
    uint32_t getStatus = 0;

    InputCheckNullPointer(g_inputInterface, INPUT_NULL_PTR);
    InputCheckNullPointer(g_inputInterface->iInputController, INPUT_NULL_PTR);
    ret = g_inputInterface->iInputController->GetPowerStatus(TOUCH_INDEX, &getStatus);
    if (ret) {
        HDF_LOGE("%s: get device1's power status failed, ret %d", __func__, ret);
    }
    HDF_LOGI("%s: device1's power status is %d:", __func__, getStatus);
    EXPECT_EQ(ret, INPUT_SUCCESS);
}

/**
  * @tc.name: GetPowerStatus002
  * @tc.desc: get device power status test
  * @tc.type: FUNC
  */
HWTEST_F (HdiInputTest, GetPowerStatus002, TestSize.Level1)
{
    HDF_LOGI("%s: [Input] GetPowerStatus002 enter", __func__);
    int32_t ret;
    uint32_t getStatus = 0;

    InputCheckNullPointer(g_inputInterface, INPUT_NULL_PTR);
    InputCheckNullPointer(g_inputInterface->iInputController, INPUT_NULL_PTR);
    /* Device "5" is used for testing nonexistent device node */
    ret = g_inputInterface->iInputController->GetPowerStatus(INVALID_INDEX, &getStatus);
    if (ret) {
        HDF_LOGE("%s: get device5's power status failed, ret %d", __func__, ret);
    }
    EXPECT_NE(ret, INPUT_SUCCESS);
}

/**
  * @tc.name: GetVendorName001
  * @tc.desc: get device vendor name test
  * @tc.type: FUNC
  */
HWTEST_F (HdiInputTest, GetVendorName001, TestSize.Level1)
{
    HDF_LOGI("%s: [Input] GetVendorName001 enter", __func__);
    int32_t ret;
    char vendorName[NAME_MAX_LEN] = {0};

    InputCheckNullPointer(g_inputInterface, INPUT_NULL_PTR);
    InputCheckNullPointer(g_inputInterface->iInputController, INPUT_NULL_PTR);
    ret = g_inputInterface->iInputController->GetVendorName(TOUCH_INDEX, vendorName, NAME_MAX_LEN);
    if (ret) {
        HDF_LOGE("%s: get device1's vendor name failed, ret %d", __func__, ret);
    }
    HDF_LOGI("%s: device1's vendor name is %s:", __func__, vendorName);
    EXPECT_EQ(ret, INPUT_SUCCESS);
}

/**
  * @tc.name: GetVendorName002
  * @tc.desc: get device vendor name test
  * @tc.type: FUNC
  */
HWTEST_F (HdiInputTest, GetVendorName002, TestSize.Level1)
{
    HDF_LOGI("%s: [Input] GetVendorName002 enter", __func__);
    int32_t ret;
    char vendorName[NAME_MAX_LEN] = {0};

    InputCheckNullPointer(g_inputInterface, INPUT_NULL_PTR);
    InputCheckNullPointer(g_inputInterface->iInputController, INPUT_NULL_PTR);
    /* Device "5" is used for testing nonexistent device node */
    ret = g_inputInterface->iInputController->GetVendorName(INVALID_INDEX, vendorName, NAME_MAX_LEN);
    if (ret) {
        HDF_LOGE("%s: get device5's vendor name failed, ret %d", __func__, ret);
    }
    EXPECT_NE(ret, INPUT_SUCCESS);
}

/**
  * @tc.name: GetChipName001
  * @tc.desc: get device chip name test
  * @tc.type: FUNC
  */
HWTEST_F (HdiInputTest, GetChipName001, TestSize.Level1)
{
    HDF_LOGI("%s: [Input] GetChipName001 enter", __func__);
    int32_t ret;
    char chipName[NAME_MAX_LEN] = {0};

    InputCheckNullPointer(g_inputInterface, INPUT_NULL_PTR);
    InputCheckNullPointer(g_inputInterface->iInputController, INPUT_NULL_PTR);
    ret = g_inputInterface->iInputController->GetChipName(TOUCH_INDEX, chipName, NAME_MAX_LEN);
    if (ret) {
        HDF_LOGE("%s: get device1's chip name failed, ret %d", __func__, ret);
    }
    HDF_LOGI("%s: device1's chip name is %s", __func__, chipName);
    EXPECT_EQ(ret, INPUT_SUCCESS);
}

/**
  * @tc.name: GetChipName002
  * @tc.desc: get device chip name test
  * @tc.type: FUNC
  */
HWTEST_F (HdiInputTest, GetChipName002, TestSize.Level1)
{
    HDF_LOGI("%s: [Input] GetChipName002 enter", __func__);
    int32_t ret;
    char chipName[NAME_MAX_LEN] = {0};

    InputCheckNullPointer(g_inputInterface, INPUT_NULL_PTR);
    InputCheckNullPointer(g_inputInterface->iInputController, INPUT_NULL_PTR);
    /* Device "5" is used for testing nonexistent device node */
    ret = g_inputInterface->iInputController->GetChipName(INVALID_INDEX, chipName, NAME_MAX_LEN);
    if (ret) {
        HDF_LOGE("%s: get device5's chip name failed, ret %d", __func__, ret);
    }
    EXPECT_NE(ret, INPUT_SUCCESS);
}

/**
  * @tc.name: SetGestureMode001
  * @tc.desc: set device gesture mode test
  * @tc.type: FUNC
  */
HWTEST_F (HdiInputTest, SetGestureMode001, TestSize.Level1)
{
    HDF_LOGI("%s: [Input] SetGestureMode001 enter", __func__);
    int32_t ret;
    uint32_t gestureMode = 1;

    InputCheckNullPointer(g_inputInterface, INPUT_NULL_PTR);
    InputCheckNullPointer(g_inputInterface->iInputController, INPUT_NULL_PTR);
    ret = g_inputInterface->iInputController->SetGestureMode(TOUCH_INDEX, gestureMode);
    if (ret) {
        HDF_LOGE("%s: get device1's gestureMode failed, ret %d", __func__, ret);
    }
    EXPECT_EQ(ret, INPUT_SUCCESS);
}

/**
  * @tc.name: SetGestureMode002
  * @tc.desc: set device gesture mode test
  * @tc.type: FUNC
  */
HWTEST_F (HdiInputTest, SetGestureMode002, TestSize.Level1)
{
    HDF_LOGI("%s: [Input] SetGestureMode001 enter", __func__);
    int32_t ret;
    uint32_t gestureMode = 1;

    InputCheckNullPointer(g_inputInterface, INPUT_NULL_PTR);
    InputCheckNullPointer(g_inputInterface->iInputController, INPUT_NULL_PTR);
    /* Device "5" is used for testing nonexistent device node */
    ret = g_inputInterface->iInputController->SetGestureMode(INVALID_INDEX, gestureMode);
    if (ret) {
        HDF_LOGE("%s: get device1's gestureMode failed, ret %d", __func__, ret);
    }
    EXPECT_NE(ret, INPUT_SUCCESS);
}

/**
  * @tc.name: RunCapacitanceTest001
  * @tc.desc: set device gesture mode test
  * @tc.type: FUNC
  */
HWTEST_F (HdiInputTest, RunCapacitanceTest001, TestSize.Level1)
{
    HDF_LOGI("%s: [Input] RunCapacitanceTest001 enter", __func__);
    int32_t ret;
    char result[TEST_RESULT_LEN] = {0};
    uint32_t testType = MMI_TEST;

    InputCheckNullPointer(g_inputInterface, INPUT_NULL_PTR);
    InputCheckNullPointer(g_inputInterface->iInputController, INPUT_NULL_PTR);
    ret = g_inputInterface->iInputController->RunCapacitanceTest(TOUCH_INDEX, testType, result, TEST_RESULT_LEN);
    if (ret) {
        HDF_LOGE("%s: get device1's gestureMode failed, ret %d", __func__, ret);
    }
    EXPECT_EQ(ret, INPUT_SUCCESS);
}

/**
  * @tc.name: RunCapacitanceTest001
  * @tc.desc: set device gesture mode test
  * @tc.type: FUNC
  */
HWTEST_F (HdiInputTest, RunExtraCommand001, TestSize.Level1)
{
    HDF_LOGI("%s: [Input] RunExtraCommand001 enter", __func__);
    int32_t ret;
    InputExtraCmd extraCmd = {0};
    extraCmd.cmdCode = "WakeUpMode";
    extraCmd.cmdValue = "Enable";

    InputCheckNullPointer(g_inputInterface, INPUT_NULL_PTR);
    InputCheckNullPointer(g_inputInterface->iInputController, INPUT_NULL_PTR);
    ret = g_inputInterface->iInputController->RunExtraCommand(TOUCH_INDEX, &extraCmd);
    if (ret) {
        HDF_LOGE("%s: get device1's gestureMode failed, ret %d", __func__, ret);
    }
    EXPECT_EQ(ret, INPUT_SUCCESS);
}

/**
  * @tc.name: RegisterCallbackAndReportData001
  * @tc.desc: get input device chip info test
  * @tc.type: FUNC
  */
HWTEST_F (HdiInputTest, RegisterCallbackAndReportData001, TestSize.Level1)
{
    HDF_LOGI("%s: [Input] RegisterCallbackAndReportData001 enter", __func__);
    int32_t ret;
    g_callback.EventPkgCallback = ReportEventPkgCallback;
    g_hotplugCb.HotPlugCallback = ReportHotPlugEventPkgCallback;

    InputCheckNullPointer(g_inputInterface, INPUT_NULL_PTR);
    InputCheckNullPointer(g_inputInterface->iInputReporter, INPUT_NULL_PTR);
    InputCheckNullPointer(g_inputInterface->iInputManager, INPUT_NULL_PTR);

    ret  = g_inputInterface->iInputReporter->RegisterReportCallback(TOUCH_INDEX, &g_callback);
    if (ret) {
        HDF_LOGE("%s: register callback failed for device 1, ret %d", __func__, ret);
    }

    ret  = g_inputInterface->iInputReporter->RegisterHotPlugCallback(&g_hotplugCb);
    if (ret) {
        HDF_LOGE("%s: register callback failed for device manager, ret %d", __func__, ret);
    }

    EXPECT_EQ(ret, INPUT_SUCCESS);
    HDF_LOGI("%s: wait 10s for testing, pls touch the panel now", __func__);
    HDF_LOGI("%s: The event data is as following:", __func__);
    OsalMSleep(KEEP_ALIVE_TIME_MS);
}

/**
  * @tc.name: UnregisterReportCallback001
  * @tc.desc: get input device chip info test
  * @tc.type: FUNC
  */
HWTEST_F (HdiInputTest, UnregisterReportCallback001, TestSize.Level1)
{
    HDF_LOGI("%s: [Input] UnregisterReportCallback001 enter", __func__);
    int32_t ret;
    InputCheckNullPointer(g_inputInterface, INPUT_NULL_PTR);
    InputCheckNullPointer(g_inputInterface->iInputReporter, INPUT_NULL_PTR);
    InputCheckNullPointer(g_inputInterface->iInputManager, INPUT_NULL_PTR);

    ret  = g_inputInterface->iInputReporter->UnregisterReportCallback(TOUCH_INDEX);
    if (ret) {
        HDF_LOGE("%s: unregister callback failed for device1, ret %d", __func__, ret);
    }
    EXPECT_EQ(ret, INPUT_SUCCESS);

    ret = g_inputInterface->iInputManager->CloseInputDevice(TOUCH_INDEX);
    if (ret) {
        HDF_LOGE("%s: close device1 failed, ret %d", __func__, ret);
    }
    EXPECT_EQ(ret, INPUT_SUCCESS);
    HDF_LOGI("%s: Close the device1 successfully after all test", __func__);
}
}
