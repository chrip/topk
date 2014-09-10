#ifndef TOPKQUEUENRD_H
#define TOPKQUEUENRD_H

#include "topkqueue.h"

class TopKQueueNRD : TopKQueue
{
public:
    TopKQueueNRD(size_t topK, size_t compressionBlockCols);
    void findTopK(const Vectorspace& vs, const QueryVector& qv, size_t startCol, size_t endCol, size_t level);
    void addVectorToLookUp(const int id, const vec& col);
private:
    std::vector<int> _columnIdLookup; // look up table for the "real" ids
    std::vector<int> _vectorLogSums; // look up table for the sums of each vector in the vector space.
    void findTopNRD(const Vectorspace &vs, const QueryVector &qv, size_t startCol, size_t endCol, size_t level, int queryLogSum, int logTotalNumberOfDocs);
};

#endif // TOPKQUEUENRD_H
