#include "forward.h"

#include "topkqueue.h"
#include "vectorspace.h"
#include "queryvector.h"

TopKQueue::TopKQueue(size_t topK, size_t compressionBlockCols):_topK(topK), _compressionBlockCols(compressionBlockCols), _topKQueue(intFloatQueue())
{
    for(size_t i = 0; i < topK; i++){
        _topKQueue.push({0,0});
    }
}

void TopKQueue::findTopK(const Vectorspace* vs, const QueryVector* qv){
  this->findTopK(vs, qv, 0, vs->getHighestCompressionCols(), vs->getCompressionLevels());
}

void TopKQueue::findTopK(const Vectorspace* vs, const QueryVector* qv, int startCol, int endCol, int level)
{
    for(int c = startCol; c < endCol; c++){
        float p = vs->innerProduct(qv, level, c);
        std::cout << "level:" << level << " col:" << c << " val:" << p << "\n";
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
