#ifndef SLASH_COMMANDS_H
#define SLASH_COMMANDS_H

#include "api.h"
#include <vector>
#include <exception>
#include <dpp/dpp.h>
#include <SQLiteCpp/SQLiteCpp.h>

namespace api {
    using slash_command_t = std::function<void(const dpp::slashcommand_t& event, dpp::cluster& bot, SQLite::Database& db)>;
    using message_context_menu_t = std::function<void(const dpp::message_context_menu_t event, dpp::cluster& bot, SQLite::Database& db)>;

    namespace slash_command_calls {
        void route_here_call (const dpp::slashcommand_t& event, dpp::cluster& bot, SQLite::Database& db);
        void integration_help_call (const dpp::slashcommand_t& event, dpp::cluster& bot, SQLite::Database& db);
        void set_pr_defaults_call (const dpp::slashcommand_t& event, dpp::cluster& bot, SQLite::Database& db);
        void pull_request_call (const dpp::message_context_menu_t event, dpp::cluster& bot, SQLite::Database& db);
    }

    struct slash_command_option {
    public:
        const std::string name;
        const std::string description;
        const dpp::command_option_type type;
        const bool required;

        slash_command_option(
            const std::string& name, const std::string& description,
            const dpp::command_option_type type, const bool required
        ) : name{name}, description{description}, type{type}, required{required} {}

        const dpp::command_option make_option() const { return {type, name, description, required}; }
    };

    struct slash_command {
    public:
        const std::string route;
        const std::string description;
        const api::slash_command_t call;
        const std::vector<slash_command_option> options;
        const api::message_context_menu_t call_msg_ctxt;
        const bool is_context;

        slash_command (
            const std::string& route,
            const std::string& description,
            const api::slash_command_t& call,
            const std::vector<slash_command_option>& options
            ) : route{route}, description{description}, call{call}, options{options}, is_context{false} {
                if (description.length() > 100) throw ("Description too long for slash command " + route);
            }

        slash_command (
            const std::string& route,
            const std::string& description,
            const api::message_context_menu_t& call_msg_ctxt
        ) : route{route}, description{description}, call_msg_ctxt{call_msg_ctxt}, options{}, is_context{true} {
            if (description.length() > 100) throw ("Description too long for slash command " + route);
        }
    };

    const slash_command

        route_here {
            "route_here",
            "Specify channel for the bot to post in.",
            api::slash_command_calls::route_here_call,
            {}
        },

        integration_help {
            "integration_help",
            "Download Perforce integration script for the current channel",
            api::slash_command_calls::integration_help_call,
            {
                {
                    "depot", 
                    "The name of the depot you want to integrate this channel to.",
                    dpp::co_string,
                    true
                }
            }
        },

        set_pr_defaults {
            "set_pr_defaults",
            "Edit the default PR content for channel. HTML comments will be omitted on actual PR submission.",
            api::slash_command_calls::set_pr_defaults_call,
            {}
        },

        pull_request(
            "pull_request",
            "Convert a message to a Pull Request thread. Message must come from " + api::names::webhook_user + ".",
            api::slash_command_calls::pull_request_call
        );
}

#endif