#include "utils/modal_utils.h"

const std::string base_label = "Pull Request #";

const std::string modal_utils::make_pr_modal_label(const std::string& msg_id) {
    return base_label + msg_id;
}

const std::string modal_utils::unpack_pr_modal_label(const std::string& modal_label) {
    if (modal_label.length() < base_label.length()) return "";
    else return modal_label.substr(base_label.length());
}

dpp::component modal_utils::make_pr_title_field(const std::string& msg_id_str) {
    return dpp::component()
        .set_label(make_pr_modal_label(msg_id_str))
        .set_id("title")
        .set_type(dpp::cot_text)
        .set_required(true)
        .set_min_length(0)
        .set_max_length(120)
        .set_placeholder("Title")
        .set_text_style(dpp::text_short);
}

dpp::component modal_utils::make_pr_content_field(const std::string& default_val) {
    auto temp = dpp::component()
        .set_label("Description")
        .set_id("description")
        .set_type(dpp::cot_text)
        .set_required(true)
        .set_min_length(0)
        .set_max_length(500)
        .set_placeholder("Describe your pull request")
        .set_text_style(dpp::text_paragraph);

    if (default_val == "") {
        temp.set_default_value(default_val);
    }

    return temp;
}