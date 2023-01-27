#include "api.h"
#include "functional_helper.h"

void examine_existing_webhook (dpp::cluster& bot, const dpp::webhook& wh, dpp::snowflake chan, bool& channel_has_webhook);
void create_webhook (dpp::cluster& bot, const dpp::snowflake chan);

void api::calls::route_here_call (const dpp::slashcommand_t& event, dpp::cluster& bot) {
    const auto channel{event.command.get_channel()};
    const auto guild{event.command.get_guild()};

    event.reply("P4DC routed to "+channel.name+" :house: \nMake sure to run /"+api::create_role.route+" and /"+api::integration_help.route+" to create custom scripts for integrating Perforce triggers.");

    bot.get_guild_webhooks(channel.id, do_or_err<dpp::webhook_map>(
        [&bot, &chan=channel.id](const dpp::webhook_map& m) {
            bool channel_has_webhook = false;
            for(const auto& [k, v] : m)
                examine_existing_webhook(bot, v, chan, channel_has_webhook);
            if (!channel_has_webhook)
                create_webhook(bot, chan);
            else
                bot.message_create(dpp::message(chan, "Webhook already exists on this channel."));
        },
        [&bot, &chan=channel.id](const std::string& err) {
            bot.message_create(dpp::message(chan, ":exclamation: Could not read webhooks from channel."));
        }
    ));
}

void api::calls::create_role_call (const dpp::slashcommand_t& event, dpp::cluster& bot) {
    
}

void api::calls::integration_help_call (const dpp::slashcommand_t& event, dpp::cluster& bot) {
    
}

bool is_my_role(const dpp::cluster& bot, const dpp::role& r) {
    return r.name == api::names::role && r.is_mentionable();
}

void examine_existing_roles (dpp::cluster& bot, const dpp::role& r, dpp::snowflake chan, bool& guild_has_role) {
    if (is_my_role(bot, r)) {
        guild_has_role = true;
    }
}

bool is_my_webhook(const dpp::cluster& bot, const dpp::webhook& wh) {
    return wh.name == api::names::webhook && wh.application_id == bot.me.id;
}

void examine_existing_webhook (dpp::cluster& bot, const dpp::webhook& wh, dpp::snowflake chan, bool& channel_has_webhook) {
    if (is_my_webhook(bot, wh)) {
        if (wh.channel_id == chan && !channel_has_webhook) {
            std::cout<<"webhook already exists on channel "<<chan<<std::endl;
            channel_has_webhook = true;
        }
        else {
            bot.delete_webhook(wh.id);
            bot.message_create(dpp::message(wh.channel_id, ":point_right: " + api::names::webhook + " already exists on this channel"));
        }
    }
}

void create_webhook (dpp::cluster& bot, const dpp::snowflake chan) {
    dpp::webhook new_hook{};
    new_hook.name = api::names::webhook;
    new_hook.application_id = bot.me.id;
    new_hook.channel_id = chan;
    bot.create_webhook(new_hook, do_or_err<dpp::webhook>(
        [&bot, &chan](const dpp::webhook& wh) {
            bot.message_create(dpp::message(chan, ":link: Webhook " + api::names::webhook + " created."));
        },
        [&bot, &chan](const std::string& err) {
            bot.message_create(dpp::message(chan, ":exclamation: Webhook " + api::names::webhook + " could not be created."));
        }
    ));
}