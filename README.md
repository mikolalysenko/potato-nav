potato-nav
==========
A work in progress navigation/path planning library for planet sized graphs.  Heavily inspired by the following work:

* D. Delling, A. Goldberg, T. Pajor, R. Werneck. (2014) "[Robust Exact Distance Queries on Massive Networks](http://research-srv.microsoft.com/pubs/208867/complexTR-rev2.pdf)" MSR Technical Report

This project is organized as a collection of small programs that perform various graph processing related tasks and communicate with some common file formats.

Once the preprocessing works well enough, it should be possible to take an index that this project generates and serve this over some content-addressable store (like bittorrent), thus allowing for fast peer-to-peer GPS navigation.  Maybe even in a web browser!

# Set up

## Building

First install cmake, then run the following commands:

```
mkdir build
cd build
cmake ..
```

That will give you a Makefile which you can run using `make`. This project won't work on Windows or systems that don't have gcc/modern mmap features, but I am ok with this.

## Workflow

1. Convert input data into arc list
1. Convert arc list into CSR file
1. Generate a transposed CSR file
1. Construct index from forward and transposed CSR file
1. Query index to get fast network distances

Up to querying, the first 4 steps in this process will eventually get wrapped into a shell script once everything is working.  The index files will be served on something like IPFS or WebTorrent and a JavaScript client for generating routes and querying the index will be written.

## Testing

**TODO**

# Tools

### `arc2csr`
Converts an ARC list into a CSR adjacency list.

### `csr2arc`
Converts a CSR adjacency list into an ARC list.

### `transpose`
Transposes a CSR list.

### `sssp`
Single Source Shortest Path: Computes all path distances from a single source node.

# File formats

### ARC: Arc list

A plain text file giving a list of arcs and their lengths.  The first line in the file is a single number giving the total number of vertices in the graph, followed by a list of directed arcs, each represnted as a `source target length` triple.  For example, a file with `n` vertices and arcs `(s0,t0), (s1,t1), ...` with respective arc lengths of `l0, l1, ...` would look like this:

```
n
s0 t0 l0
s1 t1 l1
s2 t2 l2
...
```

### CSR: Compressed sparse adjacency list (binary)

A compressed binary adjacency list.  This is similar to an arc list, except that it uses less memory and can be directly `mmap`'d  and queried.

### NAV: Navigation index (binary)

A compressed and query-able index for distance queries.

# Bibliography

* D. Delling, A. Goldberg, R. Werneck. (2014) "[Hub Labeling (2-Hop Labeling)]( http://www.cs.princeton.edu/~rwerneck/papers/DGW14-hl-encyclopedia.pdf)"

* D. Delling, A. Goldberg, T. Pajor, R. Werneck. (2014) "[Robust Exact Distance Queries on Massive Networks](http://research-srv.microsoft.com/pubs/208867/complexTR-rev2.pdf)" MSR Technical Report

* D. Delling, J. Dibbelt, T. Pajor, R. Werneck. (2015) "[Public Transit Labeling](http://arxiv.org/pdf/1505.01446.pdf)" arxiv:1505.01446

# License
(c) 2015 Mikola Lysenko. MIT License
