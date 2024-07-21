/**
 * @file tc.cpp
 * @brief Implementattion of the Floyd-Warshall algorithm for transitive closure using Boost
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
    // Begin data parsing
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
    // End data parsing
    
    std::vector<std::vector<int>> cpuBaseline (verticesCount, std::vector<int>(verticesCount));
    int edgeCount = 0;

    // Finding the number of valid edges and creating an adjacency matrix copy for future answer validation
    for (int i = 0; i < verticesCount; ++i) {
        for (int j = 0; j < verticesCount; ++j) {
            if (adjacencyList[(i * verticesCount) + j] != INT_MAX && adjacencyList[(i * verticesCount) + j] != 0) {
                ++edgeCount;
            }
            cpuBaseline[i][j] = adjacencyList[(i * verticesCount) + j];
        }
    }
  
    Graph graph;
    int *edgeVertices = new int[edgeCount * 2];
    t_weight *edgeWeights = new t_weight[edgeCount];

    // Storing all valid edges
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

    // Casting and further checks to avoid int32 overflow from Boost API call
    std::vector<std::vector<int>> resultMatrix (verticesCount, std::vector<int>(verticesCount));
    for (int i = 0; i < verticesCount; ++i) {
        for (int j = 0; j < verticesCount; ++j) {
            resultMatrix[i][j] = static_cast<int> (boostMatrix[i][j]);
            if (resultMatrix[i][j] == INT_MIN) {
                resultMatrix[i][j] = INT_MAX;
            }
        }
    }

    // CPU baseline calculations
    for (int k = 0; k < verticesCount; ++k) {
        for (int i = 0; i < verticesCount; ++i) {
            for (int j = 0; j < verticesCount; ++j) {
                // Bypassing int32 overflows by ditching computations with 'inf' values, which has no impact on result
                if (!(cpuBaseline[i][k] == INT_MAX || cpuBaseline[k][j] == INT_MAX) && (cpuBaseline[i][j] > (cpuBaseline[i][k] + cpuBaseline[k][j]))) {
                    cpuBaseline[i][j] = cpuBaseline[i][k] + cpuBaseline[k][j];
                }
            }
        }
    }

    // Comparing CPU baseline results with Boost results
    bool errorPresent = false;
    for (int i = 0; i < verticesCount; ++i) {
        for (int j = 0; j < verticesCount; ++j) {
            if (cpuBaseline[i][j] != resultMatrix[i][j]) {
                std::cout << "Incorrect result at index [" << i << "," << j << "] | Boost = " << resultMatrix[i][j] << " vs. Baseline CPU = " << cpuBaseline[i][j] << std::endl;
                errorPresent = true;
            }
        }
    }

    if (!errorPresent) {
        std::cout << "Correct!" << std::endl;
        
        if (!writeCSV(argv[1], resultMatrix)) {
            std::cout << "Failed to write 'resultMatrix' to a new .csv file" << std::endl;
            return 1;
        }
    }

    return 0;
}
