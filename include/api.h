#ifndef API_H
#define API_H
#include <string>

namespace api {
    const std::string webhooks_root = "https://discord.com/api/webhooks/";
    const std::string route_here = "route_here";
    const std::string create_role = "create_role";
    const std::string integration_help = "integration_help";

    namespace names {
        const std::string role = "P4DCReviewer";
        const std::string webhook = "P4DCHook";
    }
}

#endif