#include <setting.h>

Setting::Setting(SQLite::Column webhook, SQLite::Column role_id) {
    this->webhook = webhook.getString();
    this->webhook = role_id.getString();
}