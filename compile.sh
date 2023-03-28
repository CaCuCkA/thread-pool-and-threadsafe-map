#!/bin/bash

# Colors
RED="\e[31m"
YELLOW='\e[33m'
GREEN='\e[32m'
NORMAL='\e[0m'

Compile() {
  mkdir -p build
  cd build || exit
  cmake ..
  cmake --build .
}

Clean() {
  echo -e "${YELLOW}Cleaning...${NORMAL}"
  if [ -d "build" ]; then rm -Rf build; fi
  if [ -d "venv" ]; then rm -Rf venv; fi
  cd data || exit
  if [ -f "integers_50M.txt" ]; then rm integers_50M.txt; fi
  cd ..
  echo -e "${GREEN}Done${NORMAL}"
}

Help() {
  echo "Usage: compile.sh [options]"
  echo "Options:"
  echo "  -h, --help"
  echo "  -c, --compile: Compile only"
  echo "  --clean: Clean compiled files and Build directory"
}

if [ $# -eq 0 ]; then
  Help
  exit 1
fi

# Check if there is more than one argument
if [ $# -gt 1 ]; then
  echo -e "${RED}Error: Too many arguments${NORMAL}"
  Help
  exit 1
fi

# Check if the argument is -h or --help
if [ "$1" = "-h" ] || [ "$1" = "--help" ]; then
  Help
  exit 0
fi

# Check if the argument is -c or --compile
if [ "$1" = "-c" ] || [ "$1" = "--compile" ]; then
  Compile
  exit 0
fi

# Check if the argument is clean
if [ "$1" = "--clean" ]; then
  Clean
  exit 0
fi

# Check if the argument is not valid
if [[ ! " ${valid_args[*]} " == *" $1 "* ]]; then
  echo -e "${RED}Error: Invalid argument${NORMAL}"
  Help
  exit 1
fi