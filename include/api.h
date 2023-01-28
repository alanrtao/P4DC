#ifndef API_H
#define API_H
#include <string>
#include <functional>

#include <dpp/dpp.h>

namespace api {
    const std::string webhooks_root = "https://discord.com/api/webhooks/";

    namespace names {
        const std::string role = "P4DCReviewer";
        const std::string webhook = "P4DCHook";
        const std::string webhook_user = "P4DC Buddy";
        const std::string pull_request_mark = "#PR";
    }
}

#endif