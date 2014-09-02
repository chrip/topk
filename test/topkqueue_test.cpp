#include "vectorspace.h"
#include "queryvector.h"
#include "topkqueue.h"

#include "gtest/gtest.h"


TEST(tkqTest, topk) {

    size_t compressionBlockCols = 3;
    size_t compressionBlockRows = 3;
    const size_t compressionLevels = 2;

    vec* query = new vec {1,2,3,4};
    QueryVector* qv = new QueryVector(query, compressionBlockRows , compressionLevels);
    Vectorspace* vs = new Vectorspace(compressionBlockRows, compressionBlockCols, compressionLevels);
    vec* col0 = new vec {1,1,1,1};
    vs->addColumn(col0);
    vec* col1 = new vec {1,2,1,1};
    vs->addColumn(col1);
    vec* col2 = new vec {0,0,0,1};
    vs->addColumn(col2);
    vec* col3 = new vec {1,0,1,0};
    vs->addColumn(col3);
    vs->buildPyramide();

    // col 0 at level 0
    // 1*1 + 2*1 + 3*1 + 4*1 = 1 + 2 + 3 + 4 = 10
    ASSERT_EQ(10, vs->innerProduct(qv, 0, 0));

    // col 1 at level 0
    // 1*1 + 2*2 + 3*1 + 4*1 = 1 + 4 + 3 + 4 = 12
    ASSERT_EQ(12, vs->innerProduct(qv, 0, 1));

    // col 2 at level 0
    // 1*0 + 2*0 + 3*0 + 4*1 = 0 + 0 + 0 + 4 = 4
    ASSERT_EQ(4, vs->innerProduct(qv, 0, 2));

    // col 3 at level 0
    // 1*1 + 2*0 + 3*1 + 4*0 = 1 + 0 + 3 + 4 = 4
    ASSERT_EQ(4, vs->innerProduct(qv, 0, 2));

    // col 0 at level 1
    // 6*2 + 4*1 = 12 + 4 = 16
    ASSERT_EQ(16, vs->innerProduct(qv, 1, 0));

    // col 1 at level 1
    // 6*1 + 4*0 = 6 + 0 = 6
    ASSERT_EQ(6, vs->innerProduct(qv, 1, 1));

    // col 0 at level 2
    // 10*2 = 20
    ASSERT_EQ(20, vs->innerProduct(qv, 2, 0));

    size_t topK = 2;
    TopKQueue topKQueue = TopKQueue(topK, compressionBlockCols);
    topKQueue.findTopK(vs,qv);
    ASSERT_EQ("[1 12.000000] [0 10.000000] ", topKQueue.toString());

    topK = 3;
    topKQueue = TopKQueue(topK, compressionBlockCols);
    topKQueue.findTopK(vs,qv);
    ASSERT_EQ("[1 12.000000] [0 10.000000] [2 4.000000] ", topKQueue.toString());

    delete vs;
    delete qv;
}

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}

