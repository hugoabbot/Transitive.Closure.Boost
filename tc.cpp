/**
 * @file tc.cpp
 * @brief CPU implementattion of the Floyd-Warshall algorithm for transitive closure using Boost
 */

#include <iostream>
#include <vector>
#include <chrono>
#include <fstream>
#include <limits.h>

#include <boost/graph/undirected_graph.hpp>
#include <boost/graph/exterior_property.hpp>
#include <boost/graph/floyd_warshall_shortest.hpp>

#include "csv_reader.hpp"

// brew install boost

// Max edge value mocking as infinity
#define MAX_EDGE_VALUE 99999

// Boost type declarations
typedef double t_weight;
typedef boost::property<boost::edge_weight_t, t_weight> EdgeWeightProperty;
typedef boost::adjacency_list<boost::vecS, boost::vecS, boost::directedS, boost::no_property, EdgeWeightProperty> Graph;
typedef boost::property_map<Graph, boost::edge_weight_t>::type WeightMap;
typedef boost::exterior_vertex_property<Graph, t_weight> DistanceProperty;
typedef DistanceProperty::matrix_type DistanceMatrix;
typedef DistanceProperty::matrix_map_type DistanceMatrixMap;

void usage() {
    fprintf(stderr, "\nUsage:  ./tc-boost.out <input_file>\n");
}

int main(int argc, char *argv[]) {
    if (argc != 2) {
        usage();
        return 1;
    }

    std::vector<int> adjacencyList;
    int verticesCount;

    if (!readCSV(argv[1], adjacencyList, verticesCount)) {
      std::cout << "Failed to read input file or does not exist" << std::endl;
      return 1;
    }

    int edgeCount = 0;
    for (int i = 0; i < verticesCount; ++i) {
        for (int j = 0; j < verticesCount; ++j) {
            if (adjacencyList[(i * verticesCount) + j] != MAX_EDGE_VALUE) {
                ++edgeCount;
            }
        }
    }
  
    Graph g;
    int *edges = new int[edgeCount * 2];
    t_weight *weight = new t_weight[edgeCount];

    int edgesIndex = 0, weightIndex = 0;
    for (int i = 0; i < verticesCount; ++i) {
        for (int j = 0; j < verticesCount; ++j) {
            if (adjacencyList[(i * verticesCount) + j] != MAX_EDGE_VALUE) {
                edges[edgesIndex] = i;
                edges[++edgesIndex] = j;
                weight[weightIndex] = adjacencyList[(i * verticesCount) + j];
                ++edgesIndex;
                ++weightIndex;
            }
        }
    }

    for (std::size_t i = 0; i < edgeCount; ++i) {
        boost::add_edge(edges[i * 2], edges[i * 2 + 1], weight[i], g);
    }

    WeightMap weightPMap = boost::get(boost::edge_weight, g);
    DistanceMatrix calculatedEdges(num_vertices(g));
    DistanceMatrixMap dm(calculatedEdges, g);
  
    // Start timing
    auto start = std::chrono::high_resolution_clock::now();

    bool valid = floyd_warshall_all_pairs_shortest_paths(g, dm, boost::weight_map(weightPMap));
    if (!valid) {
        std::cerr << "Error: Negative cycle in matrix" << std::endl;
        return 1;
    }

    // End Timing
    auto end = std::chrono::high_resolution_clock::now();

    std::chrono::duration<double, std::milli> elapsedTime = end - start;
    std::cout << "Duration: " << std::fixed << std::setprecision(3) << elapsedTime.count() << " ms." << std::endl;

//     std::cout << "Distance matrix: " << std::endl;
//   for (std::size_t i = 0; i < num_vertices(g); ++i) {
//     for (std::size_t j = 0; j < num_vertices(g); ++j) {
//       std::cout << "From vertex " << i << " to " << j << " : ";
//       if(calculatedEdges[i][j] == MAX_EDGE_VALUE)
//         std::cout << "inf" << std::endl;
//       else
//         std::cout << calculatedEdges[i][j] << std::endl;
//     }
//     std::cout << std::endl;
//   }

    // Verification and validation
    std::vector<std::vector<int>> cpuBaseline (verticesCount, std::vector<int>(verticesCount));
    std::vector<std::vector<int>> test (verticesCount, std::vector<int>(verticesCount));

    for (int i = 0; i < verticesCount; ++i) {
        for (int j = 0; j < verticesCount; ++j) {
            cpuBaseline[i][j] = adjacencyList[(i * verticesCount) + j];
            test[i][j] = static_cast<int> (calculatedEdges[i][j]);
            if (test[i][j] == INT_MIN) {
                test[i][j] = MAX_EDGE_VALUE;
            }
        }
    }

    for (int k = 0; k < verticesCount; ++k) {
        for (int i = 0; i < verticesCount; ++i) {
            for (int j = 0; j < verticesCount; ++j) {
                if (cpuBaseline[i][j] > (cpuBaseline[i][k] + cpuBaseline[k][j])) {
                    cpuBaseline[i][j] = cpuBaseline[i][k] + cpuBaseline[k][j];
                }
            }
        }
    }

    int errorFlag = 0;
    for (int i = 0; i < verticesCount; ++i) {
        for (int j = 0; j < verticesCount; ++j) {
            std::cout << cpuBaseline[i][j] << " | " << test[i][j] << std::endl;
            if (cpuBaseline[i][j] != test[i][j] && i != j) {
                std::cout << "Incorrect result at index [" << i << "," << j << "] | Boost CPU = " << test[i][j] << " vs. Baseline CPU = " << cpuBaseline[i][j] << std::endl;
                errorFlag = 1;
            }
        }
    }

    if (!errorFlag) {
        std::cout << "Correct!" << std::endl;
    }

    return 0;
}
