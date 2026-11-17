#include "captcha_generator.hpp"
#include <openssl/rand.h>
#include <opencv2/imgproc.hpp>
#include <opencv2/imgcodecs.hpp>
#include <random>

std::string CaptchaGenerator::toHex(const unsigned char* data, size_t len) {
    static const char* hex = "0123456789abcdef";
    std::string out;
    out.reserve(len * 2);
    for (size_t i = 0; i < len; i++) {
        out += hex[data[i] >> 4];
        out += hex[data[i] & 0x0F];
    }
    return out;
}
std::string CaptchaGenerator::randomCode(int length) {
    static const std::string chars =
        "ABCDEFGHJKLMNPQRSTUVWXYZabcdefghjkmnpqrstuvwxyz23456789";
    static thread_local std::mt19937 rng(std::random_device{}());
    std::uniform_int_distribution<size_t> dist(0, chars.size() - 1);

    std::string code;
    for (int i = 0; i < length; i++)
        code += chars[dist(rng)];
    return code;
}
cv::Mat CaptchaGenerator::renderImage(const std::string& code, int w, int h) {
    cv::Mat img(h, w, CV_8UC3, cv::Scalar(245, 245, 245));
    std::mt19937 rng(std::random_device{}());
    std::uniform_int_distribution<int> dx(0, w), dy(0, h), ch(0, 255);

    for (int i = 0; i < 6; i++) {
        cv::line(img, {dx(rng), dy(rng)}, {dx(rng), dy(rng)},
                 {ch(rng), ch(rng), ch(rng)}, 1, cv::LINE_AA);
    }
    for (int i = 0; i < 300; i++) {
        img.at<cv::Vec3b>(dy(rng), dx(rng)) =
            {uchar(ch(rng)), uchar(ch(rng)), uchar(ch(rng))};
    }
    int x = 25;
    std::vector<cv::Point2f> centers;
    for (char c : code) {
        std::string s(1, c);
        int font = cv::FONT_HERSHEY_DUPLEX;
        double scale = 1.2 + (rng() % 80) / 100.0;
        int thick = 2 + rng() % 2;
        cv::putText(img, s, {x, h / 2 + 15}, font, scale,
                    {uchar(ch(rng)), uchar(ch(rng)), uchar(ch(rng))},
                    thick, cv::LINE_AA);
        centers.push_back({float(x + 15), float(h / 2)});
        x += w / (int)code.size();
    }
    cv::Mat src = img.clone(), mapX = img.clone(), mapY = img.clone();
    for (int y = 0; y < h; y++) {
        for (int xx = 0; xx < w; xx++) {
            mapX.at<float>(y, xx) = xx + sin(y / 8.0) * 4.0;
            mapY.at<float>(y, xx) = y + sin(xx / 10.0) * 3.0;
        }
    }
    cv::remap(src, img, mapX, mapY, cv::INTER_LINEAR, cv::BORDER_REPLICATE);

    return img;
}
CaptchaGenerator::Captcha CaptchaGenerator::generate(int length) {
    Captcha c;
    c.answer = randomCode(length);

    unsigned char buf[16];
    RAND_bytes(buf, sizeof(buf));
    c.id = toHex(buf, sizeof(buf));

    c.width = 260;
    c.height = 90;
    cv::Mat img = renderImage(c.answer, c.width, c.height);
    cv::imencode(".png", img, c.png);

    return c;
}
