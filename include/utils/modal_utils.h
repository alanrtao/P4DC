#ifndef MODAL_UTILS_H
#define MODAL_UTILS_H

#include <dpp/dpp.h>

namespace modal {
    dpp::component make_pr_title_field();
    dpp::component make_pr_channel_id_field(const std::string& channel_id_str);
    dpp::component make_pr_msg_id_field(const std::string& msg_id_str);
    dpp::component make_pr_content_field(const std::string& default_val);
}

#endif