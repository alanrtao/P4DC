#include "api.h"
#include "slash_commands.h"
#include "api_utils.h"

// note: application_id no longer set. bot only reacts to non-bot messages

void api::slash_command_calls::route_here_call (const dpp::slashcommand_t& event, dpp::cluster& bot) {
    const auto channel { event.command.get_channel() };
    const auto guild { event.command.get_guild() };
    const auto user { event.command.get_issuing_user() };

    event.reply(
        ":anchor: P4DC routed to "+channel.name+"  \n"
        "**... creating webhook `" + api::names::webhook + "` for this channel**\n"
        "**... creating role `" + api::names::role + "` for this channel**\n"
        "... if you do not receive confirmations for both, run this command again.");

    bot.get_channel_webhooks(channel.id, [&bot, channel](const auto& cb) {
        if (cb.is_error()) {
            bot.message_create(dpp::message(channel.id, ":exclamation: Could not read webhooks from channel."));
            return;
        }

        auto webhooks{ std::get<dpp::webhook_map>(cb.value) };
        const auto webhook_it = find_my_webhook(bot, channel, webhooks);
        if (webhook_it == webhooks.end()) {
            do_create_webhook(bot, channel);
        } else {
            bot.message_create(dpp::message(channel.id, ":point_right: Webhook `" + api::names::webhook + "` already exists on this channel."));
        }
    });

    // role
    bot.roles_get(guild.id, [&bot, guild, channel, user](const auto& cb){

        if (cb.is_error()) {
            bot.log(dpp::loglevel::ll_error, "Could not read roles: " + cb.get_error().message);
            bot.message_create(dpp::message(channel.id, ":exclamation: Could not read roles in this server."));
            return;
        }

        auto roles{ std::get<dpp::role_map>(cb.value) };
        const auto role_it = find_my_role(bot, channel, roles);
        if (role_it == roles.end()) {
            do_create_role(bot, guild, channel, user);
        } else {
            bot.message_create(dpp::message(channel.id, ":point_right: The `" + api::names::role + "` role already exists on this server."));
        }
    });
}

bool is_my_webhook(const dpp::cluster& bot, const dpp::webhook& wh) {
    return wh.name == api::names::webhook; // && wh.application_id == bot.me.id;
}

const dpp::webhook_map::const_iterator find_my_webhook(dpp::cluster& bot, const dpp::channel& channel, const dpp::webhook_map& map) {
    bool channel_has_webhook {};
    auto find = map.end();
    for (auto i = map.begin(); i != map.end(); ++i) {
        if (is_my_webhook(bot, i->second)) {
            if (channel_has_webhook) {
                bot.message_create(dpp::message(channel.id, ":point_right: Delete duplicate webhook"));
                bot.delete_webhook(i->second.id);
            } else {
                find = i;
                channel_has_webhook = true;
            }
        }
    }
    return find;
}

dpp::webhook make_hook(const dpp::cluster& bot, const dpp::channel& channel) {
    dpp::webhook new_hook{};
    new_hook.name = api::names::webhook;
    // new_hook.application_id = bot.me.id;
    new_hook.channel_id = channel.id;

    return new_hook;
}

void do_create_webhook (dpp::cluster& bot, const dpp::channel& channel) {
    bot.create_webhook(make_hook(bot, channel), [&bot, channel](const auto& cb) {
        if (cb.is_error()) {
            bot.log(dpp::loglevel::ll_error, cb.get_error().message);
            bot.message_create(dpp::message(channel.id, ":exclamation: Webhook `" + api::names::webhook + "` could not be created."));
        } else {
            auto wh{std::get<dpp::webhook>(cb.value)};
            // std::cout<<api::webhooks_root<<wh.id<<"/"<<wh.token<<std::endl;
            bot.message_create(dpp::message(wh.channel_id, ":link: Webhook `" + api::names::webhook + "` created."));
        }
    });
}

bool is_my_role(dpp::cluster& bot, const dpp::role& r) {
    return r.name == api::names::role;
}

bool is_my_role(dpp::cluster& bot, const dpp::role& r, const dpp::channel& channel) {
    if (is_my_role(bot, r)) {
        if (r.is_mentionable() || r.bot_id != 0) {
            return true;
        } else {
            bot.message_create(dpp::message(channel.id, 
                "Deleting invalid role `" + r.name + "`: not mentionable / is a bot webhook "
                "(P4DC needs to read webhook message content, while Discord forbids bots from reading bot webhook contents)."));
            bot.role_delete(r.guild_id, r.id);
        }
    }
    return false;
}

const dpp::role_map::const_iterator find_my_role(dpp::cluster& bot, const dpp::channel& channel, const dpp::role_map& map) {
    bool guild_has_role {};
    auto find = map.end();
    for (auto i = map.begin(); i != map.end(); ++i) {
        if (is_my_role(bot, i->second, channel)) {
            if (guild_has_role) {
                bot.message_create(dpp::message(channel.id, ":point_right: Delete duplicate role"));
                bot.role_delete(i->second.guild_id, i->second.id);
            } else {
                find = i;
                guild_has_role = true;
            }
        }
    }
    return find;
}

dpp::role make_role (const dpp::guild& guild) {
    dpp::role new_role{};
    new_role.name = api::names::role;
    new_role.guild_id = guild.id;
    new_role.flags = dpp::role_flags::r_managed | dpp::role_flags::r_mentionable;
    return new_role;
}

void do_create_role (dpp::cluster& bot, const dpp::guild& guild, const dpp::channel& channel, const dpp::user& user) {
    bot.role_create(make_role(guild), [&bot, &guild, &channel, &user](const auto& cb) {
        if (cb.is_error()) {
            bot.log(dpp::loglevel::ll_error, cb.get_error().message);
            bot.message_create(dpp::message(channel.id, ":exclamation: Role `" + api::names::role + "` could not be created."));
        } else {
            auto role{std::get<dpp::role>(cb.value)};

            bot.guild_member_add_role(guild.id, user.id, role.id, [&bot, channel, role, user](const auto& cb) {
                if (cb.is_error()) {
                    bot.message_create(dpp::message(channel.id, "... You <@!" + std::to_string(user.id) + "> could not be added to the role automatically."));
                } else {
                    bot.message_create(dpp::message(channel.id, "... You (<@!" + std::to_string(user.id) + ">) have been added to the role."));
                }
            });
            
            bot.message_create(dpp::message(channel.id, ":link: Role <@&" + std::to_string(role.id) + "> created."));
        }
    });
}