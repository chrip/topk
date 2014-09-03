#ifndef QUERYVECTOR_H
#define QUERYVECTOR_H

#include "forward.h"

class QueryVector
{
public:
    QueryVector(const vec& col, const size_t compressionBlockRows, const size_t compressionLevels);
    ~QueryVector();
    const vecVec& getQueryPyramide() const { return _queryPyramide; }
private:
    void buildPyramide(const vec& col);
    void compressVector(const vec& queryVec);
    float sum(const vec& v, int startRow);
    vecVec _queryPyramide;
    size_t _compressionBlockRows;
    size_t _compressionLevels;

};

#endif // QUERYVECTOR_H
