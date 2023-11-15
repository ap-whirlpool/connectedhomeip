/*
 *
 *    Copyright (c) 2023 Project CHIP Authors
 *    All rights reserved.
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
#include <app-common/zap-generated/cluster-objects.h>
#include <app/AttributeAccessInterface.h>

namespace chip {
namespace app {
namespace Clusters {
namespace TimerControl {

/** @brief
 *    Defines methods for implementing application-specific logic for the cooktop cluster.
 */
class Delegate
{
public:
    Delegate()          = default;
    virtual ~Delegate() = default;

    /**
     * Get the timer state at the given index in the list.
     * @param index The index of the spin speed, with 0 representing the first one.
     * @param timerSTate The MutableCharSpan to copy the string data into.  On success, the callee must update
     *        the length to the length of the copied data.
     * @return CHIP_ERROR_PROVIDER_LIST_EXHAUSTED if the index is out of range for the list of timer states.
     */
    virtual CHIP_ERROR GetTimerStateAtIndex(size_t index, MutableCharSpan & timerState) = 0;
};

} // namespace Cooktop
} // namespace Clusters
} // namespace app
} // namespace chip
