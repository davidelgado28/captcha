#pragma once
#include <string>
#include <vector>

class AudioGenerator {
public:
    static std::vector<uchar> spellOut(const std::string& code);
private:
    static std::vector<uchar> loadWav(const std::string& path);
};
