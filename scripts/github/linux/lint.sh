#!/bin/bash

python3 ./scripts/cpplint.py \
    --recursive \
    ./dev/inc \
    ./dev/src \
    ./tests