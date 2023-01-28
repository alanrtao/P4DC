#include <dpp/dpp.h>
#include <vector>
#include <unordered_map>
#include <algorithm>

#include "msg_handlers.h"
#include "slash_commands.h"

#include "env.h"

const std::string db_file = "servers.db3";

int main() {
    dpp::cluster bot(BOT_TOKEN);
    bot.on_log(dpp::utility::cout_logger());
 
    // route incoming slash commands
    const std::vector<api::slash_command> slash_commands {
        api::route_here,
        api::create_role,
        api::integration_help
    };

    std::unordered_map<std::string, api::slash_command> sc_map{};
    for(const auto& sc : slash_commands) sc_map.insert({ sc.route, sc });

    bot.on_slashcommand([&bot, &sc_map](const dpp::slashcommand_t& event) {
        const auto cmd{event.command.get_command_name()};
        if (const auto& query{sc_map.find(cmd)}; query != sc_map.end()) {
            const auto [name, command] = *query;
            command.call(event, bot);
        }
    });
 
    // register slash commands
    bot.on_ready([&bot, &slash_commands](const dpp::ready_t& event) {
        if (dpp::run_once<struct register_bot_commands>()) {
            for(const auto& cmd : slash_commands) {
                dpp::slashcommand(cmd.route, cmd.description, bot.me.id);
            }
        }
    });

    // message handlers
    const std::vector<api::msg_handler> msg_handlers {
    };

    bot.on_message_create([&bot](const dpp::message_create_t& event) {
    });
 
    bot.start(dpp::st_wait);
}