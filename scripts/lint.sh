#!/usr/bin/env bash
set -e

cpplint --recursive --exclude="/build/" dev/ tests/
