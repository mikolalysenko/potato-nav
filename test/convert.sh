#!/bin/sh

mkdir -p data

./bin/gen-graph 10 80 > ./data/test.arc
./bin/arc2csr ./data/test.csr < ./data/test.arc
./bin/csr2arc ./data/test.csr > ./data/test2.arc
./bin/arc2csr ./data/test2.csr < ./data/test2.arc

diff ./data/test.csr ./data/test2.csr
