#include "text_utils.h"
#include <iostream>
#include <re2/re2.h>

const std::string text::replace_all (const std::string& src, const std::string& pattern, const std::string& value) {
    std::string temp{};
    temp.reserve(src.size() * 2);

    std::string_view remain { src };
    auto p = remain.find(pattern);
    while (p != remain.npos) {
        temp.insert(temp.end(), remain.begin(), remain.begin() + p); // add up to first occurrence
        temp.insert(temp.end(), value.begin(), value.end()); // add value

        remain = std::string_view(remain.begin() + p + pattern.length());
        p = remain.find(pattern);
    }
    temp.insert(temp.end(), remain.begin(), remain.end());

    return temp;
}

const std::string comment_re = "<!--([\\s\\S]*?)-->";

const std::string text::remove_xml_comments (const std::string& src) {
    std::string temp { src };
    re2::RE2::GlobalReplace(&temp, comment_re, "");
    return temp;
}

void text::remove_xml_comments_inplace (std::string& src) {
    re2::RE2::GlobalReplace(&src, comment_re, "");
}