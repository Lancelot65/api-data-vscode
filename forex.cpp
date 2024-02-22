#include "forex.hpp"


const std::string base_url = "https://query2.finance.yahoo.com/v8/finance/chart/";
const std::vector<std::string> liste_timeframe = { "1m", "2m", "5m", "15m", "30m", "60m", "90m", "1h", "1d", "5d", "1wk", "1mo", "3mo" };

const std::map<std::string, __int64> dico_timeframe =
{
    {"1m", 60000},
    {"2m", 120000},
    {"5m", 300000},
    {"15m", 900000},
    {"30m", 1800000},
    {"60m", 3600000},
    {"90m", 5400000},
    {"1h", 3600000},
    {"1d", 86400000},
    {"5d", 259200000},
    {"1wk", 604800000},
    {"1mo", 2592000000},
    {"3mo", 7889238000},
};

static std::string create_url(__int64 from, __int64 to, std::string exchange, std::string timeframe)
{
    std::stringstream ss;
    ss << base_url << exchange << "=X?period1=" << from << "&period2=" << to << "&interval=" << timeframe << "&events=history";
    return ss.str();
}

OHLCV load_forex(__int64 from, __int64 to, std::string exchange, std::string timeframe)
{
    std::string url = create_url(from / 1000, to / 1000, exchange, timeframe);

    std::string buffer = requete_api(url);

    json json_yahoo = json::parse(buffer);

    if (!json_yahoo["chart"]["error"].is_null())
    {
        std::string error = json_yahoo["chart"]["error"]["code"].get<std::string>();
        if (error == "Bad Request" || error == "Not Found")
        {
            throw std::invalid_argument(json_yahoo["chart"]["error"]["description"]);
        }
        else
        {
            throw std::runtime_error(json_yahoo["chart"]["error"]["description"]);
        }
    }

    json json_ohlcv_yahoo = json_yahoo["chart"]["result"][0]["indicators"]["quote"][0];
    json json_time = json_yahoo["chart"]["result"][0]["timestamp"];

    OHLCV data;

    data.time.reserve(json_time.size());
    std::transform(json_time.begin(), json_time.end(), std::back_inserter(data.time),
        [](__int64 value) { return value * 1000; });                                     // convert into milliseonde for have same global time

    size_t null_count = std::count_if(json_ohlcv_yahoo["open"].begin(), json_ohlcv_yahoo["open"].end(), [](const auto& value) {
        return value.is_null();
        });

    if (null_count)
    {
        auto it = std::find_if(json_ohlcv_yahoo["open"].begin(), json_ohlcv_yahoo["open"].end(), [](auto i) { return i.is_null(); });

        while (it != json_ohlcv_yahoo["open"].end())
        {
            *it = -1;
            it = std::find_if(std::next(it), json_ohlcv_yahoo["open"].end(), [](auto i) { return i.is_null(); });
        }

        it = std::find_if(json_ohlcv_yahoo["high"].begin(), json_ohlcv_yahoo["high"].end(), [](auto i) { return i.is_null(); });

        while (it != json_ohlcv_yahoo["high"].end())
        {
            *it = -1;
            it = std::find_if(std::next(it), json_ohlcv_yahoo["high"].end(), [](auto i) { return i.is_null(); });
        }

        it = std::find_if(json_ohlcv_yahoo["low"].begin(), json_ohlcv_yahoo["low"].end(), [](auto i) { return i.is_null(); });

        while (it != json_ohlcv_yahoo["low"].end())
        {
            *it = -1;
            it = std::find_if(std::next(it), json_ohlcv_yahoo["low"].end(), [](auto i) { return i.is_null(); });
        }

        it = std::find_if(json_ohlcv_yahoo["close"].begin(), json_ohlcv_yahoo["close"].end(), [](auto i) { return i.is_null(); });

        while (it != json_ohlcv_yahoo["close"].end())
        {
            *it = -1;
            it = std::find_if(std::next(it), json_ohlcv_yahoo["close"].end(), [](auto i) { return i.is_null(); });
        }

        it = std::find_if(json_ohlcv_yahoo["volume"].begin(), json_ohlcv_yahoo["volume"].end(), [](auto i) { return i.is_null(); });

        while (it != json_ohlcv_yahoo["volume"].end()) {
            *it = -1;
            it = std::find_if(std::next(it), json_ohlcv_yahoo["volume"].end(), [](auto i) { return i.is_null(); });
        }
    }


    data.open.assign(json_ohlcv_yahoo["open"].begin(), json_ohlcv_yahoo["open"].end());
    data.high.assign(json_ohlcv_yahoo["high"].begin(), json_ohlcv_yahoo["high"].end());
    data.low.assign(json_ohlcv_yahoo["low"].begin(), json_ohlcv_yahoo["low"].end());
    data.close.assign(json_ohlcv_yahoo["close"].begin(), json_ohlcv_yahoo["close"].end());
    data.volume.assign(json_ohlcv_yahoo["volume"].begin(), json_ohlcv_yahoo["volume"].end());

    return data;
}

OHLCV forex(const std::string _from, const std::string _to, const std::string _exchange, const std::string _timeframe)
{
    auto exchange_valide = [&_timeframe](const std::string& elem) { return elem == _timeframe; };
    if (!std::any_of(liste_timeframe.begin(), liste_timeframe.end(), exchange_valide))
    {
        std::cout << "timeframe not in liste" << std::endl;
        throw std::runtime_error("Error : timeframe not in liste");
    }

    return load_and_save(load_forex, _from, _to, _exchange, _timeframe);
}