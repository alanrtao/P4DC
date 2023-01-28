#include "api.h"
#include "slash_commands.h"
#include "utils.h"

void roles_check(dpp::cluster& bot, const dpp::guild& guild, const dpp::channel& channel);
void webhooks_check(dpp::cluster& bot, const dpp::role& role, const dpp::guild& guild, const dpp::channel& channel);

void api::slash_command_calls::integration_help_call (const dpp::slashcommand_t& event, dpp::cluster& bot) {
    event.reply("Generating integration code, please wait...");
    const auto guild { event.command.get_guild() };
    const auto channel { event.command.get_channel() };

    // triggers series of steps
    roles_check(bot, guild, channel);
}

void roles_check(dpp::cluster& bot, const dpp::guild& guild, const dpp::channel& channel) {
    bot.roles_get(guild.id, [&bot, &channel, &guild](const auto& cb) {
        if (cb.is_error()) {
            bot.log(dpp::loglevel::ll_error, "Could not read roles: " + cb.get_error().message);
            bot.message_create(dpp::message(channel.id, ":exclamation: Could not read roles in this server."));
        } else {
            auto roles{std::get<dpp::role_map>(cb.value)};
            for(const auto& [k, role] : roles) {
                if (examine_existing_role(bot, role, channel)) {
                    bot.message_create(dpp::message(channel.id, "... proceeding with role <@&" + std::to_string(role.id) + ">"));
                    // proceed
                    webhooks_check(bot, role, guild, channel);
                    return;
                }
            }

            // no matching role
            bot.message_create(dpp::message(channel.id, "Could not find suitable role. The `/" + api::create_role.route + "` command needs to be run first."));
        }
    });
}

void webhooks_check(dpp::cluster& bot, const dpp::role& role, const dpp::guild& guild, const dpp::channel& channel) {
    bot.get_guild_webhooks(guild.id, [&bot, &channel, &role](const auto& cb) {
        if (cb.is_error()) {
            bot.log(dpp::loglevel::ll_error, "Could not read webhooks: " + cb.get_error().message);
            bot.message_create(dpp::message(channel.id, ":exclamation: Could not read webhooks from channel."));
        } else {
            auto webhooks{std::get<dpp::webhook_map>(cb.value)};
            for(const auto& [k, webhook] : webhooks) {
                if (examine_existing_webhook(bot, webhook, channel)) {
                    // proceed
                    do_integration_help(bot, role, webhook, channel);
                    return;
                }
            }
            
            // no matching webhook
            bot.message_create(dpp::message(channel.id, "Could not find suitable webhook. The `/" + api::route_here.route + "` command needs to be run first."));
        }
    });
}

void do_integration_help(dpp::cluster& bot, const dpp::role& role, const dpp::webhook& webhook, const dpp::channel& channel) {

}