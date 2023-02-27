#ifndef DB_UTILS_H
#define DB_UTILS_H

#include "SQLiteCpp/SQLiteCpp.h"
#include <string>
#include <variant>

namespace db {

    class result {
    public:
        bool is_error;
    private:
        std::string value;

    public:
        result() : is_error{true}, value{}{}
        result(std::string value) : is_error{false}, value{value}{}

        const std::string get_result() const { return value; }
    };

    void make_webhooks_table(SQLite::Database& db);
    void upsert_webhook(SQLite::Database& db, std::string id, std::string url);
    void delete_webhook(SQLite::Database& db, std::string id);
    result get_webhook(SQLite::Database& db, std::string id);

    void make_roles_table(SQLite::Database& db);
    void upsert_role(SQLite::Database& db, std::string id, std::string role);
    void delete_role(SQLite::Database& db, std::string id);
    result get_role(SQLite::Database& db, std::string id);
}

#endif