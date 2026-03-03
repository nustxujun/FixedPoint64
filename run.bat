@echo off
setlocal

set BUILD_DIR=build

cmake -B %BUILD_DIR%
if %errorlevel% neq 0 exit /b %errorlevel%

cmake --build %BUILD_DIR% --config Release
if %errorlevel% neq 0 exit /b %errorlevel%

echo.
echo ==== Running Tests ====
cd %BUILD_DIR%
ctest --build-config Release --verbose
if %errorlevel% neq 0 (
    cd ..
    exit /b %errorlevel%
)
cd ..

echo.
echo ==== Running Benchmark ====
%BUILD_DIR%\Release\fixed64_benchmark.exe
