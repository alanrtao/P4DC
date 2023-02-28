#include "api.h"
#include "slash_commands.h"
#include "api_utils.h"
#include "db_utils.h"

// note: application_id no longer set. bot only reacts to non-bot messages

void api::slash_command_calls::route_here_call (const dpp::slashcommand_t& event, dpp::cluster& bot, SQLite::Database& db) {
    const auto channel { event.command.get_channel() };
    const auto guild { event.command.get_guild() };
    const auto user { event.command.get_issuing_user() };

    if (!is_admin(bot, event)) {
        event.reply(":exclamation: Sensitive webhook information disclosed, only server admins can run this command.");
        return;
    }

    event.reply(
        ":anchor: P4DC routed to "+channel.name+"  \n"
        "**... creating webhook `" + api::names::webhook + "` for this channel**\n"
        "**... creating role `" + api::names::role + "` for this channel**\n"
        "... if you do not receive confirmations for both, run this command again.");

    bot.get_channel_webhooks(channel.id, [&bot, channel, &db](const auto& cb) {
        if (cb.is_error()) {
            bot.message_create(dpp::message(channel.id, ":exclamation: Could not read webhooks from channel."));
            return;
        }

        auto webhooks{ std::get<dpp::webhook_map>(cb.value) };
        const auto webhook_it = find_my_webhook(bot, channel, webhooks, db);
        if (webhook_it == webhooks.end()) {
            do_create_webhook(bot, channel, db);
        } else {
            const auto wh = webhook_it->second;
            const auto state = db::upsert_webhook(
                db, std::to_string(channel.id),
                api::webhooks_root+std::to_string(wh.id)+"/"+wh.token);
            if (state.is_error) {
                bot.log(dpp::ll_error, state.error);
                bot.message_create(dpp::message(channel.id, ":point_right: Webhook `" + api::names::webhook + "` exists but could not be updated to the database, please run the command again."));
            } else {
                bot.message_create(dpp::message(channel.id, ":point_right: Webhook `" + api::names::webhook + "` exists on this channel."));
            }
        }
    });

    // role
    bot.roles_get(guild.id, [&bot, guild, channel, user, &db](const auto& cb){

        if (cb.is_error()) {
            bot.log(dpp::loglevel::ll_error, "Could not read roles: " + cb.get_error().message);
            bot.message_create(dpp::message(channel.id, ":exclamation: Could not read roles in this server."));
            return;
        }

        auto roles{ std::get<dpp::role_map>(cb.value) };
        const auto role_it = find_my_role(bot, channel, roles, db);
        if (role_it == roles.end()) {
            do_create_role(bot, guild, channel, user, db);
        } else {
            const auto role = role_it->second;
            const auto state = db::upsert_role(
                db, std::to_string(guild.id),
                std::to_string(role.id)
            );
            if (state.is_error) {
                bot.log(dpp::ll_error, state.error);
                bot.message_create(dpp::message(channel.id, ":point_right: The `" + api::names::role + "` role exists but could not be updated to the database. Please run the command again."));
            } else {
                bot.message_create(dpp::message(channel.id, ":point_right: The `" + api::names::role + "` role already exists on this server."));
            }
        }
    });
}

bool is_my_webhook(const dpp::cluster& bot, const dpp::webhook& wh) {
    return wh.name == api::names::webhook; // && wh.application_id == bot.me.id;
}

const dpp::webhook_map::const_iterator find_my_webhook(dpp::cluster& bot, const dpp::channel& channel, const dpp::webhook_map& map, SQLite::Database& db) {
    bool channel_has_webhook {};
    auto find = map.end();
    for (auto i = map.begin(); i != map.end(); ++i) {
        if (is_my_webhook(bot, i->second)) {
            if (channel_has_webhook) {
                // db::delete_webhook(db, std::to_string(channel.id));
                bot.delete_webhook(i->second.id);
                bot.message_create(dpp::message(channel.id, ":point_right: Delete duplicate webhook"));
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

void do_create_webhook (dpp::cluster& bot, const dpp::channel& channel, SQLite::Database& db) {
    bot.create_webhook(make_hook(bot, channel), [&bot, channel, &db](const auto& cb) {
        if (cb.is_error()) {
            bot.log(dpp::loglevel::ll_error, cb.get_error().message);
            bot.message_create(dpp::message(channel.id, ":exclamation: Webhook `" + api::names::webhook + "` could not be created."));
        } else {
            auto wh{std::get<dpp::webhook>(cb.value)};
            // std::cout<<api::webhooks_root<<wh.id<<"/"<<wh.token<<std::endl;
            const auto status = db::upsert_webhook(db, std::to_string(wh.channel_id), api::webhooks_root+std::to_string(wh.id)+"/"+wh.token);
            if (status.is_error) {
                bot.log(dpp::ll_error, status.error);
                bot.message_create(dpp::message(wh.channel_id, ":exclamation: Webhook created but could not be updated to the database, deleting..."));
                bot.delete_webhook(wh.id);
            } else {
                bot.message_create(dpp::message(wh.channel_id, ":link: Webhook `" + api::names::webhook + "` created."));
            }
        }
    });
}

bool is_my_role(dpp::cluster& bot, const dpp::role& r) {
    return r.name == api::names::role;
}

bool is_my_role(dpp::cluster& bot, const dpp::role& r, const dpp::channel& channel, SQLite::Database& db) {
    if (is_my_role(bot, r)) {
        if (r.is_mentionable() || r.bot_id != 0) {
            return true;
        } else {
            bot.message_create(dpp::message(channel.id, 
                "Deleting invalid role `" + r.name + "`: not mentionable / is a bot webhook "
                "(P4DC needs to read webhook message content, while Discord forbids bots from reading bot webhook contents)."));
            bot.role_delete(r.guild_id, r.id);
            // db::delete_role(db, std::to_string(r.guild_id));
        }
    }
    return false;
}

const dpp::role_map::const_iterator find_my_role(dpp::cluster& bot, const dpp::channel& channel, const dpp::role_map& map, SQLite::Database& db) {
    bool guild_has_role {};
    auto find = map.end();
    for (auto i = map.begin(); i != map.end(); ++i) {
        if (is_my_role(bot, i->second, channel, db)) {
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

void do_create_role (dpp::cluster& bot, const dpp::guild& guild, const dpp::channel& channel, const dpp::user& user, SQLite::Database& db) {
    bot.role_create(make_role(guild), [&bot, &guild, &channel, &user, &db](const auto& cb) {
        if (cb.is_error()) {
            bot.log(dpp::loglevel::ll_error, cb.get_error().message);
            bot.message_create(dpp::message(channel.id, ":exclamation: Role `" + api::names::role + "` could not be created."));
        } else {
            dpp::role role{std::get<dpp::role>(cb.value)};
            const auto status = db::upsert_role(db, std::to_string(role.guild_id), std::to_string(role.id));

            if (status.is_error) {
                bot.log(dpp::ll_error, status.error);
                bot.message_create(dpp::message(channel.id, ":exclamation: Role created but could not be updated to the database, deleting..."));
                bot.role_delete(guild.id, role.id);
                return;
            }

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