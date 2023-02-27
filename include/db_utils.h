#ifndef DB_UTILS_H
#define DB_UTILS_H

#include "SQLiteCpp/SQLiteCpp.h"
#include <string.h>

void make_table(SQLite::Database& db);
void upsert_row(SQLite::Database& db, std::string id, std::string url);
void delete_row(SQLite::Database& db, std::string id);

#endif