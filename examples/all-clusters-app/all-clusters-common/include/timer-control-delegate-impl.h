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

#include <app/clusters/laundry-washer-controls-server/laundry-washer-controls-delegate.h>
#include <app/clusters/laundry-washer-controls-server/laundry-washer-controls-server.h>
#include <app/util/af.h>
#include <app/util/config.h>
#include <cstring>

namespace chip {
namespace app {
namespace Clusters {
namespace TimerControl {

/**
 * The application delegate to statically define the options.
 */

class TimerControlDelegate : public Delegate
{
    static const TimerStatusEnum supportedTimerStates[];
    static TimerControlDelegate instance;

public:
    CHIP_ERROR GetTimerStateAtIndex(size_t index, TimerStatusEnum & timerState);

    TimerControlDelegate()  = default;
    ~TimerControlDelegate() = default;

    static inline TimerControlDelegate & getTimerControlDelegate() { return instance; }
};

} // namespace TimerControls
} // namespace Clusters
} // namespace app
} // namespace chip
