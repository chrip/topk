#include "top_k_queue.h"
#include "query_vector.h"

#include <sstream>
#include <iomanip>

void TopKQueue::executeTopK(const QueryVector& qv, size_t topK) {
    _topKQueue = intFloatQueue();
    for(size_t i = 0; i < topK; i++){
        _topKQueue.push({0,0});
    }
    this->findTopK(qv, 0, _vectorSpace.getHighestCompressionCols(), _vectorSpace.getCompressionLevels());
}

void TopKQueue::addVector(const int id, const vec& v) {
	_tempIdVectorspace.insert({ id, v });
	_tempIdValues.push_back(std::make_pair(id, std::accumulate(v.begin(), v.end(), 0.0f))); // other possibility is to count zeros...
}

void TopKQueue::findTopK(const QueryVector& qv, size_t startCol, size_t endCol, size_t level) {
    size_t newEndCol = std::min(endCol, _vectorSpace.getVectorSpacePyramide()[level].size());
    for(size_t c = startCol; c < newEndCol; c++){
        float p = _vectorSpace.innerProduct(qv, level, c);
        if(p > _topKQueue.top().second){
            if(level == 0){
                _topKQueue.pop();
                _topKQueue.push({c,p});
            }
            else {
                this->findTopK(qv, c*_compressionBlockCols, c*_compressionBlockCols+_compressionBlockCols,  level-1);
            }
        }
    }
}

TopKQueue::TopKQueue() :_tempIdVectorspace(), _tempIdValues(), _vectorSpace(), _columnIdLookup()
{
}

std::string TopKQueue::toString() {
    std::string result;
    while (!_topKQueue.empty()) {
        std::ostringstream item;
        item << "[" << std::to_string(_columnIdLookup[_topKQueue.top().first])
             << ","<< std::fixed << std::setprecision(4) << _topKQueue.top().second << "],";
        result = item.str() + result;
        _topKQueue.pop();
    }
    return "[" + result.substr(0,result.size()-1) + "]";
}

void TopKQueue::buildIndex(size_t compressionBlockRows, size_t compressionBlockCols, size_t compressionLevels, bool sortVectorspace)
{
    _compressionBlockCols = compressionBlockCols;

	if (_tempIdValues.size()){
		if (sortVectorspace) {
			// sort vector space for performance gain
			std::sort(_tempIdValues.begin(), _tempIdValues.end(), IntFloatComparison());
		}
		for (auto idVal : _tempIdValues) {
			_columnIdLookup.push_back(idVal.first);
			_vectorSpace.addColumn(_tempIdVectorspace[idVal.first]);
			_tempIdVectorspace.erase(idVal.first);
		}
		_tempIdValues.clear();
		_tempIdVectorspace.clear();
	}
    _vectorSpace.buildPyramide(compressionBlockRows,compressionBlockCols,compressionLevels);
}
