#include "crow.h"
#include "captcha_generator.hpp"
#include "audio_generator.hpp"
#include "captcha_store.hpp"
#include <nlohmann/json.hpp>
#include <fstream>

using json = nlohmann::json;

static CaptchaStore store;

int main() {
    crow::SimpleApp app;

    CROW_ROUTE(app, "/")([]{
        std::ifstream f("web/index.html");
        std::string html((std::istreambuf_iterator<char>(f)), {});
        crow::response res(html);
        res.set_header("Content-Type", "text/html; charset=utf-8");
        return res;
    });

    CROW_ROUTE(app, "/captcha.js")([]{
        std::ifstream f("web/captcha.js");
        std::string js((std::istreambuf_iterator<char>(f)), {});
        crow::response res(js);
        res.set_header("Content-Type", "application/javascript");
        return res;
    });
    CROW_ROUTE(app, "/captcha.css")([]{
        std::ifstream f("web/captcha.css");
        std::string css((std::istreambuf_iterator<char>(f)), {});
        crow::response res(css);
        res.set_header("Content-Type", "text/css");
        return res;
    });

    CROW_ROUTE(app, "/api/captcha").methods("GET"_method)([]{
        auto c = CaptchaGenerator::generate();
        store.put(c.id, c.answer);

        json res = {
            {"captcha_id", c.id},
            {"image_url", "/api/captcha/" + c.id + "/image.png"},
            {"audio_url", "/api/captcha/" + c.id + "/audio.wav"},
            {"expires_in", 300}
        };
        return crow::response(200, res.dump());
    });

    CROW_ROUTE(app, "/api/captcha").methods("GET"_method)([]{
        auto c = CaptchaGenerator::generate();
        auto wav = AudioGenerator::spellOut(c.answer);
        store.putFull(c.id, c.answer, c.png, wav);

        json res = {
            {"captcha_id", c.id},
            {"image_url", "/api/captcha/" + c.id + "/image.png"},
            {"audio_url", "/api/captcha/" + c.id + "/audio.wav"},
            {"expires_in", 300}
        };
        return crow::response(200, res.dump());
    });

    CROW_ROUTE(app, "/api/captcha/<string>/image.png")
    ([](const std::string& id){
        auto png = store.getImage(id);
        if (png.empty()) return crow::response(404, "not found");
        crow::response res(std::string(png.begin(), png.end()));
        res.set_header("Content-Type", "image/png");
        res.set_header("Cache-Control", "no-store");
        return res;
    });

    CROW_ROUTE(app, "/api/captcha/<string>/audio.wav")
    ([](const std::string& id){
        auto wav = store.getAudio(id);
        if (wav.empty()) return crow::response(404, "not found");
        crow::response res(std::string(wav.begin(), wav.end()));
        res.set_header("Content-Type", "audio/wav");
        res.set_header("Cache-Control", "no-store");
        return res;
    });

    CROW_ROUTE(app, "/api/captcha/<string>/verify").methods("POST"_method)
    ([](const crow::request& req, const std::string& id){
        auto body = json::parse(req.body, nullptr, false);
        if (body.is_discarded() || !body.contains("answer"))
            return crow::response(400, R"({"error":"campo 'answer' obrigatório"})");

        int result = store.verify(id, body["answer"].get<std::string>());

        json res;
        if (result == 1)  res = {{"verified", true},  {"token", id}};
        if (result == 0)  res = {{"verified", false}, {"reason", "wrong"}};
        if (result == -1) res = {{"verified", false}, {"reason", "expired"}};
        return crow::response(200, res.dump());
    });
    CROW_ROUTE(app, "/api/captcha/<string>/confirm").methods("GET"_method)
    ([](const std::string& id){
        json res = {{"valid", store.isVerified(id)}};
        return crow::response(200, res.dump());
    });

    app.port(8080).multithreaded().run();
}
