#ifndef TOPKQUEUENRD_H
#define TOPKQUEUENRD_H

#include "topkqueue.h"

class TopKQueueNRD : public TopKQueue
{
public:
    TopKQueueNRD();

    void addVectorToLookUp(const int id, const vec &col);
protected:
    std::vector<float> _vectorLogSums; // look up table for the sums of each vector in the vector space.
    void findTopK(const Vectorspace& vs, const QueryVector& qv, size_t startCol, size_t endCol, size_t level) override;
    void findTopNRD(const Vectorspace &vs, const QueryVector &qv, size_t startCol, size_t endCol, size_t level, float queryLogSum, float logTotalNumberOfDocs);
};

#endif // TOPKQUEUENRD_H
