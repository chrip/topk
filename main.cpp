// g++ -std=c++11 main.cpp -o demo -lm -pthread

#include <iostream>
#include <queue>
#include <vector>
#include <utility>

class IntFloatComparison;

typedef std::pair<int,float> intFloat;
typedef std::priority_queue<intFloat, std::vector<intFloat>, IntFloatComparison> intFloatQueue;
typedef std::vector<float>* vecPtr;
typedef std::vector<vecPtr>* vecVecPtr;
typedef std::vector<vecVecPtr>* vecVecVecPtr;
static const int k = 3;
static const int vectorSpaceRows = 8;
static const int vectorSpaceCols = 6;
static const int compressionRows = 1000;
static const int compressionCols = 1000;
static const int compressionLevels = 2;

static vecVecVecPtr vectorSpacePyramide;

class IntFloatComparison
{
public:
  bool operator() (const intFloat& lhs, const intFloat& rhs) const
  {
    return (lhs.second > rhs.second);
  }
};

static float innerProduct(const float* lhs, const float* rhs) {
    float res = 0;
    for(int r = 0; r < vectorSpaceRows; r++) {
      res += lhs[r] * rhs[r];
    }
    return res;
};

static float maxInBlock(const vecVecPtr matrix, int startRow, int startCol) {
  int endRow = startRow + compressionRows;
  int endCol = startCol + compressionCols;
  float m = 0;
  for(int c = startCol; c < endCol; c++){
    for(int r = startRow; r < endRow; r++){
        m = std::max(m, matrix->at(c)->at(r));
    }
  }
  return m;
};

const vecVecPtr compressMatrix(const vecVecPtr matrix, int matrixRows, int matrixCols, int level) {
  if (level == 0) {
    return matrix;
  }
  const int newCols = matrixCols/compressionCols;
  const int newRows = matrixRows/compressionRows;

  vecVecPtr compressedMatrix = new std::vector<std::vector<float>*>();
  for(int c = 0; c < newCols; c++){
    for(int r = 0; r < newRows; r++){
        compressedMatrix->at(c)->at(r) = maxInBlock(matrix, c*compressionCols, r*compressionRows);
    }
  }
  return compressedMatrix;
}

int main ()
{

  intFloatQueue topKQueue = intFloatQueue();
  for(int i = 0; i < k; i++){
      topKQueue.push({0,0});
  }
  
  auto vectorSpace = new float[vectorSpaceCols][vectorSpaceRows]();
  float* query = new float[vectorSpaceRows];

  srand(42);
  for(int i = 0; i < vectorSpaceCols; i++){
    for(int j = 0; j < vectorSpaceRows; j++){
      vectorSpace[i][j] = static_cast<float>(rand()) / static_cast<float>(RAND_MAX);
    }
  }
  for(int c = 0; c < vectorSpaceCols; c++) {
    for(int r = 0; r < vectorSpaceRows; r++){
      std::cout << vectorSpace[c][r] << "\t";
    }
  std::cout << std::endl;
  }
  std::cout << std::endl;
  for(int r = 0; r < vectorSpaceRows; r++) {
    query[r] = r;
    std::cout << query[r] << std::endl;
  }
  
  for(int level = 0; level < compressionLevels; level++){
      
  }
  
  for(int c = 0; c < vectorSpaceCols; c++){
      float p = innerProduct(query, vectorSpace[c]);
      std::cout << topKQueue.top().second << "=" << p << "\t";
      if(p > topKQueue.top().second){
         topKQueue.pop();
         topKQueue.push({c,p});
      }
  }
  std::cout << std::endl;
  while (!topKQueue.empty())
  {
     std::cout << " [" << topKQueue.top().first << ' ' << topKQueue.top().second << "] ";
     topKQueue.pop();
  }
  std::cout << '\n';
  delete[] vectorSpace;
  delete[] query;
  return 0;
}
