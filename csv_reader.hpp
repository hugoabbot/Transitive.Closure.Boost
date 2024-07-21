#include <fstream>
#include <sstream>
#include <iostream>

bool readCSV(const std::string &filename, std::vector<int> &inputList, int &numVertices) {
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
        int value;
        while (iss >> value) {
            inputList.push_back(value);
            if (iss.peek() == ',') {
                iss.ignore();
            }
        }
    }  
   
    file.close();
    return true;
}
