#include "vectorspace.h"
#include "queryvector.h"
#include "topkqueue.h"

#include "gtest/gtest.h"


TEST(tkqTest, topk) {

    size_t compressionBlockCols = 3;
    size_t compressionBlockRows = 3;
    const size_t compressionLevels = 2;

    vec query = vec {1,2,3,4};
    QueryVector qv = QueryVector(query, compressionBlockRows , compressionLevels);
    Vectorspace vs = Vectorspace(compressionBlockRows, compressionBlockCols, compressionLevels);
    vec col0 = vec {1,1,1,1};
    vs.addColumn(col0);
    vec col1 = vec {1,2,1,1};
    vs.addColumn(col1);
    vec col2 = vec {0,0,0,1};
    vs.addColumn(col2);
    vec col3 = vec {1,0,1,0};
    vs.addColumn(col3);
    vs.buildPyramide();

    // col 0 at level 0
    // 1*1 + 2*1 + 3*1 + 4*1 = 1 + 2 + 3 + 4 = 10
    ASSERT_EQ(10, vs.innerProduct(qv, 0, 0));

    // col 1 at level 0
    // 1*1 + 2*2 + 3*1 + 4*1 = 1 + 4 + 3 + 4 = 12
    ASSERT_EQ(12, vs.innerProduct(qv, 0, 1));

    // col 2 at level 0
    // 1*0 + 2*0 + 3*0 + 4*1 = 0 + 0 + 0 + 4 = 4
    ASSERT_EQ(4, vs.innerProduct(qv, 0, 2));

    // col 3 at level 0
    // 1*1 + 2*0 + 3*1 + 4*0 = 1 + 0 + 3 + 4 = 4
    ASSERT_EQ(4, vs.innerProduct(qv, 0, 2));

    // col 0 at level 1
    // 6*2 + 4*1 = 12 + 4 = 16
    ASSERT_EQ(16, vs.innerProduct(qv, 1, 0));

    // col 1 at level 1
    // 6*1 + 4*0 = 6 + 0 = 6
    ASSERT_EQ(6, vs.innerProduct(qv, 1, 1));

    // col 0 at level 2
    // 10*2 = 20
    ASSERT_EQ(20, vs.innerProduct(qv, 2, 0));

    size_t topK = 2;
    TopKQueue topKQueue = TopKQueue(topK, compressionBlockCols);
    topKQueue.findTopK(vs,qv);
    ASSERT_EQ("[1 12.000000] [0 10.000000] ", topKQueue.toString());

    topK = 3;
    topKQueue = TopKQueue(topK, compressionBlockCols);
    topKQueue.findTopK(vs,qv);
    ASSERT_EQ("[1 12.000000] [0 10.000000] [2 4.000000] ", topKQueue.toString());

}

TEST(tkqTest, big_topk) {

    size_t compressionBlockCols = 100;
    size_t compressionBlockRows = 10;
    const size_t compressionLevels = 2;

    size_t vectorSpaceCols = 21212;
    size_t vectorSpaceRows = 1000;

    vec query = vec();

    srand(42);
    for(size_t j = 0; j < vectorSpaceRows; j++){
        query.push_back(static_cast<float>(rand()) / static_cast<float>(RAND_MAX));
    }

    QueryVector qv = QueryVector(query, compressionBlockRows , compressionLevels);
    Vectorspace vs = Vectorspace(compressionBlockRows, compressionBlockCols, compressionLevels);
    for(size_t i = 0; i < vectorSpaceCols; i++){
        vec col = vec();
        for(size_t j = 0; j < vectorSpaceRows; j++){
            col.push_back(static_cast<float>(rand()) / static_cast<float>(RAND_MAX));
        }
        vs.addColumn(col);
    }
    vs.buildPyramide();

    size_t topK = 2;
    TopKQueue topKQueue = TopKQueue(topK, compressionBlockCols);
    topKQueue.findTopK(vs,qv);
    ASSERT_EQ("[1759 266.816772] [435 266.692963] ", topKQueue.toString());

    topK = 3;
    topKQueue = TopKQueue(topK, compressionBlockCols);
    topKQueue.findTopK(vs,qv);
    ASSERT_EQ("[1759 266.816772] [435 266.692963] [10805 265.252289] ", topKQueue.toString());

}

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}

