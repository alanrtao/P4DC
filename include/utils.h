#ifndef UTILS_H
#define UTILS_H

#include <dpp/dpp.h>

bool is_my_role(dpp::cluster& bot, const dpp::role& r);
bool is_my_role(dpp::cluster& bot, const dpp::role& r, const dpp::channel& channel);
void do_create_role (dpp::cluster& bot, const dpp::guild& guild, const dpp::channel& channel, const dpp::user& user);
const dpp::role_map::const_iterator find_my_role(dpp::cluster& bot, const dpp::channel& channel, const dpp::role_map& map);

bool is_my_webhook(const dpp::cluster& bot, const dpp::webhook& wh);
void do_create_webhook (dpp::cluster& bot, const dpp::channel& channel);
const dpp::webhook_map::const_iterator find_my_webhook(dpp::cluster& bot, const dpp::channel& channel, const dpp::webhook_map& map);

void do_integration_help(dpp::cluster& bot, const dpp::role& role, const dpp::webhook& webhook, const dpp::channel& channel);

void ping_reviewers(dpp::cluster& bot, const dpp::message_create_t& event, const dpp::thread& thread);

#endif