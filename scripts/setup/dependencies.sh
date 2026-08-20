#!/bin/bash

mkdir -p third_party
wget https://github.com/lief-project/LIEF/releases/download/0.17.6/LIEF-0.17.6-Linux-x86_64.tar.gz \
  -O /tmp/lief-sdk.tar.gz
tar xzf /tmp/lief-sdk.tar.gz -C third_party

rm -rf third_party/LIEF
mv third_party/LIEF-0.17.6-Linux-x86_64 third_party/LIEF
rm /tmp/lief-sdk.tar.gz
