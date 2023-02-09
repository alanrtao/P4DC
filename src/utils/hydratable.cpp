#include "hydratable.h"
#include <string_view>
#include <fstream>
#include <iostream>

const hydratable hydratable::make_hydratable(const std::string& fname, const std::unordered_set<std::string>& patterns) {
    using path = std::filesystem::path;
    std::ifstream ifs { path { api::paths::hydratable_home } / fname };
    return { 
        fname,
        // https://stackoverflow.com/questions/2602013/read-whole-ascii-file-into-c-stdstring
        std::string { (std::istreambuf_iterator<char>(ifs)), (std::istreambuf_iterator<char>()) }, 
        patterns 
    };
}

const std::string hydratable::hydrate(const hydratable::context& context) const {
    auto current = raw;
    for (const auto& [pattern, value] : context) {
        if (pattern.length() == 0 || patterns.find(pattern) == patterns.end()) {
            continue;
        }

        // replace all
        std::string swap{};
        swap.reserve(raw.size() * 2);

        std::string_view remain { current };
        auto p = remain.find(pattern);
        while (p != remain.npos) {
            swap.insert(swap.end(), remain.begin(), remain.begin() + p); // add up to first occurrence
            swap.insert(swap.end(), value.begin(), value.end()); // add value

            remain = std::string_view(remain.begin() + p + pattern.length());
            p = remain.find(pattern);
        }
        swap.insert(swap.end(), remain.begin(), remain.end());

        current = swap;
    }

    return current;
}