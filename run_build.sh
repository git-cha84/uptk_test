#!/bin/bash

./lib/Scripts/install_need_libs.sh
./lib/Scripts/auto_format_clang_format.sh

if [ -d "build" ];
	then rm -r build; 
fi

cmake -Bbuild/debug -DCMAKE_BUILD_TYPE=Debug
cmake -Bbuild/release -DCMAKE_BUILD_TYPE=Release
cd build/
cd debug/
make -j`nproc`
ctest
cd ..
cd release
make -j`nproc`
ctest
