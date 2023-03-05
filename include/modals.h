#ifndef MODALS_H
#define MODALS_H

#include <SQLiteCpp/SQLiteCpp.h>
#include <dpp/dpp.h>
#include <functional>
#include <string>

namespace api {
    using modal_t = std::function<void(const dpp::form_submit_t& event, dpp::cluster& bot, SQLite::Database& db)>;

    namespace modals {
        const std::string pull_request = "p4dc_pr";
        const std::string pull_request_defaults = "p4dc_pr_defaults";
        void pull_request_modal (const dpp::form_submit_t& event, dpp::cluster& bot, SQLite::Database& db);
        void pull_request_defaults_modal (const dpp::form_submit_t& event, dpp::cluster& bot, SQLite::Database& db);
    }

    class modal {
    public:
        const std::string route;
        const modal_t call;

        modal (const std::string& route, const modal_t& call) 
        : route {route}, call {call} {}
    };

    const modal
        submit_pull_request (
            api::modals::pull_request,
            api::modals::pull_request_modal
        ),
        pull_request_defaults (
            api::modals::pull_request_defaults,
            api::modals::pull_request_defaults_modal
        );
}

#endif