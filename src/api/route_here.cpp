#include "api.h"

bool examine_existing_webhook (dpp::cluster& bot, const dpp::webhook& wh, dpp::snowflake chan, bool channel_has_webhook);
void do_create_webhook (dpp::cluster& bot, const dpp::snowflake chan);

void api::calls::route_here_call (const dpp::slashcommand_t& event, dpp::cluster& bot) {
    const auto channel{event.command.get_channel()};
    const auto guild{event.command.get_guild()};

    event.reply(
        ":anchor: P4DC routed to "+channel.name+"  \n"
        "Make sure to run /"+api::create_role.route+" and /"+api::integration_help.route+" to create custom scripts for integrating Perforce triggers.\n"
        ":point_right: I will send a message below if a webhook has been found / created...");


    const auto chan{channel.id};
    bot.get_guild_webhooks(guild.id, [&bot, &chan](auto cb) {
        if (cb.is_error()) {
            bot.log(dpp::loglevel::ll_error, "Could not read webhooks: " + cb.get_error().message);
            bot.message_create(dpp::message(chan, ":exclamation: Could not read webhooks from channel."));
        } else {
            auto m{std::get<dpp::webhook_map>(cb.value)};
            bool channel_has_webhook = false;
            for(const auto& [k, v] : m)
                channel_has_webhook = examine_existing_webhook(bot, v, chan, channel_has_webhook);
            if (!channel_has_webhook)
                do_create_webhook(bot, chan);
            else
                bot.message_create(dpp::message(chan, "Webhook already exists on this channel."));
        }
    });
}

bool is_my_webhook(const dpp::cluster& bot, const dpp::webhook& wh) {
    return wh.name == api::names::webhook && wh.application_id == bot.me.id;
}

bool examine_existing_webhook (dpp::cluster& bot, const dpp::webhook& wh, dpp::snowflake chan, bool channel_has_webhook) {
    if (is_my_webhook(bot, wh)) {
        if (wh.channel_id == chan && !channel_has_webhook) {
            channel_has_webhook = true;
        }
        else {
            bot.message_create(dpp::message(wh.channel_id, ":point_right: " + api::names::webhook + " no longer needed and has been removed."));
            bot.delete_webhook(wh.id);
        }
    }
    return channel_has_webhook;
}

void do_create_webhook (dpp::cluster& bot, const dpp::snowflake chan) {
    dpp::webhook new_hook{};
    new_hook.name = api::names::webhook;
    new_hook.application_id = bot.me.id;
    new_hook.channel_id = chan;

    bot.create_webhook(new_hook, [&bot, &chan](auto cb) {
        if (cb.is_error()) {
            bot.log(dpp::loglevel::ll_error, cb.get_error().message);
            bot.message_create(dpp::message(chan, ":exclamation: Webhook " + api::names::webhook + " could not be created."));
        } else {
            auto wh{std::get<dpp::webhook>(cb.value)};
            bot.message_create(dpp::message(wh.channel_id, ":link: Webhook " + api::names::webhook + " created."));
        }
    });
}