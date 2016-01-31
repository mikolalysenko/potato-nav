#!/bin/sh
BIN=$(pwd)/../bin
DATA=$(pwd)/../data
OBJ=$(pwd)/../obj

$BIN/arc2csr $OBJ/test.csr < $DATA/test.arc
$BIN/sssp $OBJ/test.csr 0
