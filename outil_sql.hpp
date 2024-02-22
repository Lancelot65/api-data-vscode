#ifndef OUTIL_SQL_API
#define OUTIL_SQL_API

#include <sqlite3.h>
#include <fstream>
#include <utility>
#include "outil_requete.hpp"
#include <cstdio>

#include <nlohmann/json.hpp>

using json = nlohmann::json;

sqlite3 *open_database(const char *dbName);
bool tableExists(sqlite3 *db, const std::string &tableName);

void create_table(sqlite3 *db, const std::string &tableName);
void insertOHLCV(sqlite3 *db, const OHLCV &ohlc, const std::string &tableName);

bool file_exist(const std::string &filename);

void write_json(const std::string &exchange, const std::string &timeframe, __int64 start, __int64 end);

std::pair<__int64, __int64> cours_in_json(const std::string &exchange, const std::string &timeframe, json &file);

OHLCV load_from_db(sqlite3* db, const std::string& tableName, __int64 from, __int64 to);

std::pair<__int64, __int64> cours_in_json(const std::string &exchange, const std::string &timeframe, json &file);
void delete_db();
void re_init_json();
void resetTable(sqlite3* db, const std::string& tableName);

OHLCV load_and_save(OHLCV(*load_data)(__int64, __int64, std::string, std::string), const std::string _from, const std::string _to, const std::string _exchange, const std::string _timeframe);

#endif // SQL_API