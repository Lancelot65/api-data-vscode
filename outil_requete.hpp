#ifndef OUTIL_REQUETE_HPP
#define OUTIL_REQUETE_HPP

#include <string>
#include <curl/curl.h>
#include <chrono>
#include <iostream>
#include <sstream>
#include <iomanip>
#include <vector>

std::string requete_api(std::string const url);

std::string milliseconds_to_datetime(__int64 &unixMilliseconds);
__int64 datetime_to_milliseconds(const std::string& datetime_str);

struct OHLCV
{
    std::vector<double> open;
    std::vector<double> close;
    std::vector<double> high;
    std::vector<double> low;
    std::vector<double> volume;
    std::vector<__int64> time;

    void insert(OHLCV &other)
    {
        this->open.insert(this->open.end(), other.open.begin(), other.open.end());
        this->close.insert(this->close.end(), other.close.begin(), other.close.end());
        this->high.insert(this->high.end(), other.high.begin(), other.high.end());
        this->low.insert(this->low.end(), other.low.begin(), other.low.end());
        this->volume.insert(this->volume.end(), other.volume.begin(), other.volume.end());
        this->time.insert(this->time.end(), other.time.begin(), other.time.end());
    }

    inline size_t size()
    {
        return this->close.size();
    }
};

#endif