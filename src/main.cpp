#include <dpp/dpp.h>
#include <setting.h>
#include <vector>
#include <algorithm>
#include <SQLiteCpp/SQLiteCpp.h>
#include "api.h"
#include "env.h"

const std::string db_file = "servers.db3";

void registerWebhookLaunch(dpp::cluster bot, SQLite::Database& db);

int main() {
    SQLite::Database db(db_file, SQLite::OPEN_READWRITE | SQLite::OPEN_CREATE);

    dpp::cluster bot(BOT_TOKEN);

    if (!db.tableExists("Servers")) db.exec("CREATE TABLE Servers (webhook VARCHAR(255), role_id VARCHAR(255));");
 
    bot.on_log(dpp::utility::cout_logger());
 
    bot.on_slashcommand([&db, &bot](const dpp::slashcommand_t& event) {
        auto cmd{event.command.get_command_name()};
        auto channel{event.command.get_channel()};
        auto guild{event.command.get_guild()};

        if (cmd == api::route_here) {
            event.reply("P4DC routed to "+channel.name+" :house: \nMake sure to run /"+api::create_role+" and /"+api::integration_help+" to create custom scripts for integrating Perforce triggers.");
            
            // fetch / create webhook
            auto new_hook = dpp::webhook();
            new_hook.name = api::names::webhook;
            new_hook.application_id = bot.me.id;
            new_hook.channel_id = channel.id;

            bot.create_webhook(new_hook, [&db, &channel_id = channel.id, &bot](auto cb) {
                if (!cb.is_error()) {
                    dpp::webhook h = std::get<dpp::webhook>(cb.value);
                    std::cout<<"Webhook created for channel: "<<channel_id<<std::endl;
                    std::string url = api::webhooks_root+std::to_string(h.id)+"/"+h.token;
                    bot.message_create(dpp::message(channel_id, ":link: Webhook successfully created. Please run /" + api::integration_help + " to integrate with Perforce triggers.\n Url: "+url));
                } else {
                    bot.message_create(dpp::message(channel_id, ":exclamation: "+cb.get_error().message));
                }
            });
        }
        else if (cmd == api::create_role) {
            event.reply(guild.name);
            bot.roles_get(guild.id, [&db, &channel_id = channel.id](auto cb) {

            });
        }
        else if (cmd == api::integration_help) {

        }
    });
 
    bot.on_ready([&bot](const dpp::ready_t& event) {
        if (dpp::run_once<struct register_bot_commands>()) {
            bot.global_command_create(
                dpp::slashcommand(api::route_here, "Specify channel for the bot to post in.", bot.me.id)
            );

            bot.global_command_create(
                dpp::slashcommand(api::create_role, "Create " + api::names::role + " role.", bot.me.id)
            );

            bot.global_command_create(
                dpp::slashcommand(api::integration_help, "Download Perforce integration script for the current channel", bot.me.id)
            );
        }
    });
 
    bot.start(dpp::st_wait);
}

void registerWebhookLaunch(dpp::cluster bot, SQLite::Database& db) {
    SQLite::Statement q(db, "SELECT webhook FROM Servers");
    while (q.executeStep()) {
        auto wh_id{q.getColumn(0).getString()};

    }
}