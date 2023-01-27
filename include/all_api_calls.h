#ifndef ALL_API_CALLS_H
#define ALL_API_CALLS_H

#include "api.h"

namespace api {
    const api_call
    route_here{
        "route_here",
        "Specify channel for the bot to post in.",
        api::calls::route_here_call
    },
    create_role{
        "create_role",
        "Create " + api::names::role + " role.",
        api::calls::create_role_call
    },
    integration_help{
        "integration_help",
        "Download Perforce integration script for the current channel",
        api::calls::integration_help_call
    };
}

#endif