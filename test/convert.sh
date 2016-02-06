#!/bin/sh

mkdir -p data

./build/gen-graph 10 80 > ./data/test.arc
./build/arc2csr ./data/test.csr < ./data/test.arc
./build/csr2arc ./data/test.csr > ./data/test2.arc
./build/arc2csr ./data/test2.csr < ./data/test2.arc

diff ./data/test.csr ./data/test2.csr
