#include "api.h"
#include "slash_commands.h"
#include "api_utils.h"
#include "msg_handlers.h"

bool api::msg_handler_calls::pull_request_trigger (const dpp::message_create_t& event) {
    return
        event.msg.author.username == api::names::webhook_user
        && event.msg.content.find(api::names::pull_request_mark) != std::string::npos;
}

std::string get_pr_name (const std::string& msg_content);

void api::msg_handler_calls::pull_request_call (const dpp::message_create_t& event, dpp::cluster& bot) {
    const auto channel_id { event.msg.channel_id };
    const auto guild_id { event.msg.guild_id };

    const auto name { get_pr_name(event.msg.content) };
    const uint16_t archive_duration = 1440;

    bot.thread_create_with_message(name, channel_id, event.msg.id, archive_duration, 0, [&bot, event](const auto& cb) {
        if (cb.is_error()) {
            event.reply("... thread could not be created");
            return;
        }

        auto thread = std::get<dpp::thread>(cb.value);
        bot.roles_get(event.msg.guild_id, [&bot, event, thread](const auto& cb) {

            const std::string reply_base = "Found `" + api::names::pull_request_mark + "` in commit, thread opened.";
            
            if (cb.is_error()) {
                event.send("... could not read roles");
                return;
            }

            const auto roles{std::get<dpp::role_map>(cb.value)};
            for(const auto& [k, role] : roles) {
                if (is_my_role(bot, role)) {
                    event.reply(reply_base + " <@&" + std::to_string(role.id) + ">");
                    return;
                }
            }

            // no matching role
            event.reply(reply_base + " No `" + api::names::role + "` found, you can run `/" + api::route_here.route + "` to create it.");
        });
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