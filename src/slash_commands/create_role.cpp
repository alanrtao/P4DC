#include "api.h"
#include "slash_commands.h"
#include "utils.h"

// note: no longer exposed, reference from /route_here

void api::slash_command_calls::create_role_call (const dpp::slashcommand_t& event, dpp::cluster& bot) {
    const auto channel{event.command.get_channel()};
    const auto guild{event.command.get_guild()};
    const auto user{event.command.get_issuing_user()};

    const std::string append = 
        "**... creating `" + api::names::role + "` role for "+guild.name+"**\n"
        "... you will receive a confirmation when the role is created.";
    
    bot.message_create(dpp::message(channel.id, append));
    
    bot.roles_get(guild.id, [&bot, &guild, &channel, &user](const auto& cb){
        if (cb.is_error()) {
            bot.log(dpp::loglevel::ll_error, "Could not read roles: " + cb.get_error().message);
            bot.message_create(dpp::message(channel.id, ":exclamation: Could not read roles in this server."));
        } else {
            auto roles{std::get<dpp::role_map>(cb.value)};
            bool guild_has_role = false;
            for(const auto& [k, role] : roles) {
                guild_has_role = examine_existing_role(bot, role, channel, guild_has_role);
                if (guild_has_role) break; // no need to examine the rest, since roles do not need to be deleted
            }

            if (!guild_has_role)
                do_create_role(bot, guild, channel, user);
            else
                bot.message_create(dpp::message(channel.id, "The `" + api::names::role + "` role already exists on this server."));
        }
    });
}

bool is_my_role(dpp::cluster& bot, const dpp::role& r) {
    return r.name == api::names::role;
}

bool is_my_role(dpp::cluster& bot, const dpp::role& r, const dpp::channel& channel) {
    if (r.name == api::names::role) {
        if (r.is_mentionable()) {
            return true;
        } else {
            bot.message_create(dpp::message(channel.id, "The role `" + r.name + "` exists but is not mentionable."));
        }
    }
    return false;
}

bool examine_existing_role (dpp::cluster& bot, const dpp::role& r, const bool guild_has_role) {
    return is_my_role(bot, r); // does not care about duplicate
}

bool examine_existing_role (dpp::cluster& bot, const dpp::role& r, const dpp::channel& channel, const bool guild_has_role) {
    return is_my_role(bot, r, channel); // does not care about duplicate
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

            bot.guild_member_add_role(guild.id, user.id, role.id, [&bot, &channel, &role, &user](const auto& cb) {
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