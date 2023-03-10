#include "modals.h"
#include "slash_commands.h"
#include "utils/modal_utils.h"
#include "utils/text_utils.h"
#include "utils/db_utils.h"

void api::modals::pull_request_modal (const dpp::form_submit_t& event, dpp::cluster& bot, SQLite::Database& db) {
    const auto pr_label = event.components[0].components[0].custom_id;
    const auto msg_id = dpp::snowflake (modal_utils::unpack_pr_modal_label(pr_label));
    const auto channel_id = event.command.channel_id;
    const auto guild_id = event.command.guild_id;

    const auto pr_title = std::get<std::string>(event.components[0].components[0].value);
    const auto pr_content = text::remove_xml_comments(std::get<std::string>(event.components[1].components[0].value));

    const uint16_t archive_duration = 1440;

    bot.message_get(msg_id, channel_id, 
        [&bot, event, &db, guild_id, channel_id, msg_id, pr_title, pr_content, archive_duration]
        (const auto &cb){
        if (cb.is_error()) {
            event.reply(dpp::message("PR does not reference valid message").set_flags(dpp::m_ephemeral));
            return;
        }
        const auto msg = std::get<dpp::message>(cb.value);
        if (msg.has_thread()) {
            event.reply(dpp::message("Commit already has thread attached.").set_flags(dpp::m_ephemeral));
            return;
        }

        bot.thread_create_with_message("PR: " + pr_title, channel_id, msg_id, archive_duration, 0, 
            [&bot, event, &db, pr_title, pr_content, guild_id, channel_id]
            (const auto &cb) {
            if (cb.is_error()) {
                event.reply(dpp::message(":exclamation: Thread could not be created").set_flags(dpp::m_ephemeral));
                return;
            }

            auto thread = std::get<dpp::thread>(cb.value);
            
            auto role_result = db::get_role(db, std::to_string(guild_id));
            auto webhook_result = db::get_webhook(db, std::to_string(channel_id));
            
            if (webhook_result.is_error) {
                event.reply(
                    dpp::message(" No `" + api::names::webhook + "` webhook found, you can run `/" + api::route_here.route + "` to create it.")
                    .set_flags(dpp::m_ephemeral));
                return;   
            }

            event.reply(dpp::message("Creating PR thread...").set_flags(dpp::m_ephemeral));

            const auto reply_ping {
                role_result.is_error ? 
                "> No `" + api::names::role + "` role found, you can run `/" + api::route_here.route + "` to create it.\n"
                : " <@&" + role_result.value + ">\n"
            };

            bot.execute_webhook(dpp::webhook(webhook_result.value), dpp::message(reply_ping + "\n" + pr_content), false, thread.id);
        });
    });
}

// #include "api.h"
// #include "slash_commands.h"
// #include "api_utils.h"
// #include "db_utils.h"
// #include "msg_handlers.h"

// bool api::msg_handler_calls::pull_request_trigger (const dpp::message_create_t& event) {
//     return
//         event.msg.author.username == api::names::webhook_user
//         && event.msg.content.find(api::names::pull_request_mark) != std::string::npos;
// }

// std::string get_pr_name (const std::string& msg_content);

// void api::msg_handler_calls::pull_request_call (const dpp::message_create_t& event, dpp::cluster& bot, SQLite::Database& db) {
//     const auto channel_id { event.msg.channel_id };
//     const auto guild_id { event.msg.guild_id };

//     const auto name { get_pr_name(event.msg.content) };
//     const uint16_t archive_duration = 1440;

//     bot.thread_create_with_message(name, channel_id, event.msg.id, archive_duration, 0, [&bot, event, &db](const auto& cb) {
//         if (cb.is_error()) {
//             event.reply("... thread could not be created");
//             return;
//         }

//         auto thread = std::get<dpp::thread>(cb.value);

//         const std::string reply_base = "Found `" + api::names::pull_request_mark + "` in commit, thread opened.";
        
//         auto role_result = db::get_role(db, std::to_string(event.msg.guild_id));
//         auto webhook_result = db::get_webhook(db, std::to_string(event.msg.channel_id));
        
//         if (webhook_result.is_error) {
//             event.reply(reply_base + " No `" + api::names::webhook + "` webhook found, you can run `/" + api::route_here.route + "` to create it.");
//             return;   
//         }

//         const auto reply_ping {
//             role_result.is_error ? 
//             " No `" + api::names::role + "` role found, you can run `/" + api::route_here.route + "` to create it."
//             : " <@&" + role_result.value + ">"
//         };

//         bot.execute_webhook(dpp::webhook(webhook_result.value), reply_base + reply_ping);
//     });
// }

// std::string get_pr_name (const std::string& msg_content) {
//     const auto begin { msg_content.begin() + msg_content.find(api::names::pull_request_mark) + api::names::pull_request_mark.size() };
//     for(auto i { begin }; i != msg_content.end(); ++i) {
//         if (i == msg_content.end() - 1 || *i == '\n')
//             return std::string(begin, ++i);
//     }
//     return "[no viable PR name]";
// }