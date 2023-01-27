#include "api.h"
#include "all_api_calls.h"
#include "utils.h"

void api::calls::integration_help_call (const dpp::slashcommand_t& event, dpp::cluster& bot) {
    event.reply("Generating integration code, please wait...");
    const auto guild { event.command.get_guild() };
    const auto channel { event.command.get_channel() };

    // fail if no role exists
    bot.roles_get(guild.id, [&bot, &channel](auto cb) {
        if (cb.is_error()) {
            bot.log(dpp::loglevel::ll_error, "Could not read roles: " + cb.get_error().message);
            bot.message_create(dpp::message(channel.id, ":exclamation: Could not read roles in this server."));
        } else {
            auto m{std::get<dpp::role_map>(cb.value)};
            bool guild_has_role = false;
            for(const auto& [k, role] : m) {
                if (examine_existing_role(bot, role, channel, false)) {
                    bot.message_create(dpp::message(channel.id, ""));
                    guild_has_role = true;
                }
            }

            if (!guild_has_role)
                bot.message_create(dpp::message(channel.id, "Could not find suitable role. The `/" + api::create_role.route + "` command needs to be run first."));
        }
    });

    // fail if no webhook exists
    bot.get_guild_webhooks(guild.id, [&bot, &channel](auto cb) {
        if (cb.is_error()) {
            bot.log(dpp::loglevel::ll_error, "Could not read webhooks: " + cb.get_error().message);
            bot.message_create(dpp::message(channel.id, ":exclamation: Could not read webhooks from channel."));
        } else {
            auto m{std::get<dpp::webhook_map>(cb.value)};
            bool channel_has_webhook = false;
            for(const auto& [k, v] : m)
                channel_has_webhook = examine_existing_webhook(bot, v, channel, channel_has_webhook);
            if (!channel_has_webhook)
                do_create_webhook(bot, channel);
            else
                bot.message_create(dpp::message(channel.id, "Webhook already exists on this channel."));
        }
    });
}