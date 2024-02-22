#include "outil_requete.hpp"

static size_t writeCallback(void* contents, size_t size, size_t nmemb, std::string* buffer)
{
    size_t realsize = size * nmemb;
    buffer->append((char*)contents, realsize);
    return realsize;
}

std::string requete_api(std::string const url)
{
    CURL* curl;
    CURLcode res;
    std::string buffer;
    std::cout << url << "\n";


    curl_global_init(CURL_GLOBAL_ALL);
    curl = curl_easy_init();

    if (curl) {
        curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, writeCallback);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &buffer);

        res = curl_easy_perform(curl);

        if (res != CURLE_OK) {
            std::cerr << "Erreur CURL : " << curl_easy_strerror(res) << std::endl;
        }

        curl_easy_cleanup(curl);
    } else {
        std::cerr << "Erreur CURL : Impossible d'initialiser CURL." << std::endl;
    }

    curl_global_cleanup();

    return buffer;
}


std::string milliseconds_to_datetime(__int64& unixMilliseconds)
{
    auto timePoint = std::chrono::time_point<std::chrono::system_clock>(std::chrono::milliseconds(unixMilliseconds));
    std::time_t time = std::chrono::system_clock::to_time_t(timePoint);

    std::tm tmInfo;

    if (!localtime_s(&tmInfo, &time)) {
        char buffer[20]; 
        std::strftime(buffer, sizeof(buffer), "%Y-%m-%d %H:%M", &tmInfo);

        return std::string(buffer);
    } else {
        std::cerr << "Erreur lors de la conversion de temps." << std::endl;
        return "";
    }
}

__int64 datetime_to_milliseconds(const std::string& datetime_str)
{
    std::tm tmStruct = {};
    std::istringstream ss(datetime_str);
    ss >> std::get_time(&tmStruct, "%Y-%m-%d");

    if (!ss.fail()) {
        std::chrono::system_clock::time_point customTimePoint = std::chrono::system_clock::from_time_t(std::mktime(&tmStruct));

        auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(customTimePoint.time_since_epoch());

        return duration.count();
    } else {
        std::cerr << "Erreur lors de la conversion de temps." << std::endl;
        return -1;
    }
}