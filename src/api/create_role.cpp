#include "api.h"

bool examine_existing_role (dpp::cluster& bot, const dpp::role& r, bool guild_has_role);
void do_create_role (dpp::cluster& bot, const dpp::snowflake guild, const dpp::snowflake chan);

void api::calls::create_role_call (const dpp::slashcommand_t& event, dpp::cluster& bot) {
    const auto channel{event.command.get_channel()};
    const auto guild{event.command.get_guild()};
    
    const auto chan{channel.id};
    const auto guil{guild.id};
    bot.roles_get(guild.id, [&bot, &guil, &chan](auto cb){
        if (cb.is_error()) {
            bot.log(dpp::loglevel::ll_error, "Could not read roles: " + cb.get_error().message);
            bot.message_create(dpp::message(chan, ":exclamation: Could not read roles in this server."));
        } else {
            auto m{std::get<dpp::role_map>(cb.value)};
            bool guild_has_role = false;
            for(const auto& [k, v] : m)
                guild_has_role = examine_existing_role(bot, v, guild_has_role);

            if (!guild_has_role)
                do_create_role(bot, guil, chan);
            else
                bot.message_create(dpp::message(chan, "The" + api::names::role + " already exists on this channel."));
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

void do_create_role (dpp::cluster& bot, const dpp::snowflake guild, const dpp::snowflake chan) {
    dpp::role new_role{};

    std::cout<<"Creating role for guild: " << guild <<std::endl;

    new_role.name = api::names::role;
    new_role.guild_id = guild;
    new_role.flags = dpp::role_flags::r_managed | dpp::role_flags::r_mentionable;

    bot.role_create(new_role, [&bot, &chan](auto cb) {
        if (cb.is_error()) {
            bot.log(dpp::loglevel::ll_error, cb.get_error().message);
            bot.message_create(dpp::message(chan, ":exclamation: Role " + api::names::role + " could not be created."));
        } else {
            auto r{std::get<dpp::role>(cb.value)};
            bot.message_create(dpp::message(chan, ":link: Role <@&" + std::to_string(r.id) + "> created."));
        }
    });
}