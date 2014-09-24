#include "vector_space.h"
#include "query_vector.h"
#include "math.h"

Vectorspace::Vectorspace()
    : _vectorSpacePyramide(vecVecVec())
{
    _vectorSpacePyramide.push_back(vecVec()); // add empty matrix for compression level 0
}

void Vectorspace::clearPyramide(size_t fromLevel, size_t toLevel)
{
    for(size_t level = fromLevel; level < toLevel; level++){
        for(size_t c = 0; c < _vectorSpacePyramide[level].size(); c++){
            _vectorSpacePyramide[level][c].clear();
        }
        _vectorSpacePyramide[level].clear();
    }
    _vectorSpacePyramide.erase(_vectorSpacePyramide.begin()+fromLevel,_vectorSpacePyramide.begin()+toLevel);
}

Vectorspace::~Vectorspace(){
    this->clearPyramide(0, _vectorSpacePyramide.size());
    _vectorSpacePyramide.clear();
}

void Vectorspace::addColumn(const vec& col) {
    // add origin column to the vector space at level 0
    _vectorSpacePyramide[0].push_back(col);
}

float Vectorspace::innerProduct(const QueryVector& queryVector, const size_t level, const size_t column) const {
    float res = 0;
    const vec& q = queryVector.getQueryPyramide()[level];
    const vec& vs = _vectorSpacePyramide[level][column];
    size_t rows = q.size();
    for(size_t r = 0; r < rows; r++) {
        res += q[r] * vs[r];
    }
    return res;
}

float Vectorspace::maxInBlock(const vecVec& matrix, int startCol, int startRow) {
    int endCol = std::min(startCol + _compressionBlockCols, matrix.size());
    int endRow = std::min(startRow + _compressionBlockRows, matrix[0].size());
    float m = 0;
    for(int c = startCol; c < endCol; c++){
        for(int r = startRow; r < endRow; r++){
            m = std::max(m, matrix[c][r]);
        }
    }
    return m;
}

void Vectorspace::compressMatrix(const vecVec& matrix) {

    const int newCols = static_cast<int>(ceil(static_cast<float>(matrix.size())/_compressionBlockCols));
    const int newRows = static_cast<int>(ceil(static_cast<float>(matrix[0].size())/_compressionBlockRows));

    vecVec compressedMatrix = vecVec();
    for(int c = 0; c < newCols; c++){
        compressedMatrix.push_back(vec());
        for(int r = 0; r < newRows; r++){
            compressedMatrix[c].push_back(this->maxInBlock(matrix, c*_compressionBlockCols, r*_compressionBlockRows));
        }
    }
    _vectorSpacePyramide.push_back(compressedMatrix);
}

void Vectorspace::buildPyramide(size_t compressionBlockRows, size_t compressionBlockCols, size_t compressionLevels){
    _compressionBlockRows = compressionBlockRows;
    _compressionBlockCols = compressionBlockCols;
    _compressionLevels = compressionLevels;
    this->clearPyramide(1, _vectorSpacePyramide.size());
    for(size_t level = 1; level <= _compressionLevels; level++){
        this->compressMatrix(_vectorSpacePyramide[level-1]);
    }
}
