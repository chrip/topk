#include "topkqueuenrd.h"
#include "vectorspace.h"
#include "queryvector.h"

TopKQueueNRD::TopKQueueNRD(size_t topK, size_t compressionBlockCols)
    :TopKQueue(topK, compressionBlockCols)
    , _columnIdLookup()
{   
}

void TopKQueueNRD::findTopK(const Vectorspace& vs, const QueryVector& qv, size_t startCol, size_t endCol, size_t level) {
    size_t newEndCol = std::min(endCol, vs.getVectorSpacePyramide()[level].size());
    for(size_t c = startCol; c < newEndCol; c++){
        float p = vs.innerProduct(qv, level, c);
        if(p > _topKQueue.top().second){
            if(level == 0){
                _topKQueue.pop();
                _topKQueue.push({c,p});
            }
            else {
                findTopK(vs, qv, c*_compressionBlockCols, c*_compressionBlockCols+_compressionBlockCols,  level-1);
            }
        }
    }
}
