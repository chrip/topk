#include "topkqueue.h"
#include "vectorspace.h"
#include "queryvector.h"

TopKQueue::TopKQueue(size_t topK, size_t compressionBlockCols):_topK(topK), _compressionBlockCols(compressionBlockCols), _topKQueue(intFloatQueue()) {
    for(size_t i = 0; i < topK; i++){
        _topKQueue.push({0,0});
    }
}

void TopKQueue::findTopK(const Vectorspace* vs, const QueryVector* qv) {
    this->findTopK(vs, qv, 0, vs->getHighestCompressionCols(), vs->getCompressionLevels());
}

void TopKQueue::findTopK(const Vectorspace* vs, const QueryVector* qv, size_t startCol, size_t endCol, size_t level) {
    size_t newEndCol = std::min(endCol, vs->getVectorSpacePyramide()->at(level)->size());
    for(size_t c = startCol; c < newEndCol; c++){
        float p = vs->innerProduct(qv, level, c);
        //std::cout << "level:" << level << " col:" << c << " val:" << p << "\n";
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

std::string TopKQueue::toString() {
    std::string s = "";
    while (!_topKQueue.empty()) {
        std::string item = "[" + std::to_string(_topKQueue.top().first) + " " + std::to_string(_topKQueue.top().second) + "] ";
        s = item + s;
        _topKQueue.pop();
    }
    return s;
}
