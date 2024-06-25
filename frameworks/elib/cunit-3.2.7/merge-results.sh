#!/bin/sh
set -ex

for build in $(ls -d build-*)
do
    junit2html --merge=${build}-Results.xml ${build}/Examples/CI/*-Results.xml
done