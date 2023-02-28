#ifndef DB_UTILS_H
#define DB_UTILS_H

#include "SQLiteCpp/SQLiteCpp.h"
#include <string>
#include <variant>

namespace db {

    const std::string filename = "p4dc.db3";

    template <typename T>
    class result {
    public:
        const bool is_error;
        const T value;
        const std::string error;
    
    private:
        result(const bool& is_error, const T& value, const std::string& error) :
            is_error{is_error},
            value{value},
            error{error}
        {}

    public:
        static result make_result(const T& value) {
            return { false, value, {} };
        }
        static result make_error(const std::string& error) {
            return { true, {}, error };
        }
    };

    using result_t = result<std::string>;

    result_t make_webhooks_table(SQLite::Database& db);
    result_t upsert_webhook(SQLite::Database& db, const std::string& id, const std::string& url);
    result_t delete_webhook(SQLite::Database& db, const std::string& id);
    result_t get_webhook(SQLite::Database& db, const std::string& id);

    result_t make_roles_table(SQLite::Database& db);
    result_t upsert_role(SQLite::Database& db, const std::string& id, const std::string& role);
    result_t delete_role(SQLite::Database& db, const std::string& id);
    result_t get_role(SQLite::Database& db, const std::string& id);
}

#endif