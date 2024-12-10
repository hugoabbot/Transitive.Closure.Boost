# transitive-closure-boost

## Overview

Floyd-Warshall transitive closure algorithm for weighted and directed graphs using the Boost API. This project took inspiration from a Stack Overflow [post](https://stackoverflow.com/questions/26855184/floyd-warshall-all-pairs-shortest-paths-on-weighted-undirected-graph-boost-g) for the same process but for undirected graphs.

## Boost

The Boost C++ library is required to installed and linked to use the source code. 

## Input Files

Input is required to be in the form of a .csv file, first listing the number of nodes followed by the adjacency matrix. An example input from the GeeksForGeeks [page](https://www.geeksforgeeks.org/floyd-warshall-algorithm-dp-16/) on the Floyd-Warshall algorithm can be found in `datafiles/` as `geeks_for_geeks_example.csv` and given below:

```bash
5
0,4,inf,5,inf
inf,0,1,inf,6
2,inf,0,3,inf
inf,inf,1,0,2
1,inf,inf,4,0
```

There are further examples given in `datafiles/`; however, personalized inputs can be created using `matrix_generator.py` as follows:

```bash
$ python3 matrix_generator.py <num_vertices> <sparsity_rate> <output_file>
```

`sparsity_rate` is the value that determines the number of vacant edges, and can range from 0 to 100. Additionally, one can upload their own input files, given that the format aligns with the rules given above. Given a successful run for the specified input file, the output will be stored in `results/` (ex. `test1.csv` being run, and the output saved as `test1_result.csv`).

## Compilation and Execution

The source code can first be compiled with:

```bash
$ make
```

followed by the instruction and usage to execute:

```bash
$ ./tc.out <input_file>
```
