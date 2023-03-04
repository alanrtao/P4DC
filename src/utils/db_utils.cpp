#include "utils/db_utils.h"
#include <exception>
#include <iostream>
#include <mutex>

// TODO: compile sql in threadsafe mode
std::mutex db_mutex;

db::result_t make_table(SQLite::Database& db, std::string spec);
db::result_t edit(SQLite::Database& db, SQLite::Statement& query);
db::result_t get_by_id (SQLite::Database& db, const std::string& table, const std::string& id);

db::result_t db::make_roles_table (SQLite::Database &db) {
    return make_table(
        db,
        "CREATE TABLE IF NOT EXISTS roles (\n"
        "    id BLOB NOT NULL PRIMARY KEY,\n"
        "    role BLOB NOT NULL"
        ");");
}

db::result_t db::upsert_role (SQLite::Database& db, const std::string& id, const std::string& role) {
    SQLite::Statement query { 
        db,
        "INSERT INTO roles(id, role) VALUES (?, ?)\n"
        "  ON CONFLICT(id) DO UPDATE SET role=?;"
    };
    query.bind(1, id);
    query.bind(2, role);
    query.bind(3, role);

    return edit(db, query);
}

db::result_t db::delete_role (SQLite::Database &db, const std::string& id)
{
    SQLite::Statement query {
        db,
        "DELETE FROM roles WHERE id=?;"
    };
    query.bind(1, id);
    
    return edit(db, query);
}

db::result_t db::get_role (SQLite::Database &db, const std::string& id) {
    return get_by_id(db, "roles", id);
}

db::result_t db::make_webhooks_table (SQLite::Database &db) {
    return make_table(
        db,
        "CREATE TABLE IF NOT EXISTS webhooks (\n"
        "    id BLOB NOT NULL PRIMARY KEY,\n"
        "    url TEXT NOT NULL"
        ");"
    );
}

db::result_t db::upsert_webhook (SQLite::Database& db, const std::string& id, const std::string& url) {
    SQLite::Statement query {
        db,
        "INSERT INTO webhooks(id, url) VALUES (?, ?)\n"
        "  ON CONFLICT(id) DO UPDATE SET url=?;"
    };
    query.bind(1, id);
    query.bind(2, url);
    query.bind(3, url);

    return edit(db, query);
}

db::result_t db::delete_webhook (SQLite::Database &db, const std::string& id)
{
    SQLite::Statement query {
        db,
        "DELETE FROM webhooks WHERE id=?;"
    };
    query.bind(1, id);

    return edit(db, query);
}

db::result_t db::get_webhook (SQLite::Database &db, const std::string& id) {
    return get_by_id(db, "webhooks", id);
}

db::result_t db::make_pr_formats_table(SQLite::Database& db) {
    return make_table(
        db,
        "CREATE TABLE IF NOT EXISTS formats (\n"
        "    id BLOB NOT NULL PRIMARY KEY,\n"
        "    content TEXT NOT NULL"
        ");"
    );
}

db::result_t db::upsert_pr_format(SQLite::Database& db, const std::string& id, const std::string& content) {

    if (content.length() > 500) {
        return db::result_t::make_error("Pull quest format longer than 500 characters.");
    }

    SQLite::Statement query {
        db,
        "INSERT INTO formats(id, content) VALUES (?, ?)\n"
        "  ON CONFLICT(id) DO UPDATE SET content=?;"
    };
    query.bind(1, id);
    query.bind(2, content);
    query.bind(3, content);

    return edit(db, query);
}

db::result_t db::delete_pr_format(SQLite::Database& db, const std::string& id) {
    SQLite::Statement query {
        db,
        "DELETE FROM formats WHERE id=?;"
    };
    query.bind(1, id);

    return edit(db, query);
}

db::result_t db::get_pr_format(SQLite::Database& db, const std::string& id) {
    return get_by_id(db, "formats", id);
}

db::result_t make_table(SQLite::Database& db, std::string spec) {
    const std::lock_guard<std::mutex> lock(db_mutex);
    try {
        SQLite::Transaction transaction(db);
        db.exec (spec);
        transaction.commit();

        return db::result_t::make_result({});
    } catch (const std::exception& e) {
        return db::result_t::make_error(e.what());
    }
}

db::result_t edit(SQLite::Database& db, SQLite::Statement& query) {
    const std::lock_guard<std::mutex> lock(db_mutex);
    try {
        SQLite::Transaction transaction(db);
        query.exec();
        transaction.commit();
        return db::result_t::make_result({});
    } catch (const std::exception& e) {
        return db::result_t::make_error(e.what());
    }
}

db::result_t get_by_id (SQLite::Database& db, const std::string& table, const std::string& id) {
    const std::lock_guard<std::mutex> lock(db_mutex);
    try {
        SQLite::Statement query(db, "SELECT * FROM "+table+" WHERE id = ? LIMIT 1;");
        query.bind(1, id);
        while(query.executeStep()) {
            std::string val = query.getColumn(1);
            return db::result_t::make_result(val);
        }
        
    } catch (const std::exception& e) {
        return db::result_t::make_error(e.what());
    }
}