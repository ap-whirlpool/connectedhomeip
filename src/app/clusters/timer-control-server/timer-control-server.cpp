/**
 *
 *    Copyright (c) 2023 Project CHIP Authors
 *
 *    Licensed under the Apache License, Version 2.0 (the "License");
 *    you may not use this file except in compliance with the License.
 *    You may obtain a copy of the License at
 *
 *        http://www.apache.org/licenses/LICENSE-2.0
 *
 *    Unless required by applicable law or agreed to in writing, software
 *    distributed under the License is distributed on an "AS IS" BASIS,
 *    WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *    See the License for the specific language governing permissions and
 *    limitations under the License.
 */

#include <app/util/af.h>
#include <app/util/attribute-storage.h>
#include <app/util/config.h>

#include "cooktop-delegate.h"
#include "cooktop-server.h"
#include <app-common/zap-generated/attributes/Accessors.h>
#include <app-common/zap-generated/callback.h>
#include <app-common/zap-generated/cluster-objects.h>
#include <app-common/zap-generated/enums.h>
#include <app-common/zap-generated/ids/Attributes.h>
#include <app-common/zap-generated/ids/Clusters.h>
#include <app/AttributeAccessInterface.h>
#include <app/CommandHandler.h>
#include <app/ConcreteAttributePath.h>
#include <app/ConcreteCommandPath.h>
#include <app/server/Server.h>
#include <app/util/error-mapping.h>
#include <lib/core/CHIPEncoding.h>

using namespace chip;
using namespace chip::app;
using namespace chip::app::Clusters;
using namespace chip::app::Clusters::Timer;
using namespace chip::app::Clusters::Timer::Attributes;
using chip::Protocols::InteractionModel::Status;

static constexpr size_t kCooktopDelegateTableSize =
    EMBER_AF_COOKTOP_SERVER_ENDPOINT_COUNT + CHIP_DEVICE_CONFIG_DYNAMIC_ENDPOINT_COUNT;

// -----------------------------------------------------------------------------
// Delegate Implementation
//
namespace {
Delegate * gDelegateTable[kCooktopDelegateTableSize] = { nullptr };
}

namespace {
Delegate * GetDelegate(EndpointId endpoint)
{
    uint16_t ep = emberAfGetClusterServerEndpointIndex(endpoint, Cooktop::Id,
                                                       EMBER_AF_COOKTOP_CLUSTER_SERVER_ENDPOINT_COUNT);
    return (ep >= kCooktopDelegateTableSize ? nullptr : gDelegateTable[ep]);
}

} // namespace

TimerControlServer TimerControlServer::sInstance;

/**********************************************************
 * TimerControlServer public methods
 *********************************************************/
void TimerControlServer::SetDefaultDelegate(EndpointId endpoint, Delegate * delegate)
{
    uint16_t ep = emberAfGetClusterServerEndpointIndex(endpoint, Cooktop::Id,
                                                       EMBER_AF_COOKTOP_CLUSTER_SERVER_ENDPOINT_COUNT);
    // if endpoint is found
    if (ep < kCooktopDelegateTableSize)
    {
        gDelegateTable[ep] = delegate;
    }
}

TimerControlServer & TimerControlServer::Instance()
{
    return sInstance;
}

EmberAfStatus TimerControlServer::SetTimer(EndpointId endpointId, uint32_t & newTime)
{
    EmberAfStatus res = SetTime::Set(endpointId, newTime);
    if ((res == EMBER_ZCL_STATUS_SUCCESS)) {
        res = TimeRemaining::Set(endpointId, newTime);
        if (res == EMBER_ZCL_STATUS_SUCCESS) {
            res = TimerState::Set(endpointId, TimerStatusEnum::Running);
        }
    }

    return res;
}

