#ifndef ALL_API_CALLS_H
#define ALL_API_CALLS_H

#include "api.h"

namespace api {
    using slash_command_t = std::function<void(const dpp::slashcommand_t& event, dpp::cluster& bot)>;

    namespace slash_command_calls {
        void route_here_call(const dpp::slashcommand_t& event, dpp::cluster& bot);
        void create_role_call(const dpp::slashcommand_t& event, dpp::cluster& bot);
        void integration_help_call(const dpp::slashcommand_t& event, dpp::cluster& bot);
    }

    struct slash_command {
    public:
        const std::string route;
        const std::string description;
        const api::slash_command_t call;
        slash_command(const std::string& route, const std::string& description, const api::slash_command_t& call) : route{route}, description{description}, call{call} {}
    };

    const slash_command
    route_here{
        "route_here",
        "Specify channel for the bot to post in.",
        api::slash_command_calls::route_here_call
    },
    integration_help{
        "integration_help",
        "Download Perforce integration script for the current channel",
        api::slash_command_calls::integration_help_call
    };
}

#endif