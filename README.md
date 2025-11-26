## 動作環境
- Ubuntu 24.04
- clang 18.1
- C++17

## 実行方法
プロジェクトのルートディレクトリで作業してください。

### main関数の実行方法
- コンパイル
```
clang++ -std=c++17 -Iinclude src/*.cpp -o bin/main
```
- 実行
```
bin/main
```

### 機能ごとのテストの実行方法
- コンパイル
```
clang++ -std=c++17 -Iinclude {tests/test_,src/}function.cpp -o bin/function
```
- 実行
```
bin/function
```
