#include "api.h"

void api::calls::route_here_call (const dpp::slashcommand_t& event, dpp::cluster& bot) {
    const auto channel{event.command.get_channel()};
    const auto guild{event.command.get_guild()};

    event.reply("P4DC routed to "+channel.name+" :house: \nMake sure to run /"+api::create_role.route+" and /"+api::integration_help.route+" to create custom scripts for integrating Perforce triggers.");

    bot.get_guild_webhooks(channel.id, [&bot, &channel_id = channel.id](auto cb) {
        if (!cb.is_error()) {
            bool channel_has_webhook = false;
            for(const std::pair<dpp::snowflake, dpp::webhook>& item : std::get<dpp::webhook_map>(cb.value)) {
                const auto& [k, v] = item;
                if (v.name == api::names::webhook) {
                    // add webhook on this channel
                    if (v.channel_id == channel_id && !channel_has_webhook) {
                        channel_has_webhook = true;
                        // link_to_webhook(bot, db, v);
                    }
                    // remove my webhook on other channels
                    else {
                        bot.delete_webhook(v.id);
                        bot.message_create(dpp::message(v.channel_id, ":point_right: " + api::names::webhook + " already exists on this channel"));
                    }
                }
            }
            if (channel_has_webhook) {
                dpp::webhook new_hook{};
                new_hook.name = api::names::webhook;
                new_hook.application_id = bot.me.id;
                new_hook.channel_id = channel_id;
                bot.create_webhook(new_hook); // see on_webhooks_update below for callback
                // link_to_webhook(bot, db, new_hook);
            }
        } else {
            bot.message_create(dpp::message(channel_id, ":exclamation: Could not read webhooks from channel."));
        }
    });
}

void api::calls::create_role_call (const dpp::slashcommand_t& event, dpp::cluster& bot) {
    
}

void api::calls::integration_help_call (const dpp::slashcommand_t& event, dpp::cluster& bot) {
    
}
