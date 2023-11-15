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

#pragma once

#include "timer-control-delegate.h"
#include <app-common/zap-generated/cluster-objects.h>
#include <app/AttributeAccessInterface.h>
#include <app/util/af.h>

namespace chip {
namespace app {
namespace Clusters {
namespace TimerControl {

/**
 * @brief TimerControl Server Plugin class
 */
class TimerControlServer : public AttributeAccessInterface
{
public:
    TimerControlServer() : AttributeAccessInterface(Optional<EndpointId>::Missing(), TimerControl::Id) {}
    static TimerControlServer & Instance();

    static constexpr uint8_t kMaxTimerStateLength = 64;

    /**
     * Set the default delegate of timer-control server at endpoint x
     * @param endpoint ID of the endpoint
     * @param delegate The default delegate at the endpoint
     */
    static void SetDefaultDelegate(EndpointId endpoint, Delegate * delegate);

    /**
     * API to set timer to a new value.
     * @param endpoint ID of the endpoint
     * @param newTime Set the timer to this new time
     *
     */
    EmberAfStatus SetTimer(EndpointId endpoint, uint32_t newTime);

    /**
     * API to reset timer.
     * @param endpoint ID of the endpoint
     *
     */
    EmberAfStatus ResetTimer(EndpointId endpointId);

    /**
     * API to add time to an existing timer.
     * @param endpoint ID of the endpoint
     * @param additionalTime Update timer with additional time.
     *
     */
    EmberAfStatus AddTime(EndpointId endpoint, uint32_t additionalTime);

    /**
     * API to reduce time to an existing timer.
     * @param endpoint ID of the endpoint
     * @param timeReduction Update timer with reduced time.
     *
     */
    EmberAfStatus ReduceTime(EndpointId endpoint, uint32_t timeReduction);

    /**
     * API to get the SetTime attribute
     * @param endpoint ID of the nedpoint
     * @param setTime The result of reading the SetTime attribute.
     */
    EmberAfStatus GetSetTime(EndpointId endpointId, uint32_t & setTime);

    /**
     * API to get the TimeRemaining attribute
     * @param endpoint ID of the endpoint
     * @param timeRemaining Get the time remaining left
     */
    EmberAfStatus GetTimeRemaining(EndpointId endpoint, uint32_t & timeRemaining);

    
    /**
     * API to get the TimerState attribute
     * @param endpoint ID of the endpoint
     * @param timerStatus The result of reading the TimerState attribute.
     */
    EmberAfStatus GetTimerState(EndpointId endpointId, TimerStatusEnum  & timerStatus);

private:
    CHIP_ERROR Read(const ConcreteReadAttributePath & aPath, AttributeValueEncoder & aEncoder) override;
    CHIP_ERROR ReadTimerState(const ConcreteReadAttributePath & aPath, AttributeValueEncoder & aEncoder) override;

    static TimerControlServer sInstance;
};

} // namespace TimerControl
} // namespace Clusters
} // namespace app
} // namespace chip
