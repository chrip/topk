// g++ -std=c++11 main.cpp -o demo -lm -pthread

#include <iostream>
#include <queue>
#include <vector>
#include <utility>

class IntFloatComparison;

typedef std::pair<int,float> intFloat;
typedef std::priority_queue<intFloat, std::vector<intFloat>, IntFloatComparison> intFloatQueue;

static const int k = 3;
static const int vectorSpaceRows = 8;
static const int vectorSpaceCols = 6;
static const int compressionRows = 1000;
static const int compressionCols = 1000;
static const int compressionLevels = 2;
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


static float compressMatrix(const float** matrix, int startRow, int startCol) {
  int endRow = startRow + compressionRows;
  int endCol = startCol + compressionCols;
  float norm = 0;
  for(int c = startCol; c < endCol; c++){
    for(int r = startRow; r < endRow; r++){
        compressedMatrix[c][r] = compressBlock(compressedMatrix[c][r], c*compressionCols, r*compressionRows);
    }
  }
};

static float** compressMatrix(const float** matrix, int matrixRows, int matrixCols, int level) {
  if (level == 0) {
    return matrix;
  }
  int newCols = matrixCols/compressionCols;
  int newRows = matrixRows/compressionRows;

  auto compressedMatrix = new float[newCols][newRows]();
  for(int c = 0; c < newCols; c++){
    for(int r = 0; r < newRows; r++){
        compressedMatrix[c][r] = compressBlock(compressedMatrix[c][r], c*compressionCols, r*compressionRows);
    }
  }

for i = 1 to height of D0 do
for j = 1 to width of D0 do
Let C be the submatrix in D associated with D0
i,j
D0
i,j = Lc
q,∞(C)
// Recursively Compress D0
return [D, Compress(D’, lvls -1, q, r, s)]
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
