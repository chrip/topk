#ifndef TOPKQUEUE_H
#define TOPKQUEUE_H

#include "forward.h"
#include <queue>

class Vectorspace;
class QueryVector;

typedef std::pair<int,float> intFloat;


class TopKQueue
{
public:

    TopKQueue();
    class IntFloatComparison
    {
    public:
        bool operator() (const intFloat& lhs, const intFloat& rhs) const
        {
            return (lhs.second > rhs.second);
        }
    };

    typedef std::priority_queue<intFloat, std::vector<intFloat>, IntFloatComparison> intFloatQueue;

    std::string toString();

    void findTopK(const Vectorspace &vs, const QueryVector &qv, size_t topK, size_t compressionBlockCols);

    virtual void addVectorToLookUp(const int id, const vec&);

protected:
    size_t _compressionBlockCols;
    intFloatQueue _topKQueue;
    std::vector<int> _columnIdLookup; // look up table for the "real" ids
    virtual void findTopK(const Vectorspace& vs, const QueryVector& qv, size_t startCol, size_t endCol, size_t level);
};


#endif // TOPKQUEUE_H
