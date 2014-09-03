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
    class IntFloatComparison
    {
    public:
        bool operator() (const intFloat& lhs, const intFloat& rhs) const
        {
            return (lhs.second > rhs.second);
        }
    };

    typedef std::priority_queue<intFloat, std::vector<intFloat>, IntFloatComparison> intFloatQueue;

    TopKQueue(size_t topK, size_t compressionBlockCols);
    void findTopK(const Vectorspace& vs, const QueryVector& qv);
    std::string toString();

private:
    size_t _topK;
    size_t _compressionBlockCols;
    intFloatQueue _topKQueue;
    void findTopK(const Vectorspace& vs, const QueryVector& qv, size_t startCol, size_t endCol, size_t level);

};


#endif // TOPKQUEUE_H
