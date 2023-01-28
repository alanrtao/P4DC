#include "api.h"
#include "slash_commands.h"
#include "utils.h"

// note: application_id no longer set. bot only reacts to non-bot messages

void api::slash_command_calls::route_here_call (const dpp::slashcommand_t& event, dpp::cluster& bot) {
    const auto channel{event.command.get_channel()};
    const auto guild{event.command.get_guild()};

    event.reply(
        ":anchor: P4DC routed to "+channel.name+"  \n"
        "**... creating webhook `" + api::names::webhook + "` for this channel**\n"
        "... you will receive a confirmation when the webhook is created.");

    bot.get_guild_webhooks(guild.id, [&bot, &channel](const auto& cb) {
        if (cb.is_error()) {
            bot.log(dpp::loglevel::ll_error, "Could not read webhooks: " + cb.get_error().message);
            bot.message_create(dpp::message(channel.id, ":exclamation: Could not read webhooks from channel."));
        } else {
            auto webhooks{std::get<dpp::webhook_map>(cb.value)};
            bool channel_has_webhook = false;
            for(const auto& [k, webhook] : webhooks)
                channel_has_webhook = examine_existing_webhook(bot, webhook, channel, channel_has_webhook);
            if (!channel_has_webhook)
                do_create_webhook(bot, channel);
            else
                bot.message_create(dpp::message(channel.id, "Webhook already exists on this channel."));
        }
    });

    api::slash_command_calls::create_role_call(event, bot);
}

bool is_my_webhook(const dpp::cluster& bot, const dpp::webhook& wh) {
    return wh.name == api::names::webhook; // && wh.application_id == bot.me.id;
}

bool examine_existing_webhook (dpp::cluster& bot, const dpp::webhook& wh, const dpp::channel& channel, const bool channel_has_webhook) {
    if (is_my_webhook(bot, wh)) {
        if (wh.channel_id == channel.id && !channel_has_webhook) {
            return true;
        }
        else {
            bot.message_create(dpp::message(channel.id, "... Previous webhook `" + api::names::webhook + "` in <#" + std::to_string(wh.channel_id) + "> removed."));
            bot.message_create(dpp::message(wh.channel_id, ":luggage: `" + api::names::webhook + "` no longer needed and has been removed."));
            bot.delete_webhook(wh.id);
            return false;
        }
    }
    return false;
}

dpp::webhook make_hook(const dpp::cluster& bot, const dpp::channel& channel) {
    dpp::webhook new_hook{};
    new_hook.name = api::names::webhook;
    // new_hook.application_id = bot.me.id;
    new_hook.channel_id = channel.id;

    return new_hook;
}

void do_create_webhook (dpp::cluster& bot, const dpp::channel& channel) {
    bot.create_webhook(make_hook(bot, channel), [&bot, &channel](const auto& cb) {
        if (cb.is_error()) {
            bot.log(dpp::loglevel::ll_error, cb.get_error().message);
            bot.message_create(dpp::message(channel.id, ":exclamation: Webhook `" + api::names::webhook + "` could not be created."));
        } else {
            auto wh{std::get<dpp::webhook>(cb.value)};
            std::cout<<api::webhooks_root<<wh.id<<"/"<<wh.token<<std::endl;
            bot.message_create(dpp::message(wh.channel_id, ":link: Webhook `" + api::names::webhook + "` created."));
        }
    });
}