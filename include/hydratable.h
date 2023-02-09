#ifndef HYDRATABLE_H
#define HYDRATABLE_H

#include "api.h"
#include <string>
#include <vector>
#include <unordered_map>
#include <unordered_set>
#include <filesystem>

struct hydratable {
public:

    using context = std::unordered_map<std::string, std::string>;

    const std::string fname;
    const std::string raw;
    const std::unordered_set<std::string> patterns;

    const std::string hydrate(const context& context) const;

    static const hydratable make_hydratable(const std::string& fname, const std::unordered_set<std::string>& patterns);

private:
    hydratable (
        const std::string& fname,
        const std::string& raw,
        const std::unordered_set<std::string> patterns
    ) : fname { fname }, raw { raw }, patterns { patterns } {}

};

#endif