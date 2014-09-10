#include "topkqueuenrd.h"
#include "vectorspace.h"
#include "queryvector.h"

#include <numeric>
#include <math.h>

TopKQueueNRD::TopKQueueNRD(size_t topK, size_t compressionBlockCols)
    :TopKQueue(topK, compressionBlockCols)
    , _columnIdLookup()
    , _vectorLogSums()
{   
}
void TopKQueueNRD::findTopNRD(const Vectorspace& vs, const QueryVector& qv, size_t startCol, size_t endCol, size_t level, int queryLogSum, int logTotalNumberOfDocs) {
    size_t newEndCol = std::min(endCol, vs.getVectorSpacePyramide()[level].size());
    for(size_t c = startCol; c < newEndCol; c++){
        float p = vs.innerProduct(qv, level, c);

        // normalized relevance distance
        int logCommon = log(p);
        int maxlogSum = std::max(_vectorLogSums[c], queryLogSum);
        int minlogSum = std::min(_vectorLogSums[c], queryLogSum);
        float nrd = exp(-2 * (maxlogSum - logCommon) / (logTotalNumberOfDocs - minlogSum));
        if(nrd > _topKQueue.top().second){
            if(level == 0){
                _topKQueue.pop();
                _topKQueue.push({c,p});
            }
            else {
                findTopNRD(vs, qv, c*_compressionBlockCols, c*_compressionBlockCols+_compressionBlockCols,  level-1, queryLogSum, logTotalNumberOfDocs);
            }
        }
    }
}

void TopKQueueNRD::findTopK(const Vectorspace& vs, const QueryVector& qv, size_t startCol, size_t endCol, size_t level) {
    int queryLogSum = log(std::accumulate(qv.getQueryPyramide()[0].begin(), qv.getQueryPyramide()[0].end(), 0));
    int logTotalNumberOfDocs = vs.getVectorSpacePyramide()[0].size();
    this->findTopNRD(vs, qv, startCol, endCol, level, queryLogSum, logTotalNumberOfDocs);
}

void TopKQueueNRD::addVectorToLookUp(const int id, const vec &col)
{
    _columnIdLookup.push_back(id);
    _vectorLogSums.push_back(log(std::accumulate(col.begin(), col.end(), 0)));
}
