#include "vector_space.h"
#include "query_vector.h"
#include "gtest/gtest.h"


TEST(vsTest, addCol) {
    Vectorspace vs = Vectorspace();

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
    ASSERT_EQ(vectorSpaceCols, vs.getVectorSpacePyramide()[0].size());
    // 11 rows
    ASSERT_EQ(vectorSpaceRows, vs.getVectorSpacePyramide()[0][0].size());
}

TEST(vsTest, pyramideSizeTest) {
    size_t compressionBlockRows = 4;
    size_t compressionBlockCols = 3;
    size_t compressionLevels = 2;
    Vectorspace vs = Vectorspace();

    size_t vectorSpaceCols = 12;
    size_t vectorSpaceRows = 11;

    for(size_t i = 0; i < vectorSpaceCols; i++){
        vec col = vec();
        for(size_t j = 0; j < vectorSpaceRows; j++){
            col.push_back(j);
        }
        vs.addColumn(col);
    }
    vs.buildPyramide(compressionBlockRows, compressionBlockCols, compressionLevels);

    // the pyramide has 3 levels
    ASSERT_EQ(3, vs.getVectorSpacePyramide().size());

    // 12 cols @ level 0
    ASSERT_EQ(12, vs.getVectorSpacePyramide()[0].size());

    // 11 rows @ level 0
    ASSERT_EQ(11, vs.getVectorSpacePyramide()[0][0].size());

    // ceil(12/3) = 4  cols @ level 1
    ASSERT_EQ(4, vs.getVectorSpacePyramide()[1].size());

    // ceil(11/4) = 3 rows @ level 1
    ASSERT_EQ(3, vs.getVectorSpacePyramide()[1][0].size());

    // ceil(ceil(12/3)/3) = 2  cols @ level 2
    ASSERT_EQ(2, vs.getVectorSpacePyramide()[2].size());

    // 2 cols at highest level
    ASSERT_EQ(2, vs.getHighestCompressionCols());

    // ceil(ceil(11/4)/4) = 1 rows @ level 2
    ASSERT_EQ(1, vs.getVectorSpacePyramide()[2][0].size());

}

TEST(vsTest, pyramideContentTest) {
    size_t compressionBlockRows = 2;
    size_t compressionBlockCols = 3;
    size_t compressionLevels = 2;
    Vectorspace vs = Vectorspace();


    vec col0 = vec {1,2,1,1,1};
    vs.addColumn(col0);
    vec col1 = vec {1,1,1,1,1};
    vs.addColumn(col1);

    vs.buildPyramide(compressionBlockRows, compressionBlockCols, compressionLevels);

    // level 1:
    // 1,1
    // 2,1
    // 1,1
    // 1,1
    // 1,1
    // 1,1
    ASSERT_EQ(1, vs.getVectorSpacePyramide()[0][0][0]);
    ASSERT_EQ(2, vs.getVectorSpacePyramide()[0][0][1]);
    ASSERT_EQ(1, vs.getVectorSpacePyramide()[0][0][2]);
    ASSERT_EQ(1, vs.getVectorSpacePyramide()[0][0][3]);
    ASSERT_EQ(1, vs.getVectorSpacePyramide()[0][0][4]);

    ASSERT_EQ(1, vs.getVectorSpacePyramide()[0][1][0]);
    ASSERT_EQ(1, vs.getVectorSpacePyramide()[0][1][1]);
    ASSERT_EQ(1, vs.getVectorSpacePyramide()[0][1][2]);
    ASSERT_EQ(1, vs.getVectorSpacePyramide()[0][1][3]);
    ASSERT_EQ(1, vs.getVectorSpacePyramide()[0][1][4]);

    // level 2:
    // 2
    // 1
    // 1
    ASSERT_EQ(2, vs.getVectorSpacePyramide()[1][0][0]);
    ASSERT_EQ(1, vs.getVectorSpacePyramide()[1][0][1]);
    ASSERT_EQ(1, vs.getVectorSpacePyramide()[1][0][2]);

    // level 2:
    // 2
    // 1
    ASSERT_EQ(2, vs.getVectorSpacePyramide()[2][0][0]);
    ASSERT_EQ(1, vs.getVectorSpacePyramide()[2][0][1]);

}

