#!/bin/zsh
if [ $1 = "main" ]; then
  echo "mainを実行します"
  clang++ -std=c++17 -Iinclude src/*.cpp -o bin/main
  bin/main
else
  echo "$1のテストをします"
  clang++ -std=c++17 -Iinclude {tests/test_,src/}$1.cpp -o bin/$1
  bin/$1
fi
