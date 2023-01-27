#include "api.h"
#include "all_api_calls.h"
#include "utils.h"

void api::calls::route_here_call (const dpp::slashcommand_t& event, dpp::cluster& bot) {
    const auto channel{event.command.get_channel()};
    const auto guild{event.command.get_guild()};

    event.reply(
        ":anchor: P4DC routed to "+channel.name+"  \n"
        "**Creating webhook " + api::names::webhook + " for this channel**"
        "You will receive a confirmation when the webhook is created.");


    bot.get_guild_webhooks(guild.id, [&bot, &channel](auto cb) {
        if (cb.is_error()) {
            bot.log(dpp::loglevel::ll_error, "Could not read webhooks: " + cb.get_error().message);
            bot.message_create(dpp::message(channel.id, ":exclamation: Could not read webhooks from channel."));
        } else {
            auto m{std::get<dpp::webhook_map>(cb.value)};
            bool channel_has_webhook = false;
            for(const auto& [k, webhook] : m)
                channel_has_webhook = examine_existing_webhook(bot, webhook, channel, channel_has_webhook);
            if (!channel_has_webhook)
                do_create_webhook(bot, channel);
            else
                bot.message_create(dpp::message(channel.id, "Webhook already exists on this channel."));
        }
    });
}

bool is_my_webhook(const dpp::cluster& bot, const dpp::webhook& wh) {
    return wh.name == api::names::webhook && wh.application_id == bot.me.id;
}

bool examine_existing_webhook (dpp::cluster& bot, const dpp::webhook& wh, const dpp::channel& channel, bool channel_has_webhook) {
    if (is_my_webhook(bot, wh)) {
        if (wh.channel_id == channel.id && !channel_has_webhook) {
            channel_has_webhook = true;
        }
        else {
            bot.message_create(dpp::message(channel.id, "... Previous webhook " + api::names::webhook + " in <#" + std::to_string(wh.channel_id) + "> removed."));
            bot.message_create(dpp::message(wh.channel_id, ":luggage: " + api::names::webhook + " no longer needed and has been removed."));
            bot.delete_webhook(wh.id);
        }
    }
    return channel_has_webhook;
}

void do_create_webhook (dpp::cluster& bot, const dpp::channel& channel) {
    dpp::webhook new_hook{};
    new_hook.name = api::names::webhook;
    new_hook.application_id = bot.me.id;
    new_hook.channel_id = channel.id;

    bot.create_webhook(new_hook, [&bot, &channel](auto cb) {
        if (cb.is_error()) {
            bot.log(dpp::loglevel::ll_error, cb.get_error().message);
            bot.message_create(dpp::message(channel.id, ":exclamation: Webhook " + api::names::webhook + " could not be created."));
        } else {
            auto wh{std::get<dpp::webhook>(cb.value)};
            bot.message_create(dpp::message(wh.channel_id, ":link: Webhook " + api::names::webhook + " created."));
        }
    });
}