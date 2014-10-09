#ifndef UTIL_H
#define UTIL_H

#include <chrono>
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

static int64_t getTimestampInMilliseconds()
{
	return std::chrono::duration_cast< std::chrono::milliseconds >(
		   std::chrono::high_resolution_clock::now().time_since_epoch()
		).count();
}

};

#endif // UTIL_H
