#include <iostream>
#include "../crypto.hpp"
#include "../outil_requete.hpp"
#include <iomanip>

int main()
{
    OHLCV data = crypto("2022-01-01", "2022-01-02", "BTCUSDT", "1h");

    return 0;
}