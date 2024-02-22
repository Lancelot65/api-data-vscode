#include "outil_sql.hpp"

sqlite3* open_database(const char *dbName)
{
    sqlite3 *db;
    int rc = sqlite3_open(dbName, &db);

    if (rc)
    {
        throw std::runtime_error(std::string("outil_sqlt.cpp/open_database, error open db (l.10) : \n") + sqlite3_errmsg(db));
    }

    return db;
}

void create_table(sqlite3 *db, const std::string &tableName)
{

    std::string createTableQuery = "CREATE TABLE IF NOT EXISTS " + tableName + " (" +
                                "time INTEGER,"
                                "open REAL,"
                                "high REAL,"
                                "low REAL,"
                                "close REAL,"
                                "volume REAL"
                                ");";
    const char *sql = createTableQuery.c_str();

    int rc = sqlite3_exec(db, sql, 0, 0, 0);

    if (rc != SQLITE_OK)
    {
        throw std::runtime_error(std::string("outil_sqlt.cpp/create_table, error create table (l.33) : \n") + sqlite3_errmsg(db)  + std::string("\n requetet sql : ") + createTableQuery);
    }
}

bool tableExists(sqlite3 *db, const std::string &tableName)
{

    std::string query = "SELECT name FROM sqlite_master WHERE type='table' AND name='" + tableName + "';";

    sqlite3_stmt *stmt;
    int rc = sqlite3_prepare_v2(db, query.c_str(), -1, &stmt, nullptr);

    if (rc != SQLITE_OK)
    {
        throw std::runtime_error(std::string("outil_sqlt.cpp/tableExists, error check table in db (l.47) : \n") + sqlite3_errmsg(db) + std::string("\n requetet sql : ") + query);
    }

    bool exists = false;
    if (sqlite3_step(stmt) == SQLITE_ROW)
    {
        exists = true;
    }

    sqlite3_finalize(stmt);

    return exists;
}

void insertOHLCV(sqlite3 *db, const OHLCV &ohlc, const std::string &tableName)
{
    if (!tableExists(db, tableName))
    {
        create_table(db, tableName);
    }

    std::string sql = "INSERT INTO " + tableName + " (time, open, high, low, close, volume) VALUES (?,?,?,?,?,?)";

    sqlite3_stmt *stmt;
    int result = sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, NULL);

    if (result != SQLITE_OK)
    {
        throw std::runtime_error(std::string("outil_sqlt.cpp/insertOHLCV, error prepare request (l.75) \n") + sqlite3_errmsg(db) + std::string("\n requetet sql : ") + sql);
    }

    sqlite3_exec(db, "BEGIN TRANSACTION", nullptr, nullptr, nullptr);

    for (size_t i = 0; i < ohlc.time.size(); i++)
    {
        sqlite3_bind_int64(stmt, 1, ohlc.time[i]);
        sqlite3_bind_double(stmt, 2, ohlc.open[i]);
        sqlite3_bind_double(stmt, 3, ohlc.high[i]);
        sqlite3_bind_double(stmt, 4, ohlc.low[i]);
        sqlite3_bind_double(stmt, 5, ohlc.close[i]);
        sqlite3_bind_double(stmt, 6, ohlc.volume[i]);

        result = sqlite3_step(stmt);

        if (result != SQLITE_DONE)
        {
            throw std::runtime_error(std::string("Error : execut requete to database : ") + sqlite3_errmsg(db));
        }

        sqlite3_reset(stmt);
    }

    sqlite3_exec(db, "COMMIT", nullptr, nullptr, nullptr);
    sqlite3_finalize(stmt);
}

