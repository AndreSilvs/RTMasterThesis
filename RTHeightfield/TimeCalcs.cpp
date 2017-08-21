#include "TimeCalcs.h"

#include <ctime>   // localtime
#include <sstream> // stringstream
#include <iomanip> // put_time

std::string getCurrentTimeString() {
    auto now = std::chrono::system_clock::now();
    auto in_time_t = std::chrono::system_clock::to_time_t( now );

    std::stringstream ss;
    ss << std::put_time( std::localtime( &in_time_t ), "%Y-%m-%d %X" );
    return ss.str();
}