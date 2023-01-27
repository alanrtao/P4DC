#include <dpp/dpp.h>
#include <vector>
#include <algorithm>
#include <unordered_map>
#include "all_api_calls.h"
#include "env.h"

const std::string db_file = "servers.db3";

int main() {
    dpp::cluster bot(BOT_TOKEN);
    
    const std::unordered_map<std::string, api::api_call> slash_commands{
        { api::route_here.route, api::route_here },
        { api::create_role.route, api::create_role },
        { api::integration_help.route, api::integration_help }
    };
 
    bot.on_log(dpp::utility::cout_logger());
 
    // route incoming slash commands
    bot.on_slashcommand([&bot, &slash_commands](const dpp::slashcommand_t& event) {
        const auto cmd{event.command.get_command_name()};
        auto route{slash_commands.find(cmd)};
        if (route != slash_commands.end()) {
            auto& [k, v] = *route;
            v.call(event, bot);
        }
    });
 
    // register slash commands
    bot.on_ready([&](const dpp::ready_t& event) {
        if (dpp::run_once<struct register_bot_commands>()) {
            for(const auto& [k, v] : slash_commands) {
                dpp::slashcommand(k, v.description, bot.me.id);
            }
        }
    });
 
    bot.start(dpp::st_wait);
}