OHLCV load_from_db(sqlite3* db, const std::string& tableName, __int64 from, __int64 to)
{
    std::string temp = "SELECT * FROM " + tableName + " WHERE time >= ? AND time <= ? ORDER BY time ASC;";
    sqlite3_stmt *stmt;

    OHLCV output {};

    int result = sqlite3_prepare_v2(db, temp.c_str(), -1, &stmt, nullptr);

    __int64 min = from;
    __int64 max = to;

    if (result != SQLITE_OK)
    {
        throw std::runtime_error(std::string("outil_sqlt.cpp/load_from_db, error create request (l.117) : \n") + sqlite3_errmsg(db)  + std::string("\n requetet sql : ") + temp);
    }

    sqlite3_bind_int64(stmt, 1, min);
    sqlite3_bind_int64(stmt, 2, max);

    while (sqlite3_step(stmt) == SQLITE_ROW) {
        output.open.push_back(sqlite3_column_double(stmt, 1));
        output.high.push_back(sqlite3_column_double(stmt, 2));
        output.low.push_back(sqlite3_column_double(stmt, 3));
        output.close.push_back(sqlite3_column_double(stmt, 4));
        output.volume.push_back(sqlite3_column_double(stmt, 5));
        output.time.push_back(sqlite3_column_int64(stmt, 0));
    }

    sqlite3_finalize(stmt);
    return output;
}


bool file_exist(const std::string &filename)
{

    std::ifstream file(filename);
    if (!file.good())
    {
        return false;
    }
    file.close();
    return true;
}

void write_json(const std::string &exchange, const std::string &timeframe, __int64 start, __int64 end)
{
    if (file_exist("info_db.json"))
    {
        json json_data = json::parse(std::ifstream("info_db.json"));
        json_data[exchange][timeframe]["start"] = start;
        json_data[exchange][timeframe]["end"] = end;
        std::ofstream out("info_db.json");
        out << std::setw(4) << json_data;
        out.close();
    }
    else
    {
        json json_data;
        json_data[exchange][timeframe]["start"] = start;
        json_data[exchange][timeframe]["end"] = end;
        std::ofstream out("info_db.json");
        out << std::setw(4) << json_data;
        out.close();
    }
}

std::pair<__int64, __int64> cours_in_json(const std::string &exchange, const std::string &timeframe, json &file)
{

    if (file.contains(exchange) && file[exchange].contains(timeframe))
    {
        return std::make_pair(file[exchange][timeframe]["start"].get<__int64>(), file[exchange][timeframe]["end"].get<__int64>());
    }
    else
    {
        return std::make_pair(-1, -1);
    }
}

void delete_db()
{
    try {
        std::remove("data.db");
    } catch (const std::runtime_error& e)
    {
        std::cerr << "Erreur lors de la suppression du fichier : " << e.what() << std::endl;
    }
}

void re_init_json()
{
    std::ofstream out("info_db.json");
    out << "{}";
    out.close();
}

void resetTable(sqlite3* db, const std::string& tableName)
{
    std::string deleteAllRowsQuery = "DELETE FROM " + tableName + ";";
    int result = sqlite3_exec(db, deleteAllRowsQuery.c_str(), nullptr, nullptr, nullptr);

    if (result != SQLITE_OK)
    {
        throw std::runtime_error(std::string("outil_sqlt.cpp/resetTable, error delete table (l.206) : \n") + sqlite3_errmsg(db)  + std::string("\n requetet sql : ") + deleteAllRowsQuery);
    }
}

const std::map<std::string, __int64> dico_timeframe =
{
    {"1s",  1000},
    {"1m",  60000},
    {"2m",  120000},
    {"3m",  180000},
    {"4m",  240000},
    {"5m",  300000},
    {"15m", 900000},
    {"30m", 1800000},
    {"60m", 3600000},
    {"1h",  3600000},
    {"90m", 5400000},
    {"2h",  7200000},
    {"4h",  14400000},
    {"6h",  21600000},
    {"8h",  28800000},
    {"12h", 43200000},
    {"1d",  86400000},
    {"3d",  259200000},
    {"1w",  604800000},
    {"1M",  2592000000},
    {"5d", 259200000},
    {"1wk", 604800000},
    {"1mo", 2592000000},
    {"3mo", 7889238000},
};


