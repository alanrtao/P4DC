#include "api.h"
#include "slash_commands.h"
#include "api_utils.h"
#include <archive.h>
#include <archive_entry.h>

void api::slash_command_calls::pull_request_call (const dpp::slashcommand_t& event, dpp::cluster& bot, SQLite::Database& db) {
    const auto guild { event.command.get_guild() };
    const auto channel { event.command.get_channel() };
    const auto user { event.command.get_issuing_user() };
    
    bot.log(dpp::ll_error, event.command.get_context_message().content);
}