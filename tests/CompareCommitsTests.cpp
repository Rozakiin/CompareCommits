#include "gtest/gtest.h"
#include "CompareCommits/CompareCommits.hpp"

TEST(CompareCommitsTests, DemonstrateGTestMacros) 
{
    EXPECT_TRUE(true);
}


// Tests SimilarityMatrix constructed with size 0
TEST(SimilarityMatrixConstructorTests, HandlesZeroSize) 
{
    try 
    {
        SimilarityMatrix m = SimilarityMatrix(0);
        FAIL() << "Expected: Constructor called with zero size";
    }
    catch(std::invalid_argument & e)
    {
        ASSERT_EQ(e.what(), std::string("Constructor called with zero size"));
    }
    catch(...)
    {
        FAIL() << "Expected: Constructor called with zero size";
    }
}

// Tests SimilarityMatrix constructed with negative size - not sure if correct handling
TEST(SimilarityMatrixConstructorTests, HandlesNegativeSize) 
{
    SimilarityMatrix m = SimilarityMatrix(-1);
    ASSERT_EQ(m.size(), 4294967295);
}

// Tests SimilarityMatrix constructor with positive size
TEST(SimilarityMatrixConstructorTests, HandlesPositiveSize1) 
{
    SimilarityMatrix m = SimilarityMatrix(1);
    ASSERT_EQ(m.size(), 1);
}
TEST(SimilarityMatrixConstructorTests, HandlesPositiveSize2) 
{
    SimilarityMatrix m = SimilarityMatrix(2);
    ASSERT_EQ(m.size(), 2);
}
TEST(SimilarityMatrixConstructorTests, HandlesPositiveSizeMAX)
{
    SimilarityMatrix m = SimilarityMatrix(4294967295);
    ASSERT_EQ(m.size(), 4294967295);
}

TEST(SimilarityMatrixConstructorTests, DataInitialisedToZero)
{
    SimilarityMatrix m = SimilarityMatrix(1);
    ASSERT_EQ(m.getData()[0],0);
}

TEST(SimilarityMatrixTests, CanAdd)
{
    SimilarityMatrix m = SimilarityMatrix(1);
    m.add("hash1", "hash1", 1);
    ASSERT_EQ(m.getData()[0],1);
}

//Fixture class for SimilarityMatrix
class SimilarityMatrixTest : public ::testing::Test {
    protected:
        void SetUp() override 
        {
            for (int i=0; i<1; i++)
            {
                for (int j=0; j<1; j++)
                {
                    m1_.add("hash"+std::to_string(i), "hash"+std::to_string(j), 1);
                }
            }
            for (int i=0; i<2; i++)
            {
                for (int j=0; j<2; j++)
                {
                    m2_.add("hash"+std::to_string(i), "hash"+std::to_string(j), 1);
                }
            }
        }

        SimilarityMatrix m1_ = SimilarityMatrix(1);
        SimilarityMatrix m2_ = SimilarityMatrix(2);
        //SimilarityMatrix mMAX_ = SimilarityMatrix(4294967295);
};

TEST_F(SimilarityMatrixTest, AddOverwritesExistingData) 
{
    m1_.add("hash0", "hash0", 1);
    m1_.add("hash0", "hash0", 2);
    ASSERT_EQ(m1_.getData()[0],2);
}

//TODO improve test name
TEST_F(SimilarityMatrixTest, CannotAddMoreThanSize) 
{
    try
    {
        m1_.add("hash1", "hash1", 1);
        FAIL() << "No Exception - Expected: out of range";
    }
    catch(const std::out_of_range& e) 
    {
        ASSERT_EQ(e.what(), std::string("invalid vector subscript"));
    }
    catch(...)
    {
        FAIL() << "Expected: out of range";
    }
}

TEST_F(SimilarityMatrixTest, CanGetSize1) 
{
    ASSERT_EQ(m1_.size(), 1);
}

TEST_F(SimilarityMatrixTest, CanGetSize2) 
{
    ASSERT_EQ(m2_.size(), 2);
}

TEST_F(SimilarityMatrixTest, CanGetIndex0) 
{
    ASSERT_EQ(m1_.getIndex("hash0"), 0);
}

TEST_F(SimilarityMatrixTest, CanGetIndex1) 
{
    ASSERT_EQ(m2_.getIndex("hash1"), 1);
}

TEST_F(SimilarityMatrixTest, CanGetHash0) 
{
    ASSERT_EQ(m1_.getHash(0), "hash0");
}

TEST_F(SimilarityMatrixTest, CanGetHash1) 
{
    ASSERT_EQ(m2_.getHash(1), "hash1");
}

TEST_F(SimilarityMatrixTest, CanGetData0) 
{
    ASSERT_EQ(m1_.getData().at(0), 1);
}

TEST_F(SimilarityMatrixTest, CanGetData1) 
{
    ASSERT_EQ(m2_.getData().at(1), 1);
}

TEST_F(SimilarityMatrixTest, CanGetMap0) 
{
    ASSERT_EQ(m1_.getMap().begin()->first, "hash0");
    ASSERT_EQ(m1_.getMap().begin()->second, 0);
}

TEST_F(SimilarityMatrixTest, CanGetMap1) 
{
    ASSERT_EQ(m2_.getMap().rbegin()->first, "hash1");
    ASSERT_EQ(m2_.getMap().rbegin()->second, 1);
}

TEST_F(SimilarityMatrixTest, CanGetDataReferenceByOperator) 
{
    auto data = &m2_(1,1);
    ASSERT_EQ(*data, 1);
}

TEST_F(SimilarityMatrixTest, CanGetDataValueByOperator) 
{
    auto data = m2_(1,1);
    ASSERT_EQ(data, 1);
}

TEST_F(SimilarityMatrixTest, CanEqualsOperator) 
{
    SimilarityMatrix m = m1_;
    //ASSERT_EQ(m,m1_);
}

//TODO not sure how to test nexusout
TEST_F(SimilarityMatrixTest, CanNexusOut) 
{

    //ASSERT_EQ(m,m1_);
}
