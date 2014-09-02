#include "queryvector.h"
#include "math.h"

QueryVector::QueryVector(vec *col, const size_t compressionBlockRows, const size_t compressionLevels)
    : _queryPyramide(new vecVec())
    , _compressionBlockRows(compressionBlockRows)
    , _compressionLevels(compressionLevels)
{
    this->buildPyramide(col);
}


void QueryVector::buildPyramide(vec* col){
    _queryPyramide->push_back(col);
    for(size_t level = 1; level <= _compressionLevels; level++){
        _queryPyramide->push_back(this->compressVector(_queryPyramide->at(level-1)));
    }
}


vec* QueryVector::compressVector(const vec* queryVec) {
    const int newRows = static_cast<int>(ceil(static_cast<float>(queryVec->size())/_compressionBlockRows));
    vec* compressedQuery = new vec();
    for(int r = 0; r < newRows; r++){
        compressedQuery->push_back(this->sum(queryVec, r*_compressionBlockRows));
    }
    return compressedQuery;
}

float QueryVector::sum(const vec* v, int startRow) {
    int endRow = std::min(startRow + _compressionBlockRows, v->size());
    float res = 0;
    for(int r = startRow; r < endRow; r++) {
        res += v->at(r);
    }
    return res;
}

QueryVector::~QueryVector(){
    for(size_t level = 0; level < _queryPyramide->size(); level++){
        _queryPyramide->at(level)->clear();
    }
    _queryPyramide->clear();
}
