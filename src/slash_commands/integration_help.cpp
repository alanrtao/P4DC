#include "api.h"
#include "slash_commands.h"
#include "api_utils.h"
#include "db_utils.h"
#include <archive.h>
#include <archive_entry.h>

const std::string make_integration_instruction(const hydratable::context& context);
const std::pair<std::string, std::string> make_integration_file(const hydratable::context& context);

void api::slash_command_calls::integration_help_call (const dpp::slashcommand_t& event, dpp::cluster& bot, SQLite::Database& db) {
    const auto guild { event.command.get_guild() };
    const auto channel { event.command.get_channel() };
    const auto user { event.command.get_issuing_user() };

    if (!is_admin(bot, event)) {
        event.reply(":exclamation: Only server admins can run this command.");
        return;
    }

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

    auto role_result = db::get_role(db, std::to_string(guild.id));
    if (role_result.is_error) {
        bot.message_create(
            dpp::message(
                channel.id,
                ":exclamation: Please run `" + api::route_here.route + "` first to create the `" + api::names::role + "` role."));
        return;
    } else {
        bot.message_create(
            dpp::message(
                channel.id,
                "... proceeding with role <@&" + role_result.get_result() + ">"));
    }

    auto webhook_result = db::get_webhook(db, std::to_string(channel.id));
    if (webhook_result.is_error) {
        bot.message_create(
            dpp::message(
                channel.id,
                ":exclamation: Please run `" + api::route_here.route + "` first to create the `" + api::names::webhook + "`webhook"));
    } else {
        bot.message_create(
            dpp::message(
                channel.id,
                "... proceeding with webhook `" + webhook_result.get_result() + "`"));
    }

    do_integration_help(
        bot, hydratable::context {
            { api::patterns::role, role_result.get_result() },
            { api::patterns::webhook, webhook_result.get_result() },
            { api::patterns::depot, depot }
        },
        user);
}

void do_integration_help(dpp::cluster& bot, const hydratable::context& context, const dpp::user& user) {
    const auto& [fname, fcontent] = make_integration_file(context);

    dpp::message reply {make_integration_instruction(context)};
    reply.add_file(fname, fcontent);

    bot.direct_message_create(user.id, reply);
}

const std::string make_integration_instruction(const hydratable::context& context) {

    static hydratable integration_instructions = hydratable::make_hydratable (
        api::paths::instructions,
        {
            api::patterns::depot
        }
    );

    const std::string preemble =
        "*This is a reply from your `" + api::integration_help.route + "` call"
        " containing code generated for your depot `" + context.at(api::patterns::depot) + "`*.\n"
        "Please **inspect** the code before running integration steps below, as P4DC does not guarantee them to be side-effect free.\n"
        "Please also **notify** your server admin that you are running generated code on their server.\n"
        "To install..."
        "\n  1. Extract the compressed archive inside your depot and submit changes."
        "  2. Run `p4 triggers`, a text editor should open showing a list of triggers currently configured in your server."
        " Paste the following snippet into it and save. (If you are not a server admin, let your server admin complete this step)";
    return preemble + "```" + integration_instructions.hydrate(context) + "```";
}

const std::pair<std::string, std::string> make_integration_file(const hydratable::context& context) {

    static hydratable
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
    static std::vector<hydratable> files { 
        commit_trigger,
        branch_trigger
    };

    const auto depot = context.at(api::patterns::depot);
    const std::string name { depot + "-p4dc-v" + std::to_string(VERSION_MAJOR) + "." + std::to_string(VERSION_MINOR) + ".tar.gz" };

    std::unordered_map<std::string, std::string> hydrated_files {};

    size_t buf_size = 0;
    for (const auto& f : files) {
        std::string content { f.hydrate(context) };
        hydrated_files.insert({f.fname, content});
        buf_size += content.length();
    }
    char *buf = new char[buf_size];

    size_t buf_size_used = 0;

    struct archive *a;
    struct archive_entry *e;

    a = archive_write_new();
    e = archive_entry_new();

    archive_write_add_filter_gzip(a);
    archive_write_set_format_pax_restricted(a);
    archive_write_open_memory(a, buf, buf_size, &buf_size_used);
    
    for (const auto& [fname, content] : hydrated_files) {
        archive_entry_set_pathname(e, fname.c_str());
        archive_entry_set_size(e, content.length());
        archive_entry_set_filetype(e, AE_IFREG);
        archive_entry_set_perm(e, 0755);
        archive_write_header(a, e);
        
        archive_write_data(a, content.c_str(), content.length());
        archive_entry_clear(e);
    }
    archive_entry_free(e);
    archive_write_close(a);
    archive_write_free(a);

    std::string out { buf, buf + buf_size_used };

    delete[] buf;

    return { name, out };
}