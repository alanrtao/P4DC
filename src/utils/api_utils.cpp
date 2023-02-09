#include "api_utils.h"

bool is_admin(dpp::cluster& bot, const dpp::slashcommand_t& event) {
    return event.command.get_resolved_permission(event.command.get_issuing_user().id).has(dpp::p_administrator);
}