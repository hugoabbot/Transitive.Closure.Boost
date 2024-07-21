/**
 * @file tc.hpp
 * @brief Helper file for tc.cpp to read in and parse adjacency matrices in .csv file format
 *
 */

#include <fstream>
#include <sstream>
#include <iostream>
#include <vector>
#include <limits>
#include <string>
#include <filesystem>


// Function to read in a .csv file and update 'adjacencyList' and 'numVertices' accordingly
bool readCSV(const std::string &filename, std::vector<int> &adjacencyList, int &numVertices) {
    std::ifstream file(filename);
    if (!file.is_open()) {
        std::cerr << "Error opening file: " << filename << std::endl;
        return false;
    }
    
    std::string line;
    if (std::getline(file, line)) {
        std::istringstream iss(line);
        iss >> numVertices;
    } 
    else {
        std::cerr << "Error reading number of vertices from file: " << filename << std::endl;
        return false;
    }
    
    while (std::getline(file, line)) {
        std::istringstream iss(line);
        std::string value;

        while (std::getline(iss, value, ',')) {
            if (value.find("inf") != std::string::npos) {
                adjacencyList.push_back(INT_MAX);
            } 
            else {
                try {
                    adjacencyList.push_back(std::stoi(value));
                } catch (const std::invalid_argument& e) {
                    std::cerr << "Invalid value in file: " << value << std::endl;
                    return false;
                } catch (const std::out_of_range& e) {
                    std::cerr << "Value out of range in file: " << value << std::endl;
                    return false;
                }
            }
        }
    }
    
    file.close();
    return true;
}

// Function to write the result adjacency matrix to a new .csv file in 'results/'
bool writeCSV(const std::string &filename, const std::vector<std::vector<int>> &resultMatrix) {
    std::filesystem::path inputPath(filename);
    std::filesystem::path outputPath = "results";
    outputPath /= inputPath.stem();
    outputPath += "_result.csv";

    std::ofstream outFile(outputPath);

    if (!outFile.is_open()) {
        std::cerr << "Error opening file for writing: " << outputPath << std::endl;
        return false;
    }

    for (const auto &row : resultMatrix) {
        for (size_t i = 0; i < row.size(); ++i) {
            if (row[i] == INT_MAX) {
                outFile << "inf";
            } 
            else {
                outFile << row[i];
            }
            
            if (i < row.size() - 1) {
                outFile << ",";
            }
        }
        outFile << "\n";
    }

    outFile.close();
    return true;
}
