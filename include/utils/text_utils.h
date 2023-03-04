#ifndef TEXT_UTILS_H
#define TEXT_UTILS_H

#include <string>

namespace text {
    const std::string replace_all (const std::string& src, const std::string& pattern, const std::string& value);
    const std::string remove_xml_comments(const std::string& src);
    void remove_xml_comments_inplace(std::string& src);
}

#endif