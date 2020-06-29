#include "gtest/gtest.h"
#include "CompareCommits/CompareCommits.hpp"

TEST(CompareCommitsTests, DemonstrateGTestMacros) {
    EXPECT_TRUE(true);
}




// Tests SimilarityMatrix constructed with size 0
TEST(SimilarityMatrixConstructorTests, HandlesZeroSize) {
    try {
        SimilarityMatrix m = SimilarityMatrix(0);
        FAIL() << "Expected: Constructor called with zero size";
    }
    catch(std::invalid_argument & e){
        EXPECT_EQ(e.what(), std::string("Constructor called with zero size"));
    }
    catch(...){
        FAIL() << "Expected: Constructor called with zero size";
    }
}

// Tests SimilarityMatrix constructed with negative size - not sure if correct handling
TEST(SimilarityMatrixConstructorTests, HandlesNegativeSize) {
    SimilarityMatrix m = SimilarityMatrix(-1);
    EXPECT_EQ(m.size(), 4294967295);
}

// Tests SimilarityMatrix constructor with positive size
TEST(SimilarityMatrixConstructorTests, HandlesPositiveSize) {
    {
    SimilarityMatrix m = SimilarityMatrix(1);
    EXPECT_EQ(m.size(), 1);
    }
    {
    SimilarityMatrix m = SimilarityMatrix(2);
    EXPECT_EQ(m.size(), 2);
    }
    {
    SimilarityMatrix m = SimilarityMatrix(4294967295);
    EXPECT_EQ(m.size(), 4294967295);
    }
}