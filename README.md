# fwk
framework

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
