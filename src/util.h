#ifndef UTIL_H
#define UTIL_H

#include <sys/time.h>
#include <sstream>

class util {
public:

	static void stringToVec(const std::string& line, vec& vector)
{
    std::size_t begin = line.find("[")+1;
    std::size_t end = line.find("]");
    std::stringstream ss(line.substr(begin, end-begin));

    int i = 0;
    for( std::string val; std::getline(ss, val, ','); i++) {
        vector.push_back(std::stof(val));
    }
}

static int getJsonInt(const std::string& line, const std::string& key)
{
    std::size_t begin = line.find("\"" + key + "\":") + key.size() + 3;
    std::size_t end = line.find(",", begin);
    return std::stoi(line.substr(begin, end-begin));
}

static double getTimestampInMilliseconds()
{
    struct timeval now;
    gettimeofday (&now, NULL);
    return now.tv_usec/1000.0 + (unsigned long long)now.tv_sec * 1000;
}

};

#endif // UTIL_H
