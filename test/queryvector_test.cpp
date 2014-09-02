#include "queryvector.h"
#include "gtest/gtest.h"


TEST(qvTest, ctor) {
    vec* col = new vec {1,2,3,4};
    size_t compressionBlockRows = 3;
    const size_t compressionLevels = 2;
    QueryVector* qv = new QueryVector(col, compressionBlockRows , compressionLevels);
    delete qv;
}

TEST(qvTest, pyramideSizeTest) {
    vec* col = new vec {1,2,3,4,5,6,7};
    size_t compressionBlockRows = 3;
    const size_t compressionLevels = 2;
    QueryVector* qv = new QueryVector(col, compressionBlockRows , compressionLevels);


    // the pyramide has 3 levels
    ASSERT_EQ(3, qv->getQueryPyramide()->size());

    // 7 rows @ level 0
    ASSERT_EQ(7, qv->getQueryPyramide()->at(0)->size());

    // ceil(7/3) = 3 rows @ level 1
    ASSERT_EQ(3, qv->getQueryPyramide()->at(1)->size());

    // ceil(ceil(7/3)/3) = 1  cols @ level 2
    ASSERT_EQ(1, qv->getQueryPyramide()->at(2)->size());

    delete qv;
}

TEST(qvTest, pyramideContentTest) {
    vec* col = new vec {1,2,3,4};
    size_t compressionBlockRows = 3;
    const size_t compressionLevels = 2;
    QueryVector* qv = new QueryVector(col, compressionBlockRows , compressionLevels);

    // the pyramide at level 0
    // 1
    // 2
    // 3
    // 4
    ASSERT_EQ(1, qv->getQueryPyramide()->at(0)->at(0));
    ASSERT_EQ(2, qv->getQueryPyramide()->at(0)->at(1));
    ASSERT_EQ(3, qv->getQueryPyramide()->at(0)->at(2));
    ASSERT_EQ(4, qv->getQueryPyramide()->at(0)->at(3));

    // the pyramide at level 1
    // 6
    // 4
    ASSERT_EQ(6, qv->getQueryPyramide()->at(1)->at(0));
    ASSERT_EQ(4, qv->getQueryPyramide()->at(1)->at(1));

    // the pyramide at level 2
    // 10
    ASSERT_EQ(10, qv->getQueryPyramide()->at(2)->at(0));

    delete qv;
}

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}

