# Max Weighted Matching

This project finds a maximum size matching of maximum weight in a weighted graph. A matching is a set of edges without common vertices, and the weight of a matching is the sum of weights of the edges. The algorithm finds a largest possible matching and optimizes the weight of that matching.

The code was written in 2020 to solve this problem as a replacement for an Advanced Algorithms exam. It uses a compact competitive programming style, with the entire implementation in a single C++ file. This allowed quick validation with the grading system.

While not ideal for reuse, the code achieved its purpose at the time. Today I would implement it in a more modular way with better documentation. But I'm still proud this program powered me through a difficult exam when getting started in algorithms.

## Input-output format

### Input

First line contains number of vertices `n`. Following lines describe edges as triplets `i j w` 
where `i`, `j` describe ends of an edge as numbers from 1 to n, `w` is a positive real number for weight.

For unweighted version edge weights are omitted.

### Output

First line contains number of covered vertices and total weight. Next lines contain pairs of matched vertices.

## Executables

Everything is built using `make`.

* `W.elf`: solves weighted case. Uses standard input and output.
* `A.elf`: solves unweighted case (in practice sets every weight to 1). Uses standard input and output.

* `check`: Run `./W.elf < file | ./check file` to check correctness.
* `next_graph`: Reads the file `graph` and rewrites it with the next graph in lexicographical order. Uses random weight.

## Commands

* `make random_test`: Checks correctness on 10000 random graphs.
* `make test`: Runs tests from `wtest/` folder. Runs valgrind to check for memory leaks.
* `make stress_test`: Infinitely checks every graph in lexicographical order. Quickly mash Ctrl+C to stop it =)

# Comments

Used the book Combinatorial Optimization: Networks and Matroids (https://vdoc.mx/download/combinatorial-optimization-networks-and-matroids-794qnvjetn10), 
"An O(n^4) Weighted Matching Algorithm"

At this point I'm not sure this implementation runs in O(n^4) time, but it's certainly polynomial.

Also the book has a mistake in description of label updating procedure when applying alternating path, 
which is one of the crucial differences from the unweighted algorithm.
