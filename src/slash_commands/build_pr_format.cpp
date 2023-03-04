#include "api.h"
#include "slash_commands.h"
#include "utils/api_utils.h"
#include "utils/db_utils.h"
#include "utils/modal_utils.h"

#include <archive.h>
#include <archive_entry.h>

void api::slash_command_calls::build_pr_format_call (const dpp::slashcommand_t& event, dpp::cluster& bot, SQLite::Database& db) {
    const auto channel { event.command.get_channel() };

    event.dialog(
        dpp::interaction_modal_response("p4dc_pr_format", "Edit Pull Request Default")
        .add_component(modal::make_pr_channel_id_field( std::to_string(channel.id) ))
        .add_row()
        .add_component(modal::make_pr_content_field("Edit default content here").set_required(false))
    );
}