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
    }

    namespace sql {
        const std::string init_table = 
            "CREATE TABLE Servers (channel_id CHAR(20), webhook VARCHAR(255), role_id VARCHAR(255));" 
            "CREATE UNIQUE INDEX ON TABLE Servers (channel_id);";
    }

    using api_call_t = std::function<void(const dpp::slashcommand_t& event, dpp::cluster& bot)>;

    namespace calls {
        void route_here_call(const dpp::slashcommand_t& event, dpp::cluster& bot);
        void create_role_call(const dpp::slashcommand_t& event, dpp::cluster& bot);
        void integration_help_call(const dpp::slashcommand_t& event, dpp::cluster& bot);
    }

    struct api_call {
    public:
        const std::string route;
        const std::string description;
        const api::api_call_t call;
        api_call(const std::string& route, const std::string& description, const api::api_call_t& call) : route{route}, description{description}, call{call} {}
    };

    const api_call
        route_here{
            "route_here",
            "Specify channel for the bot to post in.",
            api::calls::route_here_call
        },
        create_role{
            "create_role",
            "Create " + api::names::role + " role.",
            api::calls::create_role_call
        },
        integration_help{
            "integration_help",
            "Download Perforce integration script for the current channel",
            api::calls::integration_help_call
        };
}

#endif