TEST(vsTest, pyramideRebuildTest) {
    size_t compressionBlockRows = 2;
    size_t compressionBlockCols = 3;
    size_t compressionLevels = 2;
    Vectorspace vs = Vectorspace();


    vec col0 = vec {1,2,1,1,1};
    vs.addColumn(col0);
    vec col1 = vec {1,1,1,1,1};
    vs.addColumn(col1);

    vs.buildPyramide(compressionBlockRows, compressionBlockCols, compressionLevels);
    ASSERT_EQ(1, vs.getVectorSpacePyramide()[0][0][0]);

    ASSERT_EQ(2, vs.getVectorSpacePyramide()[1][0][0]);
    ASSERT_EQ(1, vs.getVectorSpacePyramide()[1][0][1]);
    ASSERT_EQ(1, vs.getVectorSpacePyramide()[1][0][2]);

	ASSERT_EQ(3, vs.getVectorSpacePyramide()[1][0].size());

    // compress vector space with other parameters
    vs.buildPyramide(compressionBlockRows+1, compressionBlockCols+1, compressionLevels+1);
    ASSERT_EQ(1, vs.getVectorSpacePyramide()[0][0][0]);
    ASSERT_EQ(2, vs.getVectorSpacePyramide()[1][0][0]);
    ASSERT_EQ(1, vs.getVectorSpacePyramide()[1][0][1]);

	ASSERT_EQ(2, vs.getVectorSpacePyramide()[1][0].size());

}

TEST(vsTest, innerProductTest) {

    size_t compressionBlockCols = 3;
    size_t compressionBlockRows = 3;
    const size_t compressionLevels = 2;

    vec query = vec {1,2,3,4};
    QueryVector qv = QueryVector(query, compressionBlockRows , compressionLevels);
    Vectorspace vs = Vectorspace();
    vec col0 = vec {1,2,1,1};
    vs.addColumn(col0);
    vec col1 = vec {1,1,1,1};
    vs.addColumn(col1);
    vs.buildPyramide(compressionBlockRows, compressionBlockCols, compressionLevels);

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

// Hölder's inequality
// http://en.wikipedia.org/wiki/H%C3%B6lder%27s_inequality
TEST(tkqTest,  HoeldersInequalityTest) {

    size_t compressionBlockCols = 13;
    size_t compressionBlockRows = 5;
    const size_t compressionLevels = 3;

    size_t vectorSpaceCols = 2197;
    size_t vectorSpaceRows = 1000;

    vec query = vec();

    srand(43);
    for(size_t j = 0; j < vectorSpaceRows; j++){
        query.push_back(static_cast<float>(rand()) / static_cast<float>(RAND_MAX));
    }

    QueryVector qv = QueryVector(query, compressionBlockRows , compressionLevels);
    Vectorspace vs = Vectorspace();
    for(size_t i = 0; i < vectorSpaceCols; i++){
        vec col = vec();
        for(size_t j = 0; j < vectorSpaceRows; j++){
            col.push_back(static_cast<float>(rand()) / static_cast<float>(RAND_MAX));
        }
        vs.addColumn(col);
    }
    vs.buildPyramide(compressionBlockRows, compressionBlockCols, compressionLevels);

    // HoeldersInequality
    for(size_t level = 0; level < compressionLevels; level++){
        size_t cols = vs.getVectorSpacePyramide()[level].size();
        for(size_t col = 0; col < cols; col++){
           ASSERT_LE(vs.innerProduct(qv, level, col), vs.innerProduct(qv, level+1, col/compressionBlockCols));
        }
    }
}

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}

