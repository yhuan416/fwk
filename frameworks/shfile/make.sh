#!/bin/sh

FremeWorksPath=$(dirname $0)
FwkRoot=${FremeWorksPath}/../..
FwkBuild=${FwkRoot}/build

FwkPlatform=linux-x64
FwkBuildType=Debug # Debug, Release
FwkBuildSharedLibs=FALSE # default dont build shared libs
FwkBuildTest=FALSE # default dont build test
FwkTarget=all

FwkInstall=0 # install flag, default dont install

# 参数解析 --platform linux-x64 --build-type Debug --build-shared-libs --build-test --target all --install
for arg in $@
do
    case $arg in
        --platform)
            shift
            FwkPlatform=$1
            ;;
        --build-type)
            shift
            FwkBuildType=$1
            ;;
        --build-shared-libs)
            FwkBuildSharedLibs=TRUE
            ;;
        --build-test)
            FwkBuildTest=TRUE
            ;;
        --target)
            shift
            FwkTarget=$1
            ;;
        --install)
            FwkInstall=1
            ;;
        --help)
            echo "Usage: $0 [--platform linux-x64] [--build-type Debug] [--build-shared-libs] [--build-test] [--target all] [--install]"
            exit 0
            ;;
    esac
done

# Configure
cmake -DCMAKE_INSTALL_PREFIX=${FwkRoot}/platforms/${FwkPlatform} \
        -DCMAKE_BUILD_TYPE:STRING=${FwkBuildType} \
        -DCMAKE_EXPORT_COMPILE_COMMANDS:BOOL=TRUE \
        -DBUILD_SHARED_LIBS:BOOL=${FwkBuildSharedLibs} \
        -DBUILD_TEST:BOOL=${FwkBuildTest} \
        -B${FwkBuild} \
        -S${FwkRoot} \
        -G Ninja

# Build
cmake --build ${FwkBuild} --config ${FwkBuildType} --target ${FwkTarget} --verbose

# Inistall
if [ ${FwkInstall} -eq 1 ]; then
    cmake --install ${FwkBuild}
fi
