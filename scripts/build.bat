@echo off
setlocal enabledelayedexpansion

echo.
echo [1/4] Verificando dependencias...

where cmake >nul 2>nul
if errorlevel 1 (
    echo [ERRO] CMake nao encontrado. Instale em https://cmake.org/download/
    exit /b 1
)

REM Verifica Visual Studio (cl.exe no PATH ou ambiente VS)
where cl >nul 2>nul
if errorlevel 1 (
    echo [INFO] cl.exe nao esta no PATH. Tentando localizar Visual Studio...
    if exist "C:\Program Files\Microsoft Visual Studio\2022\Community\VC\Auxiliary\Build\vcvars64.bat" (
        call "C:\Program Files\Microsoft Visual Studio\2022\Community\VC\Auxiliary\Build\vcvars64.bat"
    ) else if exist "C:\Program Files (x86)\Microsoft Visual Studio\2019\Community\VC\Auxiliary\Build\vcvars64.bat" (
        call "C:\Program Files (x86)\Microsoft Visual Studio\2019\Community\VC\Auxiliary\Build\vcvars64.bat"
    ) else (
        echo [ERRO] Visual Studio nao encontrado.
        echo        Abra o "Developer Command Prompt" e rode este script de novo.
        exit /b 1
    )
)

echo.
echo [2/4] Clonando dependencias header-only...

if not exist include\crow (
    git clone --depth 1 https://github.com/CrowCpp/Crow.git include\crow
    if errorlevel 1 exit /b 1
) else (
    echo    Crow ja presente.
)

if not exist third_party\json (
    git clone --depth 1 https://github.com/nlohmann/json.git third_party\json
    if errorlevel 1 exit /b 1
) else (
    echo    nlohmann/json ja presente.
)

echo.
echo [3/4] Procurando OpenCV / OpenSSL...

if "%OPENCV_DIR%"=="" (
    REM Tenta detectar o padrao do instalador oficial do OpenCV
    for /d %%D in ("C:\opencv\build") do set OPENCV_DIR=%%D
    for /d %%D in ("C:\Program Files\opencv\build") do set OPENCV_DIR=%%D
)

if "%OPENCV_DIR%"=="" (
    echo [ERRO] OpenCV nao encontrado.
    echo        Defina a variavel:  set OPENCV_DIR=C:\caminho\para\opencv\build
    echo        Ou instale via:     vcpkg install opencv4:x64-windows
    exit /b 1
)
echo    OpenCV: %OPENCV_DIR%

if "%OPENSSL_ROOT_DIR%"=="" (
    REM Tenta localizacao padrao
    if exist "C:\Program Files\OpenSSL-Win64" (
        set OPENSSL_ROOT_DIR=C:\Program Files\OpenSSL-Win64
    ) else if exist "C:\Program Files\OpenSSL" (
        set OPENSSL_ROOT_DIR=C:\Program Files\OpenSSL
    )
)

if "%OPENSSL_ROOT_DIR%"=="" (
    echo [AVISO] OpenSSL nao localizado automaticamente.
    echo         Defina:  set OPENSSL_ROOT_DIR=C:\caminho\para\openssl
) else (
    echo    OpenSSL: %OPENSSL_ROOT_DIR%
)

echo.
echo [4/4] Compilando...

if not exist build mkdir build
cd build || exit /b 1

cmake .. ^
    -G "Visual Studio 17 2022" -A x64 ^
    -DOPENCV_DIR=%OPENCV_DIR% ^
    -DOPENSSL_ROOT_DIR="%OPENSSL_ROOT_DIR%"
if errorlevel 1 (
    echo [ERRO] Falha no cmake configure.
    exit /b 1
)

cmake --build . --config Release
if errorlevel 1 (
    echo [ERRO] Falha na compilacao.
    exit /b 1
)

cd ..

echo.
echo ============================================
echo   Build concluido!
echo   Binario: build\Release\captcha_server.exe
echo.
echo   Para rodar (da raiz do projeto):
echo      cd build\Release ^&^& captcha_server.exe
echo.
echo   Nao esqueca de gerar os audios antes:
echo      python scripts\gen_sounds.py
echo ============================================
