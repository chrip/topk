#include "gtest/gtest.h"
#include "vectorspace.h"



TEST(vsTest, ctor) {
    size_t compressionBlockRows = 4;
    size_t compressionBlockCols = 3;
    size_t compressionLevels = 2;
    Vectorspace* vs = new Vectorspace(compressionBlockRows, compressionBlockCols, compressionLevels);
    delete vs;
}

TEST(vsTest, addCol) {
    size_t compressionBlockRows = 4;
    size_t compressionBlockCols = 3;
    size_t compressionLevels = 2;
    Vectorspace* vs = new Vectorspace(compressionBlockRows, compressionBlockCols, compressionLevels);

    size_t vectorSpaceCols = 8;
    size_t vectorSpaceRows = 11;
    srand(42);
    for(size_t i = 0; i < vectorSpaceCols; i++){
      vec* col = new vec();
      for(size_t j = 0; j < vectorSpaceRows; j++){
        col->push_back(1.0);
      }
      vs->addColumn(col);

      // 8 cols
      EXPECT_EQ(vectorSpaceCols, vs->getVectorSpacePyramide()->size());

      // 11 rows
      EXPECT_EQ(vectorSpaceRows+1, vs->getVectorSpacePyramide()->at(0)->size());
    }

    delete vs;
}
