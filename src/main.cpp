#include <dpp/dpp.h>
#include <SQLiteCpp/SQLiteCpp.h>
#include <vector>
#include <unordered_map>
#include <algorithm>

#include "msg_handlers.h"
#include "slash_commands.h"

#include "env.h"

#include "db_utils.h"

const uint64_t intent =
    dpp::i_default_intents
    | dpp::i_message_content;

int main() {
    dpp::cluster bot(BOT_TOKEN, intent);
    bot.on_log(dpp::utility::cout_logger());

    SQLite::Database db(
        db::filename,
        SQLite::OPEN_CREATE 
        | SQLite::OPEN_READWRITE
        | SQLite::OPEN_FULLMUTEX // does not seem to work
    );

    const auto webhook_state = db::make_webhooks_table(db);

    if (webhook_state.is_error) {
        std::cout<<webhook_state.error<<std::endl;
        return -1;
    }
    
    const auto role_state = db::make_roles_table(db);
    if (role_state.is_error) {
        std::cout<<role_state.error<<std::endl;
        return -1;
    }
 
    // route incoming slash commands
    const std::vector<api::slash_command> slash_commands {
        api::route_here,
        api::integration_help
    };

    std::unordered_map<std::string, api::slash_command> sc_map{};
    for(const auto& sc : slash_commands) sc_map.insert({ sc.route, sc });

    bot.on_slashcommand([&bot, &sc_map, &db](const dpp::slashcommand_t& event) {
        const auto cmd{event.command.get_command_name()};
        if (const auto& query{sc_map.find(cmd)}; query != sc_map.end()) {
            const auto [name, command] = *query;
            command.call(event, bot, db);
        }
    });
 
    // register slash commands
    std::vector<dpp::slashcommand> slash_commands_inst{};
    for (const auto& scmd : slash_commands) {
        dpp::slashcommand scmd_inst{
            scmd.route,
            scmd.description,
            bot.me.id
        };
        for (const auto& opt : scmd.options) {
            scmd_inst.add_option(opt);
        }
        slash_commands_inst.push_back(scmd_inst);
    }

    bot.on_ready([&bot, &slash_commands_inst](const dpp::ready_t& event) {
        if (dpp::run_once<struct register_bot_commands>()) {
            bot.global_bulk_command_create(slash_commands_inst);
        }
    });

    // message handlers
    const std::vector<api::msg_handler> msg_handlers {
        api::pull_request
    };

    bot.on_message_create([&bot, &msg_handlers, &db](const dpp::message_create_t& event) {
        for(const auto& handler : msg_handlers) {
            if (handler.trigger(event)) {
                handler.handle(event, bot, db);
                break;
            }
        }
    });
 
    bot.start(dpp::st_wait);
}