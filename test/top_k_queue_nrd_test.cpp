
#include "query_vector.h"
#include "top_k_queue_nrd.h"
#include "util.h"

#include <fstream>
#include "gtest/gtest.h"


TEST(tkqNrdTest, tryOutParameters) {

    size_t topK = 3;
    TopKQueueNRD tkqNRD = TopKQueueNRD();

//    std::ifstream input0( "/home/chrisschaefer/workspace/topk/job.json" );
    std::ifstream input0( "test_data/vector_space.json" );
    int i = 0;
    for( std::string line; getline( input0, line ); i++){
        vec vector = vec();
        util::stringToVec(line,vector);
        tkqNRD.addVector(util::getJsonInt(line, "id"),vector);
    }

	tkqNRD.buildIndex(1, 1, 0);
    std::cout << tkqNRD.getVectorSpaceSize() << " vectors of length " << tkqNRD.getVectorSpace().getVectorSpacePyramide()[0][0].size() << std::endl;

    std::ifstream input1( "test_data/query_vector.json" );
    for( std::string line; getline( input1, line );)
    {
        vec vector = vec();
        util::stringToVec(line, vector);

        size_t compressionLevels = 0;
        for(;compressionLevels < 10; compressionLevels++){
            size_t compressionBlockRows = 1;
            for(;compressionBlockRows < 10; compressionBlockRows++){
                size_t compressionBlockCols = 1;
                for(;compressionBlockCols < 3; compressionBlockCols++){

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
                    std::cout << "compressionLevels: " << compressionLevels
                              << ", compressionBlockCols: " << compressionBlockCols
                              << ", compressionBlockRows: " << compressionBlockRows
                              << ", time: "  <<  (end - start) << " ms" << std::endl;
                }
            }
        }
    }
}

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}

