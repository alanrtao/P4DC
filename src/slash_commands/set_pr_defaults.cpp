#include "api.h"
#include "modals.h"
#include "slash_commands.h"
#include "utils/api_utils.h"
#include "utils/db_utils.h"
#include "utils/modal_utils.h"

#include <string>

void api::slash_command_calls::set_pr_defaults_call (const dpp::slashcommand_t& event, dpp::cluster& bot, SQLite::Database& db) {
    const auto prior = db::get_pr_defaults(db, std::to_string(event.command.channel_id));
    const auto prior_value = prior.is_error ? "" : prior.value;
    event.dialog(
        dpp::interaction_modal_response(api::modals::pull_request_defaults, "Edit Default PR Contenet")
        .add_component(modal_utils::make_pr_content_field(prior_value).set_required(false))
    );
}