#ifndef TOPKQUEUENRD_H
#define TOPKQUEUENRD_H

#include "topkqueue.h"
#include <unordered_map>

class TopKQueueNRD : TopKQueue
{
public:
    TopKQueueNRD(size_t topK, size_t compressionBlockCols);
    void findTopK(const Vectorspace& vs, const QueryVector& qv, size_t startCol, size_t endCol, size_t level);

private:
    std::unordered_map<size_t,std::tuple<int,int>> _columnIdLookup;
};

#endif // TOPKQUEUENRD_H
