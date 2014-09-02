#include "vectorspace.h"
#include "queryvector.h"

Vectorspace::Vectorspace(size_t compressionBlockRows, size_t compressionBlockCols, size_t compressionLevels)
 : _compressionBlockRows(compressionBlockRows)
 , _compressionBlockCols (compressionBlockCols)
 , _compressionLevels(compressionLevels)
 , _vectorSpacePyramide(new vecVecVec())
{
  _vectorSpacePyramide->push_back(new vecVec()); // add empty matrix for compression level 0
}

Vectorspace::~Vectorspace(){
    for(size_t level = 0; level < _vectorSpacePyramide->size(); level++){
        for(size_t c = 0; c < _vectorSpacePyramide->at(level)->size(); c++){
            _vectorSpacePyramide->at(level)->at(c)->clear();
        }
        _vectorSpacePyramide->at(level)->clear();
    }
    _vectorSpacePyramide->clear();
}

void Vectorspace::addColumn(vec* col) {
    // add origin column to the vector space at level 0
  _vectorSpacePyramide->at(0)->push_back(col);
}

float Vectorspace::innerProduct(const QueryVector *queryVector, const size_t level, const size_t column) const {
    float res = 0;
    vec* q = queryVector->getQueryPyramide()->at(level);
    vec* vs = _vectorSpacePyramide->at(level)->at(column);
    int rows = q->size();
    for(int r = 0; r < rows; r++) {
      res += q->at(r) * vs->at(r);
    }
    return res;
}

float Vectorspace::maxInBlock(const vecVec* matrix, int startCol, int startRow) {
  int endCol = std::min(startCol +  _compressionBlockCols, matrix->size());
  int endRow = std::min(startRow + _compressionBlockRows, matrix->at(0)->size());
  float m = 0;
  for(int c = startCol; c < endCol; c++){
    for(int r = startRow; r < endRow; r++){
        m = std::max(m, matrix->at(c)->at(r));
    }
  }
  return m;
}

vecVec* Vectorspace::compressMatrix(const vecVec* matrix) {

  const int newCols = matrix->size()/_compressionBlockCols;
  const int newRows = matrix->at(0)->size()/_compressionBlockRows;

  vecVec* compressedMatrix = new vecVec();
  for(int c = 0; c < newCols; c++){
    compressedMatrix->push_back(new vec());
    for(int r = 0; r < newRows; r++){
        compressedMatrix->at(c)->push_back(this->maxInBlock(matrix, c*_compressionBlockCols, r*_compressionBlockRows));
    }
  }
  return compressedMatrix;
}

void Vectorspace::buildPyramide(){
    for(size_t level = 1; level < _compressionLevels; level++){
        _vectorSpacePyramide->push_back(this->compressMatrix(_vectorSpacePyramide->at(level-1)));
    }
}
