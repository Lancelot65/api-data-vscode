#ifndef FOREX_HPP
#define FOREX_HPP

#include <string>
#include <vector>

#include <nlohmann/json.hpp>
using json = nlohmann::json;

#include "outil_requete.hpp"
#include "outil_sql.hpp"

OHLCV load_forex(__int64 from, __int64 to, std::string exchange, std::string timeframe);
OHLCV forex(const std::string _from, const std::string _to, const std::string _exchange, const std::string _timeframe);

#endif // ! FOREX_HPP