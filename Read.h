#ifndef ASSIGNMENT3_READ_H
#define ASSIGNMENT3_READ_H
#include "fstream"

class Read {
public:
    std::string read(const std::string& fileName){
        std::ifstream file(fileName, std::ios::in);
        std::string line;
        std::string a;
        while (getline(file, line)){
            a += line;
            a += "\n";
        }
        return a;
    }
};


#endif //ASSIGNMENT3_READ_H
