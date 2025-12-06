#!/bin/zsh
if [ $1 = "main" ]; then
  echo "executing main..."
  echo "> clang++ -std=c++17 -Iinclude src/*.cpp -o bin/main"
  clang++ -std=c++17 -Iinclude src/*.cpp -o bin/main
else
  echo "testing $1..."
  echo "> clang++ -std=c++17 -Iinclude {tests/test_,src/}$1.cpp -o bin/$1"
  clang++ -std=c++17 -Iinclude {tests/test_,src/}$1.cpp -o bin/$1
fi

exit_code=$?

if [ $exit_code -eq 0 ]; then
  echo "> bin/$1"
  echo ""
  bin/$1
else
  echo "failed in compiling (status: $exit_code)"
  exit $exit_code
fi  
