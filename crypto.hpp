#ifndef CRYPTO_HPP
#define CRYPTO_HPP

#include <iostream>
#include <future>
#include <nlohmann/json.hpp>
#include <string>
#include <iostream>
#include <thread>
#include <mutex>
#include <algorithm>
#include <numeric>
#include <vector>
#include <cstdio>
#include <thread>
#include <chrono>

#include "outil_requete.hpp"
#include "outil_sql.hpp"

using json = nlohmann::json;

const std::map<std::string, __int64> dico_timeframe =
{
    {"1s", 1000},
    {"1m", 60000},
    {"3m", 180000},
    {"4m", 240000},
    {"5m", 300000},
    {"15m", 900000},
    {"30m", 1800000},
    {"1h", 3600000},
    {"2h", 7200000},
    {"4h", 14400000},
    {"6h", 21600000},
    {"8h", 28800000},
    {"12h", 43200000},
    {"1d", 86400000},
    {"3d", 259200000},
    {"1w", 604800000},
    {"1M", 2592000000}
};

OHLCV crypto(const std::string _from, const std::string _to, const std::string _exchange, const std::string _timeframe);

OHLCV load_crypto(__int64 from, __int64 to, std::string exchange, std::string timeframe);

#endif