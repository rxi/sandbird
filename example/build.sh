#!/bin/bash

if [ "$#" -ne 1 ]; then
  echo "usage: build.sh FILENAME"
  exit
fi

gcc $1 ../src/*.c -I ../src/ -std=c89 -pedantic -Wall -Wextra
