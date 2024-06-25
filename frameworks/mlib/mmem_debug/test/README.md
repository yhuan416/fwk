# mmem_debug_test

## 1. 如何使用

mmem_debug本体用cmake进行构建, 默认会开启宏 MMEM_DEBUG_BUILD_TEST 

该宏用于开启测试程序的构建

在构建完成后, 进入构建目录(一般是build), 运行ctest即可

``` shell
mkdir -p build
cd build
cmake ..
cmake --build .
ctest --output-on-failure
```

构建时添加 -Duse_valgrind=1, 启用valgrind工具进行内存检测

``` shell
mkdir -p build
cd build
cmake .. -Duse_valgrind=1
cmake --build .
ctest --output-on-failure
```

## 2. 添加测试用例

cmake会自动扫描tests目录下的.c文件, 并将其编译为可执行文件

预留了一份测试用例模板, 位于 tests/00_template.c

可以将其复制一份, 并修改文件名, 以添加新的测试用例

## 3. 安装valgrind工具

``` shell
sudo apt-get update && sudo apt-get install -y valgrind
```
