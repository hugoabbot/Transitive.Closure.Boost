/**
 * @file tc.cpp
 * @brief CPU implementattion of the Floyd-Warshall algorithm for transitive closure using Boost
 *
 */

#include <iostream>
#include <vector>
#include <chrono>
#include <fstream>
#include <limits>

#include <boost/graph/directed_graph.hpp>
#include <boost/graph/exterior_property.hpp>
#include <boost/graph/floyd_warshall_shortest.hpp>

#include "tc.hpp"

// Max edge value mocking as infinity
#define inf numeric_limits<float>::infinity();

// Boost type declarations
typedef double t_weight;
typedef boost::property<boost::edge_weight_t, t_weight> EdgeWeightProperty;
typedef boost::adjacency_list<boost::vecS, boost::vecS, boost::directedS, boost::no_property, EdgeWeightProperty> Graph;
typedef boost::property_map<Graph, boost::edge_weight_t>::type WeightMap;
typedef boost::exterior_vertex_property<Graph, t_weight> DistanceProperty;
typedef DistanceProperty::matrix_type DistanceMatrix;
typedef DistanceProperty::matrix_map_type DistanceMatrixMap;


void usage() {
    fprintf(stderr, "\nUsage:  ./tc.out <input_file>\n");
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
            if (adjacencyList[(i * verticesCount) + j] != INT_MAX && adjacencyList[(i * verticesCount) + j] != 0) {
                ++edgeCount;
            }
        }
    }
  
    Graph graph;
    int *edgeVertices = new int[edgeCount * 2];
    t_weight *edgeWeights = new t_weight[edgeCount];

    int verticesIndex = 0, weightsIndex = 0;
    for (int i = 0; i < verticesCount; ++i) {
        for (int j = 0; j < verticesCount; ++j) {
            if (adjacencyList[(i * verticesCount) + j] != INT_MAX && adjacencyList[(i * verticesCount) + j] != 0) {
                edgeVertices[verticesIndex++] = i;
                edgeVertices[verticesIndex++] = j;
                edgeWeights[weightsIndex++] = adjacencyList[(i * verticesCount) + j];
            }
        }
    }

    // for (int i = 0; i < edgeCount; ++i) {
    //     std::cout << edgeWeights[i] << std::endl;
    // }

    // for (int i = 0; i < edgeCount * 2; ++i) {
    //     std::cout << edgeVertices[i++] << " ";
    //     std::cout << edgeVertices[i] << std::endl;

    // }

    for (size_t i = 0; i < edgeCount; ++i) {
        boost::add_edge(edgeVertices[i * 2], edgeVertices[i * 2 + 1], edgeWeights[i], graph);
    }

    WeightMap weightMap = boost::get(boost::edge_weight, graph);
    DistanceMatrix boostMatrix(num_vertices(graph));
    DistanceMatrixMap distanceMatrixMap(boostMatrix, graph);
  
    // Start timing
    auto startTime = std::chrono::high_resolution_clock::now();

    bool valid = floyd_warshall_all_pairs_shortest_paths(graph, distanceMatrixMap, boost::weight_map(weightMap));
    if (!valid) {
        std::cerr << "Error: Negative cycle in matrix" << std::endl;
        return 1;
    }

    // End Timing
    auto endTime = std::chrono::high_resolution_clock::now();

    std::chrono::duration<double, std::milli> elapsedTime = endTime - startTime;
    std::cout << "Duration: " << std::fixed << std::setprecision(3) << elapsedTime.count() << " ms." << std::endl;

    // Validation
    std::vector<std::vector<int>> resultMatrix (verticesCount, std::vector<int>(verticesCount));
    std::vector<std::vector<int>> cpuBaseline (verticesCount, std::vector<int>(verticesCount));

    for (int i = 0; i < verticesCount; ++i) {
        for (int j = 0; j < verticesCount; ++j) {
            cpuBaseline[i][j] = adjacencyList[(i * verticesCount) + j];
            
            // Casting and further checks to avoid int32 overflow from Boost API call
            resultMatrix[i][j] = static_cast<int> (boostMatrix[i][j]);
            if (resultMatrix[i][j] == INT_MIN) {
                resultMatrix[i][j] = INT_MAX;
            }
        }
    }

    for (int k = 0; k < verticesCount; ++k) {
        for (int i = 0; i < verticesCount; ++i) {
            for (int j = 0; j < verticesCount; ++j) {
                // Avoiding int32 overflow by avoiding computations with 'inf', no impact to result
                if (!(cpuBaseline[i][k] == INT_MAX || cpuBaseline[k][j] == INT_MAX) && (cpuBaseline[i][j] > (cpuBaseline[i][k] + cpuBaseline[k][j]))) {
                    cpuBaseline[i][j] = cpuBaseline[i][k] + cpuBaseline[k][j];
                }
            }
        }
    }

    bool errorPresent = false;
    for (int i = 0; i < verticesCount; ++i) {
        for (int j = 0; j < verticesCount; ++j) {
            if (cpuBaseline[i][j] != resultMatrix[i][j]) {
                std::cout << "Incorrect result at index [" << i << "," << j << "] | Boost CPU = " << resultMatrix[i][j] << " vs. Baseline CPU = " << cpuBaseline[i][j] << std::endl;
                errorPresent = true;
            }
        }
    }

    if (!errorPresent) {
        std::cout << "Correct!" << std::endl;
    }

    return 0;
}
