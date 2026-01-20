## 動作環境
- Ubuntu 24.04
- clang 18.1
- C++17

## 実行方法
プロジェクトのルートディレクトリで作業してください。
- シェルスクリプトの実行権限付与
```
chmod +x compile_and_execute.sh
```
- 機能functionのテストをしたい場合
```
./compile_and_execute.sh function
```

- 通信プロトコルを起動する場合
1. ターミナルを4個（加法型秘密分散法）あるいは5個（複製型秘密分散法）起動する。tmuxでも可。
2. それぞれのターミナルでシェルスクリプトを走らせる。引数は以下を参照
```
./compile_and_execute.sh {起動したいプロトコルのディレクトリ名} {起動するプログラム名} {databaseの場合起動するdatabaseの番号}
```
例：複製型秘密分散法のdatabase 2を起動したい場合
```
./compile_and_execute.sh replicated_communication database 2
```
