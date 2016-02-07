cmake_minimum_required(VERSION 2.8)
project(SPUD)

add_definitions(-std=c++14)

add_executable(arc2csr arc2csr.cc csr.cc)
add_executable(csr2arc csr2arc.cc csr.cc)
add_executable(gen-graph gen-graph.cc)
add_executable(sssp sssp.cc csr.cc)
add_executable(transpose transpose.cc csr.cc)
add_executable(test-dynindex
  test-dynindex.cc
  csr.cc
  generate.cc
  dynindex.cc
  nav.cc)
