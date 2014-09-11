#include "topkqueuenrd.h"
#include "vectorspace.h"
#include "queryvector.h"

#include <numeric>
#include <math.h>

TopKQueueNRD::TopKQueueNRD()
    :TopKQueue()
    , _vectorLogSums()
{   
}
void TopKQueueNRD::findTopNRD(const Vectorspace& vs, const QueryVector& qv, size_t startCol, size_t endCol, size_t level, float queryLogSum, float logTotalNumberOfDocs) {
    size_t newEndCol = std::min(endCol, vs.getVectorSpacePyramide()[level].size());
    for(size_t c = startCol; c < newEndCol; c++){
        float p = vs.innerProduct(qv, level, c);

        // normalized relevance distance
        float logCommon = log(p);
        float maxlogSum = std::max(_vectorLogSums[c], queryLogSum);
        float minlogSum = std::min(_vectorLogSums[c], queryLogSum);
        float nrd = exp(-2 * (maxlogSum - logCommon) / (logTotalNumberOfDocs - minlogSum));

        if(nrd > _topKQueue.top().second){
            if(level == 0){
                _topKQueue.pop();
                _topKQueue.push({c, nrd});
            }
            else {
                findTopNRD(vs, qv, c*_compressionBlockCols, c*_compressionBlockCols+_compressionBlockCols,  level-1, queryLogSum, logTotalNumberOfDocs);
            }
        }
    }
}

void TopKQueueNRD::addVectorToLookUp(const int id, const vec &col)
{
    _columnIdLookup.push_back(id);
    _vectorLogSums.push_back(log(std::accumulate(col.begin(), col.end(), 0.0f)));
}


void TopKQueueNRD::findTopK(const Vectorspace& vs, const QueryVector& qv, size_t startCol, size_t endCol, size_t level) {
    float sum = 0;
    for(float f :qv.getQueryPyramide()[0]){
        sum+=f;
    }

    float queryLogSum = log(std::accumulate(qv.getQueryPyramide()[0].begin(), qv.getQueryPyramide()[0].end(), 0.0f));
    //float logTotalNumberOfDocs = log(vs.getVectorSpacePyramide()[0].size());
    float logTotalNumberOfDocs = log(2000);
    this->findTopNRD(vs, qv, startCol, endCol, level, queryLogSum, logTotalNumberOfDocs);
}
