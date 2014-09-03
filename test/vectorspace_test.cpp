#include "vectorspace.h"
#include "queryvector.h"
#include "gtest/gtest.h"


TEST(vsTest, addCol) {
    size_t compressionBlockRows = 4;
    size_t compressionBlockCols = 3;
    size_t compressionLevels = 2;
    Vectorspace vs = Vectorspace(compressionBlockRows, compressionBlockCols, compressionLevels);

    size_t vectorSpaceCols = 8;
    size_t vectorSpaceRows = 11;

    for(size_t i = 0; i < vectorSpaceCols; i++){
        vec col = vec();
        for(size_t j = 0; j < vectorSpaceRows; j++){
            col.push_back(1.0);
        }
        vs.addColumn(col);
    }

    // the pyramide has only one level
    ASSERT_EQ(1, vs.getVectorSpacePyramide().size());
    // 8 cols
    ASSERT_EQ(vectorSpaceCols, vs.getVectorSpacePyramide().at(0).size());
    // 11 rows
    ASSERT_EQ(vectorSpaceRows, vs.getVectorSpacePyramide().at(0).at(0).size());
}

TEST(vsTest, pyramideSizeTest) {
    size_t compressionBlockRows = 4;
    size_t compressionBlockCols = 3;
    size_t compressionLevels = 2;
    Vectorspace vs = Vectorspace(compressionBlockRows, compressionBlockCols, compressionLevels);

    size_t vectorSpaceCols = 12;
    size_t vectorSpaceRows = 11;

    for(size_t i = 0; i < vectorSpaceCols; i++){
        vec col = vec();
        for(size_t j = 0; j < vectorSpaceRows; j++){
            col.push_back(j);
        }
        vs.addColumn(col);
    }
    vs.buildPyramide();

    // the pyramide has 3 levels
    ASSERT_EQ(3, vs.getVectorSpacePyramide().size());

    // 12 cols @ level 0
    ASSERT_EQ(12, vs.getVectorSpacePyramide().at(0).size());

    // 11 rows @ level 0
    ASSERT_EQ(11, vs.getVectorSpacePyramide().at(0).at(0).size());

    // ceil(12/3) = 4  cols @ level 1
    ASSERT_EQ(4, vs.getVectorSpacePyramide().at(1).size());

    // ceil(11/4) = 3 rows @ level 1
    ASSERT_EQ(3, vs.getVectorSpacePyramide().at(1).at(0).size());

    // ceil(ceil(12/3)/3) = 2  cols @ level 2
    ASSERT_EQ(2, vs.getVectorSpacePyramide().at(2).size());

    // 2 cols at highest level
    ASSERT_EQ(2, vs.getHighestCompressionCols());

    // ceil(ceil(11/4)/4) = 1 rows @ level 2
    ASSERT_EQ(1, vs.getVectorSpacePyramide().at(2).at(0).size());

}

TEST(vsTest, pyramideContentTest) {
    size_t compressionBlockRows = 2;
    size_t compressionBlockCols = 3;
    size_t compressionLevels = 2;
    Vectorspace vs = Vectorspace(compressionBlockRows, compressionBlockCols, compressionLevels);


    vec col0 = vec {1,2,1,1,1};
    vs.addColumn(col0);
    vec col1 = vec {1,1,1,1,1};
    vs.addColumn(col1);

    vs.buildPyramide();

    // level 1:
    // 1,1
    // 2,1
    // 1,1
    // 1,1
    // 1,1
    // 1,1
    ASSERT_EQ(1, vs.getVectorSpacePyramide().at(0).at(0).at(0));
    ASSERT_EQ(2, vs.getVectorSpacePyramide().at(0).at(0).at(1));
    ASSERT_EQ(1, vs.getVectorSpacePyramide().at(0).at(0).at(2));
    ASSERT_EQ(1, vs.getVectorSpacePyramide().at(0).at(0).at(3));
    ASSERT_EQ(1, vs.getVectorSpacePyramide().at(0).at(0).at(4));

    ASSERT_EQ(1, vs.getVectorSpacePyramide().at(0).at(1).at(0));
    ASSERT_EQ(1, vs.getVectorSpacePyramide().at(0).at(1).at(1));
    ASSERT_EQ(1, vs.getVectorSpacePyramide().at(0).at(1).at(2));
    ASSERT_EQ(1, vs.getVectorSpacePyramide().at(0).at(1).at(3));
    ASSERT_EQ(1, vs.getVectorSpacePyramide().at(0).at(1).at(4));

    // level 2:
    // 2
    // 1
    // 1
    ASSERT_EQ(2, vs.getVectorSpacePyramide().at(1).at(0).at(0));
    ASSERT_EQ(1, vs.getVectorSpacePyramide().at(1).at(0).at(1));
    ASSERT_EQ(1, vs.getVectorSpacePyramide().at(1).at(0).at(2));

    // level 2:
    // 2
    // 1
    ASSERT_EQ(2, vs.getVectorSpacePyramide().at(2).at(0).at(0));
    ASSERT_EQ(1, vs.getVectorSpacePyramide().at(2).at(0).at(1));

}

TEST(vsTest, innerProductTest) {

    size_t compressionBlockCols = 3;
    size_t compressionBlockRows = 3;
    const size_t compressionLevels = 2;

    vec query = vec {1,2,3,4};
    QueryVector qv = QueryVector(query, compressionBlockRows , compressionLevels);
    Vectorspace vs = Vectorspace(compressionBlockRows, compressionBlockCols, compressionLevels);
    vec col0 = vec {1,2,1,1};
    vs.addColumn(col0);
    vec col1 = vec {1,1,1,1};
    vs.addColumn(col1);
    vs.buildPyramide();

    // col 0 at level 0
    // 1*1 + 2*2 + 3*1 + 4*1 = 1 + 4 + 3 + 4 = 12
    ASSERT_EQ(12, vs.innerProduct(qv, 0, 0));

    // col 1 at level 0
    // 1*1 + 2*1 + 3*1 + 4*1 = 1 + 2 + 3 + 4 = 10
    ASSERT_EQ(10, vs.innerProduct(qv, 0, 1));

    // col 0 at level 1
    // 6*2 + 4*1 = 12 + 4 = 16
    ASSERT_EQ(16, vs.innerProduct(qv, 1, 0));

    // col 0 at level 2
    // 10*2 = 20
    ASSERT_EQ(20, vs.innerProduct(qv, 2, 0));

}

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}

