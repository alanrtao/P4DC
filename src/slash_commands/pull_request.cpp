#include "api.h"
#include "slash_commands.h"
#include "api_utils.h"
#include "db_utils.h"

void api::slash_command_calls::pull_request_call (const dpp::message_context_menu_t event, dpp::cluster& bot, SQLite::Database& db) {
    const auto msg = event.get_message();
    const auto msg_id_string = std::to_string(msg.id);
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

    const auto format = db::get_pr_format(db, channel_id_str);

    dpp::interaction_modal_response modal ("p4dc_pr", "Pull Request");

    modal
    .add_component(
        dpp::component()
        .set_label("Title")
        .set_id("title")
        .set_type(dpp::cot_text)
        .set_required(true)
        .set_min_length(0)
        .set_max_length(120)
        .set_text_style(dpp::text_short)
    )
    .add_row()
    .add_component(
        dpp::component()
        .set_label("Description")
        .set_id("description")
        .set_type(dpp::cot_text)
        .set_required(true)
        .set_min_length(0)
        .set_max_length(500)
        .set_default_value(format.is_error ? "" : format.value)
        .set_text_style(dpp::text_paragraph)
    );

    event.dialog(modal);
}