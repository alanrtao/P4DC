#ifndef SETTING_H
#define SETTING_H
#include <string>
#include <SQLiteCpp/SQLiteCpp.h>

class Setting {
public:
    std::string webhook;
    std::string role_id;
    Setting(SQLite::Column webhook, SQLite::Column role_id);
};

#endif