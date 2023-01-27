#include "api.h"

bool examine_existing_role (dpp::cluster& bot, const dpp::role& r, bool guild_has_role);
void do_create_role (dpp::cluster& bot, const dpp::guild& guild, const dpp::channel& channel);

void api::calls::create_role_call (const dpp::slashcommand_t& event, dpp::cluster& bot) {
    const auto channel{event.command.get_channel()};
    const auto guild{event.command.get_guild()};

    event.reply(
        "... creating " + api::names::role + " role for "+guild.name+"  \n"
        "Make sure to run /"+api::integration_help.route+" to create custom scripts for integrating Perforce triggers.\n"
        );
    
    bot.roles_get(guild.id, [&bot, &guild, &channel](auto cb){
        if (cb.is_error()) {
            bot.log(dpp::loglevel::ll_error, "Could not read roles: " + cb.get_error().message);
            bot.message_create(dpp::message(channel.id, ":exclamation: Could not read roles in this server."));
        } else {
            auto m{std::get<dpp::role_map>(cb.value)};
            bool guild_has_role = false;
            for(const auto& [k, v] : m)
                guild_has_role = examine_existing_role(bot, v, guild_has_role);

            if (!guild_has_role)
                do_create_role(bot, guild, channel);
            else
                bot.message_create(dpp::message(channel.id, "The `" + api::names::role + "` role already exists on this channel."));
        }
    });
}

bool is_my_role(const dpp::cluster& bot, const dpp::role& r) {
    return r.name == api::names::role && r.is_mentionable();
}

bool examine_existing_role (dpp::cluster& bot, const dpp::role& r, bool guild_has_role) {
    if (is_my_role(bot, r)) {
        guild_has_role = true;
    }
    return guild_has_role;
}

void do_create_role (dpp::cluster& bot, const dpp::guild& guild, const dpp::channel& channel) {
    dpp::role new_role{};

    std::cout<<"Creating role for guild: " << guild.name <<std::endl;

    new_role.name = api::names::role;
    new_role.guild_id = guild.id;
    new_role.flags = dpp::role_flags::r_managed | dpp::role_flags::r_mentionable;

    bot.role_create(new_role, [&bot, &channel](auto cb) {
        if (cb.is_error()) {
            bot.log(dpp::loglevel::ll_error, cb.get_error().message);
            bot.message_create(dpp::message(channel.id, ":exclamation: Role `" + api::names::role + "` could not be created."));
        } else {
            auto r{std::get<dpp::role>(cb.value)};
            bot.message_create(dpp::message(channel.id, ":link: Role <@&" + std::to_string(r.id) + "> created."));
        }
    });
}