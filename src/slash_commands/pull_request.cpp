#include "api.h"
#include "modals.h"
#include "slash_commands.h"
#include "utils/api_utils.h"
#include "utils/db_utils.h"
#include "utils/modal_utils.h"

void api::slash_command_calls::pull_request_call (const dpp::message_context_menu_t event, dpp::cluster& bot, SQLite::Database& db) {
    const auto msg = event.get_message();
    const auto msg_id_str = std::to_string(msg.id);
    const auto channel_id_str = std::to_string(msg.channel_id);

    const auto status = db::get_webhook(db, channel_id_str);
    if (status.is_error) {
        event.reply(dpp::message("No valid webhook exists. Please run `"+api::route_here.route+"` first.").set_flags(dpp::m_ephemeral));
        return;
    }

    const std::string& webhook_url = status.value;
    if (
        msg.author.username != api::names::webhook_user
        // || webhook_url.find(std::to_string(msg.webhook_id)) == std::string::npos
        ) {
        event.reply(dpp::message("Message must come from "+api::names::webhook_user+" via the channel's registered webhook.").set_flags(dpp::m_ephemeral));
        return;
    }

    const auto defaults = db::get_pr_defaults(db, channel_id_str);

    dpp::interaction_modal_response modal (api::modals::pull_request, "Pull Request");

    modal
    .add_component(modal_utils::make_pr_title_field(msg_id_str))
    .add_row()
    .add_component(modal_utils::make_pr_content_field(defaults.is_error ? "" : defaults.value));

    event.dialog(modal);
}