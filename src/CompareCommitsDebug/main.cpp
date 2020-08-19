// *****************************************************************************
// * Name        : main.cpp
// * Author      : Ruari Armstrong
// * Version     : 1.0
// * Copyright   : see LICENSE.md
// * Description : main file for the debugging of CompareCommits.
// *****************************************************************************

#include "CompareCommitsLibrary/CompareCommits.hpp"
#include <sstream>
/*******************************************************************************
 * Derrived Class from CompareCommits for debugging use.
 ******************************************************************************/
class DebugCompareCommits: public CompareCommits{
public:
	DebugCompareCommits(){};
	/***************************************************************************
	 * Overrides CompareTwoCommits member function to return 1
	 *
	 * @param commit_path full path to the directory containing the files
	 * @param commit1_path full path to first file
	 * @param commit2_path full path to second file
	 * @param commit1_filename the file name of first file
	 * @param commit2_filename the file name of second file
	 * @param repo_name repositories name on GitHub
	 * @return 1
	 **************************************************************************/
	double CompareTwoCommits(
			std::string commit_path,
			std::string commit1_path,
			std::string commit2_path,
			std::string commit1_filename,
			std::string commit2_filename,
			std::string repo_name="") override
	{
        return 1;
    }
    /***************************************************************************
	 * a destructor.
	 **************************************************************************/
	~DebugCompareCommits(){};
};

/*******************************************************************************
 * main.
 *
 * @param argc number of arguments passed
 * @param argv[] array of arguments
 * @return 0 success or an error code
 ******************************************************************************/
int main(int argc, char* argv[])
{
    DebugCompareCommits dcc;
	SimilarityMatrix m;

	std::ifstream is("C:/Users/Ru/Documents/Work/Projects/testmatrix.txt");

	is >> m;

	std::cout << m << std::endl;

    //dcc.CommitCompareAllGit("test1", "TestGitRepository", "C:/Users/Ru/Documents/Work/Projects/Git2Zip/TestGitRepository", "C:/Users/Ru/Documents/Work/Projects/Git2Zip/TestGitRepositoryCommits1");
    //dcc.CommitCompareAllGit("test1", "TestGitRepository", "C:/Users/Ru/Documents/Work/Projects/Git2Zip/TestGitRepository", "C:/Users/Ru/Documents/Work/Projects/Git2Zip/TestGitRepositoryCommits");
}