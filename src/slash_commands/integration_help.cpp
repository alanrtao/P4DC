#include "api.h"
#include "slash_commands.h"
#include "api_utils.h"

const std::string make_integration_instruction(const hydratable::context& context);
const std::pair<std::string, std::string> make_integration_file(const hydratable::context& context, const std::vector<hydratable>& files);

hydratable
    integration_instructions = hydratable::make_hydratable (
        api::paths::instructions,
        {
            api::patterns::depot
        }
    ),
    commit_trigger = hydratable::make_hydratable (
        api::paths::commit_trigger,
        {
            api::patterns::webhook
        }
    ),
    branch_trigger = hydratable::make_hydratable (
        api::paths::branch_trigger,
        {
            api::patterns::webhook,
            api::patterns::role
        }
    );

void api::slash_command_calls::integration_help_call (const dpp::slashcommand_t& event, dpp::cluster& bot) {
    const auto guild { event.command.get_guild() };
    const auto channel { event.command.get_channel() };
    const auto user { event.command.get_issuing_user() };

    // TODO: permission check
    std::string depot = "";
    try {
        depot = std::get<std::string>(event.get_parameter("depot"));
    } catch (std::bad_variant_access const& ex) {
        bot.log(dpp::ll_error, "No depot parameter");
    }

    if (depot.length() == 0) {
        event.reply(":exclamation: No depot specified, terminating...");
        return;
    }

    event.reply("Generating integration code, please wait...");

    bot.roles_get(guild.id, [&bot, channel, guild, user, depot](const auto& cb) {

        if (cb.is_error()) {
            bot.message_create(dpp::message(channel.id, ":exclamation: Could not read roles in this server."));
            return;
        }

        const auto roles { std::get<dpp::role_map>(cb.value) };
        const auto role_it = find_my_role(bot, channel, roles);
        if (role_it == roles.end()) {
            bot.message_create(dpp::message(channel.id, ":exclamation: Please run `" + api::route_here.route + "` first to create `" + api::names::role + "` role."));
            return;
        }

        bot.message_create(dpp::message(channel.id, "... proceeding with role <@&" + std::to_string(role_it->first) + ">"));
        bot.get_channel_webhooks(channel.id, [&bot, channel, role = role_it->second, user, depot](const auto& cb) {\

            if (cb.is_error()) {
                bot.log(dpp::loglevel::ll_error, "Could not read webhooks: " + cb.get_error().message);
                bot.message_create(dpp::message(channel.id, ":exclamation: Could not read webhooks from channel."));
                return;
            }

            const auto webhooks { std::get<dpp::webhook_map>(cb.value) };
            const auto webhook_it = find_my_webhook(bot, channel, webhooks);

            if (webhook_it == webhooks.end()) {
                bot.message_create(dpp::message(channel.id, ":exclamation: Please run `" + api::route_here.route + "` first to create `" + api::names::webhook + "` webhook."));
                return;
            }

            const auto webhook { webhook_it->second };

            bot.message_create(dpp::message(channel.id, "... proceeding with webhook `" + api::names::webhook + "`"));

            do_integration_help(
                bot, hydratable::context {
                    { api::patterns::role, std::to_string(role.id) },
                    { api::patterns::webhook, api::webhooks_root + std::to_string(webhook.id) + "/" + webhook.token },
                    { api::patterns::depot, depot }
                },
                user);
        });
    });
}

void do_integration_help(dpp::cluster& bot, const hydratable::context& context, const dpp::user& user) {
    const auto& [fname, fcontent] = make_integration_file(context, {
        commit_trigger,
        branch_trigger
    });

    dpp::message reply {make_integration_instruction(context)};
    reply.add_file(fname, fcontent);

    bot.direct_message_create(user.id, reply);
}

const std::string make_integration_instruction(const hydratable::context& context) {
    return "";
}

const std::pair<std::string, std::string> make_integration_file(const hydratable::context& context, const std::vector<hydratable>& files) {

    const auto depot = context.at(api::patterns::depot);
    return {depot, ""};
}