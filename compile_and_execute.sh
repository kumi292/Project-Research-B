#!/bin/zsh
if [ $1 = "additive_communication" -o $1 = "replicated_communication" ]; then
  echo "executing $2 program..."
  if [ $1 = "additive_communication" ]; then
    echo "clang++ -std=c++17 -Iinclude src/beaver_triple.cpp src/$1/$2.cpp src/common_functions.cpp -o bin/$2 -lzmq"
    clang++ -std=c++17 -Iinclude src/beaver_triple.cpp src/$1/$2.cpp src/common_functions.cpp -o bin/$2 -lzmq
  else
    echo "clang++ -std=c++17 -Iinclude src/replicated.cpp src/$1/$2.cpp src/common_functions.cpp -o bin/$2 -lzmq"
    clang++ -std=c++17 -Iinclude src/replicated.cpp src/$1/$2.cpp src/common_functions.cpp -o bin/$2 -lzmq
  fi

  exit_code=$?
  if [ $exit_code -eq 0 ]; then
    if [ $2 = "database" ]; then
      echo "bin/$2 $3"
      echo ""
      bin/$2 $3
    else 
      echo "bin/$2"
      echo ""
      bin/$2
    fi
  else
    echo "failed in compiling (status: $exit_code)"
    exit $exit_code
  fi  
else
  if [ $1 = "main" ]; then
    echo "executing main..."
    echo "clang++ -std=c++17 -Iinclude src/*.cpp -o bin/main"
    clang++ -std=c++17 -Iinclude src/*.cpp -o bin/main
  else
    echo "testing $1..."
    echo "clang++ -std=c++17 -Iinclude {tests/test_,src/}$1.cpp -o bin/$1"
    clang++ -std=c++17 -Iinclude {tests/test_,src/}$1.cpp -o bin/$1
  fi

  exit_code=$?
  if [ $exit_code -eq 0 ]; then
    echo "bin/$1"
    echo ""
    bin/$1
  else
    echo "failed in compiling (status: $exit_code)"
    exit $exit_code
  fi  
fi

