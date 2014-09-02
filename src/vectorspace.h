#ifndef VECTORSPACE_H
#define VECTORSPACE_H

#include "forward.h"
class QueryVector;

class Vectorspace
{
public:
    Vectorspace(size_t compressionBlockRows, size_t compressionBlockCols, size_t compressionLevels);
    ~Vectorspace();

    void addColumn(vec *col);
    const vecVecVec* getVectorSpacePyramide() const { return _vectorSpacePyramide; }
    const size_t getHighestCompressionCols() const {return _vectorSpacePyramide->at(_compressionLevels)->size(); }
    const size_t getCompressionLevels() const {return _compressionLevels; }
    float innerProduct(const QueryVector* queryVector, const size_t level, const size_t column) const;
    void buildPyramide();

private:

    float maxInBlock(const vecVec* matrix, int startCol, int startRow);
    vecVec* compressMatrix(const vecVec* matrix);


    size_t _numRows;
    size_t _numCols;
    size_t _compressionBlockRows;
    size_t _compressionBlockCols;
    size_t _compressionLevels;

    vecVecVec* _vectorSpacePyramide;
};

#endif // VECTORSPACE_H
