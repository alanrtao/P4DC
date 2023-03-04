#ifndef MODALS_H
#define MODALS_H

#include <SQLiteCpp/SQLiteCpp.h>
#include <dpp/dpp.h>
#include <functional>

namespace api {
    using modal_t = std::function<void(const dpp::form_submit_t& event, dpp::cluster& bot, SQLite::Database& db)>;

    namespace modals {
        void pull_request_modal (const dpp::form_submit_t& event, dpp::cluster& bot, SQLite::Database& db);
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
            "submit_pull_request", api::modals::pull_request_modal
        );

}

#endif