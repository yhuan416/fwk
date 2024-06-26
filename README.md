# fwk
framework

## Download

``` sh
git clone --recursive git@github.com:yhuan416/fwk.git
```

## Build

``` sh
mkdir -p build && cd build
cmake ..
make
```

## Build Examples

``` sh
mkdir -p build && cd build
cmake -DBUILD_EXAMPLES=ON ..
make
```

## Build Tests

``` sh
mkdir -p build && cd build
cmake -DBUILD_TEST=ON ..
make
cd framework
ctest
```

## Build & Install

``` sh
mkdir -p build && cd build
cmake -DBUILD_SHARED_LIBS=ON -DCMAKE_INSTALL_PREFIX=../platforms/linux-x64 ..
make
make install
```
