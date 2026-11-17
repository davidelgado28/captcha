#!/bin/bash
set -e
mkdir -p include third_party
[ -d include/crow ] || git clone --depth 1 https://github.com/CrowCpp/Crow.git include/crow
[ -d third_party/json ] || git clone --depth 1 https://github.com/nlohmann/json.git third_party/json

mkdir -p build && cd build
cmake .. && make -j$(nproc)
echo "Binário: build/captcha_server — rode a partir da raiz do projeto:"
echo "   ./build/captcha_server"
