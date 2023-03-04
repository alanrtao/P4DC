#include "api.h"
#include "slash_commands.h"
#include "api_utils.h"
#include "db_utils.h"

const dpp::component make_title_field();
const dpp::component make_channel_id_field(const std::string& channel_id_str);
const dpp::component make_msg_id_field(const std::string& msg_id_str);
const dpp::component make_content_field(const std::string& default_val);

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

    const auto format = db::get_pr_format(db, channel_id_str);

    dpp::interaction_modal_response modal ("p4dc_pr", "Pull Request");

    modal
    .add_component(make_title_field())
    .add_row()
    .add_component(make_channel_id_field(channel_id_str))
    .add_component(make_msg_id_field(msg_id_str))
    .add_row()
    .add_component(make_content_field(format.is_error ? "" : format.value));

    event.dialog(modal);
}

const dpp::component make_title_field() {
    return dpp::component()
        .set_label("Title")
        .set_id("title")
        .set_type(dpp::cot_text)
        .set_required(true)
        .set_min_length(0)
        .set_max_length(120)
        .set_text_style(dpp::text_short);
}

const dpp::component make_channel_id_field(const std::string& channel_id_str) {
    return dpp::component()
        .set_label("Channel ID")
        .set_id("channel_id")
        .set_disabled(true)
        .set_type(dpp::cot_text)
        .set_text_style(dpp::text_short)
        .set_default_value(channel_id_str)
        .set_required(true);
}

const dpp::component make_msg_id_field(const std::string& msg_id_str) {
    return dpp::component()
        .set_label("Message ID")
        .set_id("msg_id")
        .set_disabled(true)
        .set_type(dpp::cot_text)
        .set_text_style(dpp::text_short)
        .set_default_value(msg_id_str)
        .set_required(true);
}

const dpp::component make_content_field (const std::string& default_val) {
    dpp::component()
        .set_label("Description")
        .set_id("description")
        .set_type(dpp::cot_text)
        .set_required(true)
        .set_min_length(0)
        .set_max_length(500)
        .set_default_value(default_val)
        .set_text_style(dpp::text_paragraph);
}