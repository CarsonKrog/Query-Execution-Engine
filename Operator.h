#ifndef OPERATOR_H
#define OPERATOR_H

#include <vector>
using namespace std;
#include "Record.h"

class Operator {
  public:
    virtual void open() = 0;

    virtual vector<Record> next() = 0;

    virtual void close() = 0;

  protected:
    int pageSize = 55;
};

#endif
