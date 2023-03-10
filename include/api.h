#ifndef API_H
#define API_H
#include <string>
#include "env.h"

namespace api {
    const std::string webhooks_root = "https://discord.com/api/webhooks/";
    const std::string version = std::to_string(VERSION_MAJOR) + "." + std::to_string(VERSION_MINOR);

    namespace names {
        const std::string
            role = "P4DCReviewer",
            webhook = "P4DCHook",
            webhook_user = "P4DC Buddy";
    }

    namespace paths {
        const std::string
            hydratable_home = "./templates",
            instructions = "integration_instructions.txt",
            commit_trigger = "commit_trigger.sh",
            branch_trigger = "branch_trigger.sh";
    }

    namespace patterns {
        const std::string
            depot = "__depot__",
            webhook = "__webhook__",
            role = "__role__",
            webhook_user = "__webhook_user__";
    }
}

#endif