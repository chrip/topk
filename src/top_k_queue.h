#ifndef TOPKQUEUE_H
#define TOPKQUEUE_H

#include "forward.h"
#include "vector_space.h"
#include <queue>
#include <unordered_map>

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
	virtual void buildIndex(size_t compressionBlockRows, size_t compressionBlockCols, size_t compressionLevels, bool sortVectorspace=true);
    void executeTopK(const QueryVector &qv, size_t topK);
    virtual void addVector(const int id, const vec& v);
    size_t getVectorSpaceSize() {return _vectorSpace.getVectorSpacePyramide()[0].size();}
    const Vectorspace& getVectorSpace() {return _vectorSpace;}
protected:

    size_t _compressionBlockCols;
    size_t _compressionBlockRows;

	std::unordered_map<int, const vec> _tempIdVectorspace;
	std::vector<intFloat> _idVecSums;

	Vectorspace _vectorSpace;
    intFloatQueue _topKQueue;
    std::vector<int> _columnIdLookup; // look up table for the "real" ids

    virtual void findTopK(const QueryVector& qv, size_t startCol, size_t endCol, size_t level);
};


#endif // TOPKQUEUE_H
