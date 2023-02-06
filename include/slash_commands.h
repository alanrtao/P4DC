#ifndef SLASH_COMMANDS_H
#define SLASH_COMMANDS_H

#include "api.h"
#include <vector>
#include <dpp/dpp.h>

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
        const std::vector<dpp::command_option> options;
        slash_command(
            const std::string& route,
            const std::string& description,
            const api::slash_command_t& call,
            const std::vector<dpp::command_option>& options
            ) : route{route}, description{description}, call{call}, options{options} {}
    };

    const slash_command
        route_here{
            "route_here",
            "Specify channel for the bot to post in.",
            api::slash_command_calls::route_here_call,
            {}
        },
        integration_help{
            "integration_help",
            "Download Perforce integration script for the current channel",
            api::slash_command_calls::integration_help_call,
            {
                dpp::command_option(
                    dpp::co_string,
                    "depot", 
                    "The name of the depot you want to integrate this channel to. Please omit any slashes in the beginning and end of the depot path.",
                    true)
            }
        };
}

#endif