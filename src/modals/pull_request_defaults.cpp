#include "modals.h"
#include "utils/db_utils.h"

void api::modals::pull_request_defaults_modal (const dpp::form_submit_t& event, dpp::cluster& bot, SQLite::Database& db) {
    const auto chan = std::to_string(event.command.channel_id);
    const auto content = std::get<std::string>(event.components[0].components[0].value);
    const auto status = db::upsert_pr_defaults(db, chan, content);

    event.reply(dpp::message(
        status.is_error ? "Could not update default PR content." : "Successfully updated default PR content."
    ).set_flags(dpp::m_ephemeral));
}