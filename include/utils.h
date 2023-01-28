#ifndef UTILS_H
#define UTILS_H

#include <dpp/dpp.h>

bool is_my_role(dpp::cluster& bot, const dpp::role& r, const dpp::channel& channel);
bool examine_existing_role (dpp::cluster& bot, const dpp::role& r, const dpp::channel& channel, bool guild_has_role);
void do_create_role (dpp::cluster& bot, const dpp::guild& guild, const dpp::channel& channel, const dpp::user& user);

bool is_my_webhook(const dpp::cluster& bot, const dpp::webhook& wh);
bool examine_existing_webhook (dpp::cluster& bot, const dpp::webhook& wh, const dpp::channel& channel, bool channel_has_webhook);
void do_create_webhook (dpp::cluster& bot, const dpp::channel& channel);

void do_integration_help(dpp::cluster& bot, const dpp::role& role, const dpp::webhook& webhook, const dpp::channel& channel);

#endif