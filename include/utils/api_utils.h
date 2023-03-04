#ifndef API_UTILS_H
#define API_UTILS_H

#include <SQLiteCpp/SQLiteCpp.h>
#include <dpp/dpp.h>
#include "hydratable.h"

bool is_my_role(dpp::cluster& bot, const dpp::role& r);
bool is_my_role(dpp::cluster& bot, const dpp::role& r, const dpp::channel& channel, const SQLite::Database& db);
void do_create_role (dpp::cluster& bot, const dpp::guild& guild, const dpp::channel& channel, const dpp::user& user, SQLite::Database& db);
const dpp::role_map::const_iterator find_my_role(dpp::cluster& bot, const dpp::channel& channel, const dpp::role_map& map, SQLite::Database& db);

bool is_my_webhook(const dpp::cluster& bot, const dpp::webhook& wh);
void do_create_webhook (dpp::cluster& bot, const dpp::channel& channel, SQLite::Database& db);
const dpp::webhook_map::const_iterator find_my_webhook(dpp::cluster& bot, const dpp::channel& channel, const dpp::webhook_map& map, SQLite::Database& db);

void do_integration_help(dpp::cluster& bot, const hydratable::context& context, const dpp::user& user);

void ping_reviewers(dpp::cluster& bot, const dpp::message_create_t& event, const dpp::thread& thread);

bool is_admin(dpp::cluster& bot, const dpp::slashcommand_t& event);

#endif