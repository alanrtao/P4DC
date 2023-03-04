#include <dpp/dpp.h>
#include <SQLiteCpp/SQLiteCpp.h>
#include <vector>
#include <unordered_map>
#include <algorithm>
#include <functional>

#include "slash_commands.h"
#include "modals.h"
#include "env.h"
#include "db_utils.h"

#include "text_utils.h"

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

    // make database tables
    using make_table_t = std::function<db::result_t(SQLite::Database&)>;
    std::vector<make_table_t> make_table_calls { db::make_webhooks_table, db::make_roles_table, db::make_pr_formats_table};
    for(const auto make_table : make_table_calls) {
        const auto status = make_table(db);
        if (status.is_error) {
            std::cerr<<status.error<<std::endl;
            return -1;
        }
    }
 
    // route incoming slash commands
    const std::vector<api::slash_command> slash_commands {
        api::route_here,
        api::integration_help,
        api::pull_request
    };

    std::unordered_map<std::string, api::slash_command> sc_map{};
    for(const auto& sc : slash_commands) sc_map.insert({ sc.route, sc });

    bot.on_slashcommand([&bot, &sc_map, &db](const dpp::slashcommand_t& event) {
        const auto cmd{ event.command.get_command_name() };
        if (const auto& query{ sc_map.find(cmd) }; query != sc_map.end()) {
            const auto [name, command] = *query;
            if (!command.is_context)
                command.call(event, bot, db);
            else
                bot.log(dpp::ll_critical, "Slash command mapped to context-menu route.");
        } 
    });

    bot.on_message_context_menu([&bot, &sc_map, &db](const dpp::message_context_menu_t& event) {
        const auto cmd { event.command.get_command_name() };
        if (const auto& query { sc_map.find(cmd) }; query != sc_map.end()) {
            const auto [name, command] = *query;
            if (command.is_context)
                command.call_msg_ctxt(event, bot, db);
            else
                bot.log(dpp::ll_critical, "Context menu mapped to non-context-menu route.");
        }
    });

    const std::vector<api::modal> modals {
        api::submit_pull_request
    };
    std::unordered_map<std::string, api::modal> mo_map {};
    for (const auto &mo : modals) { mo_map.insert({mo.route, mo}); }

    bot.on_form_submit([&bot, &mo_map, &db](const dpp::form_submit_t& event) {
        const auto modal { event.custom_id };
        if (const auto& query { mo_map.find(modal) }; query != mo_map.end()) {
            const auto [name, modal_call] = *query;
            modal_call.call(event, bot, db);
        }
    });

    bot.on_ready([&bot, &slash_commands](const dpp::ready_t& event) {
        if (dpp::run_once<struct register_bot_commands>()) {
            // register slash commands
            std::vector<dpp::slashcommand> global_commands_inst{};
            for (const auto& scmd : slash_commands) {
                dpp::slashcommand scmd_inst {
                    scmd.route,
                    scmd.description,
                    bot.me.id
                };
                for (const auto& opt : scmd.options)
                    scmd_inst.add_option(opt.make_option());
                if (scmd.is_context)
                    scmd_inst.set_type(dpp::ctxm_message);
                global_commands_inst.push_back(scmd_inst);
            }
            bot.global_bulk_command_create(global_commands_inst, [](const auto& event) {
                if (event.is_error())
                    std::cout<<event.get_error().message<<std::endl;
            });
        }
    });
 
    bot.start(dpp::st_wait);
}