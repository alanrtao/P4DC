#include "api.h"
#include "slash_commands.h"
#include "api_utils.h"
#include "db_utils.h"
#include "msg_handlers.h"

bool api::msg_handler_calls::pull_request_trigger (const dpp::message_create_t& event) {
    return
        event.msg.author.username == api::names::webhook_user
        && event.msg.content.find(api::names::pull_request_mark) != std::string::npos;
}

std::string get_pr_name (const std::string& msg_content);

void api::msg_handler_calls::pull_request_call (const dpp::message_create_t& event, dpp::cluster& bot, SQLite::Database& db) {
    const auto channel_id { event.msg.channel_id };
    const auto guild_id { event.msg.guild_id };

    const auto name { get_pr_name(event.msg.content) };
    const uint16_t archive_duration = 1440;

    bot.thread_create_with_message(name, channel_id, event.msg.id, archive_duration, 0, [&bot, event, &db](const auto& cb) {
        if (cb.is_error()) {
            event.reply("... thread could not be created");
            return;
        }

        auto thread = std::get<dpp::thread>(cb.value);

        const std::string reply_base = "Found `" + api::names::pull_request_mark + "` in commit, thread opened.";
        
        auto role_result = db::get_role(db, std::to_string(event.msg.guild_id));
        auto webhook_result = db::get_webhook(db, std::to_string(event.msg.channel_id));
        
        if (webhook_result.is_error) {
            event.reply(reply_base + " No `" + api::names::webhook + "` webhook found, you can run `/" + api::route_here.route + "` to create it.");
            return;   
        }

        const auto reply_ping {
            role_result.is_error ? 
            " No `" + api::names::role + "` role found, you can run `/" + api::route_here.route + "` to create it."
            : " <@&" + role_result.get_result() + ">"
        };

        bot.execute_webhook(dpp::webhook(webhook_result.get_result()), reply_base + reply_ping);
    });
}

std::string get_pr_name (const std::string& msg_content) {
    const auto begin { msg_content.begin() + msg_content.find(api::names::pull_request_mark) + api::names::pull_request_mark.size() };
    for(auto i { begin }; i != msg_content.end(); ++i) {
        if (i == msg_content.end() - 1 || *i == '\n')
            return std::string(begin, ++i);
    }
    return "[no viable PR name]";
}