#ifndef JOIN_H
#define JOIN_H

#include "Operator.h"
#include "FileWriter.h"

class Join : public Operator {
private:
    Operator* input1;
    Operator* input2;
    Operator* tf;
    FileReader f1;
    FileReader f2;
    FileReader tempFile;
    FileWriter fw;
    string pAttr1;
    string pAttr2;
    int attrType; // 1 is string, 2 is double, 3 is integer
    int attr1Index = -1;
    int attr2Index = -1;
    vector<Record> leftPage;
    vector<Record> rightPage;
    int leftIndex = -1;
    int rightIndex = -1;
    static inline int tempFileCount = 0;
    bool materialize;
    bool firstPass = true;
public:

    Join(Operator *op1, Operator *op2, string pAttr1, string pAttr2) : pAttr1(pAttr1), pAttr2(pAttr2) {
        input1 = op1;
        input2 = op2;
        materialize = true;
        tempFile.setFileName("tempFile" + to_string(tempFileCount) + ".data");
        tf = &tempFile;
        fw.setFileName("tempFile" + to_string(tempFileCount) + ".data");
        tempFileCount++;
    }
    Join(Operator *op, const string& fileName, string pAttr1, string pAttr2) : pAttr1(pAttr1), pAttr2(pAttr2) {
        f2.setFileName(fileName);
        input1 = op;
        input2 = &f2;
        materialize = false;
    }
    Join(const string& fileName1, const string& fileName2, string pAttr1, string pAttr2) : pAttr1(pAttr1), pAttr2(pAttr2) {
        f1.setFileName(fileName1);
        f2.setFileName(fileName2);
        input1 = &f1;
        input2 = &f2;
        materialize = false;
    }

    void open() override {
        input1 -> open();
        input2 -> open();
        if (materialize){
            fw.open();
        }
    }

    vector<Record> next() override {
        vector<Record> outPage;

        while (outPage.size() < pageSize){
            advanceIterators();
            if (leftPage.empty()){
                return outPage;
            }

            Record leftRecord = leftPage[leftIndex];
            Record rightRecord = rightPage[rightIndex];

            if (testPredicate(get<1>(leftRecord.rData[attr1Index]), get<1>(rightRecord.rData[attr2Index]))){
                outPage.push_back(leftRecord + rightRecord) ;
            }
        }

        return outPage;
    }

    bool testPredicate(const any& value1,const any& value2) {
        switch (attrType) {
            case 1: // string
                return any_cast<string>(value1) == any_cast<string>(value2);
            case 2: // double
                return any_cast<double>(value1) == any_cast<double>(value2);
            case 3: // integer
                return any_cast<int>(value1) == any_cast<int>(value2);
        }
        return false;
    }

    void advanceIterators() {
        if (leftIndex == -1) {
            initalize();
        } else {
            if (rightIndex == rightPage.size()-1) {
                rightIndex = 0;
                leftIndex++;
                if (leftIndex == leftPage.size()) {
                    leftIndex = 0;
                    if (materialize){
                        if (firstPass){
                            rightPage = input2 -> next();
                            fw.write(rightPage);
                        } else {
                            rightPage = tf -> next();
                        }
                    } else {
                        rightPage = input2 -> next();
                    }
                    if (rightPage.empty()) {
                        if (materialize){
                            if (firstPass){
                                firstPass = false;
                                input2->close();
                                tf -> open();
                                rightPage = tf->next();
                            } else {
                                tf -> close();
                                tf -> open();
                                rightPage = tf->next();
                            }
                            leftPage = input1->next();
                        } else {
                            leftPage = input1->next();
                            input2 -> close();
                            input2 -> open();
                            rightPage = input2->next();
                        }
                    }
                    return;
                }
            } else {
                rightIndex++;
            }
        }
    }

    void initalize() {
        leftPage = input1 -> next();
        rightPage = input2 -> next();
        if (materialize){
            fw.write(rightPage);
        }
        leftIndex = 0;
        rightIndex = 0;

        for (int index = 0; index < leftPage[0].rData.size(); index++) {
            if (pAttr1 == get<0>(leftPage[0].rData[index])) {
                attr1Index = index;
                any value = get<1>(leftPage[0].rData[index]);
                if(value.type() == typeid(string)) {
                    attrType = 1;
                }
                else if (value.type() == typeid(double)) {
                    attrType = 2;
                }
                else{
                    attrType = 3;
                }
                break;
            }
        }
        for (int index = 0; index < rightPage[0].rData.size(); index++) {
            if (pAttr2 == get<0>(rightPage[0].rData[index])) {
                attr2Index = index;
                break;
            }
        }
    }

    void close() override{
        input1 -> close();
        input2 -> close();
        if (materialize){
            tf -> close();
            fw.close();
        }
    }
};


#endif
