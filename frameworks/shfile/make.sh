#!/bin/sh -x

FremeWorksPath=$(dirname $0)
FwkRoot=${FremeWorksPath}/../..
FwkBuild=${FwkRoot}/build

cmake -DCMAKE_INSTALL_PREFIX=${FwkRoot}/platforms/linux-x64 \
        -DCMAKE_BUILD_TYPE:STRING=Debug \
        -DCMAKE_EXPORT_COMPILE_COMMANDS:BOOL=TRUE \
        -DBUILD_SHARED_LIBS:BOOL=TRUE \
        -DBUILD_TEST:BOOL=TRUE \
        -B${FwkBuild} \
        -S${FwkRoot}
        -G Ninja

cmake --build ${FwkBuild} --config Debug --target all --verbose

cmake --install ${FwkBuild}
