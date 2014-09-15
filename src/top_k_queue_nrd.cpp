#include "top_k_queue_nrd.h"
#include "vector_space.h"
#include "query_vector.h"

#include <numeric>
#include <math.h>
#include <algorithm>

TopKQueueNRD::TopKQueueNRD()
    :TopKQueue()
    , _minVectorLogSumPyramide()
{   
    _minVectorLogSumPyramide.push_back(vec());
}
float TopKQueueNRD::normalizedRelevanceDistance(const QueryVector& qv, float queryLogSum, size_t column, size_t level)
{
    float p = _vectorSpace.innerProduct(qv, level, column);
    float logCommon = log(p);
    float minlogSum = std::min(_minVectorLogSumPyramide[level][column], queryLogSum);
    float maxlogSum = std::max(_minVectorLogSumPyramide[level][column], queryLogSum);
    float nrd = exp(-2 * (maxlogSum - logCommon) / (_logTotalNumberOfDocs - minlogSum));

    return nrd;
}

void TopKQueueNRD::findTopNRD(const QueryVector& qv, size_t startCol, size_t endCol, size_t level, float queryLogSum) {
    size_t newEndCol = std::min(endCol, _vectorSpace.getVectorSpacePyramide()[level].size());
    for(size_t c = startCol; c < newEndCol; c++){
        float nrd = normalizedRelevanceDistance(qv, queryLogSum, c, level);
        if(nrd > _topKQueue.top().second){
            if(level == 0){
                _topKQueue.pop();
                _topKQueue.push({c, nrd});
            }
            else {
               findTopNRD(qv, c*_compressionBlockCols, c*_compressionBlockCols+_compressionBlockCols,  level-1, queryLogSum);
            }
        }
    }
}

void TopKQueueNRD::buildIndex(size_t compressionBlockRows, size_t compressionBlockCols, size_t compressionLevels)
{
    TopKQueue::buildIndex(compressionBlockRows, compressionBlockCols, compressionLevels);
    this->clearIndex();
    for(size_t level = 1; level <= compressionLevels; level++){
        this->sumMinMaxAtLevel(level-1);
    }
    _logTotalNumberOfDocs = log(_vectorSpace.getVectorSpacePyramide()[0][0].size());
}

void TopKQueueNRD::addVector(const int id, const vec &col)
{
    TopKQueue::addVector(id, col);
    float logSumLevel0 = log(std::accumulate(col.begin(), col.end(), 0.0f));
    // at level 0 min == max
    _minVectorLogSumPyramide[0].push_back(logSumLevel0);
}


void TopKQueueNRD::findTopK(const QueryVector& qv, size_t startCol, size_t endCol, size_t level) {

    float queryLogSum = log(std::accumulate(qv.getQueryPyramide()[0].begin(), qv.getQueryPyramide()[0].end(), 0.0f));
    this->findTopNRD(qv, startCol, endCol, level, queryLogSum);
}


void TopKQueueNRD::sumMinMaxAtLevel(size_t level){
    const int newRows = static_cast<int>(ceil(static_cast<float>(_minVectorLogSumPyramide[level].size())/_compressionBlockCols));
    vec compressedMins = vec();

    for(int r = 0; r < newRows; r++){
        size_t start = r *_compressionBlockCols;
        size_t end = std::min(start + _compressionBlockCols, _minVectorLogSumPyramide[level].size());

        auto minEl = std::min_element(_minVectorLogSumPyramide[level].begin() + start, _minVectorLogSumPyramide[level].begin() + end);
        compressedMins.push_back(*minEl);

    }
    _minVectorLogSumPyramide.push_back(compressedMins);
}


void TopKQueueNRD::clearIndex()
{
    _minVectorLogSumPyramide.erase(_minVectorLogSumPyramide.begin()+1,_minVectorLogSumPyramide.end());
}
