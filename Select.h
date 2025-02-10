#ifndef SELECT_H
#define SELECT_H

#include <utility>

#include "Operator.h"

class Select : public Operator {
private:
    Operator* input;
    FileReader fr;
    vector<Record> currInPage;
    bool initialized = false;
    int pageIndex = 0;
    int attrIndex = -1;
    int attrType; // 1 is string, 2 is double, 3 is integer
    string p_attr;
    string p_op;
    string p_val;
    string sValue;
    double dValue;
    int iValue;
public:
    // Select constructors
    Select(const string& fileName, string p_attr, string p_op, string p_val) : p_attr(std::move(p_attr)), p_op(std::move(p_op)), p_val(std::move(p_val)) {
        fr.setFileName(fileName);
        input = &fr;
    }

    Select(Operator *op, string  p_attr, string p_op, string p_val) : p_attr(std::move(p_attr)), p_op(std::move(p_op)), p_val(std::move(p_val)) {
        input = op;
    }

    void open() override {
        input -> open();
    }

    vector<Record> next() override {
        if (!initialized){
            initialize();
        }
        vector<Record> outPage;

        while (outPage.size() < pageSize) {
            if (currInPage.empty()) {
                pageIndex = 0;
                currInPage = input -> next();
                if (currInPage.empty()) return outPage;
            }

            any value = get<1>(currInPage[pageIndex].rData[attrIndex]);
            if (testRecord(value))
                outPage.push_back(currInPage[pageIndex]);
            pageIndex++;

            if (pageIndex == currInPage.size())
                currInPage.clear();
        }

        return outPage;
    }

    bool testRecord(any value) {
        switch (attrType) {
            case 1: // string
                if (p_op == "<") {
                    return any_cast<string>(value) < sValue;
                } else if (p_op == ">") {
                    return any_cast<string>(value) > sValue;
                } else if (p_op == "=") {
                    return any_cast<string>(value) == sValue;
                } else {
                    return any_cast<string>(value) != sValue;
                }
            case 2: // double
                if (p_op == "<") {
                    return any_cast<double>(value) < dValue;
                } else if (p_op == ">") {
                    return any_cast<double>(value) > dValue;
                } else if (p_op == "=") {
                    return any_cast<double>(value) == dValue;
                } else {
                    return any_cast<double>(value) != dValue;
                }
            case 3: // integer
                if (p_op == "<") {
                    return any_cast<int>(value) < iValue;
                } else if (p_op == ">") {
                    return any_cast<int>(value) > iValue;
                } else if (p_op == "=") {
                    return any_cast<int>(value) == iValue;
                } else {
                    return any_cast<int>(value) != iValue;
                }
        }
        return false;
    }

    void initialize(){
        initialized = true;
        currInPage = input -> next();

        if (!currInPage.empty()) {
            int index = 0;
            for (index; index < currInPage[0].rData.size(); index++) {
                if (p_attr == get<0>(currInPage[0].rData[index])) {
                    attrIndex = index;
                    any value = get<1>(currInPage[0].rData[index]);
                    if(value.type() == typeid(string)) {
                        attrType = 1;
                        sValue = p_val;
                    }
                    else if (value.type() == typeid(double)) {
                        attrType = 2;
                        dValue = stod(p_val);
                    }
                    else{
                        attrType = 3;
                        iValue = stoi(p_val);
                    }
                    break;
                }
            }
        }
    }

    void close() override {
        input -> close();
    }
};

#endif
