#include "top_k_queue_nrd.h"
#include "vector_space.h"
#include "query_vector.h"

#include <numeric>
#include <math.h>
#include <algorithm>

TopKQueueNRD::TopKQueueNRD()
    :TopKQueue()
    , _maxVectorLogSumPyramide()
    , _minVectorLogSumPyramide()
{   
    _maxVectorLogSumPyramide.push_back(vec());
    _minVectorLogSumPyramide.push_back(vec());
}
void TopKQueueNRD::findTopNRD(const QueryVector& qv, size_t startCol, size_t endCol, size_t level, float queryLogSum, float logTotalNumberOfDocs) {
    size_t newEndCol = std::min(endCol, _vectorSpace.getVectorSpacePyramide()[level].size());
    for(size_t c = startCol; c < newEndCol; c++){
        float p = _vectorSpace.innerProduct(qv, level, c);

        // normalized relevance distance
        float logCommon = log(p);
        float maxlogSum = std::max(_minVectorLogSumPyramide[level][c], queryLogSum);
        float minlogSum = std::min(_maxVectorLogSumPyramide[level][c], queryLogSum);
        float nrd = exp(-2 * (maxlogSum - logCommon) / (logTotalNumberOfDocs - minlogSum));
        //std::cout << "c: " << c << " logCommon: " << logCommon << " maxlogSum: " << maxlogSum << " minlogSum: " << minlogSum<< " p: " << p << " threshold: " << _topKQueue.top().second << " nrd: " << nrd << " level: " << level << std::endl;
        if(nrd > _topKQueue.top().second){
            if(level == 0){
                _topKQueue.pop();
                _topKQueue.push({c, nrd});
                //std::cout << "push({"<<c<<", "<<nrd<<"})"<< std::endl;
            }
            else {
                findTopNRD(qv, c*_compressionBlockCols, c*_compressionBlockCols+_compressionBlockCols,  level-1, queryLogSum, logTotalNumberOfDocs);
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
}

void TopKQueueNRD::addVector(const int id, const vec &col)
{
    TopKQueue::addVector(id, col);
    float logSumLevel0 = log(std::accumulate(col.begin(), col.end(), 0.0f));
    // at level 0 min == max
    _maxVectorLogSumPyramide[0].push_back(logSumLevel0);
    _minVectorLogSumPyramide[0].push_back(logSumLevel0);
}


void TopKQueueNRD::findTopK(const QueryVector& qv, size_t startCol, size_t endCol, size_t level) {

    float queryLogSum = log(std::accumulate(qv.getQueryPyramide()[0].begin(), qv.getQueryPyramide()[0].end(), 0.0f));
    float logTotalNumberOfDocs = log(_vectorSpace.getVectorSpacePyramide()[0][0].size());
    this->findTopNRD(qv, startCol, endCol, level, queryLogSum, logTotalNumberOfDocs);
}


void TopKQueueNRD::sumMinMaxAtLevel(size_t level){
    const int newRows = static_cast<int>(ceil(static_cast<float>(_maxVectorLogSumPyramide[level].size())/_compressionBlockCols));

    vec compressedMaxs = vec();
    vec compressedMins = vec();

    for(int r = 0; r < newRows; r++){

        size_t start = r *_compressionBlockCols;
        size_t end = std::min(start + _compressionBlockCols, _maxVectorLogSumPyramide[level].size());

        auto minEl = std::min_element(_minVectorLogSumPyramide[level].begin() + start, _minVectorLogSumPyramide[level].begin() + end);
        auto maxEl = std::max_element(_maxVectorLogSumPyramide[level].begin() + start, _maxVectorLogSumPyramide[level].begin() + end);

        compressedMins.push_back(*minEl);
        compressedMaxs.push_back(*maxEl);

    }
    _maxVectorLogSumPyramide.push_back(compressedMaxs);
    _minVectorLogSumPyramide.push_back(compressedMins);
}


void TopKQueueNRD::clearIndex()
{
    _maxVectorLogSumPyramide.erase(_maxVectorLogSumPyramide.begin()+1,_maxVectorLogSumPyramide.end());
    _minVectorLogSumPyramide.erase(_minVectorLogSumPyramide.begin()+1,_minVectorLogSumPyramide.end());
}
