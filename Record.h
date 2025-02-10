#ifndef RECORD_H
#define RECORD_H

#include <iostream>
#include <map>
#include <any>
#include <iomanip>
using namespace std;

class Record {
public:
    vector<tuple<string, any>> rData;

    Record(const vector<string>& attrNames, const vector<string>& types, const vector<string>& values){
        for (int i = 0; i < attrNames.size(); i++) {
            any value;
            const string& type = types[i];
            if (type == "String"){
                value = values[i];
            }
            else if (type == "Double"){
                value = stod(values[i]);
            }
            else {
                value = stoi(values[i]);
            }
            rData.emplace_back(attrNames[i], value);
        }
    }

    Record(const vector<tuple<string, any>>& rData) : rData(rData) {}

    friend ostream& operator<< (ostream& os, const Record& r) {
        string output;
        for (tuple<string, any> col : r.rData) {
            any value = get<1>(col);
            if(value.type() == typeid(string)) {
                os << " " + any_cast<string>(value);
            }
            else if (value.type() == typeid(double)) {
                os << " " << std::fixed << setprecision(2) << any_cast<double>(value);
            }
            else{
                os << " " + to_string(any_cast<int> (value));
            }
        }
        return os;
    }

    friend Record operator+ (Record r1, Record r2) {
        vector<tuple<string, any>> data;
        data.reserve(r1.rData.size() + r2.rData.size() - 1);
        data.insert(data.end(), r1.rData.begin(), r1.rData.end());
        data.insert(data.end(), r2.rData.begin(), r2.rData.end());
        Record result(data);
        return result;
    }
};

#endif
