#include "hydratable.h"
#include <string_view>
#include <fstream>
#include <iostream>
#include "text_utils.h"

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
        current = text::replace_all(current, pattern, value);
    }

    return current;
}