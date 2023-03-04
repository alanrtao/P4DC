#include "utils/modal_utils.h"

dpp::component modal::make_pr_title_field() {
    return dpp::component()
        .set_label("Title")
        .set_id("title")
        .set_type(dpp::cot_text)
        .set_required(true)
        .set_min_length(0)
        .set_max_length(120)
        .set_text_style(dpp::text_short);
}

dpp::component modal::make_pr_channel_id_field(const std::string& channel_id_str) {
    return dpp::component()
        .set_label("Channel ID")
        .set_id("channel_id")
        .set_disabled(true)
        .set_type(dpp::cot_text)
        .set_text_style(dpp::text_short)
        .set_default_value(channel_id_str)
        .set_required(true);
}

dpp::component modal::make_pr_msg_id_field(const std::string& msg_id_str) {
    return dpp::component()
        .set_label("Message ID")
        .set_id("msg_id")
        .set_disabled(true)
        .set_type(dpp::cot_text)
        .set_text_style(dpp::text_short)
        .set_default_value(msg_id_str)
        .set_required(true);
}

dpp::component modal::make_pr_content_field(const std::string& default_val) {
    return dpp::component()
        .set_label("Description")
        .set_id("description")
        .set_type(dpp::cot_text)
        .set_required(true)
        .set_min_length(0)
        .set_max_length(500)
        .set_default_value(default_val)
        .set_text_style(dpp::text_paragraph);
}