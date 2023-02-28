#include "db_utils.h"
#include <exception>
#include <iostream>

void db::make_roles_table (SQLite::Database &db) {
    try {
        db.exec (
            "CREATE TABLE IF NOT EXISTS roles (\n"
            "    id BLOB NOT NULL PRIMARY KEY,\n"
            "    role BLOB NOT NULL\n"
            ");\n"
        );
    } catch (const std::exception& e) {
        std::cout<<e.what()<<std::endl;
    }
    
}

void db::upsert_role (SQLite::Database& db, std::string id, std::string role) {
    try {
        db.exec (
            "INSERT INTO roles(id, role) VALUES ('"+id+"', '"+role+"')\n"
            "  ON CONFLICT(id) DO UPDATE SET role='"+role+"';\n"
        );
    } catch (const std::exception& e) {
        std::cout<<e.what()<<std::endl;
    }
    
}

void db::delete_role (SQLite::Database &db, std::string id)
{
    try {
        db.exec (
            "DELETE FROM roles WHERE id='"+id+"';\n"  
        );
    } catch (const std::exception& e) {
        std::cout<<e.what()<<std::endl;
    }
}

db::result db::get_role (SQLite::Database &db, std::string id) {
    try {
        SQLite::Statement query(db, "SELECT * FROM roles WHERE id = '"+id+"' LIMIT 1;\n");
        while(query.executeStep()) {
            std::string role = query.getColumn(1);
            return { role };
        }
    } catch (const std::exception& e) {
        return { };
    }
}

void db::make_webhooks_table (SQLite::Database &db) {
    try {
        db.exec (
            "CREATE TABLE IF NOT EXISTS webhooks (\n"
            "    id BLOB NOT NULL PRIMARY KEY,\n"
            "    url TEXT NOT NULL\n"
            ");\n"
        );
    } catch (const std::exception& e) {
        std::cout<<e.what()<<std::endl;
    }
}

void db::upsert_webhook (SQLite::Database& db, std::string id, std::string url) {
    try {
        db.exec (
            "INSERT INTO webhooks(id, url) VALUES ('"+id+"', '"+url+"')\n"
            "  ON CONFLICT(id) DO UPDATE SET url='"+url+"';\n"
        );
    } catch (const std::exception& e) {
        std::cout<<e.what()<<std::endl;
    }
}

void db::delete_webhook (SQLite::Database &db, std::string id)
{
    try {
        db.exec (
            "DELETE FROM webhooks WHERE id='"+id+"';\n"
        );
    } catch (const std::exception& e) {
        std::cout<<e.what()<<std::endl;
    }
}

db::result db::get_webhook (SQLite::Database &db, std::string id) {
    try {
        SQLite::Statement query(db, "SELECT * FROM webhooks WHERE id = '"+id+"' LIMIT 1;\n");
        while(query.executeStep()) {
            std::string url = query.getColumn(1);
            return { url };
        }
    } catch (const std::exception& e) {
        return {};
    }
}