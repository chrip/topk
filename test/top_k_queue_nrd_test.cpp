
#include "query_vector.h"
#include "top_k_queue_nrd.h"
#include "util.h"

#include <fstream>
#include "gtest/gtest.h"

void loadVectorSpace(TopKQueueNRD& tkqNRD, const std::string& pathToVectorSpaceJson) {
	//    std::ifstream input0( "/home/chrisschaefer/workspace/topk/job.json" );
	std::ifstream input0(pathToVectorSpaceJson);
	int i = 0;
	for (std::string line; getline(input0, line); i++){
		vec vector = vec();
		util::stringToVec(line, vector);
		tkqNRD.addVector(util::getJsonInt(line, "id"), vector);
	}
}

void tryOutParameters(bool sortVectorspace){
	size_t topK = 3;
	TopKQueueNRD tkqNRD = TopKQueueNRD();

	 //  loadVectorSpace(tkqNRD, "/home/chrisschaefer/workspace/topk/job.json"); 
	loadVectorSpace(tkqNRD, "test_data/vector_space.json");

	tkqNRD.buildIndex(1, 1, 0, sortVectorspace);
	std::cout << tkqNRD.getVectorSpaceSize() << " vectors of length " << tkqNRD.getVectorSpace().getVectorSpacePyramide()[0][0].size() << std::endl;

	std::ifstream input1("test_data/query_vector.json");
	for (std::string line; getline(input1, line);)
	{
		vec vector = vec();
		util::stringToVec(line, vector);

		size_t compressionLevels = 0;
		for (; compressionLevels < 4; compressionLevels++){
			size_t compressionBlockRows = 1;
			for (; compressionBlockRows < 10; compressionBlockRows++){
				size_t compressionBlockCols = 1;
				for (; compressionBlockCols < 3; compressionBlockCols++){

					int64_t start, end;
					tkqNRD.buildIndex(compressionBlockRows, compressionBlockCols, compressionLevels);

					start = util::getTimestampInMilliseconds();
					QueryVector qv = QueryVector(vector, compressionBlockRows, compressionLevels);
					tkqNRD.executeTopK(qv, topK);

					ASSERT_EQ(36100, util::getJsonInt(line, "id"));
					ASSERT_EQ("[[2572694,0.1704],[2570094,0.1659],[2558895,0.1657]]", tkqNRD.toString())
						//                    ASSERT_EQ("[[2548890,0.3311],[2553413,0.2964],[2571922,0.2955]]", tkqNRD.toString())
						<< " level: " << compressionLevels
						<< " compressionBlockCols: " << compressionBlockCols
						<< " compressionBlockRows: " << compressionBlockRows;
					end = util::getTimestampInMilliseconds();
					//std::cout << "compressionLevels: " << compressionLevels
					//	<< ", compressionBlockCols: " << compressionBlockCols
					//	<< ", compressionBlockRows: " << compressionBlockRows
					//	<< ", time: " << (end - start) << " ms" << std::endl;
				}
			}
		}
	}
}

TEST(tkqNrdTest, tryOutParametersUnsortedVectorspace) {
	bool sortVectorspace = false;
	tryOutParameters(sortVectorspace);
}

TEST(tkqNrdTest, tryOutParametersSortedVectorspace) {
	bool sortVectorspace = true;
	tryOutParameters(sortVectorspace);
}

typedef std::pair<std::string, int64_t> stringInt;

class StringIntComparison
{
public:
	bool operator() (const stringInt& lhs, const stringInt& rhs) const
	{
		return (lhs.second < rhs.second);
	}
};

typedef std::priority_queue<stringInt, std::vector<stringInt>, StringIntComparison> stringIntegerQueue;

void findBestParameter(bool sortVectorspace){
	size_t topK = 20;
	TopKQueueNRD tkqNRD = TopKQueueNRD();

	//   loadVectorSpace(tkqNRD, "../../job.json"); 
	   loadVectorSpace(tkqNRD, "test_data/vector_space.json");

	tkqNRD.buildIndex(1, 1, 0, sortVectorspace);
	std::cout << tkqNRD.getVectorSpaceSize() << " vectors of length " << tkqNRD.getVectorSpace().getVectorSpacePyramide()[0][0].size() << std::endl;

	stringIntegerQueue topKQueue = stringIntegerQueue();
	for (size_t i = 0; i < topK; i++){
		topKQueue.push({ "", 10000000 });
	}

	size_t compressionLevels = 0;
	for (; compressionLevels < 2; compressionLevels++){
		size_t compressionBlockRows = 1;
		for (; compressionBlockRows < 5; compressionBlockRows++){
			size_t compressionBlockCols = 1;
			for (; compressionBlockCols < 5; compressionBlockCols++){

				int64_t avgTime = 0;
				int64_t counter = 0;
				tkqNRD.buildIndex(compressionBlockRows, compressionBlockCols, compressionLevels);

				std::ifstream input1("test_data/more_query_vectors.json");
				for (std::string line; getline(input1, line); counter++)
				{
					if (counter == 2) break;
					vec vector = vec();
					util::stringToVec(line, vector);

					int64_t start, end;
					start = util::getTimestampInMilliseconds();
					QueryVector qv = QueryVector(vector, compressionBlockRows, compressionLevels);
					tkqNRD.executeTopK(qv, topK);
					end = util::getTimestampInMilliseconds();

					avgTime += end - start;
				}
				avgTime /= counter;
				std::cout << avgTime << std::endl;
				if (avgTime < topKQueue.top().second){
					std::string s = "compressionLevels: " + std::to_string(compressionLevels)
						+ ", compressionBlockCols: " + std::to_string(compressionBlockCols)
						+ ", compressionBlockRows: " + std::to_string(compressionBlockRows);
					topKQueue.pop();
					topKQueue.push({ s, avgTime});
				}
			}
		}
	}
	while (!topKQueue.empty()) {
		std::cout << topKQueue.top().first << "," << topKQueue.top().second << std::endl;
		topKQueue.pop();
	}
}

TEST(tkqNrdTest, findBestParametersUnsortedVectorspace) {
	bool sortVectorspace = false;
	findBestParameter(sortVectorspace);
}

TEST(tkqNrdTest, findBestParametersSortedVectorspace) {
	bool sortVectorspace = true;
	findBestParameter(sortVectorspace);
}

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}

