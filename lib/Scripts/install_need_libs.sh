#!/bin/bash

check_and_install_libraries() {
  local libraries=("$@")
  
  for library in "${libraries[@]}"; do
    dpkg -s "$library" &> /dev/null
    if [ $? -eq 0 ]; then
      echo "Library $library already installed"
    else
      echo "Install library $library"
      sudo apt install -y "$library"
      sudo ldconfig
    fi
  done
}

sudo apt update
check_and_install_libraries git clang-format cmake gcc g++
git clone https://github.com/google/googletest.git lib/googletest
