#include "crypto.hpp"

OHLCV load_crypto(__int64 from, __int64 to, std::string exchange, std::string timeframe)
{
    OHLCV data;

    std::vector<std::string> liste_timeframe{ "1s", "1m", "3m", "5m", "15m", "30m", "1h", "2h", "4h", "6h", "8h", "12h", "1d", "3d", "1w", "1M" };

    auto timeframe_valid = [timeframe](std::string& elem) { return elem == timeframe; };
    if (!std::any_of(liste_timeframe.begin(), liste_timeframe.end(), timeframe_valid))
    {
        throw std::invalid_argument("timeframe not in liste");
    }



    // 1h -> miliseconde
    __int64 input_timeframe_ml = dico_timeframe.at(timeframe);

    size_t number_value = (to - from) / (input_timeframe_ml);


    if (number_value < 500)
    {
        // "https://api.binance.com/api/v3/klines?symbol=""&interval=1h

        std::ostringstream url;
        url << "https://api.binance.com/api/v3/klines?symbol=" << exchange << "&interval=" << timeframe << "&startTime=" << from << "&endTime=" << to;
        std::string buffer = requete_api(url.str());
        json jsonData = json::parse(buffer);

        if (jsonData.is_null())
        {
            return data;
        }

        if (jsonData.contains("code"))
        {
            std::cout << jsonData["msg"] << std::endl;

            return data;
        }


        for (auto& tableau : jsonData)
        {
            data.time.push_back(tableau[0]);
            data.open.push_back(std::stod(tableau[1].get<std::string>()));
            data.high.push_back(std::stod(tableau[2].get<std::string>()));
            data.low.push_back(std::stod(tableau[3].get<std::string>()));
            data.close.push_back(std::stod(tableau[4].get<std::string>()));
            data.volume.push_back(std::stod(tableau[5].get<std::string>()));
        }
    }
    else
    {
        std::cout << number_value << std::endl;
        if (number_value < 1000)
        {
            std::cout << "unnnn" << '\n';
            auto t1 = std::async(load_crypto, from, from + 499 * input_timeframe_ml, exchange, timeframe);
            auto t2 = std::async(load_crypto, from + 500 * input_timeframe_ml, to, exchange, timeframe);

            OHLCV output = t1.get();
            OHLCV data = t2.get();
            output.insert(data);

            return output;
        }
        else if (number_value < 1500)
        {
            std::cout << "deuuuuxx" << '\n';

            auto t1 = std::async(load_crypto, from, from + 499 * input_timeframe_ml, exchange, timeframe);
            auto t2 = std::async(load_crypto, from + 500 * input_timeframe_ml, from + 999 * input_timeframe_ml, exchange, timeframe);
            auto t3 = std::async(load_crypto, from + 1000 * input_timeframe_ml, to, exchange, timeframe);

            OHLCV output = t1.get();
            OHLCV data_1 = t2.get();
            OHLCV data_2 = t3.get();
            output.insert(data_1);
            output.insert(data_2);

            return output;
        }
        else if (number_value < 2000)
        {
            std::cout << "troiiissss" << '\n';

            auto t1 = std::async(load_crypto, from, from + 499 * input_timeframe_ml, exchange, timeframe);
            auto t2 = std::async(load_crypto, from + 500 * input_timeframe_ml, from + 999 * input_timeframe_ml, exchange, timeframe);
            auto t3 = std::async(load_crypto, from + 1000 * input_timeframe_ml, from + 1499 * input_timeframe_ml, exchange, timeframe);
            auto t4 = std::async(load_crypto, from + 1500 * input_timeframe_ml, to, exchange, timeframe);

            OHLCV output = t1.get();
            OHLCV data_1 = t2.get();
            OHLCV data_2 = t3.get();
            OHLCV data_3 = t4.get();

            output.insert(data_1);
            output.insert(data_2);
            output.insert(data_3);

            return output;
        }
        else
        {
            std::cout << "quattttreeee" << '\n';


            OHLCV output;
            OHLCV temp_data;

            temp_data = load_crypto(from, from + 1999 * input_timeframe_ml, exchange, timeframe);
            std::cout << "**********" << temp_data.close.size() << "**********\n";
            output.insert(temp_data);

            while ((to - output.time[output.time.size() - 1]) / input_timeframe_ml > 2000)
            {
                temp_data = load_crypto(output.time[output.time.size() - 1] + input_timeframe_ml, output.time[output.time.size() - 1] + input_timeframe_ml + 1999 * input_timeframe_ml, exchange, timeframe);
                std::cout << "**********" << temp_data.close.size() << "**********\n";
                output.insert(temp_data);
            }
            temp_data = load_crypto(output.time[output.time.size() - 1] + input_timeframe_ml, to, exchange, timeframe);
            std::cout << "**********" << temp_data.close.size() << "**********\n";
            output.insert(temp_data);
            std::cout << output.close.size();

            return output;
        }
    }



    return data;
}

OHLCV crypto(const std::string _from, const std::string _to, const std::string _exchange, const std::string _timeframe)
{
    std::vector<std::string> liste_timeframe{ "1s", "1m", "3m", "5m", "15m", "30m", "1h", "2h", "4h", "6h", "8h", "12h", "1d", "3d", "1w", "1M" };

    auto exchange_valide = [_timeframe](std::string& elem) { return elem == _timeframe; };
    if (!std::any_of(liste_timeframe.begin(), liste_timeframe.end(), exchange_valide))
    {
        std::cout << "timeframe not in liste" << std::endl;
        throw std::runtime_error("Error : timeframe not in liste");
    }

    return load_and_save(load_crypto, _from, _to, _exchange, _timeframe);
}