#pragma once
#include <string>
#include <opencv2/opencv.hpp>

class CaptchaGenerator {
public:
    struct Captcha {
        std::string id;        
        std::string answer;    
        std::vector<uchar> png;    
        int width, height;
    };

    static Captcha generate(int length = 5);

private:
    static std::string randomCode(int length);
    static cv::Mat renderImage(const std::string& code, int w, int h);
    static std::string toHex(const unsigned char* data, size_t len);
};
