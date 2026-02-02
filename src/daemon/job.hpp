#pragma once

namespace daemon {
enum class JobType : int {
    kLogicJob = 0,
    kWorkJob,
    kSlowJob,
    kNetworkJob
};
}  // namespace daemon
