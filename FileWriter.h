#ifndef FILEWRITER_H
#define FILEWRITER_H

//#include "Operator.h"
#include "Record.h"
#include <fstream>
//#include <sstream>
#include <iostream>
#include <filesystem>


class FileWriter {
private:
    ofstream file;
    string fileName;
    bool initialized = false;

public:
    void open (){
        file.open(fileName);
    }

    void write(vector<Record> page) {
        if (page.empty()){
            return;
        }
        if (!initialized){
            initialize(page[0]);
        }
        ostringstream oss;
        for (const Record& record : page) {
            for (tuple<string, any> attribute : record.rData) {
                any value = get<1>(attribute);
                if(value.type() == typeid(string)) {
                    oss << any_cast<string>(value) << ",";
                }
                else if (value.type() == typeid(double)) {
                    oss << std::fixed << setprecision(2) << any_cast<double>(value) << ",";
                }
                else{
                    oss << to_string(any_cast<int> (value)) << ",";
                }
            }
            string line = oss.str();
            line.pop_back();
            file << line << endl;
            oss.str("");
        }
    }

    void initialize(const Record& record) {
        initialized = true;
        string names;
        string types;

        for (tuple<string, any> attribute : record.rData) {
            names += (get<0>(attribute)) + ",";
            any value = get<1>(attribute);
            if(value.type() == typeid(string)) {
                types += "String,";
            }
            else if (value.type() == typeid(double)) {
                types += "Double,";
            }
            else{
                types += "Integer,";
            }
        }
        names.pop_back();
        types.pop_back();

        file << names << endl;
        file << types << endl;
        file << "----------------------" << endl;
    }

    void setFileName(const string& fName) {
        fileName = fName;
    }

    void close(){
        file.close();
        filesystem::remove(fileName);
    }

};

#endif
