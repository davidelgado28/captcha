cmake_minimum_required(VERSION 3.14)
project(captcha_cpp CXX)

set(CMAKE_CXX_STANDARD 17)

find_package(OpenCV REQUIRED)

add_executable(captcha_server
    src/main.cpp
    src/captcha_generator.cpp
    src/audio_generator.cpp
)

target_include_directories(captcha_server PRIVATE
    include/crow/include
    third_party/json/include
    ${OpenCV_INCLUDE_DIRS}
)

target_link_libraries(captcha_server PRIVATE
    ${OpenCV_LIBS}
    ssl crypto
    pthread
)
