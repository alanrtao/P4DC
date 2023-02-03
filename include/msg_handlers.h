#ifndef MSG_HANDLERS_H
#define MSG_HANDLERS_H

#include "api.h"

namespace api {
    using msg_trigger_t = std::function<bool(const dpp::message_create_t&)>;
    using msg_handle_t = std::function<void(const dpp::message_create_t&, dpp::cluster& bot)>;

    namespace msg_handler_calls {
        bool pull_request_trigger(const dpp::message_create_t& event);
        void pull_request_call(const dpp::message_create_t& event, dpp::cluster& bot);
    }

    struct msg_handler {
    public:
        const msg_trigger_t trigger;
        const msg_handle_t handle;
        msg_handler(const msg_trigger_t& trigger, const msg_handle_t& handle) : trigger{trigger}, handle{handle} {}
    };

    const msg_handler
    pull_request(
        msg_handler_calls::pull_request_trigger,
        msg_handler_calls::pull_request_call
    );
}

#endif