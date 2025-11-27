#!/bin/zsh
if [ $1 = "main" ]; then
  echo "mainを実行します"
  echo "> clang++ -std=c++17 -Iinclude src/*.cpp -o bin/main"
  clang++ -std=c++17 -Iinclude src/*.cpp -o bin/main
  echo "> bin/main"
  echo ""
  bin/main
else
  echo "$1のテストをします"
  echo "> clang++ -std=c++17 -Iinclude {tests/test_,src/}$1.cpp -o bin/$1"
  clang++ -std=c++17 -Iinclude {tests/test_,src/}$1.cpp -o bin/$1
  echo "> bin/$1"
  echo ""
  bin/$1
fi
