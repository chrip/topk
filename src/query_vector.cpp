#include "query_vector.h"
#include "math.h"

QueryVector::QueryVector(const vec& col, const size_t compressionBlockRows, const size_t compressionLevels)
    : _queryPyramide(vecVec())
    , _compressionBlockRows(compressionBlockRows)
    , _compressionLevels(compressionLevels)
{
    this->buildPyramide(col);
}


void QueryVector::buildPyramide(const vec& col){
    _queryPyramide.push_back(col);
    for(size_t level = 1; level <= _compressionLevels; level++){
        this->compressVector(_queryPyramide[level-1]);
    }
}


void QueryVector::compressVector(const vec& queryVec) {
    const int newRows = static_cast<int>(ceil(static_cast<float>(queryVec.size())/_compressionBlockRows));
    vec compressedQuery = vec();
    for(int r = 0; r < newRows; r++){
        compressedQuery.push_back(this->sum(queryVec, r*_compressionBlockRows));
    }
    _queryPyramide.push_back(compressedQuery);
}

float QueryVector::sum(const vec& v, int startRow) {
    int endRow = std::min(startRow + _compressionBlockRows, v.size());
    float res = 0;
    for(int r = startRow; r < endRow; r++) {
        res += v[r];
    }
    return res;
}

QueryVector::~QueryVector(){
    for(size_t level = 0; level < _queryPyramide.size(); level++){
        _queryPyramide[level].clear();
    }
    _queryPyramide.clear();
}
