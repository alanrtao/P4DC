#ifndef FUNCTIONAL_HELPER_H
#define FUNCTIONAL_HELPER_H

#include <dpp/dpp.h>
#include <functional>

template <class T>
dpp::command_completion_event_t do_or_err(std::function<void(const T&)> f, std::function<void(const std::string&)> err) {
    return [&f, &err](const dpp::confirmation_callback_t& cb) {
        if (cb.is_error()) {
            err(cb.get_error().message);
        } else {
            f(std::get<T>(cb.value));
        }
    };
}

#endif