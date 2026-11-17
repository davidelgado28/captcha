#pragma once
#include <string>
#include <unordered_map>
#include <chrono>
#include <mutex>

class CaptchaStore {
public:
    struct Entry {
        std::string answer;
        std::chrono::steady_clock::time_point expires;
        bool verified = false;
    };

    void put(const std::string& id, const std::string& answer) {
        std::lock_guard<std::mutex> lock(m_);
        store_[id] = {answer,
            std::chrono::steady_clock::now() + std::chrono::minutes(5)};
    }
    int verify(const std::string& id, const std::string& answer) {
        std::lock_guard<std::mutex> lock(m_);
        auto it = store_.find(id);
        if (it == store_.end()) return -1;
        if (std::chrono::steady_clock::now() > it->second.expires) {
            store_.erase(it);
            return -1;
        }
        bool ok = (it->second.answer == answer);
        it->second.verified = ok;
        if (ok) it->second.expires =
            std::chrono::steady_clock::now() + std::chrono::minutes(5);
        return ok ? 1 : 0;
    }
    bool isVerified(const std::string& id) {
        std::lock_guard<std::mutex> lock(m_);
        auto it = store_.find(id);
        return it != store_.end() && it->second.verified &&
               std::chrono::steady_clock::now() <= it->second.expires;
    }

private:
    std::mutex m_;
    std::unordered_map<std::string, Entry> store_;
};