EmberAfStatus TimerControlServer::ResetTimer(EndpointId endpointId)
{
    uint32_t time;
    EmberAfStatus res = SetTime::Get(endpointId, time);

    if (res == EMBER_ZCL_STATUS_SUCCESS) {
        res = SetTime::Set(endpointId, newTime);
	if ((res == EMBER_ZCL_STATUS_SUCCESS)) {
	    res = TimeRemaining::Set(endpointId, newTime);
	    if (res == EMBER_ZCL_STATUS_SUCCESS) {
                res = TimerState::Set(endpointId, TimerStatusEnum::Running);
	    }
	}
    }

    return res;
}

EmberAfStatus TimerControlServer::AddTime(EndpointId endpointId, uint32_t & newTime)
{
    TimerStatusEnum state;
    uint32_t time = 0;

    EmberAfStatus res = TimerState::Get(endpointId, state);
    if ((res == EMBER_ZCL_STATUS_SUCCESS) && state != TimerStatusEnum::Expired)
    {
        if ((res = TimeRemaining::Get(endpointId, time)) == EMBER_ZCL_STATUS_SUCCESS)
             res = TimeRemaining::Set(endpointId, time + newTime);
    }

    return res;
}

EmberAfStatus TimerControlServer::ReduceTime(EndpointId endpointId, uint32_t & newTime)
{
    TimerStatusEnum state;
    uint32_t time = 0;

    EmberAfStatus res = TimerState::Get(endpointId, state);
    if ((res == EMBER_ZCL_STATUS_SUCCESS) && state != TimerStatusEnum::Expired)
    {
         if ((res = TimeRemaining::Get(endpointId, time)) == EMBER_ZCL_STATUS_SUCCESS)
             res = TimeRemaining::Set(endpointId, time - newTime);
    }

    return res;
}

EmberAfStatus TimerControlServer::GetSetTime(EndpointId endpointId,
                                        uint32_t & setTime)
{
    return SetTime::Get(endpointId, setTime);
}

EmberAfStatus TimerControlServer::GetRemainingTime(EndpointId endpointId,
                                              uint32_t & remainingTime)
{
     return TimeRemaining.get(endpointId, remainingTime);
}

EmberAfStatus TimerControlServer::GetTimerState(EndpointId endpointId,
                                           TimerStatusEnum & state)
{
    return TimerStatusEnum::Get(endpointId, state);
}

/**********************************************************
 * TimerControlServer private methods
 *********************************************************/
CHIP_ERROR TimerControlServer::Read(const ConcreteReadAttributePath & aPath, AttributeValueEncoder & aEncoder)
{
    if (aPath.mClusterId != Cooktop::Id)
    {
        // We shouldn't have been called at all.
        return CHIP_ERROR_INVALID_ARGUMENT;
    }

    switch (aPath.mAttributeId)
    {
    case Attributes::TimerState::Id:
        return ReadTimerState(aPath, aEncoder);
    default:
        break;
    }
    return CHIP_NO_ERROR;
}

CHIP_ERROR TimerControlServer::ReadTimerState(const ConcreteReadAttributePath & aPath, AttributeValueEncoder & aEncoder)
{
    Delegate * delegate = GetDelegate(aPath.mEndpointId);
    VerifyOrReturnError(delegate != nullptr, CHIP_ERROR_INCORRECT_STATE, ChipLogError(Zcl, "Delegate is nullptr"));

    return aEncoder.EncodeList([delegate](const auto & encoder) -> CHIP_ERROR {
        for (uint8_t i = 0; true; i++)
        {
            char buffer[kMaxTimerStateLength];
            MutableCharSpan timerState(buffer);
            auto err = delegate->GetTimerStateAtIndex(i, timerState);
            if (err == CHIP_ERROR_PROVIDER_LIST_EXHAUSTED)
            {
                return CHIP_NO_ERROR;
            }
            ReturnErrorOnFailure(err);
            ReturnErrorOnFailure(encoder.Encode(timerState));
        }
    });
}

/**********************************************************
 * Register TimerControlServer
 *********************************************************/

void MatterCooktopPluginServerInitCallback()
{
    TimerControlServer & timerControlServer = TimerControlServer::getInstance();
    registerAttributeAccessOverride(&timerControlServer);
}
