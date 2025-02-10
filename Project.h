#ifndef PROJECT_H
#define PROJECT_H

#include <vector>
using namespace std;
#include "Record.h"
#include "Operator.h"
#include "FileReader.h"

class Project : public Operator {
private:
    Operator* input;
    FileReader fr;
    const vector<string> fieldNames;
    vector<int> indexes;
    bool initialized = false;

public:
    // Project constructors
    Project(const string& fileName, const vector<string>& fieldNames) : fieldNames(fieldNames) {
        fr.setFileName(fileName);
        input = &fr;
    }

    Project(Operator *op, const vector<string>& fieldNames) : fieldNames(fieldNames) {
        input = op;
    }

    void open() override {
        input->open();
    }

    vector<Record> next() override {
        vector<Record> inPage = input->next(); // get a page
        if (!initialized) {
            initialize(inPage[0]);
        }
        vector<Record> outPage;

        for (const Record& record : inPage) {
            outPage.push_back(resultingRecord(record));
        }

        // Return an empty vector if no more pages exist
        return outPage;
    }

    void close() override {
        input->close();
    }

    Record resultingRecord(const Record& record){
        vector<tuple<string, any>> newRecordData;

        for (int index : indexes) {
            newRecordData.push_back(record.rData[index]);
        }

        Record rRecord(newRecordData);

        return rRecord;
    }

    void initialize(const Record& record){
        initialized = true;
        for (const string& fieldName : fieldNames) {
            for (int i = 0; i < record.rData.size(); i++){
                if (fieldName == get<0>(record.rData[i])){
                    indexes.push_back(i);
                    break;
                }
            }
        }
    }
};

#endif
