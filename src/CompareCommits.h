// *****************************************************************************
// * Name        : CompareCommits.h
// * Author      : Ruari Armstrong
// * Version     : 1.0
// * Copyright   : see LICENSE.md
// * Description : Compare commits, add to SimilarityMatrix and create a NEXUS
// *			   file for SplitsTree.
// *****************************************************************************

#ifndef COMPARECOMMITS_H_
#define COMPARECOMMITS_H_

/* Program Version */
#define COMPARECOMMITS_VERSION 1.0

#include <iostream>
#include <fstream>
#include <string>
#include <vector>

#include "SimilarityMatrix.h"
#include "git2.h"

#if (__cplusplus >= 201703L)
	#include <filesystem>
	namespace fs = std::filesystem;
#else
	#include <experimental/filesystem>
	namespace fs = std::experimental::filesystem;
#endif

/*******************************************************************************
 * a class to compare commits and create a NEXUS file representing the distance
 * matrix.
 ******************************************************************************/
class CompareCommits {
public:
		/*******************************************************************************
		* a pure virtual member, calculates the difference between two files.
		*
		* @param commit_path full path to the directory containing the files
		* @param commit1_path full path to first file
		* @param commit2_path full path to second file
		* @param commit1_filename the file name of first file
		* @param commit2_filename the file name of second file
		* @param repo_name repositories name on GitHub
		* @return difference metric between files or an error code
		*******************************************************************************/
		virtual double CompareTwoCommits(
				std::string commit_path,
				std::string commit1_path,
				std::string commit2_path,
				std::string commit1_filename,
				std::string commit2_filename,
				std::string repo_name="") = 0;

		/*******************************************************************************
		 * Compares all zips in given folder, gets two commits and passes to
		 * CompareTwoCommits for calculation, adds result to SimilarityMatrix.
		 *
		 * @param filename full path to the directory containing the zipped commits
		 * @param commit_path full path to commit directory
		 * @return 0 success or an error code
		 ******************************************************************************/
		int CommitCompareAllZip(
				std::string filename,
				std::string commit_path);

		/*******************************************************************************
		 * Compares all commits, gets two commits and passes to CompareTwoCommits for
		 * calculation, adds result to SimilarityMatrix.
		 *
		 * @param filename full path to the directory containing the zipped commits
		 * @param repo_name name given to repository on GitHub
		 * @param repo_path full path to repository directory
		 * @param commit_path full path to commit directory
		 * @return 0 success or an error code
		 ******************************************************************************/
		int CommitCompareAllGit(
				std::string filename,
				std::string repo_name,
				std::string repo_path,
				std::string commit_path);

		/*******************************************************************************
		 * Compares all commits in vector, gets two commits and passes to
		 * CompareTwoCommits for calculation, adds result to SimilarityMatrix.
		 *
		 * @param filename full path to the directory containing the zipped commits
		 * @param repo_name name given to repository on GitHub
		 * @param commit_path full path to commit directory
		 * @param commits vector of strings of commits to compare
		 * @return 0 success or an error code
		 ******************************************************************************/
		int CommitCompareVector(
				std::string filename,
				std::string repo_name,
				std::string commit_path,
				std::vector<std::string>& commits);

		/*******************************************************************************
		 * Gets all commits between start and end commits and adds to vector, passes to
		 * CommitCompareVector.
		 *
		 * @param filename full path to the directory containing the zipped commits
		 * @param repo_name name given to repository on GitHub
		 * @param repo_path full path to repository directory
		 * @param commit_path full path to commit directory
		 * @param start_commit_hash the long hash value for the start commit
		 * @param end_commit_hash the long hash value for the end commit
		 * @return 0 success or an error code
		 ******************************************************************************/
		int CommitCompareStartEnd(
				std::string filename,
				std::string repo_name,
				std::string repo_path,
				std::string commit_path,
				std::string start_commit_hash,
				std::string end_commit_hash);

		/***********************************************************************
		 * a virtual destructor.
		 **********************************************************************/
		virtual ~CompareCommits(){};

		/** a public variable to store the version number of the class. */
		static constexpr double VersionNo = COMPARECOMMITS_VERSION;

protected:
private:
};

#endif /* COMPARECOMMITS_H_ */
