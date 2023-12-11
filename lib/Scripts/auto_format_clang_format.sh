#!/bin/bash
find ./lib/ -iname '*.cpp' -o -iname '*.h' | xargs clang-format -i --style=file
find ./src/ -iname '*.cpp' -o -iname '*.h' | xargs clang-format -i --style=file
find ./tst/ -iname '*.cpp' -o -iname '*.h' | xargs clang-format -i --style=file
