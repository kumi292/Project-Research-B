#!/bin/zsh
if [ $1 = "additive_communication" ]; then
  echo "executing $2 program..."
  clang++ -std=c++17 -Iinclude src/beaver_triple.cpp src/additive_communication/$2.cpp src/common_functions.cpp -o bin/$2 -lzmq

  exit_code=$?
  if [ $exit_code -eq 0 ]; then
    if [ $2 = "database" ]; then
      bin/$2 $3
    else 
      bin/$2
    fi
  else
    echo "failed in compiling (status: $exit_code)"
    exit $exit_code
  fi  
fi
else
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
fi

