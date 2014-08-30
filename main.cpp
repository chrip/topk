// g++ -std=c++11 main.cpp -o demo -lm -pthread

#include <iostream>
#include <queue>
#include <vector>
#include <utility>

class IntFloatComparison;

typedef std::pair<int,float> intFloat;
typedef std::priority_queue<intFloat, std::vector<intFloat>, IntFloatComparison> intFloatQueue;

static const int k = 2;
static const int vectorSpaceRows = 8;
static const int vectorSpaceCols = 6; 
class IntFloatComparison
{
public:
  bool operator() (const intFloat& lhs, const intFloat& rhs) const
  {
    return (lhs.second<rhs.second);
  }
};

static float innerProduct(const float* lhs, const float* rhs) {
    float res = 0;
    for(int r = 0; r < vectorSpaceRows; r++) {
      res += lhs[r] * rhs[r];
    }
    return res;
};

int main ()
{

  intFloatQueue topKQueue = intFloatQueue();
  for(int i = 0; i < k; i++){
      topKQueue.push({0,0});
  }
  
  auto vectorSpace = new float[vectorSpaceCols][vectorSpaceRows]();
  float* query = new float[vectorSpaceRows];

  for(int i = 0; i < vectorSpaceCols; i++){
    for(int j = 0; j < vectorSpaceRows; j++){
      vectorSpace[i][j] = i+j;
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
      std::cout << p << "\t";
      if(p > topKQueue.top().second){
         topKQueue.pop();
         topKQueue.push({c,p});
      }
  }
  std::cout << std::endl;
  while (!topKQueue.empty())
  {
     std::cout << ' ' << topKQueue.top().first;
     topKQueue.pop();
  }
  std::cout << '\n';
  delete[] vectorSpace;
  delete[] query;
  return 0;
}
