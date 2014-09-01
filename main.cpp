// g++ -std=c++11 main.cpp -o demo -lm -pthread

#include <iostream>
#include <queue>
#include <vector>
#include <utility>

class IntFloatComparison;

typedef std::pair<int,float> intFloat;
typedef std::priority_queue<intFloat, std::vector<intFloat>, IntFloatComparison> intFloatQueue;
typedef std::vector<float> vec;
typedef std::vector<vec*> vecVec;
typedef std::vector<vecVec*> vecVecVec;
static const int k = 3;
static const int vectorSpaceRows = 8;
static const int vectorSpaceCols = 6;
static const int compressionRows = 2;
static const int compressionCols = 3;
static const int compressionLevels = 2;

class IntFloatComparison
{
public:
  bool operator() (const intFloat& lhs, const intFloat& rhs) const
  {
    return (lhs.second > rhs.second);
  }
};

float innerProduct(const vec* lhs, const vec* rhs) {
    float res = 0;
    int rows = lhs->size();
    for(int r = 0; r < rows; r++) {
      res += lhs->at(r) * rhs->at(r);
    }
    return res;
}

float sum(const vec* v, int startRow) {
    int endRow = startRow + compressionRows;
    float res = 0;
    for(int r = startRow; r < endRow; r++) {
      res += v->at(r);
    }
    return res;
}

float maxInBlock(const vecVec* matrix, int startCol, int startRow) {
  int endCol = startCol + compressionCols;
  int endRow = startRow + compressionRows;
  float m = 0;
  for(int c = startCol; c < endCol; c++){
    for(int r = startRow; r < endRow; r++){
        m = std::max(m, matrix->at(c)->at(r));
    }
  }
  return m;
}

vecVec* compressMatrix(const vecVec* matrix) {

  const int newCols = matrix->size()/compressionCols;
  const int newRows = matrix->at(0)->size()/compressionRows;

  vecVec* compressedMatrix = new vecVec();
  for(int c = 0; c < newCols; c++){
    compressedMatrix->push_back(new vec());
    for(int r = 0; r < newRows; r++){
        compressedMatrix->at(c)->push_back(maxInBlock(matrix, c*compressionCols, r*compressionRows));
    }
  }
  return compressedMatrix;
}

vec* compressQuery(const vec* queryVec) {

  const int newRows = queryVec->size()/compressionRows;
  vec* compressedQuery = new vec();
  for(int r = 0; r < newRows; r++){
    compressedQuery->push_back(sum(queryVec, r*compressionRows));
  }
  return compressedQuery;
}

void findTopK(vecVecVec* vectorSpacePyramide, vecVec* queryPyramide, intFloatQueue& topKQueue, int startCol, int endCol, int level)
{
    for(int c = startCol; c < endCol; c++){
        float p = innerProduct(queryPyramide->at(level), vectorSpacePyramide->at(level)->at(c));
        std::cout << "level:" << level << " col:" << c << " val:" << p << "\n";
        if(p > topKQueue.top().second){
           if(level == 0){
               topKQueue.pop();
               topKQueue.push({c,p});
           }
           else {
               findTopK(vectorSpacePyramide, queryPyramide, topKQueue, c*compressionCols, c*compressionCols+compressionCols,  level-1);
           }
        }
    }
}

int main ()
{

  intFloatQueue topKQueue = intFloatQueue();
  for(int i = 0; i < k; i++){
      topKQueue.push({0,0});
  }
  
  vecVec* vectorSpace = new vecVec();
  vec* query = new vec();

  srand(42);
  for(int i = 0; i < vectorSpaceCols; i++){
    vectorSpace->push_back(new vec());
    for(int j = 0; j < vectorSpaceRows; j++){
      vectorSpace->at(i)->push_back(static_cast<float>(rand()) / static_cast<float>(RAND_MAX));
    }
  }
  for(int c = 0; c < vectorSpaceCols; c++) {
    for(int r = 0; r < vectorSpaceRows; r++){
      std::cout << vectorSpace->at(c)->at(r) << "\t";
    }
  std::cout << std::endl;
  }
  std::cout << std::endl;
  for(int r = 0; r < vectorSpaceRows; r++) {
    query->push_back(r);
    std::cout << query->at(r) << std::endl;
  }
  
  vecVec* queryPyramide = new vecVec();
  queryPyramide->push_back(query);
  for(int level = 1; level < compressionLevels; level++){
      queryPyramide->push_back(compressQuery(queryPyramide->at(level-1)));
  }

  vecVecVec* vectorSpacePyramide = new vecVecVec();
  vectorSpacePyramide->push_back(vectorSpace);
  for(int level = 1; level < compressionLevels; level++){
      vectorSpacePyramide->push_back(compressMatrix(vectorSpacePyramide->at(level-1)));
  }
  
  findTopK(vectorSpacePyramide, queryPyramide, topKQueue, 0, vectorSpacePyramide->at(compressionLevels-1)->size(), compressionLevels-1);

  std::cout << std::endl;
  while (!topKQueue.empty())
  {
     std::cout << " [" << topKQueue.top().first << ' ' << topKQueue.top().second << "] ";
     topKQueue.pop();
  }
  std::cout << '\n';
  for(int level = 0; level < compressionLevels; level++){
      for(int c = 0; c < vectorSpacePyramide->at(level)->size(); c++){
          vectorSpacePyramide->at(level)->at(c)->clear();
      }
      vectorSpacePyramide->at(level)->clear();
      queryPyramide->at(level)->clear();
  }
  vectorSpacePyramide->clear();
  queryPyramide->clear();
  return 0;
}
