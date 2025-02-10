#ifndef FILEREADER_H
#define FILEREADER_H

#include <vector>
#include <sstream>
#include <fstream>
using namespace std;
#include "Record.h"
#include "Operator.h"


// This function takes in a string with items delimited by commas
// and returns a vector of those items.
// Example - input: "str1,str2,str3,str4"
//          output: {"str1", "str2", "str3", "str4"}
vector<string> split(const string& line) {
    vector<string> v;
    string s;
    istringstream is(line);
    while (getline(is, s, ','))
        v.push_back(s);
    return v;
}


class FileReader : public Operator {
private:
    string fileName;
    ifstream file;
    vector<string> attrNames;
    vector<string> types;

public:
    void open() override {
        file.open(fileName);
        string line;
        getline(file, line);
        attrNames = split(line);
        getline(file, line);
        types = split(line);
        getline(file, line);
    }

    vector<Record> next() override{
        vector<Record> outPage;

        string line;
        for(size_t i = 0; i < pageSize && getline(file, line); i++) {
            vector<string> values = split(line);
            outPage.emplace_back(attrNames, types, values );
        }

        // Return an empty vector if no more pages exist
        return outPage;
    }

    void close() override {
        file.close();
    }

    void setFileName(const string& fName) {
        fileName = fName;
    }
};

#endif
