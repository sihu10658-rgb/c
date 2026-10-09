# c
c++로계산기만듬
bash코드는
#!/usr/bin/env bash
set -e

OS="$(uname -s)"
echo "Detected OS: $OS"

if [ "$OS" = "Darwin" ]; then
    echo "Building for macOS (Apple Silicon / Intel)..."
    clang++ -O3 -ffp-contract=fast -std=c++20 ln.cpp -o ln_exec
elif [ "$(expr substr "$OS" 1 5)" = "MINGW" ] || [ "$(expr substr "$OS" 1 6)" = "CYGWIN" ] || [ "$OS" = "MSYS_NT" ]; then
    echo "Building for Windows..."
    cl /O2 /fp:fast /std:c++20 ln.cpp /Fe:ln_exec.exe
else
    echo "Building for Linux..."
    g++ -O3 -ffp-contract=fast -std=c++20 ln.cpp -o ln_exec
fi

echo "Build complete. Running..."
if [ -f "./ln_exec" ]; then
    ./ln_exec
elif [ -f "./ln_exec.exe" ]; then
    ./ln_exec.exe
fi
그리고 아직 파일 만들는중임 이런식으로
