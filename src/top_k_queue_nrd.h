#ifndef TOPKQUEUENRD_H
#define TOPKQUEUENRD_H

#include "top_k_queue.h"

class TopKQueueNRD : public TopKQueue
{
public:
    TopKQueueNRD();

    void addVector(const int id, const vec &col);
	void buildIndex(size_t compressionBlockRows, size_t compressionBlockCols, size_t compressionLevels, bool sortVectorspace = true) override;

    float normalizedRelevanceDistance(const QueryVector& qv, float queryLogSum, size_t c, size_t level);
protected:

    void findTopK(const QueryVector& qv, size_t startCol, size_t endCol, size_t level) override;
    void findTopNRD(const QueryVector &qv, size_t startCol, size_t endCol, size_t level, float queryLogSum);

private:
    void clearIndex();
    void sumMinMaxAtLevel(size_t level);
    vecVec _minVectorLogSumPyramide;// look up table for the sums of each vector in the vector space.
    float _logTotalNumberOfDocs;
};

#endif // TOPKQUEUENRD_H