OHLCV load_and_save(OHLCV (*load_data)(__int64, __int64, std::string, std::string),const std::string _from, const std::string _to, const std::string _exchange, const std::string _timeframe)
{
    

    const __int64 from = datetime_to_milliseconds(_from);
    const __int64 to = datetime_to_milliseconds(_to);

    if (to - from <= 0)
    {
        throw std::runtime_error("Error : from must be less than to");
    }

    OHLCV output{};

    std::string name = _exchange + "_" + _timeframe;

    if (file_exist("data.db"))
    {
        sqlite3* db = open_database("data.db");
        if (tableExists(db, name.c_str()))
        {
            if (file_exist("info_db.json"))
            {
                json json_data = json::parse(std::ifstream("info_db.json"));
                std::pair<__int64, __int64> dates = cours_in_json(_exchange, _timeframe, json_data);
                if (dates.first != -1 || dates.second != -1)
                {
                    if (from >= dates.first && from <= dates.second && to >= dates.first && to <= dates.second)
                    {
                        return load_from_db(db, name.c_str(), from, to);
                    }
                    else if ((from >= dates.second) || (from >= dates.first && from <= dates.second && to >= dates.second))
                    {
                        OHLCV temp = load_data((dates.second + dico_timeframe.at(_timeframe)), to, _exchange, _timeframe);

                        insertOHLCV(db, temp, name.c_str());

                        write_json(_exchange, _timeframe, dates.first, to);

                        output = load_from_db(db, name.c_str(), from, to);
                        sqlite3_close(db);
                        return output;
                    }
                    else if ((to <= dates.first) || (from < dates.first && to <= dates.second && to >= dates.first))
                    {

                        OHLCV data = load_data(from, (dates.first - dico_timeframe.at(_timeframe)), _exchange, _timeframe);

                        insertOHLCV(db, data, name.c_str());

                        write_json(_exchange, _timeframe, from, dates.second);

                        output = load_from_db(db, name.c_str(), from, to);
                        sqlite3_close(db);
                        return output;
                    }
                    else if (from < dates.first && to > dates.second)
                    {

                        OHLCV requete_1 = load_data(from, (dates.first - dico_timeframe.at(_timeframe)), _exchange, _timeframe);
                        OHLCV requete_2 = load_data((dates.second + dico_timeframe.at(_timeframe)), to, _exchange, _timeframe);

                        insertOHLCV(db, requete_1, name.c_str());
                        insertOHLCV(db, requete_2, name.c_str());

                        write_json(_exchange, _timeframe, from, to);

                        output = load_from_db(db, name.c_str(), from, to);
                        sqlite3_close(db);
                        return output;
                    }
                    else
                    {
                        throw std::runtime_error("Error, inderminate case create issues in github");
                    }
                }
                else
                {
                    resetTable(db, name.c_str());
                    output = load_data(from, to, _exchange, _timeframe);

                    create_table(db, name);
                    insertOHLCV(db, output, name);
                    sqlite3_close(db);

                    write_json(_exchange, _timeframe, from, to);

                    return output;
                }
            }
            else
            {
                sqlite3_close(db);

                delete_db();
                re_init_json();

                sqlite3* db = open_database("data.db");

                output = load_data(from, to, _exchange, _timeframe);

                create_table(db, name);
                insertOHLCV(db, output, name);
                sqlite3_close(db);

                write_json(_exchange, _timeframe, from, to);

                return output;
            }
        }
        else
        {
            output = load_data(from, to, _exchange, _timeframe);

            create_table(db, name);
            insertOHLCV(db, output, name);
            sqlite3_close(db);

            write_json(_exchange, _timeframe, from, to);

            return output;
        }
    }
    else
    {
        output = load_data(from, to, _exchange, _timeframe);

        sqlite3* db = open_database("data.db");

        re_init_json();
        create_table(db, name);
        insertOHLCV(db, output, name);
        sqlite3_close(db);

        write_json(_exchange, _timeframe, from, to);
        return output;
    }

    return output;
}