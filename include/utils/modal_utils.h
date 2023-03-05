#ifndef MODAL_UTILS_H
#define MODAL_UTILS_H

#include <dpp/dpp.h>

namespace modal_utils {
    const std::string make_pr_modal_label(const std::string& msg_id);
    const std::string unpack_pr_modal_label(const std::string& modal_label);
    dpp::component make_pr_title_field(const std::string& msg_id);
    dpp::component make_pr_content_field(const std::string& default_val);
}

#endif