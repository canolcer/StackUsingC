#ifndef ASSIGNMENT3_WRITE_H
#define ASSIGNMENT3_WRITE_H
#include "fstream"

class Write {
public:
    static void write(const std::string& writeStr, std::string filename){
        std::ofstream file(filename, std::ios::out);
        file << writeStr;
        file.close();
    }
};


#endif //ASSIGNMENT3_WRITE_H
