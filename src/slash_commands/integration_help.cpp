#include "api.h"
#include "slash_commands.h"
#include "utils.h"

void api::slash_command_calls::integration_help_call (const dpp::slashcommand_t& event, dpp::cluster& bot) {
    event.reply("Generating integration code, please wait...");
    const auto guild { event.command.get_guild() };
    const auto channel { event.command.get_channel() };

    bot.roles_get(guild.id, [&bot, channel, guild](const auto& cb) {

        if (cb.is_error()) {
            bot.message_create(dpp::message(channel.id, ":exclamation: Could not read roles in this server."));
            return;
        }

        const auto roles { std::get<dpp::role_map>(cb.value) };
        const auto role_it = find_my_role(bot, channel, roles);
        if (role_it == roles.end()) {
            bot.message_create(dpp::message(channel.id, ":exclamation: Please run `" + api::route_here.route + "` first to create `" + api::names::role + "` role."));
            return;
        }

        bot.message_create(dpp::message(channel.id, "... proceeding with role <@&" + std::to_string(role_it->first) + ">"));
        bot.get_channel_webhooks(channel.id, [&bot, channel, role = role_it->second](const auto& cb) {\

            if (cb.is_error()) {
                bot.log(dpp::loglevel::ll_error, "Could not read webhooks: " + cb.get_error().message);
                bot.message_create(dpp::message(channel.id, ":exclamation: Could not read webhooks from channel."));
                return;
            }

            const auto webhooks { std::get<dpp::webhook_map>(cb.value) };
            const auto webhook_it = find_my_webhook(bot, channel, webhooks);

            if (webhook_it == webhooks.end()) {
                bot.message_create(dpp::message(channel.id, ":exclamation: Please run `" + api::route_here.route + "` first to create `" + api::names::webhook + "` webhook."));
                return;
            }

            bot.message_create(dpp::message(channel.id, "... proceeding with webhook `" + api::names::webhook + "`"));
            do_integration_help(bot, role, webhook_it->second, channel);
        });
    });
}

void do_integration_help(dpp::cluster& bot, const dpp::role& role, const dpp::webhook& webhook, const dpp::channel& channel) {

}