#include "audio_generator.hpp"
#include <fstream>
#include <stdexcept>

std::vector<uchar> AudioGenerator::loadWav(const std::string& path) {
    std::ifstream f(path, std::ios::binary);
    if (!f) throw std::runtime_error("WAV não encontrado: " + path);
    return {std::istreambuf_iterator<char>(f), {}};
}
std::vector<uchar> AudioGenerator::spellOut(const std::string& code) {
    const int sampleRate = 16000;
    const int silenceMs = 300;
    std::vector<short> pcm;

    for (char c : code) {
        std::string lower(1, char(tolower(c)));
        std::ifstream f("assets/sounds/" + lower + ".pcm", std::ios::binary);
        if (!f) continue;
        short s;
        while (f.read(reinterpret_cast<char*>(&s), 2))
            pcm.push_back(s);
        pcm.insert(pcm.end(), sampleRate * silenceMs / 1000, 0);
    }
    uint32_t dataSize = pcm.size() * 2;
    std::vector<uchar> wav;
    auto push32 = [&](uint32_t v) {
        wav.push_back(v); wav.push_back(v >> 8);
        wav.push_back(v >> 16); wav.push_back(v >> 24);
    };
    auto push16 = [&](uint16_t v) { wav.push_back(v); wav.push_back(v >> 8); };

    wav.insert(wav.end(), {'R','I','F','F'});
    push32(36 + dataSize);
    wav.insert(wav.end(), {'W','A','V','E','f','m','t',' '});
    push32(16);          
    push16(1);           
    push16(1);
    push32(sampleRate);
    push32(sampleRate * 2); 
    push16(2);           
    push16(16);          
    wav.insert(wav.end(), {'d','a','t','a'});
    push32(dataSize);
    uchar* bytes = reinterpret_cast<uchar*>(pcm.data());
    wav.insert(wav.end(), bytes, bytes + dataSize);

    return wav;
}
