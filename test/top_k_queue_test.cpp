#include "vector_space.h"
#include "query_vector.h"
#include "top_k_queue.h"

#include "gtest/gtest.h"

TEST(tkqTest, topk) {

    size_t compressionBlockCols = 3;
    size_t compressionBlockRows = 3;
    const size_t compressionLevels = 2;
    size_t topK = 2;
    vec query = vec {1,2,3,4};
    QueryVector qv = QueryVector(query, compressionBlockRows , compressionLevels);

    TopKQueue topKQueue = TopKQueue();

    vec col0 = vec {1,1,1,1};
    topKQueue.addVector(42,col0);
    vec col1 = vec {1,2,1,1};
    topKQueue.addVector(43,col1);
    vec col2 = vec {0,0,0,1};
    topKQueue.addVector(44,col2);
    vec col3 = vec {1,0,1,0};
    topKQueue.addVector(45,col3);

	// set sortVectorspace to false because we access certain columns of the vectorspace
    topKQueue.buildIndex(compressionBlockRows, compressionBlockCols, compressionLevels, false);

    // col 0 at level 0
    // 1*1 + 2*1 + 3*1 + 4*1 = 1 + 2 + 3 + 4 = 10
    ASSERT_EQ(10, topKQueue.getVectorSpace().innerProduct(qv, 0, 0));

    // col 1 at level 0
    // 1*1 + 2*2 + 3*1 + 4*1 = 1 + 4 + 3 + 4 = 12
    ASSERT_EQ(12, topKQueue.getVectorSpace().innerProduct(qv, 0, 1));

    // col 2 at level 0
    // 1*0 + 2*0 + 3*0 + 4*1 = 0 + 0 + 0 + 4 = 4
    ASSERT_EQ(4, topKQueue.getVectorSpace().innerProduct(qv, 0, 2));

    // col 3 at level 0
    // 1*1 + 2*0 + 3*1 + 4*0 = 1 + 0 + 3 + 4 = 4
    ASSERT_EQ(4, topKQueue.getVectorSpace().innerProduct(qv, 0, 2));

    // col 0 at level 1
    // 6*2 + 4*1 = 12 + 4 = 16
    ASSERT_EQ(16, topKQueue.getVectorSpace().innerProduct(qv, 1, 0));

    // col 1 at level 1
    // 6*1 + 4*0 = 6 + 0 = 6
    ASSERT_EQ(6, topKQueue.getVectorSpace().innerProduct(qv, 1, 1));

    // col 0 at level 2
    // 10*2 = 20
    ASSERT_EQ(20, topKQueue.getVectorSpace().innerProduct(qv, 2, 0));

    topKQueue.executeTopK(qv, topK);
    ASSERT_EQ("[[43,12.0000],[42,10.0000]]", topKQueue.toString());

    topK = 3;
    topKQueue.executeTopK(qv, topK);
    ASSERT_EQ("[[43,12.0000],[42,10.0000],[44,4.0000]]", topKQueue.toString());

}

TEST(tkqTest, big_topk) {

    size_t compressionBlockCols = 100;
    size_t compressionBlockRows = 10;
    const size_t compressionLevels = 2;

    size_t vectorSpaceCols = 21212;
    size_t vectorSpaceRows = 1000;

    vec query = vec();

    for(size_t j = 0; j < vectorSpaceRows; j++){
		query.push_back(j / static_cast<float>(vectorSpaceRows));
    }

    QueryVector qv = QueryVector(query, compressionBlockRows , compressionLevels);
    TopKQueue topKQueue = TopKQueue();
    for(size_t i = 0; i < vectorSpaceCols; i++){
        vec col = vec();
        for(size_t j = 0; j < vectorSpaceRows; j++){
			col.push_back(i + j / static_cast<float>(vectorSpaceRows + vectorSpaceCols));
        }
        topKQueue.addVector(i,col);
    }
    topKQueue.buildIndex(compressionBlockRows, compressionBlockCols, compressionLevels);

    size_t topK = 2;
    topKQueue.executeTopK(qv, topK);
    ASSERT_EQ("[[21211,10594911.0000],[21210,10594410.0000]]", topKQueue.toString());

    topK = 3;
    topKQueue.executeTopK(qv, topK);
    ASSERT_EQ("[[21211,10594911.0000],[21210,10594410.0000],[21209,10593910.0000]]", topKQueue.toString());

}

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}

