#include "db_utils.h"

void make_table(SQLite::Database &db) {
    db.exec (
        "CREATE TABLE IF NOT EXISTS channels ("
        "    id BLOB NOT NULL PRIMARY KEY,"
        "    url TEXT NOT NULL,"
        ");"
    );
}

void upsert_row(SQLite::Database& db, std::string id, std::string url) {
    db.exec (
        "INSERT INTO channels(id, url) VALUES ('"+id+"', '"+url+"')"
        "  ON CONFLICT(id) DO UPDATE SET url='"+url+"';"
    );
}

void delete_row(SQLite::Database &db, std::string id)
{
    db.exec (
        "DELETE FROM channels WHERE id='"+id+"';"  
    );
}
