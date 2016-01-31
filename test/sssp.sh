#!/bin/sh

mkdir -p data

echo "3 0 1 1 1 2 1" | ./bin/arc2csr ./data/test.csr
./bin/sssp ./data/test.csr 0 > ./data/dist.txt
