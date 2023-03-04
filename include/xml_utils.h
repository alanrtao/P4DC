#ifndef XML_UTILS_H
#define XML_UTILS_H

#include <string>

namespace xml {
    std::string remove_xml_comments(const std::string& src);
}

#endif